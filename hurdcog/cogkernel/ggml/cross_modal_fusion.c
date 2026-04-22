// Define _POSIX_C_SOURCE for clock_gettime
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

/**
 * Cross-Modal Cognitive Fusion Kernel - Implementation
 * 
 * Implements unified cognitive processing using GGML tensor operations
 * Priority: HIGH | Complexity: MEDIUM | Feature: unified_cognitive_processing
 * 
 * Performance: Target <5μs per operation, Actual: TBD
 * Memory: Target <10MB per context, Actual: TBD
 */

#include "cross_modal_fusion.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>  // For getpid()

// Forward declarations of external functions
extern void cognitive_tensor_destroy(cognitive_tensor_t* tensor);

// Internal helper functions
static uint64_t get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

static float calculate_tensor_similarity(
    cognitive_tensor_t* t1,
    cognitive_tensor_t* t2
) {
    if (!t1 || !t2 || !t1->data || !t2->data) return 0.0f;
    if (t1->data_size != t2->data_size) return 0.0f;
    
    // Simple cosine similarity for demonstration
    float dot_product = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    float* data1 = (float*)t1->data;
    float* data2 = (float*)t2->data;
    size_t float_count = t1->data_size / sizeof(float);
    
    for (size_t i = 0; i < float_count; i++) {
        dot_product += data1[i] * data2[i];
        norm1 += data1[i] * data1[i];
        norm2 += data2[i] * data2[i];
    }
    
    if (norm1 == 0.0f || norm2 == 0.0f) return 0.0f;
    return dot_product / (sqrtf(norm1) * sqrtf(norm2));
}

/**
 * Initialize cross-modal fusion context
 */
cross_modal_fusion_context_t* fusion_context_init(
    fusion_strategy_t strategy,
    cognitive_kernel_config_t config
) {
    cross_modal_fusion_context_t* context = 
        (cross_modal_fusion_context_t*)calloc(1, sizeof(cross_modal_fusion_context_t));
    
    if (!context) return NULL;
    
    // Initialize random number generator for evolutionary operations
    // TODO: Consider using cryptographically secure RNG for production
    srand((unsigned int)time(NULL) ^ (unsigned int)getpid());
    
    // Initialize fusion parameters
    context->strategy = strategy;
    context->convergence_threshold = config.convergence_threshold;
    context->fusion_cycles = 0;
    
    // Initialize subsystem weights (equal initially)
    for (int i = 0; i < COGNITIVE_SUBSYSTEM_COUNT; i++) {
        context->subsystem_weights[i] = 1.0f / COGNITIVE_SUBSYSTEM_COUNT;
    }
    
    // Initialize performance metrics
    context->total_fusions = 0;
    context->successful_fusions = 0;
    context->total_time_ns = 0;
    context->average_confidence = 0.0f;
    
    // Create initial tensor states for each subsystem
    cognitive_tensor_shape_t shape = {
        .modality = MODALITY_CONCEPTUAL,
        .depth = DEPTH_ABSTRACTION,
        .context = 512,
        .salience = SALIENCE_MEDIUM,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    // Allocate subsystem states
    context->memory_state = create_cognitive_tensor(
        shape, TENSOR_TYPE_HYPERGRAPH, NULL, 0);
    context->reasoning_state = create_cognitive_tensor(
        shape, TENSOR_TYPE_SYMBOLIC, NULL, 0);
    context->learning_state = create_cognitive_tensor(
        shape, TENSOR_TYPE_HYBRID, NULL, 0);
    context->attention_state = create_cognitive_tensor(
        shape, TENSOR_TYPE_ATTENTION, NULL, 0);
    
    return context;
}

/**
 * Destroy fusion context
 */
void fusion_context_destroy(cross_modal_fusion_context_t* context) {
    if (!context) return;
    
    // Destroy subsystem tensor states
    if (context->memory_state) 
        cognitive_tensor_destroy(context->memory_state);
    if (context->reasoning_state) 
        cognitive_tensor_destroy(context->reasoning_state);
    if (context->learning_state) 
        cognitive_tensor_destroy(context->learning_state);
    if (context->attention_state) 
        cognitive_tensor_destroy(context->attention_state);
    
    free(context);
}

/**
 * Create shared representation layer in AtomSpace
 */
cognitive_result_t fusion_create_shared_representation(
    cross_modal_fusion_context_t* context,
    cognitive_tensor_t** input_tensors,
    size_t tensor_count,
    cognitive_kernel_config_t config
) {
    uint64_t start_time = get_time_ns();
    cognitive_result_t result = {0};
    
    if (!context || !input_tensors || tensor_count == 0) {
        result.confidence_score = 0.0f;
        return result;
    }
    
    // Create unified hypergraph representation
    cognitive_tensor_shape_t unified_shape = {
        .modality = MODALITY_CONCEPTUAL,
        .depth = DEPTH_ABSTRACTION,
        .context = 1024, // Larger context for fusion
        .salience = SALIENCE_HIGH,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    // Allocate unified tensor
    size_t unified_size = sizeof(float) * unified_shape.context;
    float* unified_data = (float*)calloc(unified_size, 1);
    
    // Merge input tensors using weighted average
    for (size_t t = 0; t < tensor_count; t++) {
        if (!input_tensors[t] || !input_tensors[t]->data) continue;
        
        float* tensor_data = (float*)input_tensors[t]->data;
        size_t tensor_float_count = input_tensors[t]->data_size / sizeof(float);
        
        // Weighted contribution based on tensor salience
        float weight = (float)input_tensors[t]->shape.salience / 100.0f;
        
        for (size_t i = 0; i < tensor_float_count && i < unified_shape.context; i++) {
            unified_data[i] += tensor_data[i] * weight / tensor_count;
        }
    }
    
    // Create result tensor
    result.output_tensor = create_cognitive_tensor(
        unified_shape,
        TENSOR_TYPE_HYPERGRAPH,
        unified_data,
        unified_size
    );
    
    free(unified_data);
    
    result.confidence_score = 0.85f; // Base confidence for shared representation
    result.processing_time_ns = get_time_ns() - start_time;
    result.operations_performed = tensor_count;
    result.convergence_achieved = true;
    
    return result;
}

/**
 * Implement feedback loop from PLN to MOSES
 */
feedback_loop_result_t fusion_pln_moses_feedback(
    cross_modal_fusion_context_t* context,
    cognitive_tensor_t* reasoning_result,
    cognitive_tensor_t* current_program,
    cognitive_kernel_config_t config
) {
    feedback_loop_result_t result = {0};
    
    if (!context || !reasoning_result || !current_program) {
        return result;
    }
    
    // Extract PLN reasoning confidence as fitness signal
    float reasoning_confidence = reasoning_result->confidence;
    
    // Use reasoning result to guide program evolution
    // In real implementation, this would interface with MOSES
    cognitive_tensor_shape_t evolved_shape = current_program->shape;
    evolved_shape.depth = DEPTH_META; // Elevate to meta-level
    
    // Simulate program evolution based on reasoning feedback
    size_t program_size = current_program->data_size;
    void* evolved_data = malloc(program_size);
    if (evolved_data) {
        memcpy(evolved_data, current_program->data, program_size);
        
        // Apply mutations based on reasoning confidence
        // NOTE: Using rand() for demonstration. Production systems should use
        // cryptographically secure RNG or more sophisticated mutation operators
        float* program_floats = (float*)evolved_data;
        size_t float_count = program_size / sizeof(float);
        
        for (size_t i = 0; i < float_count; i++) {
            // Higher confidence = smaller mutations
            float mutation_rate = 0.1f * (1.0f - reasoning_confidence);
            float random_delta = (float)rand() / RAND_MAX - 0.5f;
            program_floats[i] *= (1.0f + mutation_rate * random_delta);
        }
        
        result.evolved_program = create_cognitive_tensor(
            evolved_shape,
            TENSOR_TYPE_HYBRID,
            evolved_data,
            program_size
        );
        
        free(evolved_data);
    }
    
    // Calculate fitness improvement
    result.fitness_improvement = reasoning_confidence * 0.1f; // Incremental improvement
    result.learning_iterations = 1;
    result.strategy_evolved = true;
    
    // Create updated reasoning strategy tensor
    cognitive_tensor_shape_t strategy_shape = {
        .modality = MODALITY_META,
        .depth = DEPTH_META,
        .context = 256,
        .salience = SALIENCE_HIGH,
        .autonomy_index = AUTONOMY_CREATIVE
    };
    
    result.updated_reasoning_strategy = create_cognitive_tensor(
        strategy_shape,
        TENSOR_TYPE_SYMBOLIC,
        NULL,
        0
    );
    
    return result;
}

/**
 * Add meta-learning from reasoning outcomes
 */
cognitive_result_t fusion_meta_learning(
    cross_modal_fusion_context_t* context,
    cognitive_tensor_t** reasoning_history,
    size_t history_size,
    cognitive_kernel_config_t config
) {
    uint64_t start_time = get_time_ns();
    cognitive_result_t result = {0};
    
    if (!context || !reasoning_history || history_size == 0) {
        return result;
    }
    
    // Analyze patterns in reasoning history
    float average_confidence = 0.0f;
    float confidence_variance = 0.0f;
    
    for (size_t i = 0; i < history_size; i++) {
        if (reasoning_history[i]) {
            average_confidence += reasoning_history[i]->confidence;
        }
    }
    average_confidence /= history_size;
    
    // Calculate variance
    for (size_t i = 0; i < history_size; i++) {
        if (reasoning_history[i]) {
            float diff = reasoning_history[i]->confidence - average_confidence;
            confidence_variance += diff * diff;
        }
    }
    confidence_variance /= history_size;
    
    // Create meta-learning insight tensor
    cognitive_tensor_shape_t meta_shape = {
        .modality = MODALITY_META,
        .depth = DEPTH_META,
        .context = 512,
        .salience = SALIENCE_HIGH,
        .autonomy_index = AUTONOMY_CREATIVE
    };
    
    // Encode meta-insights
    size_t meta_size = sizeof(float) * meta_shape.context;
    float* meta_data = (float*)calloc(meta_size, 1);
    
    if (meta_data) {
        // Store aggregated statistics
        meta_data[0] = average_confidence;
        meta_data[1] = confidence_variance;
        meta_data[2] = (float)history_size;
        
        // Detect patterns in reasoning success
        for (size_t i = 1; i < history_size && i < meta_shape.context - 3; i++) {
            if (reasoning_history[i] && reasoning_history[i-1]) {
                // Confidence trend
                meta_data[i + 3] = reasoning_history[i]->confidence - 
                                   reasoning_history[i-1]->confidence;
            }
        }
        
        result.output_tensor = create_cognitive_tensor(
            meta_shape,
            TENSOR_TYPE_META,
            meta_data,
            meta_size
        );
        
        free(meta_data);
    }
    
    result.confidence_score = average_confidence;
    result.processing_time_ns = get_time_ns() - start_time;
    result.operations_performed = history_size;
    result.convergence_achieved = (confidence_variance < config.convergence_threshold);
    
    return result;
}

/**
 * Enable MOSES to evolve reasoning strategies
 */
cognitive_result_t fusion_evolve_reasoning_strategy(
    cross_modal_fusion_context_t* context,
    cognitive_tensor_t* strategy_genome,
    cognitive_tensor_t* performance_data,
    cognitive_kernel_config_t config
) {
    uint64_t start_time = get_time_ns();
    cognitive_result_t result = {0};
    
    if (!context || !strategy_genome || !performance_data) {
        return result;
    }
    
    // Evolve strategy genome using MOSES-like evolutionary algorithm
    cognitive_tensor_shape_t evolved_shape = strategy_genome->shape;
    evolved_shape.depth = DEPTH_RECURSIVE; // Recursive strategy evolution
    
    size_t genome_size = strategy_genome->data_size;
    void* evolved_genome = malloc(genome_size);
    
    if (evolved_genome) {
        memcpy(evolved_genome, strategy_genome->data, genome_size);
        
        // Apply evolutionary operators
        // NOTE: Using rand() for demonstration. Production systems should use
        // more sophisticated evolutionary algorithms with proper diversity maintenance
        float* genome_floats = (float*)evolved_genome;
        float* performance_floats = (float*)performance_data->data;
        size_t float_count = genome_size / sizeof(float);
        size_t perf_count = performance_data->data_size / sizeof(float);
        
        // Fitness-guided evolution
        for (size_t i = 0; i < float_count; i++) {
            // Use performance data as fitness landscape
            float fitness = (i < perf_count) ? performance_floats[i] : 0.5f;
            
            // Higher fitness = preservation, lower fitness = mutation
            if (fitness < 0.5f) {
                float random_delta = (float)rand() / RAND_MAX * 0.2f - 0.1f;
                genome_floats[i] += random_delta;
            }
        }
        
        result.output_tensor = create_cognitive_tensor(
            evolved_shape,
            TENSOR_TYPE_SYMBOLIC,
            evolved_genome,
            genome_size
        );
        
        free(evolved_genome);
    }
    
    result.confidence_score = 0.75f; // Base confidence for evolved strategy
    result.processing_time_ns = get_time_ns() - start_time;
    result.operations_performed = config.max_iterations;
    result.convergence_achieved = true;
    
    return result;
}

/**
 * Perform unified cognitive fusion
 */
fusion_result_t fusion_unified_process(
    cross_modal_fusion_context_t* context,
    cognitive_tensor_t* input,
    cognitive_kernel_config_t config
) {
    uint64_t start_time = get_time_ns();
    fusion_result_t result = {0};
    
    if (!context || !input) {
        return result;
    }
    
    // Update subsystem states with input
    cognitive_result_t memory_result = {0};
    cognitive_result_t reasoning_result = {0};
    cognitive_result_t learning_result = {0};
    cognitive_result_t attention_result = {0};
    
    // Process through each subsystem
    switch (context->strategy) {
        case FUSION_STRATEGY_SEQUENTIAL:
            // Sequential processing: Memory -> Reasoning -> Learning -> Attention
            memory_result = cognitive_convolution(input, context->memory_state, config);
            if (memory_result.output_tensor) {
                reasoning_result = symbolic_activation(memory_result.output_tensor, config);
            }
            if (reasoning_result.output_tensor) {
                learning_result = recursive_transform(
                    reasoning_result.output_tensor, config.recursive_depth_limit, config);
            }
            if (learning_result.output_tensor) {
                attention_result = attention_pooling(
                    learning_result.output_tensor, SALIENCE_HIGH, config);
            }
            break;
            
        case FUSION_STRATEGY_PARALLEL:
            // Parallel processing with merge
            memory_result = cognitive_convolution(input, context->memory_state, config);
            reasoning_result = symbolic_activation(input, config);
            learning_result = recursive_transform(input, config.recursive_depth_limit, config);
            attention_result = attention_pooling(input, SALIENCE_HIGH, config);
            break;
            
        case FUSION_STRATEGY_HIERARCHICAL:
        case FUSION_STRATEGY_ADAPTIVE:
        case FUSION_STRATEGY_COUNT:
        default:
            // Hierarchical depth-wise fusion
            memory_result = cognitive_convolution(input, context->memory_state, config);
            reasoning_result = symbolic_activation(input, config);
            learning_result = recursive_transform(input, config.recursive_depth_limit, config);
            attention_result = attention_pooling(input, SALIENCE_HIGH, config);
            break;
    }
    
    // Merge subsystem results
    cognitive_tensor_t* subsystem_results[COGNITIVE_SUBSYSTEM_COUNT];
    subsystem_results[COGNITIVE_SUBSYSTEM_MEMORY] = memory_result.output_tensor;
    subsystem_results[COGNITIVE_SUBSYSTEM_REASONING] = reasoning_result.output_tensor;
    subsystem_results[COGNITIVE_SUBSYSTEM_LEARNING] = learning_result.output_tensor;
    subsystem_results[COGNITIVE_SUBSYSTEM_ATTENTION] = attention_result.output_tensor;
    
    // Create unified representation
    cognitive_result_t unified = fusion_create_shared_representation(
        context, subsystem_results, COGNITIVE_SUBSYSTEM_COUNT, config);
    
    result.unified_representation = unified.output_tensor;
    result.processing_time_ns = get_time_ns() - start_time;
    
    // Calculate subsystem contributions (weighted)
    result.memory_contribution = context->subsystem_weights[COGNITIVE_SUBSYSTEM_MEMORY];
    result.reasoning_contribution = context->subsystem_weights[COGNITIVE_SUBSYSTEM_REASONING];
    result.learning_contribution = context->subsystem_weights[COGNITIVE_SUBSYSTEM_LEARNING];
    result.attention_contribution = context->subsystem_weights[COGNITIVE_SUBSYSTEM_ATTENTION];
    
    // Calculate overall confidence
    uint32_t converged = 0;
    float total_confidence = 0.0f;
    
    if (memory_result.convergence_achieved) {
        converged++;
        total_confidence += memory_result.confidence_score * result.memory_contribution;
    }
    if (reasoning_result.convergence_achieved) {
        converged++;
        total_confidence += reasoning_result.confidence_score * result.reasoning_contribution;
    }
    if (learning_result.convergence_achieved) {
        converged++;
        total_confidence += learning_result.confidence_score * result.learning_contribution;
    }
    if (attention_result.convergence_achieved) {
        converged++;
        total_confidence += attention_result.confidence_score * result.attention_contribution;
    }
    
    result.subsystems_converged = converged;
    
    // Calculate overall confidence with division by zero protection
    if (converged > 0) {
        result.confidence_score = total_confidence / converged;
    } else {
        // No subsystems converged, use minimum confidence
        result.confidence_score = 0.1f;
    }
    result.convergence_achieved = (converged == COGNITIVE_SUBSYSTEM_COUNT);
    
    // Update context statistics
    context->total_fusions++;
    if (result.convergence_achieved) {
        context->successful_fusions++;
    }
    context->total_time_ns += result.processing_time_ns;
    context->average_confidence = 
        (context->average_confidence * (context->total_fusions - 1) + result.confidence_score) 
        / context->total_fusions;
    
    // Create feedback tensor for meta-learning
    cognitive_tensor_shape_t feedback_shape = {
        .modality = MODALITY_META,
        .depth = DEPTH_META,
        .context = 128,
        .salience = SALIENCE_MEDIUM,
        .autonomy_index = AUTONOMY_ADAPTIVE
    };
    
    result.feedback_tensor = create_cognitive_tensor(
        feedback_shape, TENSOR_TYPE_META, NULL, 0);
    
    // Cleanup subsystem results
    destroy_cognitive_result(&memory_result);
    destroy_cognitive_result(&reasoning_result);
    destroy_cognitive_result(&learning_result);
    destroy_cognitive_result(&attention_result);
    
    return result;
}

/**
 * Update subsystem weights based on performance
 */
void fusion_update_subsystem_weight(
    cross_modal_fusion_context_t* context,
    cognitive_subsystem_t subsystem,
    float performance_delta
) {
    if (!context || subsystem >= COGNITIVE_SUBSYSTEM_COUNT) return;
    
    // Update weight with learning rate of 0.1
    float learning_rate = 0.1f;
    context->subsystem_weights[subsystem] += learning_rate * performance_delta;
    
    // Clamp to [0, 1]
    if (context->subsystem_weights[subsystem] < 0.0f) {
        context->subsystem_weights[subsystem] = 0.0f;
    }
    if (context->subsystem_weights[subsystem] > 1.0f) {
        context->subsystem_weights[subsystem] = 1.0f;
    }
    
    // Renormalize all weights
    float sum = 0.0f;
    for (int i = 0; i < COGNITIVE_SUBSYSTEM_COUNT; i++) {
        sum += context->subsystem_weights[i];
    }
    
    if (sum > 0.0f) {
        for (int i = 0; i < COGNITIVE_SUBSYSTEM_COUNT; i++) {
            context->subsystem_weights[i] /= sum;
        }
    }
}

/**
 * Get fusion performance statistics
 */
void fusion_get_statistics(
    cross_modal_fusion_context_t* context,
    uint64_t* total_fusions,
    uint64_t* successful_fusions,
    float* average_confidence,
    float* average_latency_ms
) {
    if (!context) return;
    
    if (total_fusions) *total_fusions = context->total_fusions;
    if (successful_fusions) *successful_fusions = context->successful_fusions;
    if (average_confidence) *average_confidence = context->average_confidence;
    
    if (average_latency_ms && context->total_fusions > 0) {
        *average_latency_ms = (float)context->total_time_ns / 
                             (context->total_fusions * 1000000.0f);
    }
}

/**
 * Reset fusion performance counters
 */
void fusion_reset_statistics(cross_modal_fusion_context_t* context) {
    if (!context) return;
    
    context->total_fusions = 0;
    context->successful_fusions = 0;
    context->total_time_ns = 0;
    context->average_confidence = 0.0f;
}

/**
 * Free fusion result resources
 */
void fusion_result_destroy(fusion_result_t* result) {
    if (!result) return;
    
    if (result->unified_representation) {
        cognitive_tensor_destroy(result->unified_representation);
    }
    if (result->feedback_tensor) {
        cognitive_tensor_destroy(result->feedback_tensor);
    }
}

/**
 * Free feedback loop result resources
 */
void feedback_loop_result_destroy(feedback_loop_result_t* result) {
    if (!result) return;
    
    if (result->updated_reasoning_strategy) {
        cognitive_tensor_destroy(result->updated_reasoning_strategy);
    }
    if (result->evolved_program) {
        cognitive_tensor_destroy(result->evolved_program);
    }
}
