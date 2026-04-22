# ECMA-262 JavaScript Integration for Guile-LLaMA-CPP

This document describes the integration of ECMA-262 JavaScript features with guile-llama-cpp, providing modern JavaScript language support for LLM interaction.

## Overview

The ECMA-262 integration allows developers to use modern JavaScript syntax and features when working with Large Language Models (LLMs) through guile-llama-cpp. This integration bridges the gap between JavaScript's powerful language features and Guile's Scheme-based LLM functionality.

## Features

### Core ECMA-262 Features Supported

- **ES2015+ Syntax**: Arrow functions, template literals, destructuring assignment
- **Modern String Methods**: `trim()`, `split()`, `filter()`, `map()`, `join()`
- **Array Processing**: `map()`, `filter()`, `reduce()` for token and prompt processing
- **Promise/Async Support**: `async`/`await` patterns for asynchronous LLM operations
- **Object Destructuring**: Modern parameter handling for LLM configuration
- **Template Literals**: Dynamic prompt generation with embedded expressions

### LLM Integration Features

- **JavaScript-to-LLM Bridge**: Execute LLM operations from JavaScript code
- **Prompt Processing**: ECMA-262 text processing capabilities for prompt refinement
- **Token Handling**: Modern array methods for token manipulation
- **Configuration Objects**: JavaScript object patterns for LLM parameters
- **Batch Operations**: Process multiple prompts using JavaScript array methods

## Architecture

```
JavaScript (ECMA-262) → Node.js → Guile Scheme → LLaMA.cpp
                                       ↑
                                JSON Communication
```

The integration works by:
1. JavaScript code is executed in Node.js environment
2. LLM operation requests are serialized as JSON objects
3. Guile Scheme processes the requests and calls LLaMA.cpp
4. Results are returned back through the JSON bridge

## API Reference

### Core Module: `(guile-llama-cpp ecma262)`

#### Functions

##### `(ecma262-features-available?)`
Check if ECMA-262 features (Node.js) are available.

**Returns:** `#t` if Node.js is available, `#f` otherwise

##### `(javascript-eval code)`
Evaluate JavaScript code and return the result.

**Parameters:**
- `code` (string): JavaScript code to evaluate

**Returns:** Multiple values: `success` (boolean) and `result` (any)

##### `(create-ecma262-context)`
Create ECMA-262 context with LLM integration capabilities.

**Returns:** String containing JavaScript context code

##### `(javascript-llm-eval code model-path)`
Evaluate JavaScript code with LLM context and execute LLM operations.

**Parameters:**
- `code` (string): JavaScript code to evaluate
- `model-path` (string): Path to the LLM model file

**Returns:** Result of JavaScript execution, with LLM operations executed

##### `(execute-js-with-llm js-code model-path)`
High-level function to execute JavaScript with LLM integration.

**Parameters:**
- `js-code` (string): JavaScript code to execute
- `model-path` (string): Path to the LLM model file

**Returns:** Execution result

### JavaScript API

When using the ECMA-262 integration, the following JavaScript objects are available:

#### `ECMA.llm`
Core LLM operations.

```javascript
// Basic prompt
ECMA.llm.prompt("What is AI?")

// Generation with options
ECMA.llm.generate("Tell me a story", { temperature: 0.8 })

// Tokenization
ECMA.llm.tokenize("Hello world")
```

#### `ECMA.text`
Text processing utilities using ECMA-262 features.

```javascript
// Process multi-line prompts
ECMA.text.processPrompt(`
  What is machine learning?
  Please be concise.
`)

// Template-based prompts
ECMA.text.templatePrompt("Explain ${0} in ${1} terms", "AI", "simple")
```

#### `ECMA.arrays`
Array processing for tokens and prompts.

```javascript
// Process tokens
ECMA.arrays.processTokens([1, 2, 3, 4, 5])

// Batch prompt processing
ECMA.arrays.batchPrompts(["What is AI?", "Define ML", "Explain DL"])
```

#### `ECMA.config`
Configuration object creation with destructuring support.

```javascript
// Create LLM configuration
const config = ECMA.config.createLLMConfig({
  temperature: 0.7,
  maxTokens: 512,
  topP: 0.9
});
```

#### `ECMA.async`
Promise-based async operations.

```javascript
// Async prompt
const result = await ECMA.async.prompt("What is the future of AI?");
```

## Usage Examples

### Basic JavaScript Execution

```scheme
(use-modules (guile-llama-cpp ecma262))

;; Simple JavaScript evaluation
(let-values (((success result) (javascript-eval "2 + 2")))
  (if success
      (format #t "Result: ~a~%" result)
      (format #t "Error: ~a~%" result)))
```

### LLM Integration

```scheme
;; Execute JavaScript with LLM context
(define model-path "/path/to/model.gguf")
(define js-code "ECMA.llm.prompt('What is artificial intelligence?')")

(let ((result (javascript-llm-eval js-code model-path)))
  (format #t "LLM Response: ~a~%" result))
```

### Modern JavaScript Features

```scheme
;; Template literals and arrow functions
(define advanced-js "
  const topics = ['AI', 'ML', 'DL'];
  const results = topics.map(topic => 
    ECMA.llm.prompt(`Explain ${topic} in simple terms`)
  );
  results
")

(execute-js-with-llm advanced-js model-path)
```

### Command Line Usage

The `ecma262-bridge.scm` script provides command-line access:

```bash
# Run demonstration
./ecma262-bridge.scm demo /path/to/model.gguf

# Execute JavaScript code
./ecma262-bridge.scm exec /path/to/model.gguf "ECMA.llm.prompt('Hello AI')"
```

## Error Handling

The integration provides comprehensive error handling:

```javascript
try {
  const result = ECMA.llm.prompt("What is AI?");
  console.log(result);
} catch (error) {
  console.error("LLM error:", error.message);
}
```

Error information includes:
- JavaScript syntax errors
- LLM execution errors
- Node.js runtime errors
- JSON serialization errors

## Performance Considerations

- **Node.js Overhead**: Each JavaScript evaluation spawns a Node.js process
- **JSON Serialization**: Large objects may have serialization overhead
- **Memory Usage**: JavaScript context is recreated for each execution
- **Optimization**: Consider batching operations for better performance

## Dependencies

- **Node.js**: v14+ required for ECMA-262 features
- **Guile**: 3.0+ with JSON support
- **LLaMA.cpp**: Compatible model for actual LLM operations

## Testing

Run the test suite:

```scheme
;; Load and run tests
(use-modules (guile-llama-cpp ecma262))
(load "src/test-ecma262.scm")
```

Test the JavaScript example:

```bash
node examples/ecma262-llm-example.js
```

## Integration with SKZ Framework

This ECMA-262 integration is part of the SKZ autonomous agents framework integration strategy, providing:

- Modern JavaScript syntax for agent development
- Seamless LLM integration for cognitive operations
- ECMA-262 compliance for web-based agent interfaces
- Cross-platform compatibility through Node.js

## Limitations

- Requires Node.js installation
- JavaScript execution is synchronous from Guile perspective
- Limited to text-based LLM interactions
- No direct DOM or browser API access

## Future Enhancements

- **WebAssembly Support**: Direct JavaScript engine embedding
- **Streaming Operations**: Real-time LLM streaming through JavaScript
- **Browser Integration**: Direct browser-based LLM interaction
- **TypeScript Support**: Type-safe LLM operations

## License

This integration is licensed under the GNU Lesser General Public License v3+, consistent with guile-llama-cpp licensing.

## Contributing

Contributions to the ECMA-262 integration are welcome. Please ensure:

- JavaScript code follows ES2015+ standards
- Scheme code follows Guile conventions
- Tests are provided for new features
- Documentation is updated accordingly