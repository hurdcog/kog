(add-to-load-path "/home/ubuntu/occ/src")
(use-modules (core memory atomspace))
(use-modules (core grammar inference))

(format #t "~%=== Debug Pattern Matching ===~%~%")

(let* ((as (make-atomspace))
       (cat (create-concept-node as "Cat"))
       (mammal (create-concept-node as "Mammal"))
       (cat-mammal (create-inheritance-link as cat mammal)))
  
  (format #t "Testing match-premises...~%")
  (let* ((premises '((InheritanceLink ?X ?Y) ?X))
         (matches (match-premises as premises)))
    (format #t "Premises: ~a~%" premises)
    (format #t "Number of matches: ~a~%~%" (length matches))
    (when (> (length matches) 0)
      (format #t "First match bindings:~%")
      (for-each
        (lambda (binding)
          (format #t "  ~a = ~a~%" (car binding) (cdr binding)))
        (car matches)))))

(format #t "~%=== Test Complete ===~%")
