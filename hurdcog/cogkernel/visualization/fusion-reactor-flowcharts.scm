;; Cognitive Fusion Reactor Flowchart Generator
;; Creates recursive visualization of the Distributed Agentic Cognitive Grammar Network

(define-module (cogkernel visualization fusion-reactor-flowcharts)
  #:use-module (opencog)
  #:use-module (srfi srfi-1)
  #:use-module (ice-9 format)
  #:use-module (ice-9 textual-ports)
  #:export (generate-fusion-reactor-flowchart
            generate-phase-flowcharts
            generate-recursive-architecture-diagram
            create-cognitive-tensor-visualization
            generate-attention-flow-diagram
            export-flowcharts-to-mermaid))

;; Main Fusion Reactor Flowchart
(define (generate-fusion-reactor-flowchart)
  "Generate the master flowchart for the cognitive fusion reactor"
  (let ((flowchart-data
         `((title . "🧬 Distributed Agentic Cognitive Grammar Network - Fusion Reactor")
           (type . "flowchart")
           (direction . "TD")
           (nodes . 
             ((start . ("st" "🧬 Agentic Grammar Input" "start"))
              (scheme-adapter . ("e1" "Scheme Adapter Translation" "operation"))
              (atomspace-encoding . ("e2" "AtomSpace Hypergraph Encoding" "operation"))
              (tensor-assignment . ("e3" "Tensor Shape Assignment [5D]" "operation"))
              (ecan-attention . ("e4" "ECAN Attention Kernel" "operation"))
              (ggml-symbolic . ("e5" "ggml Symbolic Kernel" "operation"))
              (distributed-api . ("e6" "Distributed API Propagation" "operation"))
              (embodiment-binding . ("e7" "Embodiment Interface Binding" "operation"))
              (meta-feedback . ("e8" "Meta-Cognitive Feedback" "operation"))
              (evolutionary-opt . ("e9" "Evolutionary Optimization" "operation"))
              (unified-field . ("e10" "Unified Cognitive Tensor Field" "end"))))
           (connections .
             (("st" . "e1")
              ("e1" . "e2")
              ("e2" . "e3")
              ("e3" . "e4")
              ("e4" . "e5")
              ("e5" . "e6")
              ("e6" . "e7")
              ("e7" . "e8")
              ("e8" . "e9")
              ("e9" . "e10")
              ;; Recursive feedback loops
              ("e8" . "e4") ;; Meta-cognitive feedback to attention
              ("e9" . "e2") ;; Evolutionary optimization to encoding
              ("e10" . "e1")))))) ;; Unified field back to input
    
    (format #t "🎨 Generating Fusion Reactor Master Flowchart...~%")
    (convert-to-mermaid flowchart-data)))

;; Phase-Specific Flowcharts
(define (generate-phase-flowcharts)
  "Generate detailed flowcharts for each phase"
  (let ((phase-charts '()))
    
    ;; Phase 1: Cognitive Primitives & Hypergraph Encoding
    (set! phase-charts
      (cons (generate-phase1-flowchart) phase-charts))
    
    ;; Phase 2: ECAN Attention Allocation
    (set! phase-charts
      (cons (generate-phase2-flowchart) phase-charts))
    
    ;; Phase 3: Neural-Symbolic ggml Kernels
    (set! phase-charts
      (cons (generate-phase3-flowchart) phase-charts))
    
    ;; Phase 4: Distributed Mesh API & Embodiment
    (set! phase-charts
      (cons (generate-phase4-flowchart) phase-charts))
    
    ;; Phase 5: Recursive Meta-Cognition
    (set! phase-charts
      (cons (generate-phase5-flowchart) phase-charts))
    
    ;; Phase 6: Testing & Unification
    (set! phase-charts
      (cons (generate-phase6-flowchart) phase-charts))
    
    (reverse phase-charts)))

;; Phase 1 Detailed Flowchart
(define (generate-phase1-flowchart)
  "Generate Phase 1: Cognitive Primitives & Hypergraph Encoding flowchart"
  `((title . "Phase 1: Cognitive Primitives & Foundational Hypergraph Encoding")
    (nodes .
      ((input . ("p1_input" "GNU Hurd Primitives" "input"))
       (scheme-microservices . ("p1_scheme" "Scheme Cognitive Grammar Microservices" "process"))
       (tensor-fragments . ("p1_tensor" "Tensor Fragment Architecture\\n[modality, depth, context, salience, autonomy]" "process"))
       (hypergraph-patterns . ("p1_hyper" "Hypergraph Pattern Encoding" "process"))
       (round-trip-test . ("p1_test" "Round-trip Translation Tests" "process"))
       (validation . ("p1_valid" "Real Data Validation" "decision"))
       (output . ("p1_output" "Cognitive Primitives Ready" "output"))))
    (connections .
      (("p1_input" . "p1_scheme")
       ("p1_scheme" . "p1_tensor")
       ("p1_tensor" . "p1_hyper")
       ("p1_hyper" . "p1_test")
       ("p1_test" . "p1_valid")
       ("p1_valid" . "p1_output")
       ;; Recursive validation loop
       ("p1_valid" . "p1_scheme")))))

;; Phase 2 Detailed Flowchart
(define (generate-phase2-flowchart)
  "Generate Phase 2: ECAN Attention Allocation flowchart"
  `((title . "Phase 2: ECAN Attention Allocation & Resource Kernel Construction")
    (nodes .
      ((cognitive-input . ("p2_input" "Cognitive Primitives" "input"))
       (ecan-kernel . ("p2_ecan" "ECAN Attention Economics\\nSTI/LTI Dynamics" "process"))
       (resource-allocation . ("p2_resource" "Resource Allocation Kernel" "process"))
       (mesh-topology . ("p2_mesh" "Dynamic Mesh Integration" "process"))
       (attention-spreading . ("p2_spread" "Activation Spreading" "process"))
       (competition . ("p2_compete" "Attention Competition" "process"))
       (optimization . ("p2_opt" "Resource Optimization" "decision"))
       (output . ("p2_output" "Attention-Optimized System" "output"))))
    (connections .
      (("p2_input" . "p2_ecan")
       ("p2_ecan" . "p2_resource")
       ("p2_resource" . "p2_mesh")
       ("p2_mesh" . "p2_spread")
       ("p2_spread" . "p2_compete")
       ("p2_compete" . "p2_opt")
       ("p2_opt" . "p2_output")
       ;; Economic feedback loops
       ("p2_compete" . "p2_ecan")
       ("p2_opt" . "p2_resource")))))

;; Phase 3 Detailed Flowchart
(define (generate-phase3-flowchart)
  "Generate Phase 3: Neural-Symbolic ggml Kernels flowchart"
  `((title . "Phase 3: Neural-Symbolic Synthesis via Custom ggml Kernels")
    (nodes .
      ((attention-input . ("p3_input" "Attention-Allocated Tensors" "input"))
       (ggml-kernels . ("p3_ggml" "Custom ggml Kernels\\nSymbolic Operations" "process"))
       (neural-symbolic . ("p3_neural" "Neural-Symbolic Synthesis" "process"))
       (tensor-ops . ("p3_ops" "Cognitive Tensor Operations" "process"))
       (inference-hooks . ("p3_hooks" "AtomSpace Inference Hooks" "process"))
       (optimization . ("p3_opt" "Performance Optimization" "process"))
       (validation . ("p3_valid" "Real Data Validation" "decision"))
       (output . ("p3_output" "Neural-Symbolic Engine" "output"))))
    (connections .
      (("p3_input" . "p3_ggml")
       ("p3_ggml" . "p3_neural")
       ("p3_neural" . "p3_ops")
       ("p3_ops" . "p3_hooks")
       ("p3_hooks" . "p3_opt")
       ("p3_opt" . "p3_valid")
       ("p3_valid" . "p3_output")
       ;; Optimization feedback
       ("p3_valid" . "p3_ggml")
       ("p3_opt" . "p3_neural")))))

;; Phase 4 Detailed Flowchart
(define (generate-phase4-flowchart)
  "Generate Phase 4: Distributed Cognitive Mesh API & Embodiment flowchart"
  `((title . "Phase 4: Distributed Cognitive Mesh API & Embodiment Layer")
    (nodes .
      ((neural-input . ("p4_input" "Neural-Symbolic Engine" "input"))
       (rest-api . ("p4_rest" "REST API Endpoints" "process"))
       (websocket . ("p4_ws" "WebSocket Real-time" "process"))
       (unity3d . ("p4_unity" "Unity3D Integration" "process"))
       (ros-bindings . ("p4_ros" "ROS Robotic Bindings" "process"))
       (web-agents . ("p4_web" "Web Agent Interfaces" "process"))
       (embodiment . ("p4_embody" "Embodiment Coordination" "process"))
       (validation . ("p4_valid" "Live Testing" "decision"))
       (output . ("p4_output" "Embodied Cognitive Network" "output"))))
    (connections .
      (("p4_input" . "p4_rest")
       ("p4_input" . "p4_ws")
       ("p4_rest" . "p4_unity")
       ("p4_rest" . "p4_ros")
       ("p4_ws" . "p4_web")
       ("p4_unity" . "p4_embody")
       ("p4_ros" . "p4_embody")
       ("p4_web" . "p4_embody")
       ("p4_embody" . "p4_valid")
       ("p4_valid" . "p4_output")
       ;; Real-time feedback
       ("p4_embody" . "p4_ws")))))

;; Phase 5 Detailed Flowchart
(define (generate-phase5-flowchart)
  "Generate Phase 5: Recursive Meta-Cognition flowchart"
  `((title . "Phase 5: Recursive Meta-Cognition & Evolutionary Optimization")
    (nodes .
      ((embodied-input . ("p5_input" "Embodied Cognitive Network" "input"))
       (self-analysis . ("p5_analyze" "Self-Analysis Modules" "process"))
       (introspection . ("p5_intro" "Cognitive Introspection" "process"))
       (moses-evolution . ("p5_moses" "MOSES Evolutionary Search" "process"))
       (fitness-eval . ("p5_fitness" "Fitness Evaluation" "process"))
       (meta-improvement . ("p5_meta" "Meta-Improvement" "process"))
       (recursive-loop . ("p5_recurse" "Recursive Optimization" "process"))
       (safety-check . ("p5_safety" "Safety Validation" "decision"))
       (output . ("p5_output" "Self-Improving System" "output"))))
    (connections .
      (("p5_input" . "p5_analyze")
       ("p5_analyze" . "p5_intro")
       ("p5_intro" . "p5_moses")
       ("p5_moses" . "p5_fitness")
       ("p5_fitness" . "p5_meta")
       ("p5_meta" . "p5_recurse")
       ("p5_recurse" . "p5_safety")
       ("p5_safety" . "p5_output")
       ;; Recursive meta-loops
       ("p5_meta" . "p5_analyze")
       ("p5_recurse" . "p5_intro")
       ("p5_safety" . "p5_moses")))))

;; Phase 6 Detailed Flowchart
(define (generate-phase6-flowchart)
  "Generate Phase 6: Testing, Documentation & Unification flowchart"
  `((title . "Phase 6: Rigorous Testing, Documentation, and Cognitive Unification")
    (nodes .
      ((improving-input . ("p6_input" "Self-Improving System" "input"))
       (comprehensive-tests . ("p6_test" "Comprehensive Testing\\nReal Data Only" "process"))
       (recursive-docs . ("p6_docs" "Recursive Documentation" "process"))
       (flowchart-gen . ("p6_flow" "Auto-Generated Flowcharts" "process"))
       (unification . ("p6_unify" "Cognitive Unification" "process"))
       (tensor-field . ("p6_tensor" "Unified Tensor Field" "process"))
       (emergent-analysis . ("p6_emerge" "Emergent Property Analysis" "process"))
       (validation . ("p6_valid" "100% Coverage Validation" "decision"))
       (output . ("p6_output" "Unified Cognitive System" "output"))))
    (connections .
      (("p6_input" . "p6_test")
       ("p6_test" . "p6_docs")
       ("p6_docs" . "p6_flow")
       ("p6_flow" . "p6_unify")
       ("p6_unify" . "p6_tensor")
       ("p6_tensor" . "p6_emerge")
       ("p6_emerge" . "p6_valid")
       ("p6_valid" . "p6_output")
       ;; Documentation recursion
       ("p6_flow" . "p6_docs")
       ("p6_emerge" . "p6_unify")))))

;; Recursive Architecture Diagram Generator
(define (generate-recursive-architecture-diagram)
  "Generate recursive architecture visualization"
  `((title . "🧬 Recursive Cognitive Architecture - Fractal Self-Similar Patterns")
    (type . "graph")
    (nodes .
      ;; Core cognitive layers (self-similar at each scale)
      ((unified-field . ("UF" "🌟 Unified Tensor Field\\n(Meta-Meta-Cognitive)" "core"))
       (meta-layer . ("ML" "🔄 Meta-Cognitive Layer\\n(Self-Improvement)" "meta"))
       (embodiment-layer . ("EL" "🌐 Embodiment Layer\\n(Physical Interface)" "interface"))
       (neural-symbolic . ("NS" "🔗 Neural-Symbolic Layer\\n(ggml Kernels)" "processing"))
       (attention-layer . ("AL" "⚡ Attention Layer\\n(ECAN Economics)" "attention"))
       (cognitive-layer . ("CL" "🧠 Cognitive Layer\\n(AtomSpace Memory)" "memory"))
       (hurd-kernel . ("HK" "🏗️ GNU Hurd Microkernel\\n(Foundation)" "foundation"))
       
       ;; Recursive pattern nodes (fractal)
       (fractal-1 . ("F1" "🔄 Fractal Pattern L1" "fractal"))
       (fractal-2 . ("F2" "🔄 Fractal Pattern L2" "fractal"))
       (fractal-3 . ("F3" "🔄 Fractal Pattern L3" "fractal"))))
    
    (connections .
      ;; Hierarchical connections
      (("UF" . "ML") ("ML" . "EL") ("EL" . "NS") ("NS" . "AL") ("AL" . "CL") ("CL" . "HK"))
      
      ;; Recursive feedback loops (self-similar patterns)
      (("ML" . "AL") ("ML" . "CL") ("EL" . "AL") ("NS" . "CL"))
      
      ;; Meta-cognitive recursion
      (("UF" . "F1") ("F1" . "F2") ("F2" . "F3") ("F3" . "UF"))
      
      ;; Cross-layer recursive patterns
      (("F1" . "ML") ("F2" . "EL") ("F3" . "NS"))))
    
    (format #t "🎨 Generated recursive architecture diagram~%")))

;; Attention Flow Visualization
(define (generate-attention-flow-diagram)
  "Generate ECAN attention flow visualization"
  `((title . "⚡ ECAN Attention Flow - Economic Resource Allocation")
    (type . "flowchart")
    (nodes .
      ((attention-bank . ("AB" "💰 Attention Bank\\n(Central Resource Pool)" "bank"))
       (sti-allocation . ("STI" "📈 STI Allocation\\n(Short-term Importance)" "allocation"))
       (lti-allocation . ("LTI" "📊 LTI Allocation\\n(Long-term Importance)" "allocation"))
       (cognitive-agents . ("CA" "🤖 Cognitive Agents\\n(Resource Consumers)" "agents"))
       (attention-rent . ("AR" "💸 Attention Rent\\n(Usage Cost)" "cost"))
       (cognitive-wages . ("CW" "💰 Cognitive Wages\\n(Activity Reward)" "reward"))
       (spreading-activation . ("SA" "🌊 Spreading Activation\\n(Network Propagation)" "propagation"))
       (competition . ("COMP" "⚔️ Resource Competition\\n(Economic Pressure)" "competition"))
       (optimization . ("OPT" "🎯 Attention Optimization\\n(Efficiency Maximization)" "optimization"))))
    
    (connections .
      (("AB" . "STI") ("AB" . "LTI")
       ("STI" . "CA") ("LTI" . "CA")
       ("CA" . "AR") ("CA" . "CW")
       ("CW" . "AB") ("AR" . "AB")
       ("CA" . "SA") ("SA" . "COMP")
       ("COMP" . "OPT") ("OPT" . "AB")))))

;; Cognitive Tensor Visualization
(define (create-cognitive-tensor-visualization)
  "Create 5D cognitive tensor visualization"
  `((title . "🧮 5D Cognitive Tensor Architecture")
    (type . "tensor-diagram")
    (dimensions .
      ((modality . ("Modality Axis" "Visual, Auditory, Conceptual, Linguistic, Motor, Emotional, Spatial, Temporal, Meta"))
       (depth . ("Depth Axis" "Primitive, Pattern, Abstraction, Meta, Recursive, Transcendent"))
       (context . ("Context Axis" "Contextual Embedding Dimension (0-∞)"))
       (salience . ("Salience Axis" "Attention Importance (0-100)"))
       (autonomy . ("Autonomy Axis" "Agent Independence (0-100)"))))
    
    (tensor-operations .
      ((cognitive-conv . "Cognitive Convolution across all dimensions")
       (attention-pool . "Attention pooling for salience concentration")
       (symbolic-activation . "Logic-preserving neural activations")
       (recursive-transform . "Self-similar pattern operations")
       (meta-reflection . "Tensor introspection operations")))
    
    (prime-factorization .
      ((modality-prime . 2)
       (depth-prime . 3)
       (context-prime . 5)
       (salience-prime . 7)
       (autonomy-prime . 11)))))

;; Mermaid Conversion Function
(define (convert-to-mermaid flowchart-data)
  "Convert flowchart data to Mermaid diagram format"
  (let ((title (assoc-ref flowchart-data 'title))
        (nodes (assoc-ref flowchart-data 'nodes))
        (connections (assoc-ref flowchart-data 'connections)))
    
    (string-append
      "```mermaid\n"
      "flowchart TD\n"
      "    %% " title "\n\n"
      
      ;; Generate node definitions
      (string-join
        (map (lambda (node)
               (let ((id (cadr node))
                     (label (caddr node))
                     (type (cadddr node)))
                 (format #f "    ~a[\"~a\"]" id label)))
             nodes)
        "\n")
      "\n\n"
      
      ;; Generate connections
      (string-join
        (map (lambda (conn)
               (format #f "    ~a --> ~a" (car conn) (cdr conn)))
             connections)
        "\n")
      "\n\n"
      
      ;; Add styling
      "    %% Styling\n"
      "    classDef startClass fill:#e1f5fe;\n"
      "    classDef processClass fill:#f3e5f5;\n"
      "    classDef endClass fill:#e8f5e8;\n"
      "```\n")))

;; Export All Flowcharts
(define (export-flowcharts-to-mermaid output-dir)
  "Export all flowcharts to Mermaid files"
  (format #t "📁 Exporting flowcharts to ~a~%" output-dir)
  
  ;; Create output directory
  (system (format #f "mkdir -p ~a" output-dir))
  
  ;; Export master fusion reactor flowchart
  (let ((master-chart (generate-fusion-reactor-flowchart)))
    (call-with-output-file (string-append output-dir "/fusion-reactor-master.md")
      (lambda (port)
        (put-string port master-chart))))
  
  ;; Export phase flowcharts
  (let ((phase-charts (generate-phase-flowcharts)))
    (for-each
      (lambda (chart index)
        (call-with-output-file
          (format #f "~a/phase-~a-flowchart.md" output-dir (+ index 1))
          (lambda (port)
            (put-string port (convert-to-mermaid chart)))))
      phase-charts
      (iota (length phase-charts))))
  
  ;; Export recursive architecture diagram
  (let ((arch-diagram (generate-recursive-architecture-diagram)))
    (call-with-output-file (string-append output-dir "/recursive-architecture.md")
      (lambda (port)
        (put-string port (convert-to-mermaid arch-diagram)))))
  
  ;; Export attention flow diagram
  (let ((attention-diagram (generate-attention-flow-diagram)))
    (call-with-output-file (string-append output-dir "/attention-flow.md")
      (lambda (port)
        (put-string port (convert-to-mermaid attention-diagram)))))
  
  (format #t "✅ All flowcharts exported successfully~%")
  (format #t "🎨 Recursive visualization architecture complete~%"))

;; Initialize visualization system
(format #t "📊 Cognitive Fusion Reactor Flowchart Generator loaded~%")
(format #t "🎨 Ready to visualize recursive cognitive architectures~%")