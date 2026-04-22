;;;
;;; attention-moses-bridge.scm
;;;
;;; Scheme interface for Attention-Guided Evolutionary Learning
;;;
;;; This module provides Scheme bindings for the Python AttentionMOSESBridge,
;;; enabling integration with AtomSpace-based cognitive architectures.
;;;
;;; Emergent Capability: focused_learning
;;;
;;; Usage:
;;;   (use-modules (opencog) (opencog attention) (opencog exec))
;;;   (load "attention-moses-bridge.scm")
;;;   
;;;   ; Extract high-STI atoms
;;;   (define important-atoms (get-high-sti-atoms 100))
;;;   
;;;   ; Create learning task
;;;   (create-attention-guided-task "task-1" important-atoms)
;;;

(use-modules (opencog))
(use-modules (opencog exec))
(use-modules (ice-9 format))

;;; ============================================================================
;;; Configuration Parameters
;;; ============================================================================

(define *sti-threshold* 100.0)
(define *lti-threshold* 50.0)
(define *fitness-scaling* 1.0)
(define *max-concurrent-tasks* 10)
(define *max-importance* 1000.0)
(define *feedback-lti-delta* 10.0)
(define *feedback-sti-boost* 50.0)

;;; ============================================================================
;;; Attention Signal Extraction
;;; ============================================================================

(define (get-attention-value atom)
  "Get the AttentionValue of an atom as an association list"
  (let ((av (cog-av atom)))
    (if (null? av)
        '((sti . 0) (lti . 0) (vlti . 0))
        (list
          (cons 'sti (assoc-ref av 'sti))
          (cons 'lti (assoc-ref av 'lti))
          (cons 'vlti (assoc-ref av 'vlti))))))

(define (get-sti atom)
  "Get Short-Term Importance of atom"
  (assoc-ref (get-attention-value atom) 'sti))

(define (get-lti atom)
  "Get Long-Term Importance of atom"
  (assoc-ref (get-attention-value atom) 'lti))

(define (atom-exceeds-sti-threshold? atom threshold)
  "Check if atom's STI exceeds threshold"
  (> (get-sti atom) threshold))

(define (atom-exceeds-lti-threshold? atom threshold)
  "Check if atom's LTI exceeds threshold"
  (> (get-lti atom) threshold))

(define (get-high-sti-atoms threshold)
  "Get all atoms with STI above threshold"
  (filter
    (lambda (atom) (atom-exceeds-sti-threshold? atom threshold))
    (cog-get-atoms 'Atom #t)))

(define (get-high-lti-atoms threshold)
  "Get all atoms with LTI above threshold"
  (filter
    (lambda (atom) (atom-exceeds-lti-threshold? atom threshold))
    (cog-get-atoms 'Atom #t)))

(define (get-attentional-focus)
  "Get atoms currently in attentional focus (high STI)"
  (get-high-sti-atoms *sti-threshold*))

;;; ============================================================================
;;; Attention Signal Structure
;;; ============================================================================

(define (make-attention-signal atom signal-type)
  "Create an attention signal structure for an atom"
  (list
    (cons 'atom atom)
    (cons 'atom-id (cog-name atom))
    (cons 'atom-type (cog-type atom))
    (cons 'sti (get-sti atom))
    (cons 'lti (get-lti atom))
    (cons 'signal-type signal-type)
    (cons 'timestamp (current-time))))

(define (extract-attention-signals atoms signal-type)
  "Extract attention signals from a list of atoms"
  (map
    (lambda (atom) (make-attention-signal atom signal-type))
    atoms))

;;; ============================================================================
;;; Fitness Bonus Calculation
;;; ============================================================================

(define (normalize-importance value max-value)
  "Normalize importance value to 0-1 range"
  (min (/ value max-value) 1.0))

(define (calculate-fitness-bonus signal)
  "Calculate MOSES fitness bonus from attention signal"
  (let* ((sti (assoc-ref signal 'sti))
         (lti (assoc-ref signal 'lti))
         (sti-weight 0.7)
         (lti-weight 0.3)
         (norm-sti (normalize-importance sti *max-importance*))
         (norm-lti (normalize-importance lti *max-importance*))
         (importance (+ (* sti-weight norm-sti) (* lti-weight norm-lti)))
         (bonus (* importance *fitness-scaling*)))
    bonus))

(define (signal-priority signal)
  "Get priority of an attention signal"
  (calculate-fitness-bonus signal))

;;; ============================================================================
;;; Learning Task Management
;;; ============================================================================

(define *learning-tasks* '())
(define *task-counter* 0)

(define (generate-task-id)
  "Generate a unique task ID"
  (set! *task-counter* (+ *task-counter* 1))
  (string-append "task-" (number->string *task-counter*)))

(define (make-learning-task task-id description signals base-fitness)
  "Create a learning task structure"
  (let* ((target-atoms (map (lambda (s) (assoc-ref s 'atom)) signals))
         (attention-bonus (if (null? signals)
                             0.0
                             (/ (apply + (map calculate-fitness-bonus signals))
                                (length signals))))
         (priority (+ base-fitness attention-bonus)))
    (list
      (cons 'task-id task-id)
      (cons 'description description)
      (cons 'target-atoms target-atoms)
      (cons 'signals signals)
      (cons 'base-fitness base-fitness)
      (cons 'attention-bonus attention-bonus)
      (cons 'priority priority)
      (cons 'status 'pending)
      (cons 'results '()))))

(define (create-attention-guided-task description atoms)
  "Create a learning task guided by attention on specified atoms"
  (let* ((signals (extract-attention-signals atoms 'attentional-focus))
         (task-id (generate-task-id))
         (task (make-learning-task task-id description signals 0.5)))
    (set! *learning-tasks* (cons task *learning-tasks*))
    (format #t "Created learning task: ~a (priority: ~,4f)~%"
            task-id (assoc-ref task 'priority))
    task))

(define (create-high-sti-learning-task description)
  "Create a learning task for high-STI atoms"
  (let ((important-atoms (get-high-sti-atoms *sti-threshold*)))
    (if (null? important-atoms)
        (begin
          (display "No atoms exceed STI threshold\n")
          '())
        (create-attention-guided-task description important-atoms))))

(define (get-task-by-id task-id)
  "Find task by ID"
  (find (lambda (task) (equal? (assoc-ref task 'task-id) task-id))
        *learning-tasks*))

(define (update-task-status! task-id new-status)
  "Update the status of a learning task"
  (let ((task (get-task-by-id task-id)))
    (if task
        (begin
          (set-cdr! (assoc 'status task) new-status)
          (format #t "Task ~a status: ~a~%" task-id new-status))
        (format #t "Task ~a not found~%" task-id))))

(define (get-prioritized-tasks limit)
  "Get learning tasks sorted by priority"
  (let ((sorted-tasks (sort *learning-tasks*
                           (lambda (a b)
                             (> (assoc-ref a 'priority)
                                (assoc-ref b 'priority))))))
    (if (> limit (length sorted-tasks))
        sorted-tasks
        (list-head sorted-tasks limit))))

;;; ============================================================================
;;; Feedback Loop to Attention System
;;; ============================================================================

(define (apply-sti-delta atom delta)
  "Apply a delta to atom's STI"
  (let ((current-sti (get-sti atom)))
    (cog-set-av! atom (cog-new-av (+ current-sti delta)
                                   (get-lti atom)
                                   0))))

(define (apply-lti-delta atom delta)
  "Apply a delta to atom's LTI"
  (let ((current-lti (get-lti atom)))
    (cog-set-av! atom (cog-new-av (get-sti atom)
                                   (+ current-lti delta)
                                   0))))

(define (feedback-successful-learning task-id patterns-learned accuracy)
  "Feed successful learning back to attention system"
  (let ((task (get-task-by-id task-id)))
    (if (not task)
        (format #t "Task ~a not found for feedback~%" task-id)
        (let ((target-atoms (assoc-ref task 'target-atoms)))
          ; Increase LTI of successfully learned atoms
          (when (> accuracy 0.7)
            (for-each
              (lambda (atom)
                (apply-lti-delta atom *feedback-lti-delta*)
                (format #t "Increased LTI of ~a by ~a~%" (cog-name atom) *feedback-lti-delta*))
              target-atoms))
          
          ; Boost STI of newly discovered patterns
          (for-each
            (lambda (pattern)
              (apply-sti-delta pattern *feedback-sti-boost*)
              (format #t "Boosted STI of pattern ~a by ~a~%" (cog-name pattern) *feedback-sti-boost*))
            patterns-learned)
          
          (format #t "Feedback applied for task ~a~%" task-id)))))

;;; ============================================================================
;;; Demonstration and Testing
;;; ============================================================================

(define (demo-attention-guided-learning)
  "Demonstrate attention-guided evolutionary learning"
  (display "============================================================\n")
  (display "ATTENTION-GUIDED EVOLUTIONARY LEARNING DEMO (Scheme)\n")
  (display "============================================================\n\n")
  
  ; Create some demo atoms with varying importance
  (display "1. Creating demo atoms with attention values...\n")
  (define concept1 (ConceptNode "important-concept-1"))
  (define concept2 (ConceptNode "important-concept-2"))
  (define concept3 (ConceptNode "less-important"))
  
  (cog-set-av! concept1 (cog-new-av 150 60 0))
  (cog-set-av! concept2 (cog-new-av 120 45 0))
  (cog-set-av! concept3 (cog-new-av 30 20 0))
  
  (format #t "   Created 3 atoms with STI: ~a, ~a, ~a~%"
          (get-sti concept1) (get-sti concept2) (get-sti concept3))
  
  ; Extract high-STI atoms
  (display "\n2. Extracting high-STI atoms...\n")
  (define important-atoms (get-high-sti-atoms 100))
  (format #t "   Found ~a atoms exceeding threshold~%" (length important-atoms))
  
  ; Create learning tasks
  (display "\n3. Creating attention-guided learning tasks...\n")
  (define task1 (create-attention-guided-task
                  "Learn patterns in important concepts"
                  important-atoms))
  
  (define task2 (create-high-sti-learning-task
                  "Learn relationships between high-STI atoms"))
  
  ; Display prioritized tasks
  (display "\n4. Prioritized learning tasks:\n")
  (for-each
    (lambda (task)
      (format #t "   - ~a: ~a (priority: ~,4f)~%"
              (assoc-ref task 'task-id)
              (assoc-ref task 'description)
              (assoc-ref task 'priority)))
    (get-prioritized-tasks 3))
  
  ; Simulate learning and feedback
  (display "\n5. Simulating learning and feedback...\n")
  (let ((top-task (car (get-prioritized-tasks 1))))
    (update-task-status! (assoc-ref top-task 'task-id) 'running)
    
    ; Simulate discovering a new pattern
    (define new-pattern (PredicateNode "learned-pattern-1"))
    
    ; Provide feedback
    (feedback-successful-learning
      (assoc-ref top-task 'task-id)
      (list new-pattern)
      0.85)
    
    (update-task-status! (assoc-ref top-task 'task-id) 'completed))
  
  ; Show final attention values
  (display "\n6. Final attention values after feedback:\n")
  (for-each
    (lambda (atom)
      (format #t "   ~a: STI=~,2f, LTI=~,2f~%"
              (cog-name atom) (get-sti atom) (get-lti atom)))
    important-atoms)
  
  (display "\n============================================================\n")
  (display "Demo completed! Emergent capability: focused_learning ✓\n")
  (display "============================================================\n"))

;;; ============================================================================
;;; Public API
;;; ============================================================================

; Export key functions
(export
  get-high-sti-atoms
  get-high-lti-atoms
  get-attentional-focus
  create-attention-guided-task
  create-high-sti-learning-task
  get-prioritized-tasks
  update-task-status!
  feedback-successful-learning
  demo-attention-guided-learning)

(display "Attention-MOSES Bridge loaded successfully.\n")
(display "Run (demo-attention-guided-learning) to see it in action!\n")
