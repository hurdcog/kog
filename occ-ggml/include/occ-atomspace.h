#ifndef OCC_ATOMSPACE_H
#define OCC_ATOMSPACE_H

/**
 * OCC AtomSpace: Tensor-based Hypergraph Memory
 * 
 * Implements the OpenCog AtomSpace as efficient tensor structures
 * for GPU-accelerated cognitive processing.
 */

#include "occ-ggml.h"

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// Atom Types
// =============================================================================

enum occ_atom_type {
    // Node types
    OCC_ATOM_CONCEPT_NODE = 0,
    OCC_ATOM_PREDICATE_NODE,
    OCC_ATOM_VARIABLE_NODE,
    OCC_ATOM_NUMBER_NODE,
    OCC_ATOM_SCHEMA_NODE,
    OCC_ATOM_GROUNDED_NODE,
    
    // Link types
    OCC_ATOM_INHERITANCE_LINK = 64,
    OCC_ATOM_SIMILARITY_LINK,
    OCC_ATOM_EVALUATION_LINK,
    OCC_ATOM_LIST_LINK,
    OCC_ATOM_SET_LINK,
    OCC_ATOM_AND_LINK,
    OCC_ATOM_OR_LINK,
    OCC_ATOM_NOT_LINK,
    OCC_ATOM_IMPLICATION_LINK,
    OCC_ATOM_EQUIVALENCE_LINK,
    OCC_ATOM_EXECUTION_LINK,
    OCC_ATOM_BIND_LINK,
    
    OCC_ATOM_TYPE_COUNT
};

// =============================================================================
// Truth Value Structure
// =============================================================================

struct occ_truth_value {
    float strength;     // Probability [0, 1]
    float confidence;   // Confidence [0, 1]
    int64_t count;      // Evidence count
};

// =============================================================================
// Attention Value Structure
// =============================================================================

struct occ_attention_value {
    float sti;   // Short-term importance
    float lti;   // Long-term importance
    float vlti;  // Very long-term importance
};

// =============================================================================
// AtomSpace Structure
// =============================================================================

/**
 * Tensor-based AtomSpace
 * 
 * Memory layout optimized for batch operations:
 * - Atoms stored as embedding vectors
 * - Links stored as adjacency tensors
 * - Truth/attention values as separate tensors for SIMD
 */
struct occ_atomspace {
    struct occ_context* ctx;
    
    // Capacity
    int64_t max_atoms;
    int64_t n_atoms;
    int64_t embed_dim;
    
    // Atom storage
    struct occ_tensor* embeddings;      // [max_atoms, embed_dim] F16/F32
    struct occ_tensor* atom_types;      // [max_atoms] I32
    struct occ_tensor* atom_names;      // [max_atoms, max_name_len] I32 (char codes)
    
    // Truth values
    struct occ_tensor* tv_strength;     // [max_atoms] F32
    struct occ_tensor* tv_confidence;   // [max_atoms] F32
    struct occ_tensor* tv_count;        // [max_atoms] I64
    
    // Attention values
    struct occ_tensor* av_sti;          // [max_atoms] F32
    struct occ_tensor* av_lti;          // [max_atoms] F32
    struct occ_tensor* av_vlti;         // [max_atoms] F32
    
    // Adjacency (sparse representation)
    struct occ_tensor* adj_row_ptr;     // [max_atoms + 1] I64 (CSR row pointers)
    struct occ_tensor* adj_col_idx;     // [max_edges] I64 (CSR column indices)
    struct occ_tensor* adj_link_type;   // [max_edges] I32 (link types)
    struct occ_tensor* adj_weights;     // [max_edges] F32 (edge weights)
    
    // Dense adjacency (for small graphs)
    struct occ_tensor* adj_dense;       // [max_atoms, max_atoms] F32 (optional)
    
    // Working memory
    struct occ_tensor* activation;      // [max_atoms] F32
    struct occ_tensor* visited;         // [max_atoms] I32
    
    // Index structures
    void* name_index;                   // Hash table: name -> atom_id
    void* type_index;                   // Hash table: type -> list of atom_ids
};

// =============================================================================
// AtomSpace Management
// =============================================================================

/**
 * Create a new AtomSpace
 * 
 * @param max_atoms Maximum number of atoms
 * @param embed_dim Embedding dimension
 * @param use_dense Use dense adjacency matrix (for small graphs)
 * @return New AtomSpace
 */
struct occ_atomspace* occ_atomspace_new(int64_t max_atoms, int64_t embed_dim, bool use_dense);

/**
 * Free an AtomSpace
 */
void occ_atomspace_free(struct occ_atomspace* as);

/**
 * Clear all atoms from AtomSpace
 */
void occ_atomspace_clear(struct occ_atomspace* as);

// =============================================================================
// Atom Operations
// =============================================================================

/**
 * Add a node to the AtomSpace
 * 
 * @param as AtomSpace
 * @param type Atom type
 * @param name Atom name
 * @param tv Truth value (NULL for default)
 * @return Atom ID or -1 on error
 */
int64_t occ_atomspace_add_node(struct occ_atomspace* as,
                                enum occ_atom_type type,
                                const char* name,
                                const struct occ_truth_value* tv);

/**
 * Add a link to the AtomSpace
 * 
 * @param as AtomSpace
 * @param type Link type
 * @param outgoing Array of outgoing atom IDs
 * @param n_outgoing Number of outgoing atoms
 * @param tv Truth value (NULL for default)
 * @return Atom ID or -1 on error
 */
int64_t occ_atomspace_add_link(struct occ_atomspace* as,
                                enum occ_atom_type type,
                                const int64_t* outgoing,
                                int n_outgoing,
                                const struct occ_truth_value* tv);

/**
 * Remove an atom from the AtomSpace
 * 
 * @param as AtomSpace
 * @param atom_id Atom ID
 * @return true if removed, false if not found
 */
bool occ_atomspace_remove(struct occ_atomspace* as, int64_t atom_id);

/**
 * Get atom by ID
 * 
 * @param as AtomSpace
 * @param atom_id Atom ID
 * @param type Output: atom type
 * @param tv Output: truth value
 * @param av Output: attention value
 * @return true if found
 */
bool occ_atomspace_get(const struct occ_atomspace* as,
                        int64_t atom_id,
                        enum occ_atom_type* type,
                        struct occ_truth_value* tv,
                        struct occ_attention_value* av);

/**
 * Find atom by name
 * 
 * @param as AtomSpace
 * @param name Atom name
 * @return Atom ID or -1 if not found
 */
int64_t occ_atomspace_find_by_name(const struct occ_atomspace* as, const char* name);

/**
 * Get atoms by type
 * 
 * @param as AtomSpace
 * @param type Atom type
 * @param out_ids Output array for atom IDs
 * @param max_ids Maximum IDs to return
 * @return Number of atoms found
 */
int64_t occ_atomspace_get_by_type(const struct occ_atomspace* as,
                                   enum occ_atom_type type,
                                   int64_t* out_ids,
                                   int64_t max_ids);

// =============================================================================
// Truth Value Operations
// =============================================================================

/**
 * Set truth value for an atom
 */
void occ_atomspace_set_tv(struct occ_atomspace* as,
                           int64_t atom_id,
                           const struct occ_truth_value* tv);

/**
 * Get truth value for an atom
 */
struct occ_truth_value occ_atomspace_get_tv(const struct occ_atomspace* as, int64_t atom_id);

/**
 * Batch update truth values (tensor operation)
 * 
 * @param as AtomSpace
 * @param atom_ids Atom IDs [n]
 * @param strengths New strengths [n]
 * @param confidences New confidences [n]
 */
void occ_atomspace_batch_set_tv(struct occ_atomspace* as,
                                 struct occ_tensor* atom_ids,
                                 struct occ_tensor* strengths,
                                 struct occ_tensor* confidences);

// =============================================================================
// Attention Value Operations
// =============================================================================

/**
 * Set attention value for an atom
 */
void occ_atomspace_set_av(struct occ_atomspace* as,
                           int64_t atom_id,
                           const struct occ_attention_value* av);

/**
 * Get attention value for an atom
 */
struct occ_attention_value occ_atomspace_get_av(const struct occ_atomspace* as, int64_t atom_id);

/**
 * Get atoms in attentional focus (STI above threshold)
 * 
 * @param as AtomSpace
 * @param threshold STI threshold
 * @param out_ids Output array for atom IDs
 * @param max_ids Maximum IDs to return
 * @return Number of atoms in focus
 */
int64_t occ_atomspace_get_attentional_focus(const struct occ_atomspace* as,
                                             float threshold,
                                             int64_t* out_ids,
                                             int64_t max_ids);

// =============================================================================
// Adjacency Operations
// =============================================================================

/**
 * Get incoming links for an atom
 * 
 * @param as AtomSpace
 * @param atom_id Atom ID
 * @param out_link_ids Output array for link IDs
 * @param max_links Maximum links to return
 * @return Number of incoming links
 */
int64_t occ_atomspace_get_incoming(const struct occ_atomspace* as,
                                    int64_t atom_id,
                                    int64_t* out_link_ids,
                                    int64_t max_links);

/**
 * Get outgoing atoms for a link
 * 
 * @param as AtomSpace
 * @param link_id Link ID
 * @param out_atom_ids Output array for atom IDs
 * @param max_atoms Maximum atoms to return
 * @return Number of outgoing atoms
 */
int64_t occ_atomspace_get_outgoing(const struct occ_atomspace* as,
                                    int64_t link_id,
                                    int64_t* out_atom_ids,
                                    int64_t max_atoms);

/**
 * Get adjacency matrix as tensor
 * 
 * @param as AtomSpace
 * @param link_type Filter by link type (-1 for all)
 * @return Adjacency tensor [n_atoms, n_atoms]
 */
struct occ_tensor* occ_atomspace_get_adjacency(struct occ_atomspace* as, int link_type);

// =============================================================================
// Embedding Operations
// =============================================================================

/**
 * Set embedding for an atom
 */
void occ_atomspace_set_embedding(struct occ_atomspace* as,
                                  int64_t atom_id,
                                  const float* embedding);

/**
 * Get embedding for an atom
 */
const float* occ_atomspace_get_embedding(const struct occ_atomspace* as, int64_t atom_id);

/**
 * Compute similarity between atoms using embeddings
 * 
 * @param as AtomSpace
 * @param atom_id1 First atom
 * @param atom_id2 Second atom
 * @return Cosine similarity [-1, 1]
 */
float occ_atomspace_similarity(const struct occ_atomspace* as,
                                int64_t atom_id1,
                                int64_t atom_id2);

/**
 * Find k nearest neighbors by embedding
 * 
 * @param as AtomSpace
 * @param atom_id Query atom
 * @param k Number of neighbors
 * @param out_ids Output array for neighbor IDs
 * @param out_scores Output array for similarity scores
 * @return Number of neighbors found
 */
int64_t occ_atomspace_knn(const struct occ_atomspace* as,
                           int64_t atom_id,
                           int k,
                           int64_t* out_ids,
                           float* out_scores);

// =============================================================================
// Batch Operations (Tensor-based)
// =============================================================================

/**
 * Batch embedding lookup
 * 
 * @param as AtomSpace
 * @param atom_ids Atom IDs tensor [n]
 * @return Embeddings tensor [n, embed_dim]
 */
struct occ_tensor* occ_atomspace_batch_get_embeddings(struct occ_atomspace* as,
                                                       struct occ_tensor* atom_ids);

/**
 * Batch similarity computation
 * 
 * @param as AtomSpace
 * @param query_ids Query atom IDs [n_queries]
 * @param candidate_ids Candidate atom IDs [n_candidates]
 * @return Similarity matrix [n_queries, n_candidates]
 */
struct occ_tensor* occ_atomspace_batch_similarity(struct occ_atomspace* as,
                                                   struct occ_tensor* query_ids,
                                                   struct occ_tensor* candidate_ids);

// =============================================================================
// Serialization
// =============================================================================

/**
 * Save AtomSpace to file
 */
bool occ_atomspace_save(const struct occ_atomspace* as, const char* path);

/**
 * Load AtomSpace from file
 */
struct occ_atomspace* occ_atomspace_load(const char* path);

#ifdef __cplusplus
}
#endif

#endif // OCC_ATOMSPACE_H
