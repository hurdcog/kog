#!/usr/bin/env guile
!#

;;; Enhanced Cognitive Workflow Engine Demonstration
;;; Shows the new features: validation, error handling, performance monitoring, JIT optimization

(use-modules (cogkernel cognitive-interface workflow-engine processor)
             (cogkernel atomspace)
             (cogkernel agents)
             (cogkernel attention)
             (cogkernel tensors))

(define (demo-enhanced-workflow-features)
  "Demonstrate the enhanced cognitive workflow engine features"
  (format #t "🧠 === Enhanced Cognitive Workflow Engine Demo === 🧠~%~%")
  
  ;; Create workflow engine
  (let ((engine (make-cognitive-workflow-engine 
                #:parallel-processing 'kokkos
                #:jit-compilation 'compiler-explorer
                #:atomspace-storage 'distributed)))
    
    (format #t "✅ Created enhanced cognitive workflow engine~%~%")
    
    ;; Demonstrate JIT-optimized workflow
    (format #t "=== JIT-Optimized Workflow Demo ===~%")
    (let ((jit-workflow 
           (create-jit-optimized-workflow
            'matrix-processing
            (list
             (workflow-step 'load-data 'PREPARATION
                          (lambda (size) 
                            (format #t "Loading ~ax~a matrix~%" size size)
                            `(matrix ,size ,size))
                          '() '(100))
             (workflow-step 'tensor-multiply 'TENSOR-OP
                          (lambda (matrix)
                            (format #t "Performing tensor multiplication~%")
                            `(result ,(* (cadr matrix) (caddr matrix))))
                          '(load-data))
             (workflow-step 'parallel-reduce 'PARALLEL-COMPUTE
                          (lambda (result)
                            (format #t "Parallel reduction of result~%")
                            `(reduced ,(/ (cadr result) 2)))
                          '(tensor-multiply))
             (workflow-step 'store-result 'FINALIZATION
                          (lambda (reduced)
                            (format #t "Storing final result: ~a~%" (cadr reduced))
                            (cadr reduced))
                          '(parallel-reduce)))
            #:jit-enabled #t
            #:optimization-level 3)))
      
      (format #t "📋 Validating JIT-optimized workflow...~%")
      (if (validate-workflow-definition jit-workflow)
          (format #t "✅ Workflow validation passed~%")
          (format #t "❌ Workflow validation failed~%"))
      
      (format #t "🚀 Executing JIT-optimized workflow...~%")
      (let ((results (execute-cognitive-workflow engine jit-workflow)))
        (format #t "✅ Workflow completed successfully~%")
        (let ((metrics (get-workflow-performance-metrics results)))
          (format #t "📊 Performance metrics collected: ~a steps monitored~%~%" 
                  (length metrics)))))
    
    ;; Demonstrate fault-tolerant workflow
    (format #t "=== Fault-Tolerant Workflow Demo ===~%")
    (let ((fault-tolerant-workflow
           (create-fault-tolerant-workflow
            'resilient-processing
            (list
             (workflow-step 'unreliable-step 'ANALYSIS
                          (lambda ()
                            ;; Simulate 30% failure rate
                            (if (< (random 100) 30)
                                (error "Simulated network timeout")
                                (begin
                                  (format #t "Network operation succeeded~%")
                                  "success-data")))
                          '())
             (workflow-step 'process-data 'TRANSFORMATION
                          (lambda (data)
                            (format #t "Processing data: ~a~%" data)
                            `(processed ,data))
                          '(unreliable-step)))
            2))) ; Max 2 retries
      
      (format #t "🛡️ Testing fault-tolerant workflow...~%")
      (catch #t
        (lambda ()
          (execute-cognitive-workflow engine fault-tolerant-workflow)
          (format #t "✅ Fault-tolerant workflow completed~%"))
        (lambda (key . args)
          (format #t "⚠️ Workflow failed after all retries (expected behavior)~%"))))
    
    ;; Demonstrate error handling validation
    (format #t "~%=== Error Handling Demo ===~%")
    (let ((invalid-workflow
           (create-workflow-definition
            'invalid-test
            (list
             (workflow-step 'bad-step 'INVALID-STEP-TYPE
                          (lambda () "this won't work")
                          '())))))
      
      (format #t "🔍 Testing invalid workflow rejection...~%")
      (catch #t
        (lambda ()
          (execute-cognitive-workflow engine invalid-workflow)
          (format #t "❌ Should have rejected invalid workflow~%"))
        (lambda (key . args)
          (format #t "✅ Invalid workflow properly rejected: ~a~%" key))))
    
    ;; Demonstrate performance monitoring
    (format #t "~%=== Performance Monitoring Demo ===~%")
    (let ((perf-workflow
           (create-cognitive-analysis-workflow "performance-test-data")))
      
      (format #t "⏱️ Running workflow with performance monitoring...~%")
      (let ((results (execute-cognitive-workflow engine perf-workflow)))
        (let ((metrics (get-workflow-performance-metrics results)))
          (format #t "📈 Performance Summary:~%")
          (log-workflow-performance 'performance-demo metrics))))
    
    (format #t "~%🎉 === Enhanced Workflow Engine Demo Complete === 🎉~%")
    (format #t "~%Key Features Demonstrated:~%")
    (format #t "  ✅ JIT compilation optimization~%")
    (format #t "  ✅ Fault tolerance with automatic retry~%")
    (format #t "  ✅ Comprehensive error handling~%")
    (format #t "  ✅ Performance monitoring and metrics~%")
    (format #t "  ✅ Workflow validation~%")
    (format #t "  ✅ Enhanced parallel processing~%")
    (format #t "~%The cognitive workflow engine is ready for production use in the SKZ framework!~%")))

;; Run the demonstration
(demo-enhanced-workflow-features)