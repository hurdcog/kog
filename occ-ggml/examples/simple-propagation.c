/**
 * OCC-GGML Example: Simple Spreading Activation
 * 
 * Demonstrates the Deep Tree Echo spreading activation algorithm
 * on a simple knowledge graph.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "occ-ggml.h"
#include "occ-atomspace.h"
#include "occ-propagation.h"

// =============================================================================
// Example: Animal Taxonomy
// =============================================================================

void example_animal_taxonomy(void) {
    printf("\n=== Animal Taxonomy Example ===\n\n");
    
    // Create AtomSpace with 1000 atoms, 64-dim embeddings, dense adjacency
    struct occ_atomspace* as = occ_atomspace_new(1000, 64, true);
    if (!as) {
        fprintf(stderr, "Failed to create AtomSpace\n");
        return;
    }
    
    // Add concept nodes
    printf("Creating knowledge graph...\n");
    
    int64_t animal = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Animal", NULL);
    int64_t mammal = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Mammal", NULL);
    int64_t bird = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Bird", NULL);
    int64_t fish = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Fish", NULL);
    
    int64_t cat = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Cat", NULL);
    int64_t dog = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Dog", NULL);
    int64_t lion = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Lion", NULL);
    
    int64_t sparrow = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Sparrow", NULL);
    int64_t eagle = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Eagle", NULL);
    
    int64_t salmon = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Salmon", NULL);
    int64_t shark = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Shark", NULL);
    
    // Add properties
    int64_t has_fur = occ_atomspace_add_node(as, OCC_ATOM_PREDICATE_NODE, "has_fur", NULL);
    int64_t has_feathers = occ_atomspace_add_node(as, OCC_ATOM_PREDICATE_NODE, "has_feathers", NULL);
    int64_t can_fly = occ_atomspace_add_node(as, OCC_ATOM_PREDICATE_NODE, "can_fly", NULL);
    int64_t can_swim = occ_atomspace_add_node(as, OCC_ATOM_PREDICATE_NODE, "can_swim", NULL);
    
    printf("  Created %lld atoms\n", (long long)as->n_atoms);
    
    // Add inheritance links
    printf("Adding inheritance relationships...\n");
    
    // Mammal -> Animal
    int64_t out1[] = {mammal, animal};
    occ_atomspace_add_link(as, OCC_ATOM_INHERITANCE_LINK, out1, 2, NULL);
    
    // Bird -> Animal
    int64_t out2[] = {bird, animal};
    occ_atomspace_add_link(as, OCC_ATOM_INHERITANCE_LINK, out2, 2, NULL);
    
    // Fish -> Animal
    int64_t out3[] = {fish, animal};
    occ_atomspace_add_link(as, OCC_ATOM_INHERITANCE_LINK, out3, 2, NULL);
    
    // Cat, Dog, Lion -> Mammal
    int64_t out4[] = {cat, mammal};
    occ_atomspace_add_link(as, OCC_ATOM_INHERITANCE_LINK, out4, 2, NULL);
    
    int64_t out5[] = {dog, mammal};
    occ_atomspace_add_link(as, OCC_ATOM_INHERITANCE_LINK, out5, 2, NULL);
    
    int64_t out6[] = {lion, mammal};
    occ_atomspace_add_link(as, OCC_ATOM_INHERITANCE_LINK, out6, 2, NULL);
    
    // Sparrow, Eagle -> Bird
    int64_t out7[] = {sparrow, bird};
    occ_atomspace_add_link(as, OCC_ATOM_INHERITANCE_LINK, out7, 2, NULL);
    
    int64_t out8[] = {eagle, bird};
    occ_atomspace_add_link(as, OCC_ATOM_INHERITANCE_LINK, out8, 2, NULL);
    
    // Salmon, Shark -> Fish
    int64_t out9[] = {salmon, fish};
    occ_atomspace_add_link(as, OCC_ATOM_INHERITANCE_LINK, out9, 2, NULL);
    
    int64_t out10[] = {shark, fish};
    occ_atomspace_add_link(as, OCC_ATOM_INHERITANCE_LINK, out10, 2, NULL);
    
    // Property links (similarity-like connections)
    int64_t out11[] = {mammal, has_fur};
    occ_atomspace_add_link(as, OCC_ATOM_SIMILARITY_LINK, out11, 2, NULL);
    
    int64_t out12[] = {bird, has_feathers};
    occ_atomspace_add_link(as, OCC_ATOM_SIMILARITY_LINK, out12, 2, NULL);
    
    int64_t out13[] = {bird, can_fly};
    occ_atomspace_add_link(as, OCC_ATOM_SIMILARITY_LINK, out13, 2, NULL);
    
    int64_t out14[] = {fish, can_swim};
    occ_atomspace_add_link(as, OCC_ATOM_SIMILARITY_LINK, out14, 2, NULL);
    
    printf("  Created %lld total atoms (including links)\n", (long long)as->n_atoms);
    
    // Create propagation engine
    printf("\nInitializing propagation engine...\n");
    struct occ_propagation_state* prop = occ_propagation_new(as);
    
    // Set propagation parameters
    struct occ_propagation_params params = OCC_DEFAULT_PROPAGATION_PARAMS;
    params.decay_rate = 0.7f;
    params.threshold = 0.05f;
    params.max_hops = 4;
    params.bidirectional = true;
    params.normalization = OCC_NORM_MAX;
    
    // Propagate from "Cat"
    printf("\n--- Propagating from 'Cat' ---\n");
    
    struct occ_tensor* activation = occ_propagate_from_atom(prop, cat, 1.0f, &params);
    
    printf("Propagation completed in %d iterations\n", prop->total_iterations);
    printf("Active atoms: %d\n", prop->n_active_atoms);
    printf("Max activation: %.4f\n", prop->max_activation);
    
    // Get top activated atoms
    printf("\nTop activated concepts:\n");
    int64_t top_ids[10];
    float top_acts[10];
    int64_t n_top = occ_propagation_get_top_k(prop, 10, top_ids, top_acts);
    
    const char* names[] = {
        "Animal", "Mammal", "Bird", "Fish",
        "Cat", "Dog", "Lion", "Sparrow", "Eagle", "Salmon", "Shark",
        "has_fur", "has_feathers", "can_fly", "can_swim"
    };
    
    for (int64_t i = 0; i < n_top; i++) {
        int64_t id = top_ids[i];
        const char* name = (id < 15) ? names[id] : "Link";
        printf("  %2lld. %-15s: %.4f\n", (long long)(i+1), name, top_acts[i]);
    }
    
    // Propagate from "can_fly" to find flying things
    printf("\n--- Propagating from 'can_fly' ---\n");
    occ_propagation_reset(prop);
    
    activation = occ_propagate_from_atom(prop, can_fly, 1.0f, &params);
    
    printf("Propagation completed in %d iterations\n", prop->total_iterations);
    
    printf("\nConcepts associated with flying:\n");
    n_top = occ_propagation_get_top_k(prop, 10, top_ids, top_acts);
    
    for (int64_t i = 0; i < n_top; i++) {
        int64_t id = top_ids[i];
        const char* name = (id < 15) ? names[id] : "Link";
        printf("  %2lld. %-15s: %.4f\n", (long long)(i+1), name, top_acts[i]);
    }
    
    // Cleanup
    occ_propagation_free(prop);
    occ_atomspace_free(as);
    
    printf("\nExample completed.\n");
}

// =============================================================================
// Example: Oscillator Dynamics
// =============================================================================

void example_oscillator_dynamics(void) {
    printf("\n=== Oscillator Dynamics Example ===\n\n");
    
    // Create a small AtomSpace
    struct occ_atomspace* as = occ_atomspace_new(100, 32, true);
    if (!as) {
        fprintf(stderr, "Failed to create AtomSpace\n");
        return;
    }
    
    // Create a ring of connected nodes
    printf("Creating ring topology...\n");
    int n_nodes = 20;
    int64_t nodes[20];
    
    for (int i = 0; i < n_nodes; i++) {
        char name[32];
        snprintf(name, sizeof(name), "Node_%d", i);
        nodes[i] = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, name, NULL);
    }
    
    // Connect in a ring
    for (int i = 0; i < n_nodes; i++) {
        int64_t out[] = {nodes[i], nodes[(i + 1) % n_nodes]};
        occ_atomspace_add_link(as, OCC_ATOM_SIMILARITY_LINK, out, 2, NULL);
    }
    
    // Create propagation state
    struct occ_propagation_state* prop = occ_propagation_new(as);
    
    // Initialize oscillators
    printf("Initializing oscillators...\n");
    struct occ_oscillator_params osc_params = {
        .frequency = 1.0f,
        .damping = 0.5f,
        .coupling = 0.3f,
        .phase_noise = 0.1f,
        .n_harmonics = 1
    };
    
    occ_oscillator_init(prop, &osc_params);
    
    // Inject activation at one node
    struct occ_propagation_params params = OCC_DEFAULT_PROPAGATION_PARAMS;
    params.decay_rate = 0.9f;
    params.max_hops = 10;
    
    occ_propagate_from_atom(prop, nodes[0], 1.0f, &params);
    
    // Run oscillator dynamics
    printf("Running oscillator dynamics...\n");
    float dt = 0.1f;
    int n_steps = 100;
    
    for (int step = 0; step < n_steps; step++) {
        occ_oscillator_step(prop, &osc_params, dt);
        
        if (step % 20 == 0) {
            // Detect clusters
            int32_t cluster_ids[100];
            int n_clusters = occ_oscillator_detect_clusters(prop, 0.3f, cluster_ids);
            printf("  Step %3d: %d phase-synchronized clusters\n", step, n_clusters);
        }
    }
    
    // Final cluster detection
    printf("\nFinal state:\n");
    int32_t cluster_ids[100];
    int n_clusters = occ_oscillator_detect_clusters(prop, 0.3f, cluster_ids);
    printf("  Detected %d synchronized clusters\n", n_clusters);
    
    // Show cluster membership
    struct occ_tensor* phase = occ_oscillator_get_phase(prop);
    struct occ_tensor* amplitude = occ_oscillator_get_amplitude(prop);
    
    if (phase && amplitude) {
        const float* ph = (const float*)phase->data;
        const float* amp = (const float*)amplitude->data;
        
        printf("\nNode states:\n");
        for (int i = 0; i < n_nodes; i++) {
            printf("  Node_%02d: phase=%.2f, amp=%.3f, cluster=%d\n",
                   i, ph[nodes[i]], amp[nodes[i]], cluster_ids[nodes[i]]);
        }
    }
    
    // Cleanup
    occ_propagation_free(prop);
    occ_atomspace_free(as);
    
    printf("\nExample completed.\n");
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char** argv) {
    printf("OCC-GGML Examples\n");
    printf("=================\n");
    
    // Seed random number generator
    srand((unsigned int)time(NULL));
    
    // Run examples
    example_animal_taxonomy();
    example_oscillator_dynamics();
    
    return 0;
}
