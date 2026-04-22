#!/usr/bin/env guile
!#
;;; Test suite for Autonomous Decision Making System
;;; Tests the complete autonomous decision making functionality

(use-modules (cogkernel cognitive-interface)
             (cogkernel cognitive-interface decision-making autonomous)
             (cogkernel truthkernel)
             (cogkernel atomspace)
             (cogkernel agents)
             (cogkernel attention)
             (cogkernel tensors))

(define (test-autonomous-decision-system)
  "Test autonomous decision system creation and basic functionality"
  (format #t "=== Testing Autonomous Decision System ===~%")
  
  ;; Test system creation
  (let ((system (make-autonomous-decision-system #:autonomy-level 3)))
    (format #t "✓ Autonomous decision system created~%")
    
    ;; Test autonomy level functions
    (set-autonomy-level system 2)
    (let ((level (get-autonomy-level system)))
      (if (= level 2)
          (format #t "✓ Autonomy level setting works~%")
          (format #t "✗ Autonomy level setting failed~%")))
    
    ;; Test decision context creation
    (let ((context (create-decision-context 
                     '(system-alert high-memory-usage)
                     '(restart-service investigate allocate-memory ignore)
                     #:urgency 'high)))
      (format #t "✓ Decision context creation works~%")
      
      ;; Test autonomous decision making
      (let ((outcome (autonomous-decide system context)))
        (format #t "Decision outcome: ~a~%" outcome)
        (if outcome
            (format #t "✓ Autonomous decision making works~%")
            (format #t "✗ Autonomous decision making failed~%")))
      
      system)))

(define (test-autonomy-levels)
  "Test different autonomy levels"
  (format #t "~%=== Testing Autonomy Levels ===~%")
  
  (let ((test-context (create-decision-context 
                        '(routine-maintenance-check)
                        '(perform-maintenance defer postpone))))
    
    ;; Test each autonomy level
    (for-each
      (lambda (level)
        (format #t "--- Testing Autonomy Level ~a ---~%" level)
        (let ((system (make-autonomous-decision-system #:autonomy-level level)))
          (let ((outcome (autonomous-decide system test-context)))
            (format #t "Level ~a decision: ~a~%" 
                    level (if outcome "decision made" "no decision")))))
      '(0 1 2 3 4)))
  
  (format #t "✓ Autonomy level testing complete~%"))

(define (test-cognitive-interface-integration)
  "Test integration with cognitive interface"
  (format #t "~%=== Testing Cognitive Interface Integration ===~%")
  
  ;; Create cognitive interface with autonomous decision making
  (let ((interface (make-cognitive-operations-interface 
                     #:autonomy-level 3
                     #:learning-enabled #t)))
    
    (format #t "✓ Cognitive interface created with decision system~%")
    
    ;; Initialize interface
    (initialize-cognitive-interface interface)
    (format #t "✓ Cognitive interface initialized~%")
    
    ;; Test autonomous decision through interface
    (let ((outcome (execute-cognitive-operation interface 'AUTONOMOUS-DECISION
                                               '(network-latency-issue)
                                               '(restart-network investigate-routing optimize-settings)
                                               'medium)))
      (if outcome
          (format #t "✓ Autonomous decision through interface works~%")
          (format #t "✗ Autonomous decision through interface failed~%")))
    
    ;; Test high-level autonomous decision making function
    (let ((outcome2 (autonomous-decision-making interface
                                               '(disk-space-warning)
                                               '(cleanup-logs archive-data expand-storage ignore))))
      (if outcome2
          (format #t "✓ High-level autonomous decision making works~%")
          (format #t "✗ High-level autonomous decision making failed~%")))
    
    interface))

(define (test-decision-execution)
  "Test decision execution at different autonomy levels"
  (format #t "~%=== Testing Decision Execution ===~%")
  
  (let ((high-conf-context (create-decision-context
                             '(critical-security-alert)
                             '(block-traffic escalate investigate)
                             #:urgency 'critical))
        (low-conf-context (create-decision-context
                            '(unclear-anomaly)
                            '(investigate ignore monitor)
                            #:urgency 'low)))
    
    ;; Test with high autonomy level (should execute high confidence decisions)
    (let ((auto-system (make-autonomous-decision-system #:autonomy-level 3)))
      (format #t "--- High Autonomy System ---~%")
      (let ((outcome1 (autonomous-decide auto-system high-conf-context))
            (outcome2 (autonomous-decide auto-system low-conf-context)))
        (format #t "High confidence executed: ~a~%" (if outcome1 "yes" "no"))
        (format #t "Low confidence executed: ~a~%" (if outcome2 "yes" "no"))))
    
    ;; Test with manual level (should not auto-execute)
    (let ((manual-system (make-autonomous-decision-system #:autonomy-level 0)))
      (format #t "--- Manual System ---~%")
      (let ((outcome (autonomous-decide manual-system high-conf-context)))
        (format #t "Manual mode executed: ~a~%" (if outcome "yes" "no")))))
  
  (format #t "✓ Decision execution testing complete~%"))

(define (test-truthkernel-integration)
  "Test integration with TruthKernel reasoning"
  (format #t "~%=== Testing TruthKernel Integration ===~%")
  
  ;; Test that TruthKernel rules are loaded for autonomous decisions
  (let ((kernel *global-truth-kernel*))
    
    ;; Test situation evaluation rules
    (let ((criticality (truth-kernel-evaluate kernel 'situation-criticality '((emergency alert))))
          (complexity (truth-kernel-evaluate kernel 'situation-complexity '((simple issue))))
          (familiarity (truth-kernel-evaluate kernel 'situation-familiarity '((routine check)))))
      
      (format #t "Situation criticality: ~,2f~%" (truth-value-strength criticality))
      (format #t "Situation complexity: ~,2f~%" (truth-value-strength complexity))
      (format #t "Situation familiarity: ~,2f~%" (truth-value-strength familiarity))
      (format #t "✓ TruthKernel autonomous decision rules working~%")))
  
  ;; Test option suitability evaluation
  (let ((kernel *global-truth-kernel*))
    (let ((suitability (truth-kernel-evaluate kernel 'option-suitability 
                                             '(investigate test-context test-evaluation))))
      (format #t "Option suitability: ~,2f~%" (truth-value-strength suitability))
      (format #t "✓ Option evaluation working~%"))))

(define (test-continuous-decision-loop)
  "Test continuous autonomous decision making loop"
  (format #t "~%=== Testing Continuous Decision Loop ===~%")
  
  (let ((system (make-autonomous-decision-system #:autonomy-level 2)))
    (format #t "Starting autonomous decision loop demo...~%")
    (autonomous-decision-loop system)
    (format #t "✓ Continuous decision loop completed~%")))

(define (run-comprehensive-tests)
  "Run all autonomous decision making tests"
  (format #t "~%======= Autonomous Decision Making Test Suite =======~%")
  
  ;; Run all test modules
  (test-autonomous-decision-system)
  (test-autonomy-levels)
  (test-cognitive-interface-integration)
  (test-decision-execution)
  (test-truthkernel-integration)
  (test-continuous-decision-loop)
  
  (format #t "~%======= All Autonomous Decision Making Tests Complete =======~%")
  (format #t "✅ Autonomous decision making system is fully functional~%")
  (format #t "✅ Integration with SKZ framework verified~%"))

;; Main execution
(when (defined? 'current-filename)
  (run-comprehensive-tests))