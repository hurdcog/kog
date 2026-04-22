/**
 * OCC-GGML Core Implementation
 * 
 * Tensor operations for Oscillator Cellular Cognition.
 * Based on GGML formal specification.
 */

#include "../include/occ-ggml.h"
#include <stdio.h>
#include <assert.h>

// =============================================================================
// Backend Selection
// =============================================================================

static enum occ_backend g_backend = OCC_BACKEND_CPU;

void occ_set_backend(enum occ_backend backend) {
    g_backend = backend;
}

enum occ_backend occ_get_backend(void) {
    return g_backend;
}

// =============================================================================
// Context Management
// =============================================================================

struct occ_context* occ_init(size_t mem_size) {
    struct occ_context* ctx = (struct occ_context*)malloc(sizeof(struct occ_context));
    if (!ctx) return NULL;
    
    ctx->mem_size = mem_size;
    ctx->mem_buffer = occ_aligned_alloc(mem_size);
    if (!ctx->mem_buffer) {
        free(ctx);
        return NULL;
    }
    
    ctx->mem_used = 0;
    ctx->n_tensors = 0;
    ctx->tensors = NULL;
    
    return ctx;
}

void occ_free(struct occ_context* ctx) {
    if (!ctx) return;
    
    if (ctx->mem_buffer) {
        occ_aligned_free(ctx->mem_buffer);
    }
    
    free(ctx);
}

// =============================================================================
// Memory Allocation from Context
// =============================================================================

static void* occ_ctx_alloc(struct occ_context* ctx, size_t size) {
    size = OCC_PAD(size, OCC_MEM_ALIGN);
    
    if (ctx->mem_used + size > ctx->mem_size) {
        fprintf(stderr, "OCC: Out of memory (requested %zu, available %zu)\n",
                size, ctx->mem_size - ctx->mem_used);
        return NULL;
    }
    
    void* ptr = (char*)ctx->mem_buffer + ctx->mem_used;
    ctx->mem_used += size;
    
    return ptr;
}

// =============================================================================
// Tensor Creation
// =============================================================================

struct occ_tensor* occ_new_tensor(struct occ_context* ctx, enum occ_type type,
                                   int64_t ne0, int64_t ne1, int64_t ne2, int64_t ne3) {
    struct occ_tensor* t = (struct occ_tensor*)occ_ctx_alloc(ctx, sizeof(struct occ_tensor));
    if (!t) return NULL;
    
    t->type = type;
    t->ne[0] = ne0;
    t->ne[1] = ne1;
    t->ne[2] = ne2;
    t->ne[3] = ne3;
    
    // Calculate strides
    t->nb[0] = occ_type_size(type);
    t->nb[1] = t->nb[0] * ne0;
    t->nb[2] = t->nb[1] * ne1;
    t->nb[3] = t->nb[2] * ne2;
    
    // Allocate data
    size_t data_size = occ_nbytes(t);
    t->data = occ_ctx_alloc(ctx, data_size);
    if (!t->data) return NULL;
    
    // Initialize
    memset(t->data, 0, data_size);
    t->name[0] = '\0';
    t->op = OCC_OP_NONE;
    for (int i = 0; i < OCC_MAX_SRC; i++) {
        t->src[i] = NULL;
    }
    t->view_src = NULL;
    t->view_offs = 0;
    
    ctx->n_tensors++;
    
    return t;
}

struct occ_tensor* occ_new_tensor_1d(struct occ_context* ctx, enum occ_type type, int64_t ne0) {
    return occ_new_tensor(ctx, type, ne0, 1, 1, 1);
}

struct occ_tensor* occ_new_tensor_2d(struct occ_context* ctx, enum occ_type type, int64_t ne0, int64_t ne1) {
    return occ_new_tensor(ctx, type, ne0, ne1, 1, 1);
}

struct occ_tensor* occ_new_tensor_3d(struct occ_context* ctx, enum occ_type type, int64_t ne0, int64_t ne1, int64_t ne2) {
    return occ_new_tensor(ctx, type, ne0, ne1, ne2, 1);
}

// =============================================================================
// Elementwise Operations
// =============================================================================

struct occ_tensor* occ_add(struct occ_context* ctx, struct occ_tensor* a, struct occ_tensor* b) {
    assert(a->type == OCC_TYPE_F32 && b->type == OCC_TYPE_F32);
    
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_F32, 
                                                a->ne[0], a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_ADD;
    result->src[0] = a;
    result->src[1] = b;
    
    return result;
}

struct occ_tensor* occ_sub(struct occ_context* ctx, struct occ_tensor* a, struct occ_tensor* b) {
    assert(a->type == OCC_TYPE_F32 && b->type == OCC_TYPE_F32);
    
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_F32,
                                                a->ne[0], a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_SUB;
    result->src[0] = a;
    result->src[1] = b;
    
    return result;
}

struct occ_tensor* occ_mul(struct occ_context* ctx, struct occ_tensor* a, struct occ_tensor* b) {
    assert(a->type == OCC_TYPE_F32 && b->type == OCC_TYPE_F32);
    
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_F32,
                                                a->ne[0], a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_MUL;
    result->src[0] = a;
    result->src[1] = b;
    
    return result;
}

struct occ_tensor* occ_div(struct occ_context* ctx, struct occ_tensor* a, struct occ_tensor* b) {
    assert(a->type == OCC_TYPE_F32 && b->type == OCC_TYPE_F32);
    
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_F32,
                                                a->ne[0], a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_DIV;
    result->src[0] = a;
    result->src[1] = b;
    
    return result;
}

struct occ_tensor* occ_scale(struct occ_context* ctx, struct occ_tensor* a, float s) {
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_F32,
                                                a->ne[0], a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_SCALE;
    result->src[0] = a;
    
    // Store scale in result data temporarily (will be used during compute)
    // In a full implementation, this would go in op_params
    
    return result;
}

// =============================================================================
// Matrix Operations
// =============================================================================

struct occ_tensor* occ_mul_mat(struct occ_context* ctx, struct occ_tensor* a, struct occ_tensor* b) {
    // C = A @ B^T
    // A: [m, k], B: [n, k] -> C: [m, n]
    assert(a->type == OCC_TYPE_F32 || occ_is_quantized(a->type));
    assert(b->type == OCC_TYPE_F32);
    assert(a->ne[0] == b->ne[0]);  // k dimension must match
    
    int64_t m = a->ne[1];
    int64_t n = b->ne[1];
    
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_F32, m, n, 1, 1);
    if (!result) return NULL;
    
    result->op = OCC_OP_MUL_MAT;
    result->src[0] = a;
    result->src[1] = b;
    
    return result;
}

// =============================================================================
// Activation Functions
// =============================================================================

struct occ_tensor* occ_sigmoid(struct occ_context* ctx, struct occ_tensor* a) {
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_F32,
                                                a->ne[0], a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_SIGMOID;
    result->src[0] = a;
    
    return result;
}

struct occ_tensor* occ_tanh(struct occ_context* ctx, struct occ_tensor* a) {
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_F32,
                                                a->ne[0], a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_TANH;
    result->src[0] = a;
    
    return result;
}

struct occ_tensor* occ_relu(struct occ_context* ctx, struct occ_tensor* a) {
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_F32,
                                                a->ne[0], a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_RELU;
    result->src[0] = a;
    
    return result;
}

struct occ_tensor* occ_silu(struct occ_context* ctx, struct occ_tensor* a) {
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_F32,
                                                a->ne[0], a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_SILU;
    result->src[0] = a;
    
    return result;
}

struct occ_tensor* occ_softmax(struct occ_context* ctx, struct occ_tensor* a) {
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_F32,
                                                a->ne[0], a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_SOFTMAX;
    result->src[0] = a;
    
    return result;
}

// =============================================================================
// Normalization
// =============================================================================

struct occ_tensor* occ_l2_norm(struct occ_context* ctx, struct occ_tensor* a, float eps) {
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_F32,
                                                a->ne[0], a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_L2_NORM;
    result->src[0] = a;
    
    return result;
}

struct occ_tensor* occ_rms_norm(struct occ_context* ctx, struct occ_tensor* a, float eps) {
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_F32,
                                                a->ne[0], a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_RMS_NORM;
    result->src[0] = a;
    
    return result;
}

// =============================================================================
// Reduction Operations
// =============================================================================

struct occ_tensor* occ_sum(struct occ_context* ctx, struct occ_tensor* a) {
    struct occ_tensor* result = occ_new_tensor_1d(ctx, OCC_TYPE_F32, 1);
    if (!result) return NULL;
    
    result->op = OCC_OP_SUM;
    result->src[0] = a;
    
    return result;
}

struct occ_tensor* occ_sum_rows(struct occ_context* ctx, struct occ_tensor* a) {
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_F32,
                                                1, a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_SUM_ROWS;
    result->src[0] = a;
    
    return result;
}

struct occ_tensor* occ_mean(struct occ_context* ctx, struct occ_tensor* a) {
    struct occ_tensor* result = occ_new_tensor_1d(ctx, OCC_TYPE_F32, 1);
    if (!result) return NULL;
    
    result->op = OCC_OP_MEAN;
    result->src[0] = a;
    
    return result;
}

struct occ_tensor* occ_argmax(struct occ_context* ctx, struct occ_tensor* a) {
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_I32,
                                                1, a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_ARGMAX;
    result->src[0] = a;
    
    return result;
}

// =============================================================================
// Shape Operations
// =============================================================================

struct occ_tensor* occ_reshape(struct occ_context* ctx, struct occ_tensor* a,
                                int64_t ne0, int64_t ne1, int64_t ne2, int64_t ne3) {
    assert(occ_nelements(a) == ne0 * ne1 * ne2 * ne3);
    
    struct occ_tensor* result = (struct occ_tensor*)occ_ctx_alloc(ctx, sizeof(struct occ_tensor));
    if (!result) return NULL;
    
    result->type = a->type;
    result->ne[0] = ne0;
    result->ne[1] = ne1;
    result->ne[2] = ne2;
    result->ne[3] = ne3;
    
    // Recalculate strides for new shape
    result->nb[0] = occ_type_size(a->type);
    result->nb[1] = result->nb[0] * ne0;
    result->nb[2] = result->nb[1] * ne1;
    result->nb[3] = result->nb[2] * ne2;
    
    // Share data (view)
    result->data = a->data;
    result->view_src = a;
    result->view_offs = 0;
    
    result->op = OCC_OP_RESHAPE;
    result->src[0] = a;
    
    return result;
}

struct occ_tensor* occ_view(struct occ_context* ctx, struct occ_tensor* a,
                             int64_t ne0, int64_t ne1, int64_t ne2, int64_t ne3, size_t offset) {
    struct occ_tensor* result = (struct occ_tensor*)occ_ctx_alloc(ctx, sizeof(struct occ_tensor));
    if (!result) return NULL;
    
    result->type = a->type;
    result->ne[0] = ne0;
    result->ne[1] = ne1;
    result->ne[2] = ne2;
    result->ne[3] = ne3;
    
    result->nb[0] = occ_type_size(a->type);
    result->nb[1] = result->nb[0] * ne0;
    result->nb[2] = result->nb[1] * ne1;
    result->nb[3] = result->nb[2] * ne2;
    
    result->data = (char*)a->data + offset;
    result->view_src = a;
    result->view_offs = offset;
    
    result->op = OCC_OP_VIEW;
    result->src[0] = a;
    
    return result;
}

struct occ_tensor* occ_permute(struct occ_context* ctx, struct occ_tensor* a,
                                int axis0, int axis1, int axis2, int axis3) {
    struct occ_tensor* result = (struct occ_tensor*)occ_ctx_alloc(ctx, sizeof(struct occ_tensor));
    if (!result) return NULL;
    
    int axes[4] = {axis0, axis1, axis2, axis3};
    
    result->type = a->type;
    result->ne[0] = a->ne[axes[0]];
    result->ne[1] = a->ne[axes[1]];
    result->ne[2] = a->ne[axes[2]];
    result->ne[3] = a->ne[axes[3]];
    
    result->nb[0] = a->nb[axes[0]];
    result->nb[1] = a->nb[axes[1]];
    result->nb[2] = a->nb[axes[2]];
    result->nb[3] = a->nb[axes[3]];
    
    result->data = a->data;
    result->view_src = a;
    result->view_offs = 0;
    
    result->op = OCC_OP_PERMUTE;
    result->src[0] = a;
    
    return result;
}

struct occ_tensor* occ_transpose(struct occ_context* ctx, struct occ_tensor* a) {
    return occ_permute(ctx, a, 1, 0, 2, 3);
}

struct occ_tensor* occ_cont(struct occ_context* ctx, struct occ_tensor* a) {
    if (occ_is_contiguous(a)) {
        return a;  // Already contiguous
    }
    
    struct occ_tensor* result = occ_new_tensor(ctx, a->type,
                                                a->ne[0], a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_CONT;
    result->src[0] = a;
    
    return result;
}

// =============================================================================
// Indexing Operations
// =============================================================================

struct occ_tensor* occ_get_rows(struct occ_context* ctx, struct occ_tensor* a, struct occ_tensor* idx) {
    assert(idx->type == OCC_TYPE_I32);
    
    int64_t n_rows = occ_nelements(idx);
    
    struct occ_tensor* result = occ_new_tensor_2d(ctx, a->type, a->ne[0], n_rows);
    if (!result) return NULL;
    
    result->op = OCC_OP_GET_ROWS;
    result->src[0] = a;
    result->src[1] = idx;
    
    return result;
}

// =============================================================================
// Special Operations
// =============================================================================

struct occ_tensor* occ_clamp(struct occ_context* ctx, struct occ_tensor* a, float min_val, float max_val) {
    struct occ_tensor* result = occ_new_tensor(ctx, OCC_TYPE_F32,
                                                a->ne[0], a->ne[1], a->ne[2], a->ne[3]);
    if (!result) return NULL;
    
    result->op = OCC_OP_CLAMP;
    result->src[0] = a;
    
    return result;
}

// =============================================================================
// OCC-Specific Operations
// =============================================================================

struct occ_tensor* occ_spread_activation(struct occ_context* ctx,
                                          struct occ_tensor* activation,
                                          struct occ_tensor* adjacency,
                                          float decay,
                                          float threshold) {
    assert(activation->type == OCC_TYPE_F32);
    assert(adjacency->type == OCC_TYPE_F32);
    assert(activation->ne[0] == adjacency->ne[0]);
    assert(adjacency->ne[0] == adjacency->ne[1]);
    
    struct occ_tensor* result = occ_new_tensor_1d(ctx, OCC_TYPE_F32, activation->ne[0]);
    if (!result) return NULL;
    
    result->op = OCC_OP_SPREAD_ACTIVATION;
    result->src[0] = activation;
    result->src[1] = adjacency;
    
    return result;
}

struct occ_tensor* occ_truth_value_combine(struct occ_context* ctx,
                                            struct occ_tensor* tv1,
                                            struct occ_tensor* tv2,
                                            int formula) {
    assert(tv1->type == OCC_TYPE_F32);
    assert(tv2->type == OCC_TYPE_F32);
    assert(tv1->ne[0] == 2);  // (strength, confidence)
    assert(tv2->ne[0] == 2);
    
    struct occ_tensor* result = occ_new_tensor_2d(ctx, OCC_TYPE_F32, 2, tv1->ne[1]);
    if (!result) return NULL;
    
    result->op = OCC_OP_TRUTH_VALUE_COMBINE;
    result->src[0] = tv1;
    result->src[1] = tv2;
    
    return result;
}

struct occ_tensor* occ_attention_update(struct occ_context* ctx,
                                         struct occ_tensor* av,
                                         struct occ_tensor* activation,
                                         float rent,
                                         float wage) {
    assert(av->type == OCC_TYPE_F32);
    assert(activation->type == OCC_TYPE_F32);
    assert(av->ne[0] == 3);  // (STI, LTI, VLTI)
    
    struct occ_tensor* result = occ_new_tensor_2d(ctx, OCC_TYPE_F32, 3, av->ne[1]);
    if (!result) return NULL;
    
    result->op = OCC_OP_ATTENTION_UPDATE;
    result->src[0] = av;
    result->src[1] = activation;
    
    return result;
}

struct occ_tensor* occ_hypergraph_conv(struct occ_context* ctx,
                                        struct occ_tensor* node_features,
                                        struct occ_tensor* hyperedge_index,
                                        struct occ_tensor* weights) {
    assert(node_features->type == OCC_TYPE_F32);
    
    struct occ_tensor* result = occ_new_tensor_2d(ctx, OCC_TYPE_F32,
                                                   node_features->ne[0], node_features->ne[1]);
    if (!result) return NULL;
    
    result->op = OCC_OP_HYPERGRAPH_CONV;
    result->src[0] = node_features;
    result->src[1] = hyperedge_index;
    result->src[2] = weights;
    
    return result;
}

// =============================================================================
// Computation Graph
// =============================================================================

struct occ_cgraph* occ_new_graph(struct occ_context* ctx) {
    struct occ_cgraph* graph = (struct occ_cgraph*)occ_ctx_alloc(ctx, sizeof(struct occ_cgraph));
    if (!graph) return NULL;
    
    graph->n_nodes = 0;
    graph->n_leafs = 0;
    
    return graph;
}

static void occ_build_forward_impl(struct occ_cgraph* graph, struct occ_tensor* tensor, bool* visited) {
    if (!tensor) return;
    
    // Check if already visited (by pointer comparison)
    // In a full implementation, use a hash set
    for (int i = 0; i < graph->n_nodes; i++) {
        if (graph->nodes[i] == tensor) return;
    }
    for (int i = 0; i < graph->n_leafs; i++) {
        if (graph->leafs[i] == tensor) return;
    }
    
    // Visit sources first
    for (int i = 0; i < OCC_MAX_SRC; i++) {
        occ_build_forward_impl(graph, tensor->src[i], visited);
    }
    
    // Add to graph
    if (tensor->op == OCC_OP_NONE) {
        // Leaf node (input)
        graph->leafs[graph->n_leafs++] = tensor;
    } else {
        // Operation node
        graph->nodes[graph->n_nodes++] = tensor;
    }
}

void occ_build_forward(struct occ_cgraph* graph, struct occ_tensor* output) {
    bool visited[4096] = {false};
    occ_build_forward_impl(graph, output, visited);
}
