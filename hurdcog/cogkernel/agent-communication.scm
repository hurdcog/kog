;;; Agent Communication - Distributed Agent Communication Protocol
;;; Implements atomspace-message-passing for distributed agent coordination
;;; Part of Phase 3: Build System Orchestration - SKZ Integration

(define-module (cogkernel agent-communication)
  #:use-module (ice-9 hash-table)
  #:use-module (ice-9 match)
  #:use-module (ice-9 threads)
  #:use-module (ice-9 format)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (cogkernel atomspace)
  #:use-module (cogkernel agents)
  #:export (make-agent-communication
            agent-communication?
            send-cognitive-message
            receive-cognitive-message
            register-agent-endpoint!
            discover-agents
            broadcast-to-agents
            setup-distributed-communication!
            agent-communication-start!
            agent-communication-stop!
            *global-agent-communication*))

;;; Message types for agent communication
(define message-types
  '(COORDINATION     ; Task coordination between agents
    STATUS-QUERY     ; Query agent status
    STATUS-RESPONSE  ; Response to status query
    TASK-ASSIGNMENT  ; Assign task to agent
    TASK-COMPLETION  ; Notify task completion
    RESOURCE-REQUEST ; Request shared resources
    RESOURCE-GRANT   ; Grant resource access
    DISCOVERY        ; Agent discovery and registration
    HEARTBEAT        ; Keep-alive messages
    SHUTDOWN))       ; Graceful shutdown notification

;;; Communication protocols
(define communication-protocols
  '(atomspace-message-passing  ; AtomSpace-based messaging
    json-rpc                   ; JSON-RPC for Theia integration
    ipc-direct                 ; Direct IPC for local communication
    tcp-socket))               ; TCP socket for network communication

;;; Agent communication record
(define-record-type <agent-communication>
  (make-agent-communication-record protocol transport serialization 
                                    endpoint-registry message-queue 
                                    active? thread-pool)
  agent-communication?
  (protocol agent-communication-protocol)
  (transport agent-communication-transport)
  (serialization agent-communication-serialization)
  (endpoint-registry agent-communication-endpoint-registry)
  (message-queue agent-communication-message-queue)
  (active? agent-communication-active? set-agent-communication-active?!)
  (thread-pool agent-communication-thread-pool))

;;; Cognitive message record
(define-record-type <cognitive-message>
  (make-cognitive-message-record id from to type payload timestamp)
  cognitive-message?
  (id cognitive-message-id)
  (from cognitive-message-from)
  (to cognitive-message-to)
  (type cognitive-message-type)
  (payload cognitive-message-payload)
  (timestamp cognitive-message-timestamp))

;;; Create agent communication system
(define* (make-agent-communication #:key 
                                   (protocol 'atomspace-message-passing)
                                   (transport 'distributed)
                                   (serialization 'atomspace-serialization))
  "Create a new agent communication system"
  (unless (member protocol communication-protocols)
    (error "Invalid communication protocol:" protocol))
  (make-agent-communication-record
    protocol
    transport
    serialization
    (make-hash-table)  ; endpoint registry
    '()                ; message queue
    #f                 ; not active initially
    #f))               ; no thread pool yet

;;; Register agent endpoint for communication
(define (register-agent-endpoint! comm agent-id endpoint-info)
  "Register an agent's communication endpoint"
  (hash-set! (agent-communication-endpoint-registry comm) 
             agent-id 
             (append endpoint-info 
                     `((registered-time ,(current-time))
                       (protocol ,(agent-communication-protocol comm))))))

;;; Discover available agents
(define (discover-agents comm)
  "Discover all registered agents in the communication system"
  (hash-fold (lambda (agent-id endpoint-info acc)
               (cons (list agent-id endpoint-info) acc))
             '()
             (agent-communication-endpoint-registry comm)))

;;; Serialize message for atomspace compatibility
(define (serialize-cognitive-message message)
  "Serialize a cognitive message for atomspace transport"
  (match (cognitive-message-type message)
    ('STATUS-QUERY
     `(atomspace-message
       (type . STATUS-QUERY)
       (from . ,(cognitive-message-from message))
       (to . ,(cognitive-message-to message))
       (id . ,(cognitive-message-id message))
       (timestamp . ,(cognitive-message-timestamp message))))
    ('TASK-ASSIGNMENT
     `(atomspace-message
       (type . TASK-ASSIGNMENT)
       (from . ,(cognitive-message-from message))
       (to . ,(cognitive-message-to message))
       (task . ,(cognitive-message-payload message))
       (id . ,(cognitive-message-id message))
       (timestamp . ,(cognitive-message-timestamp message))))
    ('COORDINATION
     `(atomspace-message
       (type . COORDINATION)
       (from . ,(cognitive-message-from message))
       (to . ,(cognitive-message-to message))
       (coordination-data . ,(cognitive-message-payload message))
       (id . ,(cognitive-message-id message))
       (timestamp . ,(cognitive-message-timestamp message))))
    (_
     `(atomspace-message
       (type . ,(cognitive-message-type message))
       (from . ,(cognitive-message-from message))
       (to . ,(cognitive-message-to message))
       (payload . ,(cognitive-message-payload message))
       (id . ,(cognitive-message-id message))
       (timestamp . ,(cognitive-message-timestamp message))))))

;;; Send cognitive message to another agent
(define (send-cognitive-message comm from-agent-id to-agent-id message-type payload)
  "Send a cognitive message from one agent to another"
  (let* ((message-id (string-append "msg-" (number->string (random 100000))))
         (message (make-cognitive-message-record
                   message-id
                   from-agent-id
                   to-agent-id
                   message-type
                   payload
                   (current-time)))
         (serialized-message (serialize-cognitive-message message)))
    
    ;; Check if destination agent is registered
    (let ((endpoint (hash-ref (agent-communication-endpoint-registry comm) to-agent-id)))
      (if endpoint
          (begin
            ;; For now, simulate message delivery via atomspace
            (format #t "📡 Sending message ~a: ~a -> ~a (~a)~%" 
                    message-id from-agent-id to-agent-id message-type)
            (format #t "   Payload: ~a~%" payload)
            ;; Add message to atomspace for cognitive routing
            (when (and (defined? '*global-atomspace*) *global-atomspace*)
              (let ((message-atom (make-atom 'MESSAGE serialized-message)))
                (atomspace-add! *global-atomspace* message-atom)))
            ;; Return message confirmation
            `(message-sent
              (id . ,message-id)
              (status . delivered)
              (timestamp . ,(current-time))))
          (begin
            (format #t "❌ Agent ~a not found in endpoint registry~%" to-agent-id)
            `(message-failed
              (id . ,message-id)
              (error . agent-not-found)
              (timestamp . ,(current-time))))))))

;;; Receive cognitive message (simulated for demonstration)
(define (receive-cognitive-message comm agent-id)
  "Receive cognitive messages for a specific agent"
  ;; For now, simulate message reception by checking atomspace
  (if (and (defined? '*global-atomspace*) *global-atomspace*)
      (begin
        (format #t "📬 Checking messages for agent ~a~%" agent-id)
        ;; In a real implementation, this would filter atomspace for messages
        ;; destined for this agent
        '((type . STATUS-QUERY) 
          (from . "system-monitor") 
          (payload . "status-check")))
      '()))

;;; Broadcast message to all registered agents
(define (broadcast-to-agents comm from-agent-id message-type payload)
  "Broadcast a message to all registered agents"
  (let ((agent-endpoints (discover-agents comm)))
    (format #t "📢 Broadcasting ~a from ~a to ~a agents~%" 
            message-type from-agent-id (length agent-endpoints))
    (map (lambda (agent-endpoint)
           (let ((agent-id (car agent-endpoint)))
             (unless (string=? agent-id from-agent-id)
               (send-cognitive-message comm from-agent-id agent-id message-type payload))))
         agent-endpoints)))

;;; Setup distributed communication for agent system
(define (setup-distributed-communication! agent-system)
  "Setup distributed communication for an existing agent system"
  (let ((comm (make-agent-communication 
               #:protocol 'atomspace-message-passing
               #:transport 'distributed
               #:serialization 'atomspace-serialization)))
    
    ;; Register all agents in the communication system
    (hash-for-each 
      (lambda (agent-id agent)
        (register-agent-endpoint! comm agent-id
          `((role . ,(agent-role agent))
            (state . ,(agent-state agent))
            (actions . ,(map car (agent-actions agent)))
            (endpoint-type . local))))
      (agent-system-agents agent-system))
    
    (format #t "🌐 Distributed communication setup for ~a agents~%" 
            (hash-count (const #t) (agent-system-agents agent-system)))
    comm))

;;; Start agent communication system
(define (agent-communication-start! comm)
  "Start the agent communication system"
  (set-agent-communication-active?! comm #t)
  (format #t "🚀 Agent communication system started (~a protocol)~%" 
          (agent-communication-protocol comm)))

;;; Stop agent communication system
(define (agent-communication-stop! comm)
  "Stop the agent communication system"
  (set-agent-communication-active?! comm #f)
  (format #t "🛑 Agent communication system stopped~%"))

;;; Global agent communication system
(define *global-agent-communication* 
  (make-agent-communication 
   #:protocol 'atomspace-message-passing
   #:transport 'distributed
   #:serialization 'atomspace-serialization))