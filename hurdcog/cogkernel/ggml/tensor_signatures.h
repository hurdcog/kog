#ifndef COGNITIVE_TENSOR_SIGNATURES_H
#define COGNITIVE_TENSOR_SIGNATURES_H

/**
 * Cognitive Tensor Signatures for Distributed Agentic Cognitive Grammar Network
 * 
 * This header defines the 5-dimensional tensor architecture for cognitive primitives
 * as specified in the Distributed Agentic Cognitive Grammar Network implementation.
 * 
 * Tensor Shape: [modality, depth, context, salience, autonomy_index]
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Cognitive Modalities
typedef enum {
    MODALITY_VISUAL = 0,
    MODALITY_AUDITORY = 1,
    MODALITY_CONCEPTUAL = 2,
    MODALITY_LINGUISTIC = 3,
    MODALITY_MOTOR = 4,
    MODALITY_EMOTIONAL = 5,
    MODALITY_SPATIAL = 6,
    MODALITY_TEMPORAL = 7,
    MODALITY_META = 8,
    MODALITY_COUNT = 9
} cognitive_modality_t;

// Processing Depth Levels
typedef enum {
    DEPTH_PRIMITIVE = 0,    // Basic cognitive primitives
    DEPTH_PATTERN = 1,      // Pattern recognition level
    DEPTH_ABSTRACTION = 2,  // Abstract concept level
    DEPTH_META = 3,         // Meta-cognitive level
    DEPTH_RECURSIVE = 4,    // Recursive self-analysis
    DEPTH_TRANSCENDENT = 5  // Emergent consciousness level
} cognitive_depth_t;

// Attention Salience Levels
typedef enum {
    SALIENCE_MINIMAL = 0,
    SALIENCE_LOW = 25,
    SALIENCE_MEDIUM = 50,
    SALIENCE_HIGH = 75,
    SALIENCE_CRITICAL = 100
} cognitive_salience_t;

// Autonomy Index Levels
typedef enum {
    AUTONOMY_PASSIVE = 0,      // No autonomous behavior
    AUTONOMY_REACTIVE = 25,    // Reactive responses only
    AUTONOMY_ADAPTIVE = 50,    // Basic adaptation capabilities
    AUTONOMY_CREATIVE = 75,    // Creative problem solving
    AUTONOMY_TRANSCENDENT = 100 // Full autonomous cognition
} cognitive_autonomy_t;

// 5-Dimensional Cognitive Tensor Shape
typedef struct {
    cognitive_modality_t modality;     // [0-8] Cognitive modality type
    cognitive_depth_t depth;           // [0-5] Processing depth level
    uint32_t context;                  // [0-∞] Context embedding dimension
    cognitive_salience_t salience;     // [0-100] Attention salience weighting
    cognitive_autonomy_t autonomy_index; // [0-100] Agent autonomy measurement
} cognitive_tensor_shape_t;

// Cognitive Tensor Data Types
typedef enum {
    TENSOR_TYPE_SYMBOLIC,      // Pure symbolic representation
    TENSOR_TYPE_NEURAL,        // Pure neural representation
    TENSOR_TYPE_HYBRID,        // Neural-symbolic hybrid
    TENSOR_TYPE_HYPERGRAPH,    // AtomSpace hypergraph
    TENSOR_TYPE_ATTENTION,     // ECAN attention allocation
    TENSOR_TYPE_META           // Meta-cognitive reflection
} cognitive_tensor_type_t;

// Forward declarations
struct atomspace_handle;
struct ecan_context;
struct hurd_primitive;

// Cognitive Tensor Structure
typedef struct cognitive_tensor {
    cognitive_tensor_shape_t shape;
    cognitive_tensor_type_t type;
    
    // Data payload
    void* data;
    size_t data_size;
    
    // Metadata
    uint64_t creation_time;
    uint64_t last_access;
    uint32_t access_count;
    float confidence;
    
    // Integration handles
    struct atomspace_handle* atomspace_ref;
    struct ecan_context* attention_ref;
    struct hurd_primitive* hurd_ref;
    
    // Tensor operations
    struct cognitive_tensor* (*clone)(struct cognitive_tensor* self);
    int (*transform)(struct cognitive_tensor* self, void* params);
    float (*similarity)(struct cognitive_tensor* self, struct cognitive_tensor* other);
    void (*destroy)(struct cognitive_tensor* self);
    
} cognitive_tensor_t;

// Cognitive Kernel Operation Types
typedef enum {
    OP_COGNITIVE_CONV,      // Cognitive convolution
    OP_ATTENTION_POOL,      // Attention pooling
    OP_SYMBOLIC_ACTIVATION, // Symbolic activation
    OP_RECURSIVE_TRANSFORM, // Recursive transformation
    OP_META_REFLECTION,     // Meta-cognitive reflection
    OP_HYPERGRAPH_MERGE,    // Hypergraph merge operation
    OP_PATTERN_MATCH,       // Pattern matching
    OP_INFERENCE,           // Logical inference
    OP_COUNT
} cognitive_operation_t;

// Cognitive Kernel Configuration
typedef struct {
    cognitive_operation_t operation;
    cognitive_tensor_shape_t input_shape;
    cognitive_tensor_shape_t output_shape;
    
    // Performance parameters
    uint32_t max_iterations;
    float convergence_threshold;
    bool enable_gpu_acceleration;
    bool enable_parallel_processing;
    
    // Cognitive parameters
    float attention_threshold;
    float salience_decay_rate;
    uint32_t recursive_depth_limit;
    
} cognitive_kernel_config_t;

// Cognitive Operation Result
typedef struct {
    cognitive_tensor_t* output_tensor;
    float confidence_score;
    uint64_t processing_time_ns;
    uint32_t operations_performed;
    bool convergence_achieved;
    char* debug_info;
} cognitive_result_t;

// Function prototypes for cognitive tensor operations

/**
 * Create a new cognitive tensor with specified shape and type
 */
cognitive_tensor_t* create_cognitive_tensor(
    cognitive_tensor_shape_t shape,
    cognitive_tensor_type_t type,
    void* initial_data,
    size_t data_size
);

/**
 * Perform cognitive convolution operation
 */
cognitive_result_t cognitive_convolution(
    cognitive_tensor_t* input,
    cognitive_tensor_t* kernel,
    cognitive_kernel_config_t config
);

/**
 * Perform ECAN-style attention pooling
 */
cognitive_result_t attention_pooling(
    cognitive_tensor_t* input,
    cognitive_salience_t target_salience,
    cognitive_kernel_config_t config
);

/**
 * Perform symbolic activation (gradient-free)
 */
cognitive_result_t symbolic_activation(
    cognitive_tensor_t* input,
    cognitive_kernel_config_t config
);

/**
 * Perform recursive self-similar transformation
 */
cognitive_result_t recursive_transform(
    cognitive_tensor_t* input,
    uint32_t depth,
    cognitive_kernel_config_t config
);

/**
 * Perform meta-cognitive reflection
 */
cognitive_result_t meta_cognitive_reflection(
    cognitive_tensor_t* input,
    cognitive_kernel_config_t config
);

/**
 * Integrate result with AtomSpace
 */
int atomspace_integrate_result(cognitive_result_t result);

/**
 * Integrate result with ECAN attention allocation
 */
int ecan_integrate_result(cognitive_result_t result);

/**
 * Integrate result with GNU Hurd primitives
 */
int hurd_integrate_result(cognitive_result_t result);

/**
 * Cleanup cognitive result and free resources
 */
void destroy_cognitive_result(cognitive_result_t* result);

// Tensor signature validation macros
#define VALIDATE_TENSOR_SHAPE(shape) \
    ((shape).modality < MODALITY_COUNT && \
     (shape).depth <= DEPTH_TRANSCENDENT && \
     (shape).salience <= SALIENCE_CRITICAL && \
     (shape).autonomy_index <= AUTONOMY_TRANSCENDENT)

#define TENSOR_SHAPE_SIGNATURE(shape) \
    (((uint64_t)(shape).modality << 32) | \
     ((uint64_t)(shape).depth << 24) | \
     ((uint64_t)(shape).salience << 16) | \
     ((uint64_t)(shape).autonomy_index << 8) | \
     ((uint64_t)(shape).context & 0xFF))

// Prime factorization mapping for cognitive primitives
#define PRIME_MODALITY_BASE 2
#define PRIME_DEPTH_BASE 3
#define PRIME_CONTEXT_BASE 5
#define PRIME_SALIENCE_BASE 7
#define PRIME_AUTONOMY_BASE 11

#define COGNITIVE_PRIME_SIGNATURE(shape) \
    (pow(PRIME_MODALITY_BASE, (shape).modality) * \
     pow(PRIME_DEPTH_BASE, (shape).depth) * \
     pow(PRIME_CONTEXT_BASE, (shape).context % 100) * \
     pow(PRIME_SALIENCE_BASE, (shape).salience / 10) * \
     pow(PRIME_AUTONOMY_BASE, (shape).autonomy_index / 10))

#endif // COGNITIVE_TENSOR_SIGNATURES_H