#ifndef OCC_GGML_H
#define OCC_GGML_H

#define _GNU_SOURCE

/**
 * OCC-GGML: Oscillator Cellular Cognition Tensor Library
 * 
 * A minimal GGML-based tensor library for the Deep Tree Echo Architecture.
 * Implements efficient tensor operations for:
 * - Hypergraph memory (AtomSpace)
 * - Echo propagation (spreading activation)
 * - Cognitive grammar (neural-symbolic reasoning)
 * 
 * Based on the ggml-spec formal specification.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// Constants
// =============================================================================

#define OCC_MAX_DIMS        4       // Maximum tensor dimensions
#define OCC_MAX_SRC         10      // Maximum source tensors per op
#define OCC_MAX_NAME        64      // Maximum tensor name length
#define OCC_MEM_ALIGN       32      // Memory alignment (AVX2 friendly)
#define OCC_MAX_ATOMS       1000000 // Maximum atoms in AtomSpace
#define OCC_MAX_LINK_TYPES  256     // Maximum link types
#define OCC_DEFAULT_EMBED   128     // Default embedding dimension

// =============================================================================
// Data Types
// =============================================================================

typedef uint16_t occ_fp16_t;

enum occ_type {
    OCC_TYPE_F32  = 0,   // 32-bit float
    OCC_TYPE_F16  = 1,   // 16-bit float
    OCC_TYPE_I32  = 2,   // 32-bit integer
    OCC_TYPE_I64  = 3,   // 64-bit integer
    OCC_TYPE_Q8_0 = 4,   // 8-bit quantized
    OCC_TYPE_COUNT
};

// =============================================================================
// Type Metadata
// =============================================================================

static inline int64_t occ_blck_size(enum occ_type type) {
    switch (type) {
        case OCC_TYPE_F32:  return 1;
        case OCC_TYPE_F16:  return 1;
        case OCC_TYPE_I32:  return 1;
        case OCC_TYPE_I64:  return 1;
        case OCC_TYPE_Q8_0: return 32;
        default: return 1;
    }
}

static inline size_t occ_type_size(enum occ_type type) {
    switch (type) {
        case OCC_TYPE_F32:  return 4;
        case OCC_TYPE_F16:  return 2;
        case OCC_TYPE_I32:  return 4;
        case OCC_TYPE_I64:  return 8;
        case OCC_TYPE_Q8_0: return 34;  // 2 (scale) + 32 (quants)
        default: return 0;
    }
}

static inline size_t occ_row_size(enum occ_type type, int64_t ne) {
    return occ_type_size(type) * (ne / occ_blck_size(type));
}

static inline bool occ_is_quantized(enum occ_type type) {
    return type == OCC_TYPE_Q8_0;
}

// =============================================================================
// FP16 Conversion
// =============================================================================

#if defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h>

static inline float occ_fp16_to_fp32(occ_fp16_t h) {
    __fp16 f16;
    memcpy(&f16, &h, sizeof(h));
    return (float)f16;
}

static inline occ_fp16_t occ_fp32_to_fp16(float f) {
    __fp16 f16 = (__fp16)f;
    occ_fp16_t h;
    memcpy(&h, &f16, sizeof(h));
    return h;
}

#elif defined(__F16C__)
#include <immintrin.h>

static inline float occ_fp16_to_fp32(occ_fp16_t h) {
    return _cvtsh_ss(h);
}

static inline occ_fp16_t occ_fp32_to_fp16(float f) {
    return _cvtss_sh(f, 0);
}

#else
// Software fallback
static inline float occ_fp16_to_fp32(occ_fp16_t h) {
    uint32_t sign = (h & 0x8000) << 16;
    uint32_t exp = (h >> 10) & 0x1F;
    uint32_t mant = h & 0x3FF;
    
    if (exp == 0) {
        if (mant == 0) {
            uint32_t result = sign;
            float f;
            memcpy(&f, &result, sizeof(f));
            return f;
        }
        // Denormalized
        while (!(mant & 0x400)) {
            mant <<= 1;
            exp--;
        }
        exp++;
        mant &= ~0x400;
    } else if (exp == 31) {
        exp = 255;
    } else {
        exp += 112;
    }
    
    uint32_t result = sign | (exp << 23) | (mant << 13);
    float f;
    memcpy(&f, &result, sizeof(f));
    return f;
}

static inline occ_fp16_t occ_fp32_to_fp16(float f) {
    uint32_t bits;
    memcpy(&bits, &f, sizeof(bits));
    
    uint32_t sign = (bits >> 16) & 0x8000;
    int32_t exp = ((bits >> 23) & 0xFF) - 127 + 15;
    uint32_t mant = bits & 0x7FFFFF;
    
    if (exp <= 0) {
        if (exp < -10) return sign;
        mant = (mant | 0x800000) >> (1 - exp);
        return sign | (mant >> 13);
    } else if (exp >= 31) {
        return sign | 0x7C00;
    }
    
    return sign | (exp << 10) | (mant >> 13);
}
#endif

// =============================================================================
// Operations (forward declaration needed for tensor struct)
// =============================================================================

enum occ_op {
    OCC_OP_NONE = 0,
    
    // Elementwise operations
    OCC_OP_ADD,
    OCC_OP_SUB,
    OCC_OP_MUL,
    OCC_OP_DIV,
    OCC_OP_SCALE,
    OCC_OP_SQR,
    OCC_OP_SQRT,
    OCC_OP_LOG,
    OCC_OP_EXP,
    OCC_OP_NEG,
    OCC_OP_ABS,
    
    // Matrix operations
    OCC_OP_MUL_MAT,
    OCC_OP_MUL_MAT_T,
    
    // Activation functions
    OCC_OP_SIGMOID,
    OCC_OP_TANH,
    OCC_OP_RELU,
    OCC_OP_GELU,
    OCC_OP_SILU,
    OCC_OP_SOFTMAX,
    
    // Normalization
    OCC_OP_L2_NORM,
    OCC_OP_RMS_NORM,
    OCC_OP_LAYER_NORM,
    
    // Reduction
    OCC_OP_SUM,
    OCC_OP_SUM_ROWS,
    OCC_OP_MEAN,
    OCC_OP_ARGMAX,
    OCC_OP_REPEAT,
    
    // Shape operations
    OCC_OP_RESHAPE,
    OCC_OP_VIEW,
    OCC_OP_PERMUTE,
    OCC_OP_TRANSPOSE,
    OCC_OP_CONT,
    
    // Indexing
    OCC_OP_GET_ROWS,
    OCC_OP_CPY,
    
    // Special
    OCC_OP_ROPE,
    OCC_OP_CLAMP,
    OCC_OP_CONCAT,
    
    // OCC-specific operations
    OCC_OP_SPREAD_ACTIVATION,
    OCC_OP_TRUTH_VALUE_COMBINE,
    OCC_OP_ATTENTION_UPDATE,
    OCC_OP_HYPERGRAPH_CONV,
    OCC_OP_OSCILLATOR_STEP,
    
    OCC_OP_COUNT
};

// =============================================================================
// Tensor Structure
// =============================================================================

struct occ_tensor {
    enum occ_type type;
    
    int64_t ne[OCC_MAX_DIMS];   // Number of elements per dimension
    size_t  nb[OCC_MAX_DIMS];   // Stride in bytes per dimension
    
    void* data;                 // Pointer to tensor data
    
    char name[OCC_MAX_NAME];    // Tensor name
    
    // Computation graph fields
    enum occ_op op;             // Operation that produced this tensor
    struct occ_tensor* src[OCC_MAX_SRC];  // Source tensors
    
    // View fields
    struct occ_tensor* view_src;
    size_t view_offs;
};

// =============================================================================
// Removed duplicate enum occ_op (now above tensor struct)
// =============================================================================

// Operations enum is now defined above the tensor struct

// =============================================================================
// Tensor Helpers
// =============================================================================

static inline int64_t occ_nelements(const struct occ_tensor* t) {
    return t->ne[0] * t->ne[1] * t->ne[2] * t->ne[3];
}

static inline size_t occ_nbytes(const struct occ_tensor* t) {
    return occ_row_size(t->type, t->ne[0]) * t->ne[1] * t->ne[2] * t->ne[3];
}

static inline bool occ_is_contiguous(const struct occ_tensor* t) {
    return t->nb[0] == occ_type_size(t->type) &&
           t->nb[1] == t->nb[0] * t->ne[0] &&
           t->nb[2] == t->nb[1] * t->ne[1] &&
           t->nb[3] == t->nb[2] * t->ne[2];
}

static inline void* occ_get_data(const struct occ_tensor* t) {
    return t->data;
}

// Element access (for F32 tensors)
static inline float* occ_get_f32_ptr(const struct occ_tensor* t, 
                                      int64_t i0, int64_t i1, int64_t i2, int64_t i3) {
    return (float*)((char*)t->data + i3*t->nb[3] + i2*t->nb[2] + i1*t->nb[1] + i0*t->nb[0]);
}

static inline float occ_get_f32(const struct occ_tensor* t,
                                 int64_t i0, int64_t i1, int64_t i2, int64_t i3) {
    return *occ_get_f32_ptr(t, i0, i1, i2, i3);
}

static inline void occ_set_f32(struct occ_tensor* t,
                                int64_t i0, int64_t i1, int64_t i2, int64_t i3, float v) {
    *occ_get_f32_ptr(t, i0, i1, i2, i3) = v;
}

// =============================================================================
// Memory Alignment
// =============================================================================

#define OCC_PAD(x, n) (((x) + (n) - 1) & ~((n) - 1))

static inline void* occ_aligned_alloc(size_t size) {
    void* ptr = NULL;
#if defined(_WIN32)
    ptr = _aligned_malloc(size, OCC_MEM_ALIGN);
#else
    posix_memalign(&ptr, OCC_MEM_ALIGN, size);
#endif
    return ptr;
}

static inline void occ_aligned_free(void* ptr) {
#if defined(_WIN32)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

// =============================================================================
// Context
// =============================================================================

struct occ_context {
    size_t mem_size;
    void*  mem_buffer;
    size_t mem_used;
    
    int n_tensors;
    struct occ_tensor* tensors;
};

// Context management
struct occ_context* occ_init(size_t mem_size);
void occ_free(struct occ_context* ctx);

// Tensor creation
struct occ_tensor* occ_new_tensor(struct occ_context* ctx, enum occ_type type,
                                   int64_t ne0, int64_t ne1, int64_t ne2, int64_t ne3);
struct occ_tensor* occ_new_tensor_1d(struct occ_context* ctx, enum occ_type type, int64_t ne0);
struct occ_tensor* occ_new_tensor_2d(struct occ_context* ctx, enum occ_type type, int64_t ne0, int64_t ne1);
struct occ_tensor* occ_new_tensor_3d(struct occ_context* ctx, enum occ_type type, int64_t ne0, int64_t ne1, int64_t ne2);

// =============================================================================
// Core Operations
// =============================================================================

// Elementwise
struct occ_tensor* occ_add(struct occ_context* ctx, struct occ_tensor* a, struct occ_tensor* b);
struct occ_tensor* occ_sub(struct occ_context* ctx, struct occ_tensor* a, struct occ_tensor* b);
struct occ_tensor* occ_mul(struct occ_context* ctx, struct occ_tensor* a, struct occ_tensor* b);
struct occ_tensor* occ_div(struct occ_context* ctx, struct occ_tensor* a, struct occ_tensor* b);
struct occ_tensor* occ_scale(struct occ_context* ctx, struct occ_tensor* a, float s);

// Matrix
struct occ_tensor* occ_mul_mat(struct occ_context* ctx, struct occ_tensor* a, struct occ_tensor* b);

// Activations
struct occ_tensor* occ_sigmoid(struct occ_context* ctx, struct occ_tensor* a);
struct occ_tensor* occ_tanh(struct occ_context* ctx, struct occ_tensor* a);
struct occ_tensor* occ_relu(struct occ_context* ctx, struct occ_tensor* a);
struct occ_tensor* occ_silu(struct occ_context* ctx, struct occ_tensor* a);
struct occ_tensor* occ_softmax(struct occ_context* ctx, struct occ_tensor* a);

// Normalization
struct occ_tensor* occ_l2_norm(struct occ_context* ctx, struct occ_tensor* a, float eps);
struct occ_tensor* occ_rms_norm(struct occ_context* ctx, struct occ_tensor* a, float eps);

// Reduction
struct occ_tensor* occ_sum(struct occ_context* ctx, struct occ_tensor* a);
struct occ_tensor* occ_sum_rows(struct occ_context* ctx, struct occ_tensor* a);
struct occ_tensor* occ_mean(struct occ_context* ctx, struct occ_tensor* a);
struct occ_tensor* occ_argmax(struct occ_context* ctx, struct occ_tensor* a);

// Shape
struct occ_tensor* occ_reshape(struct occ_context* ctx, struct occ_tensor* a, 
                                int64_t ne0, int64_t ne1, int64_t ne2, int64_t ne3);
struct occ_tensor* occ_view(struct occ_context* ctx, struct occ_tensor* a,
                             int64_t ne0, int64_t ne1, int64_t ne2, int64_t ne3, size_t offset);
struct occ_tensor* occ_permute(struct occ_context* ctx, struct occ_tensor* a,
                                int axis0, int axis1, int axis2, int axis3);
struct occ_tensor* occ_transpose(struct occ_context* ctx, struct occ_tensor* a);
struct occ_tensor* occ_cont(struct occ_context* ctx, struct occ_tensor* a);

// Indexing
struct occ_tensor* occ_get_rows(struct occ_context* ctx, struct occ_tensor* a, struct occ_tensor* idx);
struct occ_tensor* occ_set_rows(struct occ_context* ctx, struct occ_tensor* dst, 
                                 struct occ_tensor* src, struct occ_tensor* idx);

// Special
struct occ_tensor* occ_clamp(struct occ_context* ctx, struct occ_tensor* a, float min_val, float max_val);

// =============================================================================
// OCC-Specific Operations
// =============================================================================

/**
 * Spread activation through adjacency matrix
 * 
 * @param ctx Context
 * @param activation Current activation [n_atoms]
 * @param adjacency Adjacency matrix [n_atoms, n_atoms]
 * @param decay Decay factor (0.0 to 1.0)
 * @param threshold Minimum activation threshold
 * @return New activation [n_atoms]
 */
struct occ_tensor* occ_spread_activation(struct occ_context* ctx,
                                          struct occ_tensor* activation,
                                          struct occ_tensor* adjacency,
                                          float decay,
                                          float threshold);

/**
 * Combine truth values using PLN formulas
 * 
 * @param ctx Context
 * @param tv1 First truth value [n, 2] (strength, confidence)
 * @param tv2 Second truth value [n, 2]
 * @param formula PLN formula type (0=AND, 1=OR, 2=NOT, 3=IMPL)
 * @return Combined truth value [n, 2]
 */
struct occ_tensor* occ_truth_value_combine(struct occ_context* ctx,
                                            struct occ_tensor* tv1,
                                            struct occ_tensor* tv2,
                                            int formula);

/**
 * Update attention values using ECAN dynamics
 * 
 * @param ctx Context
 * @param av Current attention values [n_atoms, 3] (STI, LTI, VLTI)
 * @param activation Current activation [n_atoms]
 * @param rent Attention rent (decay)
 * @param wage Attention wage (gain)
 * @return Updated attention values [n_atoms, 3]
 */
struct occ_tensor* occ_attention_update(struct occ_context* ctx,
                                         struct occ_tensor* av,
                                         struct occ_tensor* activation,
                                         float rent,
                                         float wage);

/**
 * Hypergraph convolution (message passing on hyperedges)
 * 
 * @param ctx Context
 * @param node_features Node feature matrix [n_nodes, n_features]
 * @param hyperedge_index Hyperedge incidence [n_edges, max_nodes_per_edge]
 * @param weights Edge weights [n_edges]
 * @return Updated node features [n_nodes, n_features]
 */
struct occ_tensor* occ_hypergraph_conv(struct occ_context* ctx,
                                        struct occ_tensor* node_features,
                                        struct occ_tensor* hyperedge_index,
                                        struct occ_tensor* weights);

// =============================================================================
// Computation Graph
// =============================================================================

struct occ_cgraph {
    int n_nodes;
    int n_leafs;
    struct occ_tensor* nodes[4096];
    struct occ_tensor* leafs[4096];
};

struct occ_cgraph* occ_new_graph(struct occ_context* ctx);
void occ_build_forward(struct occ_cgraph* graph, struct occ_tensor* output);
void occ_compute(struct occ_cgraph* graph);

// =============================================================================
// Backend Selection
// =============================================================================

enum occ_backend {
    OCC_BACKEND_CPU = 0,
    OCC_BACKEND_CPU_AVX2,
    OCC_BACKEND_CPU_NEON,
    OCC_BACKEND_COUNT
};

void occ_set_backend(enum occ_backend backend);
enum occ_backend occ_get_backend(void);

#ifdef __cplusplus
}
#endif

#endif // OCC_GGML_H
