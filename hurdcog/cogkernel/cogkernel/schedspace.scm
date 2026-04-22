;;; SchedSpace - Enhanced ECAN Attention-Based Scheduling for HurdCog Core Services
;;; Implements cognitive attention allocation for system-wide resource management
;;; Part of Phase 2: Core Services implementation

(define-module (cogkernel schedspace)
  #:use-module (ice-9 format)
  #:use-module (ice-9 match)
  #:use-module (ice-9 hash-table)
  #:use-module (ice-9 threads)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (cogkernel atomspace)
  #:use-module (cogkernel attention)
  #:use-module (cogkernel cognitive-grip)
  #:export (make-sched-space
            sched-space?
            sched-space-schedule!
            sched-space-add-task!
            sched-space-allocate-attention!
            sched-space-get-focus
            sched-space-priority-inversion
            make-cognitive-task
            cognitive-task?
            cognitive-task-priority
            cognitive-task-attention
            *global-sched-space*
            bootstrap-sched-space!))

;;; Cognitive task representation
(define-record-type <cognitive-task>
  (make-cognitive-task-record id type priority attention-value 
                             resource-needs execution-time state metadata)
  cognitive-task?
  (id cognitive-task-id)
  (type cognitive-task-type set-cognitive-task-type!)
  (priority cognitive-task-priority set-cognitive-task-priority!)
  (attention-value cognitive-task-attention set-cognitive-task-attention!)
  (resource-needs cognitive-task-resource-needs set-cognitive-task-resource-needs!)
  (execution-time cognitive-task-execution-time set-cognitive-task-execution-time!)
  (state cognitive-task-state set-cognitive-task-state!)
  (metadata cognitive-task-metadata set-cognitive-task-metadata!))

;;; Task states
(define task-states '(PENDING RUNNING BLOCKED COMPLETED FAILED ATTENTION-STARVED))

;;; Create a new cognitive task
(define* (make-cognitive-task id type #:key (priority 100) (attention-value (make-attention-value 100 50 25))
                             (resource-needs '()) (execution-time 1.0) (state 'PENDING) (metadata '()))
  "Create a new cognitive task for attention-based scheduling"
  (make-cognitive-task-record id type priority attention-value resource-needs execution-time state metadata))

;;; SchedSpace record for attention-based scheduling
(define-record-type <sched-space>
  (make-sched-space-record task-queue attention-bank focus-set scheduler-state
                          cognitive-cycles attention-funds priority-threshold mutex)
  sched-space?
  (task-queue sched-space-task-queue)
  (attention-bank sched-space-attention-bank)
  (focus-set sched-space-focus-set set-sched-space-focus-set!)
  (scheduler-state sched-space-scheduler-state)
  (cognitive-cycles sched-space-cognitive-cycles set-sched-space-cognitive-cycles!)
  (attention-funds sched-space-attention-funds set-sched-space-attention-funds!)
  (priority-threshold sched-space-priority-threshold set-sched-space-priority-threshold!)
  (mutex sched-space-mutex))

;;; Create new SchedSpace
(define* (make-sched-space #:key (attention-funds 10000) (priority-threshold 150))
  "Create a new SchedSpace for attention-based scheduling"
  (make-sched-space-record 
    (make-hash-table)     ; Task queue
    (make-attention-bank attention-funds priority-threshold) ; Attention bank
    '()                   ; Focus set (currently attended tasks)
    (make-hash-table)     ; Scheduler state
    0                     ; Cognitive cycles
    attention-funds       ; Available attention funds
    priority-threshold    ; Priority threshold for attention
    (make-mutex)))        ; Thread safety mutex

;;; Global SchedSpace instance
(define *global-sched-space* (make-sched-space))

;;; Task types with different attention requirements
(define task-attention-profiles
  `((SYSTEM-CRITICAL     . ,(make-attention-value 300 200 100))  ; Highest priority
    (MEMORY-MANAGEMENT   . ,(make-attention-value 250 150 75))   ; High priority
    (TRANSLATOR-OPERATION . ,(make-attention-value 200 100 50))  ; Medium-high priority
    (SERVER-MAINTENANCE  . ,(make-attention-value 150 75 25))    ; Medium priority
    (BUILD-PROCESS      . ,(make-attention-value 100 50 10))     ; Lower priority
    (LOGGING            . ,(make-attention-value 50 25 5))      ; Low priority
    (BACKGROUND-CLEANUP . ,(make-attention-value 25 10 1))))    ; Lowest priority

;;; Add task to SchedSpace
(define (sched-space-add-task! schedspace task)
  "Add a cognitive task to the scheduling space"
  (with-mutex (sched-space-mutex schedspace)
    (let ((task-queue (sched-space-task-queue schedspace))
          (task-id (cognitive-task-id task)))
      
      ;; Set attention value based on task type if not explicitly set
      (when (equal? (cognitive-task-attention task) (make-attention-value 100 50 25))
        (let ((profile (assoc-ref task-attention-profiles (cognitive-task-type task))))
          (when profile
            (set-cognitive-task-attention! task profile))))
      
      ;; Add to task queue
      (hash-set! task-queue task-id task)
      
      ;; Update attention bank
      (attention-bank-add! (sched-space-attention-bank schedspace) 
                          (make-atom 'TASK (symbol->string task-id))
                          (cognitive-task-attention task))
      
      (format #t "Task added: ~a (~a) - STI=~a~%" 
              task-id (cognitive-task-type task)
              (attention-value-sti (cognitive-task-attention task))))))

;;; Allocate attention to tasks based on cognitive economics
(define (sched-space-allocate-attention! schedspace)
  "Allocate attention funds to tasks using cognitive economics"
  (with-mutex (sched-space-mutex schedspace)
    (let ((task-queue (sched-space-task-queue schedspace))
          (attention-bank (sched-space-attention-bank schedspace))
          (available-funds (sched-space-attention-funds schedspace))
          (allocated-tasks '()))
      
      (format #t "=== Attention Allocation Cycle ===~%")
      (format #t "Available attention funds: ~a~%" available-funds)
      
      ;; Get all tasks sorted by attention value
      (let ((all-tasks (hash-map->list (lambda (id task) task) task-queue)))
        (let ((sorted-tasks (sort all-tasks 
                                 (lambda (a b) 
                                   (> (attention-value-sti (cognitive-task-attention a))
                                      (attention-value-sti (cognitive-task-attention b)))))))
          
          ;; Allocate attention to highest priority tasks first
          (let ((remaining-funds available-funds))
            (for-each
              (lambda (task)
                (let* ((task-attention (cognitive-task-attention task))
                       (required-attention (attention-value-sti task-attention))
                       (task-cost (max 10 (floor (/ required-attention 10)))))
                  
                  (if (>= remaining-funds task-cost)
                      (begin
                        ;; Allocate attention
                        (set! remaining-funds (- remaining-funds task-cost))
                        (set! allocated-tasks (cons task allocated-tasks))
                        (set-cognitive-task-state! task 'RUNNING)
                        (format #t "  Allocated ~a attention to ~a~%" 
                               task-cost (cognitive-task-id task)))
                      ;; Not enough attention - starve task
                      (begin
                        (set-cognitive-task-state! task 'ATTENTION-STARVED)
                        (format #t "  ~a attention-starved (needs ~a, available ~a)~%" 
                               (cognitive-task-id task) task-cost remaining-funds)))))
              sorted-tasks)
            
            ;; Update remaining funds
            (set-sched-space-attention-funds! schedspace remaining-funds)
            
            ;; Update focus set with allocated tasks
            (set-sched-space-focus-set! schedspace allocated-tasks)
            
            (format #t "Attention allocated to ~a tasks, ~a funds remaining~%" 
                   (length allocated-tasks) remaining-funds)
            
            allocated-tasks))))))

;;; Get current focus set (tasks receiving attention)
(define (sched-space-get-focus schedspace)
  "Get the current set of tasks receiving attention"
  (sched-space-focus-set schedspace))

;;; Handle priority inversion in attention allocation
(define (sched-space-priority-inversion schedspace high-priority-task blocking-task)
  "Handle priority inversion by temporarily boosting blocking task attention"
  (format #t "=== Priority Inversion Detected ===~%")
  (format #t "High priority task ~a blocked by ~a~%" 
          (cognitive-task-id high-priority-task)
          (cognitive-task-id blocking-task))
  
  ;; Temporarily boost blocking task attention
  (let* ((blocking-attention (cognitive-task-attention blocking-task))
         (high-attention (cognitive-task-attention high-priority-task))
         (boosted-sti (max (attention-value-sti blocking-attention)
                          (attention-value-sti high-attention))))
    
    (set-cognitive-task-attention! blocking-task 
                                  (make-attention-value boosted-sti
                                                       (attention-value-lti blocking-attention)
                                                       (attention-value-vlti blocking-attention)))
    
    (format #t "Boosted ~a attention: STI ~a → ~a~%" 
           (cognitive-task-id blocking-task)
           (attention-value-sti blocking-attention)
           boosted-sti)
    
    ;; Re-allocate attention
    (sched-space-allocate-attention! schedspace)))

;;; Main scheduling function
(define (sched-space-schedule! schedspace)
  "Execute one cognitive scheduling cycle"
  (with-mutex (sched-space-mutex schedspace)
    (let ((cycle-number (+ (sched-space-cognitive-cycles schedspace) 1)))
      (format #t "~%=== SchedSpace Cognitive Cycle ~a ===~%" cycle-number)
      
      ;; Allocate attention to tasks
      (let ((focused-tasks (sched-space-allocate-attention! schedspace)))
        
        ;; Simulate task execution for focused tasks
        (for-each
          (lambda (task)
            (when (eq? (cognitive-task-state task) 'RUNNING)
              (format #t "Executing: ~a (~a)~%" 
                     (cognitive-task-id task) (cognitive-task-type task))
              
              ;; Simulate execution time
              (let ((execution-time (cognitive-task-execution-time task)))
                (if (< execution-time 0.5)
                    ;; Task completes
                    (begin
                      (set-cognitive-task-state! task 'COMPLETED)
                      (format #t "  Task ~a completed~%" (cognitive-task-id task)))
                    ;; Task continues
                    (set-cognitive-task-execution-time! task (- execution-time 0.5))))))
          focused-tasks)
        
        ;; Update cognitive cycle counter
        (set-sched-space-cognitive-cycles! schedspace cycle-number)
        
        ;; Replenish some attention funds (cognitive recovery)
        (let ((current-funds (sched-space-attention-funds schedspace))
              (replenishment (min 500 (floor (/ 10000 10)))))  ; 10% replenishment
          (set-sched-space-attention-funds! schedspace (+ current-funds replenishment))
          (format #t "Attention replenished: +~a (total: ~a)~%" 
                 replenishment (+ current-funds replenishment)))
        
        ;; Remove completed tasks
        (let ((task-queue (sched-space-task-queue schedspace)))
          (hash-for-each-handle
            (lambda (handle)
              (let ((task (cdr handle)))
                (when (eq? (cognitive-task-state task) 'COMPLETED)
                  (hash-remove! task-queue (cognitive-task-id task))
                  (format #t "Removed completed task: ~a~%" (cognitive-task-id task)))))
            task-queue))
        
        (format #t "Cycle ~a complete. Active tasks: ~a~%" 
               cycle-number 
               (hash-count (const #t) (sched-space-task-queue schedspace)))))))

;;; Bootstrap SchedSpace with initial tasks
(define (bootstrap-sched-space! schedspace)
  "Bootstrap SchedSpace with initial system tasks"
  (format #t "=== Bootstrapping SchedSpace ===~%")
  
  ;; Add core system tasks
  (let ((core-tasks
         (list
           (make-cognitive-task 'memory-monitor 'MEMORY-MANAGEMENT 
                               #:execution-time 2.0 #:priority 250)
           (make-cognitive-task 'translator-health 'TRANSLATOR-OPERATION 
                               #:execution-time 1.5 #:priority 200)
           (make-cognitive-task 'auth-server-check 'SERVER-MAINTENANCE 
                               #:execution-time 1.0 #:priority 150)
           (make-cognitive-task 'build-coordination 'BUILD-PROCESS 
                               #:execution-time 3.0 #:priority 100)
           (make-cognitive-task 'system-logging 'LOGGING 
                               #:execution-time 0.5 #:priority 50)
           (make-cognitive-task 'cleanup-temp-files 'BACKGROUND-CLEANUP 
                               #:execution-time 0.8 #:priority 25))))
    
    (for-each (lambda (task) (sched-space-add-task! schedspace task)) core-tasks)
    
    (format #t "Bootstrap complete: ~a initial tasks added~%" (length core-tasks))))

;;; Add a critical emergency task
(define (sched-space-add-emergency! schedspace task-id description)
  "Add an emergency task with maximum attention priority"
  (let ((emergency-task (make-cognitive-task task-id 'SYSTEM-CRITICAL
                                           #:execution-time 0.5
                                           #:priority 500
                                           #:attention-value (make-attention-value 500 300 150)
                                           #:metadata (list (cons 'description description)
                                                          (cons 'emergency #t)))))
    (sched-space-add-task! schedspace emergency-task)
    (format #t "🚨 Emergency task added: ~a - ~a~%" task-id description)
    emergency-task))

;;; Test SchedSpace functionality
(define (test-sched-space)
  "Test SchedSpace attention-based scheduling"
  (format #t "~%=== Testing SchedSpace Core Services ===~%")
  
  (let ((schedspace *global-sched-space*))
    ;; Bootstrap the scheduler
    (bootstrap-sched-space! schedspace)
    
    ;; Run several scheduling cycles
    (format #t "~%--- Testing Attention-Based Scheduling ---~%")
    (dotimes (i 3)
      (sched-space-schedule! schedspace))
    
    ;; Test emergency task handling
    (format #t "~%--- Testing Emergency Task Handling ---~%")
    (sched-space-add-emergency! schedspace 'memory-leak-critical "Critical memory leak detected")
    (sched-space-schedule! schedspace)
    
    ;; Test priority inversion handling
    (format #t "~%--- Testing Priority Inversion ---~%")
    (let ((high-task (make-cognitive-task 'critical-operation 'SYSTEM-CRITICAL))
          (blocking-task (make-cognitive-task 'blocking-operation 'BACKGROUND-CLEANUP)))
      (sched-space-add-task! schedspace high-task)
      (sched-space-add-task! schedspace blocking-task)
      (sched-space-priority-inversion schedspace high-task blocking-task))
    
    (format #t "~%✅ SchedSpace testing complete~%")))

;;; Helper function for dotimes (if not defined elsewhere)
(define (dotimes count proc)
  "Execute procedure count times"
  (let loop ((i 0))
    (when (< i count)
      (proc i)
      (loop (+ i 1)))))

;;; Initialize SchedSpace when module loads
(bootstrap-sched-space! *global-sched-space*)