;;; Attention - ECAN Attention Allocation for Cognitive Kernel
;;; Implements Economic Attention Networks for priority management
;;; Dynamically allocates cognitive resources based on importance and urgency

(define-module (cogkernel attention)
  #:use-module (ice-9 hash-table)
  #:use-module (ice-9 match)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (cogkernel atomspace)
  #:use-module (cogkernel agents)
  #:export (make-attention-value
            attention-value?
            attention-value-sti
            attention-value-lti
            attention-value-vlti
            make-attention-bank
            attention-bank-add!
            attention-bank-update!
            attention-bank-get-focus
            attention-bank-allocate!
            attention-bank-stimulate!
            *global-attention-bank*))

;;; AttentionValue record for Short/Long/Very-Long-Term Importance
(define-record-type <attention-value>
  (make-attention-value-record sti lti vlti)
  attention-value?
  (sti attention-value-sti set-attention-value-sti!)     ; Short-term importance
  (lti attention-value-lti set-attention-value-lti!)     ; Long-term importance  
  (vlti attention-value-vlti set-attention-value-vlti!)) ; Very-long-term importance

;;; AttentionBank for managing attention allocation across atoms and agents
(define-record-type <attention-bank>
  (make-attention-bank-record atom-av agent-av total-funds focus-threshold)
  attention-bank?
  (atom-av attention-bank-atom-av)      ; Hash table: atom -> attention-value
  (agent-av attention-bank-agent-av)    ; Hash table: agent -> attention-value
  (total-funds attention-bank-total-funds set-attention-bank-total-funds!)
  (focus-threshold attention-bank-focus-threshold set-attention-bank-focus-threshold!))

;;; Create new attention value
(define* (make-attention-value #:optional (sti 0) (lti 0) (vlti 0))
  "Create a new attention value with specified importance levels"
  (make-attention-value-record sti lti vlti))

;;; Create new attention bank
(define* (make-attention-bank #:optional (total-funds 1000) (focus-threshold 100))
  "Create a new attention bank with specified total funds and focus threshold"
  (make-attention-bank-record (make-hash-table) (make-hash-table) 
                              total-funds focus-threshold))

;;; Add item (atom or agent) to attention bank
(define (attention-bank-add! bank item attention-value)
  "Add an item to the attention bank with initial attention value"
  (cond
    ((atom? item)
     (hash-set! (attention-bank-atom-av bank) (atom-name item) attention-value))
    ((agent? item)
     (hash-set! (attention-bank-agent-av bank) (agent-id item) attention-value))
    (else
     (error "Invalid item type for attention bank"))))

;;; Update attention value for an item
(define (attention-bank-update! bank item delta-sti delta-lti delta-vlti)
  "Update attention values for an item with specified deltas"
  (let ((current-av (cond
                      ((atom? item)
                       (hash-ref (attention-bank-atom-av bank) (atom-name item)))
                      ((agent? item)
                       (hash-ref (attention-bank-agent-av bank) (agent-id item)))
                      (else #f))))
    (when current-av
      (set-attention-value-sti! current-av 
                                (+ (attention-value-sti current-av) delta-sti))
      (set-attention-value-lti! current-av 
                                (+ (attention-value-lti current-av) delta-lti))
      (set-attention-value-vlti! current-av 
                                 (+ (attention-value-vlti current-av) delta-vlti)))))

;;; Get current focus set (items above threshold)
(define (attention-bank-get-focus bank)
  "Get items currently in focus (above attention threshold)"
  (let ((focused-atoms '())
        (focused-agents '())
        (threshold (attention-bank-focus-threshold bank)))
    
    ;; Check atoms in focus
    (hash-for-each
      (lambda (name av)
        (when (> (attention-value-sti av) threshold)
          (set! focused-atoms (cons name focused-atoms))))
      (attention-bank-atom-av bank))
    
    ;; Check agents in focus
    (hash-for-each
      (lambda (id av)
        (when (> (attention-value-sti av) threshold)
          (set! focused-agents (cons id focused-agents))))
      (attention-bank-agent-av bank))
    
    (list focused-atoms focused-agents)))

;;; Allocate attention resources
(define* (attention-bank-allocate! bank #:optional item amount)
  "Perform attention allocation spreading and decay, or allocate specific amount to item"
  (if (and item amount)
      ; Allocate specific amount to item
      (let ((av (make-attention-value amount (quotient amount 2) (quotient amount 4))))
        (attention-bank-add! bank item av))
      ; Perform general allocation and decay
      (let ((total-sti 0)
            (decay-rate 0.05))
        
        ;; Calculate total STI and apply decay
        (hash-for-each
          (lambda (name av)
            (let ((current-sti (attention-value-sti av)))
              (set! total-sti (+ total-sti current-sti))
              ;; Apply decay
              (set-attention-value-sti! av (* current-sti (- 1 decay-rate)))))
          (attention-bank-atom-av bank))
        
        (hash-for-each
          (lambda (id av)
            (let ((current-sti (attention-value-sti av)))
              (set! total-sti (+ total-sti current-sti))
              ;; Apply decay
              (set-attention-value-sti! av (* current-sti (- 1 decay-rate)))))
          (attention-bank-agent-av bank))
        
        ;; Normalize if total exceeds funds
        (when (> total-sti (attention-bank-total-funds bank))
          (let ((scale-factor (/ (attention-bank-total-funds bank) total-sti)))
            (hash-for-each
              (lambda (name av)
                (set-attention-value-sti! av (* (attention-value-sti av) scale-factor)))
              (attention-bank-atom-av bank))
            (hash-for-each
              (lambda (id av)
                (set-attention-value-sti! av (* (attention-value-sti av) scale-factor)))
              (attention-bank-agent-av bank)))))))

;;; Stimulate attention for important events
(define (attention-bank-stimulate! bank item stimulus-type intensity)
  "Apply attention stimulus to an item based on event type and intensity"
  (let ((sti-boost (case stimulus-type
                     ((URGENT) (* intensity 50))
                     ((IMPORTANT) (* intensity 25))
                     ((NORMAL) (* intensity 10))
                     ((LOW) (* intensity 5))
                     (else intensity)))
        (lti-boost (case stimulus-type
                     ((LEARNING) (* intensity 20))
                     ((PATTERN) (* intensity 15))
                     ((MEMORY) (* intensity 10))
                     (else (* intensity 2))))
        (vlti-boost (case stimulus-type
                      ((STRUCTURAL) (* intensity 5))
                      ((SYSTEM) (* intensity 3))
                      (else (* intensity 1)))))
    
    ;; Initialize attention value if not exists
    (cond
      ((atom? item)
       (unless (hash-ref (attention-bank-atom-av bank) (atom-name item))
         (attention-bank-add! bank item (make-attention-value))))
      ((agent? item)
       (unless (hash-ref (attention-bank-agent-av bank) (agent-id item))
         (attention-bank-add! bank item (make-attention-value)))))
    
    ;; Apply stimulus
    (attention-bank-update! bank item sti-boost lti-boost vlti-boost)))

;;; Create global attention bank
(define *global-attention-bank* (make-attention-bank))

;;; Initialize attention for core Hurd components
(define (initialize-hurd-attention! bank atomspace agent-system)
  "Initialize attention values for core Hurd components"
  
  ;; Add attention for core atoms
  (let ((hurd-atom (atomspace-get atomspace "GNU-Hurd")))
    (when hurd-atom
      (attention-bank-add! bank hurd-atom (make-attention-value 200 150 100))))
  
  ;; Add attention for core agents
  (let ((monitor-agent (agent-system-get agent-system "system-monitor"))
        (repair-agent (agent-system-get agent-system "auto-repair"))
        (build-agent (agent-system-get agent-system "build-coordinator"))
        (meta-agent (agent-system-get agent-system "meta-modifier")))
    
    (when monitor-agent
      (attention-bank-add! bank monitor-agent (make-attention-value 150 100 50)))
    (when repair-agent
      (attention-bank-add! bank repair-agent (make-attention-value 120 80 40)))
    (when build-agent
      (attention-bank-add! bank build-agent (make-attention-value 100 120 60)))
    (when meta-agent
      (attention-bank-add! bank meta-agent (make-attention-value 180 200 150)))))

;;; Cognitive attention cycle
(define (attention-cycle! bank agent-system)
  "Execute one attention allocation cycle"
  (attention-bank-allocate! bank)
  (let ((focus (attention-bank-get-focus bank)))
    (format #t "Current focus: ~a~%" focus)
    ;; Trigger actions for agents in focus
    (for-each
      (lambda (agent-id)
        (let ((agent (agent-system-get agent-system agent-id)))
          (when agent
            (case (agent-role agent)
              ((MONITOR) (agent-execute! agent 'DETECT))
              ((REPAIR) (agent-execute! agent 'REPAIR "auto-detected-issue"))
              ((BUILD) (agent-execute! agent 'BUILD "system"))
              ((ANALYZE) (agent-execute! agent 'ANALYZE))
              ((META) (agent-execute! agent 'SYNTHESIZE))))))
      (cadr focus))
    focus))

;;; Initialize global attention bank (commented out for manual control)
;; (initialize-hurd-attention! *global-attention-bank* *global-atomspace* *global-agent-system*)