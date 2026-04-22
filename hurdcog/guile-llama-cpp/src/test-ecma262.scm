;;;     Copyright 2024 Li-Cheng (Andy) Tai
;;;
;;;     This file is part of guile_llama_cpp ECMA-262 integration tests.
;;;
;;;     guile_llama_cpp is free software: you can redistribute it and/or modify it
;;;     under the terms of the GNU Lesser General Public License as published by the
;;;     Free Software Foundation, either version 3 of the License, or (at your
;;;     option) any later version.

(use-modules (guile-llama-cpp ecma262)
             (srfi srfi-64)    ; testing framework
             (ice-9 format))

(test-begin "ecma262-integration")

;; Test 1: Check if ECMA-262 features are available
(test-assert "ecma262-features-available"
  (ecma262-features-available?))

;; Test 2: Basic JavaScript evaluation
(test-case "basic-javascript-eval"
  (let-values (((success result) (javascript-eval "2 + 2")))
    (test-assert "javascript-eval-success" success)
    (test-equal "javascript-eval-result" 4 result)))

;; Test 3: JavaScript string processing
(test-case "javascript-string-processing" 
  (let-values (((success result) 
                (javascript-eval "'hello world'.toUpperCase()")))
    (test-assert "string-processing-success" success)
    (test-equal "string-processing-result" "HELLO WORLD" result)))

;; Test 4: JavaScript array processing
(test-case "javascript-array-processing"
  (let-values (((success result) 
                (javascript-eval "[1,2,3].map(x => x * 2)")))
    (test-assert "array-processing-success" success)
    (test-equal "array-processing-result" '(2 4 6) result)))

;; Test 5: ECMA-262 context creation
(test-assert "ecma262-context-creation" 
  (string? (create-ecma262-context)))

;; Test 6: Modern JavaScript syntax (arrow functions, destructuring)
(test-case "modern-javascript-syntax"
  (let-values (((success result) 
                (javascript-eval "
                  const obj = { a: 1, b: 2 };
                  const { a, b } = obj;
                  const sum = (x, y) => x + y;
                  sum(a, b)
                ")))
    (test-assert "modern-syntax-success" success)
    (test-equal "modern-syntax-result" 3 result)))

;; Test 7: Template literals
(test-case "template-literals"
  (let-values (((success result) 
                (javascript-eval "
                  const name = 'ECMA-262';
                  const version = 'ES2023';
                  `${name} ${version}`
                ")))
    (test-assert "template-literals-success" success)
    (test-equal "template-literals-result" "ECMA-262 ES2023" result)))

;; Test 8: Promise syntax (basic)
(test-case "promise-syntax"
  (let-values (((success result) 
                (javascript-eval "
                  const p = new Promise(resolve => resolve('success'));
                  p.constructor.name
                ")))
    (test-assert "promise-syntax-success" success)
    (test-equal "promise-syntax-result" "Promise" result)))

;; Test 9: ECMA-262 Context LLM integration structure
(test-case "ecma262-llm-context"
  (let-values (((success result) 
                (javascript-eval (string-append
                  (create-ecma262-context)
                  "\nECMA.llm.prompt('test')"))))
    (test-assert "ecma262-llm-context-success" success)
    (test-assert "ecma262-llm-context-structure" 
                 (and (hash-table? result)
                      (equal? (hash-ref result "type") "llm-call")
                      (equal? (hash-ref result "operation") "prompt")))))

(test-end "ecma262-integration")

(format #t "~%=== ECMA-262 Integration Test Results ===~%")
(format #t "Node.js available: ~a~%" (ecma262-features-available?))
(format #t "ECMA-262 context size: ~a characters~%" 
        (string-length (create-ecma262-context)))
(format #t "Integration tests completed.~%")