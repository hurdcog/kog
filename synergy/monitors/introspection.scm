;;; synergy/monitors/introspection.scm
;;; Introspection Membrane - Self-Monitoring and Performance Tracking
;;; Enables the system to observe its own cognitive state and performance

(define-module (synergy monitors introspection)
  #:use-module (ice-9 threads)
  #:use-module (ice-9 match)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:use-module (srfi srfi-19)  ;; Time
  #:export (
    ;; Monitor lifecycle
    introspection-monitor-create
    introspection-monitor-start
    introspection-monitor-stop
    introspection-monitor-status
    
    ;; Metric collection
    record-metric
    get-metric
    get-all-metrics
    clear-metrics
    
    ;; Performance tracking
    track-component-interaction
    track-hypergraph-growth
    track-resource-usage
    track-emergent-behavior
    
    ;; Analysis
    analyze-performance
    detect-anomalies
    generate-introspection-report
    
    ;; Self-awareness
    cognitive-state-snapshot
    compare-cognitive-states
    identify-learning-patterns
  ))

;;;; ============================================================================
;;;; Type Definitions
;;;; ============================================================================

(define-record-type <introspection-monitor>
  (make-introspection-monitor-internal metrics-store running? start-time config)
  introspection-monitor?
  (metrics-store monitor-metrics-store monitor-set-metrics-store!)
  (running? monitor-running? monitor-set-running!)
  (start-time monitor-start-time monitor-set-start-time!)
  (config monitor-config monitor-set-config!))

(define-record-type <metric>
  (make-metric name value timestamp metadata)
  metric?
  (name metric-name)
  (value metric-value)
  (timestamp metric-timestamp)
  (metadata metric-metadata))

(define-record-type <cognitive-state>
  (make-cognitive-state timestamp component-states hypergraph-size resource-usage)
  cognitive-state?
  (timestamp cognitive-state-timestamp)
  (component-states cognitive-state-components)
  (hypergraph-size cognitive-state-hypergraph-size)
  (resource-usage cognitive-state-resources))

;;;; ============================================================================
;;;; Monitor Lifecycle
;;;; ============================================================================

(define* (introspection-monitor-create #:key (config '()))
  "Create a new introspection monitor with optional configuration"
  (make-introspection-monitor-internal
    (make-hash-table)  ;; metrics-store
    #f                 ;; running?
    #f                 ;; start-time
    (merge-with-default-config config)))

(define (merge-with-default-config config)
  "Merge user config with default configuration"
  (let ((defaults '((sampling-interval . 1.0)  ;; seconds
                    (history-size . 1000)
                    (anomaly-threshold . 2.0)  ;; standard deviations
                    (enable-auto-analysis . #t))))
    (append config defaults)))

(define (introspection-monitor-start monitor)
  "Start the introspection monitor"
  (monitor-set-running! monitor #t)
  (monitor-set-start-time! monitor (current-time))
  (format #t "Introspection monitor started at ~a~%" (current-time))
  monitor)

(define (introspection-monitor-stop monitor)
  "Stop the introspection monitor"
  (monitor-set-running! monitor #f)
  (format #t "Introspection monitor stopped. Runtime: ~a seconds~%"
          (time-difference (current-time) (monitor-start-time monitor)))
  monitor)

(define (introspection-monitor-status monitor)
  "Get current status of the introspection monitor"
  `((running? . ,(monitor-running? monitor))
    (start-time . ,(monitor-start-time monitor))
    (metrics-count . ,(hash-count (const #t) (monitor-metrics-store monitor)))
    (uptime . ,(if (monitor-start-time monitor)
                   (time-difference (current-time) (monitor-start-time monitor))
                   0))))

;;;; ============================================================================
;;;; Metric Collection
;;;; ============================================================================

(define (record-metric monitor name value metadata)
  "Record a new metric value with timestamp and metadata"
  (let* ((store (monitor-metrics-store monitor))
         (metric (make-metric name value (current-time) metadata))
         (existing (hash-ref store name '())))
    (hash-set! store name (cons metric existing))
    ;; Trim history if needed
    (let ((max-history (assoc-ref (monitor-config monitor) 'history-size)))
      (when (> (length existing) max-history)
        (hash-set! store name (take existing max-history))))
    metric))

(define (get-metric monitor name)
  "Get the most recent value of a metric"
  (let ((metrics (hash-ref (monitor-metrics-store monitor) name '())))
    (if (null? metrics)
        #f
        (car metrics))))

(define (get-all-metrics monitor name)
  "Get all recorded values of a metric"
  (hash-ref (monitor-metrics-store monitor) name '()))

(define (clear-metrics monitor)
  "Clear all recorded metrics"
  (monitor-set-metrics-store! monitor (make-hash-table))
  #t)

;;;; ============================================================================
;;;; Performance Tracking
;;;; ============================================================================

(define (track-component-interaction monitor source target interaction-type)
  "Track an interaction between two components"
  (record-metric monitor 'component-interaction
                 `((source . ,source)
                   (target . ,target)
                   (type . ,interaction-type))
                 `((category . interaction))))

(define (track-hypergraph-growth monitor node-count edge-count)
  "Track the growth of the hypergraph knowledge base"
  (record-metric monitor 'hypergraph-nodes node-count
                 `((category . hypergraph)))
  (record-metric monitor 'hypergraph-edges edge-count
                 `((category . hypergraph))))

(define (track-resource-usage monitor cpu-usage memory-usage)
  "Track system resource usage"
  (record-metric monitor 'cpu-usage cpu-usage
                 `((category . resources) (unit . percent)))
  (record-metric monitor 'memory-usage memory-usage
                 `((category . resources) (unit . megabytes))))

(define (track-emergent-behavior monitor behavior-description confidence)
  "Track detection of emergent behavior"
  (record-metric monitor 'emergent-behavior
                 `((description . ,behavior-description)
                   (confidence . ,confidence))
                 `((category . emergence))))

;;;; ============================================================================
;;;; Analysis
;;;; ============================================================================

(define (analyze-performance monitor metric-name)
  "Analyze performance of a specific metric over time"
  (let* ((metrics (get-all-metrics monitor metric-name))
         (values (map metric-value metrics))
         (count (length values)))
    (if (< count 2)
        '((status . insufficient-data))
        (let* ((mean (/ (apply + values) count))
               (variance (/ (apply + (map (lambda (x) (expt (- x mean) 2)) values))
                           count))
               (stddev (sqrt variance)))
          `((metric . ,metric-name)
            (count . ,count)
            (mean . ,mean)
            (stddev . ,stddev)
            (min . ,(apply min values))
            (max . ,(apply max values))
            (latest . ,(car values)))))))

(define (detect-anomalies monitor metric-name)
  "Detect anomalies in a metric using statistical analysis"
  (let* ((analysis (analyze-performance monitor metric-name))
         (threshold (assoc-ref (monitor-config monitor) 'anomaly-threshold)))
    (if (eq? (assoc-ref analysis 'status) 'insufficient-data)
        '()
        (let* ((mean (assoc-ref analysis 'mean))
               (stddev (assoc-ref analysis 'stddev))
               (latest (assoc-ref analysis 'latest))
               (z-score (if (zero? stddev) 0 (/ (- latest mean) stddev))))
          (if (> (abs z-score) threshold)
              `((anomaly-detected . #t)
                (metric . ,metric-name)
                (z-score . ,z-score)
                (latest . ,latest)
                (expected-range . (,(- mean (* threshold stddev))
                                   ,(+ mean (* threshold stddev)))))
              `((anomaly-detected . #f)))))))

(define (generate-introspection-report monitor)
  "Generate a comprehensive introspection report"
  (let* ((status (introspection-monitor-status monitor))
         (store (monitor-metrics-store monitor))
         (metric-names (hash-map->list (lambda (k v) k) store)))
    `((status . ,status)
      (metrics-summary . ,(map (lambda (name)
                                 (cons name (analyze-performance monitor name)))
                               metric-names))
      (anomalies . ,(filter (lambda (x) (assoc-ref x 'anomaly-detected))
                           (map (lambda (name) (detect-anomalies monitor name))
                                metric-names)))
      (timestamp . ,(current-time)))))

;;;; ============================================================================
;;;; Self-Awareness
;;;; ============================================================================

(define (cognitive-state-snapshot monitor component-states hypergraph-size)
  "Create a snapshot of the current cognitive state"
  (let ((resource-usage (get-current-resource-usage monitor)))
    (make-cognitive-state
      (current-time)
      component-states
      hypergraph-size
      resource-usage)))

(define (get-current-resource-usage monitor)
  "Get current resource usage from metrics"
  `((cpu . ,(or (metric-value (get-metric monitor 'cpu-usage)) 0))
    (memory . ,(or (metric-value (get-metric monitor 'memory-usage)) 0))))

(define (compare-cognitive-states state1 state2)
  "Compare two cognitive states to identify changes"
  `((time-delta . ,(time-difference (cognitive-state-timestamp state2)
                                    (cognitive-state-timestamp state1)))
    (hypergraph-growth . ,(- (cognitive-state-hypergraph-size state2)
                            (cognitive-state-hypergraph-size state1)))
    (component-changes . ,(identify-component-changes
                           (cognitive-state-components state1)
                           (cognitive-state-components state2)))))

(define (identify-component-changes old-states new-states)
  "Identify changes in component states"
  ;; Simple implementation - can be enhanced based on component structure
  (let ((old-keys (map car old-states))
        (new-keys (map car new-states)))
    `((added . ,(lset-difference equal? new-keys old-keys))
      (removed . ,(lset-difference equal? old-keys new-keys))
      (modified . ,(filter (lambda (key)
                            (and (member key old-keys)
                                 (member key new-keys)
                                 (not (equal? (assoc key old-states)
                                            (assoc key new-states)))))
                          new-keys)))))

(define (identify-learning-patterns monitor)
  "Identify patterns in the learning process over time"
  (let* ((interactions (get-all-metrics monitor 'component-interaction))
         (emergent (get-all-metrics monitor 'emergent-behavior))
         (hypergraph-nodes (get-all-metrics monitor 'hypergraph-nodes)))
    `((interaction-frequency . ,(length interactions))
      (emergence-events . ,(length emergent))
      (knowledge-growth-rate . ,(if (< (length hypergraph-nodes) 2)
                                    0
                                    (let ((recent (take hypergraph-nodes 10))
                                          (older (drop hypergraph-nodes 10)))
                                      (if (null? older)
                                          0
                                          (- (/ (apply + (map metric-value recent))
                                               (length recent))
                                             (/ (apply + (map metric-value older))
                                               (length older))))))))))

;;;; ============================================================================
;;;; Utility Functions
;;;; ============================================================================

(define (current-time)
  "Get current timestamp"
  (make-time time-utc 0 (car (gettimeofday))))

(define (time-difference t2 t1)
  "Calculate time difference in seconds"
  (if (and t1 t2)
      (- (time-second t2) (time-second t1))
      0))

;;; Module initialization
(format #t "Introspection Monitor module loaded~%")
