#!/usr/bin/env guile
!#

;;; Direct test of Plan9/Inferno namespace features

(define-module (test-namespace-direct)
  #:use-module (ice-9 format))

(format #t "🧪 Testing Plan9/Inferno namespace features directly...~%")

;; Test basic module loading
(catch #t
  (lambda ()
    (use-modules (cogkernel plan9-namespace))
    (format #t "✅ plan9-namespace module loaded~%")
    
    ;; Test creating a process namespace
    (format #t "~%👶 Testing process namespace creation...~%")
    (define proc-ns (make-process-namespace 1234))
    (format #t "Process namespace created: ~a~%" (process-namespace? proc-ns))
    
    ;; Test basic namespace operations without dependencies
    (format #t "~%🔧 Testing basic namespace functions...~%")
    
    ;; Test with simple data structures to avoid dependency issues
    (format #t "Process namespace PID: ~a~%" (process-namespace-pid proc-ns))
    
    (format #t "✅ Basic namespace operations working!~%"))
  (lambda (key . args)
    (format #t "❌ Test failed: ~a ~a~%" key args)))

(format #t "~%✅ Direct namespace test complete!~%")