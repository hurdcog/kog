# OCC-GGML: Oscillator Cellular Cognition Tensor Library

A minimal GGML-based tensor library implementing the **Deep Tree Echo Architecture** for cognitive computing. This library provides efficient tensor operations for hypergraph memory, spreading activation, and oscillator-based neural dynamics.

## Overview

OCC-GGML bridges the gap between the OpenCog cognitive architecture and efficient tensor computing by implementing:

1. **AtomSpace** - Tensor-based hypergraph memory for knowledge representation
2. **Echo Propagation** - Spreading activation through cognitive graphs
3. **Oscillator Dynamics** - Phase-coupled oscillators for emergent pattern formation
4. **PLN Operations** - Probabilistic Logic Networks truth value formulas

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        OCC-GGML Stack                           │
├─────────────────────────────────────────────────────────────────┤
│  Applications: Cognitive Agents, Knowledge Graphs, Reasoning    │
├─────────────────────────────────────────────────────────────────┤
│  Propagation Engine: Spreading Activation, ECAN, Oscillators    │
├─────────────────────────────────────────────────────────────────┤
│  AtomSpace: Hypergraph Memory, Truth Values, Attention Values   │
├─────────────────────────────────────────────────────────────────┤
│  Tensor Operations: GGML-style ops with AVX2/NEON acceleration  │
├─────────────────────────────────────────────────────────────────┤
│  Memory Management: Arena allocation, aligned buffers           │
└─────────────────────────────────────────────────────────────────┘
```

## Features

### Tensor Operations
- **Elementwise**: add, sub, mul, div, scale, sqrt, exp, log
- **Matrix**: mul_mat (with AVX2/FMA acceleration)
- **Activations**: sigmoid, tanh, relu, silu, softmax
- **Normalization**: L2 norm, RMS norm
- **Reduction**: sum, mean, argmax
- **Shape**: reshape, view, permute, transpose

### AtomSpace
- Concept nodes, predicate nodes, link types
- Truth values (strength, confidence, count)
- Attention values (STI, LTI, VLTI)
- Dense/sparse adjacency matrices
- Embedding vectors for neural-symbolic integration
- Name-based and type-based indexing

### Propagation Engine
- Spreading activation with configurable decay
- Bidirectional propagation
- Truth-value weighted edges
- Attention-weighted propagation
- Convergence detection
- Top-k result extraction

### Oscillator Dynamics
- Kuramoto-style coupled oscillators
- Phase synchronization detection
- Cluster formation analysis
- ECAN attention dynamics integration

## Building

```bash
# Build library and examples
make

# Build tests
make tests

# Run tests
make test

# Install (optional)
sudo make install
```

### Build Options

```bash
# Debug build
make DEBUG=1

# Custom install prefix
make install PREFIX=/opt/occ-ggml
```

## Usage

### Basic Example

```c
#include "occ-ggml.h"
#include "occ-atomspace.h"
#include "occ-propagation.h"

int main() {
    // Create AtomSpace with 1000 atoms, 64-dim embeddings
    struct occ_atomspace* as = occ_atomspace_new(1000, 64, true);
    
    // Add concept nodes
    int64_t cat = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Cat", NULL);
    int64_t mammal = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Mammal", NULL);
    
    // Add inheritance link
    int64_t out[] = {cat, mammal};
    occ_atomspace_add_link(as, OCC_ATOM_INHERITANCE_LINK, out, 2, NULL);
    
    // Create propagation engine
    struct occ_propagation_state* prop = occ_propagation_new(as);
    struct occ_propagation_params params = OCC_DEFAULT_PROPAGATION_PARAMS;
    
    // Propagate from "Cat"
    occ_propagate_from_atom(prop, cat, 1.0f, &params);
    
    // Get top activated concepts
    int64_t top_ids[10];
    float top_acts[10];
    int64_t n = occ_propagation_get_top_k(prop, 10, top_ids, top_acts);
    
    // Cleanup
    occ_propagation_free(prop);
    occ_atomspace_free(as);
    
    return 0;
}
```

### Oscillator Dynamics

```c
// Initialize oscillators
struct occ_oscillator_params osc_params = {
    .frequency = 1.0f,
    .damping = 0.5f,
    .coupling = 0.3f,
    .phase_noise = 0.1f,
    .n_harmonics = 1
};

occ_oscillator_init(prop, &osc_params);

// Run dynamics
for (int i = 0; i < 100; i++) {
    occ_oscillator_step(prop, &osc_params, 0.1f);
}

// Detect synchronized clusters
int32_t cluster_ids[1000];
int n_clusters = occ_oscillator_detect_clusters(prop, 0.3f, cluster_ids);
```

## API Reference

### Context Management
- `occ_init(size_t mem_size)` - Create context with memory pool
- `occ_free(ctx)` - Free context and all tensors

### Tensor Operations
- `occ_new_tensor_1d/2d/3d(ctx, type, ...)` - Create tensors
- `occ_add/sub/mul/div(ctx, a, b)` - Elementwise operations
- `occ_mul_mat(ctx, a, b)` - Matrix multiplication
- `occ_softmax/sigmoid/tanh/relu(ctx, a)` - Activations

### AtomSpace
- `occ_atomspace_new(max_atoms, embed_dim, use_dense)` - Create AtomSpace
- `occ_atomspace_add_node(as, type, name, tv)` - Add node
- `occ_atomspace_add_link(as, type, outgoing, n, tv)` - Add link
- `occ_atomspace_find_by_name(as, name)` - Find by name
- `occ_atomspace_set/get_tv(as, id, tv)` - Truth values
- `occ_atomspace_set/get_av(as, id, av)` - Attention values

### Propagation
- `occ_propagation_new(as)` - Create propagation state
- `occ_propagate_from_atom(state, id, activation, params)` - Single-source
- `occ_propagation_get_top_k(state, k, ids, acts)` - Get results
- `occ_propagation_update_attention(state, rent, wage, threshold)` - ECAN

### Oscillators
- `occ_oscillator_init(state, params)` - Initialize oscillators
- `occ_oscillator_step(state, params, dt)` - Step dynamics
- `occ_oscillator_detect_clusters(state, threshold, ids)` - Find clusters

## File Structure

```
occ-ggml/
├── include/
│   ├── occ-ggml.h          # Core tensor library
│   ├── occ-atomspace.h     # Hypergraph memory
│   └── occ-propagation.h   # Spreading activation
├── src/
│   ├── occ-ggml.c          # Core implementation
│   ├── occ-ops-avx2.c      # SIMD-optimized operations
│   ├── occ-atomspace.c     # AtomSpace implementation
│   └── occ-propagation.c   # Propagation engine
├── examples/
│   └── simple-propagation.c
├── tests/
│   ├── test-atomspace.c
│   ├── test-propagation.c
│   └── test-benchmark.c
├── ARCHITECTURE.md         # Detailed architecture docs
├── Makefile
└── README.md
```

## Performance

The library is optimized for:
- **AVX2/FMA** acceleration on x86-64
- **NEON** acceleration on ARM64 (planned)
- Cache-friendly blocked matrix multiplication
- Arena-based memory allocation

Typical performance on modern x86-64:
- Matrix multiply (256x256): ~5ms (~2.5 GFLOPS)
- Propagation (1000 nodes): ~1ms per iteration
- Oscillator step (1000 nodes): ~0.1ms

## Integration with OpenCog

OCC-GGML is designed to complement the OpenCog Hyperon framework:

1. **AtomSpace Compatibility**: Similar concepts (nodes, links, truth values)
2. **PLN Integration**: Truth value formulas for probabilistic reasoning
3. **ECAN Support**: Economic Attention Allocation dynamics
4. **Neural-Symbolic Bridge**: Embedding vectors for hybrid architectures

## Future Work

- [ ] Sparse adjacency matrix support
- [ ] GPU acceleration (CUDA/Metal)
- [ ] WASM compilation target
- [ ] Python bindings
- [ ] Integration with Hyperon MeTTa
- [ ] Distributed propagation across nodes

## License

MIT License - See LICENSE file for details.

## References

- [OpenCog Hyperon](https://github.com/trueagi-io/hyperon-experimental)
- [GGML](https://github.com/ggerganov/ggml)
- [Deep Tree Echo Architecture](https://github.com/o9nn/occ)
- [Probabilistic Logic Networks](https://wiki.opencog.org/w/PLN)
- [ECAN](https://wiki.opencog.org/w/ECAN)
