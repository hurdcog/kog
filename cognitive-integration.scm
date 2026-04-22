;;; Cognitive Integration Module for OCC + HurdCog + Cognumach
;;; This module provides the core integration primitives for the AGI-OS stack
;;;
;;; Copyright (C) 2025 OpenCog Developers
;;; License: GPL-3.0+

(define-module (cognitive-integration)
  #:use-module (ice-9 threads)
  #:use-module (ice-9 match)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (srfi srfi-26)
  #:export (
    ;; Core integration
    initialize-cognitive-integration
    shutdown-cognitive-integration
    
    ;; Layer communication
    send-cognitive-message
    receive-cognitive-message
    broadcast-cognitive-event
    
    ;; AtomSpace integration
    sync-atomspace-layer
    get-distributed-atom
    set-distributed-atom
    propagate-attention
    
    ;; Cognitive event bus
    subscribe-cognitive-event
    unsubscribe-cognitive-event
    publish-cognitive-event
    
    ;; Monitoring and control
    get-cognitive-state
    get-layer-status
    get-integration-metrics
    
    ;; Self-modification
    suggest-system-modification
    apply-cognitive-change
    rollback-cognitive-change
  ))

;;;
;;; Data Structures
;;;

;; Cognitive message record
(define-record-type <cognitive-message>
  (make-cognitive-message source target type payload attention timestamp)
  cognitive-message?
  (source cognitive-message-source)
  (target cognitive-message-target)
  (type cognitive-message-type)
  (payload cognitive-message-payload)
  (attention cognitive-message-attention)
  (timestamp cognitive-message-timestamp))

;; Cognitive event record
(define-record-type <cognitive-event>
  (make-cognitive-event type source data importance timestamp)
  cognitive-event?
  (type cognitive-event-type)
  (source cognitive-event-source)
  (data cognitive-event-data)
  (importance cognitive-event-importance)
  (timestamp cognitive-event-timestamp))

;; Layer status record
(define-record-type <layer-status>
  (make-layer-status name active atomspace-size attention-total load)
  layer-status?
  (name layer-status-name)
  (active layer-status-active)
  (atomspace-size layer-status-atomspace-size)
  (attention-total layer-status-attention-total)
  (load layer-status-load))

;;;
;;; Global State
;;;

(define *cognitive-integration-active* #f)
(define *event-subscribers* '())
(define *message-queue* '())
(define *message-queue-mutex* (make-mutex))
(define *layer-registry* '())
(define *integration-metrics* '())

;;;
;;; Core Integration Functions
;;;

(define (initialize-cognitive-integration)
  "Initialize the cognitive integration system."
  (unless *cognitive-integration-active*
    (set! *cognitive-integration-active* #t)
    (set! *event-subscribers* '())
    (set! *message-queue* '())
    (set! *layer-registry* '())
    (set! *integration-metrics* '())
    
    ;; Register the three layers
    (register-layer 'cognumach "Cognumach Microkernel" 0)
    (register-layer 'hurdcog "HurdCog Operating System" 1)
    (register-layer 'occ "OpenCog Collection" 2)
    
    ;; Start message processing thread
    (call-with-new-thread
      (lambda ()
        (message-processing-loop)))
    
    ;; Start metrics collection thread
    (call-with-new-thread
      (lambda ()
        (metrics-collection-loop)))
    
    (format #t "Cognitive integration initialized~%")
    #t))

(define (shutdown-cognitive-integration)
  "Shutdown the cognitive integration system."
  (when *cognitive-integration-active*
    (set! *cognitive-integration-active* #f)
    (set! *event-subscribers* '())
    (set! *message-queue* '())
    (format #t "Cognitive integration shutdown~%")
    #t))

(define (register-layer name description level)
  "Register a cognitive layer in the system."
  (set! *layer-registry*
    (assoc-set! *layer-registry* name
      (make-layer-status name #t 0 0.0 0.0)))
  (format #t "Registered layer: ~a (~a) at level ~a~%" name description level))

;;;
;;; Layer Communication
;;;

(define (send-cognitive-message source target type payload attention)
  "Send a cognitive message from one layer to another."
  (let ((msg (make-cognitive-message
               source
               target
               type
               payload
               attention
               (current-time))))
    (with-mutex *message-queue-mutex*
      (set! *message-queue* (append *message-queue* (list msg))))
    msg))

(define (receive-cognitive-message target)
  "Receive cognitive messages for a specific target layer."
  (with-mutex *message-queue-mutex*
    (let ((messages (filter
                      (lambda (msg)
                        (eq? (cognitive-message-target msg) target))
                      *message-queue*)))
      (set! *message-queue*
        (filter
          (lambda (msg)
            (not (eq? (cognitive-message-target msg) target)))
          *message-queue*))
      messages)))

(define (broadcast-cognitive-event type data importance)
  "Broadcast a cognitive event to all layers."
  (let ((event (make-cognitive-event
                 type
                 'system
                 data
                 importance
                 (current-time))))
    (for-each
      (lambda (subscriber)
        (let ((callback (cdr subscriber)))
          (catch #t
            (lambda () (callback event))
            (lambda (key . args)
              (format #t "Error in event subscriber: ~a~%" key)))))
      (filter
        (lambda (sub) (eq? (car sub) type))
        *event-subscribers*))
    event))

;;;
;;; AtomSpace Integration
;;;

(define (sync-atomspace-layer source-layer target-layer min-attention)
  "Synchronize atoms between layers based on attention threshold."
  (format #t "Syncing AtomSpace from ~a to ~a (min attention: ~a)~%"
    source-layer target-layer min-attention)
  
  ;; This is a placeholder for the actual AtomSpace sync logic
  ;; In a real implementation, this would:
  ;; 1. Query source layer AtomSpace for high-attention atoms
  ;; 2. Transfer atoms to target layer
  ;; 3. Handle conflicts and merging
  ;; 4. Update attention values
  
  (let ((synced-count 0))
    (format #t "Synced ~a atoms from ~a to ~a~%"
      synced-count source-layer target-layer)
    synced-count))

(define (get-distributed-atom atom-id layer)
  "Get an atom from a specific layer's AtomSpace."
  ;; Placeholder for distributed AtomSpace access
  (format #t "Fetching atom ~a from layer ~a~%" atom-id layer)
  #f)

(define (set-distributed-atom atom-id layer atom-data)
  "Set an atom in a specific layer's AtomSpace."
  ;; Placeholder for distributed AtomSpace modification
  (format #t "Setting atom ~a in layer ~a~%" atom-id layer)
  #t)

(define (propagate-attention atom-id attention-value)
  "Propagate attention value for an atom across all layers."
  (format #t "Propagating attention ~a for atom ~a~%"
    attention-value atom-id)
  
  ;; Broadcast attention change event
  (broadcast-cognitive-event
    'attention-change
    (list atom-id attention-value)
    attention-value)
  
  ;; Sync to layers if attention is high enough
  (when (> attention-value 0.7)
    (sync-atomspace-layer 'occ 'hurdcog attention-value)
    (when (> attention-value 0.9)
      (sync-atomspace-layer 'hurdcog 'cognumach attention-value)))
  
  #t)

;;;
;;; Cognitive Event Bus
;;;

(define (subscribe-cognitive-event event-type callback)
  "Subscribe to cognitive events of a specific type."
  (set! *event-subscribers*
    (cons (cons event-type callback) *event-subscribers*))
  (format #t "Subscribed to event type: ~a~%" event-type)
  #t)

(define (unsubscribe-cognitive-event event-type callback)
  "Unsubscribe from cognitive events."
  (set! *event-subscribers*
    (filter
      (lambda (sub)
        (not (and (eq? (car sub) event-type)
                  (eq? (cdr sub) callback))))
      *event-subscribers*))
  #t)

(define (publish-cognitive-event event-type data importance)
  "Publish a cognitive event to all subscribers."
  (broadcast-cognitive-event event-type data importance))

;;;
;;; Monitoring and Control
;;;

(define (get-cognitive-state)
  "Get the current cognitive state of the entire system."
  (list
    (cons 'active *cognitive-integration-active*)
    (cons 'layers (map car *layer-registry*))
    (cons 'message-queue-size (length *message-queue*))
    (cons 'event-subscribers (length *event-subscribers*))
    (cons 'metrics *integration-metrics*)))

(define (get-layer-status layer-name)
  "Get the status of a specific layer."
  (assoc-ref *layer-registry* layer-name))

(define (get-integration-metrics)
  "Get integration performance metrics."
  *integration-metrics*)

;;;
;;; Self-Modification
;;;

(define (suggest-system-modification modification-type description importance)
  "Suggest a modification to the system."
  (format #t "System modification suggested: ~a~%" description)
  (format #t "  Type: ~a, Importance: ~a~%" modification-type importance)
  
  ;; Publish event for consideration
  (publish-cognitive-event
    'modification-suggested
    (list modification-type description importance)
    importance)
  
  ;; Return modification ID for tracking
  (string-append "mod-" (number->string (current-time))))

(define (apply-cognitive-change change-id)
  "Apply a cognitive change to the system."
  (format #t "Applying cognitive change: ~a~%" change-id)
  
  ;; Placeholder for actual change application
  ;; In a real implementation, this would:
  ;; 1. Validate the change
  ;; 2. Create a rollback point
  ;; 3. Apply the change
  ;; 4. Monitor for issues
  
  (publish-cognitive-event
    'change-applied
    (list change-id)
    0.8)
  
  #t)

(define (rollback-cognitive-change change-id)
  "Rollback a cognitive change."
  (format #t "Rolling back cognitive change: ~a~%" change-id)
  
  (publish-cognitive-event
    'change-rolled-back
    (list change-id)
    0.9)
  
  #t)

;;;
;;; Internal Helper Functions
;;;

(define (message-processing-loop)
  "Background thread for processing cognitive messages."
  (while *cognitive-integration-active*
    (sleep 1)
    
    ;; Process high-priority messages
    (let ((messages (with-mutex *message-queue-mutex*
                      (take *message-queue*
                        (min 10 (length *message-queue*))))))
      (for-each process-cognitive-message messages))
    
    ;; Update metrics
    (update-message-metrics)))

(define (process-cognitive-message msg)
  "Process a single cognitive message."
  (match (cognitive-message-type msg)
    ('query
      (handle-query-message msg))
    ('command
      (handle-command-message msg))
    ('sync
      (handle-sync-message msg))
    ('event
      (handle-event-message msg))
    (_
      (format #t "Unknown message type: ~a~%"
        (cognitive-message-type msg)))))

(define (handle-query-message msg)
  "Handle a query message."
  (format #t "Processing query from ~a to ~a~%"
    (cognitive-message-source msg)
    (cognitive-message-target msg)))

(define (handle-command-message msg)
  "Handle a command message."
  (format #t "Processing command from ~a to ~a~%"
    (cognitive-message-source msg)
    (cognitive-message-target msg)))

(define (handle-sync-message msg)
  "Handle a synchronization message."
  (format #t "Processing sync from ~a to ~a~%"
    (cognitive-message-source msg)
    (cognitive-message-target msg)))

(define (handle-event-message msg)
  "Handle an event message."
  (format #t "Processing event from ~a to ~a~%"
    (cognitive-message-source msg)
    (cognitive-message-target msg)))

(define (metrics-collection-loop)
  "Background thread for collecting integration metrics."
  (while *cognitive-integration-active*
    (sleep 5)
    (collect-integration-metrics)))

(define (collect-integration-metrics)
  "Collect current integration metrics."
  (set! *integration-metrics*
    (list
      (cons 'timestamp (current-time))
      (cons 'message-queue-size (length *message-queue*))
      (cons 'event-subscribers (length *event-subscribers*))
      (cons 'active-layers
        (length (filter
                  (lambda (entry)
                    (layer-status-active (cdr entry)))
                  *layer-registry*))))))

(define (update-message-metrics)
  "Update message processing metrics."
  ;; Placeholder for metrics update
  #t)

;;;
;;; Utility Functions
;;;

(define (current-time)
  "Get current Unix timestamp."
  (car (gettimeofday)))

;;;
;;; Module Initialization
;;;

;; Auto-initialize when module is loaded
(format #t "Cognitive integration module loaded~%")
(format #t "Call (initialize-cognitive-integration) to start~%")

;;; End of cognitive-integration.scm
