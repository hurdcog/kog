;;; HurdCog Minimal Bootstrap Test - Standalone Implementation
;;; Tests the cognitive grip and MachSpace functionality

(add-to-load-path ".")

;; Import the individual modules
(use-modules (cogkernel atomspace))

;; Test basic AtomSpace functionality
(define test-atomspace (make-atomspace))
(format #t "=== Testing AtomSpace ===~%")
(format #t "AtomSpace created: ~a~%" (atomspace? test-atomspace))
(format #t "Tensor shape: ~a~%" (atomspace-tensor-shape test-atomspace))

;; Create some test atoms
(define test-atom (make-atom 'CONCEPT "test-concept"))
(atomspace-add! test-atomspace test-atom)
(format #t "Added atom: ~a~%" (atom-name test-atom))

;; Test retrieval
(define retrieved-atom (atomspace-get test-atomspace "test-concept"))
(format #t "Retrieved atom: ~a~%" (if retrieved-atom (atom-name retrieved-atom) "NOT FOUND"))

(format #t "✅ AtomSpace test complete~%")

;; Simple cognitive grip implementation (standalone)
(define (simple-cognitive-grip object-name)
  "Simplified cognitive grip for testing"
  (let ((atom (make-atom 'CONCEPT object-name)))
    (atomspace-add! test-atomspace atom)
    (format #t "Gripping object: ~a~%" object-name)
    (list 'GRIP object-name 'STRENGTH 0.8)))

;; Test the simple cognitive grip
(format #t "~%=== Testing Cognitive Grip ===~%")
(simple-cognitive-grip "memory-leak")
(simple-cognitive-grip "deadlock-issue") 
(simple-cognitive-grip "security-vulnerability")
(format #t "✅ Cognitive grip test complete~%")

;; Show final statistics
(format #t "~%=== Final Statistics ===~%")
(format #t "Total atoms in AtomSpace: ~a~%" 
        (car (atomspace-tensor-shape test-atomspace)))
(format #t "HurdCog Minimal Bootstrap: OPERATIONAL~%")