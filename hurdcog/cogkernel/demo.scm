;;; Simple Cognitive Kernel Demo
;;; Shows the basic cognitive functionality working

(format #t "=== Cognitive Kernel Demo ===~%")

;; Load AtomSpace
(format #t "Loading AtomSpace...~%")
(primitive-load "atomspace/atomspace.scm")

;; Test AtomSpace
(format #t "Testing AtomSpace functionality...~%")
(let ((test-atom (make-atom 'CONCEPT "demo-concept"))
      (test-space (make-atomspace)))
  (atomspace-add! test-space test-atom)
  (format #t "✓ Created atom: ~a~%" (atom-name test-atom))
  (format #t "✓ AtomSpace tensor shape: ~a~%" (atomspace-tensor-shape test-space)))

;; Load Agents
(format #t "Loading Agents...~%")
(primitive-load "agents/agents.scm")

;; Test Agents
(format #t "Testing Agent functionality...~%")
(let ((demo-agent (make-agent "demo-agent" 'MONITOR))
      (agent-sys (make-agent-system)))
  (agent-register-action! demo-agent 'DETECT
    (lambda () "Issue detected!"))
  (agent-system-add! agent-sys demo-agent)
  (let ((result (agent-execute! demo-agent 'DETECT)))
    (format #t "✓ Agent ~a executed: ~a~%" (agent-id demo-agent) result))
  (format #t "✓ Agent system tensor shape: ~a~%" (agent-system-tensor-shape agent-sys)))

;; Load Attention
(format #t "Loading Attention mechanism...~%")
(primitive-load "attention/ecan.scm")

;; Test Attention
(format #t "Testing Attention allocation...~%")
(let ((attention-val (make-attention-value 100 50 25))
      (attention-sys (make-attention-bank)))
  (format #t "✓ Attention value STI: ~a~%" (attention-value-sti attention-val))
  (format #t "✓ Attention bank created with ~a funds~%" 
          (attention-bank-total-funds attention-sys)))

;; Load Tensors
(format #t "Loading Tensor operations...~%")
(primitive-load "tensors/tensors.scm")

;; Test Tensors
(format #t "Testing Tensor operations...~%")
(let ((tensor1 (tensor-ones '(2 3)))
      (tensor2 (tensor-zeros '(2 3))))
  (let ((result (tensor-add tensor1 tensor2)))
    (format #t "✓ Tensor addition result shape: ~a~%" (tensor-shape result))
    (format #t "✓ Tensor data sample: ~a~%" (take (tensor-data result) 3))))

;; Show P-System membrane computing
(format #t "Testing P-System membrane computing...~%")
(let ((membrane (p-system-membrane '() '(tensor1 tensor2) 
                                   (list (lambda (objs) 
                                           (map (lambda (o) 
                                                  (format #f "evolved-~a" o)) 
                                                objs))))))
  (membrane-evolve! membrane)
  (format #t "✓ P-System membrane evolved objects: ~a~%" (membrane-objects membrane)))

;; Cognitive integration test
(format #t "Testing cognitive integration...~%")
(let ((atom-space (make-atomspace))
      (agent-system (make-agent-system))
      (attention-bank (make-attention-bank)))
  
  ;; Create cognitive scenario
  (let ((hurd-concept (make-atom 'CONCEPT "GNU-Hurd-Demo"))
        (monitor-agent (make-agent "cognitive-monitor" 'MONITOR))
        (attention-val (make-attention-value 150 100 75)))
    
    ;; Add to systems
    (atomspace-add! atom-space hurd-concept)
    (agent-system-add! agent-system monitor-agent)
    (attention-bank-add! attention-bank hurd-concept attention-val)
    
    ;; Show integration
    (format #t "✓ Cognitive system integrated:~%")
    (format #t "  - AtomSpace: ~a atoms~%" (car (atomspace-tensor-shape atom-space)))
    (format #t "  - Agents: ~a agents~%" (car (agent-system-tensor-shape agent-system)))
    (format #t "  - Attention: ~a STI for ~a~%" 
            (attention-value-sti attention-val) (atom-name hurd-concept))))

(format #t "~%=== Cognitive Kernel Demo Completed Successfully! ===~%")
(format #t "The self-evolving scaffolding is operational and ready for GNU Hurd integration.~%")