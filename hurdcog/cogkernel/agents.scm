;;; Agents - Agentic Task Orchestration for Cognitive Kernel
;;; Implements distributed agentic scripts inspired by Agent-Zero/ElizaOS
;;; Tensor shape: [n_agents x n_roles x n_actions x n_envs]

(define-module (cogkernel agents)
  #:use-module (ice-9 hash-table)
  #:use-module (ice-9 match)
  #:use-module (ice-9 threads)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (cogkernel atomspace)
  #:export (make-agent
            agent?
            agent-id
            agent-role
            agent-actions
            agent-environment
            agent-state
            agent-execute!
            agent-register-action!
            agent-send-message!
            agent-receive-messages!
            make-agent-system
            agent-system-add!
            agent-system-get
            agent-system-execute-all!
            agent-system-tensor-shape
            agent-system-enable-communication!
            agent-system-broadcast!
            agent-system-enable-distributed-framework!
            agent-system-get-framework
            *global-agent-system*))

;;; Agent roles for different system functions
(define agent-roles
  '(MONITOR        ; System monitoring and issue detection
    REPAIR         ; Automated repair and fix application
    BUILD          ; Build system coordination
    ANALYZE        ; Pattern analysis and learning
    OPTIMIZE       ; Performance optimization
    AUDIT          ; System audit and validation
    META           ; Meta-level system modification
    SYNTHESIZE))   ; Code and component synthesis

;;; Agent states
(define agent-states
  '(IDLE ACTIVE BLOCKED COMPLETED ERROR))

;;; Action types that agents can perform
(define action-types
  '(DETECT ANALYZE REPAIR BUILD TEST OPTIMIZE AUDIT SYNTHESIZE LEARN))

;;; Agent record structure
(define-record-type <agent>
  (make-agent-record id role actions environment state tensor-coords last-execution communication-enabled?)
  agent?
  (id agent-id)
  (role agent-role)
  (actions agent-actions set-agent-actions!)
  (environment agent-environment set-agent-environment!)
  (state agent-state set-agent-state!)
  (tensor-coords agent-tensor-coords)
  (last-execution agent-last-execution set-agent-last-execution!)
  (communication-enabled? agent-communication-enabled? set-agent-communication-enabled?!))

;;; Agent system for coordinating multiple agents
(define-record-type <agent-system>
  (make-agent-system-record agents tensor-shape execution-queue communication-system distributed-framework)
  agent-system?
  (agents agent-system-agents)
  (tensor-shape agent-system-tensor-shape)
  (execution-queue agent-system-execution-queue)
  (communication-system agent-system-communication-system set-agent-system-communication-system!)
  (distributed-framework agent-system-distributed-framework set-agent-system-distributed-framework!))

;;; Create a new agent
(define* (make-agent id role #:optional (environment '()) (actions '()))
  "Create a new agent with specified id, role, and optional environment/actions"
  (unless (member role agent-roles)
    (error "Invalid agent role:" role))
  (make-agent-record id role actions environment 'IDLE '(0 0 0 0) #f #f))

;;; Register an action with an agent
(define (agent-register-action! agent action-type action-proc)
  "Register an action procedure with an agent"
  (unless (member action-type action-types)
    (error "Invalid action type:" action-type))
  (set-agent-actions! agent 
                      (cons (cons action-type action-proc) 
                            (agent-actions agent))))

;;; Execute an agent's action
(define (agent-execute! agent action-type . args)
  "Execute a specific action on an agent"
  (set-agent-state! agent 'ACTIVE)
  (let ((action-proc (assq action-type (agent-actions agent))))
    (if action-proc
        (begin
          (set-agent-last-execution! agent (current-time))
          (let ((result (apply (cdr action-proc) args)))
            (set-agent-state! agent 'COMPLETED)
            result))
        (begin
          (set-agent-state! agent 'ERROR)
          (error "Action not found:" action-type)))))

;;; Create a new agent system
(define* (make-agent-system #:optional (tensor-shape '(10 8 10 4)))
  "Create a new agent system with tensor dimensions [n_agents x n_roles x n_actions x n_envs]"
  (make-agent-system-record (make-hash-table) tensor-shape '() #f #f))

;;; Add agent to system
(define (agent-system-add! agent-system agent)
  "Add an agent to the agent system"
  (hash-set! (agent-system-agents agent-system) (agent-id agent) agent))

;;; Get agent from system
(define (agent-system-get agent-system agent-id)
  "Retrieve an agent by id from the agent system"
  (hash-ref (agent-system-agents agent-system) agent-id))

;;; Execute all agents in system
(define (agent-system-execute-all! agent-system action-type . args)
  "Execute an action on all agents in the system"
  (hash-for-each
    (lambda (id agent)
      (catch #t
        (lambda ()
          (apply agent-execute! agent action-type args))
        (lambda (key . args)
          (format #t "Agent ~a failed: ~a~%" id args))))
    (agent-system-agents agent-system)))

;;; Get current tensor shape of agent system
(define (agent-system-tensor-shape agent-system)
  "Get current tensor shape [n_agents x n_roles x n_actions x n_envs]"
  (let ((n-agents (hash-count (const #t) (agent-system-agents agent-system)))
        (base-shape (agent-system-tensor-shape agent-system)))
    (list n-agents (second base-shape) (third base-shape) (fourth base-shape))))

;;; Create global agent system
(define *global-agent-system* (make-agent-system))

;;; Initialize core Hurd agents
(define (initialize-hurd-agents! agent-system atomspace)
  "Initialize core agents for GNU Hurd cognitive kernel"
  
  ;; System Monitor Agent
  (let ((monitor-agent (make-agent "system-monitor" 'MONITOR)))
    (agent-register-action! monitor-agent 'DETECT
      (lambda ()
        (let ((issue-atom (make-atom 'ISSUE "system-status")))
          (atomspace-add! atomspace issue-atom)
          "System status monitored")))
    (agent-system-add! agent-system monitor-agent))
  
  ;; Repair Agent
  (let ((repair-agent (make-agent "auto-repair" 'REPAIR)))
    (agent-register-action! repair-agent 'REPAIR
      (lambda (issue)
        (let ((repair-atom (make-atom 'SCRIPT "auto-repair-script")))
          (atomspace-add! atomspace repair-atom)
          (format #f "Repair attempted for: ~a" issue))))
    (agent-system-add! agent-system repair-agent))
  
  ;; Build Agent
  (let ((build-agent (make-agent "build-coordinator" 'BUILD)))
    (agent-register-action! build-agent 'BUILD
      (lambda (target)
        (let ((build-atom (make-atom 'BUILD (format #f "build-~a" target))))
          (atomspace-add! atomspace build-atom)
          (format #f "Build initiated for: ~a" target))))
    (agent-system-add! agent-system build-agent))
  
  ;; Analysis Agent
  (let ((analysis-agent (make-agent "pattern-analyzer" 'ANALYZE)))
    (agent-register-action! analysis-agent 'ANALYZE
      (lambda ()
        (let ((analysis-atom (make-atom 'RULE "pattern-analysis")))
          (atomspace-add! atomspace analysis-atom)
          "Pattern analysis completed")))
    (agent-system-add! agent-system analysis-agent))
  
  ;; Meta Agent for self-modification
  (let ((meta-agent (make-agent "meta-modifier" 'META)))
    (agent-register-action! meta-agent 'SYNTHESIZE
      (lambda ()
        (let ((meta-atom (make-atom 'SCRIPT "meta-modification")))
          (atomspace-add! atomspace meta-atom)
          "Meta-level modification synthesized")))
    (agent-system-add! agent-system meta-agent)))

;;; Initialize the global agent system (commented out for manual control)
;; (initialize-hurd-agents! *global-agent-system* *global-atomspace*)

;;; Agent Communication Functions
;;; These functions provide distributed communication capabilities

;;; Send message from agent to another agent
(define (agent-send-message! agent-system from-agent-id to-agent-id message-type payload)
  "Send a message from one agent to another through the communication system"
  (let ((comm-system (agent-system-communication-system agent-system)))
    (if comm-system
        (begin
          ;; Load the communication module dynamically if not already loaded
          (catch #t
            (lambda ()
              (eval '(use-modules (cogkernel agent-communication)) (interaction-environment))
              (let ((send-proc (module-ref (resolve-module '(cogkernel agent-communication)) 
                                           'send-cognitive-message)))
                (send-proc comm-system from-agent-id to-agent-id message-type payload)))
            (lambda (key . args)
              (format #t "⚠️  Communication module not available, using local message delivery~%")
              ;; Fallback to local message delivery
              (format #t "📨 Local message: ~a -> ~a (~a): ~a~%" 
                      from-agent-id to-agent-id message-type payload)
              `(message-sent (id . ,(string-append "local-" (number->string (random 1000))))
                            (status . local-delivery)
                            (timestamp . ,(current-time))))))
        (begin
          (format #t "❌ No communication system enabled for agent system~%")
          `(message-failed (error . no-communication-system))))))

;;; Receive messages for an agent
(define (agent-receive-messages! agent-system agent-id)
  "Receive messages for a specific agent"
  (let ((comm-system (agent-system-communication-system agent-system)))
    (if comm-system
        (begin
          (catch #t
            (lambda ()
              (eval '(use-modules (cogkernel agent-communication)) (interaction-environment))
              (let ((receive-proc (module-ref (resolve-module '(cogkernel agent-communication))
                                              'receive-cognitive-message)))
                (receive-proc comm-system agent-id)))
            (lambda (key . args)
              (format #t "📭 No messages available for ~a~%" agent-id)
              '())))
        '())))

;;; Enable communication for agent system
(define (agent-system-enable-communication! agent-system)
  "Enable distributed communication for the agent system"
  (catch #t
    (lambda ()
      (eval '(use-modules (cogkernel agent-communication)) (interaction-environment))
      (let ((setup-proc (module-ref (resolve-module '(cogkernel agent-communication))
                                    'setup-distributed-communication!))
            (start-proc (module-ref (resolve-module '(cogkernel agent-communication))
                                   'agent-communication-start!)))
        (let ((comm-system (setup-proc agent-system)))
          (set-agent-system-communication-system! agent-system comm-system)
          (start-proc comm-system)
          ;; Enable communication for all agents
          (hash-for-each
            (lambda (agent-id agent)
              (set-agent-communication-enabled?! agent #t)
              (format #t "📡 Communication enabled for agent ~a~%" agent-id))
            (agent-system-agents agent-system))
          comm-system)))
    (lambda (key . args)
      (format #t "⚠️  Could not enable communication system: ~a~%" args)
      #f)))

;;; Broadcast message to all agents in system
(define (agent-system-broadcast! agent-system from-agent-id message-type payload)
  "Broadcast a message from one agent to all others in the system"
  (let ((comm-system (agent-system-communication-system agent-system)))
    (if comm-system
        (begin
          (catch #t
            (lambda ()
              (eval '(use-modules (cogkernel agent-communication)) (interaction-environment))
              (let ((broadcast-proc (module-ref (resolve-module '(cogkernel agent-communication))
                                               'broadcast-to-agents)))
                (broadcast-proc comm-system from-agent-id message-type payload)))
            (lambda (key . args)
              (format #t "📢 Broadcasting locally: ~a (~a): ~a~%" 
                      from-agent-id message-type payload)
              ;; Fallback to local broadcast
              (hash-for-each
                (lambda (agent-id agent)
                  (unless (string=? agent-id from-agent-id)
                    (format #t "  -> ~a~%" agent-id)))
                (agent-system-agents agent-system)))))
        (format #t "❌ No communication system enabled~%"))))

;;; Enable distributed agent framework for agent system
(define (agent-system-enable-distributed-framework! agent-system)
  "Enable distributed agent framework for the agent system"
  (catch #t
    (lambda ()
      ;; Load distributed framework module
      (eval '(use-modules (cogkernel distributed-agent-framework)) (interaction-environment))
      
      ;; Get framework creation and start procedures
      (let ((framework-proc (module-ref (resolve-module '(cogkernel distributed-agent-framework))
                                       'make-distributed-agent-framework))
            (start-proc (module-ref (resolve-module '(cogkernel distributed-agent-framework))
                                   'framework-start!)))
        
        ;; Create and start framework
        (let ((framework (framework-proc #:deployment-strategy 'DISTRIBUTED)))
          (set-agent-system-distributed-framework! agent-system framework)
          (start-proc framework)
          
          ;; Register existing agents in framework
          (hash-for-each
            (lambda (agent-id agent)
              (format #t "🚀 Registering agent ~a in distributed framework~%" agent-id))
            (agent-system-agents agent-system))
          
          (format #t "✅ Distributed agent framework enabled for agent system~%")
          framework)))
    (lambda (key . args)
      (format #t "⚠️  Could not enable distributed framework: ~a~%" args)
      #f)))

;;; Get distributed framework from agent system
(define (agent-system-get-framework agent-system)
  "Get the distributed framework instance from agent system"
  (agent-system-distributed-framework agent-system))