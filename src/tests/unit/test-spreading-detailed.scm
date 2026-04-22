;;; test-spreading-detailed.scm --- Detailed test for full network traversal
;;;
;;; Copyright (C) 2025 Deep Tree Echo Project

(add-to-load-path "/home/ubuntu/occ/src")
(use-modules (core memory atomspace))
(use-modules (core propagation spreading))
(use-modules (srfi srfi-69))

(format #t "~%=== Detailed Spreading Activation Test ===~%~%")

;; Create test atomspace
(define as (make-atomspace))

;; Create atoms: Cat -> Mammal -> Animal
(define cat (create-concept-node as "Cat"))
(define mammal (create-concept-node as "Mammal"))
(define animal (create-concept-node as "Animal"))

;; Create links
(define cat-mammal (create-inheritance-link as cat mammal 
                     #:tv (make-truth-value #:strength 0.9 #:confidence 0.8)))
(define mammal-animal (create-inheritance-link as mammal animal
                        #:tv (make-truth-value #:strength 1.0 #:confidence 0.9)))

(format #t "Network: Cat -> Mammal -> Animal~%~%")

;; Test with detailed output
(format #t "Propagating from Cat with decay=0.8, max-hops=5, threshold=0.01...~%")
(define params (make-propagation-params 
                 #:decay-rate 0.8
                 #:threshold 0.01
                 #:max-hops 5
                 #:bidirectional? #f
                 #:use-truth-values? #f))

(define act-map (propagate-from-atom as cat 1.0 params))

;; Print ALL activations (including below threshold)
(format #t "~%All activations in map:~%")
(hash-table-walk act-map
  (lambda (uuid activation)
    (let ((item (atomspace-get-atom as uuid)))
      (if item
          (let ((name (if (link? item)
                         (format #f "~a-link" (link-type item))
                         (atom-name item))))
            (format #t "  ~a (UUID ~a): ~a~%" name uuid activation))
          (format #t "  Unknown (UUID ~a): ~a~%" uuid activation)))))

;; Get activated atoms above threshold
(format #t "~%Activated atoms above threshold (0.01):~%")
(define activated (get-activated-atoms as act-map 0.01))
(for-each
  (lambda (pair)
    (let* ((item (car pair))
           (activation (cdr pair))
           (name (if (link? item)
                    (format #f "~a-link" (link-type item))
                    (atom-name item))))
      (format #t "  ~a: ~a~%" name activation)))
  activated)

;; Check specific atoms
(format #t "~%Checking specific atoms:~%")
(format #t "  Cat activation: ~a~%" (get-activation act-map cat))
(format #t "  Cat-Mammal link activation: ~a~%" (get-activation act-map cat-mammal))
(format #t "  Mammal activation: ~a~%" (get-activation act-map mammal))
(format #t "  Mammal-Animal link activation: ~a~%" (get-activation act-map mammal-animal))
(format #t "  Animal activation: ~a~%" (get-activation act-map animal))

;; Test with truth value weighting
(format #t "~%~%Testing with truth value weighting...~%")
(define params-tv (make-propagation-params 
                    #:decay-rate 0.8
                    #:threshold 0.01
                    #:max-hops 5
                    #:bidirectional? #f
                    #:use-truth-values? #t))

(define act-map-tv (propagate-from-atom as cat 1.0 params-tv))

(format #t "Checking with TV weighting:~%")
(format #t "  Cat activation: ~a~%" (get-activation act-map-tv cat))
(format #t "  Cat-Mammal link activation: ~a~%" (get-activation act-map-tv cat-mammal))
(format #t "  Mammal activation: ~a~%" (get-activation act-map-tv mammal))
(format #t "  Mammal-Animal link activation: ~a~%" (get-activation act-map-tv mammal-animal))
(format #t "  Animal activation: ~a~%" (get-activation act-map-tv animal))

(format #t "~%Expected behavior:~%")
(format #t "  Cat: 1.0 (source)~%")
(format #t "  Cat-Mammal link: 0.8 (1.0 * 0.8 decay)~%")
(format #t "  Mammal: 0.8 (propagated from link)~%")
(format #t "  Mammal-Animal link: 0.64 (0.8 * 0.8 decay)~%")
(format #t "  Animal: 0.64 (propagated from link)~%")

(format #t "~%=== Test Complete ===~%")

;;; test-spreading-detailed.scm ends here
