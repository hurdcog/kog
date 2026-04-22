;;; test-inference.scm --- Tests for Symbolic Reasoning Engine
;;;
;;; Copyright (C) 2025 Deep Tree Echo Project

(add-to-load-path "/home/ubuntu/occ/src")
(use-modules (core memory atomspace))
(use-modules (core grammar inference))
(use-modules (srfi srfi-64))  ; Testing library

(test-begin "inference-engine")

;;;
;;; Test Unification
;;;

(test-group "unification"
  
  ;; Test variable unification
  (test-assert "unify variable with atom"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (bindings (unify '?X cat '())))
      (and bindings
           (equal? (get-bindings '?X bindings) cat))))
  
  ;; Test two variables
  (test-assert "unify two variables"
    (let ((bindings (unify '?X '?Y '())))
      (and bindings
           (assoc '?X bindings))))
  
  ;; Test identical patterns
  (test-assert "unify identical patterns"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (bindings (unify cat cat '())))
      (and bindings (list? bindings))))
  
  ;; Test list unification
  (test-assert "unify list patterns"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (mammal (create-concept-node as "Mammal"))
           (pattern1 '(ImplicationLink ?X ?Y))
           (pattern2 (list 'ImplicationLink cat mammal))
           (bindings (unify pattern1 pattern2 '())))
      (and bindings
           (equal? (get-bindings '?X bindings) cat)
           (equal? (get-bindings '?Y bindings) mammal))))
  
  ;; Test unification failure
  (test-assert "unification fails for incompatible patterns"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (dog (create-concept-node as "Dog"))
           (bindings (unify cat dog '())))
      (not bindings))))

;;;
;;; Test Pattern Instantiation
;;;

(test-group "pattern-instantiation"
  
  ;; Test simple instantiation
  (test-assert "instantiate simple pattern"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (bindings (list (cons '?X cat)))
           (result (instantiate-pattern '?X bindings)))
      (equal? result cat)))
  
  ;; Test list instantiation
  (test-assert "instantiate list pattern"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (mammal (create-concept-node as "Mammal"))
           (bindings (list (cons '?X cat) (cons '?Y mammal)))
           (pattern '(ImplicationLink ?X ?Y))
           (result (instantiate-pattern pattern bindings)))
      (and (list? result)
           (= (length result) 3)
           (equal? (cadr result) cat)
           (equal? (caddr result) mammal)))))

;;;
;;; Test Inference Rules
;;;

(test-group "inference-rules"
  
  ;; Test modus ponens structure
  (test-assert "modus ponens rule exists"
    (and (inference-rule? modus-ponens)
         (equal? (rule-name modus-ponens) 'modus-ponens)
         (= (length (rule-premises modus-ponens)) 2)
         (= (rule-confidence modus-ponens) 1.0)))
  
  ;; Test syllogism structure
  (test-assert "syllogism rule exists"
    (and (inference-rule? syllogism)
         (equal? (rule-name syllogism) 'syllogism)
         (= (length (rule-premises syllogism)) 2)))
  
  ;; Test abduction has lower confidence
  (test-assert "abduction has lower confidence"
    (< (rule-confidence abduction) 1.0)))

;;;
;;; Test Forward Chaining
;;;

(test-group "forward-chaining"
  
  ;; Test simple forward chaining
  (test-assert "forward chain derives new knowledge"
    (let* ((as (make-atomspace))
           ;; Create knowledge: Cat, Cat->Mammal, Mammal->Animal
           (cat (create-concept-node as "Cat"))
           (mammal (create-concept-node as "Mammal"))
           (animal (create-concept-node as "Animal"))
           (cat-mammal (create-inheritance-link as cat mammal))
           (mammal-animal (create-inheritance-link as mammal animal))
           ;; Create transitivity rule: If (X->Y) and (Y->Z), then (X->Z)
           (rule (make-inference-rule
                   'transitive-inheritance
                   (list (list 'InheritanceLink '?X '?Y)
                         (list 'InheritanceLink '?Y '?Z))
                   (list 'InheritanceLink '?X '?Z)
                   1.0))
           ;; Apply forward chaining
           (new-atoms (forward-chain as (list rule) 1)))
      ;; Should derive Cat->Animal
      (> (length new-atoms) 0))))

;;;
;;; Test Backward Chaining
;;;

(test-group "backward-chaining"
  
  ;; Test simple backward chaining
  (test-assert "backward chain proves goal"
    (let* ((as (make-atomspace))
           ;; Create knowledge: Cat, Cat->Mammal
           (cat (create-concept-node as "Cat"))
           (mammal (create-concept-node as "Mammal"))
           (cat-mammal (create-inheritance-link as cat mammal)))
      ;; Add Cat to atomspace
      (atomspace-add-atom! as cat)
      ;; Create rule
      (let* ((rule (make-inference-rule
                     'simple-inheritance
                     (list (list 'InheritanceLink '?X '?Y) '?X)
                     '?Y
                     1.0))
             ;; Try to prove Mammal
             (proof (backward-chain as (list rule) mammal 5)))
        ;; Should find proof
        (and proof (list? proof))))))

;;;
;;; Test Logical Operators
;;;

(test-group "logical-operators"
  
  ;; Test eval-truth
  (test-assert "eval-truth with high confidence"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat")))
      ;; Default TV has strength 1.0
      (eval-truth cat)))
  
  ;; Test eval-and
  (test-assert "eval-and with true operands"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (dog (create-concept-node as "Dog")))
      (eval-and (list cat dog) as)))
  
  ;; Test eval-or
  (test-assert "eval-or with mixed operands"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat")))
      (eval-or (list cat) as))))

;;;
;;; Test Variable Detection
;;;

(test-group "variables"
  
  ;; Test variable detection
  (test-assert "detect variable"
    (variable? '?X))
  
  (test-assert "non-variable symbol"
    (not (variable? 'Cat)))
  
  (test-assert "make variable"
    (variable? (make-variable 'X))))

;;;
;;; Performance Tests
;;;

(test-group "performance"
  
  ;; Test forward chaining performance
  (test-assert "forward chain completes in reasonable time"
    (let* ((as (make-atomspace))
           ;; Create 10 concepts
           (concepts (map (lambda (i)
                           (create-concept-node as (string-append "Concept" (number->string i))))
                         (iota 10)))
           ;; Create inheritance links
           (_ (for-each (lambda (i)
                         (when (< i 9)
                           (create-inheritance-link as
                                                   (list-ref concepts i)
                                                   (list-ref concepts (+ i 1)))))
                       (iota 10)))
           ;; Create rule
           (rule (make-inference-rule
                   'transitive-inheritance
                   (list (list 'InheritanceLink '?X '?Y)
                         (list 'InheritanceLink '?Y '?Z))
                   (list 'ImplicationLink '?X '?Z)
                   1.0))
           ;; Measure time
           (start-time (get-internal-real-time))
           (new-atoms (forward-chain as (list rule) 3))
           (end-time (get-internal-real-time))
           (elapsed-ms (/ (- end-time start-time) 1000.0)))
      ;; Should complete in < 1000ms
      (< elapsed-ms 1000))))

(test-end "inference-engine")

;;; test-inference.scm ends here
