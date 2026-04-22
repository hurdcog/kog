;;; enterprise_autogenesis.scm - Enterprise Autogenesis Model
;;;
;;; Based on the Enterprise Product Development Framework depicting:
;;; - Six stages of product/value creation
;;; - Multiple feedback loops for continuous learning
;;; - End-to-end thin slice delivery pattern
;;; - Control through learning loops (vs linear command-and-control)
;;;
;;; This module implements an organizational autogenesis framework that
;;; enables self-organizing, adaptive enterprise systems capable of
;;; generating their own operational rules and evolving autonomously.
;;;
;;; Maps to Schwarz metasystem theory:
;;; - Six stages align with the six cycles
;;; - Feedback loops implement horizontal/vertical cycle dynamics
;;; - Thin slices enable incremental autopoietic production
;;; - Learning loops drive autogenesis (self-creation of rules)

(define-module (metamodel enterprise-autogenesis)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (ice-9 match)
  #:export (
    ;; Enterprise stages
    make-enterprise-stage
    enterprise-stage?
    enterprise-stage-name
    enterprise-stage-number

    ;; Vision-Goal-Bet structure
    make-vision-goal-bet
    vision-goal-bet?
    vgb-vision
    vgb-add-goal
    vgb-add-bet
    vgb-validate-bet

    ;; Thin slices
    make-thin-slice
    thin-slice?
    thin-slice-complete-stage
    thin-slice-is-end-to-end?
    thin-slice-measure-outcome

    ;; Learning loops
    make-learning-loop
    learning-loop?
    learning-loop-step

    ;; Autonomous pods
    make-autonomous-pod
    autonomous-pod?
    pod-assign-slice
    pod-deliver-increment

    ;; Enterprise autogenesis system
    make-enterprise-autogenesis
    enterprise-autogenesis?
    enterprise-initialize
    enterprise-create-thin-slice
    enterprise-create-pod
    enterprise-step
    enterprise-evolve
    enterprise-reflect
    enterprise-state
    enterprise-metasystem-mapping

    ;; Demonstration
    demonstrate-enterprise-autogenesis
  ))


;;;; ===========================================================================
;;;; 1. ENTERPRISE STAGES
;;;; ===========================================================================

;;; The six stages of enterprise autogenesis
(define stage-definitions
  #(;; Stage 1: Executive Vision (Goal Based)
    ((number . 1)
     (name . executive-vision)
     (description . "Goal-based vision setting")
     (plane . totality)
     (cycle . self-referential))

    ;; Stage 2: Product Strategy
    ((number . 2)
     (name . product-strategy)
     (description . "Own the problem / Shape the approach")
     (plane . totality)
     (cycle . autogenesis))

    ;; Stage 3: Portfolio Management
    ((number . 3)
     (name . portfolio-management)
     (description . "Value-based investment")
     (plane . information)
     (cycle . retroaction))

    ;; Stage 4: Product Architecture
    ((number . 4)
     (name . product-architecture)
     (description . "Actionable thin slice")
     (plane . information)
     (cycle . morphogenesis))

    ;; Stage 5: Lean Delivery
    ((number . 5)
     (name . lean-delivery)
     (description . "Autonomous pods, iterative creation")
     (plane . energy)
     (cycle . autopoiesis))

    ;; Stage 6: Measure Value
    ((number . 6)
     (name . measure-value)
     (description . "Outcomes not output")
     (plane . energy)
     (cycle . vortex))))

(define-record-type <enterprise-stage>
  (make-enterprise-stage-internal number name description active throughput backlog completed)
  enterprise-stage?
  (number stage-number)
  (name enterprise-stage-name)
  (description stage-description)
  (active stage-active? set-stage-active!)
  (throughput stage-throughput set-stage-throughput!)
  (backlog stage-backlog set-stage-backlog!)
  (completed stage-completed set-stage-completed!))

(define (make-enterprise-stage number)
  "Create an enterprise stage by number (1-6)"
  (let ((def (vector-ref stage-definitions (- number 1))))
    (make-enterprise-stage-internal
      (assoc-ref def 'number)
      (assoc-ref def 'name)
      (assoc-ref def 'description)
      #f      ; active
      0.0     ; throughput
      '()     ; backlog
      '())))  ; completed

(define (stage-add-to-backlog stage item)
  "Add item to stage backlog"
  (set-stage-backlog! stage (cons item (stage-backlog stage))))

(define (stage-process stage capacity)
  "Process items through the stage"
  (let* ((backlog (stage-backlog stage))
         (to-process (max 1 (inexact->exact (floor (* capacity (length backlog))))))
         (processed (take backlog (min to-process (length backlog))))
         (remaining (drop backlog (min to-process (length backlog)))))
    (set-stage-backlog! stage remaining)
    (set-stage-completed! stage (append processed (stage-completed stage)))
    (set-stage-throughput! stage
      (/ (length processed) (max 1 (+ (length processed) (length remaining)))))
    `((processed . ,(length processed))
      (remaining . ,(length remaining))
      (throughput . ,(stage-throughput stage)))))


;;;; ===========================================================================
;;;; 2. VISION-GOAL-BET STRUCTURE
;;;; ===========================================================================

(define-record-type <vision-goal-bet>
  (make-vision-goal-bet-internal vision goals bets)
  vision-goal-bet?
  (vision vgb-vision set-vgb-vision!)
  (goals vgb-goals set-vgb-goals!)
  (bets vgb-bets set-vgb-bets!))

(define (make-vision-goal-bet vision)
  "Create a vision-goal-bet structure"
  (make-vision-goal-bet-internal vision '() '()))

(define (vgb-add-goal vgb goal)
  "Add a goal aligned with vision"
  (set-vgb-goals! vgb (cons goal (vgb-goals vgb))))

(define (vgb-add-bet vgb goal-index bet confidence)
  "Add a bet (hypothesis) under a goal"
  (set-vgb-bets! vgb
    (cons `((goal-index . ,goal-index)
            (bet . ,bet)
            (confidence . ,confidence)
            (validated . #f))
          (vgb-bets vgb))))

(define (vgb-validate-bet vgb bet-index outcome)
  "Validate a bet based on outcome"
  (let ((bets (vgb-bets vgb)))
    (when (and (>= bet-index 0) (< bet-index (length bets)))
      (let ((bet (list-ref bets bet-index)))
        (set-vgb-bets! vgb
          (list-set bets bet-index
            (append bet `((validated . #t) (outcome . ,outcome)))))))))


;;;; ===========================================================================
;;;; 3. THIN SLICES
;;;; ===========================================================================

(define-record-type <thin-slice>
  (make-thin-slice-internal id description stages-completed value-delivered outcomes)
  thin-slice?
  (id thin-slice-id)
  (description thin-slice-description)
  (stages-completed thin-slice-stages-completed set-thin-slice-stages-completed!)
  (value-delivered thin-slice-value-delivered set-thin-slice-value-delivered!)
  (outcomes thin-slice-outcomes set-thin-slice-outcomes!))

(define (make-thin-slice id description)
  "Create a new thin slice for incremental delivery"
  (make-thin-slice-internal id description '() 0.0 '()))

(define (thin-slice-complete-stage slice stage-number)
  "Mark a stage as completed for this slice"
  (let ((completed (thin-slice-stages-completed slice)))
    (unless (member stage-number completed)
      (set-thin-slice-stages-completed! slice (cons stage-number completed)))))

(define (thin-slice-is-end-to-end? slice)
  "Check if slice passes through all stages"
  (= (length (thin-slice-stages-completed slice)) 6))

(define (thin-slice-measure-outcome slice metric value)
  "Record an outcome measurement"
  (let ((outcomes (thin-slice-outcomes slice)))
    (set-thin-slice-outcomes! slice
      (cons `((metric . ,metric)
              (value . ,value)
              (stages . ,(length (thin-slice-stages-completed slice))))
            outcomes))
    (set-thin-slice-value-delivered! slice
      (/ (apply + (map (lambda (o) (assoc-ref o 'value)) (thin-slice-outcomes slice)))
         (length (thin-slice-outcomes slice))))))


;;;; ===========================================================================
;;;; 4. LEARNING LOOPS
;;;; ===========================================================================

;;; Loop types
(define loop-types
  '(;; Strategic loops (outer)
    enterprise-vision-market-opportunity
    continuous-visioning
    cross-enterprise

    ;; Tactical loops (middle)
    product-opportunity-performance
    product-strategy-architecture
    product-architecture-market-performance

    ;; Operational loops (inner)
    design-research
    agile-development
    design-delivery))

(define-record-type <learning-loop>
  (make-learning-loop-internal name loop-type phase iterations learnings)
  learning-loop?
  (name learning-loop-name)
  (loop-type learning-loop-type)
  (phase learning-loop-phase set-learning-loop-phase!)
  (iterations learning-loop-iterations set-learning-loop-iterations!)
  (learnings learning-loop-learnings set-learning-loop-learnings!))

(define (make-learning-loop name loop-type)
  "Create a learning loop: Idea -> Test -> Learn -> Outcome"
  (make-learning-loop-internal name loop-type 'idea 0 '()))

(define (learning-loop-step loop input-data)
  "Execute one step of the learning loop"
  (set-learning-loop-iterations! loop (+ 1 (learning-loop-iterations loop)))
  (let ((phase (learning-loop-phase loop)))
    (case phase
      ((idea)
       (set-learning-loop-phase! loop 'test)
       `((phase . idea)
         (hypothesis . ,(or (assoc-ref input-data 'hypothesis) "default"))
         (ready-for-test . #t)))

      ((test)
       (set-learning-loop-phase! loop 'learn)
       `((phase . test)
         (test-result . ,(or (assoc-ref input-data 'test-result) 0.5))
         (ready-for-learn . #t)))

      ((learn)
       (let ((learning `((iteration . ,(learning-loop-iterations loop))
                         (insight . ,(or (assoc-ref input-data 'insight) "no-insight"))
                         (confidence . ,(or (assoc-ref input-data 'confidence) 0.5)))))
         (set-learning-loop-learnings! loop
           (cons learning (learning-loop-learnings loop)))
         (set-learning-loop-phase! loop 'outcome)
         `((phase . learn)
           (learning . ,learning)
           (ready-for-outcome . #t))))

      ((outcome)
       (set-learning-loop-phase! loop 'idea)
       `((phase . outcome)
         (outcome-value . ,(or (assoc-ref input-data 'outcome-value) 0.5))
         (cycle-complete . #t)
         (total-learnings . ,(length (learning-loop-learnings loop)))))

      (else
       (set-learning-loop-phase! loop 'idea)
       '((phase . unknown))))))


;;;; ===========================================================================
;;;; 5. AUTONOMOUS PODS
;;;; ===========================================================================

(define-record-type <autonomous-pod>
  (make-autonomous-pod-internal id name capabilities active-slices velocity autonomy-level)
  autonomous-pod?
  (id pod-id)
  (name pod-name)
  (capabilities pod-capabilities)
  (active-slices pod-active-slices set-pod-active-slices!)
  (velocity pod-velocity set-pod-velocity!)
  (autonomy-level pod-autonomy-level set-pod-autonomy-level!))

(define* (make-autonomous-pod id name #:key (capabilities '()))
  "Create an autonomous delivery pod"
  (make-autonomous-pod-internal id name capabilities '() 0.5 0.5))

(define (pod-assign-slice pod slice)
  "Assign a thin slice to this pod"
  (set-pod-active-slices! pod (cons slice (pod-active-slices pod))))

(define (pod-deliver-increment pod)
  "Deliver an increment of value"
  (let ((slices (pod-active-slices pod)))
    (if (null? slices)
        '((delivered . #f) (reason . no-active-slices))
        (let* ((current (car slices))
               (completed (thin-slice-stages-completed current))
               (remaining (filter (lambda (n) (not (member n completed))) '(1 2 3 4 5 6))))
          (if (null? remaining)
              (begin
                (set-pod-active-slices! pod (cdr slices))
                `((delivered . #t)
                  (slice-complete . #t)
                  (slice-id . ,(thin-slice-id current))
                  (value . ,(thin-slice-value-delivered current))))
              (begin
                (thin-slice-complete-stage current (car remaining))
                `((delivered . #t)
                  (stage-complete . ,(car remaining))
                  (slice-id . ,(thin-slice-id current))
                  (progress . ,(/ (length (thin-slice-stages-completed current)) 6)))))))))


;;;; ===========================================================================
;;;; 6. ENTERPRISE AUTOGENESIS SYSTEM
;;;; ===========================================================================

(define-record-type <enterprise-autogenesis>
  (make-enterprise-autogenesis-internal vision stages loops pods thin-slices
                                         control-mode coherence emergence autonomy
                                         value-throughput cycle-count history)
  enterprise-autogenesis?
  (vision enterprise-vision set-enterprise-vision!)
  (stages enterprise-stages set-enterprise-stages!)
  (loops enterprise-loops set-enterprise-loops!)
  (pods enterprise-pods set-enterprise-pods!)
  (thin-slices enterprise-thin-slices set-enterprise-thin-slices!)
  (control-mode enterprise-control-mode set-enterprise-control-mode!)
  (coherence enterprise-coherence set-enterprise-coherence!)
  (emergence enterprise-emergence set-enterprise-emergence!)
  (autonomy enterprise-autonomy set-enterprise-autonomy!)
  (value-throughput enterprise-value-throughput set-enterprise-value-throughput!)
  (cycle-count enterprise-cycle-count set-enterprise-cycle-count!)
  (history enterprise-history set-enterprise-history!))

(define (make-enterprise-autogenesis)
  "Create an enterprise autogenesis system"
  (let ((stages (map make-enterprise-stage '(1 2 3 4 5 6)))
        (loops (map (lambda (lt) (make-learning-loop (symbol->string lt) lt)) loop-types)))
    (make-enterprise-autogenesis-internal
      (make-vision-goal-bet "")  ; vision
      stages                      ; stages
      loops                       ; loops
      '()                         ; pods
      '()                         ; thin-slices
      'learning-loops             ; control-mode
      0.0                         ; coherence
      0.0                         ; emergence
      0.0                         ; autonomy
      0.0                         ; value-throughput
      0                           ; cycle-count
      '())))                      ; history

(define* (enterprise-initialize enterprise vision #:key (goals '()))
  "Initialize the enterprise autogenesis system"
  (let ((vgb (make-vision-goal-bet vision)))
    (for-each (lambda (g) (vgb-add-goal vgb g)) goals)
    (set-enterprise-vision! enterprise vgb)
    ;; Activate first stage
    (set-stage-active! (car (enterprise-stages enterprise)) #t)
    `((initialized . #t)
      (vision . ,vision)
      (goals . ,(length goals))
      (stages-ready . ,(length (enterprise-stages enterprise))))))

(define (enterprise-create-thin-slice enterprise id description)
  "Create a new thin slice for incremental delivery"
  (let ((slice (make-thin-slice id description)))
    (set-enterprise-thin-slices! enterprise
      (cons slice (enterprise-thin-slices enterprise)))
    ;; Add to first stage backlog
    (stage-add-to-backlog (car (enterprise-stages enterprise))
      `((slice-id . ,id) (type . thin-slice)))
    slice))

(define* (enterprise-create-pod enterprise id name #:key (capabilities '()))
  "Create an autonomous delivery pod"
  (let ((pod (make-autonomous-pod id name #:capabilities capabilities)))
    (set-enterprise-pods! enterprise (cons pod (enterprise-pods enterprise)))
    pod))

(define (enterprise-step enterprise input-value)
  "Execute one enterprise autogenesis step"
  (set-enterprise-cycle-count! enterprise (+ 1 (enterprise-cycle-count enterprise)))
  (let ((result `((cycle . ,(enterprise-cycle-count enterprise)))))

    ;; 1. Process stages (pipeline flow)
    (let ((stage-results '()))
      (for-each
        (lambda (stage)
          (let ((stage-result (stage-process stage input-value)))
            (set! stage-results
              (cons `(,(stage-number stage) . ,stage-result) stage-results))))
        (enterprise-stages enterprise))
      (set! result (cons `(stages . ,(reverse stage-results)) result)))

    ;; 2. Execute feedback loops
    (let ((loop-results '()))
      (for-each
        (lambda (loop)
          (let* ((loop-input `((hypothesis . ,(string-append "cycle-"
                                 (number->string (enterprise-cycle-count enterprise))))
                               (test-result . ,input-value)
                               (insight . ,(string-append "learning-"
                                 (number->string (learning-loop-iterations loop))))
                               (confidence . ,input-value)
                               (outcome-value . ,input-value)))
                 (loop-result (learning-loop-step loop loop-input)))
            (set! loop-results
              (cons `(,(learning-loop-type loop) . ,loop-result) loop-results))))
        (enterprise-loops enterprise))
      (set! result (cons `(loops . ,(reverse loop-results)) result)))

    ;; 3. Pod delivery
    (let ((pod-results '()))
      (for-each
        (lambda (pod)
          (let ((pod-result (pod-deliver-increment pod)))
            (set! pod-results
              (cons `((pod-id . ,(pod-id pod)) (result . ,pod-result)) pod-results))))
        (enterprise-pods enterprise))
      (set! result (cons `(pods . ,(reverse pod-results)) result)))

    ;; 4. Update metrics
    (enterprise-update-metrics enterprise result)
    (set! result (cons `(metrics . ((coherence . ,(enterprise-coherence enterprise))
                                    (emergence . ,(enterprise-emergence enterprise))
                                    (autonomy . ,(enterprise-autonomy enterprise))
                                    (value-throughput . ,(enterprise-value-throughput enterprise))))
                       result))

    ;; Record history
    (set-enterprise-history! enterprise
      (cons result (enterprise-history enterprise)))

    (reverse result)))

(define (enterprise-update-metrics enterprise step-result)
  "Update enterprise metrics"
  (let* ((vision (enterprise-vision enterprise))
         (stages (enterprise-stages enterprise))
         (loops (enterprise-loops enterprise))
         (pods (enterprise-pods enterprise))
         (thin-slices (enterprise-thin-slices enterprise)))

    ;; Coherence: alignment between vision and delivery
    (let* ((vision-clarity (if (> (string-length (vgb-vision vision)) 0) 1.0 0.0))
           (goal-alignment (/ (length (vgb-goals vision))
                             (max 1 (+ (length (vgb-goals vision)) 3))))
           (stage-flow (/ (apply + (map stage-throughput stages))
                         (length stages))))
      (set-enterprise-coherence! enterprise
        (+ (* vision-clarity 0.3) (* goal-alignment 0.3) (* stage-flow 0.4))))

    ;; Emergence: new value and learnings
    (let* ((total-learnings (apply + (map (lambda (l) (length (learning-loop-learnings l))) loops)))
           (completed-slices (length (filter thin-slice-is-end-to-end? thin-slices))))
      (set-enterprise-emergence! enterprise
        (min 1.0 (+ (* total-learnings 0.05) (* completed-slices 0.1)))))

    ;; Autonomy: pod self-organization
    (let* ((avg-pod-autonomy (if (null? pods) 0.0
                                 (/ (apply + (map pod-autonomy-level pods)) (length pods))))
           (loop-maturity (/ (apply + (map learning-loop-iterations loops))
                            (* (length loops) 100))))
      (set-enterprise-autonomy! enterprise
        (+ (* avg-pod-autonomy 0.6) (* (min 1.0 loop-maturity) 0.4))))

    ;; Value throughput
    (if (null? thin-slices)
        (set-enterprise-value-throughput! enterprise 0.0)
        (set-enterprise-value-throughput! enterprise
          (/ (apply + (map thin-slice-value-delivered thin-slices))
             (length thin-slices))))))

(define* (enterprise-evolve enterprise steps #:key (input-generator #f))
  "Evolve the enterprise system for multiple steps"
  (let ((gen (or input-generator
                 (lambda (i) (+ 0.5 (* 0.3 (sin (* i 0.1))))))))
    (let loop ((i 0) (results '()))
      (if (>= i steps)
          (reverse results)
          (let* ((input-value (gen i))
                 (result (enterprise-step enterprise input-value)))
            (loop (+ i 1) (cons result results)))))))

(define (enterprise-reflect enterprise)
  "Enterprise self-reflection and diagnosis"
  (let ((diagnosis '())
        (recommendations '())
        (vision (enterprise-vision enterprise))
        (stages (enterprise-stages enterprise))
        (loops (enterprise-loops enterprise))
        (pods (enterprise-pods enterprise)))

    ;; Vision assessment
    (when (= (string-length (vgb-vision vision)) 0)
      (set! diagnosis (cons "No vision defined" diagnosis))
      (set! recommendations (cons "Establish clear executive vision" recommendations)))

    (when (and (> (string-length (vgb-vision vision)) 0) (null? (vgb-goals vision)))
      (set! diagnosis (cons "Vision lacks goals" diagnosis))
      (set! recommendations (cons "Define measurable goals aligned with vision" recommendations)))

    ;; Stage flow assessment
    (let ((bottlenecks (filter-map
                         (lambda (s)
                           (if (> (length (stage-backlog s)) 10)
                               (enterprise-stage-name s)
                               #f))
                         stages)))
      (unless (null? bottlenecks)
        (set! diagnosis (cons (format #f "Bottlenecks at: ~a" bottlenecks) diagnosis))
        (set! recommendations (cons "Increase capacity at bottleneck stages" recommendations))))

    ;; Delivery assessment
    (when (null? pods)
      (set! diagnosis (cons "No autonomous pods" diagnosis))
      (set! recommendations (cons "Create autonomous delivery pods" recommendations)))

    ;; Coherence/emergence/autonomy
    (when (< (enterprise-coherence enterprise) 0.3)
      (set! diagnosis (cons "Low coherence - vision-delivery misalignment" diagnosis))
      (set! recommendations (cons "Strengthen vision-to-delivery connection" recommendations)))

    (when (< (enterprise-emergence enterprise) 0.3)
      (set! diagnosis (cons "Low emergence - insufficient innovation" diagnosis))
      (set! recommendations (cons "Create more thin slices and run experiments" recommendations)))

    (when (< (enterprise-autonomy enterprise) 0.3)
      (set! diagnosis (cons "Low autonomy - command-control patterns" diagnosis))
      (set! recommendations (cons "Empower pods with decision authority" recommendations)))

    (when (null? diagnosis)
      (set! diagnosis '("Enterprise operating normally")))

    `((diagnosis . ,(reverse diagnosis))
      (recommendations . ,(reverse recommendations))
      (metrics . ((coherence . ,(enterprise-coherence enterprise))
                  (emergence . ,(enterprise-emergence enterprise))
                  (autonomy . ,(enterprise-autonomy enterprise))
                  (value-throughput . ,(enterprise-value-throughput enterprise))))
      (health . ((vision-defined . ,(> (string-length (vgb-vision vision)) 0))
                 (goals-count . ,(length (vgb-goals vision)))
                 (pods-count . ,(length pods))
                 (active-slices . ,(apply + (map (lambda (p) (length (pod-active-slices p))) pods)))
                 (total-learnings . ,(apply + (map (lambda (l) (length (learning-loop-learnings l))) loops))))))))

(define (enterprise-state enterprise)
  "Get complete enterprise state"
  `((cycle-count . ,(enterprise-cycle-count enterprise))
    (control-mode . ,(enterprise-control-mode enterprise))
    (vision . ((vision . ,(vgb-vision (enterprise-vision enterprise)))
               (goals . ,(vgb-goals (enterprise-vision enterprise)))
               (bets . ,(length (vgb-bets (enterprise-vision enterprise))))))
    (stages . ,(map (lambda (s)
                      `((number . ,(stage-number s))
                        (name . ,(enterprise-stage-name s))
                        (active . ,(stage-active? s))
                        (backlog . ,(length (stage-backlog s)))
                        (completed . ,(length (stage-completed s)))
                        (throughput . ,(stage-throughput s))))
                    (enterprise-stages enterprise)))
    (delivery . ((pods . ,(length (enterprise-pods enterprise)))
                 (thin-slices . ,(length (enterprise-thin-slices enterprise)))
                 (completed-slices . ,(length (filter thin-slice-is-end-to-end?
                                                (enterprise-thin-slices enterprise))))))
    (metrics . ((coherence . ,(enterprise-coherence enterprise))
                (emergence . ,(enterprise-emergence enterprise))
                (autonomy . ,(enterprise-autonomy enterprise))
                (value-throughput . ,(enterprise-value-throughput enterprise))))))

(define (enterprise-metasystem-mapping enterprise)
  "Map enterprise autogenesis to Schwarz metasystem concepts"
  `((planes
      . ((totality . ((stages . (executive-vision product-strategy))
                      (function . "Vision, identity, and strategic direction")))
         (information . ((stages . (portfolio-management product-architecture))
                         (function . "Portfolio optimization and architectural patterns")))
         (energy . ((stages . (lean-delivery measure-value))
                    (function . "Value delivery and outcome measurement")))))
    (cycles
      . ((self-referential . ((enterprise-stage . executive-vision)
                              (loops . (continuous-visioning))
                              (function . "Vision-reality dialogue")))
         (autogenesis . ((enterprise-stage . product-strategy)
                         (loops . (enterprise-vision-market-opportunity))
                         (function . "Self-creation of strategic rules")))
         (retroaction . ((enterprise-stage . portfolio-management)
                         (loops . (product-opportunity-performance))
                         (function . "Investment homeostasis")))
         (morphogenesis . ((enterprise-stage . product-architecture)
                           (loops . (product-strategy-architecture))
                           (function . "Emergence of new structures")))
         (autopoiesis . ((enterprise-stage . lean-delivery)
                         (loops . (agile-development design-research))
                         (function . "Self-production through pods")))
         (vortex . ((enterprise-stage . measure-value)
                    (loops . (cross-enterprise))
                    (function . "Value cycling and feedback")))))
    (thin-slice-as-autopoietic-unit
      . "Each thin slice is an autopoietic unit that produces value through all stages")
    (learning-loop-as-autogenesis
      . "Learning loops implement autogenesis by creating new rules from experiential feedback")))


;;;; ===========================================================================
;;;; 7. DEMONSTRATION
;;;; ===========================================================================

(define (demonstrate-enterprise-autogenesis)
  "Demonstrate the enterprise autogenesis system"
  (display "=== Enterprise Autogenesis Demonstration ===\n\n")

  ;; Create system
  (let ((enterprise (make-enterprise-autogenesis)))

    ;; Initialize with vision
    (display "1. Initializing Enterprise...\n")
    (let ((init-result (enterprise-initialize enterprise
                         "Create cognitive synergy platform for AGI research"
                         #:goals '("Integrate OpenCog components"
                                   "Enable reproducible development"
                                   "Achieve cognitive autonomy"))))
      (display "   Initialized: ") (display (assoc-ref init-result 'initialized)) (newline)
      (display "   Goals: ") (display (assoc-ref init-result 'goals)) (newline))
    (newline)

    ;; Create thin slices
    (display "2. Creating Thin Slices...\n")
    (let ((slice1 (enterprise-create-thin-slice enterprise "TS-001" "AtomSpace integration"))
          (slice2 (enterprise-create-thin-slice enterprise "TS-002" "Metasystem implementation")))
      (display "   Created ") (display (length (enterprise-thin-slices enterprise)))
      (display " thin slices\n"))
    (newline)

    ;; Create pods
    (display "3. Creating Autonomous Pods...\n")
    (let ((pod1 (enterprise-create-pod enterprise "POD-001" "Core Team"
                  #:capabilities '(atomspace cogutil)))
          (pod2 (enterprise-create-pod enterprise "POD-002" "Integration Team"
                  #:capabilities '(synergy metasystem))))
      (pod-assign-slice pod1 (car (enterprise-thin-slices enterprise)))
      (pod-assign-slice pod2 (cadr (enterprise-thin-slices enterprise)))
      (display "   Created ") (display (length (enterprise-pods enterprise)))
      (display " pods\n"))
    (newline)

    ;; Evolve
    (display "4. Evolving for 20 cycles...\n")
    (let ((results (enterprise-evolve enterprise 20)))
      (display "   Completed ") (display (length results)) (display " cycles\n"))
    (newline)

    ;; State
    (display "5. Current State:\n")
    (let ((state (enterprise-state enterprise)))
      (display "   Coherence: ") (display (assoc-ref (assoc-ref state 'metrics) 'coherence)) (newline)
      (display "   Emergence: ") (display (assoc-ref (assoc-ref state 'metrics) 'emergence)) (newline)
      (display "   Autonomy: ") (display (assoc-ref (assoc-ref state 'metrics) 'autonomy)) (newline))
    (newline)

    ;; Reflection
    (display "6. Self-Reflection:\n")
    (let ((reflection (enterprise-reflect enterprise)))
      (display "   Diagnosis: ") (display (take (assoc-ref reflection 'diagnosis) 2)) (newline)
      (display "   Recommendations: ") (display (take (assoc-ref reflection 'recommendations) 2)) (newline))
    (newline)

    ;; Metasystem mapping
    (display "7. Metasystem Mapping:\n")
    (let ((mapping (enterprise-metasystem-mapping enterprise)))
      (display "   Planes: ") (display (map car (assoc-ref mapping 'planes))) (newline)
      (display "   Cycles: ") (display (map car (assoc-ref mapping 'cycles))) (newline))
    (newline))

  (display "=== Demonstration Complete ===\n"))

;;; End of enterprise_autogenesis.scm
