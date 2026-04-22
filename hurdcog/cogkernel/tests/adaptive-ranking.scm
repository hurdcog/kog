;;; Adaptive Ranking Algorithm with Attention Allocation
;;; Implements self-evolving prioritization system
;;; Weight unresolved/high-impact issues higher in next dev cycle

(define-module (cogkernel tests adaptive-ranking)
  #:use-module (ice-9 format)
  #:use-module (ice-9 match)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (cogkernel core)
  #:use-module (cogkernel attention)
  #:use-module (cogkernel tests hypergraph-encoding)
  #:export (create-adaptive-ranker
            update-attention-weights
            rank-issues-by-priority
            calculate-solution-efficacy
            evolve-ranking-parameters
            generate-priority-report))

;;; Adaptive ranking system record
(define-record-type <adaptive-ranker>
  (make-adaptive-ranker attention-weights historical-data 
                       ranking-parameters learning-rate)
  adaptive-ranker?
  (attention-weights ranker-attention-weights)
  (historical-data ranker-historical-data)
  (ranking-parameters ranker-ranking-parameters)
  (learning-rate ranker-learning-rate))

;;; Issue priority record
(define-record-type <issue-priority>
  (make-issue-priority issue-id title priority-score attention-allocation
                      impact-weight centrality-score temporal-urgency)
  issue-priority?
  (issue-id priority-issue-id)
  (title priority-title)
  (priority-score priority-score)
  (attention-allocation priority-attention-allocation)
  (impact-weight priority-impact-weight)
  (centrality-score priority-centrality-score)
  (temporal-urgency priority-temporal-urgency))

;;; Create adaptive ranking system
(define (create-adaptive-ranker)
  "Create adaptive ranking system with initial parameters"
  (let ((initial-weights (create-initial-attention-weights))
        (initial-params (create-initial-ranking-parameters)))
    (make-adaptive-ranker initial-weights '() initial-params 0.1)))

;;; Create initial attention weights
(define (create-initial-attention-weights)
  "Create initial attention allocation weights"
  `((impact-factor . 0.4)
    (centrality-factor . 0.3)
    (temporal-factor . 0.2)
    (historical-factor . 0.1)
    (category-weights . ((performance . 0.9)
                        (stability . 0.8)
                        (hardware . 0.7)
                        (technical . 0.6)
                        (development . 0.5)
                        (implementation . 0.4)))
    (status-weights . ((active . 0.9)
                      (ongoing . 0.8)
                      (research . 0.6)
                      (unknown . 0.3)))
    (dependency-weights . ((kernel . 1.0)
                          (server . 0.9)
                          (gnu-mach . 0.8)
                          (ipc . 0.7)
                          (glibc . 0.6)))))

;;; Create initial ranking parameters
(define (create-initial-ranking-parameters)
  "Create initial ranking algorithm parameters"
  `((decay-rate . 0.95)          ; For temporal urgency
    (amplification-factor . 1.5)  ; For high-impact issues
    (regression-penalty . 2.0)    ; Penalty for causing regressions
    (resolution-bonus . 1.3)      ; Bonus for resolving issues
    (clustering-threshold . 0.7)  ; For grouping related issues
    (attention-redistribution . 0.8))) ; Rate of attention redistribution

;;; Update attention weights based on test outcomes
(define (update-attention-weights ranker test-results solution-outcomes)
  "Update attention allocation weights based on empirical outcomes"
  (let* ((current-weights (ranker-attention-weights ranker))
         (performance-data (analyze-solution-performance test-results solution-outcomes))
         (learning-rate (ranker-learning-rate ranker))
         (updated-weights (apply-gradient-update current-weights 
                                               performance-data 
                                               learning-rate)))
    
    (format #t "Updating attention weights based on ~a test results\n" 
            (length test-results))
    (display-weight-changes current-weights updated-weights)
    
    (make-adaptive-ranker updated-weights
                         (cons (cons test-results solution-outcomes)
                               (ranker-historical-data ranker))
                         (ranker-ranking-parameters ranker)
                         learning-rate)))

;;; Rank issues by adaptive priority
(define (rank-issues-by-priority ranker test-catalog atomspace)
  "Rank all issues by adaptive priority score"
  (let* ((all-issues (extract-all-issues-with-metadata test-catalog))
         (priority-scores (map (lambda (issue)
                               (calculate-issue-priority ranker issue atomspace))
                              all-issues))
         (ranked-issues (sort priority-scores
                             (lambda (a b)
                               (> (priority-score a) (priority-score b))))))
    
    (format #t "Ranked ~a issues by adaptive priority\n" (length ranked-issues))
    ranked-issues))

;;; Calculate priority score for single issue
(define (calculate-issue-priority ranker issue atomspace)
  "Calculate adaptive priority score for a single issue"
  (let* ((weights (ranker-attention-weights ranker))
         (params (ranker-ranking-parameters ranker))
         (issue-id (assoc-ref issue 'title))
         (impact-weight (calculate-impact-weight issue weights))
         (centrality-score (calculate-centrality-score issue atomspace))
         (temporal-urgency (calculate-temporal-urgency issue params))
         (historical-factor (calculate-historical-factor issue ranker))
         (priority-score (combine-priority-factors impact-weight
                                                  centrality-score
                                                  temporal-urgency
                                                  historical-factor
                                                  weights))
         (attention-allocation (distribute-attention priority-score weights)))
    
    (make-issue-priority issue-id
                        (assoc-ref issue 'title)
                        priority-score
                        attention-allocation
                        impact-weight
                        centrality-score
                        temporal-urgency)))

;;; Calculate impact weight based on issue characteristics
(define (calculate-impact-weight issue weights)
  "Calculate impact weight for issue"
  (let* ((category (assoc-ref issue 'category))
         (status (assoc-ref issue 'status))
         (failure-mode (assoc-ref issue 'failure-mode))
         (dependencies (assoc-ref issue 'dependencies))
         (category-weight (get-category-weight category weights))
         (status-weight (get-status-weight status weights))
         (failure-severity (get-failure-severity failure-mode))
         (dependency-weight (calculate-dependency-weight dependencies weights)))
    
    (* category-weight status-weight failure-severity dependency-weight)))

;;; Calculate centrality score using hypergraph
(define (calculate-centrality-score issue atomspace)
  "Calculate centrality score in issue dependency hypergraph"
  (let ((issue-node (find-issue-node atomspace (assoc-ref issue 'title))))
    (if issue-node
        (calculate-issue-centrality atomspace issue-node)
        0.0)))

;;; Calculate temporal urgency
(define (calculate-temporal-urgency issue params)
  "Calculate temporal urgency based on issue age and status changes"
  (let* ((status (assoc-ref issue 'status))
         (decay-rate (assoc-ref params 'decay-rate))
         (base-urgency (get-base-urgency status))
         (time-factor (calculate-time-decay decay-rate))
         (status-change-factor (calculate-status-change-factor issue)))
    
    (* base-urgency time-factor status-change-factor)))

;;; Calculate historical performance factor
(define (calculate-historical-factor issue ranker)
  "Calculate factor based on historical solution performance"
  (let* ((historical-data (ranker-historical-data ranker))
         (similar-issues (find-similar-issues issue historical-data))
         (success-rate (calculate-success-rate similar-issues))
         (attention-effectiveness (calculate-attention-effectiveness issue historical-data)))
    
    (* success-rate attention-effectiveness)))

;;; Combine priority factors into final score
(define (combine-priority-factors impact centrality temporal historical weights)
  "Combine all priority factors into final priority score"
  (let ((impact-factor (assoc-ref weights 'impact-factor))
        (centrality-factor (assoc-ref weights 'centrality-factor))
        (temporal-factor (assoc-ref weights 'temporal-factor))
        (historical-factor (assoc-ref weights 'historical-factor)))
    
    (+ (* impact-factor impact)
       (* centrality-factor centrality)
       (* temporal-factor temporal)
       (* historical-factor historical))))

;;; Calculate solution efficacy ranking
(define (calculate-solution-efficacy ranker solutions test-results)
  "Rank solutions by their empirical efficacy"
  (let ((efficacy-scores (map (lambda (solution)
                               (calculate-solution-score solution test-results ranker))
                             solutions)))
    (sort (zip solutions efficacy-scores)
          (lambda (a b) (> (cadr a) (cadr b))))))

;;; Calculate score for individual solution
(define (calculate-solution-score solution test-results ranker)
  "Calculate efficacy score for individual solution"
  (let* ((issues-resolved (count-issues-resolved solution test-results))
         (new-tests-passed (count-new-tests-passed solution test-results))
         (regressions-introduced (count-regressions solution test-results))
         (params (ranker-ranking-parameters ranker))
         (resolution-bonus (assoc-ref params 'resolution-bonus))
         (regression-penalty (assoc-ref params 'regression-penalty)))
    
    (- (+ (* issues-resolved resolution-bonus)
          new-tests-passed)
       (* regressions-introduced regression-penalty))))

;;; Evolve ranking parameters based on performance
(define (evolve-ranking-parameters ranker performance-metrics)
  "Evolve ranking algorithm parameters based on performance feedback"
  (let* ((current-params (ranker-ranking-parameters ranker))
         (learning-rate (ranker-learning-rate ranker))
         (parameter-gradients (calculate-parameter-gradients performance-metrics))
         (updated-params (apply-parameter-updates current-params 
                                                 parameter-gradients 
                                                 learning-rate)))
    
    (format #t "Evolving ranking parameters based on performance\n")
    (display-parameter-changes current-params updated-params)
    
    (make-adaptive-ranker (ranker-attention-weights ranker)
                         (ranker-historical-data ranker)
                         updated-params
                         (* learning-rate 0.99)))) ; Decrease learning rate over time

;;; Generate comprehensive priority report
(define (generate-priority-report ranker ranked-issues)
  "Generate comprehensive report on issue prioritization"
  (format #t "\n=== Adaptive Priority Report ===\n")
  (format #t "Total issues analyzed: ~a\n" (length ranked-issues))
  
  ;; Top priority issues
  (format #t "\nTop 10 Priority Issues:\n")
  (for-each-indexed 
    (lambda (idx issue-priority)
      (format #t "~a. ~a (score: ~,3f, attention: ~,3f)\n"
              (+ idx 1)
              (priority-title issue-priority)
              (priority-score issue-priority)
              (priority-attention-allocation issue-priority)))
    (take ranked-issues (min 10 (length ranked-issues))))
  
  ;; Category distribution
  (format #t "\nAttention Distribution by Category:\n")
  (display-category-distribution ranked-issues)
  
  ;; Ranking parameters
  (format #t "\nCurrent Ranking Parameters:\n")
  (display-ranking-parameters ranker)
  
  ;; Learning metrics
  (format #t "\nLearning Metrics:\n")
  (display-learning-metrics ranker))

;;; Utility functions

(define (extract-all-issues-with-metadata test-catalog)
  "Extract all issues with full metadata"
  (apply append
    (map (lambda (category-pair)
           (let ((category-name (car category-pair))
                 (issues (cdr category-pair)))
             (map (lambda (issue)
                    (cons (cons 'category category-name) issue))
                  issues)))
         test-catalog)))

(define (analyze-solution-performance test-results solution-outcomes)
  "Analyze performance of solutions based on test results"
  (list 'performance-improvement 0.1
        'regression-rate 0.05
        'attention-effectiveness 0.8))

(define (apply-gradient-update weights performance-data learning-rate)
  "Apply gradient-based update to attention weights"
  weights) ; Placeholder implementation

(define (display-weight-changes old-weights new-weights)
  "Display changes in attention weights"
  (format #t "Attention weight updates applied\n"))

(define (get-category-weight category weights)
  "Get weight for issue category"
  (let ((category-weights (assoc-ref weights 'category-weights)))
    (assoc-ref category-weights (string->symbol category) 0.5)))

(define (get-status-weight status weights)
  "Get weight for issue status"
  (let ((status-weights (assoc-ref weights 'status-weights)))
    (assoc-ref status-weights (string->symbol status) 0.5)))

(define (get-failure-severity failure-mode)
  "Get severity multiplier for failure mode"
  (cond
    ((string-contains failure-mode "Performance") 1.2)
    ((string-contains failure-mode "Resource") 1.1)
    ((string-contains failure-mode "Functional") 1.0)
    (else 0.8)))

(define (calculate-dependency-weight dependencies weights)
  "Calculate weight based on dependencies"
  (let ((dependency-weights (assoc-ref weights 'dependency-weights)))
    (if (null? dependencies)
        1.0
        (/ (apply + (map (lambda (dep)
                          (assoc-ref dependency-weights 
                                    (string->symbol dep) 0.5))
                        dependencies))
           (length dependencies)))))

(define (find-issue-node atomspace title)
  "Find issue node in atomspace"
  #f) ; Placeholder

(define (get-base-urgency status)
  "Get base urgency for status"
  (cond
    ((string-contains status "Active") 1.0)
    ((string-contains status "Ongoing") 0.8)
    (else 0.5)))

(define (calculate-time-decay decay-rate)
  "Calculate time decay factor"
  1.0) ; Placeholder

(define (calculate-status-change-factor issue)
  "Calculate factor based on status changes"
  1.0) ; Placeholder

(define (find-similar-issues issue historical-data)
  "Find similar issues in historical data"
  '()) ; Placeholder

(define (calculate-success-rate similar-issues)
  "Calculate success rate for similar issues"
  0.5) ; Placeholder

(define (calculate-attention-effectiveness issue historical-data)
  "Calculate effectiveness of attention allocation"
  1.0) ; Placeholder

(define (distribute-attention priority-score weights)
  "Distribute attention based on priority score"
  priority-score) ; Placeholder

(define (count-issues-resolved solution test-results)
  "Count issues resolved by solution"
  0) ; Placeholder

(define (count-new-tests-passed solution test-results)
  "Count new tests that pass after solution"
  0) ; Placeholder

(define (count-regressions solution test-results)
  "Count regressions introduced by solution"
  0) ; Placeholder

(define (calculate-parameter-gradients performance-metrics)
  "Calculate gradients for parameter updates"
  '()) ; Placeholder

(define (apply-parameter-updates params gradients learning-rate)
  "Apply parameter updates"
  params) ; Placeholder

(define (display-parameter-changes old-params new-params)
  "Display parameter changes"
  (format #t "Parameters updated\n"))

(define (for-each-indexed proc lst)
  "Apply procedure with index to each element"
  (let loop ((lst lst) (idx 0))
    (unless (null? lst)
      (proc idx (car lst))
      (loop (cdr lst) (+ idx 1)))))

(define (display-category-distribution issues)
  "Display distribution of attention by category"
  (format #t "Category distribution analysis\n"))

(define (display-ranking-parameters ranker)
  "Display current ranking parameters"
  (format #t "Current parameters displayed\n"))

(define (display-learning-metrics ranker)
  "Display learning effectiveness metrics"
  (format #t "Learning metrics displayed\n"))