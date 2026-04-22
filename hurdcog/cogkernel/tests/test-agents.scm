;;; Test suite for Agents module
;;; Validates agentic task orchestration functionality

(define-module (test agents)
  #:use-module (cogkernel agents)
  #:use-module (cogkernel atomspace)
  #:use-module (ice-9 format))

;;; Test agent creation
(define (test-agent-creation)
  "Test basic agent creation functionality"
  (format #t "Testing agent creation...~%")
  (let ((test-agent (make-agent "test-agent" 'MONITOR)))
    
    (assert (agent? test-agent))
    (assert (string=? (agent-id test-agent) "test-agent"))
    (assert (eq? (agent-role test-agent) 'MONITOR))
    (assert (eq? (agent-state test-agent) 'IDLE))
    
    (format #t "✓ Agent creation tests passed~%")))

;;; Test action registration
(define (test-action-registration)
  "Test agent action registration"
  (format #t "Testing action registration...~%")
  (let ((test-agent (make-agent "action-test" 'REPAIR)))
    
    (agent-register-action! test-agent 'REPAIR
      (lambda (issue) (format #f "Repairing: ~a" issue)))
    
    (assert (not (null? (agent-actions test-agent))))
    (assert (assq 'REPAIR (agent-actions test-agent)))
    
    (format #t "✓ Action registration tests passed~%")))

;;; Test agent execution
(define (test-agent-execution)
  "Test agent action execution"
  (format #t "Testing agent execution...~%")
  (let ((test-agent (make-agent "exec-test" 'BUILD)))
    
    (agent-register-action! test-agent 'BUILD
      (lambda (target) 
        (format #f "Building ~a" target)))
    
    (let ((result (agent-execute! test-agent 'BUILD "test-target")))
      (assert (string=? result "Building test-target"))
      (assert (eq? (agent-state test-agent) 'COMPLETED)))
    
    (format #t "✓ Agent execution tests passed~%")))

;;; Test agent system
(define (test-agent-system)
  "Test agent system coordination"
  (format #t "Testing agent system...~%")
  (let ((test-system (make-agent-system))
        (agent1 (make-agent "system-test-1" 'MONITOR))
        (agent2 (make-agent "system-test-2" 'ANALYZE)))
    
    (agent-system-add! test-system agent1)
    (agent-system-add! test-system agent2)
    
    (assert (agent? (agent-system-get test-system "system-test-1")))
    (assert (agent? (agent-system-get test-system "system-test-2")))
    
    ;; Test tensor shape
    (let ((shape (agent-system-tensor-shape test-system)))
      (assert (= (car shape) 2))) ; 2 agents
    
    (format #t "✓ Agent system tests passed~%")))

;;; Test tensor shapes
(define (test-agent-tensor-shapes)
  "Test agent tensor shape tracking"
  (format #t "Testing agent tensor shapes...~%")
  (let ((test-system (make-agent-system '(5 4 8 3))))
    (assert (equal? (agent-system-tensor-shape test-system) '(0 4 8 3)))
    
    ;; Add agents and check shape
    (agent-system-add! test-system (make-agent "shape-1" 'MONITOR))
    (agent-system-add! test-system (make-agent "shape-2" 'REPAIR))
    (assert (= (car (agent-system-tensor-shape test-system)) 2))
    
    (format #t "✓ Agent tensor shape tests passed~%")))

;;; Run all tests
(define (run-agents-tests)
  "Run all agent tests"
  (format #t "=== Agents Test Suite ===~%")
  (test-agent-creation)
  (test-action-registration)
  (test-agent-execution)
  (test-agent-system)
  (test-agent-tensor-shapes)
  (format #t "=== All Agents tests passed! ===~%"))

;; Run tests when module is loaded
(run-agents-tests)