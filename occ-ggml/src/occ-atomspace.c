/**
 * OCC AtomSpace Implementation
 * 
 * Tensor-based hypergraph memory for cognitive processing.
 */

#define _GNU_SOURCE
#include "../include/occ-atomspace.h"
#include <stdio.h>
#include <string.h>

// =============================================================================
// Simple Hash Table for Name Index
// =============================================================================

#define HASH_TABLE_SIZE 65536

struct hash_entry {
    char* key;
    int64_t value;
    struct hash_entry* next;
};

struct hash_table {
    struct hash_entry* buckets[HASH_TABLE_SIZE];
};

static uint32_t hash_string(const char* str) {
    uint32_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

static struct hash_table* hash_table_new(void) {
    struct hash_table* ht = (struct hash_table*)calloc(1, sizeof(struct hash_table));
    return ht;
}

static void hash_table_free(struct hash_table* ht) {
    if (!ht) return;
    
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        struct hash_entry* entry = ht->buckets[i];
        while (entry) {
            struct hash_entry* next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    free(ht);
}

static void hash_table_set(struct hash_table* ht, const char* key, int64_t value) {
    uint32_t idx = hash_string(key) % HASH_TABLE_SIZE;
    
    // Check if key exists
    struct hash_entry* entry = ht->buckets[idx];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return;
        }
        entry = entry->next;
    }
    
    // Add new entry
    entry = (struct hash_entry*)malloc(sizeof(struct hash_entry));
    entry->key = strdup(key);
    entry->value = value;
    entry->next = ht->buckets[idx];
    ht->buckets[idx] = entry;
}

static int64_t hash_table_get(struct hash_table* ht, const char* key) {
    uint32_t idx = hash_string(key) % HASH_TABLE_SIZE;
    
    struct hash_entry* entry = ht->buckets[idx];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return -1;
}

static void hash_table_remove(struct hash_table* ht, const char* key) {
    uint32_t idx = hash_string(key) % HASH_TABLE_SIZE;
    
    struct hash_entry** pp = &ht->buckets[idx];
    while (*pp) {
        if (strcmp((*pp)->key, key) == 0) {
            struct hash_entry* entry = *pp;
            *pp = entry->next;
            free(entry->key);
            free(entry);
            return;
        }
        pp = &(*pp)->next;
    }
}

// =============================================================================
// AtomSpace Management
// =============================================================================

struct occ_atomspace* occ_atomspace_new(int64_t max_atoms, int64_t embed_dim, bool use_dense) {
    struct occ_atomspace* as = (struct occ_atomspace*)calloc(1, sizeof(struct occ_atomspace));
    if (!as) return NULL;
    
    // Calculate memory needed
    size_t mem_size = 0;
    mem_size += max_atoms * embed_dim * sizeof(float);  // embeddings
    mem_size += max_atoms * sizeof(int32_t);            // atom_types
    mem_size += max_atoms * sizeof(float) * 6;          // tv + av
    mem_size += max_atoms * sizeof(float);              // activation
    mem_size += max_atoms * sizeof(int32_t);            // visited
    if (use_dense) {
        mem_size += max_atoms * max_atoms * sizeof(float);  // dense adjacency
    }
    mem_size *= 2;  // Safety margin
    
    as->ctx = occ_init(mem_size);
    if (!as->ctx) {
        free(as);
        return NULL;
    }
    
    as->max_atoms = max_atoms;
    as->n_atoms = 0;
    as->embed_dim = embed_dim;
    
    // Allocate tensors
    as->embeddings = occ_new_tensor_2d(as->ctx, OCC_TYPE_F32, embed_dim, max_atoms);
    as->atom_types = occ_new_tensor_1d(as->ctx, OCC_TYPE_I32, max_atoms);
    
    as->tv_strength = occ_new_tensor_1d(as->ctx, OCC_TYPE_F32, max_atoms);
    as->tv_confidence = occ_new_tensor_1d(as->ctx, OCC_TYPE_F32, max_atoms);
    as->tv_count = occ_new_tensor_1d(as->ctx, OCC_TYPE_I64, max_atoms);
    
    as->av_sti = occ_new_tensor_1d(as->ctx, OCC_TYPE_F32, max_atoms);
    as->av_lti = occ_new_tensor_1d(as->ctx, OCC_TYPE_F32, max_atoms);
    as->av_vlti = occ_new_tensor_1d(as->ctx, OCC_TYPE_F32, max_atoms);
    
    as->activation = occ_new_tensor_1d(as->ctx, OCC_TYPE_F32, max_atoms);
    as->visited = occ_new_tensor_1d(as->ctx, OCC_TYPE_I32, max_atoms);
    
    if (use_dense) {
        as->adj_dense = occ_new_tensor_2d(as->ctx, OCC_TYPE_F32, max_atoms, max_atoms);
    }
    
    // Initialize index structures
    as->name_index = hash_table_new();
    as->type_index = hash_table_new();
    
    // Initialize default values
    float* tv_s = (float*)as->tv_strength->data;
    float* tv_c = (float*)as->tv_confidence->data;
    for (int64_t i = 0; i < max_atoms; i++) {
        tv_s[i] = 1.0f;
        tv_c[i] = 0.0f;
    }
    
    return as;
}

void occ_atomspace_free(struct occ_atomspace* as) {
    if (!as) return;
    
    hash_table_free((struct hash_table*)as->name_index);
    hash_table_free((struct hash_table*)as->type_index);
    occ_free(as->ctx);
    free(as);
}

void occ_atomspace_clear(struct occ_atomspace* as) {
    as->n_atoms = 0;
    
    // Clear indices
    hash_table_free((struct hash_table*)as->name_index);
    hash_table_free((struct hash_table*)as->type_index);
    as->name_index = hash_table_new();
    as->type_index = hash_table_new();
    
    // Clear tensors
    memset(as->embeddings->data, 0, occ_nbytes(as->embeddings));
    memset(as->activation->data, 0, occ_nbytes(as->activation));
    memset(as->visited->data, 0, occ_nbytes(as->visited));
    
    if (as->adj_dense) {
        memset(as->adj_dense->data, 0, occ_nbytes(as->adj_dense));
    }
}

// =============================================================================
// Atom Operations
// =============================================================================

int64_t occ_atomspace_add_node(struct occ_atomspace* as,
                                enum occ_atom_type type,
                                const char* name,
                                const struct occ_truth_value* tv) {
    if (as->n_atoms >= as->max_atoms) {
        fprintf(stderr, "OCC: AtomSpace full\n");
        return -1;
    }
    
    // Check if atom already exists
    int64_t existing = occ_atomspace_find_by_name(as, name);
    if (existing >= 0) {
        return existing;  // Return existing atom
    }
    
    int64_t atom_id = as->n_atoms++;
    
    // Set type
    ((int32_t*)as->atom_types->data)[atom_id] = (int32_t)type;
    
    // Set truth value
    if (tv) {
        ((float*)as->tv_strength->data)[atom_id] = tv->strength;
        ((float*)as->tv_confidence->data)[atom_id] = tv->confidence;
        ((int64_t*)as->tv_count->data)[atom_id] = tv->count;
    } else {
        ((float*)as->tv_strength->data)[atom_id] = 1.0f;
        ((float*)as->tv_confidence->data)[atom_id] = 0.0f;
        ((int64_t*)as->tv_count->data)[atom_id] = 0;
    }
    
    // Initialize attention values
    ((float*)as->av_sti->data)[atom_id] = 0.0f;
    ((float*)as->av_lti->data)[atom_id] = 0.0f;
    ((float*)as->av_vlti->data)[atom_id] = 0.0f;
    
    // Add to name index
    hash_table_set((struct hash_table*)as->name_index, name, atom_id);
    
    return atom_id;
}

int64_t occ_atomspace_add_link(struct occ_atomspace* as,
                                enum occ_atom_type type,
                                const int64_t* outgoing,
                                int n_outgoing,
                                const struct occ_truth_value* tv) {
    if (as->n_atoms >= as->max_atoms) {
        fprintf(stderr, "OCC: AtomSpace full\n");
        return -1;
    }
    
    int64_t link_id = as->n_atoms++;
    
    // Set type
    ((int32_t*)as->atom_types->data)[link_id] = (int32_t)type;
    
    // Set truth value
    if (tv) {
        ((float*)as->tv_strength->data)[link_id] = tv->strength;
        ((float*)as->tv_confidence->data)[link_id] = tv->confidence;
        ((int64_t*)as->tv_count->data)[link_id] = tv->count;
    } else {
        ((float*)as->tv_strength->data)[link_id] = 1.0f;
        ((float*)as->tv_confidence->data)[link_id] = 0.0f;
        ((int64_t*)as->tv_count->data)[link_id] = 0;
    }
    
    // Add edges to adjacency matrix (if dense)
    if (as->adj_dense && n_outgoing >= 2) {
        float* adj = (float*)as->adj_dense->data;
        int64_t n = as->max_atoms;
        
        // For InheritanceLink: outgoing[0] -> outgoing[1]
        // For other links: fully connected
        if (type == OCC_ATOM_INHERITANCE_LINK && n_outgoing == 2) {
            adj[outgoing[0] + outgoing[1] * n] = 1.0f;
        } else {
            for (int i = 0; i < n_outgoing; i++) {
                for (int j = 0; j < n_outgoing; j++) {
                    if (i != j) {
                        adj[outgoing[i] + outgoing[j] * n] = 1.0f;
                    }
                }
            }
        }
    }
    
    return link_id;
}

bool occ_atomspace_remove(struct occ_atomspace* as, int64_t atom_id) {
    if (atom_id < 0 || atom_id >= as->n_atoms) {
        return false;
    }
    
    // Mark as removed by setting type to -1
    ((int32_t*)as->atom_types->data)[atom_id] = -1;
    
    // Clear adjacency
    if (as->adj_dense) {
        float* adj = (float*)as->adj_dense->data;
        int64_t n = as->max_atoms;
        
        for (int64_t i = 0; i < n; i++) {
            adj[atom_id + i * n] = 0.0f;
            adj[i + atom_id * n] = 0.0f;
        }
    }
    
    return true;
}

bool occ_atomspace_get(const struct occ_atomspace* as,
                        int64_t atom_id,
                        enum occ_atom_type* type,
                        struct occ_truth_value* tv,
                        struct occ_attention_value* av) {
    if (atom_id < 0 || atom_id >= as->n_atoms) {
        return false;
    }
    
    if (type) {
        *type = (enum occ_atom_type)((int32_t*)as->atom_types->data)[atom_id];
    }
    
    if (tv) {
        tv->strength = ((float*)as->tv_strength->data)[atom_id];
        tv->confidence = ((float*)as->tv_confidence->data)[atom_id];
        tv->count = ((int64_t*)as->tv_count->data)[atom_id];
    }
    
    if (av) {
        av->sti = ((float*)as->av_sti->data)[atom_id];
        av->lti = ((float*)as->av_lti->data)[atom_id];
        av->vlti = ((float*)as->av_vlti->data)[atom_id];
    }
    
    return true;
}

int64_t occ_atomspace_find_by_name(const struct occ_atomspace* as, const char* name) {
    return hash_table_get((struct hash_table*)as->name_index, name);
}

int64_t occ_atomspace_get_by_type(const struct occ_atomspace* as,
                                   enum occ_atom_type type,
                                   int64_t* out_ids,
                                   int64_t max_ids) {
    int64_t count = 0;
    const int32_t* types = (const int32_t*)as->atom_types->data;
    
    for (int64_t i = 0; i < as->n_atoms && count < max_ids; i++) {
        if (types[i] == (int32_t)type) {
            out_ids[count++] = i;
        }
    }
    
    return count;
}

// =============================================================================
// Truth Value Operations
// =============================================================================

void occ_atomspace_set_tv(struct occ_atomspace* as,
                           int64_t atom_id,
                           const struct occ_truth_value* tv) {
    if (atom_id < 0 || atom_id >= as->n_atoms) return;
    
    ((float*)as->tv_strength->data)[atom_id] = tv->strength;
    ((float*)as->tv_confidence->data)[atom_id] = tv->confidence;
    ((int64_t*)as->tv_count->data)[atom_id] = tv->count;
}

struct occ_truth_value occ_atomspace_get_tv(const struct occ_atomspace* as, int64_t atom_id) {
    struct occ_truth_value tv = {1.0f, 0.0f, 0};
    
    if (atom_id >= 0 && atom_id < as->n_atoms) {
        tv.strength = ((float*)as->tv_strength->data)[atom_id];
        tv.confidence = ((float*)as->tv_confidence->data)[atom_id];
        tv.count = ((int64_t*)as->tv_count->data)[atom_id];
    }
    
    return tv;
}

// =============================================================================
// Attention Value Operations
// =============================================================================

void occ_atomspace_set_av(struct occ_atomspace* as,
                           int64_t atom_id,
                           const struct occ_attention_value* av) {
    if (atom_id < 0 || atom_id >= as->n_atoms) return;
    
    ((float*)as->av_sti->data)[atom_id] = av->sti;
    ((float*)as->av_lti->data)[atom_id] = av->lti;
    ((float*)as->av_vlti->data)[atom_id] = av->vlti;
}

struct occ_attention_value occ_atomspace_get_av(const struct occ_atomspace* as, int64_t atom_id) {
    struct occ_attention_value av = {0.0f, 0.0f, 0.0f};
    
    if (atom_id >= 0 && atom_id < as->n_atoms) {
        av.sti = ((float*)as->av_sti->data)[atom_id];
        av.lti = ((float*)as->av_lti->data)[atom_id];
        av.vlti = ((float*)as->av_vlti->data)[atom_id];
    }
    
    return av;
}

int64_t occ_atomspace_get_attentional_focus(const struct occ_atomspace* as,
                                             float threshold,
                                             int64_t* out_ids,
                                             int64_t max_ids) {
    int64_t count = 0;
    const float* sti = (const float*)as->av_sti->data;
    
    for (int64_t i = 0; i < as->n_atoms && count < max_ids; i++) {
        if (sti[i] >= threshold) {
            out_ids[count++] = i;
        }
    }
    
    return count;
}

// =============================================================================
// Adjacency Operations
// =============================================================================

struct occ_tensor* occ_atomspace_get_adjacency(struct occ_atomspace* as, int link_type) {
    if (!as->adj_dense) {
        // Would need to build from sparse representation
        return NULL;
    }
    
    // For now, return the full adjacency matrix
    // In a full implementation, filter by link_type
    return as->adj_dense;
}

// =============================================================================
// Embedding Operations
// =============================================================================

void occ_atomspace_set_embedding(struct occ_atomspace* as,
                                  int64_t atom_id,
                                  const float* embedding) {
    if (atom_id < 0 || atom_id >= as->n_atoms) return;
    
    float* emb = (float*)as->embeddings->data;
    memcpy(emb + atom_id * as->embed_dim, embedding, as->embed_dim * sizeof(float));
}

const float* occ_atomspace_get_embedding(const struct occ_atomspace* as, int64_t atom_id) {
    if (atom_id < 0 || atom_id >= as->n_atoms) return NULL;
    
    const float* emb = (const float*)as->embeddings->data;
    return emb + atom_id * as->embed_dim;
}

float occ_atomspace_similarity(const struct occ_atomspace* as,
                                int64_t atom_id1,
                                int64_t atom_id2) {
    const float* e1 = occ_atomspace_get_embedding(as, atom_id1);
    const float* e2 = occ_atomspace_get_embedding(as, atom_id2);
    
    if (!e1 || !e2) return 0.0f;
    
    // Cosine similarity
    float dot = 0.0f, norm1 = 0.0f, norm2 = 0.0f;
    for (int64_t i = 0; i < as->embed_dim; i++) {
        dot += e1[i] * e2[i];
        norm1 += e1[i] * e1[i];
        norm2 += e2[i] * e2[i];
    }
    
    if (norm1 < 1e-10f || norm2 < 1e-10f) return 0.0f;
    
    return dot / (sqrtf(norm1) * sqrtf(norm2));
}

// =============================================================================
// Batch Operations
// =============================================================================

struct occ_tensor* occ_atomspace_batch_get_embeddings(struct occ_atomspace* as,
                                                       struct occ_tensor* atom_ids) {
    // Use GET_ROWS operation
    return occ_get_rows(as->ctx, as->embeddings, atom_ids);
}

struct occ_tensor* occ_atomspace_batch_similarity(struct occ_atomspace* as,
                                                   struct occ_tensor* query_ids,
                                                   struct occ_tensor* candidate_ids) {
    // Get embeddings
    struct occ_tensor* q_emb = occ_atomspace_batch_get_embeddings(as, query_ids);
    struct occ_tensor* c_emb = occ_atomspace_batch_get_embeddings(as, candidate_ids);
    
    // Normalize
    struct occ_tensor* q_norm = occ_l2_norm(as->ctx, q_emb, 1e-6f);
    struct occ_tensor* c_norm = occ_l2_norm(as->ctx, c_emb, 1e-6f);
    
    // Compute similarity matrix: Q @ C^T
    struct occ_tensor* c_t = occ_transpose(as->ctx, c_norm);
    struct occ_tensor* sim = occ_mul_mat(as->ctx, q_norm, c_t);
    
    return sim;
}
