/*
 * AtomSpace Test Suite
 * 
 * Tests for the portable AtomSpace implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

/* External declarations */
typedef struct AtomSpace AtomSpace;
typedef struct TruthValue {
    float strength;
    float confidence;
    uint32_t count;
} TruthValue;

extern void atomspace_init(void);
extern void atomspace_shutdown(void);
extern AtomSpace* get_global_atomspace(void);
extern AtomSpace* atomspace_create(void);
extern void atomspace_destroy(AtomSpace *as);
extern uint32_t atomspace_add_node(AtomSpace *as, uint16_t type, const char *name);
extern uint32_t atomspace_add_link(AtomSpace *as, uint16_t type, uint32_t *outgoing, uint32_t arity);
extern void atomspace_remove_atom(AtomSpace *as, uint32_t atom_id);
extern const char* atomspace_get_name(AtomSpace *as, uint32_t atom_id);
extern uint16_t atomspace_get_type(AtomSpace *as, uint32_t atom_id);
extern void atomspace_print_stats(AtomSpace *as);

#define ATOM_TYPE_CONCEPT 0x0002
#define ATOM_TYPE_PREDICATE 0x0003
#define ATOM_TYPE_INHERITANCE 0x0101

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { \
    printf("Running test: %s... ", #name); \
    tests_run++; \
    if (test_##name()) { \
        printf("PASSED\n"); \
        tests_passed++; \
    } else { \
        printf("FAILED\n"); \
    } \
} while(0)

/*
 * Test: Create and destroy AtomSpace
 */
int test_create_destroy(void)
{
    AtomSpace *as = atomspace_create();
    if (as == NULL) return 0;
    
    atomspace_destroy(as);
    return 1;
}

/*
 * Test: Add nodes
 */
int test_add_nodes(void)
{
    AtomSpace *as = atomspace_create();
    if (as == NULL) return 0;
    
    uint32_t id1 = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "cat");
    uint32_t id2 = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "dog");
    uint32_t id3 = atomspace_add_node(as, ATOM_TYPE_PREDICATE, "is-a");
    
    if (id1 == 0 || id2 == 0 || id3 == 0) {
        atomspace_destroy(as);
        return 0;
    }
    
    /* Verify IDs are unique */
    if (id1 == id2 || id2 == id3 || id1 == id3) {
        atomspace_destroy(as);
        return 0;
    }
    
    atomspace_destroy(as);
    return 1;
}

/*
 * Test: Get node name
 */
int test_get_name(void)
{
    AtomSpace *as = atomspace_create();
    if (as == NULL) return 0;
    
    uint32_t id = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "elephant");
    if (id == 0) {
        atomspace_destroy(as);
        return 0;
    }
    
    const char *name = atomspace_get_name(as, id);
    if (name == NULL || strcmp(name, "elephant") != 0) {
        atomspace_destroy(as);
        return 0;
    }
    
    atomspace_destroy(as);
    return 1;
}

/*
 * Test: Get atom type
 */
int test_get_type(void)
{
    AtomSpace *as = atomspace_create();
    if (as == NULL) return 0;
    
    uint32_t id1 = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "test1");
    uint32_t id2 = atomspace_add_node(as, ATOM_TYPE_PREDICATE, "test2");
    
    if (atomspace_get_type(as, id1) != ATOM_TYPE_CONCEPT) {
        atomspace_destroy(as);
        return 0;
    }
    
    if (atomspace_get_type(as, id2) != ATOM_TYPE_PREDICATE) {
        atomspace_destroy(as);
        return 0;
    }
    
    atomspace_destroy(as);
    return 1;
}

/*
 * Test: Add links
 */
int test_add_links(void)
{
    AtomSpace *as = atomspace_create();
    if (as == NULL) return 0;
    
    uint32_t cat = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "cat");
    uint32_t animal = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "animal");
    
    uint32_t outgoing[2] = {cat, animal};
    uint32_t link_id = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, outgoing, 2);
    
    if (link_id == 0) {
        atomspace_destroy(as);
        return 0;
    }
    
    /* Verify link type */
    if (atomspace_get_type(as, link_id) != ATOM_TYPE_INHERITANCE) {
        atomspace_destroy(as);
        return 0;
    }
    
    atomspace_destroy(as);
    return 1;
}

/*
 * Test: Remove atoms
 */
int test_remove_atoms(void)
{
    AtomSpace *as = atomspace_create();
    if (as == NULL) return 0;
    
    uint32_t id = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "temporary");
    if (id == 0) {
        atomspace_destroy(as);
        return 0;
    }
    
    /* Verify atom exists */
    if (atomspace_get_name(as, id) == NULL) {
        atomspace_destroy(as);
        return 0;
    }
    
    /* Remove atom */
    atomspace_remove_atom(as, id);
    
    /* Verify atom is gone */
    if (atomspace_get_name(as, id) != NULL) {
        atomspace_destroy(as);
        return 0;
    }
    
    atomspace_destroy(as);
    return 1;
}

/*
 * Test: Global AtomSpace
 */
int test_global_atomspace(void)
{
    atomspace_init();
    
    AtomSpace *as = get_global_atomspace();
    if (as == NULL) {
        atomspace_shutdown();
        return 0;
    }
    
    uint32_t id = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "global_test");
    if (id == 0) {
        atomspace_shutdown();
        return 0;
    }
    
    atomspace_shutdown();
    return 1;
}

/*
 * Test: Many atoms
 */
int test_many_atoms(void)
{
    AtomSpace *as = atomspace_create();
    if (as == NULL) return 0;
    
    char name[32];
    int i;
    
    /* Add 1000 atoms */
    for (i = 0; i < 1000; i++) {
        snprintf(name, sizeof(name), "atom_%d", i);
        uint32_t id = atomspace_add_node(as, ATOM_TYPE_CONCEPT, name);
        if (id == 0) {
            atomspace_destroy(as);
            return 0;
        }
    }
    
    atomspace_destroy(as);
    return 1;
}

int main(int argc, char **argv)
{
    printf("=== AtomSpace Test Suite ===\n\n");
    
    TEST(create_destroy);
    TEST(add_nodes);
    TEST(get_name);
    TEST(get_type);
    TEST(add_links);
    TEST(remove_atoms);
    TEST(global_atomspace);
    TEST(many_atoms);
    
    printf("\n=== Results: %d/%d tests passed ===\n", tests_passed, tests_run);
    
    return (tests_passed == tests_run) ? 0 : 1;
}
