# KoboldCpp OpenCog Integration

This package integrates KoboldCpp GGUF inference engine with OpenCog for story generation, world modeling, and narrative reasoning.

## Features

- **Story Generation**: Generate narratives using local GGUF models integrated with AtomSpace
- **World Modeling**: Create cognitive representations of story worlds and characters
- **Roleplay Integration**: Support for character-based interactions and scenario simulation
- **Fast Inference**: Local GGUF model execution with cognitive context

## Architecture

```
GGUF Models ←→ KoboldCpp Engine ←→ OpenCog Adapters ←→ AtomSpace
     ↓              ↓                    ↓                ↓
  Local Files   Inference API     Story Context    Knowledge Graph
```

## API Overview

### Scheme Functions

- `(generate-story-continuation context-atoms max-tokens)` - Generate story using AtomSpace context
- `(create-world-model story-elements)` - Build cognitive world representation
- `(atoms->story-context atoms)` - Convert AtomSpace to story text
- `(gguf-inference-request prompt-atoms temperature top-p)` - Direct GGUF inference

### Python Integration

```python
from opencog_koboldcpp import OpenCogKoboldBridge

# Initialize bridge
bridge = OpenCogKoboldBridge("http://localhost:5001")

# Generate from AtomSpace context
context = ["Character: Alice", "Setting: Forest", "Mood: Mystery"]
result = bridge.generate_from_atomspace(context, max_tokens=256)

# Create character model
character_atoms = ["Alice", "Curious", "Brave", "Young"]
character = bridge.create_character_model(character_atoms)
```

### Usage Example

```scheme
(use-modules (opencog koboldcpp))

;; Create story context from AtomSpace
(define story-context 
  (list (ConceptNode "Character:Alice")
        (ConceptNode "Setting:MysteriousForest")
        (ConceptNode "Quest:FindAncientTreasure")))

;; Generate continuation
(define story-continuation
  (generate-story-continuation story-context 512))

;; Create world model
(define world-elements '("Forest" "Cave" "River" "Village"))
(define world-model (create-world-model world-elements))
```

## Integration with OpenCog

This package enables cognitive agents to:
- Generate coherent narratives from structured knowledge
- Model story worlds as cognitive representations
- Perform character-based reasoning and roleplay
- Use local GGUF models for private, secure story generation

## Supported Models

- LLaMA models (various sizes)
- Mistral and Mixtral models  
- Code Llama for programming narratives
- Custom fine-tuned GGUF models

## Service Management

```bash
# Start KoboldCpp OpenCog service
koboldcpp-opencog --model /path/to/model.gguf --port 5001

# Check service status
curl http://localhost:5001/api/v1/info

# Generate with cognitive context
guile -c "(use-modules (opencog koboldcpp)) (display (generate-story-continuation context-atoms 256))"
```

## Performance

- **Local Inference**: No external API dependencies
- **GGUF Optimization**: Efficient quantized model execution
- **Batch Processing**: Support for multiple simultaneous requests
- **Memory Efficient**: Optimized for resource-constrained environments