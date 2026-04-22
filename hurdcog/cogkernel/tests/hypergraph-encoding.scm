;;; Hypergraph Dependency Encoding for Issue-Based Testing
;;; Maps issue dependencies as hypergraph links in AtomSpace
;;; Enables propagation of solution impact across related issues

(define-module (cogkernel tests hypergraph-encoding)
  #:use-module (ice-9 format)
  #:use-module (ice-9 match)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (cogkernel atomspace)
  #:use-module (cogkernel attention)
  #:export (encode-issue-hypergraph
            create-dependency-links
            propagate-solution-impact
            calculate-issue-centrality
            get-hypergraph-statistics
            visualize-dependency-graph))

;;; Hypergraph link types for issue relationships
(define issue-link-types
  '(depends-on
    impacts
    blocks
    enables
    similar-to
    conflicts-with
    enhances))

;;; Encode entire issue catalog as hypergraph in AtomSpace
(define (encode-issue-hypergraph test-catalog atomspace)
  "Encode all issues and dependencies as hypergraph in AtomSpace"
  (let ((issue-nodes (create-issue-nodes test-catalog atomspace))
        (dependency-links (create-dependency-links test-catalog atomspace)))
    (format #t "Created ~a issue nodes and ~a dependency links\n"
            (length issue-nodes) (length dependency-links))
    (cons issue-nodes dependency-links)))

;;; Create AtomSpace nodes for each issue
(define (create-issue-nodes test-catalog atomspace)
  "Create ConceptNode for each issue in the catalog"
  (apply append
    (map (lambda (category-pair)
           (let ((category-name (car category-pair))
                 (issues (cdr category-pair)))
             (map (lambda (issue)
                    (let* ((title (assoc-ref issue 'title))
                           (node-name (format #f "Issue_~a" 
                                             (normalize-node-name title)))
                           (concept-node (create-concept-node atomspace node-name)))
                      ;; Add properties as evaluation links
                      (add-issue-properties atomspace concept-node issue category-name)
                      concept-node))
                  issues)))
         test-catalog)))

;;; Create dependency links between issues
(define (create-dependency-links test-catalog atomspace)
  "Create hypergraph links representing dependencies between issues"
  (apply append
    (map (lambda (category-pair)
           (let ((issues (cdr category-pair)))
             (apply append
               (map (lambda (issue)
                      (create-issue-dependency-links atomspace issue test-catalog))
                    issues))))
         test-catalog)))

;;; Create dependency links for a single issue
(define (create-issue-dependency-links atomspace issue all-issues)
  "Create dependency links for a single issue"
  (let* ((issue-title (assoc-ref issue 'title))
         (dependencies (assoc-ref issue 'dependencies))
         (issue-node (find-concept-node atomspace 
                                       (format #f "Issue_~a" 
                                              (normalize-node-name issue-title)))))
    (if issue-node
        (apply append
          (map (lambda (dep)
                 (create-dependency-link atomspace issue-node dep all-issues))
               dependencies))
        '())))

;;; Create a single dependency link
(define (create-dependency-link atomspace source-node dependency all-issues)
  "Create dependency link between source issue and dependency"
  (let ((target-issues (find-issues-with-dependency dependency all-issues)))
    (map (lambda (target-issue)
           (let* ((target-title (assoc-ref target-issue 'title))
                  (target-node (find-concept-node atomspace
                                                 (format #f "Issue_~a"
                                                        (normalize-node-name target-title))))
                  (link-type (infer-dependency-type dependency))
                  (dependency-link (create-evaluation-link atomspace
                                                          link-type
                                                          (list source-node target-node))))
             dependency-link))
         target-issues)))

;;; Find issues that involve a specific dependency
(define (find-issues-with-dependency dependency all-issues)
  "Find all issues that involve a specific dependency"
  (filter (lambda (issue)
            (let ((issue-deps (assoc-ref issue 'dependencies))
                  (issue-title (assoc-ref issue 'title))
                  (issue-desc (assoc-ref issue 'description)))
              (or (member dependency issue-deps)
                  (string-contains (string-downcase issue-title) 
                                  (string-downcase dependency))
                  (string-contains (string-downcase issue-desc)
                                  (string-downcase dependency)))))
          (extract-all-issues-flat all-issues)))

;;; Extract all issues as flat list
(define (extract-all-issues-flat test-catalog)
  "Extract all issues from catalog as flat list"
  (apply append
    (map (lambda (category-pair)
           (cdr category-pair))
         test-catalog)))

;;; Infer dependency relationship type
(define (infer-dependency-type dependency)
  "Infer the type of dependency relationship"
  (cond
    ((member dependency '("kernel" "GNU Mach")) 'depends-on)
    ((member dependency '("server" "IPC")) 'impacts)
    ((member dependency '("glibc" "Linux driver")) 'enables)
    ((member dependency '("SMP" "x86_64")) 'blocks)
    (else 'depends-on)))

;;; Add issue properties as AtomSpace evaluations
(define (add-issue-properties atomspace issue-node issue category)
  "Add issue properties as evaluation links in AtomSpace"
  (let ((properties (list
                     (cons 'category category)
                     (cons 'status (assoc-ref issue 'status))
                     (cons 'failure-mode (assoc-ref issue 'failure-mode))
                     (cons 'impact-area (assoc-ref issue 'impact-area)))))
    (map (lambda (prop-pair)
           (create-evaluation-link atomspace
                                  (car prop-pair)
                                  (list issue-node 
                                        (create-concept-node atomspace 
                                                           (cdr prop-pair)))))
         properties)))

;;; Propagate solution impact through hypergraph
(define (propagate-solution-impact atomspace issue-node impact-strength)
  "Propagate solution impact through dependency hypergraph"
  (let ((connected-issues (get-connected-issues atomspace issue-node))
        (propagated-impacts '()))
    (for-each (lambda (connected-issue)
                (let* ((connection-strength (calculate-connection-strength 
                                           atomspace issue-node connected-issue))
                       (propagated-impact (* impact-strength connection-strength)))
                  (when (> propagated-impact 0.1) ; Threshold for propagation
                    (set! propagated-impacts 
                          (cons (cons connected-issue propagated-impact)
                                propagated-impacts))
                    (format #t "Propagating impact ~a -> ~a: ~a\n"
                            (get-node-name issue-node)
                            (get-node-name connected-issue)
                            propagated-impact))))
              connected-issues)
    propagated-impacts))

;;; Get issues connected to a given issue
(define (get-connected-issues atomspace issue-node)
  "Get all issues connected to the given issue via hypergraph links"
  (let ((incoming-links (get-incoming-links atomspace issue-node))
        (outgoing-links (get-outgoing-links atomspace issue-node)))
    (apply append
      (map (lambda (link)
             (filter (lambda (node) 
                       (not (equal? node issue-node)))
                     (get-link-targets link)))
           (append incoming-links outgoing-links)))))

;;; Calculate connection strength between two issues
(define (calculate-connection-strength atomspace issue1 issue2)
  "Calculate strength of connection between two issues"
  (let ((direct-links (count-direct-links atomspace issue1 issue2))
        (shared-dependencies (count-shared-dependencies atomspace issue1 issue2))
        (category-similarity (calculate-category-similarity atomspace issue1 issue2)))
    (+ (* 0.5 direct-links)
       (* 0.3 shared-dependencies) 
       (* 0.2 category-similarity))))

;;; Calculate issue centrality in hypergraph
(define (calculate-issue-centrality atomspace issue-node)
  "Calculate centrality of issue in the dependency hypergraph"
  (let* ((connected-issues (get-connected-issues atomspace issue-node))
         (direct-connections (length connected-issues))
         (transitive-connections (calculate-transitive-connections 
                                 atomspace issue-node 2)) ; 2-hop
         (weighted-centrality (+ direct-connections 
                               (* 0.5 transitive-connections))))
    (/ weighted-centrality 
       (max 1 (get-total-issues atomspace)))))

;;; Calculate transitive connections up to given depth
(define (calculate-transitive-connections atomspace issue-node max-depth)
  "Calculate transitive connections up to max-depth"
  (define (traverse-connections node current-depth visited)
    (if (>= current-depth max-depth)
        0
        (let ((connections (filter (lambda (n) (not (member n visited)))
                                  (get-connected-issues atomspace node))))
          (+ (length connections)
             (apply + (map (lambda (conn)
                            (traverse-connections conn 
                                                (+ current-depth 1)
                                                (cons node visited)))
                          connections))))))
  (traverse-connections issue-node 0 '()))

;;; Get hypergraph statistics
(define (get-hypergraph-statistics atomspace)
  "Get comprehensive statistics about the issue hypergraph"
  (let* ((all-issue-nodes (get-all-issue-nodes atomspace))
         (total-issues (length all-issue-nodes))
         (total-links (get-total-dependency-links atomspace))
         (avg-degree (if (> total-issues 0) 
                        (/ total-links total-issues) 
                        0))
         (centrality-scores (map (lambda (node)
                                  (calculate-issue-centrality atomspace node))
                                all-issue-nodes))
         (max-centrality (if (null? centrality-scores) 
                           0 
                           (apply max centrality-scores))))
    `((total-issues . ,total-issues)
      (total-links . ,total-links)
      (average-degree . ,avg-degree)
      (max-centrality . ,max-centrality)
      (connectivity-ratio . ,(/ total-links (max 1 (* total-issues total-issues)))))))

;;; Visualize dependency graph (textual representation)
(define (visualize-dependency-graph atomspace)
  "Create textual visualization of dependency graph"
  (let ((all-issues (get-all-issue-nodes atomspace)))
    (format #t "\n=== Issue Dependency Hypergraph ===\n")
    (for-each (lambda (issue-node)
                (let* ((issue-name (get-node-name issue-node))
                       (connections (get-connected-issues atomspace issue-node))
                       (centrality (calculate-issue-centrality atomspace issue-node)))
                  (format #t "~a (centrality: ~,3f)\n" issue-name centrality)
                  (for-each (lambda (conn)
                              (format #t "  -> ~a\n" (get-node-name conn)))
                            connections)))
              all-issues)
    (format #t "\nHypergraph Statistics:\n")
    (let ((stats (get-hypergraph-statistics atomspace)))
      (for-each (lambda (stat-pair)
                  (format #t "  ~a: ~a\n" (car stat-pair) (cdr stat-pair)))
                stats))))

;;; Utility functions (to be implemented based on actual AtomSpace API)
(define (normalize-node-name name)
  "Normalize issue title for use as node name"
  (string-map (lambda (c) 
                (cond ((char-alphabetic? c) c)
                      ((char-numeric? c) c)
                      (else #\_)))
              (string-downcase name)))

(define (create-concept-node atomspace name)
  "Create ConceptNode in AtomSpace (placeholder)"
  (list 'ConceptNode name))

(define (create-evaluation-link atomspace predicate args)
  "Create EvaluationLink in AtomSpace (placeholder)"
  (list 'EvaluationLink predicate args))

(define (find-concept-node atomspace name)
  "Find ConceptNode by name (placeholder)"
  (list 'ConceptNode name))

(define (get-node-name node)
  "Get name of node (placeholder)"
  (cadr node))

(define (get-incoming-links atomspace node)
  "Get incoming links for node (placeholder)"
  '())

(define (get-outgoing-links atomspace node)
  "Get outgoing links for node (placeholder)"
  '())

(define (get-link-targets link)
  "Get target nodes of link (placeholder)"
  '())

(define (count-direct-links atomspace node1 node2)
  "Count direct links between nodes (placeholder)"
  1)

(define (count-shared-dependencies atomspace node1 node2)
  "Count shared dependencies (placeholder)"
  0)

(define (calculate-category-similarity atomspace node1 node2)
  "Calculate category similarity (placeholder)"
  0.5)

(define (get-all-issue-nodes atomspace)
  "Get all issue nodes (placeholder)"
  '())

(define (get-total-dependency-links atomspace)
  "Get total dependency links (placeholder)"
  0)

(define (get-total-issues atomspace)
  "Get total number of issues (placeholder)"
  1)