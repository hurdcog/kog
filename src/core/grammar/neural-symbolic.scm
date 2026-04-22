;;; neural_symbolic.scm --- Neural-Symbolic Integration for Cognitive Grammar
;;;
;;; Copyright (C) 2025 Deep Tree Echo Project
;;;
;;; This module bridges neural (activation-based) and symbolic (logic-based)
;;; representations, enabling:
;;; - Concept formation from activation patterns
;;; - Rule extraction from co-activation patterns
;;; - Symbol grounding in sensorimotor experience
;;; - Hybrid reasoning combining both paradigms

(define-module (core grammar neural-symbolic)
  #:use-module (srfi srfi-1)      ; List library
  #:use-module (srfi srfi-9)      ; Records
  #:use-module (srfi srfi-69)     ; Hash tables
  #:use-module (ice-9 match)      ; Pattern matching
  #:use-module (core memory atomspace)
  #:use-module (core propagation spreading)
  #:use-module (core grammar inference)
  
  #:export (
    ;; Data structures
    make-activation-history
    activation-history?
    activation-history-atom
    activation-history-timestamps
    activation-history-values
    activation-history-coactivations
    
    make-concept-cluster
    concept-cluster?
    concept-cluster-atoms
    concept-cluster-coherence
    concept-cluster-frequency
    concept-cluster-concept-node
    
    make-activation-rule
    activation-rule?
    activation-rule-antecedent
    activation-rule-consequent
    activation-rule-support
    activation-rule-confidence
    activation-rule-strength
    
    ;; Concept formation
    track-activation
    detect-activation-clusters
    form-concepts-from-activation
    
    ;; Rule extraction
    mine-coactivation-patterns
    extract-rules-from-coactivation
    
    ;; Utilities
    compute-coherence
    compute-jaccard-similarity
    generate-concept-name
    generate-rule-name
    ))

;;;
;;; Data Structures
;;;

(define-record-type activation-history
  (make-activation-history atom timestamps values coactivations)
  activation-history?
  (atom activation-history-atom)
  (timestamps activation-history-timestamps activation-history-set-timestamps!)
  (values activation-history-values activation-history-set-values!)
  (coactivations activation-history-coactivations))

(define-record-type concept-cluster
  (make-concept-cluster atoms coherence frequency concept-node)
  concept-cluster?
  (atoms concept-cluster-atoms)
  (coherence concept-cluster-coherence)
  (frequency concept-cluster-frequency)
  (concept-node concept-cluster-concept-node concept-cluster-set-concept-node!))

(define-record-type activation-rule
  (make-activation-rule antecedent consequent support confidence strength)
  activation-rule?
  (antecedent activation-rule-antecedent)
  (consequent activation-rule-consequent)
  (support activation-rule-support)
  (confidence activation-rule-confidence)
  (strength activation-rule-strength))

;;;
;;; Activation Tracking
;;;

(define (track-activation history-table atom timestamp value coactivated-atoms)
  "Track an activation event for an atom.
   
   Parameters:
   - history-table: Hash table mapping atoms to activation-history records
   - atom: The atom being activated
   - timestamp: Time of activation
   - value: Activation value
   - coactivated-atoms: List of atoms activated at same time
   
   Side effects: Updates history-table"
  
  (let ((history (hash-table-ref/default history-table atom #f)))
    (if history
        ;; Update existing history
        (begin
          (activation-history-set-timestamps! 
            history 
            (cons timestamp (activation-history-timestamps history)))
          (activation-history-set-values!
            history
            (cons value (activation-history-values history)))
          ;; Update co-activations
          (for-each
            (lambda (coatom)
              (when (not (equal? coatom atom))
                (let ((count (hash-table-ref/default 
                              (activation-history-coactivations history)
                              coatom
                              0)))
                  (hash-table-set! (activation-history-coactivations history)
                                  coatom
                                  (+ count 1)))))
            coactivated-atoms))
        ;; Create new history
        (let ((new-history (make-activation-history
                            atom
                            (list timestamp)
                            (list value)
                            (make-hash-table))))
          ;; Initialize co-activations
          (for-each
            (lambda (coatom)
              (when (not (equal? coatom atom))
                (hash-table-set! (activation-history-coactivations new-history)
                                coatom
                                1)))
            coactivated-atoms)
          (hash-table-set! history-table atom new-history)))))

;;;
;;; Cluster Detection
;;;

(define (compute-coherence atom1-history atom2-history)
  "Compute coherence between two atoms (how often they co-activate).
   
   Returns: Coherence value (0-1)"
  
  (let ((total-activations (length (activation-history-timestamps atom1-history)))
        (coactivation-count (hash-table-ref/default
                             (activation-history-coactivations atom1-history)
                             (activation-history-atom atom2-history)
                             0)))
    (if (zero? total-activations)
        0.0
        (/ coactivation-count total-activations))))

(define (compute-jaccard-similarity set1 set2)
  "Compute Jaccard similarity between two sets.
   
   J(A,B) = |A ∩ B| / |A ∪ B|
   
   Returns: Similarity value (0-1)"
  
  (let ((intersection (lset-intersection equal? set1 set2))
        (union (lset-union equal? set1 set2)))
    (if (null? union)
        0.0
        (/ (length intersection) (length union)))))

(define (detect-activation-clusters history-table threshold)
  "Detect clusters of frequently co-activated atoms.
   
   Parameters:
   - history-table: Hash table of activation histories
   - threshold: Minimum coherence for clustering (0-1)
   
   Returns: List of concept-cluster records
   
   Algorithm:
   1. Build similarity matrix
   2. Find connected components with similarity > threshold
   3. Create cluster for each component"
  
  (let ((atoms (hash-table-keys history-table)))
    ;; Build clusters using greedy agglomerative approach
    (let loop ((remaining atoms)
               (clusters '()))
      (if (null? remaining)
          clusters
          (let* ((seed-atom (car remaining))
                 (seed-history (hash-table-ref history-table seed-atom))
                 ;; Find all atoms with high coherence to seed
                 (cluster-atoms
                   (filter
                     (lambda (atom)
                       (let ((atom-history (hash-table-ref history-table atom)))
                         (>= (compute-coherence seed-history atom-history)
                             threshold)))
                     remaining))
                 ;; Compute cluster statistics
                 (cluster-size (length cluster-atoms))
                 (cluster-coherence
                   (if (< cluster-size 2)
                       1.0
                       (let ((pairs (combinations cluster-atoms 2)))
                         (/ (apply +
                                  (map (lambda (pair)
                                        (let ((h1 (hash-table-ref history-table (car pair)))
                                              (h2 (hash-table-ref history-table (cadr pair))))
                                          (compute-coherence h1 h2)))
                                      pairs))
                            (length pairs)))))
                 (cluster-frequency
                   (/ (apply +
                            (map (lambda (atom)
                                  (length (activation-history-timestamps
                                          (hash-table-ref history-table atom))))
                                cluster-atoms))
                      cluster-size))
                 ;; Create cluster
                 (cluster (make-concept-cluster
                           cluster-atoms
                           cluster-coherence
                           cluster-frequency
                           #f)))  ; concept-node will be set later
            
            ;; Continue with remaining atoms
            (loop (lset-difference equal? remaining cluster-atoms)
                  (if (>= cluster-size 2)  ; Only keep clusters with 2+ atoms
                      (cons cluster clusters)
                      clusters)))))))

(define (combinations lst k)
  "Generate all k-combinations of lst."
  (cond
    ((= k 0) '(()))
    ((null? lst) '())
    (else
      (append
        (map (lambda (rest) (cons (car lst) rest))
             (combinations (cdr lst) (- k 1)))
        (combinations (cdr lst) k)))))

;;;
;;; Concept Formation
;;;

(define (generate-concept-name cluster)
  "Generate a name for a concept cluster.
   
   Uses the names of constituent atoms to create a descriptive name."
  
  (let ((atom-names (map (lambda (atom)
                          (if (atom-name atom)
                              (atom-name atom)
                              (symbol->string (atom-type atom))))
                        (concept-cluster-atoms cluster))))
    (string-append "Concept-"
                   (string-join (take atom-names (min 3 (length atom-names)))
                               "-"))))

(define (form-concepts-from-activation atomspace history-table threshold)
  "Discover new concepts from activation patterns.
   
   Parameters:
   - atomspace: The atomspace to add concepts to
   - history-table: Hash table of activation histories
   - threshold: Minimum coherence for concept formation (0-1)
   
   Returns: List of newly formed concept nodes
   
   Algorithm:
   1. Detect activation clusters
   2. For each cluster:
      a. Create concept node
      b. Link to constituent atoms
      c. Set truth value based on coherence
      d. Set attention value based on frequency"
  
  (let ((clusters (detect-activation-clusters history-table threshold)))
    (map (lambda (cluster)
           (let* ((concept-name (generate-concept-name cluster))
                  (concept-node (create-concept-node atomspace concept-name)))
             
             ;; Link concept to constituent atoms
             (for-each (lambda (atom)
                        (create-inheritance-link atomspace atom concept-node))
                      (concept-cluster-atoms cluster))
             
             ;; Set truth value based on coherence
             (atom-set-tv! concept-node
                          (make-truth-value #:strength (concept-cluster-coherence cluster)
                                          #:confidence 0.9))
             
             ;; Set attention value based on frequency
             (let ((normalized-freq (min 1.0 (/ (concept-cluster-frequency cluster) 100.0))))
               (atom-set-av! concept-node
                            (make-attention-value #:sti normalized-freq #:lti 0.5 #:vlti 0.5)))
             
             ;; Store concept node in cluster
             (concept-cluster-set-concept-node! cluster concept-node)
             
             concept-node))
         clusters)))

;;;
;;; Co-Activation Pattern Mining
;;;

(define (mine-coactivation-patterns history-table min-support)
  "Mine frequent co-activation patterns.
   
   Parameters:
   - history-table: Hash table of activation histories
   - min-support: Minimum pattern frequency
   
   Returns: List of (antecedent consequent support confidence) tuples
   
   Algorithm:
   1. Find frequent itemsets (atoms that often co-activate)
   2. Generate association rules from itemsets
   3. Compute support and confidence for each rule"
  
  (let ((atoms (hash-table-keys history-table))
        (patterns '()))
    
    ;; For each pair of atoms, check if they form a pattern
    (for-each
      (lambda (atom1)
        (let ((history1 (hash-table-ref history-table atom1)))
          (for-each
            (lambda (atom2)
              (when (not (equal? atom1 atom2))
                (let* ((history2 (hash-table-ref history-table atom2))
                       (coactivation-count (hash-table-ref/default
                                           (activation-history-coactivations history1)
                                           atom2
                                           0))
                       (atom1-count (length (activation-history-timestamps history1)))
                       (atom2-count (length (activation-history-timestamps history2)))
                       (support (/ coactivation-count (max atom1-count atom2-count)))
                       (confidence (if (zero? atom1-count)
                                     0.0
                                     (/ coactivation-count atom1-count))))
                  
                  ;; Add pattern if support is high enough
                  (when (>= support min-support)
                    (set! patterns
                          (cons (list atom1 atom2 support confidence)
                                patterns))))))
            atoms)))
      atoms)
    
    patterns))

;;;
;;; Rule Extraction
;;;

(define (generate-rule-name pattern)
  "Generate a name for an extracted rule."
  
  (let ((antecedent (car pattern))
        (consequent (cadr pattern)))
    (string-append "Rule-"
                   (if (atom-name antecedent)
                       (atom-name antecedent)
                       (symbol->string (atom-type antecedent)))
                   "-implies-"
                   (if (atom-name consequent)
                       (atom-name consequent)
                       (symbol->string (atom-type consequent))))))

(define (extract-rules-from-coactivation atomspace history-table min-support min-confidence)
  "Extract inference rules from co-activation patterns.
   
   Parameters:
   - atomspace: The atomspace
   - history-table: Hash table of activation histories
   - min-support: Minimum pattern frequency (0-1)
   - min-confidence: Minimum rule confidence (0-1)
   
   Returns: List of extracted inference rules
   
   Algorithm:
   1. Mine co-activation patterns
   2. For each pattern with high confidence:
      a. Create inference rule
      b. Add to atomspace as ImplicationLink"
  
  (let ((patterns (mine-coactivation-patterns history-table min-support)))
    (filter-map
      (lambda (pattern)
        (match pattern
          ((antecedent consequent support confidence)
           ;; Only create rule if confidence is high enough
           (if (>= confidence min-confidence)
               (let* ((rule-name (generate-rule-name pattern))
                      ;; Create rule: IF antecedent THEN consequent
                      (rule (make-inference-rule
                             (string->symbol rule-name)
                             (list antecedent)  ; Premises
                             consequent         ; Conclusion
                             confidence)))      ; Strength
                 
                 ;; Also create ImplicationLink in atomspace
                 (create-implication-link atomspace antecedent consequent)
                 
                 rule)
               #f))
          (_ #f)))
      patterns)))

;;; neural_symbolic.scm ends here
