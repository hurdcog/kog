;;; Hypergraph Flowchart Visualization
;;; Generates flowchart diagrams for cognitive fragment hypergraphs
;;; Mermaid diagram generation for documentation

(define-module (cogkernel hypergraph-viz)
  #:use-module (ice-9 format)
  #:use-module (ice-9 match)
  #:use-module (srfi srfi-1)
  #:use-module (cogkernel cognitive-primitives)
  #:use-module (cogkernel atomspace)
  #:export (generate-hypergraph-mermaid
            create-primitive-flowchart
            save-flowchart-file
            generate-all-primitive-diagrams
            *diagram-templates*))

;;; Mermaid diagram templates
(define *diagram-templates*
  '((flowchart . "flowchart TD")
    (graph . "graph TD") 
    (sequence . "sequenceDiagram")
    (class . "classDiagram")))

;;; Generate Mermaid flowchart for hypergraph
(define (generate-hypergraph-mermaid fragment primitive-name)
  "Generate Mermaid flowchart diagram for cognitive fragment hypergraph"
  (let* ((atoms (cognitive-fragment-atoms fragment))
         (tensor (cognitive-fragment-tensor fragment))
         (modality (cognitive-fragment-modality fragment))
         (metadata (cognitive-fragment-metadata fragment))
         (pattern (create-hypergraph-pattern fragment)))
    
    (string-append
      "flowchart TD\n"
      (format #f "    %PRIMITIVE%[\"~a Primitive\"]:::primitive\n" primitive-name)
      (generate-modality-node modality)
      (generate-context-node metadata)
      (generate-atom-nodes atoms)
      (generate-link-connections atoms)
      (generate-tensor-info-node tensor)
      (generate-pattern-signature-node pattern)
      (generate-connections primitive-name modality metadata)
      (generate-css-styles))))

;;; Generate modality node
(define (generate-modality-node modality)
  "Generate modality node for the diagram"
  (let ((modality-name (car (find (lambda (pair) (= (cdr pair) modality)) 
                                  *cognitive-modalities*))))
    (format #f "    %MODALITY%[\"~a Modality\"]:::modality\n" modality-name)))

;;; Generate context node
(define (generate-context-node metadata)
  "Generate context node for the diagram"
  (let* ((context (assoc-ref metadata 'context))
         (context-name (car (find (lambda (pair) (= (cdr pair) context)) 
                                  *cognitive-contexts*))))
    (format #f "    %CONTEXT%[\"~a Context\"]:::context\n" context-name)))

;;; Generate atom nodes
(define (generate-atom-nodes atoms)
  "Generate nodes for atoms in the hypergraph"
  (string-concatenate
    (map (lambda (atom index)
           (if (atom? atom)
               (format #f "    ATOM~a[\"~a: ~a\"]:::atom\n" 
                       index (atom-type atom) (atom-name atom))
               (format #f "    LINK~a[\"~a Link\"]:::link\n"
                       index (link-type atom))))
         atoms (iota (length atoms)))))

;;; Generate link connections
(define (generate-link-connections atoms)
  "Generate connections between atoms based on links"
  (let ((links (filter link? atoms))
        (atom-indices (make-atom-index-map atoms)))
    (string-concatenate
      (map (lambda (link index)
             (if (link? link)
                 (let ((outgoing (link-outgoing link)))
                   (if (and (list? outgoing) (>= (length outgoing) 2))
                       (format #f "    LINK~a --> ATOM~a\n    LINK~a --> ATOM~a\n"
                               index 
                               (find-atom-index (car outgoing) atom-indices)
                               index
                               (find-atom-index (cadr outgoing) atom-indices))
                       ""))
                 ""))
           links (iota (length links))))))

;;; Generate tensor info node
(define (generate-tensor-info-node tensor)
  "Generate tensor information node"
  (let ((shape (tensor-shape tensor))
        (data-size (length (tensor-data tensor))))
    (format #f "    %TENSOR%[\"Tensor Shape: ~a<br/>Elements: ~a\"]:::tensor\n"
            shape data-size)))

;;; Generate pattern signature node
(define (generate-pattern-signature-node pattern)
  "Generate pattern signature node"
  (let ((signature (assoc-ref pattern 'pattern-signature))
        (nodes-count (length (assoc-ref pattern 'nodes)))
        (links-count (length (assoc-ref pattern 'links))))
    (format #f "    %PATTERN%[\"Pattern Signature: ~a<br/>Nodes: ~a, Links: ~a\"]:::pattern\n"
            signature nodes-count links-count)))

;;; Generate main connections
(define (generate-connections primitive-name modality metadata)
  "Generate main connections between components"
  (string-append
    "    %PRIMITIVE% --> %MODALITY%\n"
    "    %PRIMITIVE% --> %CONTEXT%\n"
    "    %PRIMITIVE% --> %TENSOR%\n"
    "    %MODALITY% --> %PATTERN%\n"
    "    %CONTEXT% --> %PATTERN%\n"
    "    %TENSOR% --> %PATTERN%\n"))

;;; Generate CSS styles
(define (generate-css-styles)
  "Generate CSS styling for the diagram"
  (string-append
    "\n    classDef primitive fill:#ff9999,stroke:#333,stroke-width:2px,color:#000\n"
    "    classDef modality fill:#99ccff,stroke:#333,stroke-width:2px,color:#000\n"
    "    classDef context fill:#99ff99,stroke:#333,stroke-width:2px,color:#000\n"
    "    classDef atom fill:#ffcc99,stroke:#333,stroke-width:1px,color:#000\n"
    "    classDef link fill:#cc99ff,stroke:#333,stroke-width:1px,color:#000\n"
    "    classDef tensor fill:#ffff99,stroke:#333,stroke-width:2px,color:#000\n"
    "    classDef pattern fill:#ff99cc,stroke:#333,stroke-width:2px,color:#000\n"))

;;; Helper function to create atom index map
(define (make-atom-index-map atoms)
  "Create a mapping from atom names to indices"
  (let loop ((atoms atoms) (index 0) (map '()))
    (if (null? atoms)
        map
        (let ((atom (car atoms)))
          (loop (cdr atoms) (+ index 1)
                (cons (cons (if (atom? atom) (atom-name atom) 
                                (link-type atom)) index) map))))))

;;; Helper function to find atom index
(define (find-atom-index atom-name index-map)
  "Find the index of an atom by name"
  (let ((entry (assoc atom-name index-map)))
    (if entry (cdr entry) 0)))

;;; Create flowchart for a specific primitive
(define (create-primitive-flowchart primitive-name properties)
  "Create a flowchart diagram for a specific GNUMach primitive"
  (let* ((fragment (encode-gnumach-primitive primitive-name properties))
         (diagram (generate-hypergraph-mermaid fragment primitive-name)))
    (format #t "=== Hypergraph Flowchart for ~a ===~%~%" primitive-name)
    (format #t "~a~%" diagram)
    diagram))

;;; Save flowchart to file
(define (save-flowchart-file primitive-name diagram)
  "Save flowchart diagram to a markdown file"
  (let ((filename (format #f "docs/flowchart-~a.md" 
                          (string-downcase (symbol->string primitive-name)))))
    (call-with-output-file filename
      (lambda (port)
        (format port "# Hypergraph Flowchart: ~a~%~%" primitive-name)
        (format port "## Cognitive Fragment Visualization~%~%")
        (format port "```mermaid~%")
        (format port "~a" diagram)
        (format port "```~%~%")
        (format port "## Description~%~%")
        (format port "This diagram shows the hypergraph representation of the ~a primitive " primitive-name)
        (format port "in the OpenCog-GNUHurd cognitive kernel integration.~%~%")
        (format port "### Components:~%")
        (format port "- **Primitive Node**: The core GNUMach primitive~%")
        (format port "- **Modality Node**: Operational domain classification~%")
        (format port "- **Context Node**: Execution environment~%")
        (format port "- **Atom Nodes**: Individual AtomSpace concepts~%")
        (format port "- **Link Nodes**: Hypergraph relationships~%")
        (format port "- **Tensor Node**: Mathematical encoding information~%")
        (format port "- **Pattern Node**: Unique signature and statistics~%")
        (format port "~%Generated: ~a~%" (current-time))))
    (format #t "Saved flowchart to: ~a~%" filename)
    filename))

;;; Generate diagrams for all primitives
(define (generate-all-primitive-diagrams)
  "Generate flowchart diagrams for all example primitives"
  (format #t "=== Generating All Primitive Flowcharts ===~%~%")
  
  (let ((generated-files '()))
    (for-each
      (lambda (primitive-example)
        (match primitive-example
          ((name properties)
           (format #t "Generating diagram for: ~a~%" name)
           (let* ((diagram (create-primitive-flowchart name properties))
                  (filename (save-flowchart-file name diagram)))
             (set! generated-files (cons filename generated-files))))))
      gnumach-primitives-examples)
    
    (format #t "~%Generated ~a flowchart files:~%" (length generated-files))
    (for-each (lambda (file) (format #t "  - ~a~%" file)) 
              (reverse generated-files))
    
    ;; Create index file
    (create-flowchart-index generated-files)
    
    generated-files))

;;; Create index file for all flowcharts
(define (create-flowchart-index files)
  "Create an index file linking to all flowcharts"
  (let ((index-file "docs/HYPERGRAPH_FLOWCHARTS.md"))
    (call-with-output-file index-file
      (lambda (port)
        (format port "# Hypergraph Flowchart Index~%")
        (format port "## OpenCog-GNUHurd Integration Phase 1~%~%")
        (format port "This document provides an index of all hypergraph flowchart ")
        (format port "visualizations for GNUMach primitive cognitive encodings.~%~%")
        (format port "## Flowchart Diagrams~%~%")
        (for-each
          (lambda (file)
            (let* ((basename (basename file ".md"))
                   (primitive-name (string-upcase
                                     (string-drop basename 
                                                  (string-length "flowchart-")))))
              (format port "### [~a](~a)~%" primitive-name file)
              (format port "Hypergraph visualization for the ~a primitive.~%~%" primitive-name)))
          files)
        (format port "## Diagram Features~%~%")
        (format port "Each flowchart includes:~%")
        (format port "- **Primitive Classification**: Modality and context encoding~%")
        (format port "- **AtomSpace Representation**: Nodes and hypergraph links~%")
        (format port "- **Tensor Encoding**: Mathematical shape and dimensions~%")
        (format port "- **Pattern Signature**: Unique identification hash~%")
        (format port "- **Visual Styling**: Color-coded component types~%~%")
        (format port "Generated: ~a~%" (current-time))))
    (format #t "Created flowchart index: ~a~%" index-file)
    index-file))

;;; Demonstration function
(define (demo-hypergraph-visualization)
  "Demonstrate hypergraph visualization capabilities"
  (format #t "=== Hypergraph Visualization Demo ===~%~%")
  
  ;; Generate a single example
  (create-primitive-flowchart 'PORT_ALLOCATE '(IPC 1 SERVER 9 2))
  
  (format #t "~%To generate all flowcharts, run:~%")
  (format #t "(generate-all-primitive-diagrams)~%~%")
  
  #t)