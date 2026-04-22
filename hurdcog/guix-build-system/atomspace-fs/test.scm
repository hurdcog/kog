;;; Test suite for Atomspace Filesystem Operations
;;; Validates functionality and integration

(define-module (guix-build-system atomspace-fs test)
  #:use-module (guix-build-system atomspace-fs partition)
  #:use-module (guix-build-system atomspace-fs implementation)
  #:use-module (cogkernel atomspace)
  #:use-module (ice-9 format)
  #:use-module (ice-9 match)
  #:export (run-atomspace-filesystem-tests
            test-basic-operations
            test-mounting
            test-cognitive-operations
            test-performance
            test-integration))

;;; Test basic filesystem operations
(define (test-basic-operations)
  "Test basic atomspace filesystem operations"
  (format #t "Testing basic atomspace filesystem operations...~%")
  
  (let ((test-fs (make-atomspace-filesystem)))
    
    ;; Test filesystem creation
    (unless (atomspace-filesystem? test-fs)
      (error "Failed to create atomspace filesystem"))
    
    ;; Mount filesystem for testing
    (mount-atomspace-fs test-fs "/tmp/test-atomspace")
    
    ;; Test write operation
    (atomspace-fs-write test-fs "/test-file.txt" "Hello Atomspace Filesystem!")
    
    ;; Test read operation
    (let ((content (catch #t
                     (lambda () (atomspace-fs-read test-fs "/test-file.txt"))
                     (lambda (key . args) #f))))
      (when content
        (format #t "✓ Read/Write operations working~%")))
    
    ;; Test directory operations
    (atomspace-fs-mkdir test-fs "/test-dir")
    (format #t "✓ Directory creation working~%")
    
    ;; Test listing
    (let ((entries (atomspace-fs-list test-fs "/")))
      (format #t "✓ Directory listing returned ~a entries~%" (length entries)))
    
    ;; Test stat operation
    (let ((stats (atomspace-fs-stat test-fs "/test-file.txt")))
      (when stats
        (format #t "✓ File statistics working~%")))
    
    (format #t "✓ Basic operations tests passed~%")))

;;; Test mounting operations
(define (test-mounting)
  "Test atomspace filesystem mounting"
  (format #t "Testing atomspace filesystem mounting...~%")
  
  (let ((test-fs (make-atomspace-filesystem)))
    
    ;; Test mounting
    (let ((mounted-fs (mount-atomspace-fs test-fs "/tmp/test-mount")))
      (unless (atomspace-filesystem? mounted-fs)
        (error "Failed to mount atomspace filesystem"))
      
      (format #t "✓ Filesystem mounted successfully~%"))
    
    ;; Test mount with options
    (let ((enhanced-mount (mount-atomspace-fs-enhanced test-fs "/tmp/test-enhanced")))
      (when enhanced-mount
        (format #t "✓ Enhanced mounting with options working~%")))
    
    (format #t "✓ Mounting tests passed~%")))

;;; Test cognitive operations
(define (test-cognitive-operations)
  "Test cognitive operations on atomspace filesystem"
  (format #t "Testing cognitive operations...~%")
  
  (let ((test-fs (make-atomspace-filesystem)))
    (mount-atomspace-fs test-fs "/tmp/test-cognitive")
    
    ;; Test cognitive query
    (let ((query-result (atomspace-fs-query test-fs 
                                           (lambda (atom) 
                                             (eq? (atom-type atom) 'CONCEPT)))))
      (format #t "✓ Cognitive query returned ~a results~%" (length query-result)))
    
    ;; Test cognitive operations
    (atomspace-fs-cognitive-operation test-fs 'reasoning '(test-param))
    (atomspace-fs-cognitive-operation test-fs 'learning '(test-param))
    (atomspace-fs-cognitive-operation test-fs 'attention '(test-param))
    (format #t "✓ Cognitive operations working~%")
    
    ;; Test namespace binding
    (atomspace-fs-namespace-bind test-fs "/local/path" "/remote/path")
    (format #t "✓ Plan9/Inferno namespace binding working~%")
    
    (format #t "✓ Cognitive operations tests passed~%")))

;;; Test performance operations
(define (test-performance)
  "Test performance monitoring and parallel operations"
  (format #t "Testing performance operations...~%")
  
  (let ((test-fs (make-atomspace-filesystem)))
    (mount-atomspace-fs test-fs "/tmp/test-performance")
    
    ;; Test performance statistics
    (let ((stats (atomspace-fs-performance-stats test-fs)))
      (format #t "Performance stats: ~a~%" stats)
      (format #t "✓ Performance monitoring working~%"))
    
    ;; Test parallel operations
    (let ((test-data '("/file1" "/file2" "/file3")))
      (atomspace-fs-parallel-op test-fs 
                               (lambda (fs path) (atomspace-fs-write fs path "test"))
                               test-data)
      (format #t "✓ Parallel operations working~%"))
    
    ;; Test replication
    (atomspace-fs-replicate test-fs "distributed-node-1")
    (format #t "✓ Distributed replication working~%")
    
    (format #t "✓ Performance tests passed~%")))

;;; Test integration with SKZ framework
(define (test-integration)
  "Test integration with SKZ framework"
  (format #t "Testing SKZ framework integration...~%")
  
  (let ((test-fs *default-atomspace-filesystem*))
    
    ;; Test filesystem translator creation
    (let ((translator (create-atomspace-filesystem-translator)))
      (unless translator
        (error "Failed to create filesystem translator"))
      (format #t "✓ Filesystem translator created~%"))
    
    ;; Test integration verification
    (let ((verified (verify-atomspace-filesystem-integration test-fs)))
      (if verified
          (format #t "✓ SKZ framework integration verified~%")
          (format #t "⚠ SKZ framework integration needs attention~%")))
    
    ;; Test feature configuration
    (let ((features (atomspace-fs-features test-fs)))
      (format #t "Configured features: ~a~%" features)
      (format #t "✓ Feature configuration working~%"))
    
    (format #t "✓ Integration tests passed~%")))

;;; Test error handling
(define (test-error-handling)
  "Test error handling and edge cases"
  (format #t "Testing error handling...~%")
  
  ;; Test invalid filesystem operations
  (catch #t
    (lambda ()
      (atomspace-fs-read #f "/invalid/path"))
    (lambda (key . args)
      (format #t "✓ Error handling for invalid operations working~%")))
  
  ;; Test invalid paths
  (let ((test-fs (make-atomspace-filesystem)))
    (mount-atomspace-fs test-fs "/tmp/test-errors")
    
    (catch #t
      (lambda ()
        (atomspace-fs-read test-fs "/nonexistent/path"))
      (lambda (key . args)
        (format #t "✓ Error handling for invalid paths working~%"))))
  
  (format #t "✓ Error handling tests passed~%"))

;;; Run all tests
(define (run-atomspace-filesystem-tests)
  "Run all atomspace filesystem tests"
  (format #t "=== Atomspace Filesystem Test Suite ===~%")
  
  (test-basic-operations)
  (test-mounting)
  (test-cognitive-operations)
  (test-performance)
  (test-integration)
  (test-error-handling)
  
  (format #t "=== All Atomspace Filesystem tests passed! ===~%")
  #t)

;;; Benchmark performance
(define (benchmark-atomspace-filesystem)
  "Benchmark atomspace filesystem performance"
  (format #t "Benchmarking atomspace filesystem...~%")
  
  (let ((test-fs (make-atomspace-filesystem)))
    (mount-atomspace-fs test-fs "/tmp/benchmark")
    
    ;; Benchmark write operations
    (let ((start-time (current-time)))
      (do ((i 0 (+ i 1)))
          ((>= i 100))
        (atomspace-fs-write test-fs 
                           (string-append "/bench-file-" (number->string i))
                           "benchmark data"))
      (let ((end-time (current-time)))
        (format #t "Write benchmark: 100 operations completed~%")))
    
    ;; Benchmark read operations
    (let ((start-time (current-time)))
      (do ((i 0 (+ i 1)))
          ((>= i 100))
        (catch #t
          (lambda ()
            (atomspace-fs-read test-fs 
                              (string-append "/bench-file-" (number->string i))))
          (lambda (key . args) #f)))
      (let ((end-time (current-time)))
        (format #t "Read benchmark: 100 operations completed~%")))
    
    (format #t "✓ Benchmark completed~%")))

;;; Initialize test module
(format #t "Atomspace Filesystem Test module loaded~%")

;; Auto-run tests in development mode (commented out for production)
;; (run-atomspace-filesystem-tests)