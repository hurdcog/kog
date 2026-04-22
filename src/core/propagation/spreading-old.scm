;;; spreading.scm --- Echo Propagation Engine: Spreading Activation
;;;
;;; Copyright (C) 2025 Deep Tree Echo Project
;;;
;;; This file implements the spreading activation algorithm for the Echo
;;; Propagation Engine. Activation spreads from source atoms through the
;;; hypergraph, enabling associative memory and emergent cognitive patterns.
;;;
;;; Commentary:
;;;
;;; Spreading activation is inspired by neural networks and cognitive science.
;;; Activation represents "cognitive energy" that flows through the knowledge
;;; graph, activating related concepts and enabling pattern recognition.
;;;
;;; Key features:
;;; - Breadth-first propagation with decay
;;; - Truth value and attention value weighting
;;; - Bidirectional propagation (forward and backward through links)
;;; - Threshold-based pruning
;;; - Multiple normalization strategies
;;;
;;; Code:

(define-module (core propagation spreading)
  #:use-module (srfi srfi-1)    ; List library
  #:use-module (srfi srfi-9)    ; Record types
  #:use-module (srfi srfi-69)   ; Hash tables
  #:use-module (ice-9 match)    ; Pattern matching
  #:use-module (core memory atomspace)
  #:export (
    ;; Propagation parameters
    make-propagation-params
    propagation-params?
    params-decay-rate
    params-threshold
    params-max-hops
    params-normalization
    params-bidirectional?
    params-use-truth-values?
    params-use-attention?
    
    ;; Default parameters
    default-propagation-params
    
    ;; Activation operations
    make-activation-map
    get-activation
    set-activation!
    clear-activations!
    get-activated-atoms
    
    ;; Spreading activation
    propagate-activation
    propagate-from-atom
    propagate-from-atoms
    
    ;; Utilities
    normalize-activations!
    decay-activations!
    threshold-activations
    activation-to-list
  ))

;;;
;;; Propagation Parameters
;;;

(define-record-type <propagation-params>
  (%make-propagation-params decay-rate threshold max-hops normalization
                           bidirectional? use-truth-values? use-attention?)
  propagation-params?
  (decay-rate params-decay-rate)              ; Activation decay per hop [0.0, 1.0]
  (threshold params-threshold)                ; Minimum activation to propagate
  (max-hops params-max-hops)                  ; Maximum propagation distance
  (normalization params-normalization)        ; Normalization strategy
  (bidirectional? params-bidirectional?)      ; Allow backward propagation
  (use-truth-values? params-use-truth-values?) ; Weight by truth values
  (use-attention? params-use-attention?))     ; Weight by attention values

(define* (make-propagation-params
          #:key
          (decay-rate 0.8)
          (threshold 0.01)
          (max-hops 5)
          (normalization 'none)
          (bidirectional? #t)
          (use-truth-values? #t)
          (use-attention? #f))
  "Create propagation parameters.
  
  DECAY-RATE: Activation multiplier per hop (0.0 to 1.0)
  THRESHOLD: Minimum activation to continue propagating
  MAX-HOPS: Maximum distance from source atoms
  NORMALIZATION: 'none, 'sum, 'max, or 'softmax
  BIDIRECTIONAL?: Propagate backward through links
  USE-TRUTH-VALUES?: Weight propagation by link truth values
  USE-ATTENTION?: Weight propagation by attention values
  
  Example:
    (make-propagation-params #:decay-rate 0.8 #:max-hops 3)"
  (unless (and (>= decay-rate 0.0) (<= decay-rate 1.0))
    (error "Decay rate must be between 0.0 and 1.0" decay-rate))
  (unless (and (>= threshold 0.0) (<= threshold 1.0))
    (error "Threshold must be between 0.0 and 1.0" threshold))
  (unless (> max-hops 0)
    (error "Max hops must be positive" max-hops))
  (unless (member normalization '(none sum max softmax))
    (error "Invalid normalization strategy" normalization))
  (%make-propagation-params decay-rate threshold max-hops normalization
                           bidirectional? use-truth-values? use-attention?))

(define default-propagation-params
  (make-propagation-params
    #:decay-rate 0.8
    #:threshold 0.01
    #:max-hops 5
    #:normalization 'none
    #:bidirectional? #t
    #:use-truth-values? #t
    #:use-attention? #f))

;;;
;;; Activation Map
;;;

(define (make-activation-map)
  "Create a new activation map (hash table: UUID -> activation value)."
  (make-hash-table))

(define (get-activation activation-map atom-or-link)
  "Get activation value for ATOM-OR-LINK from ACTIVATION-MAP."
  (let ((uuid (if (link? atom-or-link)
                  (link-uuid atom-or-link)
                  (atom-uuid atom-or-link))))
    (hash-table-ref/default activation-map uuid 0.0)))

(define (set-activation! activation-map atom-or-link value)
  "Set activation value for ATOM-OR-LINK in ACTIVATION-MAP to VALUE."
  (let ((uuid (if (link? atom-or-link)
                  (link-uuid atom-or-link)
                  (atom-uuid atom-or-link))))
    (hash-table-set! activation-map uuid value)))

(define (clear-activations! activation-map)
  "Clear all activations from ACTIVATION-MAP."
  (hash-table-walk activation-map 
    (lambda (k v) (hash-table-delete! activation-map k))))

(define (get-activated-atoms atomspace activation-map threshold)
  "Get all atoms from ATOMSPACE with activation above THRESHOLD."
  (let ((activated '()))
    (hash-table-walk activation-map
      (lambda (uuid activation)
        (when (> activation threshold)
          (let ((atom (atomspace-get-atom atomspace uuid)))
            (when atom
              (set! activated (cons (cons atom activation) activated)))))))
    (sort activated (lambda (a b) (> (cdr a) (cdr b))))))

;;;
;;; Utility Functions
;;;

(define (clamp value min-val max-val)
  "Clamp VALUE between MIN-VAL and MAX-VAL."
  (max min-val (min max-val value)))

(define (get-link-weight link params)
  "Calculate weight for LINK based on PARAMS."
  (let ((weight 1.0))
    ;; Weight by truth value strength
    (when (params-use-truth-values? params)
      (let* ((tv (link-tv link))
             (strength (tv-strength tv)))
        (set! weight (* weight strength))))
    
    ;; Weight by attention value (normalized STI)
    (when (params-use-attention? params)
      (let* ((av (link-av link))
             (sti (av-sti av))
             (normalized-sti (/ (+ sti 1000) 2000))) ; Normalize [-1000, 1000] to [0, 1]
        (set! weight (* weight normalized-sti))))
    
    weight))

(define (normalize-activations! activation-map strategy)
  "Normalize all activations in ACTIVATION-MAP using STRATEGY.
  
  Strategies:
  - 'none: No normalization
  - 'sum: Divide by sum of all activations
  - 'max: Divide by maximum activation
  - 'softmax: Apply softmax function"
  (case strategy
    ((none) #t) ; Do nothing
    
    ((sum)
     (let ((total 0.0))
       ;; Calculate sum
       (hash-table-walk activation-map
         (lambda (k v) (set! total (+ total v))))
       ;; Normalize
       (when (> total 0.0)
         (hash-table-walk activation-map
           (lambda (k v)
             (hash-table-set! activation-map k (/ v total)))))))
    
    ((max)
     (let ((max-val 0.0))
       ;; Find maximum
       (hash-table-walk activation-map
         (lambda (k v) (set! max-val (max max-val v))))
       ;; Normalize
       (when (> max-val 0.0)
         (hash-table-walk activation-map
           (lambda (k v)
             (hash-table-set! activation-map k (/ v max-val)))))))
    
    ((softmax)
     (let ((exp-sum 0.0)
           (exp-values (make-hash-table)))
       ;; Calculate exp values and sum
       (hash-table-walk activation-map
         (lambda (k v)
           (let ((exp-v (exp v)))
             (hash-table-set! exp-values k exp-v)
             (set! exp-sum (+ exp-sum exp-v)))))
       ;; Normalize
       (when (> exp-sum 0.0)
         (hash-table-walk exp-values
           (lambda (k exp-v)
             (hash-table-set! activation-map k (/ exp-v exp-sum)))))))
    
    (else (error "Invalid normalization strategy" strategy))))

(define (decay-activations! activation-map decay-rate)
  "Apply DECAY-RATE to all activations in ACTIVATION-MAP."
  (hash-table-walk activation-map
    (lambda (k v)
      (hash-table-set! activation-map k (* v decay-rate)))))

(define (threshold-activations activation-map threshold)
  "Return new activation map with only activations above THRESHOLD."
  (let ((result (make-activation-map)))
    (hash-table-walk activation-map
      (lambda (k v)
        (when (> v threshold)
          (hash-table-set! result k v))))
    result))

(define (activation-to-list activation-map)
  "Convert ACTIVATION-MAP to list of (uuid . activation) pairs, sorted by activation."
  (let ((pairs '()))
    (hash-table-walk activation-map
      (lambda (k v)
        (set! pairs (cons (cons k v) pairs))))
    (sort pairs (lambda (a b) (> (cdr a) (cdr b))))))

;;;
;;; Spreading Activation Algorithm
;;;

(define (propagate-activation atomspace sources params)
  "Spread activation from SOURCES through ATOMSPACE using PARAMS.
  
  SOURCES: List of (atom-or-link . initial-activation) pairs
  PARAMS: Propagation parameters
  
  Returns: Activation map (hash table: UUID -> activation)
  
  Example:
    (propagate-activation as 
      (list (cons cat-atom 1.0) (cons dog-atom 0.5))
      default-propagation-params)"
  
  (let ((activation-map (make-activation-map))
        (visited (make-hash-table))
        (queue '())
        (hop-count (make-hash-table)))
    
    ;; Initialize sources
    (for-each
      (lambda (source-pair)
        (let* ((source (car source-pair))
               (initial-activation (cdr source-pair))
               (uuid (if (link? source)
                        (link-uuid source)
                        (atom-uuid source))))
          (set-activation! activation-map source initial-activation)
          (hash-table-set! hop-count uuid 0)
          (set! queue (cons source queue))))
      sources)
    
    ;; Breadth-first propagation
    (let loop ((queue queue))
      (unless (null? queue)
        (let* ((current (car queue))
               (rest-queue (cdr queue))
               (current-uuid (if (link? current)
                                (link-uuid current)
                                (atom-uuid current)))
               (current-activation (get-activation activation-map current))
               (current-hops (hash-table-ref/default hop-count current-uuid 0)))
          
          ;; Skip if already visited or below threshold or max hops reached
          (if (or (hash-table-ref/default visited current-uuid #f)
                  (< current-activation (params-threshold params))
                  (>= current-hops (params-max-hops params)))
              (loop rest-queue)
              (begin
                ;; Mark as visited
                (hash-table-set! visited current-uuid #t)
                
                ;; Propagate forward
                (let ((new-queue rest-queue))
                  (cond
                   ;; If current is a link, propagate to its outgoing atoms
                   ((link? current)
                    (let ((outgoing (get-outgoing-set current)))
                      (for-each
                        (lambda (target)
                          (let* ((propagated (* current-activation 
                                               (params-decay-rate params)
                                               (if (params-use-truth-values? params)
                                                   (tv-strength (link-tv current))
                                                   1.0)))
                                 (target-uuid (if (link? target)
                                                 (link-uuid target)
                                                 (atom-uuid target)))
                                 (old-activation (get-activation activation-map target))
                                 (new-activation (+ old-activation propagated)))
                            
                            ;; Update activation
                            (set-activation! activation-map target new-activation)
                            
                            ;; Update hop count
                            (hash-table-set! hop-count target-uuid (+ current-hops 1))
                            
                            ;; Add to queue if not visited and above threshold
                            (unless (hash-table-ref/default visited target-uuid #f)
                              (set! new-queue (cons target new-queue)))))
                        outgoing)))
                   ;; If current is an atom, propagate through outgoing links
                   ;; (links where this atom appears in the outgoing set)
                   (else
                    (let ((outgoing-links (get-incoming-set current)))
                      (for-each
                        (lambda (link-atom)
                          (let* ((is-link? (member (atom-type link-atom)
                                                  '(InheritanceLink EvaluationLink ListLink)))
                                 (weight (if (and is-link? (params-use-truth-values? params))
                                           (tv-strength (atom-tv link-atom))
                                           1.0))
                                 (propagated (* current-activation 
                                               (params-decay-rate params)
                                               weight))
                                 (link-uuid (atom-uuid link-atom))
                                 (old-activation (get-activation activation-map link-atom))
                                 (new-activation (+ old-activation propagated)))
                            
                            ;; Update activation
                            (set-activation! activation-map link-atom new-activation)
                            
                            ;; Update hop count
                            (hash-table-set! hop-count link-uuid (+ current-hops 1))
                            
                            ;; Add to queue if not visited and above threshold
                            (when is-link?
                              (unless (hash-table-ref/default visited link-uuid #f)
                                (set! new-queue (cons link-atom new-queue)))))))
                        outgoing-links))))
                  
                  ;; Propagate backward through incoming links (if bidirectional)
                  (when (params-bidirectional? params)                    (let ((incoming (get-incoming-set current)))
                      (for-each
                        (lambda (source-atom)
                          ;; Incoming set contains atoms (which may be link-type atoms)
                          (let* ((is-link-type? (member (atom-type source-atom)
                                                       '(InheritanceLink EvaluationLink ListLink)))
                                 (weight (if (and is-link-type? (params-use-truth-values? params))
                                           (tv-strength (atom-tv source-atom))
                                           1.0))
                                 (propagated (* current-activation 
                                               (params-decay-rate params)
                                               weight))
                                 (source-uuid (atom-uuid source-atom))
                                 (old-activation (get-activation activation-map source-atom))
                                 (new-activation (+ old-activation propagated)))
                            
                            ;; Update activation
                            (set-activation! activation-map source-atom new-activation)
                            
                            ;; Update hop count
                            (hash-table-set! hop-count source-uuid (+ current-hops 1))
                            
                            ;; Add to queue if not visited and above threshold
                            (unless (hash-table-ref/default visited source-uuid #f)
                              (set! new-queue (cons source-atom new-queue)))))
                        incoming)))
                  
                  ;; Continue with updated queue
                  (loop new-queue))))))))
    
    ;; Normalize if requested
    (normalize-activations! activation-map (params-normalization params))
    
    activation-map))

(define (propagate-from-atom atomspace atom initial-activation params)
  "Spread activation from single ATOM with INITIAL-ACTIVATION.
  
  Convenience wrapper around propagate-activation.
  
  Example:
    (propagate-from-atom as cat-atom 1.0 default-propagation-params)"
  (propagate-activation atomspace (list (cons atom initial-activation)) params))

(define (propagate-from-atoms atomspace atoms initial-activation params)
  "Spread activation from multiple ATOMS with same INITIAL-ACTIVATION.
  
  Convenience wrapper around propagate-activation.
  
  Example:
    (propagate-from-atoms as (list cat-atom dog-atom) 1.0 default-propagation-params)"
  (let ((sources (map (lambda (atom) (cons atom initial-activation)) atoms)))
    (propagate-activation atomspace sources params)))

;;; spreading.scm ends here
