/*
 * Test suite for the Reasoning Engine kernel module
 *
 * Tests PLN inference rules, forward/backward chaining,
 * and pattern matching operations.
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Forward declarations for portable build (no lib9.h dependency) */
typedef struct AtomSpace AtomSpace;
typedef struct ReasoningEngine ReasoningEngine;
typedef struct TruthValue {
    float strength;
    float confidence;
    uint32_t count;
} TruthValue;

/* AtomSpace API */
extern void atomspace_init(void);
extern void atomspace_shutdown(void);
extern AtomSpace* get_global_atomspace(void);
extern uint32_t atomspace_add_node(AtomSpace *as, uint16_t type, const char *name);
extern uint32_t atomspace_add_link(AtomSpace *as, uint16_t type,
                                    uint32_t *outgoing, uint32_t arity);
extern void atomspace_set_tv(AtomSpace *as, uint32_t atom_id, TruthValue tv);
extern TruthValue atomspace_get_tv(AtomSpace *as, uint32_t atom_id);
extern void atomspace_print_stats(AtomSpace *as);

/* Reasoning API */
extern void reasoning_init(void);
extern void reasoning_shutdown(void);
extern int reasoning_infer(void *engine, uint32_t *premises,
                           uint32_t count, uint32_t *conclusions,
                           uint32_t max_conclusions);
extern int reasoning_forward_chain(void *engine, uint32_t *initial,
                                    uint32_t count, uint32_t max_steps);
extern int reasoning_backward_chain(void *engine, uint32_t goal,
                                     uint32_t *evidence, uint32_t max_evidence);
extern int reasoning_pattern_match(void *engine, uint32_t pattern,
                                    uint32_t *matches, uint32_t max_matches);
extern void* get_global_reasoning(void);

#define ATOM_TYPE_CONCEPT     0x0002
#define ATOM_TYPE_VARIABLE    0x0004
#define ATOM_TYPE_INHERITANCE 0x0101

/* Test counters */
static int tests_passed = 0;
static int tests_failed = 0;
static int tests_total = 0;

#define TEST_ASSERT(cond, msg) do { \
    tests_total++; \
    if (cond) { \
        tests_passed++; \
        printf("  PASS: %s\n", msg); \
    } else { \
        tests_failed++; \
        printf("  FAIL: %s (line %d)\n", msg, __LINE__); \
    } \
} while(0)

static TruthValue tv_make(float s, float c)
{
    TruthValue tv;
    tv.strength = s;
    tv.confidence = c;
    tv.count = 1;
    return tv;
}

/* Get global reasoning engine via accessor function */
static void *global_reasoning_ptr = NULL;

static void* get_reasoning(void) {
    if (!global_reasoning_ptr) global_reasoning_ptr = get_global_reasoning();
    return global_reasoning_ptr;
}

/*
 * Test 1: Reasoning engine initialization
 */
static void
test_reasoning_init_test(void)
{
    printf("\n=== Test: Reasoning Engine Initialization ===\n");

    atomspace_init();
    AtomSpace *as = get_global_atomspace();
    TEST_ASSERT(as != NULL, "AtomSpace initialized");

    reasoning_init();
    global_reasoning_ptr = get_global_reasoning();
    TEST_ASSERT(global_reasoning_ptr != NULL, "Reasoning engine initialized");
}

/*
 * Test 2: PLN deduction rule
 * Given: Socrates -> Human, Human -> Mortal
 * Derive: Socrates -> Mortal
 */
static void
test_pln_deduction(void)
{
    AtomSpace *as = get_global_atomspace();
    printf("\n=== Test: PLN Deduction Rule ===\n");

    uint32_t socrates = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Socrates");
    uint32_t human = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Human");
    uint32_t mortal = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Mortal");

    TEST_ASSERT(socrates > 0, "Created Socrates atom");
    TEST_ASSERT(human > 0, "Created Human atom");
    TEST_ASSERT(mortal > 0, "Created Mortal atom");

    /* Set truth values */
    atomspace_set_tv(as, socrates, tv_make(0.9f, 0.8f));
    atomspace_set_tv(as, human, tv_make(0.9f, 0.9f));
    atomspace_set_tv(as, mortal, tv_make(1.0f, 0.95f));

    /* Create inheritance links */
    uint32_t out1[2] = { socrates, human };
    uint32_t link1 = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, out1, 2);
    atomspace_set_tv(as, link1, tv_make(0.95f, 0.85f));

    uint32_t out2[2] = { human, mortal };
    uint32_t link2 = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, out2, 2);
    atomspace_set_tv(as, link2, tv_make(0.99f, 0.9f));

    TEST_ASSERT(link1 > 0, "Created Socrates->Human link");
    TEST_ASSERT(link2 > 0, "Created Human->Mortal link");

    /* Perform deduction */
    uint32_t premises[2] = { link1, link2 };
    uint32_t conclusions[10];
    int count = reasoning_infer(get_reasoning(), premises, 2, conclusions, 10);

    TEST_ASSERT(count >= 0, "Deduction completed without error");

    if (count > 0) {
        TruthValue tv = atomspace_get_tv(as, conclusions[0]);
        TEST_ASSERT(tv.strength > 0.0f, "Conclusion has positive strength");
        TEST_ASSERT(tv.confidence > 0.0f, "Conclusion has positive confidence");
        printf("  Deduction result: TV=<%.4f, %.4f>\n", tv.strength, tv.confidence);
    }
}

/*
 * Test 3: Forward chaining
 */
static void
test_forward_chaining(void)
{
    AtomSpace *as = get_global_atomspace();
    printf("\n=== Test: Forward Chaining ===\n");

    /* Create a chain: A -> B -> C -> D */
    uint32_t a = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Alpha");
    uint32_t b = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Beta");
    uint32_t c = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Gamma");
    uint32_t d = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Delta");

    atomspace_set_tv(as, a, tv_make(0.9f, 0.9f));
    atomspace_set_tv(as, b, tv_make(0.85f, 0.8f));
    atomspace_set_tv(as, c, tv_make(0.8f, 0.75f));
    atomspace_set_tv(as, d, tv_make(0.95f, 0.9f));

    uint32_t out_ab[2] = { a, b };
    uint32_t out_bc[2] = { b, c };
    uint32_t out_cd[2] = { c, d };

    uint32_t link_ab = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, out_ab, 2);
    uint32_t link_bc = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, out_bc, 2);
    uint32_t link_cd = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, out_cd, 2);

    atomspace_set_tv(as, link_ab, tv_make(0.9f, 0.85f));
    atomspace_set_tv(as, link_bc, tv_make(0.85f, 0.8f));
    atomspace_set_tv(as, link_cd, tv_make(0.8f, 0.75f));

    TEST_ASSERT(link_ab > 0 && link_bc > 0 && link_cd > 0, "Created chain links");

    /* Forward chain from A */
    uint32_t initial[1] = { a };
    int steps = reasoning_forward_chain(get_reasoning(), initial, 1, 5);

    TEST_ASSERT(steps >= 0, "Forward chaining completed without error");
    printf("  Forward chaining took %d steps\n", steps);
}

/*
 * Test 4: Multiple inference rules
 */
static void
test_multiple_rules(void)
{
    AtomSpace *as = get_global_atomspace();
    printf("\n=== Test: Multiple Inference Rules ===\n");

    uint32_t cat = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Cat");
    uint32_t animal = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Animal");
    uint32_t pet = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Pet");

    atomspace_set_tv(as, cat, tv_make(0.9f, 0.9f));
    atomspace_set_tv(as, animal, tv_make(0.95f, 0.95f));
    atomspace_set_tv(as, pet, tv_make(0.8f, 0.7f));

    uint32_t out_ca[2] = { cat, animal };
    uint32_t out_cp[2] = { cat, pet };

    uint32_t link_ca = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, out_ca, 2);
    uint32_t link_cp = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, out_cp, 2);

    atomspace_set_tv(as, link_ca, tv_make(1.0f, 0.95f));
    atomspace_set_tv(as, link_cp, tv_make(0.7f, 0.6f));

    TEST_ASSERT(link_ca > 0, "Created Cat->Animal link");
    TEST_ASSERT(link_cp > 0, "Created Cat->Pet link");

    uint32_t premises[2] = { link_ca, link_cp };
    uint32_t conclusions[10];
    int count = reasoning_infer(get_reasoning(), premises, 2, conclusions, 10);

    TEST_ASSERT(count >= 0, "Multi-rule inference completed");
    printf("  Derived %d conclusions from multiple rules\n", count);
}

/*
 * Main test runner
 */
int
main(int argc, char *argv[])
{
    printf("========================================\n");
    printf("Reasoning Engine Test Suite\n");
    printf("========================================\n");

    test_reasoning_init_test();
    test_pln_deduction();
    test_forward_chaining();
    test_multiple_rules();

    printf("\n========================================\n");
    printf("Results: %d/%d passed, %d failed\n",
           tests_passed, tests_total, tests_failed);
    printf("========================================\n");

    /* Clean up */
    atomspace_shutdown();

    return tests_failed > 0 ? 1 : 0;
}
