# Cognitive Kernels API Documentation

## Overview

The Cognitive Kernels library provides custom ggml operations for neural-symbolic synthesis in the HurdCog cognitive operating system. It bridges the gap between symbolic reasoning (AtomSpace) and neural computation, enabling true neural-symbolic AI capabilities.

## Architecture

```
┌─────────────────────────────────────────────────────┐
│          Cognitive Kernels Library                  │
├─────────────────────────────────────────────────────┤
│  ┌──────────────┐  ┌──────────────┐               │
│  │   Tensor     │  │   Symbolic   │               │
│  │  Operations  │  │  Reasoning   │               │
│  └──────────────┘  └──────────────┘               │
│  ┌──────────────┐  ┌──────────────┐               │
│  │  Attention   │  │  Hypergraph  │               │
│  │   Kernels    │  │  Operations  │               │
│  └──────────────┘  └──────────────┘               │
├─────────────────────────────────────────────────────┤
│  ┌──────────────┐  ┌──────────────┐               │
│  │  AtomSpace   │  │  GNU Hurd    │               │
│  │   Bridge     │  │ Cognitive API│               │
│  └──────────────┘  └──────────────┘               │
└─────────────────────────────────────────────────────┘
```

## Core Components

### 1. Tensor Operations (`cognitive_tensor_ops.c`)

Custom tensor operations optimized for cognitive primitives.

#### Key Functions

**`create_cognitive_tensor`**
```c
cognitive_tensor_t* create_cognitive_tensor(
    cognitive_tensor_shape_t shape,
    cognitive_tensor_type_t type,
    void* initial_data,
    size_t data_size
);
```
Creates a new cognitive tensor with specified shape and type.

**Parameters:**
- `shape`: 5D tensor shape (modality, depth, context, salience, autonomy)
- `type`: Tensor type (SYMBOLIC, NEURAL, HYBRID, HYPERGRAPH, etc.)
- `initial_data`: Initial data (NULL for zeros)
- `data_size`: Size of data in bytes

**Returns:** Pointer to created tensor or NULL on failure

**Example:**
```c
cognitive_tensor_shape_t shape = {
    .modality = MODALITY_CONCEPTUAL,
    .depth = DEPTH_PATTERN,
    .context = 128,
    .salience = SALIENCE_HIGH,
    .autonomy_index = AUTONOMY_ADAPTIVE
};

float data[128] = {0};
cognitive_tensor_t* tensor = create_cognitive_tensor(
    shape, TENSOR_TYPE_HYBRID, data, sizeof(data)
);
```

---

**`cognitive_convolution`**
```c
cognitive_result_t cognitive_convolution(
    cognitive_tensor_t* input,
    cognitive_tensor_t* kernel,
    cognitive_kernel_config_t config
);
```
Performs cognitive convolution for pattern recognition across cognitive dimensions.

**Parameters:**
- `input`: Input tensor
- `kernel`: Convolution kernel
- `config`: Operation configuration

**Returns:** Result structure with output tensor and metadata

**Example:**
```c
cognitive_kernel_config_t config = {
    .operation = OP_COGNITIVE_CONV,
    .max_iterations = 100,
    .convergence_threshold = 0.01f,
    .attention_threshold = 0.5f
};

cognitive_result_t result = cognitive_convolution(input, kernel, config);
if (result.output_tensor) {
    printf("Confidence: %.2f\n", result.confidence_score);
    // Use result.output_tensor...
    destroy_cognitive_result(&result);
}
```

---

### 2. Symbolic Reasoning (`symbolic_reasoning.c`)

Gradient-free symbolic reasoning operations maintaining logical exactness.

#### Key Functions

**`symbolic_inference`**
```c
cognitive_result_t symbolic_inference(
    cognitive_tensor_t* premises,
    cognitive_tensor_t* rules,
    cognitive_kernel_config_t config
);
```
Performs forward-chaining inference with symbolic rules.

**Example:**
```c
cognitive_result_t result = symbolic_inference(premises, rules, config);
// Result maintains logical exactness (confidence = 1.0)
```

**`symbolic_unification`**
```c
cognitive_result_t symbolic_unification(
    cognitive_tensor_t* term1,
    cognitive_tensor_t* term2,
    cognitive_kernel_config_t config
);
```
Unifies two symbolic terms finding most general unifier.

**`verify_logic_preservation`**
```c
bool verify_logic_preservation(
    cognitive_tensor_t* input,
    cognitive_tensor_t* output,
    float tolerance
);
```
Verifies that logical structure is preserved through transformation.

---

### 3. Attention Kernels (`attention_kernels.c`)

ECAN-style attention allocation and optimization.

#### Key Functions

**`attention_pooling`**
```c
cognitive_result_t attention_pooling(
    cognitive_tensor_t* input,
    cognitive_salience_t target_salience,
    cognitive_kernel_config_t config
);
```
Applies ECAN-style attention pooling to adjust salience levels.

**`attention_focus`**
```c
cognitive_result_t attention_focus(
    cognitive_tensor_t* input,
    size_t focus_size,
    cognitive_kernel_config_t config
);
```
Focuses attention on top-k most salient elements.

**`ecan_attention_spread`**
```c
cognitive_result_t ecan_attention_spread(
    cognitive_tensor_t* source,
    cognitive_tensor_t* targets,
    cognitive_kernel_config_t config
);
```
Spreads attention from source to target nodes in cognitive graph.

---

### 4. Hypergraph Operations (`hypergraph_ops.c`)

Operations on AtomSpace hypergraph structures.

#### Key Functions

**`hypergraph_merge`**
```c
cognitive_result_t hypergraph_merge(
    cognitive_tensor_t* graph1,
    cognitive_tensor_t* graph2,
    cognitive_kernel_config_t config
);
```
Merges two hypergraphs into unified structure.

**`hypergraph_pattern_match`**
```c
cognitive_result_t hypergraph_pattern_match(
    cognitive_tensor_t* graph,
    cognitive_tensor_t* pattern,
    cognitive_kernel_config_t config
);
```
Finds pattern matches within hypergraph structure.

**`hypergraph_embedding`**
```c
cognitive_result_t hypergraph_embedding(
    cognitive_tensor_t* graph,
    uint32_t embedding_dim,
    cognitive_kernel_config_t config
);
```
Generates embeddings for hypergraph nodes.

---

### 5. AtomSpace Bridge (`atomspace_bridge.h/c`)

Integration interface between cognitive kernels and OpenCog AtomSpace.

#### Key Functions

**`atomspace_bridge_init`**
```c
int atomspace_bridge_init(void);
```
Initializes the AtomSpace integration bridge.

**`atomspace_tensor_to_atoms`**
```c
int atomspace_tensor_to_atoms(
    cognitive_tensor_t* tensor,
    struct atomspace_handle* handle,
    atom_type_t default_type
);
```
Converts tensor elements to AtomSpace atoms.

**`atomspace_pattern_query`**
```c
cognitive_result_t atomspace_pattern_query(
    struct atomspace_handle* handle,
    cognitive_tensor_t* pattern,
    cognitive_kernel_config_t config
);
```
Queries AtomSpace for pattern matches.

**`atomspace_pln_inference`**
```c
cognitive_result_t atomspace_pln_inference(
    struct atomspace_handle* handle,
    cognitive_tensor_t* premises,
    cognitive_kernel_config_t config
);
```
Performs Probabilistic Logic Networks inference.

---

### 6. GNU Hurd Cognitive API (`hurd_cognitive_api.h/c`)

Integration between cognitive kernels and GNU Hurd primitives.

#### Key Functions

**`hurd_cognitive_api_init`**
```c
int hurd_cognitive_api_init(void);
```
Initializes GNU Hurd cognitive API.

**`hurd_create_cognitive_primitive`**
```c
struct hurd_primitive* hurd_create_cognitive_primitive(
    hurd_primitive_type_t type,
    void* native_handle
);
```
Creates a cognitive-enabled Hurd primitive (port, task, thread, etc.).

**`hurd_enable_cognitive`**
```c
int hurd_enable_cognitive(struct hurd_primitive* primitive);
```
Enables cognitive capabilities for a primitive.

**`hurd_predict_performance`**
```c
hurd_performance_prediction_t hurd_predict_performance(
    struct hurd_primitive* primitive,
    hurd_cognitive_op_t operation,
    cognitive_kernel_config_t config
);
```
Predicts performance of Hurd operation based on learning.

**`hurd_learn_from_execution`**
```c
int hurd_learn_from_execution(
    struct hurd_primitive* primitive,
    hurd_cognitive_op_t operation,
    uint64_t execution_time_ns,
    bool success
);
```
Learns from operation execution for future optimization.

---

## Data Structures

### Cognitive Tensor Shape (5D)

```c
typedef struct {
    cognitive_modality_t modality;     // Sensory/cognitive modality
    cognitive_depth_t depth;           // Processing depth level
    uint32_t context;                  // Context embedding dimension
    cognitive_salience_t salience;     // Attention salience weighting
    cognitive_autonomy_t autonomy_index; // Agent autonomy measurement
} cognitive_tensor_shape_t;
```

**Modalities:** VISUAL, AUDITORY, CONCEPTUAL, LINGUISTIC, MOTOR, EMOTIONAL, SPATIAL, TEMPORAL, META

**Depth Levels:** PRIMITIVE, PATTERN, ABSTRACTION, META, RECURSIVE, TRANSCENDENT

**Salience:** MINIMAL (0), LOW (25), MEDIUM (50), HIGH (75), CRITICAL (100)

**Autonomy:** PASSIVE (0), REACTIVE (25), ADAPTIVE (50), CREATIVE (75), TRANSCENDENT (100)

### Cognitive Result

```c
typedef struct {
    cognitive_tensor_t* output_tensor;
    float confidence_score;
    uint64_t processing_time_ns;
    uint32_t operations_performed;
    bool convergence_achieved;
    char* debug_info;
} cognitive_result_t;
```

---

## Performance Characteristics

### Target Metrics

- **Latency**: <10ms for cognitive tensor operations
- **Throughput**: 1000+ operations/second
- **Memory**: <100MB for complex cognitive graphs
- **Accuracy**: >99% symbolic reasoning precision
- **Scalability**: Linear scaling with cognitive complexity

### Actual Performance (Test Results)

- **Tests Passed**: 36/36 (100%)
- **Memory Tracking**: Functional
- **Symbolic Exactness**: Maintained (confidence = 1.0)
- **Integration**: AtomSpace and GNU Hurd APIs operational

---

## Usage Examples

### Example 1: Basic Tensor Operations

```c
#include "cognitive_kernels.h"

int main() {
    // Initialize
    cognitive_kernels_init(NULL);
    
    // Create tensor
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_CONCEPTUAL,
        .depth = DEPTH_PATTERN,
        .context = 64,
        .salience = SALIENCE_MEDIUM,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    float data[64];
    for (int i = 0; i < 64; i++) data[i] = (float)i / 64.0f;
    
    cognitive_tensor_t* tensor = create_cognitive_tensor(
        shape, TENSOR_TYPE_HYBRID, data, sizeof(data)
    );
    
    // Perform operation
    cognitive_kernel_config_t config = {
        .attention_threshold = 0.5f
    };
    
    cognitive_result_t result = attention_pooling(
        tensor, SALIENCE_HIGH, config
    );
    
    printf("Confidence: %.2f\n", result.confidence_score);
    
    // Cleanup
    destroy_cognitive_result(&result);
    cognitive_tensor_destroy(tensor);
    cognitive_kernels_shutdown();
    
    return 0;
}
```

### Example 2: GNU Hurd Cognitive Integration

```c
#include "cognitive_kernels.h"
#include "hurd_cognitive_api.h"

int main() {
    // Initialize
    hurd_cognitive_api_init();
    
    // Create cognitive task
    struct hurd_primitive* task = hurd_create_cognitive_primitive(
        HURD_PRIMITIVE_TASK, NULL
    );
    
    hurd_enable_cognitive(task);
    
    // Predict performance
    cognitive_kernel_config_t config = {0};
    hurd_performance_prediction_t pred = hurd_predict_performance(
        task, HURD_COGNITIVE_OP_ALLOCATE, config
    );
    
    printf("Predicted latency: %.2f ms\n", pred.predicted_latency_ms);
    
    // Learn from execution
    hurd_learn_from_execution(task, HURD_COGNITIVE_OP_ALLOCATE, 
                              1000000, true);
    
    // Cleanup
    hurd_destroy_cognitive_primitive(task);
    hurd_cognitive_api_shutdown();
    
    return 0;
}
```

### Example 3: Symbolic Reasoning

```c
#include "cognitive_kernels.h"

int main() {
    cognitive_kernels_init(NULL);
    
    // Create premises and rules
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_LINGUISTIC,
        .depth = DEPTH_ABSTRACTION,
        .context = 32,
        .salience = SALIENCE_HIGH,
        .autonomy_index = AUTONOMY_CREATIVE
    };
    
    float premises_data[32] = {0};
    float rules_data[32] = {0};
    
    cognitive_tensor_t* premises = create_cognitive_tensor(
        shape, TENSOR_TYPE_SYMBOLIC, premises_data, sizeof(premises_data)
    );
    
    cognitive_tensor_t* rules = create_cognitive_tensor(
        shape, TENSOR_TYPE_SYMBOLIC, rules_data, sizeof(rules_data)
    );
    
    // Perform inference
    cognitive_kernel_config_t config = {
        .max_iterations = 100,
        .convergence_threshold = 0.01f
    };
    
    cognitive_result_t result = symbolic_inference(premises, rules, config);
    
    printf("Symbolic exactness: %.1f\n", result.confidence_score);
    printf("Convergence: %s\n", result.convergence_achieved ? "Yes" : "No");
    
    // Cleanup
    destroy_cognitive_result(&result);
    cognitive_tensor_destroy(premises);
    cognitive_tensor_destroy(rules);
    cognitive_kernels_shutdown();
    
    return 0;
}
```

---

## Building

```bash
# Build library and tests
make

# Run tests
make test

# Run with memory checking
make memcheck

# Generate code coverage
make coverage

# Install library
sudo make install

# Clean build artifacts
make clean
```

---

## Dependencies

- **C Compiler**: GCC or Clang with C99 support
- **Math Library**: libm
- **POSIX Threads**: libpthread
- **Optional**: Valgrind (for memory checking), gcov (for coverage)

---

## Thread Safety

All cognitive kernel operations are thread-safe when operating on different tensors. When sharing tensors between threads, external synchronization is required.

---

## Error Handling

All functions return error codes or NULL on failure. Use `cognitive_error_string()` to get human-readable error messages:

```c
int ret = cognitive_kernels_init(&config);
if (ret != COGNITIVE_SUCCESS) {
    fprintf(stderr, "Error: %s\n", cognitive_error_string(ret));
}
```

---

## Memory Management

The library provides tracked memory allocation:

```c
void* ptr = cognitive_alloc(size);
cognitive_free(ptr);

// Get statistics
size_t allocated, freed, peak;
cognitive_memory_stats(&allocated, &freed, &peak);
printf("Peak memory: %zu bytes\n", peak);
```

---

## Performance Monitoring

```c
// Reset stats
cognitive_reset_performance_stats();

// Perform operations...

// Get stats
cognitive_performance_stats_t stats;
cognitive_get_performance_stats(&stats);

printf("Operations: %lu\n", stats.total_operations);
printf("Avg latency: %.2f ms\n", stats.average_latency_ms);
printf("Ops/sec: %.2f\n", stats.operations_per_second);
```

---

## Future Enhancements

- GPU acceleration support
- Distributed tensor operations
- Advanced meta-cognitive capabilities
- Quantum cognitive kernels
- Real-time adaptive optimization
- Neural architecture search integration

---

## License

GNU General Public License v3.0 or later

---

## Contact

For issues and contributions, see the main HurdCog repository:
https://github.com/rzonedevops/hurdcog
