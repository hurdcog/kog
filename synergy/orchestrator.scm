;;; synergy/orchestrator.scm
;;; Cognitive Synergy Orchestrator
;;; Coordinates interactions between components to enable emergent intelligence

(define-module (synergy orchestrator)
  #:use-module (ice-9 threads)
  #:use-module (ice-9 match)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (srfi srfi-18)  ;; Time
  #:export (
    ;; Orchestrator lifecycle
    orchestrator-create
    orchestrator-start
    orchestrator-stop
    orchestrator-status
    
    ;; Component registration
    register-component
    unregister-component
    list-components
    
    ;; Synergy operations
    trigger-synergy
    create-synergy-loop
    measure-emergence
    
    ;; Event handling
    subscribe-to-events
    publish-event
    
    ;; Feedback loops
    create-feedback-loop
    activate-feedback-loop
    deactivate-feedback-loop
    
    ;; Introspection
    orchestrator-introspect
    synergy-metrics
  ))

;;;; ============================================================================
;;;; Type Definitions
;;;; ============================================================================

(define-record-type <orchestrator>
  (make-orchestrator-internal components event-handlers feedback-loops running? metrics)
  orchestrator?
  (components orchestrator-components orchestrator-set-components!)
  (event-handlers orchestrator-event-handlers orchestrator-set-event-handlers!)
  (feedback-loops orchestrator-feedback-loops orchestrator-set-feedback-loops!)
  (running? orchestrator-running? orchestrator-set-running!)
  (metrics orchestrator-metrics orchestrator-set-metrics!))

(define-record-type <component>
  (make-component name type interface state capabilities)
  component?
  (name component-name)
  (type component-type)
  (interface component-interface)
  (state component-state component-set-state!)
  (capabilities component-capabilities))

(define-record-type <synergy-event>
  (make-synergy-event type source target data timestamp)
  synergy-event?
  (type event-type)
  (source event-source)
  (target event-target)
  (data event-data)
  (timestamp event-timestamp))

(define-record-type <feedback-loop>
  (make-feedback-loop id components transform-fn active?)
  feedback-loop?
  (id feedback-loop-id)
  (components feedback-loop-components)
  (transform-fn feedback-loop-transform-fn)
  (active? feedback-loop-active? feedback-loop-set-active!))

;;;; ============================================================================
;;;; Orchestrator Lifecycle
;;;; ============================================================================

(define (orchestrator-create)
  "Create a new cognitive synergy orchestrator"
  (make-orchestrator-internal
    '()           ;; components
    (make-hash-table)  ;; event-handlers
    '()           ;; feedback-loops
    #f            ;; running?
    (make-initial-metrics)))

(define (make-initial-metrics)
  "Create initial metrics structure"
  `((start-time . #f)
    (events-processed . 0)
    (synergies-triggered . 0)
    (emergent-behaviors . 0)
    (feedback-iterations . 0)
    (component-count . 0)
    (synergy-score . 0.0)))

(define (orchestrator-start orch)
  "Start the orchestrator and begin processing"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  (orchestrator-set-running! orch #t)
  (update-metric! orch 'start-time (current-time))
  (format #t "Orchestrator started at ~a~%" (current-time))
  #t)

(define (orchestrator-stop orch)
  "Stop the orchestrator"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  (orchestrator-set-running! orch #f)
  (format #t "Orchestrator stopped~%")
  #t)

(define (orchestrator-status orch)
  "Get the current status of the orchestrator"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  `((running . ,(orchestrator-running? orch))
    (components . ,(length (orchestrator-components orch)))
    (feedback-loops . ,(length (orchestrator-feedback-loops orch)))
    (metrics . ,(orchestrator-metrics orch))))

;;;; ============================================================================
;;;; Component Registration
;;;; ============================================================================

(define* (register-component orch name type #:key (interface '()) (capabilities '()))
  "Register a component with the orchestrator
   name: Symbol identifying the component
   type: Symbol indicating component type (e.g., 'atomspace, 'cogserver, 'agent)
   interface: Association list of callable functions
   capabilities: List of capability symbols"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  
  (let ((component (make-component name type interface '() capabilities)))
    (orchestrator-set-components! 
      orch 
      (cons component (orchestrator-components orch)))
    (update-metric! orch 'component-count (length (orchestrator-components orch)))
    (format #t "Registered component: ~a (~a)~%" name type)
    
    ;; Publish registration event
    (publish-event orch
      (make-synergy-event 'component-registered
                          'orchestrator
                          name
                          `((type . ,type) (capabilities . ,capabilities))
                          (current-time)))
    component))

(define (unregister-component orch name)
  "Unregister a component from the orchestrator"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  
  (orchestrator-set-components!
    orch
    (filter (lambda (c) (not (eq? (component-name c) name)))
            (orchestrator-components orch)))
  (update-metric! orch 'component-count (length (orchestrator-components orch)))
  (format #t "Unregistered component: ~a~%" name)
  #t)

(define (list-components orch)
  "List all registered components"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  (map (lambda (c)
         `((name . ,(component-name c))
           (type . ,(component-type c))
           (capabilities . ,(component-capabilities c))))
       (orchestrator-components orch)))

(define (find-component orch name)
  "Find a component by name"
  (find (lambda (c) (eq? (component-name c) name))
        (orchestrator-components orch)))

;;;; ============================================================================
;;;; Event System
;;;; ============================================================================

(define (subscribe-to-events orch event-type handler-fn)
  "Subscribe to events of a specific type
   event-type: Symbol identifying the event type
   handler-fn: Function to call when event occurs (receives event as argument)"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  
  (let ((handlers (orchestrator-event-handlers orch)))
    (hash-set! handlers event-type
               (cons handler-fn (hash-ref handlers event-type '())))
    (format #t "Subscribed to event type: ~a~%" event-type)
    #t))

(define (publish-event orch event)
  "Publish an event to all subscribers"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  (unless (synergy-event? event)
    (error "Not a synergy event" event))
  
  (let* ((handlers (orchestrator-event-handlers orch))
         (event-type (event-type event))
         (subscribers (hash-ref handlers event-type '())))
    
    (update-metric! orch 'events-processed
                    (+ 1 (get-metric orch 'events-processed)))
    
    ;; Call all subscribers
    (for-each (lambda (handler)
                (catch #t
                  (lambda () (handler event))
                  (lambda (key . args)
                    (format #t "Error in event handler: ~a ~a~%" key args))))
              subscribers)
    
    (length subscribers)))

;;;; ============================================================================
;;;; Synergy Operations
;;;; ============================================================================

(define (trigger-synergy orch source-component target-component data)
  "Trigger a synergistic interaction between two components
   Returns the result of the synergistic interaction"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  
  (let* ((source (find-component orch source-component))
         (target (find-component orch target-component)))
    
    (unless source
      (error "Source component not found" source-component))
    (unless target
      (error "Target component not found" target-component))
    
    (format #t "Triggering synergy: ~a -> ~a~%" source-component target-component)
    
    ;; Create and publish synergy event
    (let ((event (make-synergy-event 'synergy-triggered
                                     source-component
                                     target-component
                                     data
                                     (current-time))))
      (publish-event orch event)
      (update-metric! orch 'synergies-triggered
                      (+ 1 (get-metric orch 'synergies-triggered)))
      
      ;; Check for emergent behavior
      (let ((emergence (detect-emergence orch source target data)))
        (when emergence
          (update-metric! orch 'emergent-behaviors
                          (+ 1 (get-metric orch 'emergent-behaviors)))
          (format #t "  ✨ Emergent behavior detected: ~a~%" emergence))
        
        `((synergy . success)
          (source . ,source-component)
          (target . ,target-component)
          (emergence . ,emergence))))))

(define (create-synergy-loop orch components interval-ms)
  "Create a continuous synergy loop between components
   components: List of component names to include in loop
   interval-ms: Milliseconds between synergy triggers
   Returns a feedback loop ID"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  
  (let* ((loop-id (gensym "synergy-loop-"))
         (transform-fn (lambda (state)
                         ;; Rotate through components triggering synergies
                         (let* ((current-idx (or (assq-ref state 'index) 0))
                                (next-idx (modulo (+ current-idx 1) (length components)))
                                (source (list-ref components current-idx))
                                (target (list-ref components next-idx)))
                           (trigger-synergy orch source target state)
                           `((index . ,next-idx)
                             (iteration . ,(+ 1 (or (assq-ref state 'iteration) 0)))))))
         (loop (make-feedback-loop loop-id components transform-fn #f)))
    
    (orchestrator-set-feedback-loops!
      orch
      (cons loop (orchestrator-feedback-loops orch)))
    
    (format #t "Created synergy loop: ~a with ~a components~%" loop-id (length components))
    loop-id))

(define (detect-emergence orch source target data)
  "Detect emergent behavior from component interaction
   Returns a description of the emergence or #f if none detected"
  ;; Simple heuristic: emergence occurs when capabilities combine
  (let* ((source-caps (component-capabilities source))
         (target-caps (component-capabilities target))
         (combined-caps (append source-caps target-caps))
         (unique-caps (delete-duplicates combined-caps)))
    
    (if (> (length unique-caps) (max (length source-caps) (length target-caps)))
        `((type . capability-synthesis)
          (new-capabilities . ,(lset-difference eq? unique-caps source-caps target-caps))
          (synergy-score . ,(/ (length unique-caps)
                               (+ (length source-caps) (length target-caps) 0.0))))
        #f)))

(define (measure-emergence orch)
  "Measure the overall emergence in the system
   Returns a score between 0.0 and 1.0"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  
  (let* ((components (orchestrator-components orch))
         (n-components (length components))
         (n-synergies (get-metric orch 'synergies-triggered))
         (n-emergent (get-metric orch 'emergent-behaviors))
         (max-possible-synergies (* n-components (- n-components 1))))
    
    (if (zero? max-possible-synergies)
        0.0
        (let ((synergy-coverage (/ n-synergies (+ max-possible-synergies 1.0)))
              (emergence-rate (if (zero? n-synergies)
                                  0.0
                                  (/ n-emergent (+ n-synergies 1.0)))))
          (* 0.5 (+ synergy-coverage emergence-rate))))))

;;;; ============================================================================
;;;; Feedback Loops
;;;; ============================================================================

(define (create-feedback-loop orch components transform-fn)
  "Create a feedback loop connecting multiple components
   components: List of component names
   transform-fn: Function that transforms state based on component interactions
   Returns a feedback loop ID"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  
  (let* ((loop-id (gensym "feedback-loop-"))
         (loop (make-feedback-loop loop-id components transform-fn #f)))
    
    (orchestrator-set-feedback-loops!
      orch
      (cons loop (orchestrator-feedback-loops orch)))
    
    (format #t "Created feedback loop: ~a~%" loop-id)
    loop-id))

(define (activate-feedback-loop orch loop-id)
  "Activate a feedback loop"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  
  (let ((loop (find (lambda (l) (eq? (feedback-loop-id l) loop-id))
                    (orchestrator-feedback-loops orch))))
    (if loop
        (begin
          (feedback-loop-set-active! loop #t)
          (format #t "Activated feedback loop: ~a~%" loop-id)
          #t)
        (error "Feedback loop not found" loop-id))))

(define (deactivate-feedback-loop orch loop-id)
  "Deactivate a feedback loop"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  
  (let ((loop (find (lambda (l) (eq? (feedback-loop-id l) loop-id))
                    (orchestrator-feedback-loops orch))))
    (if loop
        (begin
          (feedback-loop-set-active! loop #f)
          (format #t "Deactivated feedback loop: ~a~%" loop-id)
          #t)
        (error "Feedback loop not found" loop-id))))

;;;; ============================================================================
;;;; Metrics and Introspection
;;;; ============================================================================

(define (update-metric! orch key value)
  "Update a metric value"
  (let ((metrics (orchestrator-metrics orch)))
    (orchestrator-set-metrics! orch (assq-set! metrics key value))))

(define (get-metric orch key)
  "Get a metric value"
  (assq-ref (orchestrator-metrics orch) key))

(define (orchestrator-introspect orch)
  "Introspect the orchestrator state"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  
  `((type . "CognitiveSynergyOrchestrator")
    (status . ,(if (orchestrator-running? orch) "running" "stopped"))
    (components . ,(list-components orch))
    (feedback-loops . ,(map feedback-loop-id (orchestrator-feedback-loops orch)))
    (metrics . ,(orchestrator-metrics orch))))

(define (synergy-metrics orch)
  "Compute comprehensive synergy metrics"
  (unless (orchestrator? orch)
    (error "Not an orchestrator" orch))
  
  (let ((emergence-score (measure-emergence orch)))
    (update-metric! orch 'synergy-score emergence-score)
    
    `((emergence-score . ,emergence-score)
      (events-processed . ,(get-metric orch 'events-processed))
      (synergies-triggered . ,(get-metric orch 'synergies-triggered))
      (emergent-behaviors . ,(get-metric orch 'emergent-behaviors))
      (feedback-iterations . ,(get-metric orch 'feedback-iterations))
      (component-count . ,(get-metric orch 'component-count))
      (uptime . ,(if (get-metric orch 'start-time)
                     (- (current-time) (get-metric orch 'start-time))
                     0)))))

;;;; ============================================================================
;;;; Demo and Testing
;;;; ============================================================================

(define (run-orchestrator-demo)
  "Demonstrate the cognitive synergy orchestrator"
  (format #t "~%=== Cognitive Synergy Orchestrator Demo ===~%~%")
  
  ;; Create orchestrator
  (format #t "1. Creating orchestrator...~%")
  (define orch (orchestrator-create))
  (orchestrator-start orch)
  (format #t "   Status: ~a~%~%" (orchestrator-status orch))
  
  ;; Register components
  (format #t "2. Registering components...~%")
  (register-component orch 'atomspace 'knowledge-base
                      #:capabilities '(storage reasoning inference))
  (register-component orch 'cogserver 'server
                      #:capabilities '(networking communication))
  (register-component orch 'agent-system 'agents
                      #:capabilities '(autonomy learning adaptation))
  (format #t "   Components: ~a~%~%" (list-components orch))
  
  ;; Subscribe to events
  (format #t "3. Setting up event subscriptions...~%")
  (subscribe-to-events orch 'synergy-triggered
                       (lambda (event)
                         (format #t "   📡 Event: ~a -> ~a~%"
                                 (event-source event)
                                 (event-target event))))
  (format #t "~%")
  
  ;; Trigger synergies
  (format #t "4. Triggering synergistic interactions...~%")
  (trigger-synergy orch 'atomspace 'cogserver '((data . "knowledge-query")))
  (trigger-synergy orch 'cogserver 'agent-system '((data . "agent-command")))
  (trigger-synergy orch 'agent-system 'atomspace '((data . "learning-update")))
  (format #t "~%")
  
  ;; Create synergy loop
  (format #t "5. Creating synergy loop...~%")
  (define loop-id (create-synergy-loop orch '(atomspace cogserver agent-system) 1000))
  (activate-feedback-loop orch loop-id)
  (format #t "   Loop ID: ~a~%~%" loop-id)
  
  ;; Measure emergence
  (format #t "6. Measuring emergence...~%")
  (define emergence (measure-emergence orch))
  (format #t "   Emergence score: ~a~%~%" emergence)
  
  ;; Get metrics
  (format #t "7. Synergy metrics:~%")
  (define metrics (synergy-metrics orch))
  (for-each (lambda (pair)
              (format #t "   ~a: ~a~%" (car pair) (cdr pair)))
            metrics)
  (format #t "~%")
  
  ;; Introspection
  (format #t "8. Orchestrator introspection:~%")
  (define intro (orchestrator-introspect orch))
  (format #t "   Type: ~a~%" (assq-ref intro 'type))
  (format #t "   Status: ~a~%" (assq-ref intro 'status))
  (format #t "   Components: ~a~%" (length (assq-ref intro 'components)))
  (format #t "~%")
  
  (format #t "=== Demo Complete ===~%~%")
  
  (orchestrator-stop orch))

;; Uncomment to run demo when module is loaded
;; (run-orchestrator-demo)
