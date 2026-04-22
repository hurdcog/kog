/**
 * Performance Benchmark for Cognitive Kernels
 * Measures throughput and latency of core operations
 */

#include "cognitive_kernels.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

// Get current time in microseconds
static uint64_t get_time_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000ULL + (uint64_t)tv.tv_usec;
}

// Benchmark cognitive convolution
void benchmark_convolution(size_t num_iterations) {
    printf("\n=== Benchmark: Cognitive Convolution ===\n");
    
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_VISUAL,
        .depth = DEPTH_PATTERN,
        .context = 256,
        .salience = SALIENCE_MEDIUM,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    float input_data[256];
    float kernel_data[16];
    
    for (size_t i = 0; i < 256; i++) {
        input_data[i] = (float)i / 256.0f;
    }
    for (size_t i = 0; i < 16; i++) {
        kernel_data[i] = 1.0f / 16.0f;
    }
    
    cognitive_tensor_t* input = create_cognitive_tensor(
        shape, TENSOR_TYPE_NEURAL, input_data, sizeof(input_data)
    );
    
    cognitive_tensor_shape_t kernel_shape = shape;
    kernel_shape.context = 16;
    
    cognitive_tensor_t* kernel = create_cognitive_tensor(
        kernel_shape, TENSOR_TYPE_NEURAL, kernel_data, sizeof(kernel_data)
    );
    
    cognitive_kernel_config_t config = {
        .operation = OP_COGNITIVE_CONV,
        .attention_threshold = 0.5f
    };
    
    uint64_t start = get_time_us();
    
    for (size_t i = 0; i < num_iterations; i++) {
        cognitive_result_t result = cognitive_convolution(input, kernel, config);
        destroy_cognitive_result(&result);
    }
    
    uint64_t end = get_time_us();
    uint64_t elapsed = end - start;
    
    double avg_latency_us = (double)elapsed / num_iterations;
    double ops_per_sec = 1000000.0 / avg_latency_us;
    
    printf("Iterations: %zu\n", num_iterations);
    printf("Total time: %.2f ms\n", elapsed / 1000.0);
    printf("Avg latency: %.2f us\n", avg_latency_us);
    printf("Throughput: %.2f ops/sec\n", ops_per_sec);
    
    cognitive_tensor_destroy(input);
    cognitive_tensor_destroy(kernel);
}

// Benchmark attention operations
void benchmark_attention(size_t num_iterations) {
    printf("\n=== Benchmark: Attention Operations ===\n");
    
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_CONCEPTUAL,
        .depth = DEPTH_PATTERN,
        .context = 512,
        .salience = SALIENCE_LOW,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    float data[512];
    for (size_t i = 0; i < 512; i++) {
        data[i] = (float)rand() / RAND_MAX;
    }
    
    cognitive_tensor_t* tensor = create_cognitive_tensor(
        shape, TENSOR_TYPE_ATTENTION, data, sizeof(data)
    );
    
    cognitive_kernel_config_t config = {
        .operation = OP_ATTENTION_POOL,
        .attention_threshold = 0.5f
    };
    
    uint64_t start = get_time_us();
    
    for (size_t i = 0; i < num_iterations; i++) {
        cognitive_result_t result = attention_pooling(
            tensor, SALIENCE_HIGH, config
        );
        destroy_cognitive_result(&result);
    }
    
    uint64_t end = get_time_us();
    uint64_t elapsed = end - start;
    
    double avg_latency_us = (double)elapsed / num_iterations;
    double ops_per_sec = 1000000.0 / avg_latency_us;
    
    printf("Iterations: %zu\n", num_iterations);
    printf("Total time: %.2f ms\n", elapsed / 1000.0);
    printf("Avg latency: %.2f us\n", avg_latency_us);
    printf("Throughput: %.2f ops/sec\n", ops_per_sec);
    
    cognitive_tensor_destroy(tensor);
}

// Benchmark symbolic operations
void benchmark_symbolic(size_t num_iterations) {
    printf("\n=== Benchmark: Symbolic Operations ===\n");
    
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_LINGUISTIC,
        .depth = DEPTH_ABSTRACTION,
        .context = 128,
        .salience = SALIENCE_HIGH,
        .autonomy_index = AUTONOMY_CREATIVE
    };
    
    float data[128];
    for (size_t i = 0; i < 128; i++) {
        data[i] = (float)i / 128.0f;
    }
    
    cognitive_tensor_t* tensor = create_cognitive_tensor(
        shape, TENSOR_TYPE_SYMBOLIC, data, sizeof(data)
    );
    
    cognitive_kernel_config_t config = {
        .operation = OP_SYMBOLIC_ACTIVATION,
        .attention_threshold = 0.5f
    };
    
    uint64_t start = get_time_us();
    
    for (size_t i = 0; i < num_iterations; i++) {
        cognitive_result_t result = symbolic_activation(tensor, config);
        destroy_cognitive_result(&result);
    }
    
    uint64_t end = get_time_us();
    uint64_t elapsed = end - start;
    
    double avg_latency_us = (double)elapsed / num_iterations;
    double ops_per_sec = 1000000.0 / avg_latency_us;
    
    printf("Iterations: %zu\n", num_iterations);
    printf("Total time: %.2f ms\n", elapsed / 1000.0);
    printf("Avg latency: %.2f us\n", avg_latency_us);
    printf("Throughput: %.2f ops/sec\n", ops_per_sec);
    
    cognitive_tensor_destroy(tensor);
}

// Benchmark recursive transformation
void benchmark_recursive(size_t num_iterations) {
    printf("\n=== Benchmark: Recursive Transformation ===\n");
    
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_META,
        .depth = DEPTH_RECURSIVE,
        .context = 64,
        .salience = SALIENCE_MEDIUM,
        .autonomy_index = AUTONOMY_CREATIVE
    };
    
    float data[64];
    for (size_t i = 0; i < 64; i++) {
        data[i] = 1.0f;
    }
    
    cognitive_tensor_t* tensor = create_cognitive_tensor(
        shape, TENSOR_TYPE_HYBRID, data, sizeof(data)
    );
    
    cognitive_kernel_config_t config = {
        .operation = OP_RECURSIVE_TRANSFORM,
        .recursive_depth_limit = 5
    };
    
    uint64_t start = get_time_us();
    
    for (size_t i = 0; i < num_iterations; i++) {
        cognitive_result_t result = recursive_transform(tensor, 3, config);
        destroy_cognitive_result(&result);
    }
    
    uint64_t end = get_time_us();
    uint64_t elapsed = end - start;
    
    double avg_latency_us = (double)elapsed / num_iterations;
    double ops_per_sec = 1000000.0 / avg_latency_us;
    
    printf("Iterations: %zu\n", num_iterations);
    printf("Total time: %.2f ms\n", elapsed / 1000.0);
    printf("Avg latency: %.2f us\n", avg_latency_us);
    printf("Throughput: %.2f ops/sec\n", ops_per_sec);
    
    cognitive_tensor_destroy(tensor);
}

// Benchmark memory management
void benchmark_memory(size_t num_iterations) {
    printf("\n=== Benchmark: Memory Management ===\n");
    
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_CONCEPTUAL,
        .depth = DEPTH_PATTERN,
        .context = 1024,
        .salience = SALIENCE_MEDIUM,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    float data[1024];
    for (size_t i = 0; i < 1024; i++) {
        data[i] = (float)i / 1024.0f;
    }
    
    uint64_t start = get_time_us();
    
    for (size_t i = 0; i < num_iterations; i++) {
        cognitive_tensor_t* tensor = create_cognitive_tensor(
            shape, TENSOR_TYPE_HYBRID, data, sizeof(data)
        );
        cognitive_tensor_destroy(tensor);
    }
    
    uint64_t end = get_time_us();
    uint64_t elapsed = end - start;
    
    double avg_latency_us = (double)elapsed / num_iterations;
    double ops_per_sec = 1000000.0 / avg_latency_us;
    
    printf("Iterations: %zu\n", num_iterations);
    printf("Total time: %.2f ms\n", elapsed / 1000.0);
    printf("Avg create/destroy: %.2f us\n", avg_latency_us);
    printf("Throughput: %.2f ops/sec\n", ops_per_sec);
    
    size_t allocated, freed, peak;
    cognitive_memory_stats(&allocated, &freed, &peak);
    printf("Peak memory: %.2f MB\n", peak / (1024.0 * 1024.0));
}

int main(int argc, char** argv) {
    size_t iterations = 1000;
    
    if (argc > 1) {
        iterations = atoi(argv[1]);
    }
    
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Cognitive Kernels Performance Benchmark                ║\n");
    printf("║  Phase 3: Neural-Symbolic Synthesis                     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    printf("\nIterations per benchmark: %zu\n", iterations);
    
    // Initialize
    cognitive_perf_config_t config = {
        .enable_gpu_acceleration = false,
        .enable_parallel_processing = true,
        .num_threads = 4,
        .memory_limit_bytes = 1024 * 1024 * 100,
        .attention_threshold = 0.5f
    };
    
    cognitive_kernels_init(&config);
    cognitive_reset_performance_stats();
    
    // Run benchmarks
    benchmark_convolution(iterations);
    benchmark_attention(iterations);
    benchmark_symbolic(iterations);
    benchmark_recursive(iterations);
    benchmark_memory(iterations);
    
    // Print overall statistics
    printf("\n=== Overall Performance Statistics ===\n");
    cognitive_performance_stats_t stats;
    cognitive_get_performance_stats(&stats);
    
    printf("Total operations: %lu\n", (unsigned long)stats.total_operations);
    printf("Total time: %.2f ms\n", stats.total_time_ns / 1000000.0);
    printf("Average latency: %.2f ms\n", stats.average_latency_ms);
    printf("Operations/sec: %.2f\n", stats.operations_per_second);
    printf("Peak memory: %.2f MB\n", stats.peak_memory_bytes / (1024.0 * 1024.0));
    
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Benchmark Complete                                      ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    
    // Cleanup
    cognitive_kernels_shutdown();
    
    return 0;
}
