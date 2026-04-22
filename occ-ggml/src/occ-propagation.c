/**
 * OCC Echo Propagation Implementation
 * 
 * Spreading activation engine for Deep Tree Echo architecture.
 */

#define _GNU_SOURCE
#include "../include/occ-propagation.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <stdio.h>
#include <string.h>
#include <math.h>

// =============================================================================
// Propagation State Management
// =============================================================================

struct occ_propagation_state* occ_propagation_new(struct occ_atomspace* as) {
    struct occ_propagation_state* state = 
        (struct occ_propagation_state*)calloc(1, sizeof(struct occ_propagation_state));
    if (!state) return NULL;
    
    state->atomspace = as;
    state->ctx = as->ctx;
    
    int64_t n = as->max_atoms;
    
    // Allocate state tensors
    state->activation = occ_new_tensor_1d(state->ctx, OCC_TYPE_F32, n);
    state->prev_activation = occ_new_tensor_1d(state->ctx, OCC_TYPE_F32, n);
    state->hop_count = occ_new_tensor_1d(state->ctx, OCC_TYPE_I32, n);
    state->visited = occ_new_tensor_1d(state->ctx, OCC_TYPE_I32, n);
    
    occ_propagation_reset(state);
    
    return state;
}

void occ_propagation_free(struct occ_propagation_state* state) {
    if (!state) return;
    // Tensors are managed by context, just free the state struct
    free(state);
}

void occ_propagation_reset(struct occ_propagation_state* state) {
    memset(state->activation->data, 0, occ_nbytes(state->activation));
    memset(state->prev_activation->data, 0, occ_nbytes(state->prev_activation));
    memset(state->hop_count->data, 0, occ_nbytes(state->hop_count));
    memset(state->visited->data, 0, occ_nbytes(state->visited));
    
    state->current_hop = 0;
    state->total_iterations = 0;
    state->max_activation = 0.0f;
    state->n_active_atoms = 0;
    state->converged = false;
}

// =============================================================================
// Propagation Initialization
// =============================================================================

void occ_propagation_init(struct occ_propagation_state* state,
                           struct occ_tensor* source_ids,
                           struct occ_tensor* initial_activations) {
    occ_propagation_reset(state);
    
    float* act = (float*)state->activation->data;
    int32_t* hops = (int32_t*)state->hop_count->data;
    
    const int32_t* ids = (const int32_t*)source_ids->data;
    const float* init_act = (const float*)initial_activations->data;
    
    int64_t n_sources = occ_nelements(source_ids);
    
    for (int64_t i = 0; i < n_sources; i++) {
        int32_t id = ids[i];
        if (id >= 0 && id < state->atomspace->n_atoms) {
            act[id] = init_act[i];
            hops[id] = 0;
            
            if (init_act[i] > state->max_activation) {
                state->max_activation = init_act[i];
            }
        }
    }
    
    state->n_active_atoms = (int)n_sources;
}

// =============================================================================
// Single Propagation Step
// =============================================================================

bool occ_propagation_step(struct occ_propagation_state* state,
                           const struct occ_propagation_params* params) {
    if (state->current_hop >= params->max_hops) {
        return false;
    }
    
    struct occ_atomspace* as = state->atomspace;
    
    if (!as->adj_dense) {
        fprintf(stderr, "OCC: Propagation requires dense adjacency matrix\n");
        return false;
    }
    
    float* act = (float*)state->activation->data;
    float* prev_act = (float*)state->prev_activation->data;
    int32_t* hops = (int32_t*)state->hop_count->data;
    int32_t* visited = (int32_t*)state->visited->data;
    const float* adj = (const float*)as->adj_dense->data;
    const float* tv_strength = (const float*)as->tv_strength->data;
    const float* av_sti = (const float*)as->av_sti->data;
    
    int64_t n = as->n_atoms;
    
    // Save previous activation for convergence check
    memcpy(prev_act, act, n * sizeof(float));
    
    // Create new activation buffer
    float* new_act = (float*)calloc(n, sizeof(float));
    
    // Propagate activation
    for (int64_t i = 0; i < n; i++) {
        if (act[i] < params->threshold) continue;
        if (visited[i]) continue;
        
        // Mark as visited
        visited[i] = 1;
        
        // Propagate to neighbors
        for (int64_t j = 0; j < n; j++) {
            float edge_weight = adj[i + j * n];
            if (edge_weight < 1e-6f) continue;
            
            // Calculate propagation weight
            float weight = edge_weight;
            
            if (params->use_truth_values) {
                // Weight by link truth value (use source atom's TV as proxy)
                weight *= tv_strength[i];
            }
            
            if (params->use_attention) {
                // Weight by attention
                float sti_norm = (av_sti[i] + 1000.0f) / 2000.0f;  // Normalize to [0, 1]
                weight *= sti_norm;
            }
            
            // Propagate
            float propagated = act[i] * params->decay_rate * weight;
            new_act[j] += propagated;
            
            // Update hop count
            if (hops[j] == 0 || hops[j] > hops[i] + 1) {
                hops[j] = hops[i] + 1;
            }
        }
        
        // Bidirectional propagation
        if (params->bidirectional) {
            for (int64_t j = 0; j < n; j++) {
                float edge_weight = adj[j + i * n];  // Reverse direction
                if (edge_weight < 1e-6f) continue;
                
                float weight = edge_weight;
                if (params->use_truth_values) {
                    weight *= tv_strength[j];
                }
                
                float propagated = act[i] * params->decay_rate * weight;
                new_act[j] += propagated;
            }
        }
    }
    
    // Apply threshold and combine with existing activation
    state->n_active_atoms = 0;
    state->max_activation = 0.0f;
    
    for (int64_t i = 0; i < n; i++) {
        // Keep existing activation and add new
        act[i] = act[i] + new_act[i];
        
        // Apply threshold
        if (act[i] < params->threshold) {
            act[i] = 0.0f;
        }
        
        if (act[i] > 0.0f) {
            state->n_active_atoms++;
            if (act[i] > state->max_activation) {
                state->max_activation = act[i];
            }
        }
    }
    
    free(new_act);
    
    // Apply normalization
    if (params->normalization != OCC_NORM_NONE && state->n_active_atoms > 0) {
        switch (params->normalization) {
            case OCC_NORM_SUM: {
                float sum = 0.0f;
                for (int64_t i = 0; i < n; i++) sum += act[i];
                if (sum > 0.0f) {
                    for (int64_t i = 0; i < n; i++) act[i] /= sum;
                }
                break;
            }
            case OCC_NORM_MAX: {
                if (state->max_activation > 0.0f) {
                    for (int64_t i = 0; i < n; i++) act[i] /= state->max_activation;
                }
                break;
            }
            case OCC_NORM_L2: {
                float sum_sq = 0.0f;
                for (int64_t i = 0; i < n; i++) sum_sq += act[i] * act[i];
                float norm = sqrtf(sum_sq + 1e-6f);
                for (int64_t i = 0; i < n; i++) act[i] /= norm;
                break;
            }
            case OCC_NORM_SOFTMAX: {
                float max_val = state->max_activation;
                float sum_exp = 0.0f;
                for (int64_t i = 0; i < n; i++) {
                    if (act[i] > 0.0f) {
                        act[i] = expf(act[i] - max_val);
                        sum_exp += act[i];
                    }
                }
                if (sum_exp > 0.0f) {
                    for (int64_t i = 0; i < n; i++) act[i] /= sum_exp;
                }
                break;
            }
            default:
                break;
        }
    }
    
    // Check convergence
    float diff = 0.0f;
    for (int64_t i = 0; i < n; i++) {
        float d = act[i] - prev_act[i];
        diff += d * d;
    }
    diff = sqrtf(diff / n);
    
    if (diff < params->convergence_eps) {
        state->converged = true;
    }
    
    state->current_hop++;
    state->total_iterations++;
    
    return !state->converged && state->current_hop < params->max_hops;
}

// =============================================================================
// Run Propagation to Completion
// =============================================================================

int occ_propagation_run(struct occ_propagation_state* state,
                         const struct occ_propagation_params* params) {
    int iterations = 0;
    
    while (occ_propagation_step(state, params)) {
        iterations++;
        if (iterations >= params->max_iterations) {
            break;
        }
    }
    
    return iterations;
}

// =============================================================================
// Single-Source Propagation
// =============================================================================

struct occ_tensor* occ_propagate_from_atom(struct occ_propagation_state* state,
                                            int64_t source_id,
                                            float initial_activation,
                                            const struct occ_propagation_params* params) {
    // Create source tensors
    struct occ_tensor* source_ids = occ_new_tensor_1d(state->ctx, OCC_TYPE_I32, 1);
    struct occ_tensor* init_acts = occ_new_tensor_1d(state->ctx, OCC_TYPE_F32, 1);
    
    ((int32_t*)source_ids->data)[0] = (int32_t)source_id;
    ((float*)init_acts->data)[0] = initial_activation;
    
    occ_propagation_init(state, source_ids, init_acts);
    occ_propagation_run(state, params);
    
    return state->activation;
}

struct occ_tensor* occ_propagate_from_atoms(struct occ_propagation_state* state,
                                             struct occ_tensor* source_ids,
                                             struct occ_tensor* initial_activations,
                                             const struct occ_propagation_params* params) {
    occ_propagation_init(state, source_ids, initial_activations);
    occ_propagation_run(state, params);
    
    return state->activation;
}

// =============================================================================
// Result Extraction
// =============================================================================

int64_t occ_propagation_get_activated(const struct occ_propagation_state* state,
                                       float threshold,
                                       int64_t* out_ids,
                                       float* out_activations,
                                       int64_t max_results) {
    const float* act = (const float*)state->activation->data;
    int64_t n = state->atomspace->n_atoms;
    int64_t count = 0;
    
    for (int64_t i = 0; i < n && count < max_results; i++) {
        if (act[i] >= threshold) {
            out_ids[count] = i;
            out_activations[count] = act[i];
            count++;
        }
    }
    
    return count;
}

int64_t occ_propagation_get_top_k(const struct occ_propagation_state* state,
                                   int k,
                                   int64_t* out_ids,
                                   float* out_activations) {
    const float* act = (const float*)state->activation->data;
    int64_t n = state->atomspace->n_atoms;
    
    // Simple selection sort for top-k (efficient for small k)
    int64_t count = 0;
    
    // Temporary array to track selected indices
    bool* selected = (bool*)calloc(n, sizeof(bool));
    
    for (int i = 0; i < k; i++) {
        float max_val = -INFINITY;
        int64_t max_idx = -1;
        
        for (int64_t j = 0; j < n; j++) {
            if (!selected[j] && act[j] > max_val) {
                max_val = act[j];
                max_idx = j;
            }
        }
        
        if (max_idx >= 0 && max_val > 0.0f) {
            out_ids[count] = max_idx;
            out_activations[count] = max_val;
            selected[max_idx] = true;
            count++;
        } else {
            break;
        }
    }
    
    free(selected);
    return count;
}

float occ_propagation_get_activation(const struct occ_propagation_state* state, int64_t atom_id) {
    if (atom_id < 0 || atom_id >= state->atomspace->n_atoms) {
        return 0.0f;
    }
    return ((const float*)state->activation->data)[atom_id];
}

// =============================================================================
// ECAN Integration
// =============================================================================

void occ_propagation_update_attention(struct occ_propagation_state* state,
                                       float rent,
                                       float wage,
                                       float lti_threshold) {
    struct occ_atomspace* as = state->atomspace;
    const float* act = (const float*)state->activation->data;
    float* sti = (float*)as->av_sti->data;
    float* lti = (float*)as->av_lti->data;
    
    int64_t n = as->n_atoms;
    
    for (int64_t i = 0; i < n; i++) {
        // Update STI: gain from activation, lose rent
        sti[i] = sti[i] - rent + act[i] * wage;
        
        // Clamp STI
        if (sti[i] < -1000.0f) sti[i] = -1000.0f;
        if (sti[i] > 1000.0f) sti[i] = 1000.0f;
        
        // LTI slowly follows high STI
        if (sti[i] > lti_threshold) {
            lti[i] += 0.01f * (sti[i] - lti[i]);
        }
    }
}

// =============================================================================
// Oscillator Dynamics
// =============================================================================

// Extended state for oscillators
static struct {
    struct occ_tensor* phase;
    struct occ_tensor* amplitude;
    struct occ_tensor* frequency;
    bool initialized;
} g_oscillator_state = {NULL, NULL, NULL, false};

void occ_oscillator_init(struct occ_propagation_state* state,
                          const struct occ_oscillator_params* params) {
    int64_t n = state->atomspace->max_atoms;
    
    if (!g_oscillator_state.initialized) {
        g_oscillator_state.phase = occ_new_tensor_1d(state->ctx, OCC_TYPE_F32, n);
        g_oscillator_state.amplitude = occ_new_tensor_1d(state->ctx, OCC_TYPE_F32, n);
        g_oscillator_state.frequency = occ_new_tensor_1d(state->ctx, OCC_TYPE_F32, n);
        g_oscillator_state.initialized = true;
    }
    
    float* phase = (float*)g_oscillator_state.phase->data;
    float* amp = (float*)g_oscillator_state.amplitude->data;
    float* freq = (float*)g_oscillator_state.frequency->data;
    
    // Initialize with random phases and uniform frequency
    for (int64_t i = 0; i < n; i++) {
        phase[i] = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        amp[i] = 0.0f;
        freq[i] = params->frequency;
    }
}

void occ_oscillator_step(struct occ_propagation_state* state,
                          const struct occ_oscillator_params* params,
                          float dt) {
    if (!g_oscillator_state.initialized) return;
    
    struct occ_atomspace* as = state->atomspace;
    int64_t n = as->n_atoms;
    
    float* phase = (float*)g_oscillator_state.phase->data;
    float* amp = (float*)g_oscillator_state.amplitude->data;
    float* freq = (float*)g_oscillator_state.frequency->data;
    const float* act = (const float*)state->activation->data;
    const float* adj = as->adj_dense ? (const float*)as->adj_dense->data : NULL;
    
    // Temporary arrays for update
    float* new_phase = (float*)malloc(n * sizeof(float));
    float* new_amp = (float*)malloc(n * sizeof(float));
    
    for (int64_t i = 0; i < n; i++) {
        // Base phase evolution
        float dphase = freq[i] * dt;
        
        // Coupling from neighbors (Kuramoto-like)
        if (adj) {
            float coupling_sum = 0.0f;
            for (int64_t j = 0; j < n; j++) {
                float edge = adj[i + j * n];
                if (edge > 0.0f) {
                    coupling_sum += edge * sinf(phase[j] - phase[i]);
                }
            }
            dphase += params->coupling * coupling_sum * dt;
        }
        
        // Add noise
        float noise = ((float)rand() / RAND_MAX - 0.5f) * params->phase_noise;
        dphase += noise * dt;
        
        new_phase[i] = fmodf(phase[i] + dphase, 2.0f * M_PI);
        if (new_phase[i] < 0.0f) new_phase[i] += 2.0f * M_PI;
        
        // Amplitude follows activation with damping
        float target_amp = act[i];
        new_amp[i] = amp[i] + (target_amp - amp[i]) * params->damping * dt;
    }
    
    // Update state
    memcpy(phase, new_phase, n * sizeof(float));
    memcpy(amp, new_amp, n * sizeof(float));
    
    free(new_phase);
    free(new_amp);
}

struct occ_tensor* occ_oscillator_get_phase(struct occ_propagation_state* state) {
    return g_oscillator_state.phase;
}

struct occ_tensor* occ_oscillator_get_amplitude(struct occ_propagation_state* state) {
    return g_oscillator_state.amplitude;
}

int occ_oscillator_detect_clusters(struct occ_propagation_state* state,
                                    float sync_threshold,
                                    int32_t* out_cluster_ids) {
    if (!g_oscillator_state.initialized) return 0;
    
    int64_t n = state->atomspace->n_atoms;
    const float* phase = (const float*)g_oscillator_state.phase->data;
    const float* amp = (const float*)g_oscillator_state.amplitude->data;
    
    // Initialize all atoms as unassigned
    for (int64_t i = 0; i < n; i++) {
        out_cluster_ids[i] = -1;
    }
    
    int n_clusters = 0;
    
    // Simple clustering: group atoms with similar phase
    for (int64_t i = 0; i < n; i++) {
        if (amp[i] < 0.01f) continue;  // Skip inactive atoms
        if (out_cluster_ids[i] >= 0) continue;  // Already assigned
        
        // Start new cluster
        out_cluster_ids[i] = n_clusters;
        
        // Find all atoms synchronized with this one
        for (int64_t j = i + 1; j < n; j++) {
            if (amp[j] < 0.01f) continue;
            if (out_cluster_ids[j] >= 0) continue;
            
            // Check phase difference
            float phase_diff = fabsf(phase[i] - phase[j]);
            if (phase_diff > M_PI) phase_diff = 2.0f * M_PI - phase_diff;
            
            if (phase_diff < sync_threshold) {
                out_cluster_ids[j] = n_clusters;
            }
        }
        
        n_clusters++;
    }
    
    return n_clusters;
}
