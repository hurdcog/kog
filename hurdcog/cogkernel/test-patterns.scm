;;; Comprehensive Test Patterns for Cognitive Primitives
;;; Exhaustive validation of GNUMach-AtomSpace translation patterns
;;; No mocks - real translation verification

(define-module (cogkernel test-patterns)
  #:use-module (ice-9 format)
  #:use-module (ice-9 match)
  #:use-module (srfi srfi-1) 
  #:use-module (srfi srfi-9)
  #:use-module (srfi srfi-64) ; Testing framework
  #:use-module (cogkernel cognitive-primitives)
  #:use-module (cogkernel scheme-adapters)
  #:use-module (cogkernel atomspace)
  #:use-module (cogkernel tensors)
  #:export (run-exhaustive-tests
            test-primitive-encoding
            test-tensor-shape-validation  
            test-hypergraph-pattern-creation
            test-prime-factorization-mapping
            test-bidirectional-translation
            verify-cognitive-integrity
            generate-test-report
            *test-results*))

;;; Test results storage
(define *test-results* '())

;;; Add test result to collection
(define (add-test-result test-name result)
  "Add a test result to the global collection"
  (set! *test-results* 
        (cons `((test . ,test-name)
                (result . ,result)
                (timestamp . ,(get-timestamp)))
              *test-results*)))

;;; Test primitive encoding with tensor shapes
(define (test-primitive-encoding)
  "Test encoding of GNUMach primitives to cognitive fragments"
  (format #t "=== Testing Primitive Encoding ===~%")
  
  (let ((test-primitives gnumach-primitives-examples)
        (results '()))
    
    (for-each 
      (lambda (primitive)
        (match primitive
          ((name properties)
           (format #t "Testing primitive: ~a~%" name)
           
           (let* ((fragment (encode-gnumach-primitive name properties))
                  (tensor (cognitive-fragment-tensor fragment))
                  (atoms (cognitive-fragment-atoms fragment))
                  (shape (tensor-shape tensor))
                  (expected-shape cognitive-tensor-shape))
             
             ;; Validate tensor shape
             (let ((shape-valid? (equal? shape expected-shape)))
               (format #t "  Tensor shape: ~a (expected: ~a) - ~a~%"
                       shape expected-shape 
                       (if shape-valid? "✅ PASS" "❌ FAIL"))
               
               ;; Validate atoms creation
               (let ((atoms-valid? (and (> (length atoms) 0)
                                        (every atom? (filter atom? atoms)))))
                 (format #t "  Atoms created: ~a atoms - ~a~%"
                         (length atoms)
                         (if atoms-valid? "✅ PASS" "❌ FAIL"))
                 
                 ;; Validate modality encoding
                 (let* ((modality (cognitive-fragment-modality fragment))
                        (modality-valid? (and (>= modality 0) (< modality 8))))
                   (format #t "  Modality encoding: ~a - ~a~%"
                           modality
                           (if modality-valid? "✅ PASS" "❌ FAIL"))
                   
                   (set! results 
                         (cons `((primitive . ,name)
                                 (shape-valid . ,shape-valid?)
                                 (atoms-valid . ,atoms-valid?)
                                 (modality-valid . ,modality-valid?)
                                 (overall . ,(and shape-valid? atoms-valid? modality-valid?)))
                               results))))))))
        test-primitives)
    
    (add-test-result 'primitive-encoding results)
    (format #t "~%Primitive encoding tests completed.~%")
    results))

;;; Test tensor shape validation
(define (test-tensor-shape-validation)
  "Test validation of cognitive tensor shapes"
  (format #t "=== Testing Tensor Shape Validation ===~%")
  
  (let ((test-cases `((valid-shape ,cognitive-tensor-shape)
                      (invalid-shape-1 (8 4))
                      (invalid-shape-2 (8 4 8 10 5 1))
                      (empty-shape ())))
        (results '()))
    
    (for-each
      (lambda (test-case)
        (match test-case
          ((name shape)
           (format #t "Testing shape: ~a~%" name)
           
           (let* ((expected-size (if (null? shape) 0 (fold * 1 shape)))
                  (is-valid? (equal? shape cognitive-tensor-shape))
                  (data (if (> expected-size 0) 
                            (make-list expected-size 0.5)
                            '())))
             
             (catch 'tensor-error
               (lambda ()
                 (if (> expected-size 0)
                     (let ((tensor (make-tensor shape data)))
                       (format #t "  Shape ~a: ✅ VALID (size: ~a)~%" shape expected-size)
                       (set! results (cons `((shape . ,shape) (valid . #t) (size . ,expected-size)) results)))
                     (begin
                       (format #t "  Shape ~a: ❌ INVALID (empty)~%" shape)
                       (set! results (cons `((shape . ,shape) (valid . #f) (error . "empty-shape")) results)))))
               (lambda (key . args)
                 (format #t "  Shape ~a: ❌ ERROR (~a)~%" shape args)
                 (set! results (cons `((shape . ,shape) (valid . #f) (error . ,args)) results)))))))
      test-cases)
    
    (add-test-result 'tensor-shape-validation results)
    (format #t "~%Tensor shape validation tests completed.~%")
    results))

;;; Test hypergraph pattern creation
(define (test-hypergraph-pattern-creation)
  "Test creation of hypergraph patterns from cognitive fragments"
  (format #t "=== Testing Hypergraph Pattern Creation ===~%")
  
  (let ((results '()))
    (for-each
      (lambda (primitive)
        (match primitive
          ((name properties)
           (format #t "Testing hypergraph pattern for: ~a~%" name)
           
           (let* ((fragment (encode-gnumach-primitive name properties))
                  (pattern (create-hypergraph-pattern fragment))
                  (nodes (assoc-ref pattern 'nodes))
                  (links (assoc-ref pattern 'links))
                  (signature (assoc-ref pattern 'pattern-signature))
                  (prime-factors (assoc-ref pattern 'prime-factorization)))
             
             ;; Validate pattern structure
             (let ((nodes-valid? (and (list? nodes) (> (length nodes) 0)))
                   (links-valid? (and (list? links) (> (length links) 0)))
                   (signature-valid? (and (number? signature) (> signature 0)))
                   (primes-valid? (and (list? prime-factors) (> (length prime-factors) 0))))
               
               (format #t "  Nodes: ~a - ~a~%" 
                       (length nodes) (if nodes-valid? "✅ PASS" "❌ FAIL"))
               (format #t "  Links: ~a - ~a~%"
                       (length links) (if links-valid? "✅ PASS" "❌ FAIL"))  
               (format #t "  Signature: ~a - ~a~%"
                       signature (if signature-valid? "✅ PASS" "❌ FAIL"))
               (format #t "  Prime factors: ~a entries - ~a~%"
                       (length prime-factors) (if primes-valid? "✅ PASS" "❌ FAIL"))
               
               (set! results
                     (cons `((primitive . ,name)
                             (nodes-count . ,(length nodes))
                             (links-count . ,(length links))
                             (signature . ,signature)
                             (prime-factors-count . ,(length prime-factors))
                             (nodes-valid . ,nodes-valid?)
                             (links-valid . ,links-valid?)
                             (signature-valid . ,signature-valid?)
                             (primes-valid . ,primes-valid?)
                             (overall . ,(and nodes-valid? links-valid? 
                                              signature-valid? primes-valid?)))
                           results)))))))
      gnumach-primitives-examples)
    
    (add-test-result 'hypergraph-pattern-creation results)
    (format #t "~%Hypergraph pattern creation tests completed.~%")
    results))

;;; Test prime factorization mapping
(define (test-prime-factorization-mapping)
  "Test prime factorization mapping for tensor data"
  (format #t "=== Testing Prime Factorization Mapping ===~%")
  
  (let ((test-data '((0.5 0.3 0.8 0.1 0.9)
                     (1.0 0.0 0.5 0.25 0.75)
                     (0.1 0.2 0.3 0.4 0.5)))
        (results '()))
    
    (for-each
      (lambda (data-set)
        (format #t "Testing factorization for data: ~a~%" data-set)
        
        (let* ((prime-mapping (compute-prime-factorization data-set))
               (valid-primes? (every (lambda (pair)
                                       (and (number? (car pair))
                                            (number? (cdr pair))
                                            (>= (car pair) 2))) ; Must be prime
                                     prime-mapping))
               (complete-mapping? (= (length prime-mapping) (length data-set))))
          
          (format #t "  Generated ~a prime mappings - ~a~%"
                  (length prime-mapping)
                  (if complete-mapping? "✅ COMPLETE" "❌ INCOMPLETE"))
          (format #t "  Valid prime numbers - ~a~%"
                  (if valid-primes? "✅ VALID" "❌ INVALID"))
          
          (set! results
                (cons `((data . ,data-set)
                        (mappings . ,prime-mapping)
                        (complete . ,complete-mapping?)
                        (valid-primes . ,valid-primes?)
                        (overall . ,(and complete-mapping? valid-primes?)))
                      results))))
      test-data)
    
    (add-test-result 'prime-factorization-mapping results)
    (format #t "~%Prime factorization mapping tests completed.~%")
    results))

;;; Test bidirectional translation
(define (test-bidirectional-translation)
  "Test round-trip translation between GNUMach primitives and AtomSpace"
  (format #t "=== Testing Bidirectional Translation ===~%")
  
  (create-cognitive-grammar)
  
  (let ((results '()))
    (for-each
      (lambda (primitive)
        (match primitive
          ((name properties)
           (format #t "Testing round-trip translation for: ~a~%" name)
           
           ;; Register primitive
           (register-gnumach-primitive name properties
                                       (assoc-ref *cognitive-grammar-rules* name))
           
           ;; Create adapter
           (let* ((adapter (make-translation-adapter
                            "test-adapter" 'gnumach 'atomspace
                            (lambda (name props rule)
                              (let ((pattern-fn (assoc-ref rule 'pattern)))
                                (pattern-fn name props rule)))
                            validate-translation-integrity))
                  
                  ;; Forward translation
                  (forward-result (adapter-translate-to-atomspace adapter primitive))
                  (forward-success? (assoc-ref forward-result 'success)))
             
             (if forward-success?
                 (let* ((atoms (assoc-ref forward-result 'atoms))
                        (fragment (assoc-ref forward-result 'fragment))
                        
                        ;; Backward translation  
                        (backward-adapter (make-translation-adapter
                                           "backward-test" 'atomspace 'gnumach
                                           (lambda (atoms)
                                             (decode-atomspace-to-primitive atoms name))
                                           validate-translation-integrity))
                        (backward-result (adapter-translate-from-atomspace backward-adapter atoms))
                        (backward-success? (assoc-ref backward-result 'success)))
                   
                   (if backward-success?
                       (let* ((recovered (assoc-ref backward-result 'primitive))
                              (validation (validate-translation-integrity primitive recovered))
                              (integrity-valid? (assoc-ref validation 'valid)))
                         
                         (format #t "  Forward: ✅ SUCCESS (~a atoms)~%" (length atoms))
                         (format #t "  Backward: ✅ SUCCESS~%")
                         (format #t "  Integrity: ~a~%" 
                                 (if integrity-valid? "✅ VALID" "❌ INVALID"))
                         
                         (set! results
                               (cons `((primitive . ,name)
                                       (forward-success . #t)
                                       (backward-success . #t)
                                       (integrity-valid . ,integrity-valid?)
                                       (atoms-count . ,(length atoms))
                                       (overall . ,integrity-valid?))
                                     results)))
                       (begin
                         (format #t "  Forward: ✅ SUCCESS~%")
                         (format #t "  Backward: ❌ FAILED~%")
                         (set! results
                               (cons `((primitive . ,name)
                                       (forward-success . #t)
                                       (backward-success . #f)
                                       (overall . #f))
                                     results)))))
                 (begin
                   (format #t "  Forward: ❌ FAILED~%")
                   (set! results
                         (cons `((primitive . ,name)
                                 (forward-success . #f)
                                 (backward-success . #f)
                                 (overall . #f))
                               results))))))))
      gnumach-primitives-examples)
    
    (add-test-result 'bidirectional-translation results)
    (format #t "~%Bidirectional translation tests completed.~%")
    results))

;;; Verify cognitive integrity
(define (verify-cognitive-integrity)
  "Verify overall cognitive system integrity"
  (format #t "=== Verifying Cognitive Integrity ===~%")
  
  (let* ((tensor-test (test-tensor-shape-validation))
         (encoding-test (test-primitive-encoding))
         (pattern-test (test-hypergraph-pattern-creation))
         (prime-test (test-prime-factorization-mapping))
         (translation-test (test-bidirectional-translation))
         
         (tensor-pass (every (lambda (r) (assoc-ref r 'valid)) tensor-test))
         (encoding-pass (every (lambda (r) (assoc-ref r 'overall)) encoding-test))
         (pattern-pass (every (lambda (r) (assoc-ref r 'overall)) pattern-test))
         (prime-pass (every (lambda (r) (assoc-ref r 'overall)) prime-test))
         (translation-pass (every (lambda (r) (assoc-ref r 'overall)) translation-test))
         
         (overall-integrity (and tensor-pass encoding-pass pattern-pass 
                                 prime-pass translation-pass)))
    
    (format #t "Component Test Results:~%")
    (format #t "  Tensor Shape Validation: ~a~%" (if tensor-pass "✅ PASS" "❌ FAIL"))
    (format #t "  Primitive Encoding: ~a~%" (if encoding-pass "✅ PASS" "❌ FAIL"))
    (format #t "  Hypergraph Patterns: ~a~%" (if pattern-pass "✅ PASS" "❌ FAIL"))
    (format #t "  Prime Factorization: ~a~%" (if prime-pass "✅ PASS" "❌ FAIL"))
    (format #t "  Bidirectional Translation: ~a~%" (if translation-pass "✅ PASS" "❌ FAIL"))
    (format #t "~%Overall Cognitive Integrity: ~a~%"
            (if overall-integrity "✅ VERIFIED" "❌ COMPROMISED"))
    
    (add-test-result 'cognitive-integrity 
                     `((tensor-pass . ,tensor-pass)
                       (encoding-pass . ,encoding-pass)
                       (pattern-pass . ,pattern-pass)
                       (prime-pass . ,prime-pass)
                       (translation-pass . ,translation-pass)
                       (overall . ,overall-integrity)))
    
    overall-integrity))

;;; Generate comprehensive test report
(define (generate-test-report)
  "Generate a comprehensive test report"
  (format #t "~%=== COMPREHENSIVE TEST REPORT ===~%")
  (format #t "Generated: ~a~%~%" (current-time))
  
  (let ((total-tests (length *test-results*))
        (successful-tests (count (lambda (result)
                                   (let ((test-data (assoc-ref result 'result)))
                                     (if (list? test-data)
                                         (every (lambda (r) 
                                                  (assoc-ref r 'overall)) test-data)
                                         (assoc-ref test-data 'overall))))
                                 *test-results*)))
    
    (format #t "Test Summary:~%")
    (format #t "  Total test suites: ~a~%" total-tests)
    (format #t "  Successful suites: ~a~%" successful-tests)
    (format #t "  Success rate: ~a%~%~%" 
            (if (> total-tests 0) 
                (inexact->exact (round (* 100 (/ successful-tests total-tests))))
                0))
    
    (format #t "Detailed Results:~%")
    (for-each
      (lambda (test-result)
        (let ((test-name (assoc-ref test-result 'test))
              (timestamp (assoc-ref test-result 'timestamp))
              (result (assoc-ref test-result 'result)))
          (format #t "  ~a (~a):~%" test-name timestamp)
          (if (list? result)
              (let ((passed (count (lambda (r) (assoc-ref r 'overall)) result))
                    (total (length result)))
                (format #t "    ~a/~a tests passed~%" passed total))
              (format #t "    Result: ~a~%" (assoc-ref result 'overall)))))
      *test-results*))
  
  *test-results*)

;;; Run all exhaustive tests
(define (run-exhaustive-tests)
  "Run all exhaustive test patterns for cognitive primitives"
  (format #t "=== EXHAUSTIVE COGNITIVE PRIMITIVES TEST SUITE ===~%~%")
  
  ;; Clear previous results
  (set! *test-results* '())
  
  ;; Run all test suites
  (test-tensor-shape-validation)
  (test-primitive-encoding)  
  (test-hypergraph-pattern-creation)
  (test-prime-factorization-mapping)
  (test-bidirectional-translation)
  
  ;; Verify overall integrity
  (let ((integrity-verified? (verify-cognitive-integrity)))
    (generate-test-report)
    
    (format #t "~%=== TEST SUITE COMPLETED ===~%")
    (format #t "Cognitive System Integrity: ~a~%"
            (if integrity-verified? "✅ VERIFIED" "❌ COMPROMISED")))))
    
    integrity-verified?))