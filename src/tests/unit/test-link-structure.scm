;;; test-link-structure.scm --- Understand link structure
;;;
;;; Copyright (C) 2025 Deep Tree Echo Project

(add-to-load-path "/home/ubuntu/occ/src")
(use-modules (core memory atomspace))

(format #t "~%=== Understanding Link Structure ===~%~%")

;; Create A -> B
(define as (make-atomspace))
(define a (create-concept-node as "A"))
(define b (create-concept-node as "B"))
(define a-b (create-inheritance-link as a b))

(format #t "Created: A -> B (InheritanceLink)~%~%")

(format #t "A (source):~%")
(format #t "  UUID: ~a~%" (atom-uuid a))
(format #t "  Incoming set: ~a~%" (map (lambda (x) (atom-type x)) (get-incoming-set a)))
(format #t "  Incoming set UUIDs: ~a~%~%" (map atom-uuid (get-incoming-set a)))

(format #t "B (target):~%")
(format #t "  UUID: ~a~%" (atom-uuid b))
(format #t "  Incoming set: ~a~%" (map (lambda (x) (atom-type x)) (get-incoming-set b)))
(format #t "  Incoming set UUIDs: ~a~%~%" (map atom-uuid (get-incoming-set b)))

(format #t "A-B link:~%")
(format #t "  UUID: ~a~%" (link-uuid a-b))
(format #t "  Type: ~a~%" (link-type a-b))
(format #t "  Outgoing set: ~a~%" (map atom-name (link-outgoing a-b)))
(format #t "  Outgoing set UUIDs: ~a~%~%" (map atom-uuid (link-outgoing a-b)))

(format #t "~%Interpretation:~%")
(format #t "  InheritanceLink(A, B) means 'A inherits from B' or 'A is-a B'~%")
(format #t "  The link's outgoing set is [A, B] (both source and target)~%")
(format #t "  A's incoming set contains the link (A is in the link's outgoing set)~%")
(format #t "  B's incoming set contains the link (B is in the link's outgoing set)~%")

(format #t "~%=== Test Complete ===~%")

;;; test-link-structure.scm ends here
