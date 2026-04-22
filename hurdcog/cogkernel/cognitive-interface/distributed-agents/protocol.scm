;;; Distributed Agent Framework - Agent Communication Protocol
;;; File: cognitive-interface/distributed-agents/protocol.scm
;;; Implements agent communication using atomspace-message-passing

(define-module (cogkernel cognitive-interface distributed-agents protocol)
  #:use-module (cogkernel atomspace)
  #:use-module (cogkernel agents)
  #:use-module (cogkernel attention)
  #:use-module (ice-9 match)
  #:use-module (ice-9 threads)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:export (make-agent-communication
            agent-communication?
            send-cognitive-message
            receive-cognitive-message
            broadcast-message
            setup-agent-network
            register-message-handler
            make-cognitive-message
            cognitive-message?
            cognitive-message-id
            cognitive-message-sender
            cognitive-message-recipient
            cognitive-message-type
            cognitive-message-payload
            *global-agent-communication*))

;;; Message record structure
(define-record-type <cognitive-message>
  (make-cognitive-message-record id sender recipient type payload timestamp priority)
  cognitive-message?
  (id cognitive-message-id)
  (sender cognitive-message-sender)
  (recipient cognitive-message-recipient)
  (type cognitive-message-type)
  (payload cognitive-message-payload)
  (timestamp cognitive-message-timestamp)
  (priority cognitive-message-priority))

;;; Agent communication system
(define-record-type <agent-communication>
  (make-agent-communication-record protocol transport serialization message-queue handlers)
  agent-communication?
  (protocol agent-communication-protocol)
  (transport agent-communication-transport)
  (serialization agent-communication-serialization)
  (message-queue agent-communication-message-queue)
  (handlers agent-communication-handlers set-agent-communication-handlers!))

;;; Message types for cognitive communication
(define message-types
  '(TASK-REQUEST
    TASK-RESPONSE
    STATUS-UPDATE
    RESOURCE-REQUEST
    RESOURCE-GRANT
    COORDINATION
    LEARNING-UPDATE
    ERROR-REPORT
    ATTENTION-ALERT))

;;; Create agent communication system
(define* (make-agent-communication #:key 
                                   (protocol 'atomspace-message-passing)
                                   (transport 'distributed)
                                   (serialization 'atomspace-serialization))
  "Create a new agent communication system"
  (make-agent-communication-record protocol transport serialization 
                                   (make-queue) (make-hash-table)))

;;; Create cognitive message
(define* (make-cognitive-message sender recipient type payload #:optional (priority 1))
  "Create a new cognitive message"
  (unless (member type message-types)
    (error "Unknown message type" type))
  (make-cognitive-message-record (gensym "msg") sender recipient type payload 
                                (current-time) priority))

;;; Send cognitive message via atomspace
(define (send-cognitive-message comm agent-id message)
  "Send a cognitive message to a specific agent via atomspace message passing"
  
  ; Convert message to atomspace representation
  (let* ((message-atom (make-atom 'MESSAGE (cognitive-message-id message)))
         (sender-link (make-link 'EVALUATION 
                                (list (make-atom 'PREDICATE "sender")
                                      (make-atom 'AGENT (cognitive-message-sender message)))))
         (recipient-link (make-link 'EVALUATION
                                   (list (make-atom 'PREDICATE "recipient")
                                         (make-atom 'AGENT agent-id))))
         (type-link (make-link 'EVALUATION
                              (list (make-atom 'PREDICATE "message-type")
                                    (make-atom 'CONCEPT (symbol->string (cognitive-message-type message))))))
         (payload-link (make-link 'EVALUATION
                                 (list (make-atom 'PREDICATE "payload")
                                       (make-atom 'STRING (format #f "~a" (cognitive-message-payload message)))))))
    
    ; Add to global atomspace for distributed access
    (atomspace-add! *global-atomspace* message-atom)
    (atomspace-add! *global-atomspace* sender-link)
    (atomspace-add! *global-atomspace* recipient-link)
    (atomspace-add! *global-atomspace* type-link)
    (atomspace-add! *global-atomspace* payload-link)
    
    ; Queue message for processing
    (enqueue! (agent-communication-message-queue comm) message)
    
    ; Stimulate attention for urgent messages
    (when (> (cognitive-message-priority message) 5)
      (attention-bank-stimulate! *global-attention-bank* message-atom 'URGENT 
                                 (cognitive-message-priority message)))
    
    message))

;;; Receive cognitive messages for an agent
(define (receive-cognitive-message comm agent-id)
  "Receive all pending cognitive messages for a specific agent"
  (let ((messages '()))
    ; Query atomspace for messages to this agent
    (let ((recipient-atoms (atomspace-query *global-atomspace* 
                                           `(recipient ,agent-id))))
      (for-each (lambda (atom)
                  ; Extract message details from atomspace links
                  (let ((message-links (atomspace-query *global-atomspace*
                                                       `(message-for ,(atom-name atom)))))
                    (when (not (null? message-links))
                      (set! messages (cons atom messages)))))
                recipient-atoms))
    messages))

;;; Broadcast message to all agents
(define (broadcast-message comm message)
  "Broadcast a message to all agents in the system"
  (let ((all-agents (hash-map->list (lambda (k v) k) 
                                   (agent-system-agents *global-agent-system*))))
    (for-each (lambda (agent-id)
                (send-cognitive-message comm agent-id message))
              all-agents)))

;;; Setup agent network
(define (setup-agent-network comm agents)
  "Setup communication network for a list of agents"
  (for-each (lambda (agent)
              ; Register agent in atomspace
              (let ((agent-atom (make-atom 'AGENT (agent-id agent))))
                (atomspace-add! *global-atomspace* agent-atom)
                
                ; Create network links
                (let ((network-link (make-link 'MEMBER
                                              (list agent-atom
                                                    (make-atom 'CONCEPT "agent-network")))))
                  (atomspace-add! *global-atomspace* network-link))))
            agents))

;;; Register message handler
(define (register-message-handler comm message-type handler)
  "Register a handler function for a specific message type"
  (hash-set! (agent-communication-handlers comm) message-type handler))

;;; Process message queue
(define (process-message-queue comm)
  "Process all pending messages in the communication queue"
  (let ((queue (agent-communication-message-queue comm))
        (handlers (agent-communication-handlers comm)))
    (while (not (queue-empty? queue))
      (let* ((message (dequeue! queue))
             (handler (hash-ref handlers (cognitive-message-type message))))
        (when handler
          (handler message))))))

;;; Simple queue implementation
(define (make-queue)
  (cons '() '()))

(define (queue-empty? queue)
  (null? (car queue)))

(define (enqueue! queue item)
  (set-cdr! queue (cons item (cdr queue))))

(define (dequeue! queue)
  (if (queue-empty? queue)
      #f
      (let ((item (car (car queue))))
        (set-car! queue (cdr (car queue)))
        (when (null? (car queue))
          (set-car! queue (reverse (cdr queue)))
          (set-cdr! queue '()))
        item)))

;;; Global agent communication instance
(define *global-agent-communication* 
  (make-agent-communication #:protocol 'atomspace-message-passing
                            #:transport 'distributed
                            #:serialization 'atomspace-serialization))