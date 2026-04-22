(add-to-load-path "/home/ubuntu/occ/src")
(use-modules (core memory atomspace))
(use-modules (core grammar inference))

(format #t "~%=== Debug Forward Chaining ===~%~%")

(let* ((as (make-atomspace))
       (cat (create-concept-node as "Cat"))
       (mammal (create-concept-node as "Mammal"))
       (cat-mammal (create-inheritance-link as cat mammal)))
  
  (format #t "Created atoms:~%")
  (format #t "  Cat: ~a~%" (atom-name cat))
  (format #t "  Mammal: ~a~%" (atom-name mammal))
  (format #t "  Link: ~a~%~%" (link-type cat-mammal))
  
  (format #t "AtomSpace contents:~%")
  (format #t "  Total atoms: ~a~%~%" (atomspace-size as))
  
  (let ((rule (make-inference-rule
                'simple-inheritance
                (list (list 'InheritanceLink '?X '?Y) '?X)
                '?Y
                1.0)))
    
    (format #t "Rule:~%")
    (format #t "  Name: ~a~%" (rule-name rule))
    (format #t "  Premises: ~a~%" (rule-premises rule))
    (format #t "  Conclusion: ~a~%~%" (rule-conclusion rule))
    
    (format #t "Applying forward chaining...~%")
    (let ((new-atoms (forward-chain as (list rule) 1)))
      (format #t "Derived ~a new atoms~%" (length new-atoms))
      (when (> (length new-atoms) 0)
        (format #t "New atoms: ~a~%" new-atoms)))))

(format #t "~%=== Test Complete ===~%")
