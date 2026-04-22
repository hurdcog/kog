# OpenCog Collection - External Package Integration

This document describes the integration of external projects into the OpenCog Collection monorepo as modular Guix packages.

## Integrated Packages

### 1. Gnucash (Cognitive Accounting)
- **Purpose**: Double-entry accounting with cognitive attention and ECAN integration
- **Location**: `gnucash/`
- **Guix Package**: `.guix/modules/gnucash.scm`
- **Build Option**: `-DBUILD_GNUCASH=ON`
- **Service**: `gnucash-opencog` daemon on port 8080

**Key Features**:
- Convert financial transactions to AtomSpace representations
- Apply cognitive attention to significant balance changes
- ECAN integration for financial pattern recognition
- Scheme bindings for direct AtomSpace integration

### 2. KoboldCpp (Story/World Modeling)
- **Purpose**: GGUF inference for story generation and world modeling
- **Location**: `koboldcpp/`
- **Guix Package**: `.guix/modules/koboldcpp.scm`
- **Build Option**: `-DBUILD_KOBOLDCPP=ON`
- **Service**: `koboldcpp-opencog` daemon on port 5001

**Key Features**:
- Local GGUF model inference with cognitive context
- Convert AtomSpace knowledge to story prompts
- Character and world modeling capabilities
- Python bridge for advanced integration

### 3. Aphrodite Engine (LLM Inference)
- **Purpose**: High-performance LLM inference over AtomSpace knowledge
- **Location**: `aphrodite-engine/`
- **Guix Package**: `.guix/modules/aphrodite.scm`
- **Build Option**: `-DBUILD_APHRODITE=ON`
- **Service**: `aphrodite-opencog` daemon on port 2242

**Key Features**:
- Powerful LLM inference with vLLM backend
- OpenAI-compatible API for seamless integration
- Batch processing for multiple AtomSpace queries
- GPU acceleration support

## Integration Architecture

```
External Projects ←→ OpenCog Adapters ←→ AtomSpace ←→ Cognitive Agents
       ↓                    ↓                ↓             ↓
   Native APIs        Bridge Modules    Knowledge Graph  Reasoning
```

## Build System Integration

### CMake Options
```bash
# Enable all external packages
cmake .. -DBUILD_GNUCASH=ON -DBUILD_KOBOLDCPP=ON -DBUILD_APHRODITE=ON

# Enable specific packages
cmake .. -DBUILD_GNUCASH=ON -DBUILD_KOBOLDCPP=OFF -DBUILD_APHRODITE=ON

# Default (all disabled)
cmake ..
```

### Guix Environment
```bash
# Development environment with all dependencies
guix shell -m .guix/manifest.scm

# Build complete package
guix build -f guix.scm
```

## Service Management

### Shepherd Services
All external integrations are managed as Shepherd services defined in `services/opencog-external.scm`:

```bash
# Start all external services
sudo herd start opencog-external

# Start individual services
sudo herd start gnucash-opencog
sudo herd start koboldcpp-opencog  
sudo herd start aphrodite-opencog

# Check service status
sudo herd status opencog-external
```

### Service Endpoints
- **Gnucash**: `http://localhost:8080` - Cognitive accounting API
- **KoboldCpp**: `http://localhost:5001` - Story generation API
- **Aphrodite**: `http://localhost:2242` - LLM inference API (OpenAI compatible)

## Usage Examples

### Scheme Integration
```scheme
;; Load all external modules
(use-modules (opencog gnucash)
             (opencog koboldcpp)
             (opencog aphrodite))

;; Convert financial data to cognitive representation
(define savings-account 
  (gnucash-account->atom '((name . "Savings") (balance . 5000.00))))

;; Generate story from cognitive context
(define story-context (list (ConceptNode "Character:Alice")))
(define story (generate-story-continuation story-context 256))

;; Perform LLM inference over AtomSpace knowledge
(define knowledge-atoms (list (ConceptNode "Alice") (ConceptNode "Bob")))
(define response (generate-cognitive-response knowledge-atoms 
                                            "What is the relationship between Alice and Bob?" 
                                            0.7))
```

### Python Integration
```python
# Import bridge modules
from opencog_koboldcpp import OpenCogKoboldBridge
from opencog_aphrodite import SyncOpenCogAphrodite

# Story generation
bridge = OpenCogKoboldBridge()
result = bridge.generate_from_atomspace(["Character: Alice", "Setting: Forest"])

# LLM inference
client = SyncOpenCogAphrodite()
atoms = [{"type": "ConceptNode", "name": "Alice"}]
response = client.inference(atoms, "You are an AGI system.", "Tell me about Alice.")
```

## Dependencies

### System Requirements
- **Gnucash**: GTK+, WebKit, SQLite, libxml2
- **KoboldCpp**: OpenBLAS, LLVM, CUDA (optional)
- **Aphrodite**: Python 3.10+, PyTorch, CUDA, FastAPI

### Model Requirements
- **KoboldCpp**: GGUF models (4-70B parameters)
- **Aphrodite**: HuggingFace Transformers models (various sizes)

## Configuration

### Environment Variables
```bash
# Gnucash
export GNUCASH_OPENCOG_CONFIG=/etc/opencog/gnucash.conf

# KoboldCpp  
export OPENCOG_KOBOLD_PORT=5001
export OPENCOG_KOBOLD_MODEL_PATH=/var/lib/opencog/models

# Aphrodite
export OPENCOG_APHRODITE_HOST=0.0.0.0
export OPENCOG_APHRODITE_PORT=2242
export CUDA_VISIBLE_DEVICES=0
```

### Directory Structure
```
/var/lib/opencog/
├── models/           # GGUF and transformer models
├── config/          # Service configurations  
└── logs/            # Service logs

/etc/opencog/
├── gnucash.conf     # Gnucash integration config
├── koboldcpp.yaml   # KoboldCpp service config
└── aphrodite.yaml   # Aphrodite Engine config
```

## Development

### Adding New External Packages
1. Create package directory: `new-package/`
2. Add Guix definition: `.guix/modules/new-package.scm`
3. Update CMakeLists.txt with build option
4. Create adapter/bridge modules
5. Add Shepherd service definition
6. Update manifest.scm with dependencies

### Testing Integration
```bash
# Test CMake configuration
cmake .. -DBUILD_NEW_PACKAGE=ON

# Test Guix package
guix build -f .guix/modules/new-package.scm

# Test service
sudo herd start new-package-opencog
```

This integration approach enables OpenCog to leverage powerful external tools while maintaining modularity, reproducibility, and cognitive coherence.