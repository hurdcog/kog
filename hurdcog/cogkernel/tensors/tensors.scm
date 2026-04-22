;;; Tensors - Tensor Operations for Cognitive Kernel
;;; Implements tensor-shaped membranes and ggml-style operations
;;; Supports the P-System membrane computing paradigm

(define-module (cogkernel tensors)
  #:use-module (ice-9 match)
  #:use-module (srfi srfi-1)
  #:use-module (srfi srfi-9)
  #:export (make-tensor
            tensor?
            tensor-shape
            tensor-data
            tensor-create
            tensor-zeros
            tensor-ones
            tensor-random
            tensor-add
            tensor-multiply
            tensor-dot
            tensor-reshape
            tensor-normalize
            p-system-membrane
            membrane-evolve!))

;;; Tensor record structure
(define-record-type <tensor>
  (make-tensor-record shape data metadata)
  tensor?
  (shape tensor-shape)
  (data tensor-data set-tensor-data!)
  (metadata tensor-metadata set-tensor-metadata!))

;;; P-System membrane record
(define-record-type <membrane>
  (make-membrane-record rules objects evolution-rules)
  membrane?
  (rules membrane-rules set-membrane-rules!)
  (objects membrane-objects set-membrane-objects!)
  (evolution-rules membrane-evolution-rules))

;;; Create tensor with specified shape and data
(define (make-tensor shape data)
  "Create a tensor with given shape and data"
  (let ((expected-size (fold * 1 shape))
        (actual-size (length data)))
    (unless (= expected-size actual-size)
      (error "Data size doesn't match tensor shape" expected-size actual-size))
    (make-tensor-record shape data '())))

;;; Create tensor with specified shape
(define (tensor-create shape init-value)
  "Create a tensor with given shape initialized to init-value"
  (let ((size (fold * 1 shape)))
    (make-tensor shape (make-list size init-value))))

;;; Create zero tensor
(define (tensor-zeros shape)
  "Create a tensor filled with zeros"
  (tensor-create shape 0))

;;; Create ones tensor
(define (tensor-ones shape)
  "Create a tensor filled with ones"
  (tensor-create shape 1))

;;; Create random tensor
(define (tensor-random shape)
  "Create a tensor filled with random values between 0 and 1"
  (let ((size (fold * 1 shape)))
    (make-tensor shape (map (lambda (_) (random 1.0)) (iota size)))))

;;; Add two tensors element-wise
(define (tensor-add tensor1 tensor2)
  "Add two tensors element-wise"
  (unless (equal? (tensor-shape tensor1) (tensor-shape tensor2))
    (error "Tensor shapes must match for addition"))
  (let ((result-data (map + (tensor-data tensor1) (tensor-data tensor2))))
    (make-tensor (tensor-shape tensor1) result-data)))

;;; Multiply two tensors element-wise
(define (tensor-multiply tensor1 tensor2)
  "Multiply two tensors element-wise"
  (unless (equal? (tensor-shape tensor1) (tensor-shape tensor2))
    (error "Tensor shapes must match for multiplication"))
  (let ((result-data (map * (tensor-data tensor1) (tensor-data tensor2))))
    (make-tensor (tensor-shape tensor1) result-data)))

;;; Dot product of two tensors (simplified)
(define (tensor-dot tensor1 tensor2)
  "Compute dot product of two tensors"
  (fold + 0 (map * (tensor-data tensor1) (tensor-data tensor2))))

;;; Reshape tensor
(define (tensor-reshape tensor new-shape)
  "Reshape tensor to new shape"
  (let ((old-size (fold * 1 (tensor-shape tensor)))
        (new-size (fold * 1 new-shape)))
    (unless (= old-size new-size)
      (error "New shape must have same total size"))
    (make-tensor new-shape (tensor-data tensor))))

;;; Normalize tensor values
(define (tensor-normalize tensor)
  "Normalize tensor values to range [0, 1]"
  (let* ((data (tensor-data tensor))
         (min-val (fold min +inf.0 data))
         (max-val (fold max -inf.0 data))
         (range (- max-val min-val)))
    (if (= range 0)
        tensor
        (let ((normalized-data (map (lambda (x) (/ (- x min-val) range)) data)))
          (make-tensor (tensor-shape tensor) normalized-data)))))

;;; Create P-System membrane
(define (p-system-membrane objects rules evolution-rules)
  "Create a P-System membrane with objects and rules"
  (make-membrane-record rules objects evolution-rules))

;;; Evolve membrane one step
(define (membrane-evolve! membrane)
  "Evolve the membrane one step according to its rules"
  (let ((current-objects (membrane-objects membrane))
        (rules (membrane-rules membrane)))
    ; Apply evolution rules (simplified)
    (for-each (lambda (rule)
                (when (procedure? rule)
                  (rule current-objects)))
              rules)
    membrane))