;;; aar-core.scm - Agent-Arena-Relation Core Architecture
;;;
;;; Implements a geometric architecture for encoding a model's sense of 'self'
;;; through the continuous, dynamic interplay between Agent (urge-to-act) and
;;; Arena (need-to-be), with Relation (self) emerging from their feedback loops.
;;;
;;; This Scheme implementation provides the foundational metamodel for
;;; self-awareness in cognitive systems.

(define-module (metamodel aar-core)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (srfi srfi-43) ; vectors
  #:use-module (ice-9 match)
  #:export (
    ;; AAR Core
    make-aar-core
    aar-core?
    
    ;; Agent operations
    aar-perceive
    aar-act
    aar-update-agent
    
    ;; Arena operations
    aar-constrain
    aar-expand-arena
    aar-arena-state
    
    ;; Relation operations
    aar-reflect
    aar-coherence
    aar-emergence
    aar-self-state
    
    ;; Feedback loops
    aar-cognitive-cycle
    aar-run-cycles
    
    ;; Introspection
    aar-metrics
    aar-trajectory
  ))

;;;; ===========================================================================
;;;; 1. DATA STRUCTURES
;;;; ===========================================================================

(define-record-type <aar-core>
  (make-aar-core-internal agent arena relation history config)
  aar-core?
  (agent aar-agent set-aar-agent!)
  (arena aar-arena set-aar-arena!)
  (relation aar-relation set-aar-relation!)
  (history aar-history set-aar-history!)
  (config aar-config))

(define-record-type <agent>
  (make-agent state action-vector momentum)
  agent?
  (state agent-state set-agent-state!)
  (action-vector agent-action-vector set-agent-action-vector!)
  (momentum agent-momentum set-agent-momentum!))

(define-record-type <arena>
  (make-arena state constraints capacity)
  arena?
  (state arena-state set-arena-state!)
  (constraints arena-constraints set-arena-constraints!)
  (capacity arena-capacity set-arena-capacity!))

(define-record-type <relation>
  (make-relation coherence emergence self-representation)
  relation?
  (coherence relation-coherence set-relation-coherence!)
  (emergence relation-emergence set-relation-emergence!)
  (self-representation relation-self-representation set-relation-self-representation!))

;;;; ===========================================================================
;;;; 2. VECTOR OPERATIONS
;;;; ===========================================================================

(define (vector-dot v1 v2)
  "Compute dot product of two vectors"
  (let ((len (vector-length v1)))
    (let loop ((i 0) (sum 0))
      (if (>= i len)
          sum
          (loop (+ i 1) (+ sum (* (vector-ref v1 i) (vector-ref v2 i))))))))

(define (vector-norm v)
  "Compute Euclidean norm of vector"
  (sqrt (vector-dot v v)))

(define (vector-normalize v)
  "Normalize vector to unit length"
  (let ((norm (vector-norm v)))
    (if (< norm 1e-10)
        v
        (vector-map (lambda (i x) (/ x norm)) v))))

(define (vector-add v1 v2)
  "Add two vectors"
  (vector-map (lambda (i x) (+ x (vector-ref v2 i))) v1))

(define (vector-scale v scalar)
  "Scale vector by scalar"
  (vector-map (lambda (i x) (* x scalar)) v))

(define (vector-distance v1 v2)
  "Compute Euclidean distance between vectors"
  (vector-norm (vector-map (lambda (i x) (- x (vector-ref v2 i))) v1)))

(define (random-vector dim)
  "Create random vector of given dimension"
  (let ((v (make-vector dim)))
    (vector-map! (lambda (i x) (- (* 2 (random:uniform)) 1)) v)
    (vector-normalize v)))

;;;; ===========================================================================
;;;; 3. INITIALIZATION
;;;; ===========================================================================

(define* (make-aar-core #:key 
                        (dimension 32)
                        (learning-rate 0.01)
                        (momentum-decay 0.9)
                        (arena-capacity 1.0))
  "Create a new AAR Core with specified configuration"
  (let* ((config `((dimension . ,dimension)
                   (learning-rate . ,learning-rate)
                   (momentum-decay . ,momentum-decay)
                   (arena-capacity . ,arena-capacity)))
         (agent (make-agent
                  (random-vector dimension)  ; state
                  (random-vector dimension)  ; action-vector
                  (make-vector dimension 0))) ; momentum
         (arena (make-arena
                  (random-vector dimension)  ; state
                  '()                        ; constraints
                  arena-capacity))           ; capacity
         (relation (make-relation
                     0.0   ; coherence
                     0.0   ; emergence
                     (make-vector dimension 0)))) ; self-representation
    (make-aar-core-internal agent arena relation '() config)))

;;;; ===========================================================================
;;;; 4. AGENT OPERATIONS (Urge-to-Act)
;;;; ===========================================================================

(define (aar-perceive core perception)
  "Process perception through agent"
  (let* ((agent (aar-agent core))
         (config (aar-config core))
         (lr (assoc-ref config 'learning-rate))
         (current-state (agent-state agent))
         (perception-vec (if (vector? perception) 
                            perception 
                            (random-vector (assoc-ref config 'dimension))))
         ;; Update agent state with perception
         (new-state (vector-normalize
                      (vector-add current-state
                                  (vector-scale perception-vec lr)))))
    (set-agent-state! agent new-state)
    core))

(define (aar-act core)
  "Generate action from agent state"
  (let* ((agent (aar-agent core))
         (arena (aar-arena core))
         (agent-state (agent-state agent))
         (arena-state (arena-state arena))
         ;; Action is influenced by both agent state and arena constraints
         (action-direction (vector-normalize
                             (vector-add agent-state
                                         (vector-scale arena-state 0.3))))
         ;; Apply momentum
         (momentum (agent-momentum agent))
         (config (aar-config core))
         (decay (assoc-ref config 'momentum-decay))
         (new-momentum (vector-add (vector-scale momentum decay)
                                    (vector-scale action-direction 0.1)))
         (action (vector-normalize (vector-add action-direction new-momentum))))
    (set-agent-action-vector! agent action)
    (set-agent-momentum! agent new-momentum)
    action))

(define (aar-update-agent core delta)
  "Update agent state with delta vector"
  (let* ((agent (aar-agent core))
         (current-state (agent-state agent))
         (new-state (vector-normalize (vector-add current-state delta))))
    (set-agent-state! agent new-state)
    core))

;;;; ===========================================================================
;;;; 5. ARENA OPERATIONS (Need-to-Be)
;;;; ===========================================================================

(define (aar-constrain core constraint)
  "Add constraint to arena"
  (let* ((arena (aar-arena core))
         (constraints (arena-constraints arena))
         (new-constraints (cons constraint constraints)))
    (set-arena-constraints! arena new-constraints)
    core))

(define (aar-expand-arena core expansion)
  "Expand arena capacity"
  (let* ((arena (aar-arena core))
         (capacity (arena-capacity arena))
         (new-capacity (+ capacity expansion)))
    (set-arena-capacity! arena new-capacity)
    core))

(define (aar-arena-state core)
  "Get current arena state"
  (arena-state (aar-arena core)))

;;;; ===========================================================================
;;;; 6. RELATION OPERATIONS (Emergent Self)
;;;; ===========================================================================

(define (aar-reflect core)
  "Reflect on agent-arena interaction to update self-awareness"
  (let* ((agent (aar-agent core))
         (arena (aar-arena core))
         (relation (aar-relation core))
         (agent-state (agent-state agent))
         (arena-state (arena-state arena))
         ;; Compute coherence as normalized dot product
         (coherence (/ (+ 1 (vector-dot agent-state arena-state)) 2))
         ;; Compute emergence as distance between agent and arena
         (emergence (- 1 (/ (vector-distance agent-state arena-state)
                           (sqrt (* 2 (vector-length agent-state))))))
         ;; Self-representation emerges from agent-arena interaction
         (self-rep (vector-normalize
                     (vector-add (vector-scale agent-state coherence)
                                 (vector-scale arena-state (- 1 coherence))))))
    (set-relation-coherence! relation coherence)
    (set-relation-emergence! relation emergence)
    (set-relation-self-representation! relation self-rep)
    ;; Record in history
    (let ((history (aar-history core))
          (snapshot `((coherence . ,coherence)
                      (emergence . ,emergence)
                      (timestamp . ,(current-time)))))
      (set-aar-history! core (cons snapshot history)))
    core))

(define (aar-coherence core)
  "Get current coherence measure"
  (relation-coherence (aar-relation core)))

(define (aar-emergence core)
  "Get current emergence measure"
  (relation-emergence (aar-relation core)))

(define (aar-self-state core)
  "Get current self-representation"
  (let* ((relation (aar-relation core))
         (coherence (relation-coherence relation))
         (emergence (relation-emergence relation))
         (self-rep (relation-self-representation relation)))
    `((coherence . ,coherence)
      (emergence . ,emergence)
      (self-representation . ,self-rep))))

;;;; ===========================================================================
;;;; 7. COGNITIVE CYCLE
;;;; ===========================================================================

(define* (aar-cognitive-cycle core perception #:key (reflect? #t))
  "Execute one cognitive cycle: perceive -> act -> reflect"
  (aar-perceive core perception)
  (let ((action (aar-act core)))
    (when reflect?
      (aar-reflect core))
    action))

(define* (aar-run-cycles core n #:key (perception-generator random-vector))
  "Run n cognitive cycles with generated perceptions"
  (let ((dim (assoc-ref (aar-config core) 'dimension)))
    (let loop ((i 0))
      (when (< i n)
        (let ((perception (perception-generator dim)))
          (aar-cognitive-cycle core perception)
          (loop (+ i 1))))))
  core)

;;;; ===========================================================================
;;;; 8. INTROSPECTION AND METRICS
;;;; ===========================================================================

(define (aar-metrics core)
  "Get comprehensive metrics about AAR core state"
  (let* ((agent (aar-agent core))
         (arena (aar-arena core))
         (relation (aar-relation core))
         (history (aar-history core))
         (agent-state (agent-state agent))
         (arena-state (arena-state arena))
         (coherence (relation-coherence relation))
         (emergence (relation-emergence relation)))
    `((agent-norm . ,(vector-norm agent-state))
      (arena-norm . ,(vector-norm arena-state))
      (agent-arena-distance . ,(vector-distance agent-state arena-state))
      (coherence . ,coherence)
      (emergence . ,emergence)
      (history-length . ,(length history))
      (arena-constraints . ,(length (arena-constraints arena)))
      (arena-capacity . ,(arena-capacity arena)))))

(define (aar-trajectory core)
  "Get historical trajectory of coherence and emergence"
  (reverse (aar-history core)))

;;;; ===========================================================================
;;;; 9. DEMONSTRATION
;;;; ===========================================================================

(define (demonstrate-aar-core)
  "Demonstrate AAR Core capabilities"
  (display "=== AAR Core Demonstration ===\n\n")
  
  ;; Create AAR Core
  (display "1. Creating AAR Core (dimension=8)...\n")
  (let ((core (make-aar-core #:dimension 8 #:learning-rate 0.1)))
    
    ;; Initial state
    (display "2. Initial state:\n")
    (let ((metrics (aar-metrics core)))
      (display "   Coherence: ") (display (assoc-ref metrics 'coherence)) (newline)
      (display "   Emergence: ") (display (assoc-ref metrics 'emergence)) (newline))
    (newline)
    
    ;; Run cognitive cycles
    (display "3. Running 10 cognitive cycles...\n")
    (aar-run-cycles core 10)
    (newline)
    
    ;; Final state
    (display "4. Final state:\n")
    (let ((metrics (aar-metrics core)))
      (display "   Coherence: ") (display (assoc-ref metrics 'coherence)) (newline)
      (display "   Emergence: ") (display (assoc-ref metrics 'emergence)) (newline)
      (display "   History length: ") (display (assoc-ref metrics 'history-length)) (newline))
    (newline)
    
    ;; Self state
    (display "5. Self state:\n")
    (let ((self-state (aar-self-state core)))
      (display "   Coherence: ") (display (assoc-ref self-state 'coherence)) (newline)
      (display "   Emergence: ") (display (assoc-ref self-state 'emergence)) (newline))
    (newline))
  
  (display "=== AAR Core Demonstration Complete ===\n"))

;;; End of aar-core.scm
