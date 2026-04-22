;;; test-atomspace.scm --- Unit tests for AtomSpace
;;;
;;; Copyright (C) 2025 Deep Tree Echo Project
;;;
;;; This file contains unit tests for the AtomSpace implementation.
;;;
;;; Code:

(use-modules (srfi srfi-64))  ; Testing library
(add-to-load-path "/home/ubuntu/occ/src")
(use-modules (core memory atomspace))

(test-begin "atomspace-tests")

;;;
;;; Truth Value Tests
;;;

(test-group "truth-value"
  (test-assert "create truth value"
    (truth-value? (make-truth-value #:strength 0.8 #:confidence 0.9 #:count 100)))
  
  (test-equal "truth value strength"
    0.8
    (tv-strength (make-truth-value #:strength 0.8)))
  
  (test-equal "truth value confidence"
    0.9
    (tv-confidence (make-truth-value #:confidence 0.9)))
  
  (test-equal "truth value count"
    100
    (tv-count (make-truth-value #:count 100)))
  
  (test-error "invalid strength (too high)"
    (make-truth-value #:strength 1.5))
  
  (test-error "invalid strength (too low)"
    (make-truth-value #:strength -0.5)))

;;;
;;; Attention Value Tests
;;;

(test-group "attention-value"
  (test-assert "create attention value"
    (attention-value? (make-attention-value #:sti 100 #:lti 50 #:vlti 25)))
  
  (test-equal "attention value STI"
    100
    (av-sti (make-attention-value #:sti 100)))
  
  (test-equal "attention value LTI"
    50
    (av-lti (make-attention-value #:lti 50)))
  
  (test-equal "attention value VLTI"
    25
    (av-vlti (make-attention-value #:vlti 25))))

;;;
;;; Atom Tests
;;;

(test-group "atom"
  (test-assert "create atom"
    (atom? (make-atom 'ConceptNode #:name "Cat")))
  
  (test-equal "atom type"
    'ConceptNode
    (atom-type (make-atom 'ConceptNode #:name "Cat")))
  
  (test-equal "atom name"
    "Cat"
    (atom-name (make-atom 'ConceptNode #:name "Cat")))
  
  (test-assert "atom has truth value"
    (truth-value? (atom-tv (make-atom 'ConceptNode #:name "Cat"))))
  
  (test-assert "atom has attention value"
    (attention-value? (atom-av (make-atom 'ConceptNode #:name "Cat"))))
  
  (test-assert "atom has UUID"
    (number? (atom-uuid (make-atom 'ConceptNode #:name "Cat"))))
  
  (test-assert "atoms have unique UUIDs"
    (let ((atom1 (make-atom 'ConceptNode #:name "Cat"))
          (atom2 (make-atom 'ConceptNode #:name "Dog")))
      (not (= (atom-uuid atom1) (atom-uuid atom2))))))

;;;
;;; Link Tests
;;;

(test-group "link"
  (let* ((cat (make-atom 'ConceptNode #:name "Cat"))
         (animal (make-atom 'ConceptNode #:name "Animal"))
         (inh-link (make-link 'InheritanceLink (list cat animal))))
    
    (test-assert "create link"
      (link? inh-link))
    
    (test-equal "link type"
      'InheritanceLink
      (link-type inh-link))
    
    (test-equal "link outgoing count"
      2
      (length (link-outgoing inh-link)))
    
    (test-assert "link outgoing contains cat"
      (member cat (link-outgoing inh-link)))
    
    (test-assert "link outgoing contains animal"
      (member animal (link-outgoing inh-link)))
    
    (test-assert "cat has incoming link"
      (not (null? (atom-incoming cat))))
    
    (test-assert "animal has incoming link"
      (not (null? (atom-incoming animal))))))

;;;
;;; AtomSpace Tests
;;;

(test-group "atomspace"
  (let ((as (make-atomspace)))
    
    (test-assert "create atomspace"
      (atomspace? as))
    
    (test-equal "empty atomspace size"
      0
      (atomspace-size as))
    
    (test-group "add atoms"
      (let ((cat (create-concept-node as "Cat"))
            (dog (create-concept-node as "Dog")))
        
        (test-equal "atomspace size after adding 2 atoms"
          2
          (atomspace-size as))
        
        (test-assert "can retrieve atom by UUID"
          (atomspace-get-atom as (atom-uuid cat)))
        
        (test-equal "retrieved atom is same"
          cat
          (atomspace-get-atom as (atom-uuid cat)))))
    
    (test-group "get atoms by type"
      (atomspace-clear! as)
      (create-concept-node as "Cat")
      (create-concept-node as "Dog")
      (create-predicate-node as "is-mammal")
      
      (test-equal "get ConceptNodes"
        2
        (length (atomspace-get-atoms-by-type as 'ConceptNode)))
      
      (test-equal "get PredicateNodes"
        1
        (length (atomspace-get-atoms-by-type as 'PredicateNode))))
    
    (test-group "get atoms by name"
      (atomspace-clear! as)
      (create-concept-node as "Cat")
      (create-concept-node as "Cat")  ; Duplicate name
      (create-concept-node as "Dog")
      
      (test-equal "get atoms named 'Cat'"
        2
        (length (get-atoms-by-name as "Cat")))
      
      (test-equal "get atoms named 'Dog'"
        1
        (length (get-atoms-by-name as "Dog"))))
    
    (test-group "remove atoms"
      (atomspace-clear! as)
      (let ((cat (create-concept-node as "Cat")))
        (test-equal "size before removal"
          1
          (atomspace-size as))
        
        (atomspace-remove-atom! as cat)
        
        (test-equal "size after removal"
          0
          (atomspace-size as))
        
        (test-assert "atom no longer retrievable"
          (not (atomspace-get-atom as (atom-uuid cat))))))
    
    (test-group "clear atomspace"
      (create-concept-node as "Cat")
      (create-concept-node as "Dog")
      (create-concept-node as "Bird")
      
      (test-assert "atomspace not empty"
        (> (atomspace-size as) 0))
      
      (atomspace-clear! as)
      
      (test-equal "atomspace empty after clear"
        0
        (atomspace-size as)))))

;;;
;;; Atom Creation Helper Tests
;;;

(test-group "atom-creation-helpers"
  (let ((as (make-atomspace)))
    
    (test-assert "create-concept-node"
      (atom? (create-concept-node as "Cat")))
    
    (test-assert "create-predicate-node"
      (atom? (create-predicate-node as "is-mammal")))
    
    (test-assert "create-variable-node"
      (atom? (create-variable-node as "$X")))
    
    (test-assert "create-number-node"
      (atom? (create-number-node as 42)))
    
    (test-group "create-inheritance-link"
      (atomspace-clear! as)
      (let* ((cat (create-concept-node as "Cat"))
             (animal (create-concept-node as "Animal"))
             (inh (create-inheritance-link as cat animal)))
        
        (test-assert "inheritance link created"
          (link? inh))
        
        (test-equal "inheritance link type"
          'InheritanceLink
          (link-type inh))
        
        (test-equal "inheritance link connects cat and animal"
          (list cat animal)
          (link-outgoing inh))))
    
    (test-group "create-evaluation-link"
      (atomspace-clear! as)
      (let* ((cat (create-concept-node as "Cat"))
             (is-mammal (create-predicate-node as "is-mammal"))
             (eval (create-evaluation-link as is-mammal (list cat))))
        
        (test-assert "evaluation link created"
          (link? eval))
        
        (test-equal "evaluation link type"
          'EvaluationLink
          (link-type eval))))))

;;;
;;; Query Operation Tests
;;;

(test-group "query-operations"
  (let ((as (make-atomspace)))
    
    (test-group "incoming and outgoing sets"
      (let* ((cat (create-concept-node as "Cat"))
             (animal (create-concept-node as "Animal"))
             (inh (create-inheritance-link as cat animal)))
        
        (test-assert "cat has incoming links"
          (not (null? (get-incoming-set cat))))
        
        (test-assert "animal has incoming links"
          (not (null? (get-incoming-set animal))))
        
        (test-equal "inheritance link has 2 outgoing atoms"
          2
          (length (get-outgoing-set inh)))
        
        (test-assert "outgoing set contains cat"
          (member cat (get-outgoing-set inh)))
        
        (test-assert "outgoing set contains animal"
          (member animal (get-outgoing-set inh)))))
    
    (test-group "attentional focus"
      (atomspace-clear! as)
      (let ((cat (create-concept-node as "Cat" 
                   #:av (make-attention-value #:sti 150)))
            (dog (create-concept-node as "Dog"
                   #:av (make-attention-value #:sti 50)))
            (bird (create-concept-node as "Bird"
                    #:av (make-attention-value #:sti 200))))
        
        (test-equal "atoms with STI > 100"
          2
          (length (get-atoms-in-attentional-focus as #:sti-threshold 100)))
        
        (test-equal "atoms with STI > 0"
          3
          (length (get-atoms-in-attentional-focus as #:sti-threshold 0)))))))

;;;
;;; Performance Tests
;;;

(test-group "performance"
  (let ((as (make-atomspace)))
    
    (test-group "large atomspace"
      ;; Create 1000 atoms
      (let ((start-time (get-internal-real-time)))
        (do ((i 0 (+ i 1)))
            ((>= i 1000))
          (create-concept-node as (string-append "Concept" (number->string i))))
        (let ((end-time (get-internal-real-time))
              (elapsed-ms (/ (- end-time start-time) (/ internal-time-units-per-second 1000))))
          
          (test-equal "created 1000 atoms"
            1000
            (atomspace-size as))
          
          (test-assert "creation time reasonable (<1000ms)"
            (< elapsed-ms 1000))
          
          (format #t "~%  Created 1000 atoms in ~ams~%" elapsed-ms)))
      
      ;; Test query performance
      (let* ((start-time (get-internal-real-time))
             (_ (atomspace-get-atoms-by-type as 'ConceptNode))
             (end-time (get-internal-real-time))
             (elapsed-ms (/ (- end-time start-time) (/ internal-time-units-per-second 1000))))
        
        (test-assert "query time reasonable (<100ms)"
          (< elapsed-ms 100))
        
        (format #t "  Queried 1000 atoms in ~ams~%" elapsed-ms))))))

(test-end "atomspace-tests")

;;; test-atomspace.scm ends here
