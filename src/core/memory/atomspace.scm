;;; atomspace.scm --- Hypergraph Memory Space: Atom and Link Definitions
;;;
;;; Copyright (C) 2025 Deep Tree Echo Project
;;;
;;; This file implements the foundational atom and link structures for the
;;; Hypergraph Memory Space. It provides the core data structures and operations
;;; for representing knowledge as a hypergraph.
;;;
;;; Commentary:
;;;
;;; The AtomSpace is inspired by OpenCog's AtomSpace but simplified for the
;;; Deep Tree Echo Architecture. It represents knowledge as a hypergraph where:
;;; - Atoms are vertices (nodes) representing concepts, predicates, or values
;;; - Links are hyperedges connecting multiple atoms
;;; - Truth values represent uncertainty and confidence
;;; - Attention values enable focus and resource allocation
;;;
;;; Code:

(define-module (core memory atomspace)
  #:use-module (srfi srfi-1)   ; List library
  #:use-module (srfi srfi-9)   ; Record types
  #:use-module (srfi srfi-69)  ; Hash tables
  #:use-module (ice-9 match)   ; Pattern matching
  #:export (
    ;; Atom types
    make-atom
    atom?
    atom-type
    atom-name
    atom-tv
    atom-set-tv!
    atom-av
    atom-set-av!
    atom-incoming
    atom-uuid
    
    ;; Truth value
    make-truth-value
    truth-value?
    tv-strength
    tv-confidence
    tv-count
    
    ;; Attention value
    make-attention-value
    attention-value?
    av-sti
    av-lti
    av-vlti
    
    ;; Link types
    make-link
    link?
    link-atom
    link-type
    link-tv
    link-av
    link-uuid
    link-outgoing
    
    ;; AtomSpace operations
    make-atomspace
    atomspace?
    atomspace-add-atom!
    atomspace-remove-atom!
    atomspace-get-atom
    atomspace-get-atoms-by-type
    atomspace-get-all-atoms
    atomspace-size
    atomspace-clear!
    
    ;; Atom creation helpers
    create-concept-node
    create-predicate-node
    create-variable-node
    create-number-node
    create-inheritance-link
    create-evaluation-link
    create-list-link
    
    ;; Query operations
    get-incoming-set
    get-outgoing-set
    get-atoms-by-name
    get-atoms-in-attentional-focus
  ))

;;;
;;; Truth Value - Represents probabilistic truth with strength and confidence
;;;

(define-record-type <truth-value>
  (%make-truth-value strength confidence count)
  truth-value?
  (strength tv-strength)      ; Probability of truth [0.0, 1.0]
  (confidence tv-confidence)  ; Confidence in the strength [0.0, 1.0]
  (count tv-count))          ; Evidence count (for Bayesian updates)

(define* (make-truth-value #:key (strength 1.0) (confidence 0.0) (count 0))
  "Create a truth value with STRENGTH, CONFIDENCE, and COUNT.
  
  STRENGTH: Probability that the statement is true (0.0 to 1.0)
  CONFIDENCE: Confidence in the strength value (0.0 to 1.0)
  COUNT: Amount of evidence supporting this truth value
  
  Example:
    (make-truth-value #:strength 0.8 #:confidence 0.9 #:count 100)"
  (unless (and (>= strength 0.0) (<= strength 1.0))
    (error "Truth value strength must be between 0.0 and 1.0" strength))
  (unless (and (>= confidence 0.0) (<= confidence 1.0))
    (error "Truth value confidence must be between 0.0 and 1.0" confidence))
  (%make-truth-value strength confidence count))

(define (tv-to-string tv)
  "Convert truth value to string representation."
  (format #f "<TV: strength=~a confidence=~a count=~a>"
          (tv-strength tv)
          (tv-confidence tv)
          (tv-count tv)))

;;;
;;; Attention Value - Represents importance for resource allocation
;;;

(define-record-type <attention-value>
  (%make-attention-value sti lti vlti)
  attention-value?
  (sti av-sti)    ; Short-term importance
  (lti av-lti)    ; Long-term importance
  (vlti av-vlti)) ; Very long-term importance

(define* (make-attention-value #:key (sti 0) (lti 0) (vlti 0))
  "Create an attention value with STI, LTI, and VLTI.
  
  STI: Short-term importance (current focus)
  LTI: Long-term importance (persistent relevance)
  VLTI: Very long-term importance (core knowledge)
  
  Example:
    (make-attention-value #:sti 100 #:lti 50)"
  (%make-attention-value sti lti vlti))

;;;
;;; Atom - Base type for all atoms (nodes and links)
;;;

(define *atom-uuid-counter* 0)

(define (generate-uuid)
  "Generate a unique identifier for an atom."
  (set! *atom-uuid-counter* (+ *atom-uuid-counter* 1))
  *atom-uuid-counter*)

(define-record-type <atom>
  (%make-atom type name tv av incoming uuid)
  atom?
  (type atom-type)              ; Symbol representing atom type
  (name atom-name)              ; String name (for nodes) or #f (for links)
  (tv atom-tv atom-set-tv!)     ; Truth value
  (av atom-av atom-set-av!)     ; Attention value
  (incoming atom-incoming atom-set-incoming!) ; List of atoms pointing to this atom
  (uuid atom-uuid))             ; Unique identifier

(define* (make-atom type #:key (name #f) (tv (make-truth-value)) (av (make-attention-value)))
  "Create an atom of TYPE with optional NAME, truth value TV, and attention value AV.
  
  TYPE: Symbol representing the atom type (e.g., 'ConceptNode, 'InheritanceLink)
  NAME: String name for nodes, #f for links
  TV: Truth value (defaults to strength=1.0, confidence=0.0)
  AV: Attention value (defaults to all zeros)
  
  Example:
    (make-atom 'ConceptNode #:name \"Cat\" #:tv (make-truth-value #:strength 0.9))"
  (%make-atom type name tv av '() (generate-uuid)))

;;;
;;; Link - Specialized atom connecting other atoms
;;;

(define-record-type <link>
  (%make-link atom outgoing)
  link?
  (atom link-atom)              ; The underlying atom structure
  (outgoing link-outgoing))     ; List of atoms this link connects

(define* (make-link type outgoing #:key (tv (make-truth-value)) (av (make-attention-value)))
  "Create a link of TYPE connecting OUTGOING atoms or links.
  
  TYPE: Symbol representing the link type (e.g., 'InheritanceLink, 'EvaluationLink)
  OUTGOING: List of atoms or links this link connects
  TV: Truth value
  AV: Attention value
  
  Example:
    (make-link 'InheritanceLink (list cat-atom animal-atom))"
  (unless (list? outgoing)
    (error "Link outgoing must be a list" outgoing))
  (unless (every (lambda (x) (or (atom? x) (link? x))) outgoing)
    (error "All outgoing elements must be atoms or links" outgoing))
  (let ((atom (make-atom type #:name #f #:tv tv #:av av)))
    ;; Add this link to incoming set of all outgoing atoms/links
    (for-each
     (lambda (out-item)
       (let ((out-atom (if (link? out-item)
                          (link-atom out-item)
                          out-item)))
         (atom-set-incoming! out-atom
                            (cons atom (atom-incoming out-atom)))))
     outgoing)
    (%make-link atom outgoing)))

;; Helper to get atom from link
(define (link-type link)
  "Get the type of a link."
  (atom-type (link-atom link)))

(define (link-tv link)
  "Get the truth value of a link."
  (atom-tv (link-atom link)))

(define (link-av link)
  "Get the attention value of a link."
  (atom-av (link-atom link)))

(define (link-uuid link)
  "Get the UUID of a link."
  (atom-uuid (link-atom link)))

;;;
;;; AtomSpace - Container for all atoms
;;;

(define-record-type <atomspace>
  (%make-atomspace atoms-by-uuid atoms-by-type atoms-by-name)
  atomspace?
  (atoms-by-uuid atomspace-atoms-by-uuid)     ; Hash table: uuid -> atom
  (atoms-by-type atomspace-atoms-by-type)     ; Hash table: type -> list of atoms
  (atoms-by-name atomspace-atoms-by-name))    ; Hash table: name -> list of atoms

(define (make-atomspace)
  "Create a new empty atomspace.
  
  Example:
    (define as (make-atomspace))"
  (%make-atomspace
   (make-hash-table)  ; atoms-by-uuid
   (make-hash-table)  ; atoms-by-type
   (make-hash-table))) ; atoms-by-name

(define (atomspace-add-atom! as atom-or-link)
  "Add an atom or link to the atomspace AS.
  
  Returns the added atom/link.
  
  Example:
    (atomspace-add-atom! as (make-atom 'ConceptNode #:name \"Cat\"))"
  (let* ((atom (if (link? atom-or-link)
                   (link-atom atom-or-link)
                   atom-or-link))
         (uuid (atom-uuid atom))
         (type (atom-type atom))
         (name (atom-name atom))
         (by-uuid (atomspace-atoms-by-uuid as))
         (by-type (atomspace-atoms-by-type as))
         (by-name (atomspace-atoms-by-name as)))
    
    ;; Add to uuid index
    (hash-table-set! by-uuid uuid atom-or-link)
    
    ;; Add to type index
    (let ((type-list (hash-table-ref/default by-type type '())))
      (hash-table-set! by-type type (cons atom-or-link type-list)))
    
    ;; Add to name index (if atom has a name)
    (when name
      (let ((name-list (hash-table-ref/default by-name name '())))
        (hash-table-set! by-name name (cons atom-or-link name-list))))
    
    atom-or-link))

(define (atomspace-remove-atom! as atom-or-link)
  "Remove an atom or link from the atomspace AS.
  
  Example:
    (atomspace-remove-atom! as my-atom)"
  (let* ((atom (if (link? atom-or-link)
                   (link-atom atom-or-link)
                   atom-or-link))
         (uuid (atom-uuid atom))
         (type (atom-type atom))
         (name (atom-name atom))
         (by-uuid (atomspace-atoms-by-uuid as))
         (by-type (atomspace-atoms-by-type as))
         (by-name (atomspace-atoms-by-name as)))
    
    ;; Remove from uuid index
    (hash-table-delete! by-uuid uuid)
    
    ;; Remove from type index
    (let ((type-list (hash-table-ref/default by-type type '())))
      (hash-table-set! by-type type (delete atom-or-link type-list)))
    
    ;; Remove from name index
    (when name
      (let ((name-list (hash-table-ref/default by-name name '())))
        (hash-table-set! by-name name (delete atom-or-link name-list))))))

(define (atomspace-get-atom as uuid)
  "Get an atom from atomspace AS by UUID.
  
  Example:
    (atomspace-get-atom as 42)"
  (hash-table-ref/default (atomspace-atoms-by-uuid as) uuid #f))

(define (atomspace-get-atoms-by-type as type)
  "Get all atoms of TYPE from atomspace AS.
  
  Example:
    (atomspace-get-atoms-by-type as 'ConceptNode)"
  (hash-table-ref/default (atomspace-atoms-by-type as) type '()))

(define (atomspace-get-all-atoms as)
  "Get all atoms from atomspace AS.
  
  Example:
    (atomspace-get-all-atoms as)"
  (hash-table-values (atomspace-atoms-by-uuid as)))

(define (atomspace-size as)
  "Get the number of atoms in atomspace AS.
  
  Example:
    (atomspace-size as)"
  (hash-table-size (atomspace-atoms-by-uuid as)))

(define (atomspace-clear! as)
  "Remove all atoms from atomspace AS.
  
  Example:
    (atomspace-clear! as)"
  (let ((by-uuid (atomspace-atoms-by-uuid as))
        (by-type (atomspace-atoms-by-type as))
        (by-name (atomspace-atoms-by-name as)))
    (hash-table-walk by-uuid (lambda (k v) (hash-table-delete! by-uuid k)))
    (hash-table-walk by-type (lambda (k v) (hash-table-delete! by-type k)))
    (hash-table-walk by-name (lambda (k v) (hash-table-delete! by-name k)))))

;;;
;;; Atom Creation Helpers
;;;

(define* (create-concept-node as name #:key (tv (make-truth-value)) (av (make-attention-value)))
  "Create a ConceptNode with NAME in atomspace AS.
  
  Example:
    (create-concept-node as \"Cat\")"
  (let ((atom (make-atom 'ConceptNode #:name name #:tv tv #:av av)))
    (atomspace-add-atom! as atom)))

(define* (create-predicate-node as name #:key (tv (make-truth-value)) (av (make-attention-value)))
  "Create a PredicateNode with NAME in atomspace AS.
  
  Example:
    (create-predicate-node as \"is-mammal\")"
  (let ((atom (make-atom 'PredicateNode #:name name #:tv tv #:av av)))
    (atomspace-add-atom! as atom)))

(define* (create-variable-node as name #:key (tv (make-truth-value)) (av (make-attention-value)))
  "Create a VariableNode with NAME in atomspace AS.
  
  Example:
    (create-variable-node as \"$X\")"
  (let ((atom (make-atom 'VariableNode #:name name #:tv tv #:av av)))
    (atomspace-add-atom! as atom)))

(define* (create-number-node as number #:key (tv (make-truth-value)) (av (make-attention-value)))
  "Create a NumberNode with NUMBER in atomspace AS.
  
  Example:
    (create-number-node as 42)"
  (let ((atom (make-atom 'NumberNode #:name (number->string number) #:tv tv #:av av)))
    (atomspace-add-atom! as atom)))

(define* (create-inheritance-link as child parent #:key (tv (make-truth-value)) (av (make-attention-value)))
  "Create an InheritanceLink from CHILD to PARENT in atomspace AS.
  
  Represents: CHILD is-a PARENT
  
  Example:
    (create-inheritance-link as cat-node animal-node)"
  (let ((link (make-link 'InheritanceLink (list child parent) #:tv tv #:av av)))
    (atomspace-add-atom! as link)))

(define* (create-evaluation-link as predicate arguments #:key (tv (make-truth-value)) (av (make-attention-value)))
  "Create an EvaluationLink with PREDICATE and ARGUMENTS in atomspace AS.
  
  Represents: PREDICATE(ARGUMENTS)
  
  Example:
    (create-evaluation-link as is-mammal-pred (list cat-node))"
  (let* ((list-link (make-link 'ListLink arguments #:tv (make-truth-value) #:av (make-attention-value)))
         (eval-link (make-link 'EvaluationLink (list predicate list-link) #:tv tv #:av av)))
    (atomspace-add-atom! as list-link)
    (atomspace-add-atom! as eval-link)))

(define* (create-list-link as elements #:key (tv (make-truth-value)) (av (make-attention-value)))
  "Create a ListLink containing ELEMENTS in atomspace AS.
  
  Example:
    (create-list-link as (list atom1 atom2 atom3))"
  (let ((link (make-link 'ListLink elements #:tv tv #:av av)))
    (atomspace-add-atom! as link)))

;;;
;;; Query Operations
;;;

(define (get-incoming-set atom-or-link)
  "Get all atoms/links that point to ATOM-OR-LINK.
  
  Example:
    (get-incoming-set my-atom)"
  (let ((atom (if (link? atom-or-link)
                  (link-atom atom-or-link)
                  atom-or-link)))
    (atom-incoming atom)))

(define (get-outgoing-set link)
  "Get all atoms that LINK points to.
  
  Example:
    (get-outgoing-set my-link)"
  (if (link? link)
      (link-outgoing link)
      '()))

(define (get-atoms-by-name as name)
  "Get all atoms with NAME from atomspace AS.
  
  Example:
    (get-atoms-by-name as \"Cat\")"
  (hash-table-ref/default (atomspace-atoms-by-name as) name '()))

(define* (get-atoms-in-attentional-focus as #:key (sti-threshold 0))
  "Get all atoms with STI above STI-THRESHOLD from atomspace AS.
  
  Example:
    (get-atoms-in-attentional-focus as #:sti-threshold 100)"
  (filter
   (lambda (atom-or-link)
     (let* ((atom (if (link? atom-or-link)
                      (link-atom atom-or-link)
                      atom-or-link))
            (av (atom-av atom))
            (sti (av-sti av)))
       (> sti sti-threshold)))
   (atomspace-get-all-atoms as)))

;;;
;;; Display and Debugging
;;;

(define (atom-to-string atom-or-link)
  "Convert atom or link to string representation."
  (if (link? atom-or-link)
      (let* ((link atom-or-link)
             (atom (link-atom link))
             (type (atom-type atom))
             (outgoing (link-outgoing link))
             (tv (atom-tv atom)))
        (format #f "(~a ~a ~a)"
                type
                (map (lambda (a) (if (link? a)
                                    (atom-to-string a)
                                    (atom-name a)))
                     outgoing)
                (tv-to-string tv)))
      (let* ((atom atom-or-link)
             (type (atom-type atom))
             (name (atom-name atom))
             (tv (atom-tv atom)))
        (format #f "(~a \"~a\" ~a)"
                type
                (or name "")
                (tv-to-string tv)))))

(define (display-atomspace as)
  "Display all atoms in atomspace AS."
  (format #t "AtomSpace (size: ~a)~%" (atomspace-size as))
  (for-each
   (lambda (atom)
     (format #t "  ~a~%" (atom-to-string atom)))
   (atomspace-get-all-atoms as)))

;;; atomspace.scm ends here
