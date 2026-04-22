#ifndef OCC_PROPAGATION_H
#define OCC_PROPAGATION_H

/**
 * OCC Echo Propagation: Spreading Activation Engine
 * 
 * Implements the Deep Tree Echo spreading activation algorithm
 * using efficient tensor operations for batch processing.
 */

#include "occ-ggml.h"
#include "occ-atomspace.h"

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// Propagation Parameters
// =============================================================================

/**
 * Parameters controlling spreading activation behavior
 */
struct occ_propagation_params {
    float decay_rate;       // Activation decay per hop (0.0 to 1.0)
    float threshold;        // Minimum activation to propagate
    int max_hops;           // Maximum propagation distance
    
    // Normalization
    enum {
        OCC_NORM_NONE = 0,
        OCC_NORM_SUM,       // Normalize by sum
        OCC_NORM_MAX,       // Normalize by max
        OCC_NORM_SOFTMAX,   // Softmax normalization
        OCC_NORM_L2         // L2 normalization
    } normalization;
    
    // Direction
    bool bidirectional;     // Propagate in both directions
    
    // Weighting
    bool use_truth_values;  // Weight by link truth values
    bool use_attention;     // Weight by attention values
    
    // Convergence
    float convergence_eps;  // Convergence threshold
    int max_iterations;     // Maximum iterations for convergence
};

/**
 * Default propagation parameters
 */
static const struct occ_propagation_params OCC_DEFAULT_PROPAGATION_PARAMS = {
    .decay_rate = 0.8f,
    .threshold = 0.01f,
    .max_hops = 5,
    .normalization = OCC_NORM_NONE,
    .bidirectional = true,
    .use_truth_values = true,
    .use_attention = false,
    .convergence_eps = 1e-6f,
    .max_iterations = 100
};

// =============================================================================
// Propagation State
// =============================================================================

/**
 * State for iterative propagation
 */
struct occ_propagation_state {
    struct occ_context* ctx;
    struct occ_atomspace* atomspace;
    
    // Current state
    struct occ_tensor* activation;       // [n_atoms] F32
    struct occ_tensor* prev_activation;  // [n_atoms] F32 (for convergence)
    struct occ_tensor* hop_count;        // [n_atoms] I32
    struct occ_tensor* visited;          // [n_atoms] I32 (boolean)
    
    // Statistics
    int current_hop;
    int total_iterations;
    float max_activation;
    int n_active_atoms;
    bool converged;
};

// =============================================================================
// Propagation Engine
// =============================================================================

/**
 * Create propagation state
 * 
 * @param as AtomSpace to propagate through
 * @return New propagation state
 */
struct occ_propagation_state* occ_propagation_new(struct occ_atomspace* as);

/**
 * Free propagation state
 */
void occ_propagation_free(struct occ_propagation_state* state);

/**
 * Reset propagation state
 */
void occ_propagation_reset(struct occ_propagation_state* state);

// =============================================================================
// Single-Source Propagation
// =============================================================================

/**
 * Propagate activation from a single source atom
 * 
 * @param state Propagation state
 * @param source_id Source atom ID
 * @param initial_activation Initial activation value
 * @param params Propagation parameters
 * @return Activation tensor [n_atoms]
 */
struct occ_tensor* occ_propagate_from_atom(struct occ_propagation_state* state,
                                            int64_t source_id,
                                            float initial_activation,
                                            const struct occ_propagation_params* params);

// =============================================================================
// Multi-Source Propagation
// =============================================================================

/**
 * Propagate activation from multiple source atoms
 * 
 * @param state Propagation state
 * @param source_ids Source atom IDs [n_sources]
 * @param initial_activations Initial activations [n_sources]
 * @param params Propagation parameters
 * @return Activation tensor [n_atoms]
 */
struct occ_tensor* occ_propagate_from_atoms(struct occ_propagation_state* state,
                                             struct occ_tensor* source_ids,
                                             struct occ_tensor* initial_activations,
                                             const struct occ_propagation_params* params);

// =============================================================================
// Iterative Propagation
// =============================================================================

/**
 * Initialize propagation with sources
 * 
 * @param state Propagation state
 * @param source_ids Source atom IDs [n_sources]
 * @param initial_activations Initial activations [n_sources]
 */
void occ_propagation_init(struct occ_propagation_state* state,
                           struct occ_tensor* source_ids,
                           struct occ_tensor* initial_activations);

/**
 * Perform one propagation step
 * 
 * @param state Propagation state
 * @param params Propagation parameters
 * @return true if more steps needed, false if converged or max hops reached
 */
bool occ_propagation_step(struct occ_propagation_state* state,
                           const struct occ_propagation_params* params);

/**
 * Run propagation to completion
 * 
 * @param state Propagation state
 * @param params Propagation parameters
 * @return Number of iterations performed
 */
int occ_propagation_run(struct occ_propagation_state* state,
                         const struct occ_propagation_params* params);

// =============================================================================
// Result Extraction
// =============================================================================

/**
 * Get atoms with activation above threshold
 * 
 * @param state Propagation state
 * @param threshold Activation threshold
 * @param out_ids Output array for atom IDs
 * @param out_activations Output array for activations
 * @param max_results Maximum results
 * @return Number of results
 */
int64_t occ_propagation_get_activated(const struct occ_propagation_state* state,
                                       float threshold,
                                       int64_t* out_ids,
                                       float* out_activations,
                                       int64_t max_results);

/**
 * Get top-k activated atoms
 * 
 * @param state Propagation state
 * @param k Number of top atoms
 * @param out_ids Output array for atom IDs
 * @param out_activations Output array for activations
 * @return Number of results (may be less than k)
 */
int64_t occ_propagation_get_top_k(const struct occ_propagation_state* state,
                                   int k,
                                   int64_t* out_ids,
                                   float* out_activations);

/**
 * Get activation for specific atom
 */
float occ_propagation_get_activation(const struct occ_propagation_state* state, int64_t atom_id);

// =============================================================================
// Specialized Propagation Patterns
// =============================================================================

/**
 * Inheritance-based spreading (follows InheritanceLinks)
 * 
 * Propagates activation upward through inheritance hierarchy.
 * Useful for concept generalization.
 */
struct occ_tensor* occ_propagate_inheritance(struct occ_propagation_state* state,
                                              int64_t source_id,
                                              float initial_activation,
                                              bool upward,  // true=generalize, false=specialize
                                              const struct occ_propagation_params* params);

/**
 * Similarity-based spreading (follows SimilarityLinks)
 * 
 * Propagates activation to similar concepts.
 * Useful for associative retrieval.
 */
struct occ_tensor* occ_propagate_similarity(struct occ_propagation_state* state,
                                             int64_t source_id,
                                             float initial_activation,
                                             const struct occ_propagation_params* params);

/**
 * Implication-based spreading (follows ImplicationLinks)
 * 
 * Propagates activation through logical implications.
 * Useful for forward chaining inference.
 */
struct occ_tensor* occ_propagate_implication(struct occ_propagation_state* state,
                                              int64_t source_id,
                                              float initial_activation,
                                              bool forward,  // true=forward, false=backward
                                              const struct occ_propagation_params* params);

// =============================================================================
// Batch Propagation
// =============================================================================

/**
 * Batch propagation from multiple independent sources
 * 
 * Runs multiple propagations in parallel for efficiency.
 * 
 * @param state Propagation state
 * @param source_ids Source atom IDs [n_batch, n_sources_per_batch]
 * @param initial_activations Initial activations [n_batch, n_sources_per_batch]
 * @param params Propagation parameters
 * @return Activation tensor [n_batch, n_atoms]
 */
struct occ_tensor* occ_propagate_batch(struct occ_propagation_state* state,
                                        struct occ_tensor* source_ids,
                                        struct occ_tensor* initial_activations,
                                        const struct occ_propagation_params* params);

// =============================================================================
// ECAN Integration
// =============================================================================

/**
 * Update attention values based on propagation results
 * 
 * Implements ECAN (Economic Attention Allocation) dynamics:
 * - Atoms receiving activation gain STI
 * - All atoms pay rent (lose STI)
 * - High STI atoms may gain LTI
 * 
 * @param state Propagation state
 * @param rent STI rent (decay per step)
 * @param wage STI wage (gain from activation)
 * @param lti_threshold STI threshold for LTI promotion
 */
void occ_propagation_update_attention(struct occ_propagation_state* state,
                                       float rent,
                                       float wage,
                                       float lti_threshold);

// =============================================================================
// Oscillator Dynamics (Deep Tree Echo)
// =============================================================================

/**
 * Oscillator-based propagation parameters
 */
struct occ_oscillator_params {
    float frequency;        // Base oscillation frequency
    float damping;          // Damping coefficient
    float coupling;         // Coupling strength between oscillators
    float phase_noise;      // Phase noise amplitude
    int n_harmonics;        // Number of harmonics to track
};

/**
 * Initialize oscillator state for atoms
 * 
 * Each atom becomes an oscillator with phase and amplitude.
 * 
 * @param state Propagation state
 * @param params Oscillator parameters
 */
void occ_oscillator_init(struct occ_propagation_state* state,
                          const struct occ_oscillator_params* params);

/**
 * Step oscillator dynamics
 * 
 * Updates phase and amplitude based on coupled oscillator equations.
 * Implements the "echo" in Deep Tree Echo.
 * 
 * @param state Propagation state
 * @param params Oscillator parameters
 * @param dt Time step
 */
void occ_oscillator_step(struct occ_propagation_state* state,
                          const struct occ_oscillator_params* params,
                          float dt);

/**
 * Get oscillator phase tensor
 * 
 * @param state Propagation state
 * @return Phase tensor [n_atoms]
 */
struct occ_tensor* occ_oscillator_get_phase(struct occ_propagation_state* state);

/**
 * Get oscillator amplitude tensor
 * 
 * @param state Propagation state
 * @return Amplitude tensor [n_atoms]
 */
struct occ_tensor* occ_oscillator_get_amplitude(struct occ_propagation_state* state);

/**
 * Detect phase synchronization clusters
 * 
 * Finds groups of atoms with synchronized oscillation.
 * These represent emergent cognitive patterns.
 * 
 * @param state Propagation state
 * @param sync_threshold Phase difference threshold for synchronization
 * @param out_cluster_ids Output: cluster ID for each atom [n_atoms]
 * @return Number of clusters found
 */
int occ_oscillator_detect_clusters(struct occ_propagation_state* state,
                                    float sync_threshold,
                                    int32_t* out_cluster_ids);

#ifdef __cplusplus
}
#endif

#endif // OCC_PROPAGATION_H
