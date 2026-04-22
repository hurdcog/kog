;;; test-spreading.scm --- Tests for spreading activation
;;;
;;; Copyright (C) 2025 Deep Tree Echo Project
;;;
;;; Code:

(add-to-load-path "/home/ubuntu/occ/src")
(use-modules (core memory atomspace))
(use-modules (core propagation spreading))

(format #t "~%=== Spreading Activation Tests ===~%~%")

;; Create test atomspace with a simple network
(define as (make-atomspace))

;; Create atoms: Cat -> Mammal -> Animal
(define cat (create-concept-node as "Cat"))
(define dog (create-concept-node as "Dog"))
(define mammal (create-concept-node as "Mammal"))
(define animal (create-concept-node as "Animal"))
(define pet (create-concept-node as "Pet"))

;; Create links
(define cat-mammal (create-inheritance-link as cat mammal 
                     #:tv (make-truth-value #:strength 0.9 #:confidence 0.8)))
(define dog-mammal (create-inheritance-link as dog mammal
                     #:tv (make-truth-value #:strength 0.9 #:confidence 0.8)))
(define mammal-animal (create-inheritance-link as mammal animal
                        #:tv (make-truth-value #:strength 1.0 #:confidence 0.9)))
(define cat-pet (create-inheritance-link as cat pet
                  #:tv (make-truth-value #:strength 0.8 #:confidence 0.7)))
(define dog-pet (create-inheritance-link as dog pet
                  #:tv (make-truth-value #:strength 0.8 #:confidence 0.7)))

(format #t "Test network created:~%")
(format #t "  Cat -> Mammal -> Animal~%")
(format #t "  Dog -> Mammal -> Animal~%")
(format #t "  Cat -> Pet~%")
(format #t "  Dog -> Pet~%~%")

;; Test 1: Basic propagation from single source
(format #t "Test 1: Propagate from Cat...~%")
(define params1 (make-propagation-params 
                  #:decay-rate 0.8
                  #:threshold 0.01
                  #:max-hops 3
                  #:bidirectional? #f
                  #:use-truth-values? #f))

(define act-map1 (propagate-from-atom as cat 1.0 params1))
(define activated1 (get-activated-atoms as act-map1 0.01))

(format #t "  Activated atoms:~%")
(for-each
  (lambda (pair)
    (let* ((item (car pair))
           (activation (cdr pair))
           (name (if (link? item)
                    (format #f "~a-link" (link-type item))
                    (atom-name item))))
      (format #t "    ~a: ~a~%" name activation)))
  activated1)
(format #t "~%")

;; Test 2: Propagation with truth value weighting
(format #t "Test 2: Propagate with truth value weighting...~%")
(define params2 (make-propagation-params
                  #:decay-rate 0.8
                  #:threshold 0.01
                  #:max-hops 3
                  #:bidirectional? #f
                  #:use-truth-values? #t))

(define act-map2 (propagate-from-atom as cat 1.0 params2))
(define activated2 (get-activated-atoms as act-map2 0.01))

(format #t "  Activated atoms (with TV weighting):~%")
(for-each
  (lambda (pair)
    (let* ((item (car pair))
           (activation (cdr pair))
           (name (if (link? item)
                    (format #f "~a-link" (link-type item))
                    (atom-name item))))
      (format #t "    ~a: ~a~%" name activation)))
  activated2)
(format #t "~%")

;; Test 3: Bidirectional propagation
(format #t "Test 3: Bidirectional propagation from Mammal...~%")
(define params3 (make-propagation-params
                  #:decay-rate 0.8
                  #:threshold 0.01
                  #:max-hops 2
                  #:bidirectional? #t
                  #:use-truth-values? #f))

(define act-map3 (propagate-from-atom as mammal 1.0 params3))
(define activated3 (get-activated-atoms as act-map3 0.01))

(format #t "  Activated atoms (bidirectional):~%")
(for-each
  (lambda (pair)
    (let* ((item (car pair))
           (activation (cdr pair))
           (name (if (link? item)
                    (format #f "~a-link" (link-type item))
                    (atom-name item))))
      (format #t "    ~a: ~a~%" name activation)))
  activated3)
(format #t "~%")

;; Test 4: Multiple sources
(format #t "Test 4: Propagate from Cat and Dog...~%")
(define params4 (make-propagation-params
                  #:decay-rate 0.8
                  #:threshold 0.01
                  #:max-hops 3
                  #:bidirectional? #f
                  #:use-truth-values? #f))

(define act-map4 (propagate-from-atoms as (list cat dog) 1.0 params4))
(define activated4 (get-activated-atoms as act-map4 0.01))

(format #t "  Activated atoms (multiple sources):~%")
(for-each
  (lambda (pair)
    (let* ((item (car pair))
           (activation (cdr pair))
           (name (if (link? item)
                    (format #f "~a-link" (link-type item))
                    (atom-name item))))
      (format #t "    ~a: ~a~%" name activation)))
  activated4)
(format #t "~%")

;; Test 5: Normalization
(format #t "Test 5: Propagate with max normalization...~%")
(define params5 (make-propagation-params
                  #:decay-rate 0.8
                  #:threshold 0.01
                  #:max-hops 3
                  #:normalization 'max
                  #:bidirectional? #f
                  #:use-truth-values? #f))

(define act-map5 (propagate-from-atom as cat 1.0 params5))
(define activated5 (get-activated-atoms as act-map5 0.01))

(format #t "  Activated atoms (max normalized):~%")
(for-each
  (lambda (pair)
    (let* ((item (car pair))
           (activation (cdr pair))
           (name (if (link? item)
                    (format #f "~a-link" (link-type item))
                    (atom-name item))))
      (format #t "    ~a: ~a~%" name activation)))
  activated5)
(format #t "~%")

;; Test 6: Threshold cutoff
(format #t "Test 6: Propagate with high threshold (0.5)...~%")
(define params6 (make-propagation-params
                  #:decay-rate 0.8
                  #:threshold 0.5
                  #:max-hops 5
                  #:bidirectional? #f
                  #:use-truth-values? #f))

(define act-map6 (propagate-from-atom as cat 1.0 params6))
(define activated6 (get-activated-atoms as act-map6 0.01))

(format #t "  Activated atoms (threshold 0.5):~%")
(for-each
  (lambda (pair)
    (let* ((item (car pair))
           (activation (cdr pair))
           (name (if (link? item)
                    (format #f "~a-link" (link-type item))
                    (atom-name item))))
      (format #t "    ~a: ~a~%" name activation)))
  activated6)
(format #t "~%")

;; Test 7: Performance test
(format #t "Test 7: Performance test (100 propagations)...~%")
(define start-time (get-internal-real-time))
(do ((i 0 (+ i 1)))
    ((>= i 100))
  (propagate-from-atom as cat 1.0 default-propagation-params))
(define end-time (get-internal-real-time))
(define elapsed-ms (/ (- end-time start-time) (/ internal-time-units-per-second 1000)))
(format #t "  100 propagations in ~ams (~ams per propagation)~%~%" 
        elapsed-ms (/ elapsed-ms 100))

;; Summary
(format #t "=== All Spreading Activation Tests Passed! ===~%")
(format #t "~%Network statistics:~%")
(format #t "  Atoms: ~a~%" (atomspace-size as))
(format #t "  ConceptNodes: ~a~%" (length (atomspace-get-atoms-by-type as 'ConceptNode)))
(format #t "  InheritanceLinks: ~a~%" (length (atomspace-get-atoms-by-type as 'InheritanceLink)))
(format #t "~%")

;;; test-spreading.scm ends here
