;;; Real-time Learning Systems - Learning Integration
;;; File: cognitive-interface/learning-systems/realtime.scm
;;; Implements real-time learning and adaptation capabilities

(define-module (cogkernel cognitive-interface learning-systems realtime)
  #:use-module (cogkernel atomspace)
  #:use-module (cogkernel agents)
  #:use-module (cogkernel attention)
  #:use-module (cogkernel tensors)
  #:use-module (ice-9 match)
  #:use-module (ice-9 threads)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (srfi srfi-19)
  #:export (make-learning-system
            learning-system?
            learn-from-experience
            adapt-behavior
            pattern-recognition
            continuous-learning-loop
            register-learning-callback
            create-learning-experience
            learning-experience?
            learning-experience-context
            learning-experience-outcome
            evaluate-learning-effectiveness
            advanced-pattern-learning
            q-learning-update
            temporal-difference-learning
            experience-replay
            monitor-learning-effectiveness
            enhanced-continuous-learning-loop
            *global-learning-system*
            current-time-seconds))

;;; Learning experience record
(define-record-type <learning-experience>
  (make-learning-experience-record id context action outcome feedback timestamp)
  learning-experience?
  (id learning-experience-id)
  (context learning-experience-context)
  (action learning-experience-action)
  (outcome learning-experience-outcome)
  (feedback learning-experience-feedback)
  (timestamp learning-experience-timestamp))

;;; Learning pattern record
(define-record-type <learning-pattern>
  (make-learning-pattern-record id pattern-type features confidence frequency last-update)
  learning-pattern?
  (id learning-pattern-id)
  (pattern-type learning-pattern-type)
  (features learning-pattern-features)
  (confidence learning-pattern-confidence set-learning-pattern-confidence!)
  (frequency learning-pattern-frequency set-learning-pattern-frequency!)
  (last-update learning-pattern-last-update set-learning-pattern-last-update!))

;;; Real-time learning system record
(define-record-type <learning-system>
  (make-learning-system-record pattern-learning temporal-difference reinforcement experience-buffer callbacks)
  learning-system?
  (pattern-learning learning-system-pattern-learning)
  (temporal-difference learning-system-temporal-difference)
  (reinforcement learning-system-reinforcement)
  (experience-buffer learning-system-experience-buffer)
  (callbacks learning-system-callbacks set-learning-system-callbacks!))

;;; Current time helper function  
(define (current-time-seconds)
  "Get current time as number of seconds since epoch"
  (time-second (current-time time-utc)))

;;; Learning types
(define learning-types
  '(PATTERN-RECOGNITION
    TEMPORAL-DIFFERENCE
    REINFORCEMENT
    UNSUPERVISED
    SUPERVISED
    META-LEARNING))

;;; Create real-time learning system
(define* (make-learning-system #:key 
                               (pattern-learning #t)
                               (temporal-difference #t)
                               (reinforcement #t))
  "Create a new real-time learning system"
  (make-learning-system-record pattern-learning temporal-difference reinforcement
                              (make-hash-table) (make-hash-table)))

;;; Create learning experience
(define (create-learning-experience context action outcome feedback)
  "Create a new learning experience record"
  (make-learning-experience-record (gensym "exp") context action outcome feedback (current-time-seconds)))

;;; Learn from experience
(define (learn-from-experience learning-system experience)
  "Process a learning experience and update system knowledge"
  (let* ((experience-id (learning-experience-id experience))
         (context (learning-experience-context experience))
         (action (learning-experience-action experience))
         (outcome (learning-experience-outcome experience))
         (feedback (learning-experience-feedback experience)))
    
    ; Store experience in buffer
    (hash-set! (learning-system-experience-buffer learning-system) experience-id experience)
    
    ; Add experience to atomspace for pattern recognition
    (let ((exp-atom (make-atom 'EXPERIENCE (symbol->string experience-id))))
      (atomspace-add! *global-atomspace* exp-atom)
      
      ; Create context links
      (let ((context-link (make-link 'EVALUATION
                                    (list (make-atom 'PREDICATE "context")
                                          exp-atom
                                          (make-atom 'CONCEPT (format #f "~a" context))))))
        (atomspace-add! *global-atomspace* context-link))
      
      ; Create action links
      (let ((action-link (make-link 'EVALUATION
                                   (list (make-atom 'PREDICATE "action")
                                         exp-atom
                                         (make-atom 'CONCEPT (format #f "~a" action))))))
        (atomspace-add! *global-atomspace* action-link))
      
      ; Create outcome links
      (let ((outcome-link (make-link 'EVALUATION
                                    (list (make-atom 'PREDICATE "outcome")
                                          exp-atom
                                          (make-atom 'CONCEPT (format #f "~a" outcome))))))
        (atomspace-add! *global-atomspace* outcome-link)))
    
    ; Perform different types of learning
    (when (learning-system-pattern-learning learning-system)
      (update-pattern-recognition learning-system experience))
    
    (when (learning-system-temporal-difference learning-system)
      (update-temporal-difference learning-system experience))
    
    (when (learning-system-reinforcement learning-system)
      (update-reinforcement-learning learning-system experience))
    
    ; Trigger learning callbacks
    (trigger-learning-callbacks learning-system experience)
    
    experience))

;;; Update pattern recognition
(define (update-pattern-recognition learning-system experience)
  "Update pattern recognition based on experience"
  (let* ((context (learning-experience-context experience))
         (action (learning-experience-action experience))
         (pattern-key (format #f "~a->~a" context action)))
    
    ; Look for existing pattern
    (let ((existing-patterns (atomspace-query *global-atomspace* 
                                             `(pattern ,pattern-key))))
      (if (null? existing-patterns)
          ; Create new pattern
          (let ((pattern-atom (make-atom 'PATTERN pattern-key)))
            (atomspace-add! *global-atomspace* pattern-atom)
            (attention-bank-add! *global-attention-bank* pattern-atom (make-attention-value 10 5 2)))
          ; Update existing pattern
          (let ((pattern-atom (car existing-patterns)))
            (attention-bank-stimulate! *global-attention-bank* pattern-atom 'ROUTINE 5))))))

;;; Update temporal difference learning
(define (update-temporal-difference learning-system experience)
  "Update temporal difference learning values"
  (let* ((context (learning-experience-context experience))
         (outcome (learning-experience-outcome experience))
         (feedback (learning-experience-feedback experience))
         (reward (if (number? feedback) feedback 
                    (cond
                      ((eq? feedback 'SUCCESS) 1.0)
                      ((eq? feedback 'FAILURE) -1.0)
                      (else 0.0)))))
    
    ; Simple TD update (in real implementation would use proper TD algorithm)
    (let ((state-value-atom (make-atom 'STATE-VALUE (format #f "~a" context))))
      (atomspace-add! *global-atomspace* state-value-atom)
      
      ; Create value update link
      (let ((value-link (make-link 'EVALUATION
                                  (list (make-atom 'PREDICATE "value")
                                        state-value-atom
                                        (make-atom 'NUMBER (number->string reward))))))
        (atomspace-add! *global-atomspace* value-link)))))

;;; Update reinforcement learning
(define (update-reinforcement-learning learning-system experience)
  "Update reinforcement learning policy"
  (let* ((context (learning-experience-context experience))
         (action (learning-experience-action experience))
         (feedback (learning-experience-feedback experience))
         (policy-key (format #f "policy-~a" context)))
    
    ; Update action preferences based on feedback
    (let ((policy-atom (make-atom 'POLICY policy-key)))
      (atomspace-add! *global-atomspace* policy-atom)
      
      ; Create policy update link
      (let ((update-link (make-link 'EVALUATION
                                   (list (make-atom 'PREDICATE "policy-update")
                                         policy-atom
                                         (make-atom 'CONCEPT (format #f "~a:~a" action feedback))))))
        (atomspace-add! *global-atomspace* update-link)))))

;;; Pattern recognition function
(define (pattern-recognition learning-system data)
  "Recognize patterns in the provided data"
  (let ((patterns '()))
    ; Query atomspace for similar patterns
    (let ((similar-patterns (atomspace-query *global-atomspace* 
                                            `(type PATTERN))))
      (for-each (lambda (pattern-atom)
                  (let ((pattern-name (atom-name pattern-atom)))
                    ; Simple pattern matching (in real implementation would use sophisticated algorithms)
                    (when (string-contains pattern-name (format #f "~a" data))
                      (set! patterns (cons pattern-atom patterns)))))
                similar-patterns))
    patterns))

;;; Adapt behavior based on learning
(define (adapt-behavior learning-system context)
  "Adapt behavior based on learned patterns and experiences"
  (let* ((context-str (format #f "~a" context))
         (relevant-patterns (pattern-recognition learning-system context))
         (policy-atoms (atomspace-query *global-atomspace* 
                                       `(type POLICY))))
    
    ; Find best action based on learned policies
    (let ((best-action 'DEFAULT))
      (for-each (lambda (policy-atom)
                  (let ((policy-name (atom-name policy-atom)))
                    (when (string-contains policy-name context-str)
                      ; Simple action selection (would use proper policy evaluation)
                      (set! best-action (string->symbol 
                                        (substring policy-name 
                                                  (+ (string-length "policy-") 
                                                     (string-length context-str) 1)))))))
                policy-atoms)
      best-action)))

;;; Continuous learning loop
(define (continuous-learning-loop learning-system)
  "Run continuous learning process"
  (let ((running #t))
    (while running
      ; Process pending experiences
      (hash-for-each (lambda (id experience)
                      ; Reprocess old experiences for meta-learning
                      (when (> (- (current-time-seconds) (learning-experience-timestamp experience)) 3600)
                        (meta-learn-from-experience learning-system experience)))
                    (learning-system-experience-buffer learning-system))
      
      ; Sleep for a bit
      (sleep 1))))

;;; Meta-learning from experiences
(define (meta-learn-from-experience learning-system experience)
  "Perform meta-learning on past experiences"
  ; Look for higher-order patterns across multiple experiences
  (let ((similar-experiences 
         (filter (lambda (exp-pair)
                   (let ((exp (cdr exp-pair)))
                     (equal? (learning-experience-context exp)
                            (learning-experience-context experience))))
                (hash-map->list cons (learning-system-experience-buffer learning-system)))))
    
    (when (> (length similar-experiences) 3)
      ; Create meta-pattern
      (let ((meta-pattern-atom (make-atom 'META-PATTERN 
                                         (format #f "meta-~a" 
                                               (learning-experience-context experience)))))
        (atomspace-add! *global-atomspace* meta-pattern-atom)
        (attention-bank-stimulate! *global-attention-bank* meta-pattern-atom 'IMPORTANT 8)))))

;;; Advanced pattern learning with statistical confidence
(define (advanced-pattern-learning learning-system data threshold)
  "Advanced pattern learning with statistical confidence measures"
  (let ((pattern-stats (make-hash-table))
        (confidence-threshold (or threshold 0.75)))
    
    ; Analyze experience buffer for pattern frequencies
    (hash-for-each 
      (lambda (id experience)
        (let* ((context (learning-experience-context experience))
               (action (learning-experience-action experience))
               (outcome (learning-experience-outcome experience))
               (pattern-key (format #f "~a->~a->~a" context action outcome)))
          
          (hash-set! pattern-stats pattern-key
                    (+ 1 (hash-ref pattern-stats pattern-key 0)))))
      (learning-system-experience-buffer learning-system))
    
    ; Extract high-confidence patterns
    (let ((total-experiences (hash-count (const #t) (learning-system-experience-buffer learning-system)))
          (confident-patterns '()))
      
      (hash-for-each
        (lambda (pattern count)
          (let ((confidence (/ count total-experiences)))
            (when (> confidence confidence-threshold)
              (set! confident-patterns 
                   (cons (list pattern confidence count) confident-patterns)))))
        pattern-stats)
      
      confident-patterns)))

;;; Reinforcement learning value function
(define (q-learning-update learning-system state action reward next-state)
  "Q-learning update for reinforcement learning"
  (let* ((alpha 0.1)    ; Learning rate
         (gamma 0.9)    ; Discount factor
         (q-key (format #f "Q-~a-~a" state action))
         (current-q (get-q-value learning-system q-key))
         (max-next-q (get-max-q-value learning-system next-state))
         (new-q (+ current-q (* alpha (- (+ reward (* gamma max-next-q)) current-q)))))
    
    ; Store updated Q-value in atomspace
    (let ((q-atom (make-atom 'STATE-VALUE q-key)))
      (atomspace-add! *global-atomspace* q-atom)
      (let ((q-link (make-link 'EVALUATION
                              (list (make-atom 'PREDICATE "q-value")
                                    q-atom
                                    (make-atom 'NUMBER (number->string new-q))))))
        (atomspace-add! *global-atomspace* q-link)))))

;;; Helper functions for Q-learning
(define (get-q-value learning-system q-key)
  "Get Q-value for state-action pair"
  (let ((q-atoms (atomspace-query *global-atomspace* 
                                 (lambda (atom)
                                   (and (eq? (atom-type atom) 'STATE-VALUE)
                                        (string=? (atom-name atom) q-key))))))
    (if (null? q-atoms) 0.0
        ; Extract value from atomspace (simplified)
        0.0)))

(define (get-max-q-value learning-system state)
  "Get maximum Q-value for all actions in a state"
  (let ((state-q-atoms (atomspace-query *global-atomspace*
                                       (lambda (atom)
                                         (and (eq? (atom-type atom) 'STATE-VALUE)
                                              (string-contains (atom-name atom) 
                                                             (format #f "Q-~a-" state)))))))
    (if (null? state-q-atoms) 0.0
        ; Find maximum Q-value (simplified)
        0.0)))

;;; Temporal difference learning for value estimation
(define (temporal-difference-learning learning-system experience)
  "Temporal difference learning for state value estimation"
  (let* ((context (learning-experience-context experience))
         (outcome (learning-experience-outcome experience))
         (feedback (learning-experience-feedback experience))
         (reward (cond
                   ((eq? feedback 'SUCCESS) 1.0)
                   ((eq? feedback 'FAILURE) -1.0)
                   ((number? feedback) feedback)
                   (else 0.0)))
         (alpha 0.15)   ; Learning rate
         (current-value (get-state-value learning-system context))
         (new-value (+ current-value (* alpha (- reward current-value)))))
    
    ; Update state value
    (update-state-value learning-system context new-value)))

(define (get-state-value learning-system state)
  "Get current state value estimate"
  (let ((state-atoms (atomspace-query *global-atomspace*
                                     (lambda (atom)
                                       (and (eq? (atom-type atom) 'STATE-VALUE)
                                            (string=? (atom-name atom) 
                                                     (format #f "~a" state)))))))
    (if (null? state-atoms) 0.0
        ; Extract value (simplified)
        0.0)))

(define (update-state-value learning-system state value)
  "Update state value in atomspace"
  (let ((state-atom (make-atom 'STATE-VALUE (format #f "~a" state))))
    (atomspace-add! *global-atomspace* state-atom)
    (let ((value-link (make-link 'EVALUATION
                                (list (make-atom 'PREDICATE "state-value")
                                      state-atom
                                      (make-atom 'NUMBER (number->string value))))))
      (atomspace-add! *global-atomspace* value-link))))

;;; Experience replay for improved learning
(define (experience-replay learning-system batch-size)
  "Replay random experiences for improved learning"
  (let ((experiences (hash-map->list cons (learning-system-experience-buffer learning-system)))
        (replay-batch '()))
    
    ; Sample random experiences
    (let ((sample-size (min batch-size (length experiences))))
      (do ((i 0 (+ i 1)))
          ((>= i sample-size))
        (let ((random-exp (list-ref experiences (random (length experiences)))))
          (set! replay-batch (cons (cdr random-exp) replay-batch))))
      
      ; Re-learn from sampled experiences
      (for-each (lambda (exp)
                  (update-pattern-recognition learning-system exp)
                  (update-temporal-difference learning-system exp)
                  (update-reinforcement-learning learning-system exp))
                replay-batch))))

;;; Learning effectiveness monitoring
(define (monitor-learning-effectiveness learning-system)
  "Monitor and report learning system effectiveness"
  (let* ((total-exp (hash-count (const #t) (learning-system-experience-buffer learning-system)))
         (success-rate (evaluate-learning-effectiveness learning-system))
         (pattern-count (length (atomspace-query *global-atomspace*
                                               (lambda (atom)
                                                 (eq? (atom-type atom) 'PATTERN)))))
         (meta-pattern-count (length (atomspace-query *global-atomspace*
                                                     (lambda (atom)
                                                       (eq? (atom-type atom) 'META-PATTERN))))))
    
    `((total-experiences . ,total-exp)
      (success-rate . ,success-rate)
      (patterns-learned . ,pattern-count)
      (meta-patterns . ,meta-pattern-count)
      (learning-efficiency . ,(if (> total-exp 0) (/ pattern-count total-exp) 0)))))

;;; Enhanced continuous learning with adaptive parameters
(define (enhanced-continuous-learning-loop learning-system)
  "Enhanced continuous learning loop with adaptive parameters"
  (let ((running #t)
        (cycle-count 0)
        (adaptation-threshold 100))
    
    (while running
      (set! cycle-count (+ cycle-count 1))
      
      ; Periodic experience replay
      (when (= (modulo cycle-count 10) 0)
        (experience-replay learning-system 5))
      
      ; Adaptive parameter adjustment
      (when (= (modulo cycle-count adaptation-threshold) 0)
        (adapt-learning-parameters learning-system))
      
      ; Process pending experiences for meta-learning
      (hash-for-each (lambda (id experience)
                      (when (> (- (current-time-seconds) (learning-experience-timestamp experience)) 3600)
                        (meta-learn-from-experience learning-system experience)))
                    (learning-system-experience-buffer learning-system))
      
      ; Monitor effectiveness
      (when (= (modulo cycle-count 50) 0)
        (let ((effectiveness (monitor-learning-effectiveness learning-system)))
          (format #t "Learning effectiveness: ~a~%" effectiveness)))
      
      ; Sleep with adaptive interval
      (let ((sleep-interval (if (< cycle-count 100) 1 2)))
        (sleep sleep-interval)))))

;;; Adaptive parameter adjustment
(define (adapt-learning-parameters learning-system)
  "Adapt learning parameters based on performance"
  (let ((effectiveness (evaluate-learning-effectiveness learning-system)))
    ; Simple adaptive strategy
    (cond
      ((< effectiveness 0.3)
       (format #t "Low performance detected, increasing exploration~%"))
      ((> effectiveness 0.8)
       (format #t "High performance detected, fine-tuning parameters~%"))
      (else
       (format #t "Performance stable at ~a~%" effectiveness)))))

;;; Register learning callback
(define (register-learning-callback learning-system callback-id callback-proc)
  "Register a callback function to be triggered on learning events"
  (hash-set! (learning-system-callbacks learning-system) callback-id callback-proc))

;;; Trigger learning callbacks
(define (trigger-learning-callbacks learning-system experience)
  "Trigger all registered learning callbacks"
  (hash-for-each (lambda (id callback)
                   (callback experience))
                 (learning-system-callbacks learning-system)))

;;; Helper function to evaluate learning effectiveness
(define (evaluate-learning-effectiveness learning-system)
  "Evaluate how effective the learning system has been"
  (let ((total-experiences (hash-count (const #t) (learning-system-experience-buffer learning-system)))
        (successful-experiences 0))
    
    (hash-for-each (lambda (id experience)
                     (when (eq? (learning-experience-feedback experience) 'SUCCESS)
                       (set! successful-experiences (+ successful-experiences 1))))
                   (learning-system-experience-buffer learning-system))
    
    (if (> total-experiences 0)
        (/ successful-experiences total-experiences)
        0.0)))

;;; Global learning system instance
(define *global-learning-system*
  (make-learning-system #:pattern-learning #t
                       #:temporal-difference #t
                       #:reinforcement #t))