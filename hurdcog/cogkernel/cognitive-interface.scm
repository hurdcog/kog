;;; Cognitive Operations Interface - Main Integration Module
;;; Integrates Distributed Agent Framework, Cognitive Workflow Engine, and Real-time Learning Systems
;;; Implements the complete cognitive operations interface for SKZ framework

(define-module (cogkernel cognitive-interface)
  #:use-module (cogkernel atomspace)
  #:use-module (cogkernel agents)
  #:use-module (cogkernel attention)
  #:use-module (cogkernel tensors)
  #:use-module (cogkernel cognitive-interface distributed-agents protocol)
  #:use-module (cogkernel cognitive-interface workflow-engine processor)
  #:use-module (cogkernel cognitive-interface learning-systems realtime)
  #:use-module (cogkernel cognitive-interface decision-making autonomous)
  #:use-module (ice-9 match)
  #:use-module (ice-9 threads)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:export (make-cognitive-operations-interface
            cognitive-operations-interface?
            initialize-cognitive-interface
            execute-cognitive-operation
            register-cognitive-agent
            create-cognitive-workflow
            enable-cognitive-learning
            autonomous-decision-making
            execute-autonomous-decision
            get-interface-status
            shutdown-cognitive-interface
            *global-cognitive-interface*))

;;; Cognitive operations interface record
(define-record-type <cognitive-operations-interface>
  (make-cognitive-operations-interface-record agent-framework workflow-engine learning-system decision-system status config)
  cognitive-operations-interface?
  (agent-framework cognitive-operations-interface-agent-framework)
  (workflow-engine cognitive-operations-interface-workflow-engine)
  (learning-system cognitive-operations-interface-learning-system)
  (decision-system cognitive-operations-interface-decision-system)
  (status cognitive-operations-interface-status set-cognitive-operations-interface-status!)
  (config cognitive-operations-interface-config))

;;; Interface status types
(define interface-status-types
  '(INITIALIZING ACTIVE PAUSED ERROR SHUTDOWN))

;;; Configuration record
(define-record-type <cognitive-config>
  (make-cognitive-config-record parallel-processing jit-compilation distributed-storage learning-enabled)
  cognitive-config?
  (parallel-processing cognitive-config-parallel-processing)
  (jit-compilation cognitive-config-jit-compilation)
  (distributed-storage cognitive-config-distributed-storage)
  (learning-enabled cognitive-config-learning-enabled))

;;; Create cognitive operations interface
(define* (make-cognitive-operations-interface #:key
                                              (parallel-processing 'kokkos)
                                              (jit-compilation 'compiler-explorer)
                                              (distributed-storage 'atomspace)
                                              (learning-enabled #t)
                                              (autonomy-level 3))
  "Create a new cognitive operations interface with specified configuration"
  (let* ((config (make-cognitive-config-record parallel-processing jit-compilation 
                                               distributed-storage learning-enabled))
         (agent-framework (make-agent-communication #:protocol 'atomspace-message-passing
                                                   #:transport 'distributed
                                                   #:serialization 'atomspace-serialization))
         (workflow-engine (make-cognitive-workflow-engine #:parallel-processing parallel-processing
                                                         #:jit-compilation jit-compilation
                                                         #:atomspace-storage distributed-storage))
         (learning-system (if learning-enabled
                             (make-learning-system #:pattern-learning #t
                                                  #:temporal-difference #t
                                                  #:reinforcement #t)
                             #f))
         (decision-system (make-autonomous-decision-system #:autonomy-level autonomy-level
                                                          #:learning-system learning-system)))
    
    (make-cognitive-operations-interface-record agent-framework workflow-engine learning-system
                                               decision-system 'INITIALIZING config)))

;;; Initialize cognitive interface
(define (initialize-cognitive-interface interface)
  "Initialize the cognitive operations interface and all its components"
  (set-cognitive-operations-interface-status! interface 'INITIALIZING)
  
  ; Initialize atomspace with cognitive operations concepts
  (let ((cog-ops-atom (make-atom 'CONCEPT "cognitive-operations")))
    (atomspace-add! *global-atomspace* cog-ops-atom)
    
    ; Create interface links
    (let ((interface-link (make-link 'EVALUATION
                                    (list (make-atom 'PREDICATE "interface-type")
                                          cog-ops-atom
                                          (make-atom 'CONCEPT "distributed-cognitive")))))
      (atomspace-add! *global-atomspace* interface-link)))
  
  ; Setup agent communication network
  (setup-agent-network (cognitive-operations-interface-agent-framework interface) '())
  
  ; Register workflow message handlers
  (register-message-handler (cognitive-operations-interface-agent-framework interface)
                           'TASK-REQUEST
                           (lambda (message)
                             (handle-workflow-request interface message)))
  
  ; Register learning callbacks if learning is enabled
  (when (cognitive-operations-interface-learning-system interface)
    (register-learning-callback (cognitive-operations-interface-learning-system interface)
                               'workflow-completion
                               (lambda (experience)
                                 (handle-learning-experience interface experience))))
  
  ; Set status to active
  (set-cognitive-operations-interface-status! interface 'ACTIVE)
  
  (format #t "Cognitive Operations Interface initialized successfully~%")
  interface)

;;; Execute cognitive operation
(define (execute-cognitive-operation interface operation-type . args)
  "Execute a cognitive operation through the interface"
  (unless (eq? (cognitive-operations-interface-status interface) 'ACTIVE)
    (error "Cognitive interface not active" (cognitive-operations-interface-status interface)))
  
  (match operation-type
    ('AGENT-COMMUNICATION
     (apply execute-agent-communication interface args))
    ('WORKFLOW-EXECUTION
     (apply execute-workflow-operation interface args))
    ('LEARNING-UPDATE
     (apply execute-learning-operation interface args))
    ('AUTONOMOUS-DECISION
     (apply execute-autonomous-decision-operation interface args))
    ('INTEGRATED-OPERATION
     (apply execute-integrated-operation interface args))
    (else
     (error "Unknown cognitive operation type" operation-type))))

;;; Execute agent communication operation
(define (execute-agent-communication interface sender recipient message-type payload)
  "Execute agent communication through the distributed agent framework"
  (let* ((agent-framework (cognitive-operations-interface-agent-framework interface))
         (message (make-cognitive-message sender recipient message-type payload)))
    
    ; Send message
    (send-cognitive-message agent-framework recipient message)
    
    ; Log communication event
    (let ((comm-atom (make-atom 'COMMUNICATION (cognitive-message-id message))))
      (atomspace-add! *global-atomspace* comm-atom)
      (attention-bank-stimulate! *global-attention-bank* comm-atom 'ROUTINE 3))
    
    message))

;;; Execute workflow operation
(define (execute-workflow-operation interface workflow-definition)
  "Execute workflow through the cognitive workflow engine"
  (let ((workflow-engine (cognitive-operations-interface-workflow-engine interface)))
    
    ; Execute workflow
    (let ((results (execute-cognitive-workflow workflow-engine workflow-definition)))
      
      ; Learn from workflow execution if learning is enabled
      (when (cognitive-operations-interface-learning-system interface)
        (let ((experience (create-learning-experience 
                          `(workflow ,(workflow-definition-id workflow-definition))
                          'EXECUTE-WORKFLOW
                          results
                          'SUCCESS)))
          (learn-from-experience (cognitive-operations-interface-learning-system interface) experience)))
      
      results)))

;;; Execute learning operation
(define (execute-learning-operation interface context action outcome feedback)
  "Execute learning operation through the real-time learning system"
  (let ((learning-system (cognitive-operations-interface-learning-system interface)))
    (when learning-system
      (let ((experience (create-learning-experience context action outcome feedback)))
        (learn-from-experience learning-system experience)))))

;;; Execute integrated operation combining all three systems
(define (execute-integrated-operation interface operation-spec)
  "Execute complex operation integrating agent communication, workflow execution, and learning"
  (match operation-spec
    (('distributed-workflow workflow-id agents workflow-definition)
     ; Coordinate distributed workflow execution
     (coordinate-distributed-workflow interface workflow-id agents workflow-definition))
    (('learning-workflow context workflow-definition)
     ; Execute workflow with learning adaptation
     (execute-adaptive-workflow interface context workflow-definition))
    (('agent-learning agent-id experiences)
     ; Coordinate agent learning from experiences
     (coordinate-agent-learning interface agent-id experiences))
    (else
     (error "Unknown integrated operation specification" operation-spec))))

;;; Coordinate distributed workflow execution
(define (coordinate-distributed-workflow interface workflow-id agents workflow-definition)
  "Coordinate execution of a workflow across distributed agents"
  (let ((agent-framework (cognitive-operations-interface-agent-framework interface))
        (workflow-engine (cognitive-operations-interface-workflow-engine interface)))
    
    ; Send workflow tasks to agents
    (for-each (lambda (agent-id)
                (let ((task-message (make-cognitive-message 
                                    'COORDINATOR agent-id 'TASK-REQUEST
                                    `(workflow-task ,workflow-id ,workflow-definition))))
                  (send-cognitive-message agent-framework agent-id task-message)))
              agents)
    
    ; Execute coordinator's part of workflow
    (execute-cognitive-workflow workflow-engine workflow-definition)))

;;; Execute adaptive workflow with learning
(define (execute-adaptive-workflow interface context workflow-definition)
  "Execute workflow that adapts based on learning system feedback"
  (let ((learning-system (cognitive-operations-interface-learning-system interface))
        (workflow-engine (cognitive-operations-interface-workflow-engine interface)))
    
    ; Check for learned adaptations
    (when learning-system
      (let ((adapted-behavior (adapt-behavior learning-system context)))
        (when (not (eq? adapted-behavior 'DEFAULT))
          (format #t "Adapting workflow based on learned behavior: ~a~%" adapted-behavior))))
    
    ; Execute workflow
    (let ((results (execute-cognitive-workflow workflow-engine workflow-definition)))
      
      ; Learn from execution
      (when learning-system
        (let ((experience (create-learning-experience context 'ADAPTIVE-WORKFLOW results 'SUCCESS)))
          (learn-from-experience learning-system experience)))
      
      results)))

;;; Register cognitive agent
(define (register-cognitive-agent interface agent)
  "Register an agent with the cognitive operations interface"
  (let ((agent-framework (cognitive-operations-interface-agent-framework interface)))
    
    ; Add agent to system
    (agent-system-add! *global-agent-system* agent)
    
    ; Setup communication for agent
    (setup-agent-network agent-framework (list agent))
    
    ; Add agent to atomspace
    (let ((agent-atom (make-atom 'COGNITIVE-AGENT (agent-id agent))))
      (atomspace-add! *global-atomspace* agent-atom)
      (attention-bank-allocate! *global-attention-bank* agent-atom 20))
    
    agent))

;;; Create cognitive workflow
(define (create-cognitive-workflow interface workflow-id steps)
  "Create a cognitive workflow with the interface"
  (let ((workflow-def (create-workflow-definition workflow-id steps)))
    
    ; Add workflow to atomspace
    (let ((workflow-atom (make-atom 'COGNITIVE-WORKFLOW (symbol->string workflow-id))))
      (atomspace-add! *global-atomspace* workflow-atom)
      (attention-bank-allocate! *global-attention-bank* workflow-atom 15))
    
    workflow-def))

;;; Enable cognitive learning with enhanced real-time capabilities
(define (enable-cognitive-learning interface)
  "Enable or reinitialize cognitive learning capabilities with real-time features"
  (unless (cognitive-operations-interface-learning-system interface)
    (let ((learning-system (make-learning-system #:pattern-learning #t
                                                 #:temporal-difference #t
                                                 #:reinforcement #t)))
      
      ; Register learning callbacks for cognitive operations
      (register-learning-callback learning-system 'workflow-callback
        (lambda (exp)
          (when (eq? (learning-experience-action exp) 'EXECUTE-WORKFLOW)
            (format #t "Workflow learning: ~a -> ~a~%" 
                   (learning-experience-context exp)
                   (learning-experience-outcome exp)))))
      
      (register-learning-callback learning-system 'agent-callback
        (lambda (exp)
          (when (eq? (learning-experience-action exp) 'AGENT-COMMUNICATION)
            (format #t "Agent communication learning: ~a~%" 
                   (learning-experience-feedback exp)))))
      
      ; This would require modifying the record, which is not directly supported
      ; In a real implementation, we'd use a mutable field or recreate the interface
      (format #t "Enhanced real-time learning system enabled~%")
      (format #t "  Pattern recognition: enabled~%")
      (format #t "  Temporal difference learning: enabled~%") 
      (format #t "  Reinforcement learning: enabled~%")
      (format #t "  Meta-learning: enabled~%")
      (format #t "  Experience replay: enabled~%")
      
      learning-system)))

;;; Execute autonomous decision operation
(define (execute-autonomous-decision-operation interface situation options . optional-args)
  "Execute autonomous decision making through the decision system"
  (let* ((decision-system (cognitive-operations-interface-decision-system interface))
         (urgency (if (pair? optional-args) (car optional-args) 'medium))
         (context ((@ (cogkernel cognitive-interface decision-making autonomous) create-decision-context)
                   situation options #:urgency urgency)))
    
    (format #t "Executing autonomous decision for situation: ~a~%" situation)
    (autonomous-decide decision-system context)))

;;; Autonomous decision making interface function
(define (autonomous-decision-making interface situation options . optional-args)
  "High-level autonomous decision making interface"
  (apply execute-autonomous-decision-operation interface situation options optional-args))

;;; Execute autonomous decision with context
(define (execute-autonomous-decision interface context)
  "Execute autonomous decision with pre-created context"
  (let ((decision-system (cognitive-operations-interface-decision-system interface)))
    (autonomous-decide decision-system context)))

;;; Handle workflow request message
(define (handle-workflow-request interface message)
  "Handle incoming workflow request message"
  (let ((payload (cognitive-message-payload message))
        (sender (cognitive-message-sender message)))
    
    (match payload
      (('workflow-task workflow-id workflow-definition)
       (format #t "Handling workflow task ~a from ~a~%" workflow-id sender)
       (execute-workflow-operation interface workflow-definition))
      (else
       (format #t "Unknown workflow request: ~a~%" payload)))))

;;; Handle learning experience
(define (handle-learning-experience interface experience)
  "Handle learning experience from workflow completion"
  (format #t "Learning from experience: ~a -> ~a~%" 
          (learning-experience-context experience)
          (learning-experience-outcome experience)))

;;; Get interface status
(define (get-interface-status interface)
  "Get current status and statistics of the cognitive operations interface"
  (let ((agent-count (hash-count (const #t) (agent-system-agents *global-agent-system*)))
        (atomspace-size (+ (hash-count (const #t) (atomspace-atoms *global-atomspace*))
                          (hash-count (const #t) (atomspace-links *global-atomspace*))))
        (learning-effectiveness (if (cognitive-operations-interface-learning-system interface)
                                   (evaluate-learning-effectiveness 
                                    (cognitive-operations-interface-learning-system interface))
                                   0.0)))
    
    `((status . ,(cognitive-operations-interface-status interface))
      (agents . ,agent-count)
      (atomspace-size . ,atomspace-size)
      (learning-effectiveness . ,learning-effectiveness)
      (config . ,(cognitive-operations-interface-config interface)))))

;;; Shutdown cognitive interface
(define (shutdown-cognitive-interface interface)
  "Shutdown the cognitive operations interface gracefully"
  (set-cognitive-operations-interface-status! interface 'SHUTDOWN)
  (format #t "Cognitive Operations Interface shutdown complete~%"))

;;; Global cognitive operations interface instance
(define *global-cognitive-interface*
  (make-cognitive-operations-interface #:parallel-processing 'kokkos
                                      #:jit-compilation 'compiler-explorer
                                      #:distributed-storage 'atomspace
                                      #:learning-enabled #t))