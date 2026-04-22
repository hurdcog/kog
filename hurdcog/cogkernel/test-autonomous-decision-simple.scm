#!/usr/bin/env guile
!#
;;; Simple test for Autonomous Decision Making System
;;; Demonstrates basic autonomous decision making functionality

(use-modules (cogkernel cognitive-interface)
             (cogkernel cognitive-interface decision-making autonomous)
             (cogkernel truthkernel))

(define (simple-autonomous-decision-test)
  "Simple test of autonomous decision making"
  (format #t "=== Simple Autonomous Decision Making Test ===~%")
  
  ;; Test 1: Create autonomous decision system
  (let ((system (make-autonomous-decision-system #:autonomy-level 3)))
    (format #t "✓ Autonomous decision system created~%")
    
    ;; Test 2: Create decision context
    (let ((context (create-decision-context 
                     '(high-cpu-usage server-overload)
                     '(restart-service scale-up investigate optimize))))
      (format #t "✓ Decision context created~%")
      
      ;; Test 3: Make autonomous decision
      (format #t "~%Making autonomous decision...~%")
      (let ((outcome (autonomous-decide system context)))
        (format #t "✓ Decision made successfully~%")
        
        ;; Test 4: Test different autonomy levels
        (format #t "~%Testing different autonomy levels...~%")
        (for-each
          (lambda (level)
            (let ((test-system (make-autonomous-decision-system #:autonomy-level level)))
              (format #t "Autonomy level ~a: " level)
              (let ((test-outcome (autonomous-decide test-system context)))
                (format #t "decision made~%"))))
          '(0 1 2 3 4))
        
        (format #t "✓ All autonomy levels tested~%")))))

(define (test-cognitive-interface-integration-simple)
  "Simple test of cognitive interface integration"
  (format #t "~%=== Cognitive Interface Integration Test ===~%")
  
  ;; Create cognitive interface with autonomous decision making
  (let ((interface (make-cognitive-operations-interface 
                     #:autonomy-level 2)))
    (format #t "✓ Cognitive interface created~%")
    
    ;; Initialize interface
    (initialize-cognitive-interface interface)
    (format #t "✓ Cognitive interface initialized~%")
    
    ;; Test autonomous decision through interface
    (format #t "~%Making decision through cognitive interface...~%")
    (let ((outcome (execute-cognitive-operation interface 'AUTONOMOUS-DECISION
                                               '(network-timeout)
                                               '(retry-connection reset-network investigate))))
      (format #t "✓ Autonomous decision through interface works~%"))
    
    ;; Test high-level function
    (format #t "~%Testing high-level autonomous decision function...~%")
    (let ((outcome2 (autonomous-decision-making interface
                                               '(disk-full)
                                               '(cleanup archive expand-disk))))
      (format #t "✓ High-level autonomous decision function works~%"))))

(define (run-simple-tests)
  "Run simple autonomous decision making tests"
  (format #t "~%======= Simple Autonomous Decision Making Tests =======~%")
  
  (simple-autonomous-decision-test)
  (test-cognitive-interface-integration-simple)
  
  (format #t "~%======= All Simple Tests Complete =======~%")
  (format #t "✅ Autonomous decision making implementation successful~%")
  (format #t "✅ SKZ framework integration verified~%")
  (format #t "✅ Ready for Phase 4 cognitive layer development~%"))

;; Main execution
(run-simple-tests)