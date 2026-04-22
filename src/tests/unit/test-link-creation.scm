;;; test-link-creation.scm --- Test Link Creation in Forward Chaining
;;;
;;; Copyright (C) 2025 Deep Tree Echo Project

(add-to-load-path "/home/ubuntu/occ/src")
(use-modules (core memory atomspace))
(use-modules (core grammar inference))
(use-modules (srfi srfi-64))  ; Testing library

(test-begin "link-creation")

;;;
;;; Test Transitivity (Cat -> Mammal -> Animal => Cat -> Animal)
;;;

(test-group "transitivity"
  
  (test-assert "derive transitive inheritance"
    (let* ((as (make-atomspace))
           ;; Create knowledge: Cat -> Mammal -> Animal
           (cat (create-concept-node as "Cat"))
           (mammal (create-concept-node as "Mammal"))
           (animal (create-concept-node as "Animal"))
           (cat-mammal (create-inheritance-link as cat mammal))
           (mammal-animal (create-inheritance-link as mammal animal))
           
           ;; Create transitivity rule
           (rule (make-inference-rule
                   'transitive-inheritance
                   (list (list 'InheritanceLink '?X '?Y)
                         (list 'InheritanceLink '?Y '?Z))
                   (list 'InheritanceLink '?X '?Z)
                   1.0))
           
           ;; Apply forward chaining
           (new-atoms (forward-chain as (list rule) 1)))
      
      ;; Should derive Cat -> Animal
      (and (> (length new-atoms) 0)
           ;; Verify the derived link exists
           (let ((cat-animal-links (atomspace-get-atoms-by-type as 'InheritanceLink)))
             (format #t "~%Total InheritanceLinks: ~a~%" (length cat-animal-links))
             (format #t "New atoms derived: ~a~%~%" (length new-atoms))
             ;; Should have 3 inheritance links: Cat->Mammal, Mammal->Animal, Cat->Animal
             (= (length cat-animal-links) 3))))))

;;;
;;; Test Syllogism (P -> Q, Q -> R => P -> R)
;;;

(test-group "syllogism"
  
  (test-assert "derive syllogistic implication"
    (let* ((as (make-atomspace))
           ;; Create knowledge
           (p (create-concept-node as "P"))
           (q (create-concept-node as "Q"))
           (r (create-concept-node as "R"))
           
           ;; Create implications: P -> Q, Q -> R
           (p-q (create-implication-link as p q))
           (q-r (create-implication-link as q r))
           
           ;; Apply syllogism rule
           (new-atoms (forward-chain as (list syllogism) 1)))
      
      ;; Should derive P -> R
      (and (> (length new-atoms) 0)
           (let ((impl-links (atomspace-get-atoms-by-type as 'ImplicationLink)))
             (format #t "~%Total ImplicationLinks: ~a~%" (length impl-links))
             (format #t "New atoms derived: ~a~%~%" (length new-atoms))
             ;; Should have 3 implication links
             (= (length impl-links) 3))))))

;;;
;;; Test Analogy (similar(A, B) and P(A) => P(B))
;;;

(test-group "analogy"
  
  (test-assert "derive analogical property"
    (let* ((as (make-atomspace))
           ;; Create knowledge
           (dog (create-concept-node as "Dog"))
           (wolf (create-concept-node as "Wolf"))
           (predator (create-predicate-node as "Predator"))
           
           ;; Dog and Wolf are similar
           (dog-wolf-sim (create-similarity-link as dog wolf))
           
           ;; Dog is a Predator
           (dog-pred (create-evaluation-link as predator (list dog)))
           
           ;; Apply analogy rule
           (new-atoms (forward-chain as (list analogy) 1)))
      
      ;; Should derive Wolf is a Predator
      (and (> (length new-atoms) 0)
           (let ((eval-links (atomspace-get-atoms-by-type as 'EvaluationLink)))
             (format #t "~%Total EvaluationLinks: ~a~%" (length eval-links))
             (format #t "New atoms derived: ~a~%~%" (length new-atoms))
             ;; Should have 2 evaluation links
             (= (length eval-links) 2))))))

;;;
;;; Test Multi-Step Inference
;;;

(test-group "multi-step"
  
  (test-assert "derive knowledge through multiple steps"
    (let* ((as (make-atomspace))
           ;; Create knowledge chain: A -> B -> C -> D
           (a (create-concept-node as "A"))
           (b (create-concept-node as "B"))
           (c (create-concept-node as "C"))
           (d (create-concept-node as "D"))
           (a-b (create-inheritance-link as a b))
           (b-c (create-inheritance-link as b c))
           (c-d (create-inheritance-link as c d))
           
           ;; Create transitivity rule
           (rule (make-inference-rule
                   'transitive-inheritance
                   (list (list 'InheritanceLink '?X '?Y)
                         (list 'InheritanceLink '?Y '?Z))
                   (list 'InheritanceLink '?X '?Z)
                   1.0))
           
           ;; Apply forward chaining with depth 3
           (new-atoms (forward-chain as (list rule) 3)))
      
      ;; Should derive: A->C, B->D, A->D
      (and (> (length new-atoms) 0)
           (let ((inh-links (atomspace-get-atoms-by-type as 'InheritanceLink)))
             (format #t "~%Total InheritanceLinks: ~a~%" (length inh-links))
             (format #t "New atoms derived: ~a~%~%" (length new-atoms))
             ;; Should have original 3 + derived 3 = 6 links
             (>= (length inh-links) 6))))))

;;;
;;; Test Link Creation from Pattern
;;;

(test-group "create-link-from-pattern"
  
  (test-assert "create inheritance link from pattern"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (mammal (create-concept-node as "Mammal"))
           (pattern (list 'InheritanceLink cat mammal))
           (link (create-link-from-pattern as pattern)))
      (and link
           (link? link)
           (equal? (link-type link) 'InheritanceLink))))
  
  (test-assert "create implication link from pattern"
    (let* ((as (make-atomspace))
           (p (create-concept-node as "P"))
           (q (create-concept-node as "Q"))
           (pattern (list 'ImplicationLink p q))
           (link (create-link-from-pattern as pattern)))
      (and link
           (link? link)
           (equal? (link-type link) 'ImplicationLink))))
  
  (test-assert "fail on invalid pattern"
    (let* ((as (make-atomspace))
           (pattern '(InvalidLink))
           (link (create-link-from-pattern as pattern)))
      (not link))))

;;;
;;; Performance Test
;;;

(test-group "performance"
  
  (test-assert "derive many transitive links efficiently"
    (let* ((as (make-atomspace))
           ;; Create chain of 5 concepts
           (concepts (map (lambda (i)
                           (create-concept-node as (string-append "C" (number->string i))))
                         (iota 5)))
           ;; Create inheritance chain
           (_ (for-each (lambda (i)
                         (when (< i 4)
                           (create-inheritance-link as
                                                   (list-ref concepts i)
                                                   (list-ref concepts (+ i 1)))))
                       (iota 5)))
           ;; Create transitivity rule
           (rule (make-inference-rule
                   'transitive-inheritance
                   (list (list 'InheritanceLink '?X '?Y)
                         (list 'InheritanceLink '?Y '?Z))
                   (list 'InheritanceLink '?X '?Z)
                   1.0))
           ;; Measure time
           (start-time (get-internal-real-time))
           (new-atoms (forward-chain as (list rule) 3))
           (end-time (get-internal-real-time))
           (elapsed-ms (/ (- end-time start-time) 1000.0)))
      
      (format #t "~%Derived ~a new links in ~a ms~%" (length new-atoms) elapsed-ms)
      
      ;; Should complete in < 500ms and derive multiple links
      (and (< elapsed-ms 500)
           (> (length new-atoms) 0)))))

(test-end "link-creation")

;;; test-link-creation.scm ends here
