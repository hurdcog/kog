;;; inference.scm --- Symbolic Reasoning Engine for Cognitive Grammar Kernel
;;;
;;; Copyright (C) 2025 Deep Tree Echo Project
;;;
;;; This file implements the symbolic reasoning engine with inference rules,
;;; forward/backward chaining, and unification for logical reasoning.
;;;
;;; Commentary:
;;;
;;; The inference engine provides classical symbolic AI capabilities including:
;;; - Inference rules (deduction, induction, abduction, analogy)
;;; - Forward chaining (data-driven reasoning)
;;; - Backward chaining (goal-driven reasoning)
;;; - Unification (pattern matching with variables)
;;; - Logical operators (AND, OR, NOT, IMPLIES, FORALL, EXISTS)
;;;
;;; Code:

(define-module (core grammar inference)
  #:use-module (srfi srfi-1)   ; List library
  #:use-module (srfi srfi-9)   ; Record types
  #:use-module (srfi srfi-69)  ; Hash tables
  #:use-module (ice-9 match)   ; Pattern matching
  #:use-module (core memory atomspace)
  #:export (
    ;; Inference rules
    make-inference-rule
    inference-rule?
    rule-name
    rule-premises
    rule-conclusion
    rule-confidence
    
    ;; Standard rules
    modus-ponens
    modus-tollens
    syllogism
    generalization
    abduction
    analogy
    
    ;; Inference engine
    forward-chain
    backward-chain
    apply-rule
    create-link-from-pattern
    create-implication-link
    create-similarity-link
    
    ;; Unification
    unify
    instantiate-pattern
    match-premises
    variable?
    
    ;; Logical operators
    eval-logical-expr
    eval-truth
    eval-and
    eval-or
    eval-not
    eval-implies
    eval-forall
    eval-exists
    
    ;; Utilities
    make-variable
    get-bindings
    merge-bindings
  ))

;;;
;;; Inference Rule Data Structure
;;;

(define-record-type <inference-rule>
  (make-inference-rule name premises conclusion confidence)
  inference-rule?
  (name rule-name)                    ; Symbol: 'modus-ponens
  (premises rule-premises)            ; List of patterns
  (conclusion rule-conclusion)        ; Pattern
  (confidence rule-confidence))       ; Float: 0.0-1.0

;;;
;;; Variables
;;;

(define (variable? x)
  "Check if x is a variable (symbol starting with ?)."
  (and (symbol? x)
       (let ((str (symbol->string x)))
         (and (> (string-length str) 0)
              (char=? (string-ref str 0) #\?)))))

(define (make-variable name)
  "Create a variable symbol."
  (string->symbol (string-append "?" (symbol->string name))))

;;;
;;; Standard Inference Rules
;;;

;; Modus Ponens: If P and (P → Q), then Q
(define modus-ponens
  (make-inference-rule
    'modus-ponens
    '((ImplicationLink ?P ?Q) ?P)
    '?Q
    1.0))

;; Modus Tollens: If (P → Q) and ¬Q, then ¬P
(define modus-tollens
  (make-inference-rule
    'modus-tollens
    '((ImplicationLink ?P ?Q) (NotLink ?Q))
    '(NotLink ?P)
    1.0))

;; Syllogism: If (P → Q) and (Q → R), then (P → R)
(define syllogism
  (make-inference-rule
    'syllogism
    '((ImplicationLink ?P ?Q) (ImplicationLink ?Q ?R))
    '(ImplicationLink ?P ?R)
    1.0))

;; Generalization: If P(a), P(b), P(c), then ∀x P(x)
(define generalization
  (make-inference-rule
    'generalization
    '((EvaluationLink ?P ?a) (EvaluationLink ?P ?b) (EvaluationLink ?P ?c))
    '(ForAllLink ?x (EvaluationLink ?P ?x))
    0.8))  ; Lower confidence (inductive)

;; Abduction: If Q and (P → Q), then maybe P
(define abduction
  (make-inference-rule
    'abduction
    '(?Q (ImplicationLink ?P ?Q))
    '?P
    0.6))  ; Even lower confidence (abductive)

;; Analogy: If similar(A, B) and P(A), then P(B)
(define analogy
  (make-inference-rule
    'analogy
    '((SimilarityLink ?A ?B) (EvaluationLink ?P ?A))
    '(EvaluationLink ?P ?B)
    0.7))

;;;
;;; Unification
;;;

(define (unify pattern1 pattern2 bindings)
  "Find variable bindings that make pattern1 and pattern2 identical.
   Returns updated bindings or #f if unification fails."
  (cond
    ;; Unification failed in previous step
    ((not bindings) #f)
    
    ;; Patterns are equal
    ((equal? pattern1 pattern2) bindings)
    
    ;; pattern1 is variable
    ((variable? pattern1)
     (unify-variable pattern1 pattern2 bindings))
    
    ;; pattern2 is variable
    ((variable? pattern2)
     (unify-variable pattern2 pattern1 bindings))
    
    ;; Both are atoms
    ((and (atom? pattern1) (atom? pattern2))
     (if (equal? (atom-uuid pattern1) (atom-uuid pattern2))
         bindings
         #f))
    
    ;; Both are lists (compound patterns)
    ((and (list? pattern1) (list? pattern2))
     (if (= (length pattern1) (length pattern2))
         (unify-lists pattern1 pattern2 bindings)
         #f))
    
    ;; Otherwise: unification failed
    (else #f)))

(define (unify-variable var value bindings)
  "Unify variable with value."
  (let ((binding (assoc var bindings)))
    (cond
      ;; Variable already bound
      (binding
       (unify (cdr binding) value bindings))
      
      ;; Value is variable and already bound
      ((and (variable? value) (assoc value bindings))
       (unify var (cdr (assoc value bindings)) bindings))
      
      ;; Occurs check: prevent infinite structures
      ((occurs-check? var value bindings)
       #f)
      
      ;; Add new binding
      (else
       (cons (cons var value) bindings)))))

(define (occurs-check? var value bindings)
  "Check if var occurs in value (prevents infinite structures)."
  (cond
    ((equal? var value) #t)
    ((variable? value)
     (let ((binding (assoc value bindings)))
       (and binding (occurs-check? var (cdr binding) bindings))))
    ((list? value)
     (any (lambda (item) (occurs-check? var item bindings)) value))
    (else #f)))

(define (unify-lists list1 list2 bindings)
  "Unify two lists element by element."
  (if (null? list1)
      bindings
      (let ((new-bindings (unify (car list1) (car list2) bindings)))
        (and new-bindings
             (unify-lists (cdr list1) (cdr list2) new-bindings)))))

(define (instantiate-pattern pattern bindings)
  "Replace variables in pattern with their bindings."
  (cond
    ;; Variable: look up binding
    ((variable? pattern)
     (let ((binding (assoc pattern bindings)))
       (if binding
           (instantiate-pattern (cdr binding) bindings)
           pattern)))
    
    ;; List: recursively instantiate elements
    ((list? pattern)
     (map (lambda (item) (instantiate-pattern item bindings)) pattern))
    
    ;; Otherwise: return as is
    (else pattern)))

;;;
;;; Pattern Matching
;;;

(define (match-premises atomspace premises)
  "Find all variable bindings that match premises in atomspace.
   Returns list of binding sets."
  (match-premises-helper atomspace premises '() '(())))

(define (match-premises-helper atomspace premises current-bindings all-bindings)
  "Helper for match-premises with accumulator."
  (if (null? premises)
      all-bindings
      (let ((premise (car premises))
            (rest-premises (cdr premises)))
        ;; Find matches for current premise
        (let ((matches (match-pattern atomspace premise)))
          ;; For each match, extend bindings and continue
          (let ((new-all-bindings
                 (append-map
                   (lambda (old-bindings)
                     (filter-map
                       (lambda (match-bindings)
                         (let ((merged (merge-bindings old-bindings match-bindings)))
                           (and merged merged)))
                       matches))
                   all-bindings)))
            (if (null? new-all-bindings)
                '()
                (match-premises-helper atomspace rest-premises 
                                      current-bindings new-all-bindings)))))))

(define (match-pattern atomspace pattern)
  "Find all atoms/links that match pattern.
   Returns list of binding sets."
  (cond
    ;; Variable: matches any atom
    ((variable? pattern)
     (map (lambda (atom) (list (cons pattern atom)))
          (atomspace-get-all-atoms atomspace)))
    
    ;; Atom: matches itself
    ((atom? pattern)
     (if (atomspace-get-atom atomspace (atom-uuid pattern))
         '(())  ; Empty bindings (already matched)
         '()))  ; No match
    
    ;; List pattern: match link structure
    ((list? pattern)
     (match-link-pattern atomspace pattern))
    
    ;; Otherwise: no match
    (else '())))

(define (match-link-pattern atomspace pattern)
  "Match a link pattern like (ImplicationLink ?P ?Q)."
  (if (< (length pattern) 2)
      '()
      (let ((link-type (car pattern))
            (outgoing-patterns (cdr pattern)))
        ;; Get all links of this type
        (let ((links (atomspace-get-atoms-by-type atomspace link-type)))
          ;; Try to match each link
          (filter-map
            (lambda (link)
              ;; Try to unify outgoing set with patterns
              (let ((outgoing (if (link? link)
                                 (link-outgoing link)
                                 '())))
                (if (= (length outgoing) (length outgoing-patterns))
                    (unify-lists outgoing-patterns outgoing '())
                    #f)))
            links)))))

;;;
;;; Binding Utilities
;;;

(define (get-bindings var bindings)
  "Get value bound to variable."
  (let ((binding (assoc var bindings)))
    (if binding
        (cdr binding)
        #f)))

(define (merge-bindings bindings1 bindings2)
  "Merge two binding sets. Returns #f if conflict."
  (cond
    ((null? bindings2) bindings1)
    (else
     (let ((binding (car bindings2)))
       (let ((var (car binding))
             (val (cdr binding)))
         (let ((existing (assoc var bindings1)))
           (cond
             ;; Variable not bound: add binding
             ((not existing)
              (merge-bindings (cons binding bindings1) (cdr bindings2)))
             
             ;; Variable bound to same value: continue
             ((equal? (cdr existing) val)
              (merge-bindings bindings1 (cdr bindings2)))
             
             ;; Conflict: fail
             (else #f))))))))

;;;
;;; Link Creation from Patterns
;;;

(define (create-link-from-pattern atomspace pattern)
  "Create a link in atomspace from a pattern like (InheritanceLink atom1 atom2).
   Returns the created link or #f if pattern is invalid."
  (if (and (list? pattern) (>= (length pattern) 2))
      (let ((link-type (car pattern))
            (outgoing (cdr pattern)))
        ;; Verify all outgoing elements are atoms
        (if (every atom? outgoing)
            (case link-type
              ((InheritanceLink)
               (if (= (length outgoing) 2)
                   (create-inheritance-link atomspace (car outgoing) (cadr outgoing))
                   #f))
              ((EvaluationLink)
               (if (>= (length outgoing) 1)
                   (create-evaluation-link atomspace (car outgoing) (cdr outgoing))
                   #f))
              ((ImplicationLink)
               (if (= (length outgoing) 2)
                   (create-implication-link atomspace (car outgoing) (cadr outgoing))
                   #f))
              ((SimilarityLink)
               (if (= (length outgoing) 2)
                   (create-similarity-link atomspace (car outgoing) (cadr outgoing))
                   #f))
              ((ListLink)
               (create-list-link atomspace outgoing))
              (else
               ;; Generic link creation for unknown types
               (let ((link (make-link link-type outgoing)))
                 (atomspace-add-atom! atomspace (link-atom link))
                 link)))
            #f))
      #f))

(define (create-implication-link atomspace premise conclusion)
  "Create an ImplicationLink."
  (let ((link (make-link 'ImplicationLink (list premise conclusion))))
    (atomspace-add-atom! atomspace (link-atom link))
    link))

(define (create-similarity-link atomspace atom1 atom2)
  "Create a SimilarityLink."
  (let ((link (make-link 'SimilarityLink (list atom1 atom2))))
    (atomspace-add-atom! atomspace (link-atom link))
    link))

;;;
;;; Forward Chaining (Data-Driven Reasoning)
;;;

(define (forward-chain atomspace rules max-depth)
  "Apply inference rules to derive new knowledge.
   Returns list of newly derived atoms."
  (let ((new-atoms '())
        (depth 0))
    (let loop ((current-depth 0))
      (if (>= current-depth max-depth)
          new-atoms
          (let ((derived-this-round '()))
            ;; For each rule
            (for-each
              (lambda (rule)
                ;; Find matches for premises
                (let ((matches (match-premises atomspace (rule-premises rule))))
                  ;; Apply rule to each match
                  (for-each
                    (lambda (bindings)
                      (let ((conclusion (instantiate-pattern 
                                        (rule-conclusion rule) 
                                        bindings)))
                        ;; Handle both atom and link conclusions
                        (cond
                          ;; Atom conclusion: add if new
                          ((atom? conclusion)
                           (when (not (atomspace-get-atom atomspace (atom-uuid conclusion)))
                             (atomspace-add-atom! atomspace conclusion)
                             (set! new-atoms (cons conclusion new-atoms))
                             (set! derived-this-round (cons conclusion derived-this-round))))
                          
                          ;; Link pattern conclusion: create link
                          ((and (list? conclusion) (>= (length conclusion) 2))
                           (let ((new-link (create-link-from-pattern atomspace conclusion)))
                             (when new-link
                               ;; Check if link is new
                               (let ((link-atom (link-atom new-link)))
                                 (when (not (member link-atom new-atoms))
                                   (set! new-atoms (cons link-atom new-atoms))
                                   (set! derived-this-round (cons link-atom derived-this-round)))))))
                          
                          ;; Otherwise: skip
                          (else #f))))
                    matches)))
              rules)
            ;; Continue if we derived new atoms
            (if (null? derived-this-round)
                new-atoms
                (loop (+ current-depth 1))))))))

;;;
;;; Backward Chaining (Goal-Driven Reasoning)
;;;

(define (backward-chain atomspace rules goal max-depth)
  "Prove goal by working backward from conclusion to premises.
   Returns proof tree or #f if goal cannot be proved."
  (backward-chain-helper atomspace rules goal max-depth '()))

(define (backward-chain-helper atomspace rules goal depth visited)
  "Helper for backward chaining with visited tracking."
  (cond
    ;; Base case: goal already in atomspace
    ((and (atom? goal) (atomspace-get-atom atomspace (atom-uuid goal)))
     (list 'fact goal))
    
    ;; Base case: max depth reached
    ((>= depth 0)
     #f)
    
    ;; Base case: goal already visited (prevent cycles)
    ((member goal visited)
     #f)
    
    ;; Recursive case: try to prove goal using rules
    (else
     (let ((new-visited (cons goal visited)))
       (let try-rules ((remaining-rules rules))
         (if (null? remaining-rules)
             #f
             (let ((rule (car remaining-rules)))
               ;; Try to unify goal with rule conclusion
               (let ((bindings (unify goal (rule-conclusion rule) '())))
                 (if bindings
                     ;; Try to prove all premises
                     (let ((instantiated-premises
                            (map (lambda (premise)
                                   (instantiate-pattern premise bindings))
                                 (rule-premises rule))))
                       (let ((premise-proofs
                              (map (lambda (premise)
                                     (backward-chain-helper atomspace rules premise
                                                          (- depth 1) new-visited))
                                   instantiated-premises)))
                         ;; If all premises proved, return proof tree
                         (if (every (lambda (p) p) premise-proofs)
                             (list 'rule (rule-name rule) premise-proofs goal)
                             ;; Otherwise, try next rule
                             (try-rules (cdr remaining-rules)))))
                     ;; Unification failed, try next rule
                     (try-rules (cdr remaining-rules)))))))))))

;;;
;;; Rule Application
;;;

(define (apply-rule atomspace rule bindings)
  "Apply inference rule with given bindings.
   Returns conclusion atom or #f if application fails."
  (let ((conclusion (instantiate-pattern (rule-conclusion rule) bindings)))
    (if (atom? conclusion)
        (begin
          (atomspace-add-atom! atomspace conclusion)
          conclusion)
        #f)))

;;;
;;; Logical Operators
;;;

(define (eval-logical-expr expr atomspace)
  "Evaluate logical expression."
  (cond
    ;; Atom: evaluate truth value
    ((atom? expr)
     (eval-truth expr))
    
    ;; List: evaluate operator
    ((list? expr)
     (let ((operator (car expr))
           (operands (cdr expr)))
       (case operator
         ((AND) (eval-and operands atomspace))
         ((OR) (eval-or operands atomspace))
         ((NOT) (eval-not (car operands) atomspace))
         ((IMPLIES) (eval-implies (car operands) (cadr operands) atomspace))
         ((FORALL) (eval-forall (car operands) (cadr operands) atomspace))
         ((EXISTS) (eval-exists (car operands) (cadr operands) atomspace))
         (else #f))))
    
    ;; Otherwise: false
    (else #f)))

(define (eval-truth atom)
  "Evaluate truth of atom."
  (> (tv-strength (atom-tv atom)) 0.5))

(define (eval-and operands atomspace)
  "Evaluate AND of operands."
  (every (lambda (op) (eval-logical-expr op atomspace)) operands))

(define (eval-or operands atomspace)
  "Evaluate OR of operands."
  (any (lambda (op) (eval-logical-expr op atomspace)) operands))

(define (eval-not operand atomspace)
  "Evaluate NOT of operand."
  (not (eval-logical-expr operand atomspace)))

(define (eval-implies premise conclusion atomspace)
  "Evaluate IMPLIES (P → Q)."
  (or (eval-not premise atomspace) 
      (eval-logical-expr conclusion atomspace)))

(define (eval-forall variable predicate atomspace)
  "Evaluate FORALL: ∀x P(x)."
  (let ((all-atoms (atomspace-get-all-atoms atomspace)))
    (every (lambda (atom)
             (let ((bindings (list (cons variable atom))))
               (let ((instantiated (instantiate-pattern predicate bindings)))
                 (eval-logical-expr instantiated atomspace))))
           all-atoms)))

(define (eval-exists variable predicate atomspace)
  "Evaluate EXISTS: ∃x P(x)."
  (let ((all-atoms (atomspace-get-all-atoms atomspace)))
    (any (lambda (atom)
           (let ((bindings (list (cons variable atom))))
             (let ((instantiated (instantiate-pattern predicate bindings)))
               (eval-logical-expr instantiated atomspace))))
         all-atoms)))

;;; inference.scm ends here
