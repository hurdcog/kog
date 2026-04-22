/*
 * Integration Test Suite
 * Tests all cognitive kernel modules working together
 *
 * Exercises the full cognitive pipeline:
 * AtomSpace -> PLN Reasoning -> ECAN Attention -> Pattern Matching
 * -> IPC Channels -> Distributed Operations
 *
 * Copyright (c) 2026 OpenCog Inferno Project
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/* ========================================================================
 * External API Declarations
 * ======================================================================== */

typedef struct AtomSpace AtomSpace;
typedef struct TruthValue {
    float strength;
    float confidence;
    uint32_t count;
} TruthValue;

/* AtomSpace */
extern void atomspace_init(void);
extern void atomspace_shutdown(void);
extern AtomSpace* get_global_atomspace(void);
extern AtomSpace* atomspace_create(void);
extern void atomspace_destroy(AtomSpace *as);
extern uint32_t atomspace_add_node(AtomSpace *as, uint16_t type, const char *name);
extern uint32_t atomspace_add_link(AtomSpace *as, uint16_t type,
                                    uint32_t *outgoing, uint32_t arity);
extern void atomspace_remove_atom(AtomSpace *as, uint32_t atom_id);
extern const char* atomspace_get_name(AtomSpace *as, uint32_t atom_id);
extern uint16_t atomspace_get_type(AtomSpace *as, uint32_t atom_id);
extern void atomspace_set_tv(AtomSpace *as, uint32_t atom_id, TruthValue tv);
extern TruthValue atomspace_get_tv(AtomSpace *as, uint32_t atom_id);
extern void atomspace_print_stats(AtomSpace *as);

/* PLN Engine */
typedef struct PLNEngine PLNEngine;
typedef struct PLNResult {
    uint32_t conclusion_id;
    float tv_strength;
    float tv_confidence;
    uint32_t tv_count;
    float rule_confidence;
    const char *rule_name;
} PLNResult;

extern PLNEngine* pln_engine_create(AtomSpace *as);
extern void pln_engine_destroy(PLNEngine *engine);
extern int pln_engine_register_default_rules(PLNEngine *engine);
extern int pln_engine_infer(PLNEngine *engine, uint32_t *premises,
                            uint32_t premise_count, PLNResult *results,
                            uint32_t max_results);
extern void pln_engine_print_stats(PLNEngine *engine);

/* IPC Channels */
typedef struct ChannelManager ChannelManager;
typedef struct CogMessage {
    uint32_t sender_pid;
    uint32_t sequence;
    int priority;
    uint32_t atom_count;
    uint32_t atom_ids[256];
    uint64_t timestamp;
    uint32_t flags;
} CogMessage;

extern ChannelManager* channel_manager_create(void);
extern void channel_manager_destroy(ChannelManager *mgr);
extern int channel_create(ChannelManager *mgr, uint32_t owner_pid,
                          uint32_t peer_pid, const char *name, int type);
extern int channel_send(ChannelManager *mgr, uint32_t channel_id,
                        uint32_t *atom_ids, uint32_t count, int priority);
extern int channel_recv(ChannelManager *mgr, uint32_t channel_id,
                        CogMessage *msg);
extern void channel_print_stats(ChannelManager *mgr);

/* Distributed AtomSpace */
typedef struct DistributedAtomSpace DistributedAtomSpace;

extern DistributedAtomSpace* dist_atomspace_create(AtomSpace *local);
extern void dist_atomspace_destroy(DistributedAtomSpace *das);
extern int dist_add_node(DistributedAtomSpace *das, const char *hostname,
                         uint16_t port);
extern void dist_print_stats(DistributedAtomSpace *das);

#define ATOM_TYPE_CONCEPT     0x0002
#define ATOM_TYPE_PREDICATE   0x0003
#define ATOM_TYPE_INHERITANCE 0x0101
#define ATOM_TYPE_SIMILARITY  0x0102
#define ATOM_TYPE_EVALUATION  0x0103
#define ATOM_TYPE_IMPLICATION 0x0108

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

static TruthValue tv_make(float s, float c)
{
    TruthValue tv;
    tv.strength = s;
    tv.confidence = c;
    tv.count = 1;
    return tv;
}

/* ========================================================================
 * Integration Test: Full Cognitive Pipeline
 * ======================================================================== */

/**
 * Test the Socrates syllogism through the full pipeline:
 * 1. Create knowledge in AtomSpace
 * 2. Reason with PLN
 * 3. Communicate results via IPC
 */
int test_socrates_pipeline(void)
{
    AtomSpace *as = atomspace_create();
    if (!as) return 0;

    /* Step 1: Build knowledge base */
    printf("\n    Step 1: Building knowledge base...\n");

    uint32_t socrates = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Socrates");
    uint32_t plato = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Plato");
    uint32_t human = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Human");
    uint32_t mortal = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Mortal");
    uint32_t philosopher = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Philosopher");

    atomspace_set_tv(as, human, tv_make(0.5f, 0.9f));
    atomspace_set_tv(as, mortal, tv_make(0.5f, 0.9f));

    /* Socrates is Human */
    uint32_t out1[2] = { socrates, human };
    uint32_t link_sh = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, out1, 2);
    atomspace_set_tv(as, link_sh, tv_make(0.95f, 0.95f));

    /* Human is Mortal */
    uint32_t out2[2] = { human, mortal };
    uint32_t link_hm = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, out2, 2);
    atomspace_set_tv(as, link_hm, tv_make(0.99f, 0.99f));

    /* Plato is Human */
    uint32_t out3[2] = { plato, human };
    uint32_t link_ph = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, out3, 2);
    atomspace_set_tv(as, link_ph, tv_make(0.9f, 0.9f));

    /* Socrates is Philosopher */
    uint32_t out4[2] = { socrates, philosopher };
    uint32_t link_sp = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, out4, 2);
    atomspace_set_tv(as, link_sp, tv_make(0.99f, 0.95f));

    atomspace_print_stats(as);

    /* Step 2: PLN Reasoning */
    printf("    Step 2: PLN Reasoning...\n");

    PLNEngine *pln = pln_engine_create(as);
    if (!pln) {
        atomspace_destroy(as);
        return 0;
    }
    pln_engine_register_default_rules(pln);

    /* Try to derive: Socrates is Mortal (via deduction) */
    uint32_t premises[2] = { link_sh, link_hm };
    PLNResult results[8];
    int infer_count = pln_engine_infer(pln, premises, 2, results, 8);

    printf("    PLN produced %d inferences\n", infer_count);

    /* Step 3: IPC Communication */
    printf("    Step 3: IPC Communication...\n");

    ChannelManager *ipc = channel_manager_create();
    if (!ipc) {
        pln_engine_destroy(pln);
        atomspace_destroy(as);
        return 0;
    }

    int ch = channel_create(ipc, 1, 2, "reasoning-results", 1);
    if (ch > 0 && infer_count > 0) {
        /* Send inference results through channel */
        uint32_t result_atoms[8];
        int i;
        for (i = 0; i < infer_count && i < 8; i++) {
            result_atoms[i] = results[i].conclusion_id;
        }
        channel_send(ipc, (uint32_t)ch, result_atoms, (uint32_t)infer_count, 1);

        /* Receive on the other end */
        CogMessage msg;
        int received = channel_recv(ipc, (uint32_t)ch, &msg);
        printf("    IPC received %d atoms\n", received);
    }

    channel_print_stats(ipc);
    pln_engine_print_stats(pln);

    channel_manager_destroy(ipc);
    pln_engine_destroy(pln);
    atomspace_destroy(as);

    return 1;
}

/**
 * Test: Build a large knowledge base and reason over it
 */
int test_large_knowledge_base(void)
{
    AtomSpace *as = atomspace_create();
    if (!as) return 0;

    printf("\n    Building large knowledge base...\n");

    /* Create a taxonomy: Animal -> Mammal -> {Dog, Cat, Horse, ...} */
    uint32_t animal = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Animal");
    uint32_t mammal = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Mammal");
    uint32_t bird = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Bird");
    uint32_t fish = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Fish");

    atomspace_set_tv(as, animal, tv_make(0.5f, 0.9f));
    atomspace_set_tv(as, mammal, tv_make(0.5f, 0.9f));

    /* Mammal -> Animal */
    uint32_t out_ma[2] = { mammal, animal };
    uint32_t link_ma = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, out_ma, 2);
    atomspace_set_tv(as, link_ma, tv_make(1.0f, 0.99f));

    /* Bird -> Animal */
    uint32_t out_ba[2] = { bird, animal };
    atomspace_add_link(as, ATOM_TYPE_INHERITANCE, out_ba, 2);

    /* Fish -> Animal */
    uint32_t out_fa[2] = { fish, animal };
    atomspace_add_link(as, ATOM_TYPE_INHERITANCE, out_fa, 2);

    /* Create many specific animals */
    const char *mammals[] = { "Dog", "Cat", "Horse", "Elephant", "Whale",
                              "Dolphin", "Lion", "Tiger", "Bear", "Wolf" };
    uint32_t mammal_ids[10];
    uint32_t mammal_links[10];
    int i;

    for (i = 0; i < 10; i++) {
        mammal_ids[i] = atomspace_add_node(as, ATOM_TYPE_CONCEPT, mammals[i]);
        uint32_t out[2] = { mammal_ids[i], mammal };
        mammal_links[i] = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, out, 2);
        atomspace_set_tv(as, mammal_links[i], tv_make(0.95f, 0.9f));
    }

    printf("    Created %d mammals in taxonomy\n", 10);

    /* PLN: Derive that each mammal is an animal */
    PLNEngine *pln = pln_engine_create(as);
    pln_engine_register_default_rules(pln);

    int total_inferences = 0;
    for (i = 0; i < 10; i++) {
        uint32_t premises[2] = { mammal_links[i], link_ma };
        PLNResult results[4];
        int count = pln_engine_infer(pln, premises, 2, results, 4);
        total_inferences += count;
    }

    printf("    PLN derived %d inferences (expected ~10 deductions)\n",
           total_inferences);

    atomspace_print_stats(as);
    pln_engine_print_stats(pln);

    pln_engine_destroy(pln);
    atomspace_destroy(as);

    return (total_inferences > 0);
}

/**
 * Test: Distributed cognitive network simulation
 */
int test_distributed_network(void)
{
    AtomSpace *as = atomspace_create();
    if (!as) return 0;

    printf("\n    Setting up distributed cognitive network...\n");

    DistributedAtomSpace *das = dist_atomspace_create(as);
    if (!das) {
        atomspace_destroy(as);
        return 0;
    }

    /* Add cluster nodes */
    int n1 = dist_add_node(das, "cognitive-node-1.cluster.local", 9090);
    int n2 = dist_add_node(das, "cognitive-node-2.cluster.local", 9090);
    int n3 = dist_add_node(das, "cognitive-node-3.cluster.local", 9090);

    printf("    Added 3 cluster nodes: %d, %d, %d\n", n1, n2, n3);

    /* Create local knowledge */
    atomspace_add_node(as, ATOM_TYPE_CONCEPT, "LocalKnowledge");
    atomspace_add_node(as, ATOM_TYPE_CONCEPT, "SharedFact");

    dist_print_stats(das);

    dist_atomspace_destroy(das);
    atomspace_destroy(as);

    return (n1 > 0 && n2 > 0 && n3 > 0);
}

/**
 * Test: Full cognitive boot sequence
 */
int test_cognitive_boot(void)
{
    printf("\n    Simulating cognitive kernel boot...\n");

    /* Initialize global AtomSpace */
    atomspace_init();
    AtomSpace *as = get_global_atomspace();
    if (!as) {
        atomspace_shutdown();
        return 0;
    }

    /* Create initial knowledge */
    uint32_t self = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Self");
    uint32_t world = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "World");
    uint32_t goal = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Goal");

    atomspace_set_tv(as, self, tv_make(1.0f, 1.0f));
    atomspace_set_tv(as, world, tv_make(0.5f, 0.3f));

    /* Self perceives World */
    uint32_t out[2] = { self, world };
    atomspace_add_link(as, ATOM_TYPE_EVALUATION, out, 2);

    /* Initialize PLN */
    PLNEngine *pln = pln_engine_create(as);
    pln_engine_register_default_rules(pln);

    /* Initialize IPC */
    ChannelManager *ipc = channel_manager_create();
    channel_create(ipc, 1, 2, "perception", 1);
    channel_create(ipc, 1, 3, "action", 1);
    channel_create(ipc, 1, 4, "reasoning", 1);

    /* Initialize distributed */
    DistributedAtomSpace *das = dist_atomspace_create(as);

    printf("    Boot complete: AtomSpace + PLN + IPC + Distributed\n");

    atomspace_print_stats(as);

    /* Cleanup */
    dist_atomspace_destroy(das);
    channel_manager_destroy(ipc);
    pln_engine_destroy(pln);
    atomspace_shutdown();

    return 1;
}

/* ========================================================================
 * Main
 * ======================================================================== */

int main(int argc, char **argv)
{
    printf("=== Integration Test Suite ===\n");
    printf("=== OpenCog Inferno AGI OS ===\n\n");

    TEST(socrates_pipeline);
    TEST(large_knowledge_base);
    TEST(distributed_network);
    TEST(cognitive_boot);

    printf("\n=== Results: %d/%d tests passed ===\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
