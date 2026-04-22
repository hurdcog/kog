#ifndef CROSS_MODAL_FUSION_H
#define CROSS_MODAL_FUSION_H

/**
 * Cross-Modal Cognitive Fusion Kernel
 * 
 * Implements unified cognitive processing by integrating:
 * - AtomSpace (Memory) - Hypergraph knowledge representation
 * - PLN (Reasoning) - Probabilistic logic inference
 * - MOSES (Learning) - Evolutionary program synthesis
 * 
 * Priority Feature from Autogenesis Engine:
 * "Integrate reasoning, learning, and memory into unified cognitive loops"
 * 
 * Performance Target: <5μs per fusion operation
 * Memory Target: <10MB per cognitive context
 */

#include "cognitive_kernels.h"
#include "tensor_signatures.h"
#include "atomspace_bridge.h"
#include <stdint.h>
#include <stdbool.h>

// Cognitive subsystem types
typedef enum {
    COGNITIVE_SUBSYSTEM_MEMORY,    // AtomSpace operations
    COGNITIVE_SUBSYSTEM_REASONING, // PLN inference
    COGNITIVE_SUBSYSTEM_LEARNING,  // MOSES evolution
    COGNITIVE_SUBSYSTEM_ATTENTION, // ECAN allocation
    COGNITIVE_SUBSYSTEM_COUNT
} cognitive_subsystem_t;

// Fusion strategy types
typedef enum {
    FUSION_STRATEGY_SEQUENTIAL,    // Process subsystems in sequence
    FUSION_STRATEGY_PARALLEL,      // Parallel processing with merge
    FUSION_STRATEGY_HIERARCHICAL,  // Hierarchical depth-wise fusion
    FUSION_STRATEGY_ADAPTIVE,      // Adaptive strategy selection
    FUSION_STRATEGY_COUNT
} fusion_strategy_t;

// Cross-modal fusion context
typedef struct {
    // Subsystem tensor states
    cognitive_tensor_t* memory_state;      // AtomSpace representation
    cognitive_tensor_t* reasoning_state;   // PLN inference state
    cognitive_tensor_t* learning_state;    // MOSES program state
    cognitive_tensor_t* attention_state;   // ECAN allocation state
    
    // Fusion parameters
    fusion_strategy_t strategy;
    float subsystem_weights[COGNITIVE_SUBSYSTEM_COUNT];
    uint32_t fusion_cycles;
    float convergence_threshold;
    
    // Performance metrics
    uint64_t total_fusions;
    uint64_t successful_fusions;
    uint64_t total_time_ns;
    float average_confidence;
    
    // Integration handles
    void* atomspace_handle;
    void* pln_engine_handle;
    void* moses_optimizer_handle;
    void* ecan_allocator_handle;
    
} cross_modal_fusion_context_t;

// Fusion result
typedef struct {
    cognitive_tensor_t* unified_representation;
    float confidence_score;
    uint64_t processing_time_ns;
    uint32_t subsystems_converged;
    bool convergence_achieved;
    
    // Subsystem contributions
    float memory_contribution;
    float reasoning_contribution;
    float learning_contribution;
    float attention_contribution;
    
    // Meta-learning feedback
    cognitive_tensor_t* feedback_tensor;
    
} fusion_result_t;

// Feedback loop result
typedef struct {
    cognitive_tensor_t* updated_reasoning_strategy;
    cognitive_tensor_t* evolved_program;
    float fitness_improvement;
    uint32_t learning_iterations;
    bool strategy_evolved;
} feedback_loop_result_t;

/**
 * Initialize cross-modal fusion context
 * 
 * @param strategy Fusion strategy to use
 * @param config Kernel configuration
 * @return Initialized fusion context or NULL on error
 */
cross_modal_fusion_context_t* fusion_context_init(
    fusion_strategy_t strategy,
    cognitive_kernel_config_t config
);

/**
 * Destroy fusion context and free resources
 */
void fusion_context_destroy(cross_modal_fusion_context_t* context);

/**
 * Create shared representation layer in AtomSpace
 * 
 * Converts cognitive tensors from different modalities into a unified
 * hypergraph representation that can be processed by all subsystems.
 * 
 * @param context Fusion context
 * @param input_tensors Array of tensors from different modalities
 * @param tensor_count Number of input tensors
 * @param config Kernel configuration
 * @return Cognitive result with unified representation
 */
cognitive_result_t fusion_create_shared_representation(
    cross_modal_fusion_context_t* context,
    cognitive_tensor_t** input_tensors,
    size_t tensor_count,
    cognitive_kernel_config_t config
);

/**
 * Implement feedback loop from PLN to MOSES
 * 
 * Takes reasoning outcomes from PLN and uses them to guide MOSES
 * evolutionary optimization. PLN inference results become fitness
 * signals for program evolution.
 * 
 * @param context Fusion context
 * @param reasoning_result PLN inference result tensor
 * @param current_program Current MOSES program representation
 * @param config Kernel configuration
 * @return Feedback loop result with evolved program
 */
feedback_loop_result_t fusion_pln_moses_feedback(
    cross_modal_fusion_context_t* context,
    cognitive_tensor_t* reasoning_result,
    cognitive_tensor_t* current_program,
    cognitive_kernel_config_t config
);

/**
 * Add meta-learning from reasoning outcomes
 * 
 * Analyzes patterns in PLN reasoning results to extract meta-level
 * insights about reasoning strategies and their effectiveness.
 * 
 * @param context Fusion context
 * @param reasoning_history Array of historical reasoning results
 * @param history_size Number of historical results
 * @param config Kernel configuration
 * @return Cognitive result with meta-learning insights
 */
cognitive_result_t fusion_meta_learning(
    cross_modal_fusion_context_t* context,
    cognitive_tensor_t** reasoning_history,
    size_t history_size,
    cognitive_kernel_config_t config
);

/**
 * Enable MOSES to evolve reasoning strategies
 * 
 * Allows MOSES to evolve the PLN reasoning strategies themselves,
 * creating a self-improving reasoning system.
 * 
 * @param context Fusion context
 * @param strategy_genome Current reasoning strategy representation
 * @param performance_data Historical performance metrics
 * @param config Kernel configuration
 * @return Cognitive result with evolved reasoning strategy
 */
cognitive_result_t fusion_evolve_reasoning_strategy(
    cross_modal_fusion_context_t* context,
    cognitive_tensor_t* strategy_genome,
    cognitive_tensor_t* performance_data,
    cognitive_kernel_config_t config
);

/**
 * Perform unified cognitive fusion
 * 
 * Main fusion operation that integrates all cognitive subsystems
 * according to the configured strategy.
 * 
 * @param context Fusion context
 * @param input Input tensor to process
 * @param config Kernel configuration
 * @return Fusion result with unified cognitive representation
 */
fusion_result_t fusion_unified_process(
    cross_modal_fusion_context_t* context,
    cognitive_tensor_t* input,
    cognitive_kernel_config_t config
);

/**
 * Update subsystem weights based on performance
 * 
 * Adaptively adjusts the contribution of each subsystem based on
 * their effectiveness in achieving cognitive goals.
 * 
 * @param context Fusion context
 * @param subsystem Subsystem to update
 * @param performance_delta Change in performance (-1.0 to 1.0)
 */
void fusion_update_subsystem_weight(
    cross_modal_fusion_context_t* context,
    cognitive_subsystem_t subsystem,
    float performance_delta
);

/**
 * Get fusion performance statistics
 */
void fusion_get_statistics(
    cross_modal_fusion_context_t* context,
    uint64_t* total_fusions,
    uint64_t* successful_fusions,
    float* average_confidence,
    float* average_latency_ms
);

/**
 * Reset fusion performance counters
 */
void fusion_reset_statistics(cross_modal_fusion_context_t* context);

/**
 * Free fusion result resources
 */
void fusion_result_destroy(fusion_result_t* result);

/**
 * Free feedback loop result resources
 */
void feedback_loop_result_destroy(feedback_loop_result_t* result);

#endif // CROSS_MODAL_FUSION_H
