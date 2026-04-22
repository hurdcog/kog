;;; test-neural-symbolic.scm --- Test Neural-Symbolic Integration
;;;
;;; Copyright (C) 2025 Deep Tree Echo Project

(add-to-load-path "/home/ubuntu/occ/src")
(use-modules (core memory atomspace))
(use-modules (core grammar neural-symbolic))
(use-modules (srfi srfi-64))  ; Testing library
(use-modules (srfi srfi-69))  ; Hash tables

(test-begin "neural-symbolic")

;;;
;;; Test Activation Tracking
;;;

(test-group "activation-tracking"
  
  (test-assert "track single activation"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (history-table (make-hash-table)))
      
      ;; Track activation
      (track-activation history-table cat 0 1.0 (list cat))
      
      ;; Verify history was created
      (let ((history (hash-table-ref/default history-table cat #f)))
        (and history
             (= (length (activation-history-timestamps history)) 1)
             (= (car (activation-history-values history)) 1.0)))))
  
  (test-assert "track multiple activations"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (history-table (make-hash-table)))
      
      ;; Track multiple activations
      (track-activation history-table cat 0 1.0 (list cat))
      (track-activation history-table cat 1 0.8 (list cat))
      (track-activation history-table cat 2 0.6 (list cat))
      
      ;; Verify history
      (let ((history (hash-table-ref/default history-table cat #f)))
        (and history
             (= (length (activation-history-timestamps history)) 3)
             (= (length (activation-history-values history)) 3)))))
  
  (test-assert "track co-activations"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (mammal (create-concept-node as "Mammal"))
           (history-table (make-hash-table)))
      
      ;; Track co-activations
      (track-activation history-table cat 0 1.0 (list cat mammal))
      (track-activation history-table mammal 0 0.8 (list cat mammal))
      
      ;; Verify co-activation was recorded
      (let ((cat-history (hash-table-ref history-table cat)))
        (= (hash-table-ref/default (activation-history-coactivations cat-history)
                                   mammal
                                   0)
           1)))))

;;;
;;; Test Coherence Computation
;;;

(test-group "coherence"
  
  (test-assert "compute coherence for frequently co-activated atoms"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (mammal (create-concept-node as "Mammal"))
           (history-table (make-hash-table)))
      
      ;; Create activation pattern: cat and mammal often co-activate
      (track-activation history-table cat 0 1.0 (list cat mammal))
      (track-activation history-table cat 1 0.9 (list cat mammal))
      (track-activation history-table cat 2 0.8 (list cat mammal))
      (track-activation history-table mammal 0 0.8 (list cat mammal))
      (track-activation history-table mammal 1 0.7 (list cat mammal))
      (track-activation history-table mammal 2 0.6 (list cat mammal))
      
      ;; Compute coherence
      (let ((cat-history (hash-table-ref history-table cat))
            (mammal-history (hash-table-ref history-table mammal)))
        (let ((coherence (compute-coherence cat-history mammal-history)))
          (format #t "~%Coherence: ~a~%" coherence)
          (> coherence 0.8)))))  ; Should be high
  
  (test-assert "compute low coherence for rarely co-activated atoms"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (dog (create-concept-node as "Dog"))
           (history-table (make-hash-table)))
      
      ;; Create activation pattern: cat and dog rarely co-activate
      (track-activation history-table cat 0 1.0 (list cat))
      (track-activation history-table cat 1 0.9 (list cat))
      (track-activation history-table cat 2 0.8 (list cat dog))  ; Only once
      (track-activation history-table dog 0 0.7 (list dog))
      (track-activation history-table dog 1 0.6 (list dog))
      (track-activation history-table dog 2 0.5 (list cat dog))
      
      ;; Compute coherence
      (let ((cat-history (hash-table-ref history-table cat))
            (dog-history (hash-table-ref history-table dog)))
        (let ((coherence (compute-coherence cat-history dog-history)))
          (format #t "~%Low coherence: ~a~%" coherence)
          (< coherence 0.5))))))  ; Should be low

;;;
;;; Test Cluster Detection
;;;

(test-group "cluster-detection"
  
  (test-assert "detect single cluster"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (whiskers (create-concept-node as "Whiskers"))
           (meow (create-concept-node as "Meow"))
           (history-table (make-hash-table)))
      
      ;; Create activation pattern: cat, whiskers, meow often co-activate
      (for-each
        (lambda (i)
          (track-activation history-table cat i 1.0 (list cat whiskers meow))
          (track-activation history-table whiskers i 0.9 (list cat whiskers meow))
          (track-activation history-table meow i 0.8 (list cat whiskers meow)))
        (iota 5))
      
      ;; Detect clusters
      (let ((clusters (detect-activation-clusters history-table 0.7)))
        (format #t "~%Found ~a clusters~%" (length clusters))
        (and (= (length clusters) 1)
             (= (length (concept-cluster-atoms (car clusters))) 3)))))
  
  (test-assert "detect multiple clusters"
    (let* ((as (make-atomspace))
           ;; Cluster 1: Feline concepts
           (cat (create-concept-node as "Cat"))
           (whiskers (create-concept-node as "Whiskers"))
           ;; Cluster 2: Canine concepts
           (dog (create-concept-node as "Dog"))
           (bark (create-concept-node as "Bark"))
           (history-table (make-hash-table)))
      
      ;; Create two separate activation patterns
      (for-each
        (lambda (i)
          ;; Feline cluster
          (track-activation history-table cat i 1.0 (list cat whiskers))
          (track-activation history-table whiskers i 0.9 (list cat whiskers))
          ;; Canine cluster
          (track-activation history-table dog i 0.8 (list dog bark))
          (track-activation history-table bark i 0.7 (list dog bark)))
        (iota 5))
      
      ;; Detect clusters
      (let ((clusters (detect-activation-clusters history-table 0.7)))
        (format #t "~%Found ~a clusters~%" (length clusters))
        (= (length clusters) 2)))))

;;;
;;; Test Concept Formation
;;;

(test-group "concept-formation"
  
  (test-assert "form concept from activation pattern"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (whiskers (create-concept-node as "Whiskers"))
           (meow (create-concept-node as "Meow"))
           (history-table (make-hash-table)))
      
      ;; Create activation pattern
      (for-each
        (lambda (i)
          (track-activation history-table cat i 1.0 (list cat whiskers meow))
          (track-activation history-table whiskers i 0.9 (list cat whiskers meow))
          (track-activation history-table meow i 0.8 (list cat whiskers meow)))
        (iota 5))
      
      ;; Form concepts
      (let ((concepts (form-concepts-from-activation as history-table 0.7)))
        (format #t "~%Formed ~a concepts~%" (length concepts))
        (and (= (length concepts) 1)
             (atom? (car concepts))
             (equal? (atom-type (car concepts)) 'ConceptNode)))))
  
  (test-assert "concept has appropriate truth value"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (whiskers (create-concept-node as "Whiskers"))
           (history-table (make-hash-table)))
      
      ;; Create strong activation pattern
      (for-each
        (lambda (i)
          (track-activation history-table cat i 1.0 (list cat whiskers))
          (track-activation history-table whiskers i 0.9 (list cat whiskers)))
        (iota 10))
      
      ;; Form concepts
      (let ((concepts (form-concepts-from-activation as history-table 0.8)))
        (and (= (length concepts) 1)
             (let ((concept (car concepts)))
               (let ((tv (atom-tv concept)))
                 (format #t "~%Concept TV: strength=~a confidence=~a~%"
                        (tv-strength tv)
                        (tv-confidence tv))
                 (> (truth-value-strength tv) 0.8)))))))
  
  (test-assert "concept links to constituent atoms"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (whiskers (create-concept-node as "Whiskers"))
           (history-table (make-hash-table)))
      
      ;; Create activation pattern
      (for-each
        (lambda (i)
          (track-activation history-table cat i 1.0 (list cat whiskers))
          (track-activation history-table whiskers i 0.9 (list cat whiskers)))
        (iota 5))
      
      ;; Form concepts
      (let ((concepts (form-concepts-from-activation as history-table 0.7)))
        ;; Check that inheritance links were created
        (let ((inh-links (atomspace-get-atoms-by-type as 'InheritanceLink)))
          (format #t "~%Created ~a inheritance links~%" (length inh-links))
          (>= (length inh-links) 2))))))  ; At least 2 links (cat→concept, whiskers→concept)

;;;
;;; Test Co-Activation Pattern Mining
;;;

(test-group "pattern-mining"
  
  (test-assert "mine frequent co-activation patterns"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (mammal (create-concept-node as "Mammal"))
           (history-table (make-hash-table)))
      
      ;; Create pattern: cat often activates with mammal
      (for-each
        (lambda (i)
          (track-activation history-table cat i 1.0 (list cat mammal))
          (track-activation history-table mammal i 0.8 (list cat mammal)))
        (iota 10))
      
      ;; Mine patterns
      (let ((patterns (mine-coactivation-patterns history-table 0.5)))
        (format #t "~%Found ~a patterns~%" (length patterns))
        (> (length patterns) 0))))
  
  (test-assert "pattern has correct support and confidence"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (mammal (create-concept-node as "Mammal"))
           (history-table (make-hash-table)))
      
      ;; Create strong pattern
      (for-each
        (lambda (i)
          (track-activation history-table cat i 1.0 (list cat mammal))
          (track-activation history-table mammal i 0.8 (list cat mammal)))
        (iota 10))
      
      ;; Mine patterns
      (let ((patterns (mine-coactivation-patterns history-table 0.5)))
        (and (> (length patterns) 0)
             (let ((pattern (car patterns)))
               (let ((support (caddr pattern))
                     (confidence (cadddr pattern)))
                 (format #t "~%Pattern: support=~a confidence=~a~%" support confidence)
                 (and (> support 0.8)
                      (> confidence 0.8)))))))))

;;;
;;; Test Rule Extraction
;;;

(test-group "rule-extraction"
  
  (test-assert "extract rules from co-activation"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (mammal (create-concept-node as "Mammal"))
           (history-table (make-hash-table)))
      
      ;; Create pattern: cat → mammal
      (for-each
        (lambda (i)
          (track-activation history-table cat i 1.0 (list cat mammal))
          (track-activation history-table mammal i 0.8 (list cat mammal)))
        (iota 10))
      
      ;; Extract rules
      (let ((rules (extract-rules-from-coactivation as history-table 0.5 0.7)))
        (format #t "~%Extracted ~a rules~%" (length rules))
        (> (length rules) 0))))
  
  (test-assert "extracted rule has correct structure"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (mammal (create-concept-node as "Mammal"))
           (history-table (make-hash-table)))
      
      ;; Create pattern
      (for-each
        (lambda (i)
          (track-activation history-table cat i 1.0 (list cat mammal))
          (track-activation history-table mammal i 0.8 (list cat mammal)))
        (iota 10))
      
      ;; Extract rules
      (let ((rules (extract-rules-from-coactivation as history-table 0.5 0.7)))
        (and (> (length rules) 0)
             (let ((rule (car rules)))
               (and (inference-rule? rule)
                    (> (inference-rule-strength rule) 0.7)))))))
  
  (test-assert "rule creates implication link in atomspace"
    (let* ((as (make-atomspace))
           (cat (create-concept-node as "Cat"))
           (mammal (create-concept-node as "Mammal"))
           (history-table (make-hash-table)))
      
      ;; Create pattern
      (for-each
        (lambda (i)
          (track-activation history-table cat i 1.0 (list cat mammal))
          (track-activation history-table mammal i 0.8 (list cat mammal)))
        (iota 10))
      
      ;; Extract rules
      (let ((rules (extract-rules-from-coactivation as history-table 0.5 0.7)))
        ;; Check that implication links were created
        (let ((impl-links (atomspace-get-atoms-by-type as 'ImplicationLink)))
          (format #t "~%Created ~a implication links~%" (length impl-links))
          (> (length impl-links) 0))))))

;;;
;;; Performance Tests
;;;

(test-group "performance"
  
  (test-assert "concept formation completes in reasonable time"
    (let* ((as (make-atomspace))
           (atoms (map (lambda (i)
                        (create-concept-node as (string-append "Atom" (number->string i))))
                      (iota 20)))
           (history-table (make-hash-table)))
      
      ;; Create activation patterns
      (for-each
        (lambda (t)
          (let ((active-atoms (list-head atoms 10)))  ; First 10 atoms co-activate
            (for-each
              (lambda (atom)
                (track-activation history-table atom t 1.0 active-atoms))
              active-atoms)))
        (iota 50))
      
      ;; Measure time
      (let ((start-time (get-internal-real-time)))
        (let ((concepts (form-concepts-from-activation as history-table 0.7)))
          (let* ((end-time (get-internal-real-time))
                 (elapsed-ms (/ (- end-time start-time) 1000.0)))
            (format #t "~%Formed ~a concepts in ~a ms~%" (length concepts) elapsed-ms)
            (< elapsed-ms 100))))))  ; Should be < 100ms
  
  (test-assert "rule extraction completes in reasonable time"
    (let* ((as (make-atomspace))
           (atoms (map (lambda (i)
                        (create-concept-node as (string-append "Atom" (number->string i))))
                      (iota 20)))
           (history-table (make-hash-table)))
      
      ;; Create activation patterns
      (for-each
        (lambda (t)
          (for-each
            (lambda (atom)
              (track-activation history-table atom t 1.0 atoms))
            atoms))
        (iota 50))
      
      ;; Measure time
      (let ((start-time (get-internal-real-time)))
        (let ((rules (extract-rules-from-coactivation as history-table 0.5 0.7)))
          (let* ((end-time (get-internal-real-time))
                 (elapsed-ms (/ (- end-time start-time) 1000.0)))
            (format #t "~%Extracted ~a rules in ~a ms~%" (length rules) elapsed-ms)
            (< elapsed-ms 500)))))))  ; Should be < 500ms

(test-end "neural-symbolic")

;;; test-neural-symbolic.scm ends here
