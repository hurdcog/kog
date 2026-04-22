# Aphrodite Engine OpenCog Integration

This package integrates Aphrodite Engine high-performance LLM inference server with OpenCog for powerful reasoning over AtomSpace knowledge graphs.

## Features

- **High-Performance LLM Inference**: Optimized large language model serving with vLLM backend
- **AtomSpace Integration**: Convert knowledge graphs to LLM prompts and back to structured knowledge
- **Batch Processing**: Parallel inference over multiple AtomSpace queries
- **OpenAI Compatibility**: Standard API interface for seamless integration
- **Cognitive Reasoning**: LLM-powered reasoning over structured cognitive knowledge

## Architecture

```
Large Models ←→ Aphrodite Engine ←→ OpenCog Adapters ←→ AtomSpace
     ↓              ↓                     ↓                ↓
   GPU Memory    vLLM Backend      Cognitive Bridge   Knowledge Graph
```

## API Overview

### Scheme Functions

- `(llm-inference-over-atoms atom-set system-prompt max-tokens)` - Reason over AtomSpace knowledge
- `(atomspace-to-prompt atoms system-prompt)` - Convert knowledge to LLM format
- `(generate-cognitive-response context-atoms query temperature)` - Generate responses with cognitive context
- `(parallel-inference-batch atom-queries system-prompt)` - Batch processing for multiple queries

### Python Integration

```python
from opencog_aphrodite import OpenCogAphroditeClient, SyncOpenCogAphrodite

# Async usage
async with OpenCogAphroditeClient() as client:
    atoms = [
        {"type": "ConceptNode", "name": "Alice"},
        {"type": "EvaluationLink", "predicate": "friend", "arguments": ["Alice", "Bob"]}
    ]
    
    result = await client.atomspace_inference(
        atoms=atoms,
        system_prompt="You are reasoning over social relationships.",
        query="What can you infer about Alice's social connections?"
    )

# Sync usage
client = SyncOpenCogAphrodite()
result = client.inference(atoms, system_prompt, query)
```

### Usage Example

```scheme
(use-modules (opencog aphrodite))

;; Create knowledge context
(define knowledge-atoms
  (list (ConceptNode "Alice")
        (ConceptNode "Bob") 
        (EvaluationLink
          (PredicateNode "friend")
          (ListLink (ConceptNode "Alice") (ConceptNode "Bob")))))

;; Generate cognitive response
(define response
  (generate-cognitive-response 
    knowledge-atoms
    "What can you tell me about the relationship between Alice and Bob?"
    0.7))

;; Batch inference for multiple queries
(define query-sets
  (list knowledge-atoms
        (list (ConceptNode "Weather") (ConceptNode "Sunny"))))

(define batch-results
  (parallel-inference-batch query-sets "Analyze the given knowledge."))
```

## Integration with OpenCog

This package enables cognitive agents to:
- Perform natural language reasoning over structured knowledge
- Convert between symbolic and neural representations
- Scale cognitive reasoning with powerful language models
- Integrate emergent LLM capabilities with logical inference

## Supported Models

- **Large Language Models**: GPT-style models, LLaMA, Mistral, CodeLlama
- **Reasoning Models**: Specialized models for logical reasoning and QA
- **Multimodal Models**: Vision-language models for rich cognitive integration
- **Custom Models**: Support for domain-specific fine-tuned models

## Performance Features

- **GPU Acceleration**: CUDA and ROCm support for high-throughput inference
- **Dynamic Batching**: Efficient request batching for optimal GPU utilization  
- **Memory Optimization**: PagedAttention for handling large context windows
- **Parallel Processing**: Concurrent inference over multiple AtomSpace queries

## Service Management

```bash
# Start Aphrodite OpenCog service
aphrodite-opencog /path/to/model --port 2242 --gpu-memory-utilization 0.8

# Check server health
curl http://localhost:2242/health

# Monitor inference metrics
curl http://localhost:2242/metrics

# Test cognitive reasoning
guile -c "(use-modules (opencog aphrodite)) (display (aphrodite-server-status))"
```

## Configuration

### Environment Variables

- `OPENCOG_APHRODITE_HOST` - Server host (default: 0.0.0.0)
- `OPENCOG_APHRODITE_PORT` - Server port (default: 2242)  
- `OPENCOG_APHRODITE_MODEL_PATH` - Model directory path

### Resource Requirements

- **GPU Memory**: 8GB+ for 7B models, 24GB+ for 13B models, 80GB+ for 70B models
- **System RAM**: 16GB+ recommended
- **Storage**: Fast SSD for model loading and caching

## OpenAI API Compatibility

Fully compatible with OpenAI API format for easy integration:

```python
import openai

# Configure client for Aphrodite
client = openai.OpenAI(
    base_url="http://localhost:2242/v1",
    api_key="opencog-key"
)

# Use standard OpenAI interface
response = client.chat.completions.create(
    model="opencog-reasoning",
    messages=[
        {"role": "system", "content": "You are reasoning over AtomSpace knowledge."},
        {"role": "user", "content": "Analyze the given cognitive context..."}
    ]
)
```