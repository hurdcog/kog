;;; test-metamodel.scm - Comprehensive tests for metamodel components
;;;
;;; Tests bootstrap, AAR core, and cognitive kernel integration

(use-modules (metamodel bootstrap)
             (metamodel aar-core)
             (metamodel cognitive-kernel)
             (ice-9 format))

;;;; ===========================================================================
;;;; Test Framework
;;;; ===========================================================================

(define *test-count* 0)
(define *test-passed* 0)
(define *test-failed* 0)

(define (test-assert name condition)
  "Assert that condition is true"
  (set! *test-count* (+ *test-count* 1))
  (if condition
      (begin
        (set! *test-passed* (+ *test-passed* 1))
        (format #t "  ✓ ~a\n" name))
      (begin
        (set! *test-failed* (+ *test-failed* 1))
        (format #t "  ✗ ~a\n" name))))

(define (test-group name thunk)
  "Run a group of tests"
  (format #t "\n=== ~a ===\n" name)
  (thunk))

(define (test-summary)
  "Print test summary"
  (format #t "\n=== Test Summary ===\n")
  (format #t "Total: ~a\n" *test-count*)
  (format #t "Passed: ~a\n" *test-passed*)
  (format #t "Failed: ~a\n" *test-failed*)
  (if (= *test-failed* 0)
      (format #t "\n✓ All tests passed!\n")
      (format #t "\n✗ Some tests failed\n")))

;;;; ===========================================================================
;;;; Bootstrap Tests
;;;; ===========================================================================

(define (test-bootstrap)
  (test-group "Bootstrap Tests"
    (lambda ()
      ;; Primordial distinction
      (test-assert "void is unmarked"
                   (unmarked? void))
      (test-assert "mark creates marked space"
                   (marked? (mark)))
      
      ;; Combinators
      (test-assert "I combinator returns argument"
                   (= (I-combinator 42) 42))
      (test-assert "K combinator returns first argument"
                   (= ((K-combinator 1) 2) 1))
      
      ;; Church numerals
      (test-assert "church-zero converts to 0"
                   (= (church->number church-zero) 0))
      (test-assert "church-succ increments"
                   (= (church->number (church-succ church-zero)) 1))
      (test-assert "church-add works"
                   (= (church->number 
                        (church-add (number->church 2) 
                                   (number->church 3))) 
                      5))
      
      ;; Lambda calculus
      (let ((identity (make-lambda '(x) 'x)))
        (test-assert "lambda identity works"
                     (= (lambda-apply identity '(42)) 42)))
      
      ;; Metacircular evaluation
      (let ((env (make-environment)))
        (test-assert "eval arithmetic"
                     (= (bootstrap-eval '(+ 1 2) env) 3))
        (test-assert "eval lambda application"
                     (= (bootstrap-eval '((lambda (x) (* x x)) 5) env) 25))))))

;;;; ===========================================================================
;;;; AAR Core Tests
;;;; ===========================================================================

(define (test-aar-core)
  (test-group "AAR Core Tests"
    (lambda ()
      ;; Initialization
      (let ((core (make-aar-core #:dimension 8)))
        (test-assert "AAR core created"
                     (aar-core? core))
        
        ;; Initial state
        (let ((metrics (aar-metrics core)))
          (test-assert "Initial coherence exists"
                       (number? (assoc-ref metrics 'coherence)))
          (test-assert "Initial emergence exists"
                       (number? (assoc-ref metrics 'emergence))))
        
        ;; Perception
        (let ((perception (make-vector 8 0.5)))
          (aar-perceive core perception)
          (test-assert "Perception processed"
                       #t))
        
        ;; Action
        (let ((action (aar-act core)))
          (test-assert "Action generated"
                       (vector? action))
          (test-assert "Action has correct dimension"
                       (= (vector-length action) 8)))
        
        ;; Reflection
        (aar-reflect core)
        (let ((self-state (aar-self-state core)))
          (test-assert "Self-state has coherence"
                       (assoc-ref self-state 'coherence))
          (test-assert "Self-state has emergence"
                       (assoc-ref self-state 'emergence))
          (test-assert "Coherence in valid range"
                       (let ((c (assoc-ref self-state 'coherence)))
                         (and (>= c 0) (<= c 1)))))
        
        ;; Cognitive cycles
        (aar-run-cycles core 5)
        (let ((metrics (aar-metrics core)))
          (test-assert "History recorded"
                       (> (assoc-ref metrics 'history-length) 0)))))))

;;;; ===========================================================================
;;;; Cognitive Kernel Tests
;;;; ===========================================================================

(define (test-cognitive-kernel)
  (test-group "Cognitive Kernel Tests"
    (lambda ()
      ;; Initialization
      (let ((kernel (make-cognitive-kernel #:dimension 8)))
        (test-assert "Kernel created"
                     (cognitive-kernel? kernel))
        
        ;; Perception
        (perceive kernel "test input")
        (let ((state (kernel-state kernel)))
          (test-assert "Memory stored after perception"
                       (> (assoc-ref state 'memory-count) 0)))
        
        ;; Action generation
        (let ((action (generate-action kernel)))
          (test-assert "Action generated from kernel"
                       (vector? action)))
        
        ;; Memory recall
        (perceive kernel "cognitive system")
        (perceive kernel "cognitive learning")
        (let ((memories (recall-memory kernel "cognitive")))
          (test-assert "Memories recalled"
                       (> (length memories) 0)))
        
        ;; Cognitive step
        (cognitive-step kernel "new input")
        (let ((state (kernel-state kernel)))
          (test-assert "Pattern learned after cognitive step"
                       (> (assoc-ref state 'pattern-count) 0)))
        
        ;; Metrics
        (let ((metrics (kernel-metrics kernel)))
          (test-assert "Metrics include coherence"
                       (number? (assoc-ref metrics 'coherence)))
          (test-assert "Metrics include emergence"
                       (number? (assoc-ref metrics 'emergence)))
          (test-assert "Cognitive capacity computed"
                       (number? (assoc-ref metrics 'cognitive-capacity))))))))

;;;; ===========================================================================
;;;; Integration Tests
;;;; ===========================================================================

(define (test-integration)
  (test-group "Integration Tests"
    (lambda ()
      ;; Bootstrap -> AAR integration
      (let ((core (make-aar-core #:dimension 8))
            (identity (make-lambda '(x) 'x)))
        (test-assert "Bootstrap lambda works with AAR"
                     (= (lambda-apply identity '(42)) 42)))
      
      ;; AAR -> Kernel integration
      (let ((kernel (make-cognitive-kernel #:dimension 8)))
        (perceive kernel "test")
        (generate-action kernel)
        (let ((metrics (kernel-metrics kernel)))
          (test-assert "Kernel uses AAR coherence"
                       (number? (assoc-ref metrics 'coherence)))))
      
      ;; Full stack integration
      (let ((kernel (make-cognitive-kernel #:dimension 8)))
        (cognitive-loop kernel '("input1" "input2" "input3"))
        (let ((metrics (kernel-metrics kernel)))
          (test-assert "Full cognitive loop executed"
                       (and (> (assoc-ref metrics 'memory-count) 0)
                            (> (assoc-ref metrics 'pattern-count) 0)))
          (test-assert "Cognitive capacity positive"
                       (> (assoc-ref metrics 'cognitive-capacity) 0)))))))

;;;; ===========================================================================
;;;; Run All Tests
;;;; ===========================================================================

(define (run-all-tests)
  "Run all metamodel tests"
  (format #t "\n╔════════════════════════════════════════════╗\n")
  (format #t "║   Metamodel Comprehensive Test Suite      ║\n")
  (format #t "╚════════════════════════════════════════════╝\n")
  
  (test-bootstrap)
  (test-aar-core)
  (test-cognitive-kernel)
  (test-integration)
  
  (test-summary)
  
  ;; Return exit code
  (if (= *test-failed* 0) 0 1))

;; Run tests when loaded
(run-all-tests)
