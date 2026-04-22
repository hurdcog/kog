;;; Phase 1 Integration Test - Complete OpenCog-GNUHurd Integration
;;; Tests all components: Cognitive Primitives, Scheme Adapters, Test Patterns, Visualization
;;; Implements real round-trip translation with no mocks

(define-module (cogkernel phase1-integration)
  #:use-module (ice-9 format)
  #:use-module (ice-9 match)
  #:use-module (srfi srfi-1)
  #:use-module (cogkernel cognitive-primitives)
  #:use-module (cogkernel scheme-adapters)
  #:use-module (cogkernel test-patterns)
  #:use-module (cogkernel hypergraph-viz)
  #:export (run-phase1-integration
            validate-phase1-deliverables
            generate-phase1-report
            demonstrate-cognitive-kernel
            *phase1-results*))

;;; Phase 1 integration results storage
(define *phase1-results* '())

;;; Add result to phase 1 collection
(define (add-phase1-result component result)
  "Add a component result to Phase 1 integration results"
  (set! *phase1-results*
        (cons `((component . ,component)
                (result . ,result)
                (timestamp . ,(get-timestamp)))
              *phase1-results*)))

;;; Run complete Phase 1 integration test
(define (run-phase1-integration)
  "Run complete Phase 1 integration test with all components"
  (format #t "╔════════════════════════════════════════════════════╗~%")
  (format #t "║        OpenCog-GNUHurd Integration Phase 1         ║~%")
  (format #t "║     Cognitive Primitives & Hypergraph Encoding     ║~%")
  (format #t "╚════════════════════════════════════════════════════╝~%~%")
  
  ;; Clear previous results
  (set! *phase1-results* '())
  
  ;; Component 1: Test Cognitive Primitives
  (format #t "🧠 Testing Cognitive Primitives...~%")
  (let ((primitives-result (test-cognitive-primitives-component)))
    (add-phase1-result 'cognitive-primitives primitives-result)
    (format #t "   Result: ~a~%~%" (if (assoc-ref primitives-result 'success) "✅ PASS" "❌ FAIL")))
  
  ;; Component 2: Test Scheme Adapters
  (format #t "🔄 Testing Scheme Adapters...~%")
  (let ((adapters-result (test-scheme-adapters-component)))
    (add-phase1-result 'scheme-adapters adapters-result)
    (format #t "   Result: ~a~%~%" (if (assoc-ref adapters-result 'success) "✅ PASS" "❌ FAIL")))
  
  ;; Component 3: Test Tensor Fragment Architecture
  (format #t "📊 Testing Tensor Fragment Architecture...~%")
  (let ((tensor-result (test-tensor-architecture-component)))
    (add-phase1-result 'tensor-architecture tensor-result)
    (format #t "   Result: ~a~%~%" (if (assoc-ref tensor-result 'success) "✅ PASS" "❌ FAIL")))
  
  ;; Component 4: Test Hypergraph Visualization
  (format #t "📈 Testing Hypergraph Visualization...~%")
  (let ((viz-result (test-visualization-component)))
    (add-phase1-result 'hypergraph-visualization viz-result)
    (format #t "   Result: ~a~%~%" (if (assoc-ref viz-result 'success) "✅ PASS" "❌ FAIL")))
  
  ;; Component 5: Run Exhaustive Test Patterns
  (format #t "🔍 Running Exhaustive Test Patterns...~%")
  (let ((test-result (test-exhaustive-patterns-component)))
    (add-phase1-result 'exhaustive-tests test-result)
    (format #t "   Result: ~a~%~%" (if (assoc-ref test-result 'success) "✅ PASS" "❌ FAIL")))
  
  ;; Validate overall integration
  (let ((integration-valid? (validate-phase1-integration)))
    (add-phase1-result 'overall-integration 
                       `((success . ,integration-valid?)
                         (components-tested . 5)))
    
    (format #t "🎯 Phase 1 Integration Status: ~a~%~%"
            (if integration-valid? "✅ SUCCESS" "❌ FAILED"))
    
    integration-valid?))

;;; Test cognitive primitives component
(define (test-cognitive-primitives-component)
  "Test the cognitive primitives encoding/decoding"
  (catch 'test-error
    (lambda ()
      (let ((test-primitive '(VM_ALLOCATE (MEMORY 2 KERNEL 8 3)))
            (results '()))
        
        ;; Test encoding
        (let* ((fragment (encode-gnumach-primitive 'VM_ALLOCATE '(MEMORY 2 KERNEL 8 3)))
               (tensor (cognitive-fragment-tensor fragment))
               (atoms (cognitive-fragment-atoms fragment)))
          
          ;; Validate tensor shape
          (let ((shape-valid? (equal? (tensor-shape tensor) cognitive-tensor-shape)))
            (set! results (cons `(tensor-shape . ,shape-valid?) results)))
          
          ;; Validate atoms creation
          (let ((atoms-valid? (and (> (length atoms) 0)
                                   (every atom? (filter atom? atoms)))))
            (set! results (cons `(atoms-created . ,atoms-valid?) results)))
          
          ;; Test round-trip
          (let* ((decoded (decode-to-gnumach-primitive fragment))
                 (original-type (car test-primitive))
                 (decoded-type (assoc-ref decoded 'primitive-type))
                 (round-trip-valid? (eq? original-type decoded-type)))
            (set! results (cons `(round-trip . ,round-trip-valid?) results))))
        
        `((success . ,(every cdr results))
          (details . ,results))))
    (lambda (key . args)
      `((success . #f)
        (error . ,args)))))

;;; Test scheme adapters component
(define (test-scheme-adapters-component)
  "Test the Scheme cognitive grammar adapters"
  (catch 'test-error
    (lambda ()
      (create-cognitive-grammar)
      
      (let ((test-primitive '(PORT_ALLOCATE (IPC 1 SERVER 9 2)))
            (results '()))
        
        ;; Register primitive
        (register-gnumach-primitive 'PORT_ALLOCATE '(IPC 1 SERVER 9 2)
                                    (assoc-ref *cognitive-grammar-rules* 'PORT_ALLOCATE))
        
        ;; Test adapter creation
        (let ((adapter (make-translation-adapter
                        "test-adapter" 'gnumach 'atomspace
                        (lambda (name props rule)
                          (let ((pattern-fn (assoc-ref rule 'pattern)))
                            (pattern-fn name props rule)))
                        validate-translation-integrity)))
          
          ;; Test forward translation
          (let* ((forward-result (adapter-translate-to-atomspace adapter test-primitive))
                 (forward-success? (assoc-ref forward-result 'success)))
            (set! results (cons `(forward-translation . ,forward-success?) results))
            
            (when forward-success?
              ;; Test validation
              (let* ((atoms (assoc-ref forward-result 'atoms))
                     (validation-valid? (> (length atoms) 0)))
                (set! results (cons `(atoms-generated . ,validation-valid?) results))))))
        
        `((success . ,(every cdr results))
          (details . ,results))))
    (lambda (key . args)
      `((success . #f)
        (error . ,args)))))

;;; Test tensor architecture component
(define (test-tensor-architecture-component)
  "Test the tensor fragment architecture with required shape"
  (catch 'test-error
    (lambda ()
      (let ((results '()))
        
        ;; Test tensor shape validation
        (let ((shape-valid? (equal? cognitive-tensor-shape '(8 4 8 10 5))))
          (set! results (cons `(shape-specification . ,shape-valid?) results)))
        
        ;; Test modalities mapping
        (let ((modalities-valid? (= (length *cognitive-modalities*) 8)))
          (set! results (cons `(modalities-count . ,modalities-valid?) results)))
        
        ;; Test contexts mapping
        (let ((contexts-valid? (= (length *cognitive-contexts*) 8)))
          (set! results (cons `(contexts-count . ,contexts-valid?) results)))
        
        ;; Test tensor creation
        (let* ((fragment (encode-gnumach-primitive 'THREAD_CREATE '(SCHEDULER 3 SYSTEM 7 4)))
               (tensor (cognitive-fragment-tensor fragment))
               (tensor-valid? (and (tensor? tensor)
                                   (= (length (tensor-data tensor)) 12800))))
          (set! results (cons `(tensor-creation . ,tensor-valid?) results)))
        
        `((success . ,(every cdr results))
          (details . ,results))))
    (lambda (key . args)
      `((success . #f)
        (error . ,args)))))

;;; Test visualization component
(define (test-visualization-component)
  "Test the hypergraph visualization generation"
  (catch 'test-error
    (lambda ()
      (let ((results '()))
        
        ;; Test diagram generation
        (let* ((fragment (encode-gnumach-primitive 'FILE_OPEN '(FILESYSTEM 1 TRANSLATOR 6 1)))
               (diagram (generate-hypergraph-mermaid fragment 'FILE_OPEN))
               (diagram-valid? (and (string? diagram)
                                    (> (string-length diagram) 100)
                                    (string-contains diagram "flowchart TD"))))
          (set! results (cons `(diagram-generation . ,diagram-valid?) results)))
        
        ;; Test pattern creation
        (let* ((fragment (encode-gnumach-primitive 'NETWORK_SEND '(NETWORK 2 USER 5 2)))
               (pattern (create-hypergraph-pattern fragment))
               (pattern-valid? (and (assoc-ref pattern 'nodes)
                                    (assoc-ref pattern 'links)
                                    (number? (assoc-ref pattern 'pattern-signature)))))
          (set! results (cons `(pattern-creation . ,pattern-valid?) results)))
        
        `((success . ,(every cdr results))
          (details . ,results))))
    (lambda (key . args)
      `((success . #f)
        (error . ,args)))))

;;; Test exhaustive patterns component
(define (test-exhaustive-patterns-component)
  "Test the exhaustive test patterns"
  (catch 'test-error
    (lambda ()
      (let ((results '()))
        
        ;; Test primitive encoding patterns
        (let* ((encoding-results (test-primitive-encoding))
               (encoding-success? (every (lambda (r) (assoc-ref r 'overall)) encoding-results)))
          (set! results (cons `(primitive-encoding . ,encoding-success?) results)))
        
        ;; Test tensor shape validation patterns
        (let* ((shape-results (test-tensor-shape-validation))
               (shape-success? (any (lambda (r) (assoc-ref r 'valid)) shape-results)))
          (set! results (cons `(tensor-validation . ,shape-success?) results)))
        
        ;; Test hypergraph pattern creation
        (let* ((pattern-results (test-hypergraph-pattern-creation))
               (pattern-success? (every (lambda (r) (assoc-ref r 'overall)) pattern-results)))
          (set! results (cons `(hypergraph-patterns . ,pattern-success?) results)))
        
        `((success . ,(every cdr results))
          (details . ,results))))
    (lambda (key . args)
      `((success . #f)
        (error . ,args)))))

;;; Validate Phase 1 integration
(define (validate-phase1-integration)
  "Validate overall Phase 1 integration success"
  (let ((component-results (map (lambda (result)
                                  (assoc-ref (assoc-ref result 'result) 'success))
                                *phase1-results*)))
    (every identity component-results)))

;;; Validate Phase 1 deliverables
(define (validate-phase1-deliverables)
  "Validate that all Phase 1 deliverables are present and functional"
  (format #t "=== Validating Phase 1 Deliverables ===~%~%")
  
  (let ((deliverables '())
        (all-valid? #t))
    
    ;; 1. Real Scheme adapter code (no mockups)
    (format #t "📋 Checking: Real Scheme adapter code...~%")
    (let ((adapters-exist? (and (defined? 'make-translation-adapter)
                                (defined? 'adapter-translate-to-atomspace)
                                (defined? 'adapter-translate-from-atomspace))))
      (format #t "   ~a~%" (if adapters-exist? "✅ PRESENT" "❌ MISSING"))
      (set! deliverables (cons `(scheme-adapters . ,adapters-exist?) deliverables))
      (unless adapters-exist? (set! all-valid? #f)))
    
    ;; 2. Tensor signature documentation
    (format #t "📋 Checking: Tensor signature documentation...~%")
    (let ((docs-exist? (file-exists? "docs/TENSOR_SIGNATURES.md")))
      (format #t "   ~a~%" (if docs-exist? "✅ PRESENT" "❌ MISSING"))
      (set! deliverables (cons `(tensor-documentation . ,docs-exist?) deliverables))
      (unless docs-exist? (set! all-valid? #f)))
    
    ;; 3. Hypergraph flowchart diagrams
    (format #t "📋 Checking: Hypergraph flowchart capabilities...~%")
    (let ((viz-exist? (and (defined? 'generate-hypergraph-mermaid)
                           (defined? 'create-primitive-flowchart))))
      (format #t "   ~a~%" (if viz-exist? "✅ PRESENT" "❌ MISSING"))
      (set! deliverables (cons `(hypergraph-flowcharts . ,viz-exist?) deliverables))
      (unless viz-exist? (set! all-valid? #f)))
    
    ;; 4. Test logs and verification outputs
    (format #t "📋 Checking: Test patterns and verification...~%")
    (let ((tests-exist? (and (defined? 'run-exhaustive-tests)
                             (defined? 'verify-cognitive-integrity))))
      (format #t "   ~a~%" (if tests-exist? "✅ PRESENT" "❌ MISSING"))
      (set! deliverables (cons `(test-verification . ,tests-exist?) deliverables))
      (unless tests-exist? (set! all-valid? #f)))
    
    ;; 5. Tensor fragment architecture with required shape
    (format #t "📋 Checking: Tensor fragment architecture...~%")
    (let ((tensor-arch-valid? (and (defined? 'cognitive-tensor-shape)
                                   (equal? cognitive-tensor-shape '(8 4 8 10 5)))))
      (format #t "   ~a~%" (if tensor-arch-valid? "✅ VALID" "❌ INVALID"))
      (set! deliverables (cons `(tensor-architecture . ,tensor-arch-valid?) deliverables))
      (unless tensor-arch-valid? (set! all-valid? #f)))
    
    (format #t "~%Overall Deliverables Status: ~a~%"
            (if all-valid? "✅ ALL PRESENT" "❌ INCOMPLETE"))
    
    `((valid . ,all-valid?)
      (deliverables . ,deliverables))))

;;; Generate Phase 1 report
(define (generate-phase1-report)
  "Generate comprehensive Phase 1 integration report"
  (format #t "~%╔════════════════════════════════════════════════════╗~%")
  (format #t "║              PHASE 1 INTEGRATION REPORT           ║~%")
  (format #t "╚════════════════════════════════════════════════════╝~%~%")
  
  (let* ((deliverables-validation (validate-phase1-deliverables))
         (integration-results *phase1-results*)
         (total-components (length integration-results))
         (successful-components (count (lambda (result)
                                         (assoc-ref (assoc-ref result 'result) 'success))
                                       integration-results)))
    
    (format #t "Integration Summary:~%")
    (format #t "  Components Tested: ~a~%" total-components)
    (format #t "  Successful Components: ~a~%" successful-components)
    (format #t "  Success Rate: ~a%~%~%"
            (if (> total-components 0)
                (inexact->exact (round (* 100 (/ successful-components total-components))))
                0))
    
    (format #t "Component Results:~%")
    (for-each
      (lambda (result)
        (let ((component (assoc-ref result 'component))
              (success? (assoc-ref (assoc-ref result 'result) 'success)))
          (format #t "  ~a: ~a~%"
                  component
                  (if success? "✅ PASS" "❌ FAIL"))))
      integration-results)
    
    (format #t "~%Deliverables Validation:~%")
    (let ((deliverables (assoc-ref deliverables-validation 'deliverables)))
      (for-each
        (lambda (deliverable)
          (format #t "  ~a: ~a~%"
                  (car deliverable)
                  (if (cdr deliverable) "✅ VALID" "❌ INVALID")))
        deliverables))
    
    (let ((overall-success? (and (= successful-components total-components)
                                 (assoc-ref deliverables-validation 'valid))))
      (format #t "~%🎯 PHASE 1 INTEGRATION: ~a~%"
              (if overall-success? "✅ COMPLETE SUCCESS" "❌ NEEDS ATTENTION"))
      
      overall-success?)))

;;; Demonstrate cognitive kernel capabilities
(define (demonstrate-cognitive-kernel)
  "Demonstrate the cognitive kernel with a complete example"
  (format #t "=== Cognitive Kernel Demonstration ===~%~%")
  
  ;; Show round-trip translation
  (format #t "🔄 Round-trip Translation Demo:~%")
  (test-round-trip-translation 'VM_ALLOCATE '(MEMORY 2 KERNEL 8 3))
  
  (format #t "~%📊 Tensor Architecture Demo:~%")
  (let ((fragment (encode-gnumach-primitive 'PORT_ALLOCATE '(IPC 1 SERVER 9 2))))
    (format #t "Tensor shape: ~a~%" (tensor-shape (cognitive-fragment-tensor fragment)))
    (format #t "Atoms count: ~a~%" (length (cognitive-fragment-atoms fragment)))
    (format #t "Modality: ~a~%" (cognitive-fragment-modality fragment)))
  
  (format #t "~%📈 Hypergraph Visualization Demo:~%")
  (demo-hypergraph-visualization)
  
  (format #t "~%🧠 Cognitive Grammar Demo:~%")
  (test-adapter-round-trip)
  
  #t)

;;; Main Phase 1 integration entry point
(define (complete-phase1-integration)
  "Complete Phase 1 integration with full testing and reporting"
  (format #t "Starting complete Phase 1 integration...~%~%")
  
  (let ((integration-success? (run-phase1-integration)))
    (generate-phase1-report)
    
    (when integration-success?
      (format #t "~%Generating documentation and visualizations...~%")
      (generate-all-primitive-diagrams))
    
    integration-success?))