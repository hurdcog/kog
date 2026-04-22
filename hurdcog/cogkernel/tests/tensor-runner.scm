;;; Tensor-Based Test Runner with Coverage Mapping
;;; Outputs test coverage as tensors for cognitive grammar integration
;;; Dimensions: test sets × issues × solution commits

(define-module (cogkernel tests tensor-runner)
  #:use-module (ice-9 format)
  #:use-module (ice-9 threads)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (cogkernel core)
  #:use-module (cogkernel tensors)
  #:use-module (cogkernel tests unit-test-generator)
  #:use-module (cogkernel tests hypergraph-encoding)
  #:export (create-tensor-test-runner
            execute-test-suite-with-tensors
            generate-coverage-tensor
            track-solution-impact
            optimize-test-schedule
            export-tensor-results))

;;; Tensor test runner record
(define-record-type <tensor-test-runner>
  (make-tensor-test-runner test-catalog coverage-tensor solution-history 
                          current-session tensor-shapes)
  tensor-test-runner?
  (test-catalog runner-test-catalog)
  (coverage-tensor runner-coverage-tensor)
  (solution-history runner-solution-history)
  (current-session runner-current-session)
  (tensor-shapes runner-tensor-shapes))

;;; Test result record
(define-record-type <test-result>
  (make-test-result test-name category status execution-time 
                   pass-count fail-count tensor-output)
  test-result?
  (test-name result-test-name)
  (category result-category)
  (status result-status)
  (execution-time result-execution-time)
  (pass-count result-pass-count)
  (fail-count result-fail-count)
  (tensor-output result-tensor-output))

;;; Create tensor test runner
(define (create-tensor-test-runner test-catalog)
  "Create tensor-based test runner with coverage tracking"
  (let* ((num-tests (count-total-tests test-catalog))
         (num-issues (count-total-issues test-catalog))
         (tensor-shapes (calculate-tensor-shapes num-tests num-issues))
         (coverage-tensor (initialize-coverage-tensor tensor-shapes)))
    (make-tensor-test-runner test-catalog coverage-tensor '() 
                            (generate-session-id) tensor-shapes)))

;;; Calculate tensor shapes based on test catalog
(define (calculate-tensor-shapes num-tests num-issues)
  "Calculate optimal tensor shapes for coverage mapping"
  `((coverage-shape . (,num-tests ,num-issues 1))
    (solution-impact-shape . (,num-issues ,num-issues))
    (temporal-shape . (,num-tests 100)) ; 100 time steps
    (attention-shape . (,num-issues 8)))) ; 8 attention categories

;;; Initialize coverage tensor with zeros
(define (initialize-coverage-tensor tensor-shapes)
  "Initialize coverage tensor with appropriate dimensions"
  (let ((shape (assoc-ref tensor-shapes 'coverage-shape)))
    (make-tensor shape 0.0)))

;;; Execute test suite with tensor output
(define (execute-test-suite-with-tensors runner test-functions)
  "Execute test suite and generate tensor coverage maps"
  (format #t "=== Tensor Test Runner Session ===\n")
  (format #t "Session ID: ~a\n" (runner-current-session runner))
  (format #t "Executing ~a tests\n" (length test-functions))
  
  (let* ((start-time (current-time))
         (test-results (map (lambda (test-func)
                             (execute-single-test-with-tensor runner test-func))
                           test-functions))
         (end-time (current-time))
         (total-time (- end-time start-time))
         (updated-runner (update-coverage-tensor runner test-results)))
    
    (format #t "Total execution time: ~a seconds\n" total-time)
    (display-test-summary test-results)
    (export-tensor-results updated-runner)
    updated-runner))

;;; Execute single test with tensor output
(define (execute-single-test-with-tensor runner test-function)
  "Execute single test and capture tensor output"
  (format #t "Executing test: ~a\n" test-function)
  
  (let* ((start-time (get-internal-real-time))
         (test-result (catch #t
                        (lambda () (test-function))
                        (lambda (key . args)
                          (format #t "Test failed with exception: ~a ~a\n" key args)
                          '(failed))))
         (end-time (get-internal-real-time))
         (execution-time (/ (- end-time start-time) internal-time-units-per-second))
         (pass-count (count-passes test-result))
         (fail-count (count-failures test-result))
         (tensor-output (generate-test-tensor test-result runner)))
    
    (make-test-result (symbol->string test-function)
                     (infer-test-category test-function)
                     (if (> fail-count 0) 'failed 'passed)
                     execution-time
                     pass-count
                     fail-count
                     tensor-output)))

;;; Generate tensor representation of test results
(define (generate-test-tensor test-result runner)
  "Generate tensor representation of individual test result"
  (let* ((tensor-shapes (runner-tensor-shapes runner))
         (coverage-shape (assoc-ref tensor-shapes 'coverage-shape))
         (test-tensor (make-tensor '(1 10) 0.0))) ; Simple 1x10 tensor for now
    
    ;; Encode test outcomes as tensor values
    (tensor-set! test-tensor 0 0 (if (eq? test-result 'passed) 1.0 0.0))
    (tensor-set! test-tensor 0 1 (if (list? test-result) (length test-result) 0))
    
    test-tensor))

;;; Update coverage tensor with new test results
(define (update-coverage-tensor runner test-results)
  "Update coverage tensor with results from test execution"
  (let ((current-tensor (runner-coverage-tensor runner))
        (test-catalog (runner-test-catalog runner)))
    
    (for-each (lambda (result)
                (let* ((test-name (result-test-name result))
                       (test-index (find-test-index test-catalog test-name))
                       (issues-covered (find-issues-covered test-catalog test-name)))
                  
                  (for-each (lambda (issue-index)
                              (let ((current-value (tensor-ref current-tensor test-index issue-index 0))
                                    (new-value (if (eq? (result-status result) 'passed) 1.0 0.0)))
                                (tensor-set! current-tensor test-index issue-index 0
                                           (max current-value new-value))))
                            issues-covered)))
              test-results)
    
    (make-tensor-test-runner (runner-test-catalog runner)
                            current-tensor
                            (cons test-results (runner-solution-history runner))
                            (runner-current-session runner)
                            (runner-tensor-shapes runner))))

;;; Generate coverage tensor for specific test set
(define (generate-coverage-tensor runner test-subset issue-subset)
  "Generate coverage tensor for specific subset of tests and issues"
  (let* ((subset-tests (filter-tests-by-subset runner test-subset))
         (subset-issues (filter-issues-by-subset runner issue-subset))
         (coverage-shape (list (length subset-tests) (length subset-issues) 1))
         (coverage-tensor (make-tensor coverage-shape 0.0)))
    
    ;; Fill tensor with coverage data
    (for-each-indexed 
      (lambda (test-idx test)
        (for-each-indexed
          (lambda (issue-idx issue)
            (let ((coverage-value (calculate-test-issue-coverage test issue)))
              (tensor-set! coverage-tensor test-idx issue-idx 0 coverage-value)))
          subset-issues))
      subset-tests)
    
    coverage-tensor))

;;; Track solution impact through tensor operations
(define (track-solution-impact runner solution-commit)
  "Track impact of solution implementation through tensor analysis"
  (let* ((pre-solution-tensor (runner-coverage-tensor runner))
         (post-solution-tensor (execute-tests-after-solution runner solution-commit))
         (impact-tensor (tensor-subtract post-solution-tensor pre-solution-tensor))
         (impact-statistics (analyze-impact-tensor impact-tensor)))
    
    (format #t "Solution impact analysis for commit: ~a\n" solution-commit)
    (format #t "  Issues resolved: ~a\n" (assoc-ref impact-statistics 'resolved-count))
    (format #t "  New regressions: ~a\n" (assoc-ref impact-statistics 'regression-count))
    (format #t "  Overall improvement: ~a\n" (assoc-ref impact-statistics 'improvement-score))
    
    impact-tensor))

;;; Optimize test schedule based on attention allocation
(define (optimize-test-schedule runner attention-weights)
  "Optimize test execution order based on cognitive attention allocation"
  (let* ((test-catalog (runner-test-catalog runner))
         (all-tests (extract-all-tests test-catalog))
         (prioritized-tests (sort all-tests
                                 (lambda (a b)
                                   (> (calculate-test-priority a attention-weights)
                                      (calculate-test-priority b attention-weights))))))
    
    (format #t "Optimized test schedule:\n")
    (for-each-indexed
      (lambda (idx test)
        (format #t "  ~a. ~a (priority: ~,3f)\n" 
                (+ idx 1) 
                (assoc-ref test 'title)
                (calculate-test-priority test attention-weights)))
      prioritized-tests)
    
    prioritized-tests))

;;; Calculate test priority based on attention weights
(define (calculate-test-priority test attention-weights)
  "Calculate test priority using attention allocation weights"
  (let* ((category (assoc-ref test 'category))
         (impact (assoc-ref test 'impact-area))
         (dependencies (assoc-ref test 'dependencies))
         (base-priority (assoc-ref attention-weights category 0.5))
         (impact-multiplier (get-impact-multiplier impact))
         (dependency-bonus (* 0.1 (length dependencies))))
    
    (* base-priority impact-multiplier (+ 1.0 dependency-bonus))))

;;; Export tensor results in multiple formats
(define (export-tensor-results runner)
  "Export tensor results in various formats for analysis"
  (let* ((session-id (runner-current-session runner))
         (coverage-tensor (runner-coverage-tensor runner))
         (export-dir (format #f "tensor-exports/~a" session-id)))
    
    ;; Create export directory
    (system* "mkdir" "-p" export-dir)
    
    ;; Export as JSON
    (export-tensor-as-json coverage-tensor 
                          (string-append export-dir "/coverage-tensor.json"))
    
    ;; Export as CSV
    (export-tensor-as-csv coverage-tensor
                         (string-append export-dir "/coverage-tensor.csv"))
    
    ;; Export metadata
    (export-test-metadata runner
                         (string-append export-dir "/metadata.json"))
    
    (format #t "Tensor results exported to: ~a\n" export-dir)))

;;; Export tensor as JSON
(define (export-tensor-as-json tensor filename)
  "Export tensor data as JSON format"
  (let ((json-data (tensor-to-json tensor)))
    (with-output-to-file filename
      (lambda ()
        (display json-data)))))

;;; Export tensor as CSV
(define (export-tensor-as-csv tensor filename)
  "Export tensor data as CSV format"
  (with-output-to-file filename
    (lambda ()
      (let ((shape (tensor-shape tensor)))
        (cond
          ((= (length shape) 2)
           (export-2d-tensor-csv tensor))
          ((= (length shape) 3)
           (export-3d-tensor-csv tensor))
          (else
           (format #t "Unsupported tensor dimensions for CSV export\n")))))))

;;; Utility functions

(define (count-total-tests test-catalog)
  "Count total number of tests in catalog"
  (apply + (map (lambda (cat) (length (cdr cat))) test-catalog)))

(define (count-total-issues test-catalog)
  "Count total number of issues in catalog"
  (count-total-tests test-catalog)) ; Same as tests for now

(define (generate-session-id)
  "Generate unique session ID"
  (format #f "session-~a" (current-time)))

(define (count-passes test-result)
  "Count passing assertions in test result"
  (if (list? test-result)
      (length (filter identity test-result))
      (if (eq? test-result 'passed) 1 0)))

(define (count-failures test-result)
  "Count failing assertions in test result"
  (if (list? test-result)
      (length (filter not test-result))
      (if (eq? test-result 'failed) 1 0)))

(define (infer-test-category test-function)
  "Infer test category from function name"
  (let ((name (symbol->string test-function)))
    (cond
      ((string-contains name "performance") 'performance)
      ((string-contains name "stability") 'stability)
      ((string-contains name "hardware") 'hardware)
      (else 'general))))

(define (find-test-index test-catalog test-name)
  "Find index of test in catalog"
  0) ; Placeholder

(define (find-issues-covered test-catalog test-name)
  "Find issues covered by test"
  '(0)) ; Placeholder

(define (for-each-indexed proc lst)
  "Apply procedure with index to each element"
  (let loop ((lst lst) (idx 0))
    (unless (null? lst)
      (proc idx (car lst))
      (loop (cdr lst) (+ idx 1)))))

(define (filter-tests-by-subset runner subset)
  "Filter tests by subset criteria"
  '()) ; Placeholder

(define (filter-issues-by-subset runner subset)
  "Filter issues by subset criteria"
  '()) ; Placeholder

(define (calculate-test-issue-coverage test issue)
  "Calculate coverage between test and issue"
  0.5) ; Placeholder

(define (execute-tests-after-solution runner solution-commit)
  "Execute tests after solution implementation"
  (runner-coverage-tensor runner)) ; Placeholder

(define (analyze-impact-tensor tensor)
  "Analyze impact tensor for statistics"
  '((resolved-count . 0)
    (regression-count . 0)
    (improvement-score . 0.0))) ; Placeholder

(define (extract-all-tests test-catalog)
  "Extract all tests from catalog"
  (apply append (map cdr test-catalog)))

(define (get-impact-multiplier impact)
  "Get multiplier based on impact area"
  (cond
    ((string-contains impact "performance") 1.2)
    ((string-contains impact "stability") 1.1)
    (else 1.0)))

(define (tensor-to-json tensor)
  "Convert tensor to JSON representation"
  "{}") ; Placeholder

(define (export-2d-tensor-csv tensor)
  "Export 2D tensor as CSV"
  #t) ; Placeholder

(define (export-3d-tensor-csv tensor)
  "Export 3D tensor as CSV"
  #t) ; Placeholder

(define (export-test-metadata runner filename)
  "Export test metadata"
  #t) ; Placeholder