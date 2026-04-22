/**
 * OCC-GGML AVX2-Optimized Operations
 * 
 * SIMD-accelerated tensor operations for x86-64 with AVX2.
 */

#include "../include/occ-ggml.h"
#include <stdio.h>
#include <math.h>

#if defined(__AVX2__) || defined(__AVX__)
#include <immintrin.h>
#define OCC_USE_AVX2 1
#else
#define OCC_USE_AVX2 0
#endif

// =============================================================================
// Scalar Fallbacks
// =============================================================================

static void occ_compute_add_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const struct occ_tensor* b = dst->src[1];
    
    const float* pa = (const float*)a->data;
    const float* pb = (const float*)b->data;
    float* pd = (float*)dst->data;
    
    int64_t n = occ_nelements(dst);
    for (int64_t i = 0; i < n; i++) {
        pd[i] = pa[i] + pb[i];
    }
}

static void occ_compute_sub_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const struct occ_tensor* b = dst->src[1];
    
    const float* pa = (const float*)a->data;
    const float* pb = (const float*)b->data;
    float* pd = (float*)dst->data;
    
    int64_t n = occ_nelements(dst);
    for (int64_t i = 0; i < n; i++) {
        pd[i] = pa[i] - pb[i];
    }
}

static void occ_compute_mul_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const struct occ_tensor* b = dst->src[1];
    
    const float* pa = (const float*)a->data;
    const float* pb = (const float*)b->data;
    float* pd = (float*)dst->data;
    
    int64_t n = occ_nelements(dst);
    for (int64_t i = 0; i < n; i++) {
        pd[i] = pa[i] * pb[i];
    }
}

static void occ_compute_div_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const struct occ_tensor* b = dst->src[1];
    
    const float* pa = (const float*)a->data;
    const float* pb = (const float*)b->data;
    float* pd = (float*)dst->data;
    
    int64_t n = occ_nelements(dst);
    for (int64_t i = 0; i < n; i++) {
        pd[i] = pa[i] / pb[i];
    }
}

// =============================================================================
// AVX2 Implementations
// =============================================================================

#if OCC_USE_AVX2

static void occ_compute_add_avx2(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const struct occ_tensor* b = dst->src[1];
    
    const float* pa = (const float*)a->data;
    const float* pb = (const float*)b->data;
    float* pd = (float*)dst->data;
    
    int64_t n = occ_nelements(dst);
    int64_t i = 0;
    
    // Process 8 floats at a time with AVX
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(pa + i);
        __m256 vb = _mm256_loadu_ps(pb + i);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(pd + i, vc);
    }
    
    // Handle remainder
    for (; i < n; i++) {
        pd[i] = pa[i] + pb[i];
    }
}

static void occ_compute_sub_avx2(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const struct occ_tensor* b = dst->src[1];
    
    const float* pa = (const float*)a->data;
    const float* pb = (const float*)b->data;
    float* pd = (float*)dst->data;
    
    int64_t n = occ_nelements(dst);
    int64_t i = 0;
    
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(pa + i);
        __m256 vb = _mm256_loadu_ps(pb + i);
        __m256 vc = _mm256_sub_ps(va, vb);
        _mm256_storeu_ps(pd + i, vc);
    }
    
    for (; i < n; i++) {
        pd[i] = pa[i] - pb[i];
    }
}

static void occ_compute_mul_avx2(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const struct occ_tensor* b = dst->src[1];
    
    const float* pa = (const float*)a->data;
    const float* pb = (const float*)b->data;
    float* pd = (float*)dst->data;
    
    int64_t n = occ_nelements(dst);
    int64_t i = 0;
    
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(pa + i);
        __m256 vb = _mm256_loadu_ps(pb + i);
        __m256 vc = _mm256_mul_ps(va, vb);
        _mm256_storeu_ps(pd + i, vc);
    }
    
    for (; i < n; i++) {
        pd[i] = pa[i] * pb[i];
    }
}

static void occ_compute_div_avx2(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const struct occ_tensor* b = dst->src[1];
    
    const float* pa = (const float*)a->data;
    const float* pb = (const float*)b->data;
    float* pd = (float*)dst->data;
    
    int64_t n = occ_nelements(dst);
    int64_t i = 0;
    
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(pa + i);
        __m256 vb = _mm256_loadu_ps(pb + i);
        __m256 vc = _mm256_div_ps(va, vb);
        _mm256_storeu_ps(pd + i, vc);
    }
    
    for (; i < n; i++) {
        pd[i] = pa[i] / pb[i];
    }
}

#endif // OCC_USE_AVX2

// =============================================================================
// Matrix Multiplication
// =============================================================================

// Naive implementation for reference
static void occ_compute_mul_mat_naive(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];  // [k, m]
    const struct occ_tensor* b = dst->src[1];  // [k, n]
    // Result: [m, n]
    
    const float* pa = (const float*)a->data;
    const float* pb = (const float*)b->data;
    float* pd = (float*)dst->data;
    
    int64_t m = a->ne[1];
    int64_t n = b->ne[1];
    int64_t k = a->ne[0];
    
    // C[i,j] = sum_l A[l,i] * B[l,j]
    for (int64_t i = 0; i < m; i++) {
        for (int64_t j = 0; j < n; j++) {
            float sum = 0.0f;
            for (int64_t l = 0; l < k; l++) {
                sum += pa[l + i * k] * pb[l + j * k];
            }
            pd[i + j * m] = sum;
        }
    }
}

#if OCC_USE_AVX2

// AVX2-optimized matrix multiplication with FMA
static void occ_compute_mul_mat_avx2(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const struct occ_tensor* b = dst->src[1];
    
    const float* pa = (const float*)a->data;
    const float* pb = (const float*)b->data;
    float* pd = (float*)dst->data;
    
    int64_t m = a->ne[1];
    int64_t n = b->ne[1];
    int64_t k = a->ne[0];
    
    // Initialize result to zero
    memset(pd, 0, m * n * sizeof(float));
    
    // Blocked matrix multiplication for cache efficiency
    const int64_t BLOCK = 64;
    
    for (int64_t i0 = 0; i0 < m; i0 += BLOCK) {
        int64_t i1 = (i0 + BLOCK < m) ? i0 + BLOCK : m;
        
        for (int64_t j0 = 0; j0 < n; j0 += BLOCK) {
            int64_t j1 = (j0 + BLOCK < n) ? j0 + BLOCK : n;
            
            for (int64_t l0 = 0; l0 < k; l0 += BLOCK) {
                int64_t l1 = (l0 + BLOCK < k) ? l0 + BLOCK : k;
                
                // Inner kernel
                for (int64_t i = i0; i < i1; i++) {
                    for (int64_t j = j0; j < j1; j++) {
                        __m256 sum = _mm256_setzero_ps();
                        int64_t l = l0;
                        
                        // Vectorized inner loop
                        for (; l + 8 <= l1; l += 8) {
                            __m256 va = _mm256_loadu_ps(pa + l + i * k);
                            __m256 vb = _mm256_loadu_ps(pb + l + j * k);
                            #ifdef __FMA__
                            sum = _mm256_fmadd_ps(va, vb, sum);
                            #else
                            sum = _mm256_add_ps(sum, _mm256_mul_ps(va, vb));
                            #endif
                        }
                        
                        // Horizontal sum
                        __m128 hi = _mm256_extractf128_ps(sum, 1);
                        __m128 lo = _mm256_castps256_ps128(sum);
                        __m128 sum128 = _mm_add_ps(hi, lo);
                        sum128 = _mm_hadd_ps(sum128, sum128);
                        sum128 = _mm_hadd_ps(sum128, sum128);
                        
                        float partial = _mm_cvtss_f32(sum128);
                        
                        // Handle remainder
                        for (; l < l1; l++) {
                            partial += pa[l + i * k] * pb[l + j * k];
                        }
                        
                        pd[i + j * m] += partial;
                    }
                }
            }
        }
    }
}

#endif // OCC_USE_AVX2

// =============================================================================
// Activation Functions
// =============================================================================

static void occ_compute_sigmoid_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const float* pa = (const float*)a->data;
    float* pd = (float*)dst->data;
    
    int64_t n = occ_nelements(dst);
    for (int64_t i = 0; i < n; i++) {
        pd[i] = 1.0f / (1.0f + expf(-pa[i]));
    }
}

static void occ_compute_tanh_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const float* pa = (const float*)a->data;
    float* pd = (float*)dst->data;
    
    int64_t n = occ_nelements(dst);
    for (int64_t i = 0; i < n; i++) {
        pd[i] = tanhf(pa[i]);
    }
}

static void occ_compute_relu_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const float* pa = (const float*)a->data;
    float* pd = (float*)dst->data;
    
    int64_t n = occ_nelements(dst);
    for (int64_t i = 0; i < n; i++) {
        pd[i] = pa[i] > 0.0f ? pa[i] : 0.0f;
    }
}

static void occ_compute_silu_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const float* pa = (const float*)a->data;
    float* pd = (float*)dst->data;
    
    int64_t n = occ_nelements(dst);
    for (int64_t i = 0; i < n; i++) {
        pd[i] = pa[i] / (1.0f + expf(-pa[i]));
    }
}

static void occ_compute_softmax_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const float* pa = (const float*)a->data;
    float* pd = (float*)dst->data;
    
    int64_t ne0 = dst->ne[0];
    int64_t ne1 = dst->ne[1];
    
    // Softmax along dimension 0
    for (int64_t j = 0; j < ne1; j++) {
        // Find max for numerical stability
        float max_val = -INFINITY;
        for (int64_t i = 0; i < ne0; i++) {
            float v = pa[i + j * ne0];
            if (v > max_val) max_val = v;
        }
        
        // Compute exp and sum
        float sum = 0.0f;
        for (int64_t i = 0; i < ne0; i++) {
            float v = expf(pa[i + j * ne0] - max_val);
            pd[i + j * ne0] = v;
            sum += v;
        }
        
        // Normalize
        float inv_sum = 1.0f / sum;
        for (int64_t i = 0; i < ne0; i++) {
            pd[i + j * ne0] *= inv_sum;
        }
    }
}

// =============================================================================
// Normalization
// =============================================================================

static void occ_compute_l2_norm_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const float* pa = (const float*)a->data;
    float* pd = (float*)dst->data;
    
    int64_t ne0 = dst->ne[0];
    int64_t ne1 = dst->ne[1];
    
    const float eps = 1e-6f;
    
    for (int64_t j = 0; j < ne1; j++) {
        // Compute L2 norm
        float sum_sq = 0.0f;
        for (int64_t i = 0; i < ne0; i++) {
            float v = pa[i + j * ne0];
            sum_sq += v * v;
        }
        
        float norm = sqrtf(sum_sq + eps);
        float inv_norm = 1.0f / norm;
        
        // Normalize
        for (int64_t i = 0; i < ne0; i++) {
            pd[i + j * ne0] = pa[i + j * ne0] * inv_norm;
        }
    }
}

static void occ_compute_rms_norm_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const float* pa = (const float*)a->data;
    float* pd = (float*)dst->data;
    
    int64_t ne0 = dst->ne[0];
    int64_t ne1 = dst->ne[1];
    
    const float eps = 1e-5f;
    
    for (int64_t j = 0; j < ne1; j++) {
        // Compute RMS
        float sum_sq = 0.0f;
        for (int64_t i = 0; i < ne0; i++) {
            float v = pa[i + j * ne0];
            sum_sq += v * v;
        }
        
        float rms = sqrtf(sum_sq / ne0 + eps);
        float inv_rms = 1.0f / rms;
        
        // Normalize
        for (int64_t i = 0; i < ne0; i++) {
            pd[i + j * ne0] = pa[i + j * ne0] * inv_rms;
        }
    }
}

// =============================================================================
// Reduction Operations
// =============================================================================

static void occ_compute_sum_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const float* pa = (const float*)a->data;
    float* pd = (float*)dst->data;
    
    int64_t n = occ_nelements(a);
    float sum = 0.0f;
    for (int64_t i = 0; i < n; i++) {
        sum += pa[i];
    }
    pd[0] = sum;
}

static void occ_compute_sum_rows_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const float* pa = (const float*)a->data;
    float* pd = (float*)dst->data;
    
    int64_t ne0 = a->ne[0];
    int64_t ne1 = a->ne[1];
    
    for (int64_t j = 0; j < ne1; j++) {
        float sum = 0.0f;
        for (int64_t i = 0; i < ne0; i++) {
            sum += pa[i + j * ne0];
        }
        pd[j] = sum;
    }
}

static void occ_compute_mean_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const float* pa = (const float*)a->data;
    float* pd = (float*)dst->data;
    
    int64_t n = occ_nelements(a);
    float sum = 0.0f;
    for (int64_t i = 0; i < n; i++) {
        sum += pa[i];
    }
    pd[0] = sum / n;
}

static void occ_compute_argmax_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const float* pa = (const float*)a->data;
    int32_t* pd = (int32_t*)dst->data;
    
    int64_t ne0 = a->ne[0];
    int64_t ne1 = a->ne[1];
    
    for (int64_t j = 0; j < ne1; j++) {
        float max_val = -INFINITY;
        int32_t max_idx = 0;
        for (int64_t i = 0; i < ne0; i++) {
            float v = pa[i + j * ne0];
            if (v > max_val) {
                max_val = v;
                max_idx = (int32_t)i;
            }
        }
        pd[j] = max_idx;
    }
}

// =============================================================================
// Indexing Operations
// =============================================================================

static void occ_compute_get_rows_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];    // Embedding table
    const struct occ_tensor* idx = dst->src[1];  // Indices
    
    const float* pa = (const float*)a->data;
    const int32_t* pidx = (const int32_t*)idx->data;
    float* pd = (float*)dst->data;
    
    int64_t ne0 = a->ne[0];  // Embedding dimension
    int64_t n_idx = occ_nelements(idx);
    
    for (int64_t i = 0; i < n_idx; i++) {
        int32_t row = pidx[i];
        memcpy(pd + i * ne0, pa + row * ne0, ne0 * sizeof(float));
    }
}

// =============================================================================
// Special Operations
// =============================================================================

static void occ_compute_clamp_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    const float* pa = (const float*)a->data;
    float* pd = (float*)dst->data;
    
    // In a full implementation, min/max would come from op_params
    float min_val = 0.0f;
    float max_val = 1.0f;
    
    int64_t n = occ_nelements(dst);
    for (int64_t i = 0; i < n; i++) {
        float v = pa[i];
        if (v < min_val) v = min_val;
        if (v > max_val) v = max_val;
        pd[i] = v;
    }
}

static void occ_compute_cont_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* a = dst->src[0];
    
    // Copy non-contiguous tensor to contiguous memory
    float* pd = (float*)dst->data;
    
    int64_t ne0 = a->ne[0];
    int64_t ne1 = a->ne[1];
    int64_t ne2 = a->ne[2];
    int64_t ne3 = a->ne[3];
    
    for (int64_t i3 = 0; i3 < ne3; i3++) {
        for (int64_t i2 = 0; i2 < ne2; i2++) {
            for (int64_t i1 = 0; i1 < ne1; i1++) {
                for (int64_t i0 = 0; i0 < ne0; i0++) {
                    *pd++ = *occ_get_f32_ptr(a, i0, i1, i2, i3);
                }
            }
        }
    }
}

// =============================================================================
// OCC-Specific Operations
// =============================================================================

static void occ_compute_spread_activation_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* activation = dst->src[0];
    const struct occ_tensor* adjacency = dst->src[1];
    
    const float* pa = (const float*)activation->data;
    const float* padj = (const float*)adjacency->data;
    float* pd = (float*)dst->data;
    
    int64_t n = activation->ne[0];
    
    // Parameters (would come from op_params in full implementation)
    float decay = 0.8f;
    float threshold = 0.01f;
    
    // spread = adjacency @ activation
    for (int64_t i = 0; i < n; i++) {
        float sum = 0.0f;
        for (int64_t j = 0; j < n; j++) {
            sum += padj[j + i * n] * pa[j];
        }
        
        // Apply decay and threshold
        sum *= decay;
        if (sum < threshold) sum = 0.0f;
        
        pd[i] = sum;
    }
}

static void occ_compute_truth_value_combine_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* tv1 = dst->src[0];
    const struct occ_tensor* tv2 = dst->src[1];
    
    const float* p1 = (const float*)tv1->data;
    const float* p2 = (const float*)tv2->data;
    float* pd = (float*)dst->data;
    
    int64_t n = tv1->ne[1];
    
    // PLN AND formula (simplified)
    // s = s1 * s2
    // c = c1 * c2 * s1 * s2
    for (int64_t i = 0; i < n; i++) {
        float s1 = p1[0 + i * 2];
        float c1 = p1[1 + i * 2];
        float s2 = p2[0 + i * 2];
        float c2 = p2[1 + i * 2];
        
        pd[0 + i * 2] = s1 * s2;
        pd[1 + i * 2] = c1 * c2 * s1 * s2;
    }
}

static void occ_compute_attention_update_scalar(struct occ_tensor* dst) {
    const struct occ_tensor* av = dst->src[0];
    const struct occ_tensor* activation = dst->src[1];
    
    const float* pav = (const float*)av->data;
    const float* pact = (const float*)activation->data;
    float* pd = (float*)dst->data;
    
    int64_t n = av->ne[1];
    
    // ECAN parameters (would come from op_params)
    float rent = 0.1f;
    float wage = 1.0f;
    
    for (int64_t i = 0; i < n; i++) {
        float sti = pav[0 + i * 3];
        float lti = pav[1 + i * 3];
        float vlti = pav[2 + i * 3];
        float act = pact[i];
        
        // Update STI: gain from activation, lose rent
        sti = sti - rent + act * wage;
        
        // Clamp STI
        if (sti < -1000.0f) sti = -1000.0f;
        if (sti > 1000.0f) sti = 1000.0f;
        
        // LTI slowly follows high STI
        if (sti > 100.0f) {
            lti += 0.01f * (sti - lti);
        }
        
        pd[0 + i * 3] = sti;
        pd[1 + i * 3] = lti;
        pd[2 + i * 3] = vlti;
    }
}

// =============================================================================
// Compute Dispatcher
// =============================================================================

void occ_compute(struct occ_cgraph* graph) {
    for (int i = 0; i < graph->n_nodes; i++) {
        struct occ_tensor* node = graph->nodes[i];
        
        switch (node->op) {
            case OCC_OP_ADD:
                #if OCC_USE_AVX2
                occ_compute_add_avx2(node);
                #else
                occ_compute_add_scalar(node);
                #endif
                break;
                
            case OCC_OP_SUB:
                #if OCC_USE_AVX2
                occ_compute_sub_avx2(node);
                #else
                occ_compute_sub_scalar(node);
                #endif
                break;
                
            case OCC_OP_MUL:
                #if OCC_USE_AVX2
                occ_compute_mul_avx2(node);
                #else
                occ_compute_mul_scalar(node);
                #endif
                break;
                
            case OCC_OP_DIV:
                #if OCC_USE_AVX2
                occ_compute_div_avx2(node);
                #else
                occ_compute_div_scalar(node);
                #endif
                break;
                
            case OCC_OP_MUL_MAT:
                #if OCC_USE_AVX2
                occ_compute_mul_mat_avx2(node);
                #else
                occ_compute_mul_mat_naive(node);
                #endif
                break;
                
            case OCC_OP_SIGMOID:
                occ_compute_sigmoid_scalar(node);
                break;
                
            case OCC_OP_TANH:
                occ_compute_tanh_scalar(node);
                break;
                
            case OCC_OP_RELU:
                occ_compute_relu_scalar(node);
                break;
                
            case OCC_OP_SILU:
                occ_compute_silu_scalar(node);
                break;
                
            case OCC_OP_SOFTMAX:
                occ_compute_softmax_scalar(node);
                break;
                
            case OCC_OP_L2_NORM:
                occ_compute_l2_norm_scalar(node);
                break;
                
            case OCC_OP_RMS_NORM:
                occ_compute_rms_norm_scalar(node);
                break;
                
            case OCC_OP_SUM:
                occ_compute_sum_scalar(node);
                break;
                
            case OCC_OP_SUM_ROWS:
                occ_compute_sum_rows_scalar(node);
                break;
                
            case OCC_OP_MEAN:
                occ_compute_mean_scalar(node);
                break;
                
            case OCC_OP_ARGMAX:
                occ_compute_argmax_scalar(node);
                break;
                
            case OCC_OP_GET_ROWS:
                occ_compute_get_rows_scalar(node);
                break;
                
            case OCC_OP_CLAMP:
                occ_compute_clamp_scalar(node);
                break;
                
            case OCC_OP_CONT:
                occ_compute_cont_scalar(node);
                break;
                
            case OCC_OP_SPREAD_ACTIVATION:
                occ_compute_spread_activation_scalar(node);
                break;
                
            case OCC_OP_TRUTH_VALUE_COMBINE:
                occ_compute_truth_value_combine_scalar(node);
                break;
                
            case OCC_OP_ATTENTION_UPDATE:
                occ_compute_attention_update_scalar(node);
                break;
                
            // View operations don't need computation
            case OCC_OP_RESHAPE:
            case OCC_OP_VIEW:
            case OCC_OP_PERMUTE:
            case OCC_OP_TRANSPOSE:
                break;
                
            default:
                fprintf(stderr, "OCC: Unknown operation %d\n", node->op);
                break;
        }
    }
}
