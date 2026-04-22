;;; AtomSpace - Hypergraph Memory System for Cognitive Kernel
;;; Implements a hypergraph-based memory architecture with tensor shapes
;;; Based on OpenCog AtomSpace concepts adapted for GNU Hurd ecosystem

(define-module (cogkernel atomspace)
  #:use-module (ice-9 hash-table)
  #:use-module (ice-9 match)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:export (make-atom
            make-link
            atom?
            link?
            atom-type
            atom-name
            atom-value
            atom-confidence
            link-type
            link-outgoing
            atomspace-add!
            atomspace-get
            atomspace-query
            atomspace-get-atoms
            atomspace-tensor-shape
            make-atomspace
            atomspace?
            atomspace-atoms
            atomspace-links
            *global-atomspace*))

;;; Atom types for the cognitive kernel
(define atom-types
  '(CONCEPT
    PREDICATE
    VARIABLE
    NUMBER
    STRING
    ISSUE
    CAPABILITY
    BUILD
    TASK
    AGENT
    RULE
    SCRIPT
    MESSAGE
    NETWORK
    EXPERIENCE
    PATTERN
    COMPONENT))

;;; Link types for hypergraph connections
(define link-types
  '(INHERITANCE
    SIMILARITY
    EVALUATION
    EXECUTION
    IMPLICATION
    EQUIVALENCE
    SUBSET
    MEMBER
    CONTEXT))

;;; Atom record structure with tensor coordinates
(define-record-type <atom>
  (make-atom-record type name value confidence tensor-coords)
  atom?
  (type atom-type)
  (name atom-name)  
  (value atom-value set-atom-value!)
  (confidence atom-confidence set-atom-confidence!)
  (tensor-coords atom-tensor-coords set-atom-tensor-coords!))

;;; Link record structure for hypergraph connections
(define-record-type <link>
  (make-link-record type outgoing confidence tensor-coords)
  link?
  (type link-type)
  (outgoing link-outgoing)
  (confidence link-confidence set-link-confidence!)
  (tensor-coords link-tensor-coords set-link-tensor-coords!))

;;; AtomSpace record for hypergraph storage
(define-record-type <atomspace>
  (make-atomspace-record atoms links indices tensor-dims)
  atomspace?
  (atoms atomspace-atoms)        ; Hash table: atom-name -> atom
  (links atomspace-links)        ; Hash table: link-id -> link  
  (indices atomspace-indices)    ; Hash table for queries
  (tensor-dims atomspace-tensor-dims set-atomspace-tensor-dims!))

;;; Create new atom
(define* (make-atom type name #:optional (value #f) (confidence 1.0))
  "Create a new atom with specified type and name"
  (unless (member type atom-types)
    (error "Unknown atom type" type))
  (make-atom-record type name value confidence '(0 0 0 0)))

;;; Create new link
(define* (make-link type outgoing #:optional (confidence 1.0))
  "Create a new link with specified type and outgoing atoms"
  (unless (member type link-types)
    (error "Unknown link type" type))
  (make-link-record type outgoing confidence '(0 0 0 0)))

;;; Create new atomspace
(define* (make-atomspace #:optional (tensor-dims '(1000 1000 100 10)))
  "Create a new atomspace with specified tensor dimensions"
  (make-atomspace-record (make-hash-table) (make-hash-table) (make-hash-table) tensor-dims))

;;; Add atom to atomspace
(define (atomspace-add! atomspace atom)
  "Add an atom to the atomspace"
  (cond
    ((atom? atom)
     (hash-set! (atomspace-atoms atomspace) (atom-name atom) atom))
    ((link? atom)
     (let ((link-id (gensym "link")))
       (hash-set! (atomspace-links atomspace) link-id atom)))
    (else
     (error "Object is neither atom nor link" atom))))

;;; Get atom from atomspace
(define (atomspace-get atomspace name)
  "Get an atom by name from the atomspace"
  (hash-ref (atomspace-atoms atomspace) name))

;;; Get all atoms of a specific type
(define (atomspace-get-atoms atomspace type)
  "Get all atoms of a specific type"
  (filter (lambda (atom)
            (eq? (atom-type atom) type))
          (hash-map->list (lambda (k v) v) (atomspace-atoms atomspace))))

;;; Simple query interface
(define (atomspace-query atomspace pattern)
  "Query the atomspace with a simple pattern"
  (match pattern
    (('type type-name)
     (atomspace-get-atoms atomspace type-name))
    (('name atom-name)
     (let ((atom (atomspace-get atomspace atom-name)))
       (if atom (list atom) '())))
    (else '())))

;;; Get tensor shape for atomspace
(define (atomspace-tensor-shape atomspace)
  "Get the tensor shape representing this atomspace"
  (let ((num-atoms (hash-count (const #t) (atomspace-atoms atomspace)))
        (num-links (hash-count (const #t) (atomspace-links atomspace))))
    (list num-atoms num-links 
          (car (atomspace-tensor-dims atomspace))
          (cadr (atomspace-tensor-dims atomspace)))))

;;; Global atomspace instance
(define *global-atomspace* (make-atomspace))