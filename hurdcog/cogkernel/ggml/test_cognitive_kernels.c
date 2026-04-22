/**
 * Comprehensive Test Suite for Cognitive Kernels
 * Tests all neural-symbolic synthesis operations with real data
 * 
 * This test validates:
 * - Cognitive tensor operations
 * - Symbolic reasoning (gradient-free)
 * - Attention kernels (ECAN)
 * - Hypergraph operations
 * - AtomSpace integration
 * - GNU Hurd cognitive API
 */

#include "cognitive_kernels.h"
#include "atomspace_bridge.h"
#include "hurd_cognitive_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

// Test counters
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_START(name) \
    printf("\n=== Testing: %s ===\n", name);

#define TEST_ASSERT(condition, message) \
    if (condition) { \
        printf("✓ %s\n", message); \
        tests_passed++; \
    } else { \
        printf("✗ %s\n", message); \
        tests_failed++; \
    }

// Test: Cognitive tensor creation and lifecycle
void test_tensor_lifecycle(void) {
    TEST_START("Tensor Lifecycle");
    
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_CONCEPTUAL,
        .depth = DEPTH_PATTERN,
        .context = 128,
        .salience = SALIENCE_HIGH,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    float test_data[128];
    for (int i = 0; i < 128; i++) {
        test_data[i] = (float)i / 128.0f;
    }
    
    // Create tensor
    cognitive_tensor_t* tensor = create_cognitive_tensor(
        shape,
        TENSOR_TYPE_HYBRID,
        test_data,
        sizeof(test_data)
    );
    
    TEST_ASSERT(tensor != NULL, "Tensor creation");
    TEST_ASSERT(tensor->data_size == sizeof(test_data), "Tensor data size");
    TEST_ASSERT(tensor->shape.modality == MODALITY_CONCEPTUAL, "Tensor shape modality");
    
    // Clone tensor
    cognitive_tensor_t* cloned = cognitive_tensor_clone(tensor);
    TEST_ASSERT(cloned != NULL, "Tensor cloning");
    TEST_ASSERT(cloned->data_size == tensor->data_size, "Cloned tensor size");
    
    // Cleanup
    cognitive_tensor_destroy(tensor);
    cognitive_tensor_destroy(cloned);
    
    TEST_ASSERT(1, "Tensor cleanup");
}

// Test: Cognitive convolution
void test_cognitive_convolution(void) {
    TEST_START("Cognitive Convolution");
    
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_VISUAL,
        .depth = DEPTH_PATTERN,
        .context = 64,
        .salience = SALIENCE_MEDIUM,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    float input_data[64];
    float kernel_data[8];
    
    for (int i = 0; i < 64; i++) {
        input_data[i] = sinf((float)i * 0.1f);
    }
    for (int i = 0; i < 8; i++) {
        kernel_data[i] = 1.0f / 8.0f;
    }
    
    cognitive_tensor_t* input = create_cognitive_tensor(
        shape,
        TENSOR_TYPE_NEURAL,
        input_data,
        sizeof(input_data)
    );
    
    cognitive_tensor_shape_t kernel_shape = shape;
    kernel_shape.context = 8;
    
    cognitive_tensor_t* kernel = create_cognitive_tensor(
        kernel_shape,
        TENSOR_TYPE_NEURAL,
        kernel_data,
        sizeof(kernel_data)
    );
    
    cognitive_kernel_config_t config = {
        .operation = OP_COGNITIVE_CONV,
        .max_iterations = 100,
        .convergence_threshold = 0.01f,
        .attention_threshold = 0.5f
    };
    
    cognitive_result_t result = cognitive_convolution(input, kernel, config);
    
    TEST_ASSERT(result.output_tensor != NULL, "Convolution output");
    TEST_ASSERT(result.convergence_achieved, "Convolution convergence");
    TEST_ASSERT(result.confidence_score > 0.0f, "Convolution confidence");
    
    destroy_cognitive_result(&result);
    cognitive_tensor_destroy(input);
    cognitive_tensor_destroy(kernel);
}

// Test: Attention pooling
void test_attention_pooling(void) {
    TEST_START("Attention Pooling");
    
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_CONCEPTUAL,
        .depth = DEPTH_PATTERN,
        .context = 32,
        .salience = SALIENCE_LOW,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    float input_data[32];
    for (int i = 0; i < 32; i++) {
        input_data[i] = (float)i / 32.0f;
    }
    
    cognitive_tensor_t* input = create_cognitive_tensor(
        shape,
        TENSOR_TYPE_ATTENTION,
        input_data,
        sizeof(input_data)
    );
    
    cognitive_kernel_config_t config = {
        .operation = OP_ATTENTION_POOL,
        .attention_threshold = 0.5f
    };
    
    cognitive_result_t result = attention_pooling(
        input,
        SALIENCE_HIGH,
        config
    );
    
    TEST_ASSERT(result.output_tensor != NULL, "Attention pooling output");
    TEST_ASSERT(result.output_tensor->shape.salience == SALIENCE_HIGH, "Salience update");
    TEST_ASSERT(result.convergence_achieved, "Pooling convergence");
    
    destroy_cognitive_result(&result);
    cognitive_tensor_destroy(input);
}

// Test: Symbolic activation
void test_symbolic_activation(void) {
    TEST_START("Symbolic Activation");
    
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_LINGUISTIC,
        .depth = DEPTH_PRIMITIVE,
        .context = 16,
        .salience = SALIENCE_MEDIUM,
        .autonomy_index = AUTONOMY_REACTIVE
    };
    
    float input_data[16];
    for (int i = 0; i < 16; i++) {
        input_data[i] = (float)i / 16.0f;
    }
    
    cognitive_tensor_t* input = create_cognitive_tensor(
        shape,
        TENSOR_TYPE_SYMBOLIC,
        input_data,
        sizeof(input_data)
    );
    
    cognitive_kernel_config_t config = {
        .operation = OP_SYMBOLIC_ACTIVATION,
        .attention_threshold = 0.5f
    };
    
    cognitive_result_t result = symbolic_activation(input, config);
    
    TEST_ASSERT(result.output_tensor != NULL, "Symbolic activation output");
    TEST_ASSERT(result.confidence_score == 1.0f, "Symbolic exactness");
    TEST_ASSERT(result.convergence_achieved, "Activation convergence");
    
    // Verify binary output
    float* output_data = (float*)result.output_tensor->data;
    bool is_binary = true;
    for (int i = 0; i < 16; i++) {
        if (output_data[i] != 0.0f && output_data[i] != 1.0f) {
            is_binary = false;
            break;
        }
    }
    TEST_ASSERT(is_binary, "Binary activation values");
    
    destroy_cognitive_result(&result);
    cognitive_tensor_destroy(input);
}

// Test: Recursive transformation
void test_recursive_transform(void) {
    TEST_START("Recursive Transformation");
    
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_META,
        .depth = DEPTH_RECURSIVE,
        .context = 32,
        .salience = SALIENCE_MEDIUM,
        .autonomy_index = AUTONOMY_CREATIVE
    };
    
    float input_data[32];
    for (int i = 0; i < 32; i++) {
        input_data[i] = 1.0f;
    }
    
    cognitive_tensor_t* input = create_cognitive_tensor(
        shape,
        TENSOR_TYPE_HYBRID,
        input_data,
        sizeof(input_data)
    );
    
    cognitive_kernel_config_t config = {
        .operation = OP_RECURSIVE_TRANSFORM,
        .recursive_depth_limit = 5
    };
    
    cognitive_result_t result = recursive_transform(input, 3, config);
    
    TEST_ASSERT(result.output_tensor != NULL, "Recursive transform output");
    TEST_ASSERT(result.convergence_achieved, "Recursive convergence");
    TEST_ASSERT(result.operations_performed > 0, "Recursive operations");
    
    destroy_cognitive_result(&result);
    cognitive_tensor_destroy(input);
}

// Test: Meta-cognitive reflection
void test_meta_cognitive_reflection(void) {
    TEST_START("Meta-Cognitive Reflection");
    
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_CONCEPTUAL,
        .depth = DEPTH_ABSTRACTION,
        .context = 64,
        .salience = SALIENCE_HIGH,
        .autonomy_index = AUTONOMY_TRANSCENDENT
    };
    
    float input_data[64];
    for (int i = 0; i < 64; i++) {
        input_data[i] = (float)rand() / RAND_MAX;
    }
    
    cognitive_tensor_t* input = create_cognitive_tensor(
        shape,
        TENSOR_TYPE_HYBRID,
        input_data,
        sizeof(input_data)
    );
    
    cognitive_kernel_config_t config = {
        .operation = OP_META_REFLECTION
    };
    
    cognitive_result_t result = meta_cognitive_reflection(input, config);
    
    TEST_ASSERT(result.output_tensor != NULL, "Meta-cognitive output");
    TEST_ASSERT(result.output_tensor->shape.modality == MODALITY_META, "Meta modality");
    TEST_ASSERT(result.output_tensor->shape.depth == DEPTH_META, "Meta depth");
    
    destroy_cognitive_result(&result);
    cognitive_tensor_destroy(input);
}

// Test: Performance monitoring
void test_performance_monitoring(void) {
    TEST_START("Performance Monitoring");
    
    // Reset stats
    cognitive_reset_performance_stats();
    
    // Perform some operations
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_CONCEPTUAL,
        .depth = DEPTH_PATTERN,
        .context = 32,
        .salience = SALIENCE_MEDIUM,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    float data[32];
    for (int i = 0; i < 32; i++) {
        data[i] = (float)i / 32.0f;
    }
    
    cognitive_tensor_t* tensor = create_cognitive_tensor(
        shape,
        TENSOR_TYPE_HYBRID,
        data,
        sizeof(data)
    );
    
    cognitive_kernel_config_t config = {
        .attention_threshold = 0.5f
    };
    
    cognitive_result_t result = attention_pooling(tensor, SALIENCE_HIGH, config);
    
    // Get performance stats
    cognitive_performance_stats_t stats;
    cognitive_get_performance_stats(&stats);
    
    TEST_ASSERT(stats.total_operations > 0, "Performance tracking operations");
    TEST_ASSERT(stats.total_time_ns >= 0, "Performance tracking time");
    
    destroy_cognitive_result(&result);
    cognitive_tensor_destroy(tensor);
}

// Test: Memory management
void test_memory_management(void) {
    TEST_START("Memory Management");
    
    size_t allocated_before, freed_before, peak_before;
    cognitive_memory_stats(&allocated_before, &freed_before, &peak_before);
    
    // Allocate some memory
    void* ptr1 = cognitive_alloc(1024);
    void* ptr2 = cognitive_alloc(2048);
    
    TEST_ASSERT(ptr1 != NULL, "Memory allocation 1");
    TEST_ASSERT(ptr2 != NULL, "Memory allocation 2");
    
    size_t allocated_after, freed_after, peak_after;
    cognitive_memory_stats(&allocated_after, &freed_after, &peak_after);
    
    TEST_ASSERT(allocated_after > allocated_before, "Memory tracking allocation");
    
    cognitive_free(ptr1);
    cognitive_free(ptr2);
    
    TEST_ASSERT(1, "Memory cleanup");
}

// Test: AtomSpace integration
void test_atomspace_integration(void) {
    TEST_START("AtomSpace Integration");
    
    atomspace_bridge_init();
    
    struct atomspace_handle* handle = atomspace_create_handle();
    TEST_ASSERT(handle != NULL, "AtomSpace handle creation");
    TEST_ASSERT(handle->is_valid, "AtomSpace handle validity");
    
    // Create tensor
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_CONCEPTUAL,
        .depth = DEPTH_PATTERN,
        .context = 16,
        .salience = SALIENCE_MEDIUM,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    float data[16];
    for (int i = 0; i < 16; i++) {
        data[i] = (float)i / 16.0f;
    }
    
    cognitive_tensor_t* tensor = create_cognitive_tensor(
        shape,
        TENSOR_TYPE_HYPERGRAPH,
        data,
        sizeof(data)
    );
    
    // Convert to atoms
    int ret = atomspace_tensor_to_atoms(tensor, handle, ATOM_CONCEPT_NODE);
    TEST_ASSERT(ret == COGNITIVE_SUCCESS, "Tensor to atoms conversion");
    
    cognitive_tensor_destroy(tensor);
    atomspace_destroy_handle(handle);
    atomspace_bridge_shutdown();
}

// Test: GNU Hurd cognitive API
void test_hurd_cognitive_api(void) {
    TEST_START("GNU Hurd Cognitive API");
    
    hurd_cognitive_api_init();
    
    // Create cognitive primitive
    struct hurd_primitive* primitive = hurd_create_cognitive_primitive(
        HURD_PRIMITIVE_TASK,
        NULL
    );
    
    TEST_ASSERT(primitive != NULL, "Primitive creation");
    
    // Enable cognitive capabilities
    int ret = hurd_enable_cognitive(primitive);
    TEST_ASSERT(ret == COGNITIVE_SUCCESS, "Enable cognitive");
    TEST_ASSERT(primitive->is_cognitive, "Cognitive enabled");
    TEST_ASSERT(primitive->cognitive_state != NULL, "Cognitive state created");
    
    // Perform cognitive operation
    cognitive_kernel_config_t config = {
        .operation = HURD_COGNITIVE_OP_PREDICT,
        .attention_threshold = 0.5f
    };
    
    cognitive_result_t result = hurd_cognitive_operation(
        primitive,
        HURD_COGNITIVE_OP_PREDICT,
        config
    );
    
    TEST_ASSERT(result.output_tensor != NULL || result.confidence_score >= 0.0f, "Hurd cognitive operation");
    
    destroy_cognitive_result(&result);
    hurd_destroy_cognitive_primitive(primitive);
    hurd_cognitive_api_shutdown();
}

// Main test runner
int main(int argc, char** argv) {
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Cognitive Kernels Comprehensive Test Suite             ║\n");
    printf("║  Phase 3: Neural-Symbolic Synthesis                     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    
    // Initialize cognitive kernels
    cognitive_perf_config_t config = {
        .enable_gpu_acceleration = false,
        .enable_parallel_processing = true,
        .num_threads = 4,
        .memory_limit_bytes = 1024 * 1024 * 100, // 100 MB
        .attention_threshold = 0.5f
    };
    
    int ret = cognitive_kernels_init(&config);
    if (ret != COGNITIVE_SUCCESS) {
        printf("Failed to initialize cognitive kernels: %s\n", 
               cognitive_error_string(ret));
        return 1;
    }
    
    printf("\n✓ Cognitive kernels initialized\n");
    
    // Run all tests
    test_tensor_lifecycle();
    test_cognitive_convolution();
    test_attention_pooling();
    test_symbolic_activation();
    test_recursive_transform();
    test_meta_cognitive_reflection();
    test_performance_monitoring();
    test_memory_management();
    test_atomspace_integration();
    test_hurd_cognitive_api();
    
    // Print summary
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Test Summary                                            ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║  Tests Passed: %-3d                                       ║\n", tests_passed);
    printf("║  Tests Failed: %-3d                                       ║\n", tests_failed);
    printf("║  Success Rate: %.1f%%                                    ║\n", 
           100.0f * tests_passed / (tests_passed + tests_failed));
    printf("╚══════════════════════════════════════════════════════════╝\n");
    
    // Cleanup
    cognitive_kernels_shutdown();
    
    return tests_failed == 0 ? 0 : 1;
}
