;;; metasystem.scm - Viable System Metasystem Implementation
;;;
;;; Based on Eric Schwarz's organizational systems theory depicting:
;;; - Three planes of existence (Energy/Objects, Information/Relations, Totality/Whole)
;;; - Six cycles defining viable natural systems
;;; - Seven steps of self-production (Spiral and Triad)
;;; - 11-stage evolutionary helix
;;;
;;; This module implements the theoretical framework for self-organizing,
;;; autopoietic cognitive systems with full metasystem capabilities.

(define-module (metamodel metasystem)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (srfi srfi-43)
  #:use-module (ice-9 match)
  #:export (
    ;; Planes of existence
    make-plane-of-energy
    make-plane-of-information
    make-plane-of-totality

    ;; Six cycles
    make-vortex-cycle
    make-retroaction-cycle
    make-self-referential-cycle
    make-morphogenesis-cycle
    make-autopoiesis-cycle
    make-autogenesis-cycle

    ;; Metasystem core
    make-metasystem
    metasystem?
    metasystem-step
    metasystem-evolve
    metasystem-reflect
    metasystem-state

    ;; Spiral and Triad (Seven Steps)
    make-spiral-triad
    spiral-step-unity
    spiral-step-distinction
    spiral-step-distanciation
    spiral-step-emergence
    spiral-step-autopoiesis
    spiral-step-self-reference
    spiral-step-autogenesis
    spiral-current-step
    spiral-advance

    ;; Evolutionary Helix (11 stages)
    make-evolutionary-helix
    helix-stage
    helix-spiral-level
    helix-advance
    helix-momentum

    ;; Autopoietic operations
    autopoietic-produce
    autopoietic-maintain-boundary
    autopoietic-regenerate

    ;; Integration
    metasystem-coherence
    metasystem-emergence-score
    metasystem-autonomy-level

    ;; Demonstration
    demonstrate-metasystem
  ))

;;;; ===========================================================================
;;;; 1. THREE PLANES OF EXISTENCE
;;;; ===========================================================================

;;; Plane of Energy - Physical structures, objects in space-time
(define-record-type <plane-of-energy>
  (make-plane-of-energy-internal objects structures flux stability)
  plane-of-energy?
  (objects energy-objects set-energy-objects!)
  (structures energy-structures set-energy-structures!)
  (flux energy-flux set-energy-flux!)
  (stability energy-stability set-energy-stability!))

(define* (make-plane-of-energy #:key (initial-flux 0.5))
  "Create a plane of energy - the physical/objectal plane"
  (make-plane-of-energy-internal
    '()      ; objects
    '()      ; structures
    initial-flux  ; flux - rate of energy exchange
    0.5))    ; stability

;;; Plane of Information - Relations, causal networks
(define-record-type <plane-of-information>
  (make-plane-of-information-internal relations causation coherence entropy)
  plane-of-information?
  (relations info-relations set-info-relations!)
  (causation info-causation set-info-causation!)
  (coherence info-coherence set-info-coherence!)
  (entropy info-entropy set-info-entropy!))

(define* (make-plane-of-information #:key (initial-coherence 0.5))
  "Create a plane of information - the relational/logical plane"
  (make-plane-of-information-internal
    '()      ; relations
    '()      ; causation network
    initial-coherence  ; coherence
    0.5))    ; entropy

;;; Plane of Totality - The whole, being, identity, self
(define-record-type <plane-of-totality>
  (make-plane-of-totality-internal being image identity wholeness)
  plane-of-totality?
  (being totality-being set-totality-being!)
  (image totality-image set-totality-image!)
  (identity totality-identity set-totality-identity!)
  (wholeness totality-wholeness set-totality-wholeness!))

(define* (make-plane-of-totality #:key (initial-wholeness 0.0))
  "Create a plane of totality - the existential/holistic plane"
  (make-plane-of-totality-internal
    '()      ; being - current existence state
    '()      ; image - self-image/representation
    '()      ; identity - stable core identity
    initial-wholeness))  ; wholeness measure

;;;; ===========================================================================
;;;; 2. SIX CYCLES DEFINING VIABLE NATURAL SYSTEMS
;;;; ===========================================================================

;;; Abstract cycle type
(define-record-type <cycle>
  (make-cycle-internal name plane type state rate feedback)
  cycle?
  (name cycle-name)
  (plane cycle-plane)
  (type cycle-type)
  (state cycle-state set-cycle-state!)
  (rate cycle-rate set-cycle-rate!)
  (feedback cycle-feedback set-cycle-feedback!))

;;; Horizontal Cycles (within planes - stability)

;; Cycle 2: Vortices - Cyclic exchanges of energy and matter
(define* (make-vortex-cycle #:key (rate 0.5))
  "Create vortex cycle - cyclic matter/energy exchanges (metabolism)"
  (make-cycle-internal
    'vortex
    'energy
    'horizontal
    'cycling      ; state: cycling, dissipating, accumulating
    rate
    '()))

;; Cycle 3: Retroaction Loops - Homeostasis and cybernetic controls
(define* (make-retroaction-cycle #:key (rate 0.5))
  "Create retroaction cycle - homeostasis and regulation"
  (make-cycle-internal
    'retroaction
    'information
    'horizontal
    'regulating   ; state: regulating, oscillating, diverging
    rate
    '()))

;; Cycle 5: Self-Referential Loop - Self-knowledge, autonomy
(define* (make-self-referential-cycle #:key (rate 0.5))
  "Create self-referential cycle - dialogue between being and image"
  (make-cycle-internal
    'self-referential
    'totality
    'horizontal
    'observing    ; state: observing, modeling, integrating
    rate
    '()))

;;; Vertical Cycles (between planes - change/evolution)

;; Cycle 1: Morphogenesis - Emergence, evolution, structure creation
(define* (make-morphogenesis-cycle #:key (rate 0.5))
  "Create morphogenesis cycle - emergence and structural evolution"
  (make-cycle-internal
    'morphogenesis
    'energy->information  ; spans energy to information
    'vertical
    'emerging     ; state: emerging, differentiating, integrating
    rate
    '()))

;; Cycle 4: Autopoiesis - Mutual production of network and structures
(define* (make-autopoiesis-cycle #:key (rate 0.5))
  "Create autopoiesis cycle - self-production"
  (make-cycle-internal
    'autopoiesis
    'information->energy  ; spans information to energy
    'vertical
    'producing    ; state: producing, maintaining, regenerating
    rate
    '()))

;; Cycle 6: Autogenesis - Self-creation, metacoupling to autonomy
(define* (make-autogenesis-cycle #:key (rate 0.5))
  "Create autogenesis cycle - self-creation leading to autonomy"
  (make-cycle-internal
    'autogenesis
    'totality->energy     ; spans all planes
    'vertical
    'creating     ; state: creating, transcending, autonomous
    rate
    '()))

;;; Cycle operations
(define (cycle-step cycle input)
  "Execute one step of a cycle"
  (let* ((name (cycle-name cycle))
         (state (cycle-state cycle))
         (rate (cycle-rate cycle)))
    (case name
      ((vortex)
       ;; Vortex cycles through energy states
       (set-cycle-state! cycle
         (case state
           ((cycling) 'accumulating)
           ((accumulating) 'dissipating)
           ((dissipating) 'cycling)
           (else 'cycling)))
       `((output . ,(* rate input))
         (state . ,(cycle-state cycle))))

      ((retroaction)
       ;; Retroaction maintains stability
       (let ((error (- input 0.5)))  ; deviation from setpoint
         (set-cycle-feedback! cycle (cons error (cycle-feedback cycle)))
         (set-cycle-state! cycle
           (cond
             ((< (abs error) 0.1) 'regulating)
             ((< (abs error) 0.3) 'oscillating)
             (else 'diverging)))
         `((output . ,(- input (* rate error)))
           (state . ,(cycle-state cycle)))))

      ((self-referential)
       ;; Self-reference compares being with image
       (set-cycle-state! cycle
         (case state
           ((observing) 'modeling)
           ((modeling) 'integrating)
           ((integrating) 'observing)
           (else 'observing)))
       `((output . ,input)
         (state . ,(cycle-state cycle))
         (self-awareness . ,(* rate input))))

      ((morphogenesis)
       ;; Morphogenesis creates new structures
       (set-cycle-state! cycle
         (case state
           ((emerging) 'differentiating)
           ((differentiating) 'integrating)
           ((integrating) 'emerging)
           (else 'emerging)))
       `((output . ,(* (+ 1 rate) input))
         (state . ,(cycle-state cycle))
         (new-structure . ,(> input 0.7))))

      ((autopoiesis)
       ;; Autopoiesis produces and maintains
       (set-cycle-state! cycle
         (case state
           ((producing) 'maintaining)
           ((maintaining) 'regenerating)
           ((regenerating) 'producing)
           (else 'producing)))
       `((output . ,input)
         (state . ,(cycle-state cycle))
         (production . ,(* rate input))))

      ((autogenesis)
       ;; Autogenesis creates autonomous identity
       (set-cycle-state! cycle
         (case state
           ((creating) 'transcending)
           ((transcending) 'autonomous)
           ((autonomous) 'creating)
           (else 'creating)))
       `((output . ,(* (+ 1 (* 0.5 rate)) input))
         (state . ,(cycle-state cycle))
         (autonomy . ,(eq? state 'autonomous))))

      (else
       `((output . ,input))))))

;;;; ===========================================================================
;;;; 3. SPIRAL AND TRIAD - SEVEN STEPS OF SELF-PRODUCTION
;;;; ===========================================================================

;;; The seven steps from Unity (0) through Autogenesis (6)

(define-record-type <spiral-triad>
  (make-spiral-triad-internal step energy integration history)
  spiral-triad?
  (step spiral-current-step set-spiral-step!)
  (energy spiral-energy set-spiral-energy!)
  (integration spiral-integration set-spiral-integration!)
  (history spiral-history set-spiral-history!))

(define step-names
  #(unity               ; 0 - Entity in eigenbehavior
    distinction         ; 1 - Separation of ONE into TWO (morphogenesis)
    distanciation       ; 2 - Appearance of relations (vortices)
    emergence           ; 3 - Emergence of wholeness (self-regulation)
    autopoiesis         ; 4 - Self-productive dialogue
    self-reference      ; 5 - Self-referential dialogue
    autogenesis))       ; 6 - Autonomous whole

(define* (make-spiral-triad #:key (initial-energy 1.0))
  "Create a spiral-triad at step 0 (Unity)"
  (make-spiral-triad-internal
    0            ; current step
    initial-energy
    0.0          ; integration level
    '()))        ; history

(define (spiral-step-name step-num)
  "Get the name of a spiral step"
  (if (and (>= step-num 0) (< step-num 7))
      (vector-ref step-names step-num)
      'unknown))

;; Step implementations

(define (spiral-step-unity triad)
  "Step 0: Unity - Entity in eigenbehavior, being without relations"
  (let ((energy (spiral-energy triad)))
    `((step . 0)
      (name . unity)
      (description . "Spontaneous state of being, entity in eigenbehavior")
      (characteristics . (vacuity chaos undifferentiated indefinite))
      (energy . ,energy)
      (ready-for-next . ,(> energy 0.5)))))

(define (spiral-step-distinction triad)
  "Step 1: Distinction - Separation of ONE into TWO, morphogenesis"
  (let ((energy (spiral-energy triad)))
    `((step . 1)
      (name . distinction)
      (description . "Separation, differentiation, replication, division")
      (characteristics . (duality difference alterity reproduction))
      (energy . ,(* 0.9 energy))
      (produces . (differentiation polarization))
      (ready-for-next . ,(> energy 0.4)))))

(define (spiral-step-distanciation triad)
  "Step 2: Distanciation - Appearance of relations, vortices"
  (let ((energy (spiral-energy triad)))
    `((step . 2)
      (name . distanciation)
      (description . "Establishment of relations between parts, enfolding")
      (characteristics . (complementarity enantiodromia yin-yang vortex))
      (energy . ,(* 0.85 energy))
      (produces . (circular-relation material-perennity oscillations))
      (ready-for-next . ,(> energy 0.35)))))

(define (spiral-step-emergence triad)
  "Step 3: Emergence of Wholeness - Self-regulation"
  (let ((energy (spiral-energy triad))
        (integration (spiral-integration triad)))
    `((step . 3)
      (name . emergence)
      (description . "Integration leading to coherence, containing the third")
      (characteristics . (wholeness self-regulation correlation paradox))
      (energy . ,(* 0.8 energy))
      (integration . ,(+ integration 0.2))
      (produces . (coherence stability functional-unity))
      (ready-for-next . ,(and (> energy 0.3) (> integration 0.2))))))

(define (spiral-step-autopoiesis triad)
  "Step 4: Self-Productive Dialogue - Autopoiesis"
  (let ((energy (spiral-energy triad))
        (integration (spiral-integration triad)))
    `((step . 4)
      (name . autopoiesis)
      (description . "Mutual production of physical structure and logical network")
      (characteristics . (self-production emergence-of-self-reference dialogue))
      (energy . ,(* 0.75 energy))
      (integration . ,(+ integration 0.25))
      (produces . (self-maintaining-structure operational-closure))
      (ready-for-next . ,(and (> energy 0.25) (> integration 0.4))))))

(define (spiral-step-self-reference triad)
  "Step 5: Self-Referential Dialogue - Self-reference"
  (let ((energy (spiral-energy triad))
        (integration (spiral-integration triad)))
    `((step . 5)
      (name . self-reference)
      (description . "Dialogue between compatible structures leading to identity")
      (characteristics . (self-knowledge object-image-dialogue autonomy-emergence))
      (energy . ,(* 0.7 energy))
      (integration . ,(+ integration 0.25))
      (produces . (identity self-awareness mutual-recognition))
      (ready-for-next . ,(and (> energy 0.2) (> integration 0.6))))))

(define (spiral-step-autogenesis triad)
  "Step 6: Autonomous Whole - Autogenesis"
  (let ((energy (spiral-energy triad))
        (integration (spiral-integration triad)))
    `((step . 6)
      (name . autogenesis)
      (description . "Self-creation by metacoupling, leading to full autonomy")
      (characteristics . (identity unity self-creation autonomy))
      (energy . ,(* 0.65 energy))
      (integration . ,(min 1.0 (+ integration 0.2)))
      (produces . (autonomous-whole self-creating-entity))
      (complete . #t))))

(define (spiral-advance triad)
  "Advance the spiral to the next step if conditions are met"
  (let* ((step (spiral-current-step triad))
         (result (case step
                   ((0) (spiral-step-unity triad))
                   ((1) (spiral-step-distinction triad))
                   ((2) (spiral-step-distanciation triad))
                   ((3) (spiral-step-emergence triad))
                   ((4) (spiral-step-autopoiesis triad))
                   ((5) (spiral-step-self-reference triad))
                   ((6) (spiral-step-autogenesis triad))
                   (else '())))
         (ready? (assoc-ref result 'ready-for-next))
         (complete? (assoc-ref result 'complete)))
    ;; Update state
    (when (assoc-ref result 'energy)
      (set-spiral-energy! triad (assoc-ref result 'energy)))
    (when (assoc-ref result 'integration)
      (set-spiral-integration! triad (assoc-ref result 'integration)))
    ;; Record history
    (set-spiral-history! triad (cons result (spiral-history triad)))
    ;; Advance if ready
    (cond
      (complete?
       ;; Completed full cycle - can restart at higher level
       (set-spiral-step! triad 0)
       (set-spiral-energy! triad (* 1.2 (spiral-energy triad)))  ; gain energy
       `((advanced . #t)
         (to . 0)
         (spiral-complete . #t)
         (result . ,result)))
      (ready?
       (set-spiral-step! triad (+ step 1))
       `((advanced . #t)
         (to . ,(+ step 1))
         (result . ,result)))
      (else
       `((advanced . #f)
         (at . ,step)
         (result . ,result))))))

;;;; ===========================================================================
;;;; 4. EVOLUTIONARY HELIX - 11 STAGES
;;;; ===========================================================================

(define-record-type <evolutionary-helix>
  (make-evolutionary-helix-internal stage spiral-level momentum characteristics)
  evolutionary-helix?
  (stage helix-stage set-helix-stage!)
  (spiral-level helix-spiral-level set-helix-spiral-level!)
  (momentum helix-momentum set-helix-momentum!)
  (characteristics helix-characteristics set-helix-characteristics!))

(define helix-stage-definitions
  #(;; Objectal Plane (Energy/Structures)
    ((name . entropic-drift)
     (stage . 0)
     (plane . objectal)
     (description . "Internal/external energy dissipation, uniformisation"))

    ((name . self-organization)
     (stage . 1)
     (plane . objectal)
     (description . "Morphogenesis - creation of space-time forms"))

    ((name . vortices)
     (stage . 2)
     (plane . objectal)
     (description . "Matter recycling, oscillations, pulsations"))

    ;; Relational Plane (Information/Networks)
    ((name . self-regulation)
     (stage . 3)
     (plane . relational)
     (description . "Homeostasis, functional stability"))

    ((name . starting-self-production)
     (stage . 4)
     (plane . relational)
     (description . "Appearance of positive self-producing ontological cycles"))

    ((name . information-drift)
     (stage . 5)
     (plane . relational)
     (description . "Diffusion of information, complexification"))

    ;; Existential Plane (Whole/Being)
    ((name . self-production)
     (stage . 6)
     (plane . existential)
     (description . "Autopoiesis - creation of self-producing structures"))

    ((name . self-reference)
     (stage . 7)
     (plane . existential)
     (description . "Mutuality between things and laws"))

    ((name . referential-drift)
     (stage . 8)
     (plane . existential)
     (description . "Intensification of self-reference, identisation"))

    ((name . self-creation)
     (stage . 9)
     (plane . existential)
     (description . "Autogenesis - entity producing its own rules"))

    ((name . toward-autonomy)
     (stage . 10)
     (plane . existential)
     (description . "Full autonomy and self-determination"))))

(define* (make-evolutionary-helix #:key (initial-momentum 0.5))
  "Create an evolutionary helix at stage 0"
  (make-evolutionary-helix-internal
    0              ; current stage
    0              ; spiral level (how many complete cycles)
    initial-momentum
    '()))          ; accumulated characteristics

(define (helix-stage-info helix)
  "Get information about current helix stage"
  (let ((stage (helix-stage helix)))
    (if (and (>= stage 0) (<= stage 10))
        (vector-ref helix-stage-definitions stage)
        '())))

(define (helix-advance helix input-energy)
  "Advance the evolutionary helix based on input energy"
  (let* ((stage (helix-stage helix))
         (momentum (helix-momentum helix))
         (level (helix-spiral-level helix))
         (stage-info (helix-stage-info helix))
         ;; Calculate advancement threshold based on stage
         (threshold (+ 0.3 (* 0.05 stage)))
         ;; Combined energy
         (combined (+ (* 0.7 input-energy) (* 0.3 momentum)))
         ;; Can advance?
         (can-advance? (> combined threshold)))
    ;; Update momentum with input
    (set-helix-momentum! helix (* 0.9 (+ (* 0.6 momentum) (* 0.4 input-energy))))
    ;; Record characteristic
    (when (assoc-ref stage-info 'description)
      (set-helix-characteristics! helix
        (cons (assoc-ref stage-info 'description)
              (helix-characteristics helix))))

    (cond
      ;; Completed full cycle
      ((and can-advance? (>= stage 10))
       (set-helix-stage! helix 0)
       (set-helix-spiral-level! helix (+ level 1))
       (set-helix-momentum! helix (* 1.1 momentum))  ; momentum boost
       `((advanced . #t)
         (from . ,stage)
         (to . 0)
         (spiral-level . ,(+ level 1))
         (cosmic-return . #t)))

      ;; Normal advancement
      (can-advance?
       (set-helix-stage! helix (+ stage 1))
       `((advanced . #t)
         (from . ,stage)
         (to . ,(+ stage 1))
         (stage-info . ,stage-info)))

      ;; Not ready
      (else
       `((advanced . #f)
         (at . ,stage)
         (needed . ,threshold)
         (had . ,combined)
         (stage-info . ,stage-info))))))

;;;; ===========================================================================
;;;; 5. AUTOPOIETIC OPERATIONS
;;;; ===========================================================================

(define (autopoietic-produce system-state)
  "Execute autopoietic self-production"
  (let* ((network (assoc-ref system-state 'network))
         (structures (assoc-ref system-state 'structures))
         (boundary (assoc-ref system-state 'boundary))
         ;; Network produces structures
         (new-structures (cons 'produced-component structures))
         ;; Structures realize network
         (new-network (cons 'realized-relation network)))
    `((network . ,new-network)
      (structures . ,new-structures)
      (boundary . ,boundary)
      (production-occurred . #t))))

(define (autopoietic-maintain-boundary system-state)
  "Maintain autopoietic boundary"
  (let* ((boundary (or (assoc-ref system-state 'boundary) 1.0))
         (internal (or (assoc-ref system-state 'internal-flux) 0.5))
         (external (or (assoc-ref system-state 'external-flux) 0.5))
         ;; Boundary strength depends on balance
         (new-boundary (* boundary (- 1 (abs (- internal external))))))
    `((boundary . ,new-boundary)
      (boundary-maintained . ,(> new-boundary 0.5)))))

(define (autopoietic-regenerate system-state)
  "Regenerate damaged components through autopoiesis"
  (let* ((damaged (or (assoc-ref system-state 'damaged-components) '()))
         (network (or (assoc-ref system-state 'network) '()))
         (production-capacity (length network))
         ;; Regenerate up to production capacity
         (to-regenerate (take damaged (min (length damaged) production-capacity)))
         (remaining (drop damaged (min (length damaged) production-capacity))))
    `((regenerated . ,to-regenerate)
      (remaining-damaged . ,remaining)
      (regeneration-complete . ,(null? remaining)))))

;;;; ===========================================================================
;;;; 6. METASYSTEM CORE
;;;; ===========================================================================

(define-record-type <metasystem>
  (make-metasystem-internal planes cycles spiral helix state)
  metasystem?
  (planes metasystem-planes set-metasystem-planes!)
  (cycles metasystem-cycles set-metasystem-cycles!)
  (spiral metasystem-spiral set-metasystem-spiral!)
  (helix metasystem-helix set-metasystem-helix!)
  (state metasystem-state set-metasystem-state!))

(define* (make-metasystem #:key
                          (initial-energy 0.5)
                          (dimension 32))
  "Create a complete metasystem with all components"
  (let* ((planes `((energy . ,(make-plane-of-energy))
                   (information . ,(make-plane-of-information))
                   (totality . ,(make-plane-of-totality))))
         (cycles `((vortex . ,(make-vortex-cycle))
                   (retroaction . ,(make-retroaction-cycle))
                   (self-referential . ,(make-self-referential-cycle))
                   (morphogenesis . ,(make-morphogenesis-cycle))
                   (autopoiesis . ,(make-autopoiesis-cycle))
                   (autogenesis . ,(make-autogenesis-cycle))))
         (spiral (make-spiral-triad #:initial-energy initial-energy))
         (helix (make-evolutionary-helix #:initial-momentum initial-energy))
         (state `((coherence . 0.0)
                  (emergence . 0.0)
                  (autonomy . 0.0)
                  (cycle-count . 0))))
    (make-metasystem-internal planes cycles spiral helix state)))

(define (metasystem-step ms input)
  "Execute one metasystem step with input"
  (let* ((cycles (metasystem-cycles ms))
         (spiral (metasystem-spiral ms))
         (helix (metasystem-helix ms))
         (state (metasystem-state ms))
         ;; Run all six cycles
         (vortex-result (cycle-step (assoc-ref cycles 'vortex) input))
         (retroaction-result (cycle-step (assoc-ref cycles 'retroaction) input))
         (self-ref-result (cycle-step (assoc-ref cycles 'self-referential) input))
         (morpho-result (cycle-step (assoc-ref cycles 'morphogenesis) input))
         (autopoiesis-result (cycle-step (assoc-ref cycles 'autopoiesis) input))
         (autogenesis-result (cycle-step (assoc-ref cycles 'autogenesis) input))
         ;; Aggregate outputs
         (avg-output (/ (+ (assoc-ref vortex-result 'output)
                          (assoc-ref retroaction-result 'output)
                          (assoc-ref morpho-result 'output)
                          (assoc-ref autopoiesis-result 'output))
                       4))
         ;; Advance spiral and helix
         (spiral-result (spiral-advance spiral))
         (helix-result (helix-advance helix avg-output))
         ;; Update state
         (new-coherence (or (assoc-ref self-ref-result 'self-awareness) 0))
         (new-emergence (if (assoc-ref morpho-result 'new-structure)
                           (+ 0.1 (assoc-ref state 'emergence))
                           (assoc-ref state 'emergence)))
         (new-autonomy (if (assoc-ref autogenesis-result 'autonomy)
                          (min 1.0 (+ 0.1 (assoc-ref state 'autonomy)))
                          (assoc-ref state 'autonomy)))
         (cycle-count (+ 1 (assoc-ref state 'cycle-count))))
    ;; Update metasystem state
    (set-metasystem-state! ms
      `((coherence . ,new-coherence)
        (emergence . ,new-emergence)
        (autonomy . ,new-autonomy)
        (cycle-count . ,cycle-count)))
    ;; Return results
    `((cycles . ((vortex . ,vortex-result)
                 (retroaction . ,retroaction-result)
                 (self-referential . ,self-ref-result)
                 (morphogenesis . ,morpho-result)
                 (autopoiesis . ,autopoiesis-result)
                 (autogenesis . ,autogenesis-result)))
      (spiral . ,spiral-result)
      (helix . ,helix-result)
      (state . ,(metasystem-state ms))
      (output . ,avg-output))))

(define (metasystem-evolve ms n)
  "Evolve the metasystem for n steps"
  (let loop ((i 0) (results '()))
    (if (>= i n)
        (reverse results)
        (let* ((input (* 0.5 (+ 1 (sin (* i 0.1)))))  ; oscillating input
               (result (metasystem-step ms input)))
          (loop (+ i 1) (cons result results))))))

(define (metasystem-reflect ms)
  "Metasystem reflects on its own state"
  (let* ((state (metasystem-state ms))
         (spiral (metasystem-spiral ms))
         (helix (metasystem-helix ms))
         (coherence (assoc-ref state 'coherence))
         (emergence (assoc-ref state 'emergence))
         (autonomy (assoc-ref state 'autonomy)))
    `((self-assessment
       . ((coherence-level . ,coherence)
          (emergence-level . ,emergence)
          (autonomy-level . ,autonomy)
          (spiral-step . ,(spiral-step-name (spiral-current-step spiral)))
          (helix-stage . ,(helix-stage helix))
          (spiral-level . ,(helix-spiral-level helix))))
      (diagnosis
       . ,(cond
            ((< coherence 0.3) "Low coherence - strengthen self-referential cycles")
            ((< emergence 0.3) "Low emergence - activate morphogenesis")
            ((< autonomy 0.3) "Low autonomy - develop autogenesis")
            (else "System operating normally")))
      (recommendations
       . ,(filter identity
            (list
              (and (< coherence 0.5) 'increase-self-reference)
              (and (< emergence 0.5) 'activate-morphogenesis)
              (and (< autonomy 0.5) 'develop-autopoiesis)))))))

;;;; ===========================================================================
;;;; 7. INTEGRATION METRICS
;;;; ===========================================================================

(define (metasystem-coherence ms)
  "Calculate overall metasystem coherence"
  (let* ((state (metasystem-state ms))
         (base-coherence (assoc-ref state 'coherence))
         (spiral (metasystem-spiral ms))
         (spiral-integration (spiral-integration spiral))
         (helix (metasystem-helix ms))
         (helix-level (helix-spiral-level helix)))
    (+ (* 0.4 base-coherence)
       (* 0.3 spiral-integration)
       (* 0.3 (/ helix-level (+ 1 helix-level))))))  ; asymptotic contribution

(define (metasystem-emergence-score ms)
  "Calculate emergence score across all levels"
  (let* ((state (metasystem-state ms))
         (emergence (assoc-ref state 'emergence))
         (spiral (metasystem-spiral ms))
         (spiral-step (spiral-current-step spiral))
         (helix (metasystem-helix ms))
         (helix-stage (helix-stage helix)))
    (+ (* 0.3 emergence)
       (* 0.35 (/ spiral-step 6))    ; normalized spiral progress
       (* 0.35 (/ helix-stage 10))))) ; normalized helix progress

(define (metasystem-autonomy-level ms)
  "Calculate autonomy level"
  (let* ((state (metasystem-state ms))
         (autonomy (assoc-ref state 'autonomy))
         (spiral (metasystem-spiral ms))
         (at-autogenesis? (= (spiral-current-step spiral) 6))
         (helix (metasystem-helix ms))
         (toward-autonomy? (>= (helix-stage helix) 9)))
    (+ (* 0.5 autonomy)
       (* 0.25 (if at-autogenesis? 1 0))
       (* 0.25 (if toward-autonomy? 1 0)))))

;;;; ===========================================================================
;;;; 8. DEMONSTRATION
;;;; ===========================================================================

(define (demonstrate-metasystem)
  "Demonstrate metasystem capabilities"
  (display "=== Metasystem Demonstration ===\n\n")

  ;; Create metasystem
  (display "1. Creating Metasystem...\n")
  (let ((ms (make-metasystem #:initial-energy 0.7)))

    ;; Show initial state
    (display "2. Initial State:\n")
    (let ((state (metasystem-state ms)))
      (display "   Coherence: ") (display (assoc-ref state 'coherence)) (newline)
      (display "   Emergence: ") (display (assoc-ref state 'emergence)) (newline)
      (display "   Autonomy: ") (display (assoc-ref state 'autonomy)) (newline))
    (newline)

    ;; Run evolution
    (display "3. Evolving for 20 steps...\n")
    (let ((results (metasystem-evolve ms 20)))
      (display "   Completed ") (display (length results)) (display " steps\n"))
    (newline)

    ;; Show evolved state
    (display "4. Evolved State:\n")
    (let ((state (metasystem-state ms)))
      (display "   Coherence: ") (display (assoc-ref state 'coherence)) (newline)
      (display "   Emergence: ") (display (assoc-ref state 'emergence)) (newline)
      (display "   Autonomy: ") (display (assoc-ref state 'autonomy)) (newline)
      (display "   Cycle count: ") (display (assoc-ref state 'cycle-count)) (newline))
    (newline)

    ;; Show spiral state
    (display "5. Spiral-Triad State:\n")
    (let ((spiral (metasystem-spiral ms)))
      (display "   Current step: ")
      (display (spiral-step-name (spiral-current-step spiral))) (newline)
      (display "   Energy: ") (display (spiral-energy spiral)) (newline)
      (display "   Integration: ") (display (spiral-integration spiral)) (newline))
    (newline)

    ;; Show helix state
    (display "6. Evolutionary Helix State:\n")
    (let ((helix (metasystem-helix ms)))
      (display "   Stage: ") (display (helix-stage helix))
      (display " (")
      (display (assoc-ref (helix-stage-info helix) 'name))
      (display ")\n")
      (display "   Spiral level: ") (display (helix-spiral-level helix)) (newline)
      (display "   Momentum: ") (display (helix-momentum helix)) (newline))
    (newline)

    ;; Self-reflection
    (display "7. Metasystem Self-Reflection:\n")
    (let ((reflection (metasystem-reflect ms)))
      (display "   Diagnosis: ")
      (display (assoc-ref reflection 'diagnosis)) (newline)
      (display "   Recommendations: ")
      (display (assoc-ref reflection 'recommendations)) (newline))
    (newline)

    ;; Integration metrics
    (display "8. Integration Metrics:\n")
    (display "   Overall coherence: ")
    (display (metasystem-coherence ms)) (newline)
    (display "   Emergence score: ")
    (display (metasystem-emergence-score ms)) (newline)
    (display "   Autonomy level: ")
    (display (metasystem-autonomy-level ms)) (newline)
    (newline))

  (display "=== Metasystem Demonstration Complete ===\n"))

;;; End of metasystem.scm
