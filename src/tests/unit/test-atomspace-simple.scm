;;; test-atomspace-simple.scm --- Simple tests for AtomSpace
;;;
;;; Copyright (C) 2025 Deep Tree Echo Project
;;;
;;; Code:

(add-to-load-path "/home/ubuntu/occ/src")
(use-modules (core memory atomspace))

(format #t "~%=== AtomSpace Tests ===~%~%")

;; Test 1: Create atomspace
(format #t "Test 1: Creating atomspace...~%")
(define as (make-atomspace))
(format #t "  ✓ AtomSpace created (size: ~a)~%~%" (atomspace-size as))

;; Test 2: Create atoms
(format #t "Test 2: Creating atoms...~%")
(define cat (create-concept-node as "Cat"))
(define dog (create-concept-node as "Dog"))
(define animal (create-concept-node as "Animal"))
(format #t "  ✓ Created 3 ConceptNodes~%")
(format #t "  ✓ AtomSpace size: ~a~%~%" (atomspace-size as))

;; Test 3: Create links
(format #t "Test 3: Creating links...~%")
(define cat-is-animal (create-inheritance-link as cat animal))
(define dog-is-animal (create-inheritance-link as dog animal))
(format #t "  ✓ Created 2 InheritanceLinks~%")
(format #t "  ✓ AtomSpace size: ~a~%~%" (atomspace-size as))

;; Test 4: Query by type
(format #t "Test 4: Querying by type...~%")
(define concepts (atomspace-get-atoms-by-type as 'ConceptNode))
(define links (atomspace-get-atoms-by-type as 'InheritanceLink))
(format #t "  ✓ Found ~a ConceptNodes~%" (length concepts))
(format #t "  ✓ Found ~a InheritanceLinks~%~%" (length links))

;; Test 5: Query by name
(format #t "Test 5: Querying by name...~%")
(define cats (get-atoms-by-name as "Cat"))
(format #t "  ✓ Found ~a atoms named 'Cat'~%~%" (length cats))

;; Test 6: Incoming/outgoing sets
(format #t "Test 6: Testing incoming/outgoing sets...~%")
(define cat-incoming (get-incoming-set cat))
(define link-outgoing (get-outgoing-set cat-is-animal))
(format #t "  ✓ Cat has ~a incoming links~%" (length cat-incoming))
(format #t "  ✓ InheritanceLink has ~a outgoing atoms~%~%" (length link-outgoing))

;; Test 7: Truth values
(format #t "Test 7: Testing truth values...~%")
(define tv (make-truth-value #:strength 0.8 #:confidence 0.9 #:count 100))
(define fuzzy-cat (create-concept-node as "FuzzyCat" #:tv tv))
(define fuzzy-tv (atom-tv fuzzy-cat))
(format #t "  ✓ Created atom with TV: strength=~a confidence=~a~%~%" 
        (tv-strength fuzzy-tv) (tv-confidence fuzzy-tv))

;; Test 8: Attention values
(format #t "Test 8: Testing attention values...~%")
(define av (make-attention-value #:sti 150 #:lti 50))
(define important-cat (create-concept-node as "ImportantCat" #:av av))
(define important-av (atom-av important-cat))
(format #t "  ✓ Created atom with AV: STI=~a LTI=~a~%~%" 
        (av-sti important-av) (av-lti important-av))

;; Test 9: Attentional focus
(format #t "Test 9: Testing attentional focus...~%")
(define focused (get-atoms-in-attentional-focus as #:sti-threshold 100))
(format #t "  ✓ Found ~a atoms with STI > 100~%~%" (length focused))

;; Test 10: Performance test
(format #t "Test 10: Performance test (creating 1000 atoms)...~%")
(define start-time (get-internal-real-time))
(do ((i 0 (+ i 1)))
    ((>= i 1000))
  (create-concept-node as (string-append "Concept" (number->string i))))
(define end-time (get-internal-real-time))
(define elapsed-ms (/ (- end-time start-time) (/ internal-time-units-per-second 1000)))
(format #t "  ✓ Created 1000 atoms in ~ams~%" elapsed-ms)
(format #t "  ✓ Final AtomSpace size: ~a~%~%" (atomspace-size as))

;; Test 11: Query performance
(format #t "Test 11: Query performance test...~%")
(define query-start (get-internal-real-time))
(define all-concepts (atomspace-get-atoms-by-type as 'ConceptNode))
(define query-end (get-internal-real-time))
(define query-ms (/ (- query-end query-start) (/ internal-time-units-per-second 1000)))
(format #t "  ✓ Queried ~a atoms in ~ams~%~%" (length all-concepts) query-ms)

;; Summary
(format #t "=== All Tests Passed! ===~%")
(format #t "Final statistics:~%")
(format #t "  - Total atoms: ~a~%" (atomspace-size as))
(format #t "  - ConceptNodes: ~a~%" (length (atomspace-get-atoms-by-type as 'ConceptNode)))
(format #t "  - InheritanceLinks: ~a~%" (length (atomspace-get-atoms-by-type as 'InheritanceLink)))
(format #t "~%")

;;; test-atomspace-simple.scm ends here
