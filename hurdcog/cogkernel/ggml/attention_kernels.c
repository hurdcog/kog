/**
 * Attention Kernels
 * ECAN (Economic Attention Networks) attention allocation optimization
 * 
 * Implements attention-based resource allocation and cognitive focus
 */

#include "cognitive_kernels.h"
#include "tensor_signatures.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// ECAN attention value structure
typedef struct {
    float short_term_importance;    // STI
    float long_term_importance;     // LTI
    float very_long_term_importance; // VLTI
} attention_value_t;

// Attention allocation context
typedef struct {
    float* attention_map;
    size_t map_size;
    float total_attention;
    float attention_decay_rate;
} attention_context_t;

// Internal: Calculate attention value
static float calculate_attention(attention_value_t av, float time_factor) {
    return av.short_term_importance * expf(-time_factor * 0.1f) +
           av.long_term_importance * 0.5f +
           av.very_long_term_importance * 0.3f;
}

// ECAN attention spread
cognitive_result_t ecan_attention_spread(
    cognitive_tensor_t* source,
    cognitive_tensor_t* targets,
    cognitive_kernel_config_t config
) {
    cognitive_result_t result = {0};
    
    if (!source || !targets) {
        result.confidence_score = 0.0f;
        result.convergence_achieved = false;
        return result;
    }
    
    // Create output tensor
    result.output_tensor = cognitive_tensor_clone(targets);
    if (!result.output_tensor) {
        result.confidence_score = 0.0f;
        result.convergence_achieved = false;
        return result;
    }
    
    float* source_data = (float*)source->data;
    float* output_data = (float*)result.output_tensor->data;
    
    size_t source_size = source->data_size / sizeof(float);
    size_t targets_size = result.output_tensor->data_size / sizeof(float);
    
    // Calculate total source attention
    float total_source_attention = 0.0f;
    for (size_t i = 0; i < source_size; i++) {
        total_source_attention += source_data[i];
    }
    
    // Spread attention to targets based on salience
    float source_salience = (float)source->shape.salience / 100.0f;
    
    for (size_t i = 0; i < targets_size; i++) {
        // Spread attention proportionally
        float spread_amount = (total_source_attention / targets_size) * source_salience;
        output_data[i] += spread_amount * config.attention_threshold;
        
        // Normalize to [0, 1]
        if (output_data[i] > 1.0f) {
            output_data[i] = 1.0f;
        }
    }
    
    result.confidence_score = 0.85f;
    result.processing_time_ns = 0;
    result.operations_performed = source_size + targets_size;
    result.convergence_achieved = true;
    result.debug_info = NULL;
    
    return result;
}

// Attention decay operation
cognitive_result_t attention_decay(
    cognitive_tensor_t* attention_tensor,
    float decay_rate,
    cognitive_kernel_config_t config
) {
    cognitive_result_t result = {0};
    
    if (!attention_tensor || decay_rate < 0.0f || decay_rate > 1.0f) {
        result.confidence_score = 0.0f;
        result.convergence_achieved = false;
        return result;
    }
    
    // Create output tensor
    result.output_tensor = cognitive_tensor_clone(attention_tensor);
    if (!result.output_tensor) {
        result.confidence_score = 0.0f;
        result.convergence_achieved = false;
        return result;
    }
    
    float* attention_data = (float*)result.output_tensor->data;
    size_t num_elements = result.output_tensor->data_size / sizeof(float);
    
    // Apply exponential decay to attention values
    for (size_t i = 0; i < num_elements; i++) {
        attention_data[i] *= (1.0f - decay_rate);
        
        // Remove attention values below threshold
        if (attention_data[i] < config.attention_threshold) {
            attention_data[i] = 0.0f;
        }
    }
    
    result.confidence_score = 1.0f;
    result.processing_time_ns = 0;
    result.operations_performed = num_elements;
    result.convergence_achieved = true;
    result.debug_info = NULL;
    
    return result;
}

// Attention focusing operation
cognitive_result_t attention_focus(
    cognitive_tensor_t* input,
    size_t focus_size,
    cognitive_kernel_config_t config
) {
    cognitive_result_t result = {0};
    
    if (!input || focus_size == 0) {
        result.confidence_score = 0.0f;
        result.convergence_achieved = false;
        return result;
    }
    
    float* input_data = (float*)input->data;
    size_t num_elements = input->data_size / sizeof(float);
    
    if (focus_size > num_elements) {
        focus_size = num_elements;
    }
    
    // Create array of indices sorted by attention value
    typedef struct {
        size_t index;
        float value;
    } indexed_value_t;
    
    indexed_value_t* sorted = cognitive_alloc(sizeof(indexed_value_t) * num_elements);
    if (!sorted) {
        result.confidence_score = 0.0f;
        result.convergence_achieved = false;
        return result;
    }
    
    for (size_t i = 0; i < num_elements; i++) {
        sorted[i].index = i;
        sorted[i].value = input_data[i];
    }
    
    // Simple selection sort for top k elements
    for (size_t i = 0; i < focus_size; i++) {
        size_t max_idx = i;
        for (size_t j = i + 1; j < num_elements; j++) {
            if (sorted[j].value > sorted[max_idx].value) {
                max_idx = j;
            }
        }
        
        // Swap
        if (max_idx != i) {
            indexed_value_t temp = sorted[i];
            sorted[i] = sorted[max_idx];
            sorted[max_idx] = temp;
        }
    }
    
    // Create focused output tensor
    result.output_tensor = create_cognitive_tensor(
        input->shape,
        TENSOR_TYPE_ATTENTION,
        NULL,
        input->data_size
    );
    
    if (!result.output_tensor) {
        cognitive_free(sorted);
        result.confidence_score = 0.0f;
        result.convergence_achieved = false;
        return result;
    }
    
    float* output_data = (float*)result.output_tensor->data;
    memset(output_data, 0, result.output_tensor->data_size);
    
    // Set focused elements
    for (size_t i = 0; i < focus_size; i++) {
        output_data[sorted[i].index] = sorted[i].value;
    }
    
    cognitive_free(sorted);
    
    result.confidence_score = 0.9f;
    result.processing_time_ns = 0;
    result.operations_performed = num_elements * focus_size;
    result.convergence_achieved = true;
    result.debug_info = NULL;
    
    return result;
}

// Attention normalization operation
cognitive_result_t attention_normalize(
    cognitive_tensor_t* attention_tensor,
    cognitive_kernel_config_t config
) {
    cognitive_result_t result = {0};
    
    if (!attention_tensor) {
        result.confidence_score = 0.0f;
        result.convergence_achieved = false;
        return result;
    }
    
    // Create output tensor
    result.output_tensor = cognitive_tensor_clone(attention_tensor);
    if (!result.output_tensor) {
        result.confidence_score = 0.0f;
        result.convergence_achieved = false;
        return result;
    }
    
    float* attention_data = (float*)result.output_tensor->data;
    size_t num_elements = result.output_tensor->data_size / sizeof(float);
    
    // Calculate total attention
    float total_attention = 0.0f;
    for (size_t i = 0; i < num_elements; i++) {
        total_attention += attention_data[i];
    }
    
    // Normalize to sum to 1.0
    if (total_attention > 0.0f) {
        for (size_t i = 0; i < num_elements; i++) {
            attention_data[i] /= total_attention;
        }
    }
    
    result.confidence_score = 1.0f;
    result.processing_time_ns = 0;
    result.operations_performed = num_elements * 2;
    result.convergence_achieved = true;
    result.debug_info = NULL;
    
    return result;
}

// Attention competition operation (winner-take-all)
cognitive_result_t attention_competition(
    cognitive_tensor_t* competitors,
    cognitive_kernel_config_t config
) {
    cognitive_result_t result = {0};
    
    if (!competitors) {
        result.confidence_score = 0.0f;
        result.convergence_achieved = false;
        return result;
    }
    
    float* competitors_data = (float*)competitors->data;
    size_t num_competitors = competitors->data_size / sizeof(float);
    
    // Find winner
    size_t winner_idx = 0;
    float max_attention = competitors_data[0];
    
    for (size_t i = 1; i < num_competitors; i++) {
        if (competitors_data[i] > max_attention) {
            max_attention = competitors_data[i];
            winner_idx = i;
        }
    }
    
    // Create output tensor with winner-take-all
    result.output_tensor = create_cognitive_tensor(
        competitors->shape,
        TENSOR_TYPE_ATTENTION,
        NULL,
        competitors->data_size
    );
    
    if (!result.output_tensor) {
        result.confidence_score = 0.0f;
        result.convergence_achieved = false;
        return result;
    }
    
    float* output_data = (float*)result.output_tensor->data;
    memset(output_data, 0, result.output_tensor->data_size);
    
    // Winner gets all attention
    output_data[winner_idx] = 1.0f;
    
    result.confidence_score = 1.0f;
    result.processing_time_ns = 0;
    result.operations_performed = num_competitors;
    result.convergence_achieved = true;
    result.debug_info = NULL;
    
    return result;
}

// Salience-based attention weighting
cognitive_result_t salience_weighting(
    cognitive_tensor_t* input,
    cognitive_salience_t* salience_map,
    size_t map_size,
    cognitive_kernel_config_t config
) {
    cognitive_result_t result = {0};
    
    if (!input || !salience_map) {
        result.confidence_score = 0.0f;
        result.convergence_achieved = false;
        return result;
    }
    
    // Create output tensor
    result.output_tensor = cognitive_tensor_clone(input);
    if (!result.output_tensor) {
        result.confidence_score = 0.0f;
        result.convergence_achieved = false;
        return result;
    }
    
    float* output_data = (float*)result.output_tensor->data;
    size_t num_elements = result.output_tensor->data_size / sizeof(float);
    
    // Apply salience weighting
    for (size_t i = 0; i < num_elements && i < map_size; i++) {
        float salience_weight = (float)salience_map[i] / 100.0f;
        output_data[i] *= salience_weight;
    }
    
    result.confidence_score = 0.95f;
    result.processing_time_ns = 0;
    result.operations_performed = num_elements;
    result.convergence_achieved = true;
    result.debug_info = NULL;
    
    return result;
}
