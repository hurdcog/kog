;;; Generate Hypergraph Flowchart Diagrams
;;; Creates Mermaid diagrams for all GNUMach primitives

(use-modules (ice-9 format))

(format #t "=== Generating Hypergraph Flowchart Diagrams ===~%~%")

;;; GNUMach primitives for diagram generation
(define gnumach-primitives
  '((VM_ALLOCATE (MEMORY 2 KERNEL 8 3))
    (PORT_ALLOCATE (IPC 1 SERVER 9 2))
    (THREAD_CREATE (SCHEDULER 3 SYSTEM 7 4))
    (FILE_OPEN (FILESYSTEM 1 TRANSLATOR 6 1))
    (NETWORK_SEND (NETWORK 2 USER 5 2))
    (SIGNAL_POST (SIGNAL 1 SYSTEM 8 3))))

;;; Generate Mermaid diagram for a primitive
(define (generate-primitive-diagram primitive-name properties)
  (let* ((modality (car properties))
         (depth (cadr properties))
         (context (caddr properties))
         (salience (cadddr properties))
         (autonomy (car (cddddr properties))))
    (string-append
      "flowchart TD\n"
      (format #f "    PRIM[\"~a Primitive\"]:::primitive\n" primitive-name)
      (format #f "    MOD[\"~a Modality\"]:::modality\n" modality)
      (format #f "    CTX[\"~a Context\"]:::context\n" context)
      "    ATOM1[\"CONCEPT: " (symbol->string primitive-name) "\"]:::atom\n"
      "    ATOM2[\"CONCEPT: " (symbol->string modality) "\"]:::atom\n"
      "    ATOM3[\"CONCEPT: " (symbol->string context) "\"]:::atom\n"
      "    LINK1[\"INHERITANCE Link\"]:::link\n"
      "    LINK2[\"EVALUATION Link\"]:::link\n"
      (format #f "    TENSOR[\"Tensor Shape: [8,4,8,10,5]<br/>Elements: 12,800\"]:::tensor\n")
      (format #f "    PATTERN[\"Pattern Signature<br/>Salience: ~a, Autonomy: ~a\"]:::pattern\n" salience autonomy)
      "\n"
      "    PRIM --> MOD\n"
      "    PRIM --> CTX\n"
      "    PRIM --> TENSOR\n"
      "    MOD --> ATOM2\n"
      "    CTX --> ATOM3\n"
      "    ATOM1 --> LINK1\n"
      "    ATOM2 --> LINK1\n"
      "    LINK1 --> PATTERN\n"
      "    LINK2 --> PATTERN\n"
      "    TENSOR --> PATTERN\n"
      "\n"
      "    classDef primitive fill:#ff9999,stroke:#333,stroke-width:2px,color:#000\n"
      "    classDef modality fill:#99ccff,stroke:#333,stroke-width:2px,color:#000\n"
      "    classDef context fill:#99ff99,stroke:#333,stroke-width:2px,color:#000\n"
      "    classDef atom fill:#ffcc99,stroke:#333,stroke-width:1px,color:#000\n"
      "    classDef link fill:#cc99ff,stroke:#333,stroke-width:1px,color:#000\n"
      "    classDef tensor fill:#ffff99,stroke:#333,stroke-width:2px,color:#000\n"
      "    classDef pattern fill:#ff99cc,stroke:#333,stroke-width:2px,color:#000\n")))

;;; Save diagram to file
(define (save-diagram primitive-name diagram)
  (let ((filename (format #f "docs/flowchart-~a.md" 
                          (string-downcase (symbol->string primitive-name)))))
    (call-with-output-file filename
      (lambda (port)
        (format port "# Hypergraph Flowchart: ~a~%~%" primitive-name)
        (format port "## Cognitive Fragment Visualization~%~%")
        (format port "This diagram shows the hypergraph representation of the ~a primitive " primitive-name)
        (format port "in the OpenCog-GNUHurd cognitive kernel integration.~%~%")
        (format port "```mermaid~%")
        (format port "~a" diagram)
        (format port "```~%~%")
        (format port "### Components:~%")
        (format port "- **Primitive Node**: The core GNUMach primitive~%")
        (format port "- **Modality Node**: Operational domain classification~%")
        (format port "- **Context Node**: Execution environment~%")
        (format port "- **Atom Nodes**: Individual AtomSpace concepts~%")
        (format port "- **Link Nodes**: Hypergraph relationships~%")
        (format port "- **Tensor Node**: Mathematical encoding information~%")
        (format port "- **Pattern Node**: Unique signature and statistics~%")
        (format port "~%Generated: ~a~%" (current-time))))
    (format #t "Generated: ~a~%" filename)
    filename))

;;; Generate all diagrams
(let ((generated-files '()))
  (for-each
    (lambda (primitive)
      (let* ((name (car primitive))
             (properties (cadr primitive))
             (diagram (generate-primitive-diagram name properties))
             (filename (save-diagram name diagram)))
        (set! generated-files (cons filename generated-files))))
    gnumach-primitives)
  
  ;; Create index file
  (call-with-output-file "docs/HYPERGRAPH_FLOWCHARTS.md"
    (lambda (port)
      (format port "# Hypergraph Flowchart Index~%")
      (format port "## OpenCog-GNUHurd Integration Phase 1~%~%")
      (format port "This document provides an index of all hypergraph flowchart ")
      (format port "visualizations for GNUMach primitive cognitive encodings.~%~%")
      (format port "## Flowchart Diagrams~%~%")
      (for-each
        (lambda (primitive)
          (let ((name (car primitive)))
            (format port "### [~a](flowchart-~a.md)~%" 
                    name (string-downcase (symbol->string name)))
            (format port "Hypergraph visualization for the ~a primitive.~%~%" name)))
        gnumach-primitives)
      (format port "## Diagram Features~%~%")
      (format port "Each flowchart includes:~%")
      (format port "- **Primitive Classification**: Modality and context encoding~%")
      (format port "- **AtomSpace Representation**: Nodes and hypergraph links~%")
      (format port "- **Tensor Encoding**: Mathematical shape and dimensions~%")
      (format port "- **Pattern Signature**: Unique identification hash~%")
      (format port "- **Visual Styling**: Color-coded component types~%~%")
      (format port "Generated: ~a~%" (current-time))))
  
  (format #t "~%Created index: docs/HYPERGRAPH_FLOWCHARTS.md~%")
  (format #t "Generated ~a flowchart diagrams total~%~%" (length generated-files)))

(format #t "✅ Hypergraph flowchart generation complete!~%")