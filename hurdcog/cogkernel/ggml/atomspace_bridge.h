#ifndef ATOMSPACE_BRIDGE_H
#define ATOMSPACE_BRIDGE_H

/**
 * AtomSpace Bridge Header
 * Integration interface between cognitive kernels and AtomSpace
 * 
 * Provides seamless integration between tensor operations and 
 * OpenCog AtomSpace hypergraph knowledge base
 */

#include "cognitive_kernels.h"
#include "tensor_signatures.h"
#include <stdint.h>
#include <stdbool.h>

// AtomSpace handle structure
struct atomspace_handle {
    void* atomspace_ptr;
    uint64_t handle_id;
    bool is_valid;
};

// Atom types
typedef enum {
    ATOM_NODE,
    ATOM_LINK,
    ATOM_CONCEPT_NODE,
    ATOM_PREDICATE_NODE,
    ATOM_EVALUATION_LINK,
    ATOM_INHERITANCE_LINK,
    ATOM_SIMILARITY_LINK,
    ATOM_COUNT
} atom_type_t;

// Truth value structure
typedef struct {
    float strength;     // [0.0, 1.0]
    float confidence;   // [0.0, 1.0]
} truth_value_t;

// Atom structure
typedef struct {
    uint64_t atom_id;
    atom_type_t type;
    char* name;
    truth_value_t truth_value;
    struct atomspace_handle* atomspace_ref;
} atom_t;

// AtomSpace integration functions

/**
 * Initialize AtomSpace bridge
 */
int atomspace_bridge_init(void);

/**
 * Shutdown AtomSpace bridge
 */
void atomspace_bridge_shutdown(void);

/**
 * Create new AtomSpace handle
 */
struct atomspace_handle* atomspace_create_handle(void);

/**
 * Destroy AtomSpace handle
 */
void atomspace_destroy_handle(struct atomspace_handle* handle);

/**
 * Integrate cognitive result with AtomSpace
 */
int atomspace_integrate_result(cognitive_result_t result);

/**
 * Convert tensor to AtomSpace atoms
 */
int atomspace_tensor_to_atoms(
    cognitive_tensor_t* tensor,
    struct atomspace_handle* handle,
    atom_type_t default_type
);

/**
 * Convert AtomSpace atoms to tensor
 */
cognitive_tensor_t* atomspace_atoms_to_tensor(
    struct atomspace_handle* handle,
    cognitive_tensor_shape_t shape
);

/**
 * Query AtomSpace for pattern
 */
cognitive_result_t atomspace_pattern_query(
    struct atomspace_handle* handle,
    cognitive_tensor_t* pattern,
    cognitive_kernel_config_t config
);

/**
 * Add atom to AtomSpace
 */
atom_t* atomspace_add_atom(
    struct atomspace_handle* handle,
    atom_type_t type,
    const char* name,
    truth_value_t tv
);

/**
 * Get atom from AtomSpace
 */
atom_t* atomspace_get_atom(
    struct atomspace_handle* handle,
    uint64_t atom_id
);

/**
 * Remove atom from AtomSpace
 */
int atomspace_remove_atom(
    struct atomspace_handle* handle,
    uint64_t atom_id
);

/**
 * Create link between atoms
 */
atom_t* atomspace_create_link(
    struct atomspace_handle* handle,
    atom_type_t link_type,
    atom_t** outgoing,
    size_t outgoing_count,
    truth_value_t tv
);

/**
 * Perform PLN inference
 */
cognitive_result_t atomspace_pln_inference(
    struct atomspace_handle* handle,
    cognitive_tensor_t* premises,
    cognitive_kernel_config_t config
);

/**
 * Update attention values
 */
int atomspace_update_attention(
    struct atomspace_handle* handle,
    cognitive_tensor_t* attention_tensor
);

/**
 * Get attention allocation
 */
cognitive_tensor_t* atomspace_get_attention(
    struct atomspace_handle* handle,
    cognitive_tensor_shape_t shape
);

/**
 * Perform pattern mining
 */
cognitive_result_t atomspace_pattern_mining(
    struct atomspace_handle* handle,
    cognitive_tensor_t* data,
    uint32_t min_support,
    cognitive_kernel_config_t config
);

/**
 * Free atom structure
 */
void atomspace_free_atom(atom_t* atom);

#endif // ATOMSPACE_BRIDGE_H
