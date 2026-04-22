#!/usr/bin/env node

/**
 * ECMA-262 LLM Integration Example
 * 
 * This example demonstrates how ECMA-262 JavaScript features can be integrated
 * with guile-llama-cpp to provide modern JavaScript language features for
 * LLM interaction.
 * 
 * Copyright 2024 Li-Cheng (Andy) Tai
 * Licensed under GNU LGPL v3+
 */

// ECMA-262 LLM Integration Context (mirrors what's in the Scheme module)
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
        .split('\n')
        .filter(line => line.length > 0)
        .map(line => line.trim())
        .join(' ');
    },
    
    // Template literals for dynamic prompts
    templatePrompt: function(template, ...args) {
      return template.replace(/\${(\d+)}/g, (match, index) => {
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

// Global exports (as done in the Scheme module)
global.ECMA = ECMALLMContext;
global.llm = ECMALLMContext.llm;

console.log("=== ECMA-262 + LLM Integration Examples ===\n");

// Example 1: Basic LLM call with modern JavaScript
console.log("1. Basic LLM call:");
const basicCall = llm.prompt("What is artificial intelligence?");
console.log("JavaScript:", "llm.prompt('What is artificial intelligence?')");
console.log("Result:", JSON.stringify(basicCall, null, 2));

// Example 2: Template literals for dynamic prompts
console.log("\n2. Template literals:");
const topic = "machine learning";
const style = "beginner-friendly";
const templateResult = llm.prompt(`Explain ${topic} in a ${style} way`);
console.log("JavaScript:", `llm.prompt(\`Explain \${topic} in a \${style} way\`)`);
console.log("Result:", JSON.stringify(templateResult, null, 2));

// Example 3: Arrow functions and array processing
console.log("\n3. Array processing with arrow functions:");
const prompts = ["What is AI?", "Define ML", "Explain neural networks"];
const batchResult = ECMA.arrays.batchPrompts(prompts);
console.log("JavaScript:", "ECMA.arrays.batchPrompts(prompts).map(p => p.processed)");
console.log("Result:", batchResult.map(p => p.processed));

// Example 4: Object destructuring for configuration
console.log("\n4. Object destructuring for config:");
const config = ECMA.config.createLLMConfig({ 
  temperature: 0.8, 
  maxTokens: 100 
});
const configCall = llm.generate("Tell me a joke", config);
console.log("JavaScript:", "const config = ECMA.config.createLLMConfig({ temperature: 0.8, maxTokens: 100 })");
console.log("Config:", JSON.stringify(config, null, 2));
console.log("Result:", JSON.stringify(configCall, null, 2));

// Example 5: Modern async/await pattern
console.log("\n5. Async/await pattern:");
(async () => {
  try {
    const asyncResult = await ECMA.async.prompt("What is the future of AI?");
    console.log("JavaScript:", "await ECMA.async.prompt('What is the future of AI?')");
    console.log("Result:", JSON.stringify(asyncResult, null, 2));
  } catch (error) {
    console.error("Async error:", error);
  }
})();

// Example 6: Complex prompt processing
console.log("\n6. Complex prompt processing:");
const rawPrompt = `
  What is artificial intelligence?
  
  Please explain:
  - The basic concepts
  - Current applications
  - Future potential
`;

const processedPrompt = ECMA.text.processPrompt(rawPrompt);
const complexResult = llm.prompt(processedPrompt);
console.log("Original prompt had newlines and whitespace");
console.log("Processed:", processedPrompt);
console.log("Result:", JSON.stringify(complexResult, null, 2));

// Example 7: Token processing
console.log("\n7. Token processing:");
const sampleTokens = [1, 15, 287, 13, 1421];
const processedTokens = ECMA.arrays.processTokens(sampleTokens);
console.log("JavaScript:", "ECMA.arrays.processTokens([1, 15, 287, 13, 1421])");
console.log("Result:", processedTokens);

// Example 8: Modern syntax combination
console.log("\n8. Combined modern syntax:");
const advancedExample = (config = {}) => {
  const { model = 'default', ...options } = config;
  const prompts = [
    "Explain quantum computing",
    "What are neural networks?",
    "Define machine learning"
  ];
  
  return prompts
    .map(prompt => ECMA.text.processPrompt(prompt))
    .map(processed => llm.generate(processed, options))
    .filter(result => result.type === 'llm-call');
};

const advancedResult = advancedExample({ temperature: 0.9, maxTokens: 200 });
console.log("JavaScript:", "Complex function with destructuring, arrow functions, map/filter");
console.log("Result count:", advancedResult.length);
console.log("First result:", JSON.stringify(advancedResult[0], null, 2));

console.log("\n=== Integration Complete ===");
console.log("All ECMA-262 features are working with LLM integration!");