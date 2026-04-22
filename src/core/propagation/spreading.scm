;;; spreading-v2.scm --- Echo Propagation Engine: Spreading Activation (Clean Version)
;;;
;;; Copyright (C) 2025 Deep Tree Echo Project
;;;
;;; This file implements the spreading activation algorithm for the Echo
;;; Propagation Engine. This is a cleaner rewrite with simpler structure.
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
  (decay-rate params-decay-rate)
  (threshold params-threshold)
  (max-hops params-max-hops)
  (normalization params-normalization)
  (bidirectional? params-bidirectional?)
  (use-truth-values? params-use-truth-values?)
  (use-attention? params-use-attention?))

(define* (make-propagation-params
          #:key
          (decay-rate 0.8)
          (threshold 0.01)
          (max-hops 5)
          (normalization 'none)
          (bidirectional? #t)
          (use-truth-values? #t)
          (use-attention? #f))
  "Create propagation parameters."
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
  "Create a new activation map."
  (make-hash-table))

(define (get-activation activation-map atom-or-link)
  "Get activation value for ATOM-OR-LINK."
  (let ((uuid (if (link? atom-or-link)
                  (link-uuid atom-or-link)
                  (atom-uuid atom-or-link))))
    (hash-table-ref/default activation-map uuid 0.0)))

(define (set-activation! activation-map atom-or-link value)
  "Set activation value for ATOM-OR-LINK."
  (let ((uuid (if (link? atom-or-link)
                  (link-uuid atom-or-link)
                  (atom-uuid atom-or-link))))
    (hash-table-set! activation-map uuid value)))

(define (clear-activations! activation-map)
  "Clear all activations."
  (hash-table-walk activation-map 
    (lambda (k v) (hash-table-delete! activation-map k))))

(define (get-activated-atoms atomspace activation-map threshold)
  "Get all atoms with activation above THRESHOLD."
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

(define (is-link-type? atom)
  "Check if ATOM is a link type."
  (member (atom-type atom) '(InheritanceLink EvaluationLink ListLink)))

(define (get-weight atom params)
  "Calculate weight for ATOM based on PARAMS."
  (let ((weight 1.0))
    (when (and (is-link-type? atom) (params-use-truth-values? params))
      (set! weight (* weight (tv-strength (atom-tv atom)))))
    (when (params-use-attention? params)
      (let* ((sti (av-sti (atom-av atom)))
             (normalized-sti (/ (+ sti 1000) 2000)))
        (set! weight (* weight normalized-sti))))
    weight))

(define (normalize-activations! activation-map strategy)
  "Normalize all activations using STRATEGY."
  (case strategy
    ((none) #t)
    ((sum)
     (let ((total 0.0))
       (hash-table-walk activation-map
         (lambda (k v) (set! total (+ total v))))
       (when (> total 0.0)
         (hash-table-walk activation-map
           (lambda (k v)
             (hash-table-set! activation-map k (/ v total)))))))
    ((max)
     (let ((max-val 0.0))
       (hash-table-walk activation-map
         (lambda (k v) (set! max-val (max max-val v))))
       (when (> max-val 0.0)
         (hash-table-walk activation-map
           (lambda (k v)
             (hash-table-set! activation-map k (/ v max-val)))))))
    ((softmax)
     (let ((exp-sum 0.0)
           (exp-values (make-hash-table)))
       (hash-table-walk activation-map
         (lambda (k v)
           (let ((exp-v (exp v)))
             (hash-table-set! exp-values k exp-v)
             (set! exp-sum (+ exp-sum exp-v)))))
       (when (> exp-sum 0.0)
         (hash-table-walk exp-values
           (lambda (k exp-v)
             (hash-table-set! activation-map k (/ exp-v exp-sum)))))))
    (else (error "Invalid normalization strategy" strategy))))

(define (decay-activations! activation-map decay-rate)
  "Apply DECAY-RATE to all activations."
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
  "Convert activation map to sorted list."
  (let ((pairs '()))
    (hash-table-walk activation-map
      (lambda (k v)
        (set! pairs (cons (cons k v) pairs))))
    (sort pairs (lambda (a b) (> (cdr a) (cdr b))))))

;;;
;;; Helper: Propagate to neighbors
;;;

(define (propagate-to-neighbors atomspace current-atom current-activation activation-map 
                                hop-count visited new-queue current-hops params)
  "Propagate activation from CURRENT-ATOM to its neighbors."
  (let ((queue new-queue))
    ;; Get outgoing links (stored in incoming set of atom)
    (let ((outgoing-link-atoms (get-incoming-set current-atom)))
      (for-each
        (lambda (link-atom)
          (when (is-link-type? link-atom)
            ;; Retrieve the link record from atomspace
            (let* ((link-uuid (atom-uuid link-atom))
                   (link-record (atomspace-get-atom atomspace link-uuid)))
              
              (when (and link-record (link? link-record))
                ;; Activate the link
                (let* ((weight (get-weight link-atom params))
                       (propagated (* current-activation 
                                     (params-decay-rate params)
                                     weight))
                       (old-activation (hash-table-ref/default activation-map link-uuid 0.0))
                       (new-activation (+ old-activation propagated)))
                  
                  ;; Update link activation
                  (hash-table-set! activation-map link-uuid new-activation)
                  (hash-table-set! hop-count link-uuid (+ current-hops 1))
                  
                  ;; Add link to queue if not visited
                  (unless (hash-table-ref/default visited link-uuid #f)
                    (set! queue (cons link-atom queue)))
                  
                  ;; Propagate from link to its target atoms
                  ;; For directed links (InheritanceLink), only propagate to targets, not sources
                  ;; Convention: outgoing[0] = source, outgoing[1..n] = targets
                  (let* ((all-outgoing (link-outgoing link-record))
                         (targets (if (> (length all-outgoing) 1)
                                     (cdr all-outgoing)  ; Skip first element (source)
                                     all-outgoing)))     ; For single-element, use all
                    (for-each
                      (lambda (target-item)
                        (let* ((target-atom (if (link? target-item)
                                               (link-atom target-item)
                                               target-item))
                               (target-uuid (atom-uuid target-atom))
                               (old-target-activation (hash-table-ref/default activation-map target-uuid 0.0))
                               (new-target-activation (+ old-target-activation propagated)))
                          
                          ;; Update target activation
                          (hash-table-set! activation-map target-uuid new-target-activation)
                          (hash-table-set! hop-count target-uuid (+ current-hops 1))
                          
                          ;; Add target to queue if not visited
                          (unless (hash-table-ref/default visited target-uuid #f)
                            (set! queue (cons target-atom queue)))))
                      targets)))))))
        outgoing-link-atoms))
    queue))

;;;
;;; Main Propagation Algorithm
;;;

(define (propagate-activation atomspace sources params)
  "Spread activation from SOURCES through ATOMSPACE using PARAMS."
  
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
          (hash-table-set! activation-map uuid initial-activation)
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
               (current-activation (hash-table-ref/default activation-map current-uuid 0.0))
               (current-hops (hash-table-ref/default hop-count current-uuid 0)))
          
          ;; Skip if already visited, below threshold, or max hops reached
          (if (or (hash-table-ref/default visited current-uuid #f)
                  (< current-activation (params-threshold params))
                  (>= current-hops (params-max-hops params)))
              (loop rest-queue)
              (begin
                ;; Mark as visited
                (hash-table-set! visited current-uuid #t)
                
                ;; Propagate to neighbors
                (let ((updated-queue 
                       (propagate-to-neighbors atomspace current current-activation activation-map
                                             hop-count visited rest-queue current-hops params)))
                  (loop updated-queue)))))))
    
    ;; Normalize if requested
    (normalize-activations! activation-map (params-normalization params))
    
    activation-map))

(define (propagate-from-atom atomspace atom initial-activation params)
  "Spread activation from single ATOM."
  (propagate-activation atomspace (list (cons atom initial-activation)) params))

(define (propagate-from-atoms atomspace atoms initial-activation params)
  "Spread activation from multiple ATOMS."
  (let ((sources (map (lambda (atom) (cons atom initial-activation)) atoms)))
    (propagate-activation atomspace sources params)))

;;; spreading-v2.scm ends here
