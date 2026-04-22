;;; Cognitive Primitives - OpenCog GNUHurd Integration Phase 1
;;; Implements tensor fragment architecture with cognitive primitive encoding
;;; Tensor shape: [modality, depth, context, salience, autonomy_index]

(define-module (cogkernel cognitive-primitives)
  #:use-module (ice-9 match)
  #:use-module (ice-9 format)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (srfi srfi-26)
  #:use-module (cogkernel tensors)
  #:use-module (cogkernel atomspace)
  #:export (make-cognitive-fragment
            cognitive-fragment?
            cognitive-fragment-tensor
            cognitive-fragment-atoms
            cognitive-fragment-modality
            encode-gnumach-primitive
            decode-to-gnumach-primitive
            create-hypergraph-pattern
            compute-prime-factorization
            cognitive-tensor-shape
            *cognitive-modalities*
            *cognitive-contexts*
            test-round-trip-translation
            get-timestamp))

;;; Helper function for timestamps
(define (get-timestamp)
  "Get current timestamp as a number"
  (get-internal-real-time))

;;; Cognitive fragment record combining tensor and hypergraph representations
(define-record-type <cognitive-fragment>
  (make-cognitive-fragment-record tensor atoms modality primitive-type metadata)
  cognitive-fragment?
  (tensor cognitive-fragment-tensor)
  (atoms cognitive-fragment-atoms set-cognitive-fragment-atoms!)
  (modality cognitive-fragment-modality)
  (primitive-type cognitive-fragment-primitive-type)
  (metadata cognitive-fragment-metadata set-cognitive-fragment-metadata!))

;;; Cognitive modalities for tensor encoding
(define *cognitive-modalities*
  '((IPC . 0)           ; Inter-process communication
    (MEMORY . 1)        ; Memory management  
    (FILESYSTEM . 2)    ; File system operations
    (NETWORK . 3)       ; Network operations
    (SECURITY . 4)      ; Security and capabilities
    (SCHEDULER . 5)     ; Process scheduling
    (DEVICE . 6)        ; Device management
    (SIGNAL . 7)))      ; Signal handling

;;; Cognitive contexts for semantic encoding
(define *cognitive-contexts*
  '((KERNEL . 0)        ; Microkernel operations
    (SERVER . 1)        ; Hurd server operations
    (TRANSLATOR . 2)    ; Filesystem translator operations
    (USER . 3)          ; User-space operations
    (SYSTEM . 4)        ; System-wide operations
    (DEBUG . 5)         ; Debugging and introspection
    (META . 6)          ; Meta-cognitive operations
    (EVOLUTION . 7)))   ; Self-modification operations

;;; Standard cognitive tensor shape: [modality, depth, context, salience, autonomy_index]
(define cognitive-tensor-shape '(8 4 8 10 5))

;;; Create cognitive fragment from GNUMach primitive
(define (make-cognitive-fragment primitive-type modality depth context salience autonomy-index)
  "Create a cognitive fragment encoding a GNUMach primitive as hypergraph + tensor"
  (let* ((tensor-data (create-cognitive-tensor-data modality depth context salience autonomy-index))
         (tensor (make-tensor cognitive-tensor-shape tensor-data))
         (atoms (create-primitive-atoms primitive-type modality context)))
    (make-cognitive-fragment-record tensor atoms modality primitive-type 
                                    `((depth . ,depth)
                                      (context . ,context) 
                                      (salience . ,salience)
                                      (autonomy . ,autonomy-index)
                                      (timestamp . ,(get-timestamp))))))

;;; Create tensor data for cognitive encoding
(define (create-cognitive-tensor-data modality depth context salience autonomy-index)
  "Create tensor data encoding cognitive parameters"
  (let ((total-size (fold * 1 cognitive-tensor-shape)))
    (map (lambda (i)
           (let ((mod-factor (modulo i 8))
                 (depth-factor (modulo (quotient i 8) 4))
                 (context-factor (modulo (quotient i 32) 8))
                 (salience-factor (modulo (quotient i 256) 10))
                 (autonomy-factor (modulo (quotient i 2560) 5)))
             (* (if (= mod-factor modality) 1.0 0.1)
                (if (= depth-factor depth) 1.0 0.2)
                (if (= context-factor context) 1.0 0.15)
                (/ salience-factor 10.0)
                (/ autonomy-factor 5.0))))
         (iota total-size))))

;;; Create AtomSpace representation of primitive
(define (create-primitive-atoms primitive-type modality context)
  "Create hypergraph atoms representing a GNUMach primitive"
  (let ((modality-name (car (find (lambda (pair) (= (cdr pair) modality)) *cognitive-modalities*)))
        (context-name (car (find (lambda (pair) (= (cdr pair) context)) *cognitive-contexts*))))
    (list (make-atom 'CONCEPT primitive-type)
          (make-atom 'CONCEPT modality-name)
          (make-atom 'CONCEPT context-name)
          (make-link 'INHERITANCE (list primitive-type modality-name))
          (make-link 'EVALUATION 
                     (list 'OPERATES-IN 
                           (list primitive-type context-name))))))

;;; Encode GNUMach primitive to cognitive fragment
(define (encode-gnumach-primitive primitive-type properties)
  "Encode a GNUMach primitive as a cognitive fragment with hypergraph + tensor"
  (match properties
    ((modality depth context salience autonomy)
     (let ((mod-index (or (assoc-ref *cognitive-modalities* modality) 0))
           (ctx-index (or (assoc-ref *cognitive-contexts* context) 0)))
       (make-cognitive-fragment primitive-type mod-index depth ctx-index salience autonomy)))
    (_ (error "Invalid properties format. Expected: (modality depth context salience autonomy)"))))

;;; Decode cognitive fragment back to GNUMach primitive
(define (decode-to-gnumach-primitive fragment)
  "Decode a cognitive fragment back to GNUMach primitive representation"
  (let* ((metadata (cognitive-fragment-metadata fragment))
         (modality (cognitive-fragment-modality fragment))
         (depth (assoc-ref metadata 'depth))
         (context (assoc-ref metadata 'context))
         (salience (assoc-ref metadata 'salience))
         (autonomy (assoc-ref metadata 'autonomy))
         (modality-name (car (find (lambda (pair) (= (cdr pair) modality)) *cognitive-modalities*)))
         (context-name (car (find (lambda (pair) (= (cdr pair) context)) *cognitive-contexts*))))
    `((primitive-type . ,(cognitive-fragment-primitive-type fragment))
      (modality . ,modality-name)
      (depth . ,depth)
      (context . ,context-name)
      (salience . ,salience)
      (autonomy . ,autonomy)
      (atoms . ,(cognitive-fragment-atoms fragment))
      (tensor-shape . ,(tensor-shape (cognitive-fragment-tensor fragment))))))

;;; Create hypergraph pattern from cognitive fragment
(define (create-hypergraph-pattern fragment)
  "Create a hypergraph pattern representation from cognitive fragment"
  (let ((atoms (cognitive-fragment-atoms fragment))
        (tensor (cognitive-fragment-tensor fragment)))
    `((nodes . ,(filter atom? atoms))
      (links . ,(filter link? atoms))
      (tensor-encoding . ,(tensor-data tensor))
      (pattern-signature . ,(compute-pattern-signature atoms))
      (prime-factorization . ,(compute-prime-factorization (tensor-data tensor))))))

;;; Compute pattern signature for hypergraph
(define (compute-pattern-signature atoms)
  "Compute a unique signature for a hypergraph pattern"
  (let* ((atom-types (map atom-type (filter atom? atoms)))
         (link-types (map link-type (filter link? atoms)))
         (type-counts (map (lambda (type) 
                             (cons type (count (cut eq? <> type) 
                                              (append atom-types link-types))))
                          (delete-duplicates (append atom-types link-types)))))
    (fold (lambda (pair acc) 
            (+ acc (* (cdr pair) (string-hash (symbol->string (car pair)) 1000))))
          0 type-counts)))

;;; Compute prime factorization for tensor data
(define (compute-prime-factorization data)
  "Compute prime factorization mapping for tensor data"
  (define (next-prime n)
    "Find next prime number after n"
    (define (is-prime? num)
      (if (< num 2) #f
          (not (any (lambda (i) (zero? (modulo num i)))
                   (iota (- (floor (sqrt num)) 1) 2)))))
    (let loop ((candidate (+ n 1)))
      (if (is-prime? candidate) candidate (loop (+ candidate 1)))))
  
  (let ((primes (let loop ((n 2) (count 0) (acc '()))
                  (if (>= count 20) (reverse acc)
                      (loop (next-prime n) (+ count 1) (cons n acc))))))
    (map (lambda (value index)
           (let ((prime (list-ref primes (modulo index (length primes))))
                 (scaled-value (inexact->exact (round (* value 100)))))
             (cons prime (modulo scaled-value prime))))
         data (iota (length data)))))

;;; Round-trip translation test
(define (test-round-trip-translation primitive-type properties)
  "Test bidirectional translation between GNUMach primitives and cognitive fragments"
  (format #t "=== Round-trip Translation Test ===~%")
  (format #t "Original primitive: ~a with properties: ~a~%" primitive-type properties)
  
  ;; Encode to cognitive fragment
  (let* ((fragment (encode-gnumach-primitive primitive-type properties))
         (decoded (decode-to-gnumach-primitive fragment))
         (pattern (create-hypergraph-pattern fragment)))
    
    (format #t "~%Encoded cognitive fragment:~%")
    (format #t "  Tensor shape: ~a~%" (tensor-shape (cognitive-fragment-tensor fragment)))
    (format #t "  Modality: ~a~%" (cognitive-fragment-modality fragment))
    (format #t "  Atoms count: ~a~%" (length (cognitive-fragment-atoms fragment)))
    
    (format #t "~%Hypergraph pattern:~%")
    (format #t "  Nodes: ~a~%" (length (assoc-ref pattern 'nodes)))
    (format #t "  Links: ~a~%" (length (assoc-ref pattern 'links)))
    (format #t "  Pattern signature: ~a~%" (assoc-ref pattern 'pattern-signature))
    
    (format #t "~%Decoded back to primitive:~%")
    (format #t "  Primitive type: ~a~%" (assoc-ref decoded 'primitive-type))
    (format #t "  Modality: ~a~%" (assoc-ref decoded 'modality))
    (format #t "  Context: ~a~%" (assoc-ref decoded 'context))
    (format #t "  Salience: ~a~%" (assoc-ref decoded 'salience))
    
    ;; Verify round-trip integrity
    (let ((original-modality (car properties))
          (decoded-modality (assoc-ref decoded 'modality)))
      (if (eq? original-modality decoded-modality)
          (format #t "~%✅ Round-trip translation successful!~%")
          (format #t "~%❌ Round-trip translation failed! Modality mismatch.~%")))
    
    (list fragment decoded pattern)))

;;; Example GNUMach primitives for testing
(define gnumach-primitives-examples
  '((VM_ALLOCATE (MEMORY 2 KERNEL 8 3))
    (PORT_ALLOCATE (IPC 1 SERVER 9 2))
    (THREAD_CREATE (SCHEDULER 3 SYSTEM 7 4))
    (FILE_OPEN (FILESYSTEM 1 TRANSLATOR 6 1))
    (NETWORK_SEND (NETWORK 2 USER 5 2))
    (SIGNAL_POST (SIGNAL 1 SYSTEM 8 3))))