# OCC-GGML Architecture: Mapping Oscillator Cellular Cognition to GGML

## Overview

This document maps the Deep Tree Echo Architecture (occ) cognitive primitives to GGML tensor operations, enabling high-performance inference for the OpenCog-based cognitive system.

## 1. Core Cognitive Components

### 1.1 AtomSpace → Tensor Representation

The AtomSpace hypergraph can be represented as sparse tensors:

| AtomSpace Concept | GGML Representation |
|-------------------|---------------------|
| Atoms (nodes) | Embedding vectors `[vocab_size, embed_dim]` |
| Links (hyperedges) | Adjacency tensors `[n_atoms, n_atoms, link_types]` |
| Truth Values (strength, confidence) | F32 tensor pairs `[n_atoms, 2]` |
| Attention Values (STI, LTI, VLTI) | F32 tensor `[n_atoms, 3]` |

### 1.2 Echo Propagation → Tensor Operations

Spreading activation maps directly to matrix operations:

```
Activation_t+1 = decay * (Adjacency @ Activation_t) + bias
```

| Propagation Step | GGML Operation |
|------------------|----------------|
| Activation spread | MUL_MAT |
| Decay application | MUL (scalar) |
| Threshold filtering | Custom (CLAMP + mask) |
| Normalization | SOFT_MAX or L2_NORM |
| Bidirectional spread | MUL_MAT + TRANSPOSE |

### 1.3 Cognitive Grammar → Neural-Symbolic Operations

| Grammar Operation | GGML Mapping |
|-------------------|--------------|
| Pattern matching | MUL_MAT (similarity) |
| Unification | Custom (symbolic) |
| Inference rules | MUL_MAT + SOFT_MAX |
| Truth value combination | Custom (PLN formulas) |

## 2. Required GGML Operations for OCC

### 2.1 Core Operations (Priority 1)

| Operation | Use in OCC | Notes |
|-----------|------------|-------|
| **MUL_MAT** | Activation propagation, pattern matching | Critical path |
| **ADD** | Combining activations, bias terms | Elementwise |
| **MUL** | Decay, truth value weighting | Elementwise + scalar |
| **SOFT_MAX** | Attention normalization | Row-wise |
| **GET_ROWS** | Atom embedding lookup | Sparse access |
| **SUM_ROWS** | Aggregating activations | Reduction |

### 2.2 Normalization Operations (Priority 2)

| Operation | Use in OCC |
|-----------|------------|
| **L2_NORM** | Activation normalization |
| **RMS_NORM** | Optional (for neural components) |
| **CLAMP** | Threshold filtering |

### 2.3 Shape Operations (Priority 2)

| Operation | Use in OCC |
|-----------|------------|
| **RESHAPE** | View manipulation |
| **PERMUTE** | Dimension reordering |
| **TRANSPOSE** | Bidirectional propagation |
| **CONT** | Contiguous memory |

### 2.4 Custom Operations (Priority 3)

| Operation | Description |
|-----------|-------------|
| **SPREAD_ACTIVATION** | Fused propagation kernel |
| **TRUTH_VALUE_COMBINE** | PLN truth value formulas |
| **ATTENTION_UPDATE** | STI/LTI update dynamics |
| **HYPERGRAPH_CONV** | Hyperedge message passing |

## 3. Data Types

### 3.1 Required Types

| Type | Use Case |
|------|----------|
| **F32** | Activations, truth values, attention values |
| **F16** | Embeddings (memory efficiency) |
| **I32** | Atom indices, link types |

### 3.2 Optional Quantization

For large AtomSpaces (>100K atoms):

| Type | Use Case |
|------|----------|
| **Q8_0** | Compressed embeddings |
| **Q4_K** | Ultra-compressed for mobile |

## 4. Memory Layout

### 4.1 AtomSpace Tensor Layout

```
AtomSpace Memory:
┌─────────────────────────────────────────────────────────────┐
│ Embeddings: [n_atoms, embed_dim] F16/F32                    │
├─────────────────────────────────────────────────────────────┤
│ Truth Values: [n_atoms, 2] F32 (strength, confidence)       │
├─────────────────────────────────────────────────────────────┤
│ Attention Values: [n_atoms, 3] F32 (STI, LTI, VLTI)        │
├─────────────────────────────────────────────────────────────┤
│ Adjacency (sparse): CSR format or dense [n, n, n_types]    │
├─────────────────────────────────────────────────────────────┤
│ Activation Buffer: [n_atoms] F32 (working memory)          │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 Propagation State

```
Propagation State:
┌─────────────────────────────────────────────────────────────┐
│ Current Activation: [n_atoms] F32                           │
├─────────────────────────────────────────────────────────────┤
│ Previous Activation: [n_atoms] F32 (for convergence check) │
├─────────────────────────────────────────────────────────────┤
│ Hop Count: [n_atoms] I32                                    │
├─────────────────────────────────────────────────────────────┤
│ Visited Mask: [n_atoms] I32 (boolean)                       │
└─────────────────────────────────────────────────────────────┘
```

## 5. Computation Graph

### 5.1 Single Propagation Step

```
Input: activation[n], adjacency[n,n], decay, threshold

1. spread = MUL_MAT(adjacency, activation)      // O(n²) or O(nnz)
2. decayed = MUL(spread, decay)                 // O(n)
3. combined = ADD(decayed, activation)          // O(n)
4. normalized = SOFT_MAX(combined) or L2_NORM   // O(n)
5. filtered = CLAMP(normalized, threshold, 1.0) // O(n)

Output: filtered[n]
```

### 5.2 Full Echo Propagation

```
for hop in 1..max_hops:
    activation = propagation_step(activation, adjacency, params)
    if converged(activation, prev_activation):
        break
    prev_activation = activation
```

## 6. Performance Targets

| Metric | Target | Notes |
|--------|--------|-------|
| Atoms supported | 100,000+ | Scalability |
| Propagation latency | <10ms | For 10K atoms |
| Memory footprint | <100MB | For 10K atoms |
| Throughput | 1000+ steps/sec | Batch processing |

## 7. Architecture Selection

Based on the analysis, we'll generate a **custom** GGML scaffold with:

- **Architecture**: x86-64 (AVX2) for development, with ARM64 support
- **Model**: custom (occ-specific operations)
- **Types**: f32, f16, i32
- **Operations**: MUL_MAT, ADD, MUL, SOFT_MAX, GET_ROWS, SUM_ROWS, L2_NORM, CLAMP, RESHAPE, PERMUTE, TRANSPOSE, CONT

Plus custom operations:
- SPREAD_ACTIVATION (fused propagation)
- TRUTH_VALUE_COMBINE (PLN formulas)
- ATTENTION_UPDATE (ECAN dynamics)

## 8. File Structure

```
occ-ggml/
├── include/
│   ├── occ-ggml.h           # Main header
│   ├── occ-atomspace.h      # AtomSpace tensor representation
│   ├── occ-propagation.h    # Echo propagation
│   └── occ-grammar.h        # Cognitive grammar
├── src/
│   ├── occ-ggml.c           # Core implementation
│   ├── occ-ops-avx2.c       # AVX2-optimized operations
│   ├── occ-ops-neon.c       # ARM NEON operations
│   ├── occ-atomspace.c      # AtomSpace implementation
│   ├── occ-propagation.c    # Propagation implementation
│   └── occ-grammar.c        # Grammar implementation
├── tests/
│   ├── test-atomspace.c     # AtomSpace tests
│   ├── test-propagation.c   # Propagation tests
│   └── test-benchmark.c     # Performance benchmarks
├── examples/
│   └── simple-propagation.c # Example usage
├── Makefile
└── README.md
```
