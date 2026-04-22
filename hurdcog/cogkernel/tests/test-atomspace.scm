;;; Test suite for AtomSpace module
;;; Validates hypergraph memory functionality

(define-module (test atomspace)
  #:use-module (cogkernel atomspace)
  #:use-module (ice-9 format))

;;; Test atom creation
(define (test-atom-creation)
  "Test basic atom creation functionality"
  (format #t "Testing atom creation...~%")
  (let ((concept-atom (make-atom 'CONCEPT "test-concept"))
        (number-atom (make-atom 'NUMBER "test-number" 42)))
    
    (assert (atom? concept-atom))
    (assert (eq? (atom-type concept-atom) 'CONCEPT))
    (assert (string=? (atom-name concept-atom) "test-concept"))
    
    (assert (atom? number-atom))
    (assert (eq? (atom-type number-atom) 'NUMBER))
    (assert (= (atom-value number-atom) 42))
    
    (format #t "✓ Atom creation tests passed~%")))

;;; Test link creation
(define (test-link-creation)
  "Test link creation between atoms"
  (format #t "Testing link creation...~%")
  (let* ((atom1 (make-atom 'CONCEPT "parent"))
         (atom2 (make-atom 'CONCEPT "child"))
         (inheritance-link (make-link 'INHERITANCE (list atom1 atom2))))
    
    (assert (link? inheritance-link))
    (assert (eq? (link-type inheritance-link) 'INHERITANCE))
    (assert (= (length (link-outgoing inheritance-link)) 2))
    
    (format #t "✓ Link creation tests passed~%")))

;;; Test atomspace operations
(define (test-atomspace-operations)
  "Test atomspace add/get operations"
  (format #t "Testing atomspace operations...~%")
  (let ((test-space (make-atomspace))
        (test-atom (make-atom 'CONCEPT "test-atomspace-atom")))
    
    (atomspace-add! test-space test-atom)
    (let ((retrieved (atomspace-get test-space "test-atomspace-atom")))
      (assert (atom? retrieved))
      (assert (string=? (atom-name retrieved) "test-atomspace-atom")))
    
    (format #t "✓ AtomSpace operations tests passed~%")))

;;; Test tensor shape tracking
(define (test-tensor-shapes)
  "Test tensor shape functionality"
  (format #t "Testing tensor shapes...~%")
  (let ((test-space (make-atomspace '(50 25 10 5))))
    (assert (equal? (atomspace-tensor-shape test-space) '(0 0 10 5)))
    
    ;; Add some atoms and check shape updates
    (atomspace-add! test-space (make-atom 'CONCEPT "shape-test-1"))
    (atomspace-add! test-space (make-atom 'CONCEPT "shape-test-2"))
    (assert (equal? (car (atomspace-tensor-shape test-space)) 2))
    
    (format #t "✓ Tensor shape tests passed~%")))

;;; Run all tests
(define (run-atomspace-tests)
  "Run all atomspace tests"
  (format #t "=== AtomSpace Test Suite ===~%")
  (test-atom-creation)
  (test-link-creation)
  (test-atomspace-operations)
  (test-tensor-shapes)
  (format #t "=== All AtomSpace tests passed! ===~%"))

;; Run tests when module is loaded
(run-atomspace-tests)