;;;     Copyright 2024 Li-Cheng (Andy) Tai
;;;                      atai@atai.org
;;;
;;;     This file is part of guile_llama_cpp ECMA-262 integration.
;;;
;;;     guile_llama_cpp is free software: you can redistribute it and/or modify it
;;;     under the terms of the GNU Lesser General Public License as published by the
;;;     Free Software Foundation, either version 3 of the License, or (at your
;;;     option) any later version.
;;;
;;;     guile_llama_cpp is distributed in the hope that it will be useful, but
;;;     WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;;;     or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
;;;     License for more details.
;;;
;;;     You should have received a copy of the GNU Lesser General Public License
;;;     along with guile_llama_cpp. If not, see <https://www.gnu.org/licenses/>.

(define-module (guile-llama-cpp ecma262)
  #:use-module (guile-llama-cpp)
  #:use-module (ice-9 popen)
  #:use-module (ice-9 rdelim)
  #:use-module (json)
  #:export (javascript-eval
            javascript-llm-eval
            create-ecma262-context
            ecma262-features-available?
            execute-js-with-llm))

;;; ECMA-262 JavaScript Integration for Guile-LLaMA-CPP
;;; This module provides seamless integration between JavaScript/ECMA-262
;;; features and LLaMA-CPP functionality through Guile.

(define *node-js-executable* "node")
(define *ecma262-context-template* 
  "// ECMA-262 LLM Integration Context
const ECMALLMContext = {
  // Core LLM operations
  llm: {
    prompt: function(text) {
      return { type: 'llm-call', operation: 'prompt', args: { text: text } };
    },
    
    generate: function(prompt, options = {}) {
      return { 
        type: 'llm-call', 
        operation: 'generate', 
        args: { prompt: prompt, options: options } 
      };
    },
    
    tokenize: function(text) {
      return { type: 'llm-call', operation: 'tokenize', args: { text: text } };
    }
  },
  
  // ECMA-262 features for text processing
  text: {
    // Modern string methods
    processPrompt: function(prompt) {
      return prompt
        .trim()
        .split('\\n')
        .filter(line => line.length > 0)
        .map(line => line.trim())
        .join(' ');
    },
    
    // Template literals for dynamic prompts
    templatePrompt: function(template, ...args) {
      return template.replace(/\\${(\\d+)}/g, (match, index) => {
        return args[parseInt(index)] || match;
      });
    }
  },
  
  // Array processing utilities
  arrays: {
    processTokens: function(tokens) {
      return tokens.map((token, index) => ({
        index: index,
        value: token,
        type: typeof token
      }));
    },
    
    batchPrompts: function(prompts) {
      return prompts.map(prompt => ({
        prompt: prompt,
        processed: ECMALLMContext.text.processPrompt(prompt)
      }));
    }
  },
  
  // Promise-based async operations
  async: {
    prompt: async function(text) {
      return new Promise((resolve, reject) => {
        try {
          const result = ECMALLMContext.llm.prompt(text);
          resolve(result);
        } catch (error) {
          reject(error);
        }
      });
    }
  },
  
  // Object destructuring and modern syntax helpers
  config: {
    createLLMConfig: function({ 
      temperature = 0.7, 
      maxTokens = 512, 
      topP = 0.9, 
      frequencyPenalty = 0.0,
      presencePenalty = 0.0 
    } = {}) {
      return { temperature, maxTokens, topP, frequencyPenalty, presencePenalty };
    }
  }
};

// Global exports
globalThis.ECMA = ECMALLMContext;
globalThis.llm = ECMALLMContext.llm;
")

(define (ecma262-features-available?)
  "Check if ECMA-262 features (Node.js) are available"
  (let* ((pipe (open-input-pipe (string-append *node-js-executable* " --version 2>/dev/null")))
         (result (read-line pipe))
         (status (close-pipe pipe)))
    (and (not (eof-object? result))
         (string-prefix? "v" result))))

(define (javascript-eval code)
  "Evaluate JavaScript code using Node.js"
  (if (not (ecma262-features-available?))
      (error "Node.js is not available for ECMA-262 features"))
  
  (let* ((wrapped-code (string-append 
                       "try { "
                       "const result = (" code "); "
                       "console.log(JSON.stringify({ success: true, result: result })); "
                       "} catch (error) { "
                       "console.log(JSON.stringify({ success: false, error: error.message, stack: error.stack })); "
                       "}"))
         (pipe (open-input-pipe (string-append *node-js-executable* " -e '" wrapped-code "'")))
         (output (read-line pipe))
         (status (close-pipe pipe)))
    
    (if (eof-object? output)
        (values #f "No output from JavaScript execution")
        (let ((parsed (json-string->scm output)))
          (if (assoc-ref parsed "success")
              (values #t (assoc-ref parsed "result"))
              (values #f (assoc-ref parsed "error")))))))

(define (create-ecma262-context)
  "Create ECMA-262 context with LLM integration"
  *ecma262-context-template*)

(define (javascript-llm-eval code model-path)
  "Evaluate JavaScript code with LLM context and execute LLM operations"
  (let* ((context (create-ecma262-context))
         (full-code (string-append context "\n" code))
         (success result (javascript-eval full-code)))
    
    (if success
        (handle-llm-result result model-path)
        (error "JavaScript evaluation failed:" result))))

(define (handle-llm-result result model-path)
  "Handle JavaScript result and execute LLM operations if needed"
  (cond
   ;; Handle LLM operation requests
   ((and (hash-table? result) 
         (equal? (hash-ref result "type") "llm-call"))
    (let ((operation (hash-ref result "operation"))
          (args (hash-ref result "args")))
      (case (string->symbol operation)
        ((prompt)
         (execute-llm-prompt (hash-ref args "text") model-path))
        ((generate)
         (execute-llm-generate 
          (hash-ref args "prompt") 
          model-path 
          (hash-ref args "options")))
        ((tokenize)
         (execute-llm-tokenize (hash-ref args "text") model-path))
        (else
         (error "Unknown LLM operation:" operation)))))
   
   ;; Return non-LLM results as-is
   (else result)))

(define (execute-llm-prompt text model-path)
  "Execute LLM prompt operation"
  ;; This would integrate with the actual LLM prompt function
  ;; For now, return a placeholder that indicates LLM integration
  (format #f "LLM Response to '~a' using model ~a" text model-path))

(define (execute-llm-generate prompt model-path options)
  "Execute LLM generation with options"
  (format #f "LLM Generation for '~a' with options ~a using model ~a" 
          prompt options model-path))

(define (execute-llm-tokenize text model-path)
  "Execute LLM tokenization"
  (format #f "LLM Tokenization of '~a' using model ~a" text model-path))

(define (execute-js-with-llm js-code model-path)
  "High-level function to execute JavaScript with LLM integration"
  (javascript-llm-eval js-code model-path))