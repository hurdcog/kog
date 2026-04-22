#ifndef COGNITIVE_KERNELS_H
#define COGNITIVE_KERNELS_H

/**
 * Cognitive Kernels - Main API Header
 * Custom ggml kernels for neural-symbolic synthesis in HurdCog
 * 
 * This header provides the main API for cognitive tensor operations,
 * neural-symbolic synthesis, and AtomSpace integration.
 */

#include "tensor_signatures.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Version information
#define COGNITIVE_KERNELS_VERSION_MAJOR 1
#define COGNITIVE_KERNELS_VERSION_MINOR 0
#define COGNITIVE_KERNELS_VERSION_PATCH 0

// Performance configuration
typedef struct {
    bool enable_gpu_acceleration;
    bool enable_parallel_processing;
    uint32_t num_threads;
    size_t memory_limit_bytes;
    float attention_threshold;
} cognitive_perf_config_t;

// Kernel initialization and cleanup
int cognitive_kernels_init(cognitive_perf_config_t* config);
void cognitive_kernels_shutdown(void);

// Memory management
void* cognitive_alloc(size_t size);
void cognitive_free(void* ptr);
void cognitive_memory_stats(size_t* allocated, size_t* freed, size_t* peak);

// Tensor lifecycle management
cognitive_tensor_t* cognitive_tensor_clone(cognitive_tensor_t* tensor);
void cognitive_tensor_destroy(cognitive_tensor_t* tensor);

// Performance monitoring
typedef struct {
    uint64_t total_operations;
    uint64_t total_time_ns;
    uint64_t peak_memory_bytes;
    float average_latency_ms;
    float operations_per_second;
} cognitive_performance_stats_t;

void cognitive_get_performance_stats(cognitive_performance_stats_t* stats);
void cognitive_reset_performance_stats(void);

// Error handling
typedef enum {
    COGNITIVE_SUCCESS = 0,
    COGNITIVE_ERROR_INVALID_PARAM = -1,
    COGNITIVE_ERROR_OUT_OF_MEMORY = -2,
    COGNITIVE_ERROR_SHAPE_MISMATCH = -3,
    COGNITIVE_ERROR_NOT_INITIALIZED = -4,
    COGNITIVE_ERROR_ATOMSPACE_INTEGRATION = -5,
    COGNITIVE_ERROR_COMPUTATION_FAILED = -6
} cognitive_error_t;

const char* cognitive_error_string(cognitive_error_t error);

// Additional attention operations (from attention_kernels.c)
cognitive_result_t attention_focus(
    cognitive_tensor_t* input,
    size_t focus_size,
    cognitive_kernel_config_t config
);

cognitive_result_t attention_normalize(
    cognitive_tensor_t* attention_tensor,
    cognitive_kernel_config_t config
);

#endif // COGNITIVE_KERNELS_H
