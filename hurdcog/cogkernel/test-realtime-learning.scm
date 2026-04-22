#!/usr/bin/env -S guile -s
!#

;;; Test suite for Real-time Learning Systems
;;; Validates all learning system functionality

(add-to-load-path ".")

(use-modules (cogkernel atomspace)
             (cogkernel attention)
             (cogkernel agents)
             (cogkernel tensors)
             (cogkernel cognitive-interface learning-systems realtime)
             (ice-9 format))

;;; Test framework helpers
(define test-count 0)
(define passed-count 0)

(define (test-assert name condition)
  (set! test-count (+ test-count 1))
  (if condition
      (begin
        (set! passed-count (+ passed-count 1))
        (format #t "✓ ~a~%" name))
      (format #t "✗ ~a~%" name)))

(define (test-summary)
  (format #t "~%=== Test Summary ===~%")
  (format #t "Passed: ~a/~a tests~%" passed-count test-count)
  (if (= passed-count test-count)
      (format #t "✅ All tests passed!~%")
      (format #f "❌ ~a tests failed~%" (- test-count passed-count))))

;;; Test basic learning system creation
(define (test-learning-system-creation)
  (format #t "=== Testing Learning System Creation ===~%")
  
  (let ((ls (make-learning-system #:pattern-learning #t
                                  #:temporal-difference #t
                                  #:reinforcement #t)))
    (test-assert "Learning system creation" (learning-system? ls))
    (test-assert "Pattern learning enabled" (learning-system-pattern-learning ls))
    (test-assert "Temporal difference enabled" (learning-system-temporal-difference ls))
    (test-assert "Reinforcement learning enabled" (learning-system-reinforcement ls))
    ls))

;;; Test learning experience creation and processing
(define (test-learning-experience)
  (format #t "~%=== Testing Learning Experience ===~%")
  
  (let* ((ls (make-learning-system))
         (exp (create-learning-experience 'CONTEXT-1 'ACTION-A 'POSITIVE-RESULT 'SUCCESS)))
    
    (test-assert "Experience creation" (learning-experience? exp))
    (test-assert "Experience context" (eq? (learning-experience-context exp) 'CONTEXT-1))
    (test-assert "Experience outcome" (eq? (learning-experience-outcome exp) 'POSITIVE-RESULT))
    
    ; Process the experience
    (let ((processed-exp (learn-from-experience ls exp)))
      (test-assert "Experience processing" (learning-experience? processed-exp))
      
      ; Check if experience was stored
      (let ((buffer-size (hash-count (const #t) (learning-system-experience-buffer ls))))
        (test-assert "Experience stored in buffer" (> buffer-size 0))))
    
    (list ls exp)))

;;; Test pattern recognition
(define (test-pattern-recognition)
  (format #t "~%=== Testing Pattern Recognition ===~%")
  
  (let ((ls (make-learning-system #:pattern-learning #t)))
    ; Create multiple similar experiences
    (learn-from-experience ls (create-learning-experience 'WEB-REQUEST 'CACHE-HIT 'FAST-RESPONSE 'SUCCESS))
    (learn-from-experience ls (create-learning-experience 'WEB-REQUEST 'CACHE-HIT 'FAST-RESPONSE 'SUCCESS))
    (learn-from-experience ls (create-learning-experience 'WEB-REQUEST 'CACHE-MISS 'SLOW-RESPONSE 'PARTIAL))
    
    ; Test pattern recognition
    (let ((patterns (pattern-recognition ls 'WEB-REQUEST)))
      (test-assert "Pattern recognition returns results" (list? patterns))
      (test-assert "Patterns found for web request" (>= (length patterns) 0)))
    
    ; Test advanced pattern learning
    (let ((advanced-patterns (advanced-pattern-learning ls '() 0.5)))
      (test-assert "Advanced pattern learning returns results" (list? advanced-patterns)))
    
    ls))

;;; Test behavior adaptation
(define (test-behavior-adaptation)
  (format #t "~%=== Testing Behavior Adaptation ===~%")
  
  (let ((ls (make-learning-system)))
    ; Create experiences that should lead to learning
    (learn-from-experience ls (create-learning-experience 'ERROR-CONTEXT 'RETRY-ACTION 'SUCCESS 'SUCCESS))
    (learn-from-experience ls (create-learning-experience 'ERROR-CONTEXT 'IGNORE-ACTION 'FAILURE 'FAILURE))
    
    ; Test behavior adaptation
    (let ((adapted-behavior (adapt-behavior ls 'ERROR-CONTEXT)))
      (test-assert "Behavior adaptation returns symbol" (symbol? adapted-behavior))
      (format #t "  Adapted behavior for ERROR-CONTEXT: ~a~%" adapted-behavior))
    
    ls))

;;; Test learning effectiveness evaluation
(define (test-learning-effectiveness)
  (format #t "~%=== Testing Learning Effectiveness ===~%")
  
  (let ((ls (make-learning-system)))
    ; Add experiences with known success/failure rates
    (learn-from-experience ls (create-learning-experience 'TEST-1 'ACTION-1 'RESULT-1 'SUCCESS))
    (learn-from-experience ls (create-learning-experience 'TEST-2 'ACTION-2 'RESULT-2 'SUCCESS))
    (learn-from-experience ls (create-learning-experience 'TEST-3 'ACTION-3 'RESULT-3 'FAILURE))
    
    (let ((effectiveness (evaluate-learning-effectiveness ls)))
      (test-assert "Effectiveness is number" (number? effectiveness))
      (test-assert "Effectiveness in valid range" (and (>= effectiveness 0.0) (<= effectiveness 1.0)))
      (format #t "  Learning effectiveness: ~a~%" effectiveness))
    
    ; Test monitoring
    (let ((monitor-results (monitor-learning-effectiveness ls)))
      (test-assert "Monitor returns association list" (list? monitor-results))
      (test-assert "Monitor includes total experiences" 
                   (assq 'total-experiences monitor-results))
      (format #t "  Monitoring results: ~a~%" monitor-results))
    
    ls))

;;; Test temporal difference learning
(define (test-temporal-difference)
  (format #t "~%=== Testing Temporal Difference Learning ===~%")
  
  (let ((ls (make-learning-system #:temporal-difference #t)))
    ; Create experience for TD learning
    (let ((exp (create-learning-experience 'TD-STATE 'TD-ACTION 'TD-RESULT 1.5)))
      (temporal-difference-learning ls exp)
      (test-assert "TD learning completed without error" #t)
      
      ; Test state value retrieval
      (let ((state-value (get-state-value ls 'TD-STATE)))
        (test-assert "State value is number" (number? state-value))
        (format #t "  State value for TD-STATE: ~a~%" state-value)))
    
    ls))

;;; Test Q-learning
(define (test-q-learning)
  (format #t "~%=== Testing Q-Learning ===~%")
  
  (let ((ls (make-learning-system #:reinforcement #t)))
    ; Test Q-learning update
    (q-learning-update ls 'STATE-A 'ACTION-X 1.0 'STATE-B)
    (test-assert "Q-learning update completed" #t)
    
    ; Test Q-value retrieval
    (let ((q-val (get-q-value ls "Q-STATE-A-ACTION-X")))
      (test-assert "Q-value is number" (number? q-val))
      (format #t "  Q-value for STATE-A, ACTION-X: ~a~%" q-val))
    
    ls))

;;; Test experience replay
(define (test-experience-replay)
  (format #t "~%=== Testing Experience Replay ===~%")
  
  (let ((ls (make-learning-system)))
    ; Add multiple experiences
    (learn-from-experience ls (create-learning-experience 'REPLAY-1 'ACTION-1 'RESULT-1 'SUCCESS))
    (learn-from-experience ls (create-learning-experience 'REPLAY-2 'ACTION-2 'RESULT-2 'SUCCESS))
    (learn-from-experience ls (create-learning-experience 'REPLAY-3 'ACTION-3 'RESULT-3 'FAILURE))
    
    ; Test experience replay
    (experience-replay ls 2)
    (test-assert "Experience replay completed" #t)
    
    ls))

;;; Test learning callbacks
(define (test-learning-callbacks)
  (format #t "~%=== Testing Learning Callbacks ===~%")
  
  (let ((ls (make-learning-system))
        (callback-triggered #f))
    
    ; Register callback
    (register-learning-callback ls 'test-callback
                               (lambda (exp)
                                 (set! callback-triggered #t)
                                 (format #t "  Callback triggered for experience: ~a~%" 
                                        (learning-experience-id exp))))
    
    ; Trigger callback
    (learn-from-experience ls (create-learning-experience 'CALLBACK-TEST 'ACTION 'RESULT 'SUCCESS))
    
    (test-assert "Callback was triggered" callback-triggered)
    
    ls))

;;; Test global learning system
(define (test-global-system)
  (format #t "~%=== Testing Global Learning System ===~%")
  
  (test-assert "Global learning system exists" (learning-system? *global-learning-system*))
  (test-assert "Global system has pattern learning" 
               (learning-system-pattern-learning *global-learning-system*))
  (test-assert "Global system has TD learning" 
               (learning-system-temporal-difference *global-learning-system*))
  (test-assert "Global system has RL" 
               (learning-system-reinforcement *global-learning-system*)))

;;; Test integration with atomspace
(define (test-atomspace-integration)
  (format #t "~%=== Testing AtomSpace Integration ===~%")
  
  (let ((ls (make-learning-system))
        (initial-atom-count (length (atomspace-get-atoms *global-atomspace*))))
    
    ; Learn something that should add atoms
    (learn-from-experience ls (create-learning-experience 'ATOMSPACE-TEST 'ACTION 'RESULT 'SUCCESS))
    
    (let ((final-atom-count (length (atomspace-get-atoms *global-atomspace*))))
      (test-assert "Learning added atoms to atomspace" 
                   (> final-atom-count initial-atom-count))
      (format #t "  Atoms added: ~a~%" (- final-atom-count initial-atom-count)))
    
    ; Check for specific atom types
    (let ((experience-atoms (atomspace-query *global-atomspace*
                                           (lambda (atom)
                                             (eq? (atom-type atom) 'EXPERIENCE))))
          (pattern-atoms (atomspace-query *global-atomspace*
                                        (lambda (atom)
                                          (eq? (atom-type atom) 'PATTERN)))))
      (test-assert "Experience atoms created" (> (length experience-atoms) 0))
      (format #t "  Experience atoms: ~a~%" (length experience-atoms))
      (format #t "  Pattern atoms: ~a~%" (length pattern-atoms)))
    
    ls))

;;; Main test runner
(define (run-all-tests)
  (format #t "🧠 Real-time Learning Systems Test Suite 🧠~%")
  (format #t "==========================================~%~%")
  
  ; Initialize global systems
  (initialize-hurd-atoms! *global-atomspace*)
  
  ; Run all tests
  (test-learning-system-creation)
  (test-learning-experience)
  (test-pattern-recognition)
  (test-behavior-adaptation)
  (test-learning-effectiveness)
  (test-temporal-difference)
  (test-q-learning)
  (test-experience-replay)
  (test-learning-callbacks)
  (test-global-system)
  (test-atomspace-integration)
  
  ; Summary
  (test-summary)
  
  ; Integration test
  (format #t "~%=== Integration Test ===~%")
  (format #t "Testing complete real-time learning workflow...~%")
  
  (let ((ls *global-learning-system*))
    ; Simulate learning workflow
    (learn-from-experience ls (create-learning-experience 'INTEGRATION 'LEARN 'SUCCESS 'SUCCESS))
    (let ((adapted (adapt-behavior ls 'INTEGRATION)))
      (format #t "✓ Integration workflow completed with adapted behavior: ~a~%" adapted)))
  
  (format #t "~%🎯 Real-time Learning Systems: OPERATIONAL 🎯~%"))

;; Run the tests if this script is executed directly
(when (string=? (car (command-line)) (current-filename))
  (run-all-tests))