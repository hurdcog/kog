/**
 * OCC-GGML Tests: AtomSpace
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "occ-ggml.h"
#include "occ-atomspace.h"

#define TEST(name) printf("  Testing %s... ", name)
#define PASS() printf("PASS\n")
#define FAIL(msg) do { printf("FAIL: %s\n", msg); exit(1); } while(0)

void test_atomspace_creation(void) {
    TEST("AtomSpace creation");
    
    struct occ_atomspace* as = occ_atomspace_new(1000, 64, true);
    assert(as != NULL);
    assert(as->max_atoms == 1000);
    assert(as->embed_dim == 64);
    assert(as->n_atoms == 0);
    
    occ_atomspace_free(as);
    PASS();
}

void test_add_nodes(void) {
    TEST("Adding nodes");
    
    struct occ_atomspace* as = occ_atomspace_new(100, 32, false);
    
    int64_t id1 = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Test1", NULL);
    assert(id1 == 0);
    assert(as->n_atoms == 1);
    
    int64_t id2 = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Test2", NULL);
    assert(id2 == 1);
    assert(as->n_atoms == 2);
    
    // Adding same name should return existing ID
    int64_t id3 = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Test1", NULL);
    assert(id3 == id1);
    assert(as->n_atoms == 2);
    
    occ_atomspace_free(as);
    PASS();
}

void test_find_by_name(void) {
    TEST("Find by name");
    
    struct occ_atomspace* as = occ_atomspace_new(100, 32, false);
    
    occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Alpha", NULL);
    occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Beta", NULL);
    occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Gamma", NULL);
    
    assert(occ_atomspace_find_by_name(as, "Alpha") == 0);
    assert(occ_atomspace_find_by_name(as, "Beta") == 1);
    assert(occ_atomspace_find_by_name(as, "Gamma") == 2);
    assert(occ_atomspace_find_by_name(as, "Delta") == -1);
    
    occ_atomspace_free(as);
    PASS();
}

void test_truth_values(void) {
    TEST("Truth values");
    
    struct occ_atomspace* as = occ_atomspace_new(100, 32, false);
    
    struct occ_truth_value tv = {0.8f, 0.9f, 100};
    int64_t id = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Test", &tv);
    
    struct occ_truth_value tv2 = occ_atomspace_get_tv(as, id);
    assert(fabsf(tv2.strength - 0.8f) < 0.001f);
    assert(fabsf(tv2.confidence - 0.9f) < 0.001f);
    assert(tv2.count == 100);
    
    // Update truth value
    struct occ_truth_value tv3 = {0.5f, 0.6f, 200};
    occ_atomspace_set_tv(as, id, &tv3);
    
    struct occ_truth_value tv4 = occ_atomspace_get_tv(as, id);
    assert(fabsf(tv4.strength - 0.5f) < 0.001f);
    
    occ_atomspace_free(as);
    PASS();
}

void test_attention_values(void) {
    TEST("Attention values");
    
    struct occ_atomspace* as = occ_atomspace_new(100, 32, false);
    
    int64_t id = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Test", NULL);
    
    // Default attention values should be zero
    struct occ_attention_value av = occ_atomspace_get_av(as, id);
    assert(fabsf(av.sti) < 0.001f);
    assert(fabsf(av.lti) < 0.001f);
    
    // Set attention values
    struct occ_attention_value av2 = {100.0f, 50.0f, 10.0f};
    occ_atomspace_set_av(as, id, &av2);
    
    struct occ_attention_value av3 = occ_atomspace_get_av(as, id);
    assert(fabsf(av3.sti - 100.0f) < 0.001f);
    assert(fabsf(av3.lti - 50.0f) < 0.001f);
    
    occ_atomspace_free(as);
    PASS();
}

void test_add_links(void) {
    TEST("Adding links");
    
    struct occ_atomspace* as = occ_atomspace_new(100, 32, true);
    
    int64_t a = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "A", NULL);
    int64_t b = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "B", NULL);
    
    int64_t out[] = {a, b};
    int64_t link = occ_atomspace_add_link(as, OCC_ATOM_INHERITANCE_LINK, out, 2, NULL);
    
    assert(link >= 0);
    assert(as->n_atoms == 3);  // 2 nodes + 1 link
    
    // Check adjacency matrix
    if (as->adj_dense) {
        const float* adj = (const float*)as->adj_dense->data;
        int64_t n = as->max_atoms;
        
        // A -> B should be connected
        assert(adj[a + b * n] > 0.5f);
    }
    
    occ_atomspace_free(as);
    PASS();
}

void test_embeddings(void) {
    TEST("Embeddings");
    
    struct occ_atomspace* as = occ_atomspace_new(100, 4, false);
    
    int64_t id = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "Test", NULL);
    
    float emb[] = {1.0f, 2.0f, 3.0f, 4.0f};
    occ_atomspace_set_embedding(as, id, emb);
    
    const float* emb2 = occ_atomspace_get_embedding(as, id);
    assert(emb2 != NULL);
    assert(fabsf(emb2[0] - 1.0f) < 0.001f);
    assert(fabsf(emb2[1] - 2.0f) < 0.001f);
    assert(fabsf(emb2[2] - 3.0f) < 0.001f);
    assert(fabsf(emb2[3] - 4.0f) < 0.001f);
    
    occ_atomspace_free(as);
    PASS();
}

void test_similarity(void) {
    TEST("Embedding similarity");
    
    struct occ_atomspace* as = occ_atomspace_new(100, 4, false);
    
    int64_t id1 = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "A", NULL);
    int64_t id2 = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "B", NULL);
    int64_t id3 = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "C", NULL);
    
    float emb1[] = {1.0f, 0.0f, 0.0f, 0.0f};
    float emb2[] = {1.0f, 0.0f, 0.0f, 0.0f};  // Same as emb1
    float emb3[] = {0.0f, 1.0f, 0.0f, 0.0f};  // Orthogonal
    
    occ_atomspace_set_embedding(as, id1, emb1);
    occ_atomspace_set_embedding(as, id2, emb2);
    occ_atomspace_set_embedding(as, id3, emb3);
    
    float sim12 = occ_atomspace_similarity(as, id1, id2);
    float sim13 = occ_atomspace_similarity(as, id1, id3);
    
    assert(fabsf(sim12 - 1.0f) < 0.001f);  // Identical vectors
    assert(fabsf(sim13) < 0.001f);          // Orthogonal vectors
    
    occ_atomspace_free(as);
    PASS();
}

void test_get_by_type(void) {
    TEST("Get atoms by type");
    
    struct occ_atomspace* as = occ_atomspace_new(100, 32, false);
    
    occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "C1", NULL);
    occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "C2", NULL);
    occ_atomspace_add_node(as, OCC_ATOM_PREDICATE_NODE, "P1", NULL);
    occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "C3", NULL);
    
    int64_t ids[10];
    int64_t count = occ_atomspace_get_by_type(as, OCC_ATOM_CONCEPT_NODE, ids, 10);
    
    assert(count == 3);
    
    count = occ_atomspace_get_by_type(as, OCC_ATOM_PREDICATE_NODE, ids, 10);
    assert(count == 1);
    
    occ_atomspace_free(as);
    PASS();
}

void test_attentional_focus(void) {
    TEST("Attentional focus");
    
    struct occ_atomspace* as = occ_atomspace_new(100, 32, false);
    
    int64_t id1 = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "A", NULL);
    int64_t id2 = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "B", NULL);
    int64_t id3 = occ_atomspace_add_node(as, OCC_ATOM_CONCEPT_NODE, "C", NULL);
    
    struct occ_attention_value av1 = {100.0f, 0.0f, 0.0f};
    struct occ_attention_value av2 = {50.0f, 0.0f, 0.0f};
    struct occ_attention_value av3 = {10.0f, 0.0f, 0.0f};
    
    occ_atomspace_set_av(as, id1, &av1);
    occ_atomspace_set_av(as, id2, &av2);
    occ_atomspace_set_av(as, id3, &av3);
    
    int64_t focus_ids[10];
    int64_t count = occ_atomspace_get_attentional_focus(as, 60.0f, focus_ids, 10);
    
    assert(count == 1);
    assert(focus_ids[0] == id1);
    
    count = occ_atomspace_get_attentional_focus(as, 20.0f, focus_ids, 10);
    assert(count == 2);
    
    occ_atomspace_free(as);
    PASS();
}

int main(void) {
    printf("OCC-GGML AtomSpace Tests\n");
    printf("========================\n\n");
    
    test_atomspace_creation();
    test_add_nodes();
    test_find_by_name();
    test_truth_values();
    test_attention_values();
    test_add_links();
    test_embeddings();
    test_similarity();
    test_get_by_type();
    test_attentional_focus();
    
    printf("\nAll tests passed!\n");
    return 0;
}
