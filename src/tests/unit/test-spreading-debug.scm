;;; test-spreading-debug.scm --- Debug test for propagation tracing
;;;
;;; Copyright (C) 2025 Deep Tree Echo Project

(add-to-load-path "/home/ubuntu/occ/src")
(use-modules (core memory atomspace))
(use-modules (core propagation spreading))
(use-modules (srfi srfi-69))

(format #t "~%=== Debug Spreading Activation Test ===~%~%")

;; Create simple test: A -> B
(define as (make-atomspace))
(define a (create-concept-node as "A"))
(define b (create-concept-node as "B"))
(define a-b (create-inheritance-link as a b))

(format #t "Simple network: A -> B~%")
(format #t "  A UUID: ~a~%" (atom-uuid a))
(format #t "  B UUID: ~a~%" (atom-uuid b))
(format #t "  A-B link UUID: ~a~%~%" (atom-uuid (link-atom a-b)))

;; Propagate with debug
(define params (make-propagation-params 
                 #:decay-rate 0.8
                 #:threshold 0.0
                 #:max-hops 5
                 #:bidirectional? #f
                 #:use-truth-values? #f))

(format #t "Propagating from A with initial activation 1.0...~%~%")
(define act-map (propagate-from-atom as a 1.0 params))

(format #t "Final activations:~%")
(format #t "  A: ~a~%" (get-activation act-map a))
(format #t "  A-B link: ~a~%" (get-activation act-map a-b))
(format #t "  B: ~a~%~%" (get-activation act-map b))

(format #t "Expected:~%")
(format #t "  A: 1.0 (source)~%")
(format #t "  A-B link: 0.8 (1.0 * 0.8)~%")
(format #t "  B: 0.8 (propagated from link)~%~%")

;; Check if values match expected
(define (approx-equal? a b tolerance)
  (< (abs (- a b)) tolerance))

(define a-act (get-activation act-map a))
(define ab-act (get-activation act-map a-b))
(define b-act (get-activation act-map b))

(if (and (approx-equal? a-act 1.0 0.01)
         (approx-equal? ab-act 0.8 0.01)
         (approx-equal? b-act 0.8 0.01))
    (format #t "✓ Test PASSED: Values match expected~%")
    (format #t "✗ Test FAILED: Values don't match~%  A: ~a (expected 1.0)~%  A-B: ~a (expected 0.8)~%  B: ~a (expected 0.8)~%"
            a-act ab-act b-act))

(format #t "~%=== Test Complete ===~%")

;;; test-spreading-debug.scm ends here
