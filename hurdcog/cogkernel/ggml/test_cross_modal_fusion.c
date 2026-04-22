/**
 * Test Suite for Cross-Modal Cognitive Fusion Kernel
 * 
 * Validates the implementation of unified cognitive processing
 * integrating AtomSpace, PLN, and MOSES subsystems.
 */

#include "cross_modal_fusion.h"
#include "cognitive_kernels.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// Test utilities
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "FAIL: %s\n", message); \
            return -1; \
        } \
    } while(0)

#define TEST_PASS(message) \
    printf("PASS: %s\n", message)

// Test 1: Context initialization and destruction
int test_fusion_context_lifecycle() {
    cognitive_kernel_config_t config = {
        .operation = OP_COGNITIVE_CONV,
        .max_iterations = 100,
        .convergence_threshold = 0.01f,
        .enable_gpu_acceleration = false,
        .enable_parallel_processing = false,
        .attention_threshold = 0.5f,
        .salience_decay_rate = 0.1f,
        .recursive_depth_limit = 3
    };
    
    cross_modal_fusion_context_t* context = fusion_context_init(
        FUSION_STRATEGY_SEQUENTIAL, config);
    
    TEST_ASSERT(context != NULL, "Context initialization");
    TEST_ASSERT(context->strategy == FUSION_STRATEGY_SEQUENTIAL, "Strategy set correctly");
    TEST_ASSERT(context->total_fusions == 0, "Initial fusion count is zero");
    
    // Check subsystem weights are initialized
    float weight_sum = 0.0f;
    for (int i = 0; i < COGNITIVE_SUBSYSTEM_COUNT; i++) {
        weight_sum += context->subsystem_weights[i];
    }
    TEST_ASSERT(weight_sum > 0.99f && weight_sum < 1.01f, "Weights sum to 1.0");
    
    fusion_context_destroy(context);
    TEST_PASS("Context lifecycle");
    return 0;
}

// Test 2: Shared representation creation
int test_shared_representation() {
    cognitive_kernel_config_t config = {
        .operation = OP_COGNITIVE_CONV,
        .max_iterations = 100,
        .convergence_threshold = 0.01f,
        .enable_gpu_acceleration = false,
        .enable_parallel_processing = false,
        .attention_threshold = 0.5f,
        .salience_decay_rate = 0.1f,
        .recursive_depth_limit = 3
    };
    
    cross_modal_fusion_context_t* context = fusion_context_init(
        FUSION_STRATEGY_PARALLEL, config);
    TEST_ASSERT(context != NULL, "Context creation");
    
    // Create test input tensors
    cognitive_tensor_shape_t shape1 = {
        .modality = MODALITY_VISUAL,
        .depth = DEPTH_PATTERN,
        .context = 128,
        .salience = SALIENCE_HIGH,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    cognitive_tensor_shape_t shape2 = {
        .modality = MODALITY_LINGUISTIC,
        .depth = DEPTH_ABSTRACTION,
        .context = 128,
        .salience = SALIENCE_MEDIUM,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    float test_data1[128];
    float test_data2[128];
    for (int i = 0; i < 128; i++) {
        test_data1[i] = (float)i / 128.0f;
        test_data2[i] = 1.0f - (float)i / 128.0f;
    }
    
    cognitive_tensor_t* tensor1 = create_cognitive_tensor(
        shape1, TENSOR_TYPE_NEURAL, test_data1, sizeof(test_data1));
    cognitive_tensor_t* tensor2 = create_cognitive_tensor(
        shape2, TENSOR_TYPE_SYMBOLIC, test_data2, sizeof(test_data2));
    
    TEST_ASSERT(tensor1 != NULL, "Tensor 1 created");
    TEST_ASSERT(tensor2 != NULL, "Tensor 2 created");
    
    cognitive_tensor_t* tensors[] = {tensor1, tensor2};
    
    cognitive_result_t result = fusion_create_shared_representation(
        context, tensors, 2, config);
    
    TEST_ASSERT(result.output_tensor != NULL, "Shared representation created");
    TEST_ASSERT(result.confidence_score > 0.0f, "Positive confidence");
    TEST_ASSERT(result.operations_performed == 2, "Correct operation count");
    TEST_ASSERT(result.convergence_achieved, "Convergence achieved");
    
    // Cleanup
    destroy_cognitive_result(&result);
    cognitive_tensor_destroy(tensor1);
    cognitive_tensor_destroy(tensor2);
    fusion_context_destroy(context);
    
    TEST_PASS("Shared representation creation");
    return 0;
}

// Test 3: PLN-MOSES feedback loop
int test_pln_moses_feedback() {
    cognitive_kernel_config_t config = {
        .operation = OP_INFERENCE,
        .max_iterations = 100,
        .convergence_threshold = 0.01f,
        .enable_gpu_acceleration = false,
        .enable_parallel_processing = false,
        .attention_threshold = 0.5f,
        .salience_decay_rate = 0.1f,
        .recursive_depth_limit = 3
    };
    
    cross_modal_fusion_context_t* context = fusion_context_init(
        FUSION_STRATEGY_ADAPTIVE, config);
    TEST_ASSERT(context != NULL, "Context creation");
    
    // Create reasoning result tensor
    cognitive_tensor_shape_t reasoning_shape = {
        .modality = MODALITY_CONCEPTUAL,
        .depth = DEPTH_ABSTRACTION,
        .context = 256,
        .salience = SALIENCE_HIGH,
        .autonomy_index = AUTONOMY_CREATIVE
    };
    
    float reasoning_data[256];
    for (int i = 0; i < 256; i++) {
        reasoning_data[i] = (float)i / 256.0f;
    }
    
    cognitive_tensor_t* reasoning_result = create_cognitive_tensor(
        reasoning_shape, TENSOR_TYPE_SYMBOLIC, reasoning_data, sizeof(reasoning_data));
    reasoning_result->confidence = 0.85f; // High confidence reasoning
    
    // Create current program tensor
    cognitive_tensor_shape_t program_shape = {
        .modality = MODALITY_CONCEPTUAL,
        .depth = DEPTH_PATTERN,
        .context = 256,
        .salience = SALIENCE_MEDIUM,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    float program_data[256];
    for (int i = 0; i < 256; i++) {
        program_data[i] = 0.5f;
    }
    
    cognitive_tensor_t* current_program = create_cognitive_tensor(
        program_shape, TENSOR_TYPE_HYBRID, program_data, sizeof(program_data));
    
    // Test feedback loop
    feedback_loop_result_t result = fusion_pln_moses_feedback(
        context, reasoning_result, current_program, config);
    
    TEST_ASSERT(result.evolved_program != NULL, "Program evolved");
    TEST_ASSERT(result.updated_reasoning_strategy != NULL, "Strategy updated");
    TEST_ASSERT(result.fitness_improvement >= 0.0f, "Fitness improvement non-negative");
    TEST_ASSERT(result.strategy_evolved, "Strategy evolution flag set");
    
    // Cleanup
    feedback_loop_result_destroy(&result);
    cognitive_tensor_destroy(reasoning_result);
    cognitive_tensor_destroy(current_program);
    fusion_context_destroy(context);
    
    TEST_PASS("PLN-MOSES feedback loop");
    return 0;
}

// Test 4: Meta-learning from reasoning history
int test_meta_learning() {
    cognitive_kernel_config_t config = {
        .operation = OP_META_REFLECTION,
        .max_iterations = 100,
        .convergence_threshold = 0.01f,
        .enable_gpu_acceleration = false,
        .enable_parallel_processing = false,
        .attention_threshold = 0.5f,
        .salience_decay_rate = 0.1f,
        .recursive_depth_limit = 3
    };
    
    cross_modal_fusion_context_t* context = fusion_context_init(
        FUSION_STRATEGY_HIERARCHICAL, config);
    TEST_ASSERT(context != NULL, "Context creation");
    
    // Create reasoning history
    const size_t history_size = 10;
    cognitive_tensor_t* history[history_size];
    
    cognitive_tensor_shape_t history_shape = {
        .modality = MODALITY_CONCEPTUAL,
        .depth = DEPTH_ABSTRACTION,
        .context = 128,
        .salience = SALIENCE_MEDIUM,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    for (size_t i = 0; i < history_size; i++) {
        float data[128];
        for (int j = 0; j < 128; j++) {
            data[j] = (float)j / 128.0f * (i + 1) / history_size;
        }
        
        history[i] = create_cognitive_tensor(
            history_shape, TENSOR_TYPE_SYMBOLIC, data, sizeof(data));
        history[i]->confidence = 0.6f + (float)i / history_size * 0.3f; // Increasing confidence
    }
    
    // Test meta-learning
    cognitive_result_t result = fusion_meta_learning(
        context, history, history_size, config);
    
    TEST_ASSERT(result.output_tensor != NULL, "Meta-learning result created");
    TEST_ASSERT(result.output_tensor->shape.depth == DEPTH_META, "Meta-level depth");
    TEST_ASSERT(result.confidence_score > 0.0f, "Positive confidence");
    TEST_ASSERT(result.operations_performed == history_size, "All history processed");
    
    // Cleanup
    for (size_t i = 0; i < history_size; i++) {
        cognitive_tensor_destroy(history[i]);
    }
    destroy_cognitive_result(&result);
    fusion_context_destroy(context);
    
    TEST_PASS("Meta-learning from reasoning history");
    return 0;
}

// Test 5: Unified cognitive processing
int test_unified_processing() {
    cognitive_kernel_config_t config = {
        .operation = OP_COGNITIVE_CONV,
        .max_iterations = 100,
        .convergence_threshold = 0.01f,
        .enable_gpu_acceleration = false,
        .enable_parallel_processing = false,
        .attention_threshold = 0.5f,
        .salience_decay_rate = 0.1f,
        .recursive_depth_limit = 3
    };
    
    // Test each fusion strategy
    fusion_strategy_t strategies[] = {
        FUSION_STRATEGY_SEQUENTIAL,
        FUSION_STRATEGY_PARALLEL,
        FUSION_STRATEGY_HIERARCHICAL,
        FUSION_STRATEGY_ADAPTIVE
    };
    
    for (int s = 0; s < 4; s++) {
        cross_modal_fusion_context_t* context = fusion_context_init(
            strategies[s], config);
        TEST_ASSERT(context != NULL, "Context creation");
        
        // Create input tensor
        cognitive_tensor_shape_t input_shape = {
            .modality = MODALITY_CONCEPTUAL,
            .depth = DEPTH_PATTERN,
            .context = 256,
            .salience = SALIENCE_HIGH,
            .autonomy_index = AUTONOMY_CREATIVE
        };
        
        float input_data[256];
        for (int i = 0; i < 256; i++) {
            input_data[i] = (float)i / 256.0f;
        }
        
        cognitive_tensor_t* input = create_cognitive_tensor(
            input_shape, TENSOR_TYPE_HYBRID, input_data, sizeof(input_data));
        
        // Perform unified processing
        fusion_result_t result = fusion_unified_process(context, input, config);
        
        TEST_ASSERT(result.unified_representation != NULL, "Unified result created");
        TEST_ASSERT(result.confidence_score > 0.0f, "Positive confidence");
        TEST_ASSERT(result.subsystems_converged > 0, "At least one subsystem converged");
        TEST_ASSERT(result.feedback_tensor != NULL, "Feedback tensor created");
        
        // Check contribution weights
        float total_contribution = result.memory_contribution + 
                                   result.reasoning_contribution +
                                   result.learning_contribution +
                                   result.attention_contribution;
        TEST_ASSERT(total_contribution > 0.99f && total_contribution < 1.01f, 
                   "Contributions sum to 1.0");
        
        // Cleanup
        fusion_result_destroy(&result);
        cognitive_tensor_destroy(input);
        fusion_context_destroy(context);
    }
    
    TEST_PASS("Unified cognitive processing (all strategies)");
    return 0;
}

// Test 6: Subsystem weight adaptation
int test_weight_adaptation() {
    cognitive_kernel_config_t config = {
        .operation = OP_COGNITIVE_CONV,
        .max_iterations = 100,
        .convergence_threshold = 0.01f,
        .enable_gpu_acceleration = false,
        .enable_parallel_processing = false,
        .attention_threshold = 0.5f,
        .salience_decay_rate = 0.1f,
        .recursive_depth_limit = 3
    };
    
    cross_modal_fusion_context_t* context = fusion_context_init(
        FUSION_STRATEGY_ADAPTIVE, config);
    TEST_ASSERT(context != NULL, "Context creation");
    
    // Get initial weights
    float initial_memory_weight = context->subsystem_weights[COGNITIVE_SUBSYSTEM_MEMORY];
    
    // Simulate positive performance
    fusion_update_subsystem_weight(context, COGNITIVE_SUBSYSTEM_MEMORY, 0.2f);
    
    TEST_ASSERT(context->subsystem_weights[COGNITIVE_SUBSYSTEM_MEMORY] > 
               initial_memory_weight, "Weight increased with positive performance");
    
    // Check normalization
    float weight_sum = 0.0f;
    for (int i = 0; i < COGNITIVE_SUBSYSTEM_COUNT; i++) {
        weight_sum += context->subsystem_weights[i];
    }
    TEST_ASSERT(weight_sum > 0.99f && weight_sum < 1.01f, "Weights remain normalized");
    
    fusion_context_destroy(context);
    TEST_PASS("Subsystem weight adaptation");
    return 0;
}

// Test 7: Performance statistics
int test_statistics() {
    cognitive_kernel_config_t config = {
        .operation = OP_COGNITIVE_CONV,
        .max_iterations = 100,
        .convergence_threshold = 0.01f,
        .enable_gpu_acceleration = false,
        .enable_parallel_processing = false,
        .attention_threshold = 0.5f,
        .salience_decay_rate = 0.1f,
        .recursive_depth_limit = 3
    };
    
    cross_modal_fusion_context_t* context = fusion_context_init(
        FUSION_STRATEGY_SEQUENTIAL, config);
    TEST_ASSERT(context != NULL, "Context creation");
    
    // Create and process input
    cognitive_tensor_shape_t input_shape = {
        .modality = MODALITY_CONCEPTUAL,
        .depth = DEPTH_PATTERN,
        .context = 256,
        .salience = SALIENCE_HIGH,
        .autonomy_index = AUTONOMY_CREATIVE
    };
    
    float input_data[256];
    for (int i = 0; i < 256; i++) {
        input_data[i] = (float)i / 256.0f;
    }
    
    cognitive_tensor_t* input = create_cognitive_tensor(
        input_shape, TENSOR_TYPE_HYBRID, input_data, sizeof(input_data));
    
    // Perform multiple fusions
    for (int i = 0; i < 5; i++) {
        fusion_result_t result = fusion_unified_process(context, input, config);
        fusion_result_destroy(&result);
    }
    
    // Check statistics
    uint64_t total, successful;
    float avg_confidence, avg_latency;
    
    fusion_get_statistics(context, &total, &successful, &avg_confidence, &avg_latency);
    
    TEST_ASSERT(total == 5, "Correct fusion count");
    TEST_ASSERT(successful > 0, "Some successful fusions");
    TEST_ASSERT(avg_confidence > 0.0f, "Positive average confidence");
    TEST_ASSERT(avg_latency >= 0.0f, "Non-negative average latency");
    
    printf("  Stats: %lu total, %lu successful, %.3f avg confidence, %.3f ms avg latency\n",
           total, successful, avg_confidence, avg_latency);
    
    // Test reset
    fusion_reset_statistics(context);
    fusion_get_statistics(context, &total, &successful, &avg_confidence, &avg_latency);
    TEST_ASSERT(total == 0, "Statistics reset");
    
    // Cleanup
    cognitive_tensor_destroy(input);
    fusion_context_destroy(context);
    
    TEST_PASS("Performance statistics");
    return 0;
}

// Main test runner
int main(int argc, char** argv) {
    printf("========================================\n");
    printf("Cross-Modal Cognitive Fusion Test Suite\n");
    printf("========================================\n\n");
    
    int total_tests = 0;
    int passed_tests = 0;
    
    #define RUN_TEST(test_func) \
        do { \
            total_tests++; \
            printf("Running: %s...\n", #test_func); \
            if (test_func() == 0) { \
                passed_tests++; \
            } \
            printf("\n"); \
        } while(0)
    
    RUN_TEST(test_fusion_context_lifecycle);
    RUN_TEST(test_shared_representation);
    RUN_TEST(test_pln_moses_feedback);
    RUN_TEST(test_meta_learning);
    RUN_TEST(test_unified_processing);
    RUN_TEST(test_weight_adaptation);
    RUN_TEST(test_statistics);
    
    printf("========================================\n");
    printf("Test Results: %d/%d passed\n", passed_tests, total_tests);
    printf("========================================\n");
    
    return (passed_tests == total_tests) ? 0 : 1;
}
