/*
 * Cognitive Kernel Test Suite
 * 
 * Comprehensive tests for all cognitive kernel modules:
 * - AtomSpace operations
 * - Reasoning engine
 * - Attention allocation
 * - Cognitive 9P filesystem
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../atomspace/atomspace.h"
#include "../reasoning/reasoning.h"
#include "../attention/attention.h"
#include "../cognitive9p/cognitive9p.h"

/* Test counters */
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* Test macros */
#define TEST(name) \
    printf("\n=== Test: %s ===\n", name); \
    tests_run++;

#define ASSERT(condition, message) \
    if (!(condition)) { \
        printf("  ❌ FAILED: %s\n", message); \
        tests_failed++; \
        return -1; \
    } else { \
        printf("  ✓ %s\n", message); \
    }

#define TEST_PASS() \
    printf("  ✅ Test passed\n"); \
    tests_passed++; \
    return 0;

/* Forward declarations */
int test_atomspace_basic(void);
int test_atomspace_links(void);
int test_atomspace_truth_values(void);
int test_reasoning_rules(void);
int test_reasoning_inference(void);
int test_attention_stimulation(void);
int test_attention_spreading(void);
int test_cognitive9p_filesystem(void);
int test_integration(void);

/*
 * Main test runner
 */
int
main(int argc, char *argv[])
{
    printf("\n");
    printf("========================================\n");
    printf("Cognitive Kernel Test Suite\n");
    printf("========================================\n");
    
    /* Initialize cognitive kernel */
    printf("\nInitializing cognitive kernel...\n");
    cognitive_kernel_init();
    
    /* Run test suites */
    printf("\n");
    printf("========================================\n");
    printf("Running Tests\n");
    printf("========================================\n");
    
    /* AtomSpace tests */
    test_atomspace_basic();
    test_atomspace_links();
    test_atomspace_truth_values();
    
    /* Reasoning tests */
    test_reasoning_rules();
    test_reasoning_inference();
    
    /* Attention tests */
    test_attention_stimulation();
    test_attention_spreading();
    
    /* Cognitive9P tests */
    test_cognitive9p_filesystem();
    
    /* Integration tests */
    test_integration();
    
    /* Print summary */
    printf("\n");
    printf("========================================\n");
    printf("Test Summary\n");
    printf("========================================\n");
    printf("Tests run:    %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("\n");
    
    if (tests_failed == 0) {
        printf("✅ All tests passed!\n");
        return 0;
    } else {
        printf("❌ Some tests failed\n");
        return 1;
    }
}

/*
 * Test: AtomSpace basic operations
 */
int
test_atomspace_basic(void)
{
    TEST("AtomSpace Basic Operations");
    
    AtomSpace *as = atomspace_create();
    ASSERT(as != NULL, "Create AtomSpace");
    
    /* Create nodes */
    u32int atom1 = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Human");
    ASSERT(atom1 != 0, "Create ConceptNode 'Human'");
    
    u32int atom2 = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Mortal");
    ASSERT(atom2 != 0, "Create ConceptNode 'Mortal'");
    
    u32int atom3 = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Socrates");
    ASSERT(atom3 != 0, "Create ConceptNode 'Socrates'");
    
    /* Retrieve atoms */
    Atom *retrieved = atomspace_get_atom(as, atom1);
    ASSERT(retrieved != NULL, "Retrieve atom by ID");
    ASSERT(retrieved->id == atom1, "Retrieved atom has correct ID");
    ASSERT(retrieved->type == ATOM_TYPE_CONCEPT, "Retrieved atom has correct type");
    
    /* Check statistics */
    ASSERT(as->atom_count == 3, "AtomSpace has 3 atoms");
    ASSERT(as->node_count == 3, "AtomSpace has 3 nodes");
    ASSERT(as->link_count == 0, "AtomSpace has 0 links");
    
    atomspace_destroy(as);
    
    TEST_PASS();
}

/*
 * Test: AtomSpace link operations
 */
int
test_atomspace_links(void)
{
    TEST("AtomSpace Link Operations");
    
    AtomSpace *as = atomspace_create();
    ASSERT(as != NULL, "Create AtomSpace");
    
    /* Create nodes */
    u32int human = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Human");
    u32int mortal = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Mortal");
    u32int socrates = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Socrates");
    
    /* Create links */
    u32int outgoing1[2] = {socrates, human};
    u32int link1 = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, outgoing1, 2);
    ASSERT(link1 != 0, "Create InheritanceLink (Socrates -> Human)");
    
    u32int outgoing2[2] = {human, mortal};
    u32int link2 = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, outgoing2, 2);
    ASSERT(link2 != 0, "Create InheritanceLink (Human -> Mortal)");
    
    /* Retrieve link */
    Atom *retrieved_link = atomspace_get_atom(as, link1);
    ASSERT(retrieved_link != NULL, "Retrieve link by ID");
    ASSERT(retrieved_link->type == ATOM_TYPE_INHERITANCE, "Link has correct type");
    
    LinkData *link_data = (LinkData*)retrieved_link->data;
    ASSERT(link_data->arity == 2, "Link has correct arity");
    ASSERT(link_data->outgoing[0] == socrates, "Link has correct first outgoing");
    ASSERT(link_data->outgoing[1] == human, "Link has correct second outgoing");
    
    /* Check incoming sets */
    Atom *human_atom = atomspace_get_atom(as, human);
    ASSERT(human_atom->incoming == 2, "Human has 2 incoming links");
    
    /* Check statistics */
    ASSERT(as->atom_count == 5, "AtomSpace has 5 atoms");
    ASSERT(as->node_count == 3, "AtomSpace has 3 nodes");
    ASSERT(as->link_count == 2, "AtomSpace has 2 links");
    
    atomspace_destroy(as);
    
    TEST_PASS();
}

/*
 * Test: Truth value operations
 */
int
test_atomspace_truth_values(void)
{
    TEST("Truth Value Operations");
    
    /* Create truth values */
    TruthValue tv1 = tv_create(0.8, 0.9);
    ASSERT(tv1.strength == 0.8f, "TV strength is 0.8");
    ASSERT(tv1.confidence == 0.9f, "TV confidence is 0.9");
    ASSERT(tv1.count == 1, "TV count is 1");
    
    TruthValue tv2 = tv_default();
    ASSERT(tv2.strength == 0.0f, "Default TV strength is 0.0");
    ASSERT(tv2.confidence == 0.0f, "Default TV confidence is 0.0");
    
    /* Create attention values */
    AttentionValue av1 = av_create(100, 50, 10);
    ASSERT(av1.sti == 100, "AV STI is 100");
    ASSERT(av1.lti == 50, "AV LTI is 50");
    ASSERT(av1.vlti == 10, "AV VLTI is 10");
    
    AttentionValue av2 = av_default();
    ASSERT(av2.sti == 0, "Default AV STI is 0");
    ASSERT(av2.lti == 0, "Default AV LTI is 0");
    
    TEST_PASS();
}

/*
 * Test: Reasoning rule management
 */
int
test_reasoning_rules(void)
{
    TEST("Reasoning Rule Management");
    
    ReasoningEngine *re = reasoning_create();
    ASSERT(re != NULL, "Create reasoning engine");
    
    /* Initialize PLN and URE */
    pln_init(re);
    ure_init(re);
    
    ASSERT(re->rule_count > 0, "Rules were added");
    
    /* Check that rules exist */
    Rule *rule = re->rules;
    int rule_count = 0;
    while (rule != NULL) {
        rule_count++;
        ASSERT(rule->name != NULL, "Rule has a name");
        ASSERT(rule->func != NULL, "Rule has a function");
        rule = rule->next;
    }
    
    ASSERT(rule_count == re->rule_count, "Rule count matches");
    
    reasoning_destroy(re);
    
    TEST_PASS();
}

/*
 * Test: Reasoning inference
 */
int
test_reasoning_inference(void)
{
    TEST("Reasoning Inference");
    
    AtomSpace *as = atomspace_create();
    ReasoningEngine *re = reasoning_create();
    
    /* Create test atoms */
    u32int human = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Human");
    u32int mortal = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Mortal");
    u32int socrates = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Socrates");
    
    u32int outgoing1[2] = {socrates, human};
    u32int link1 = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, outgoing1, 2);
    
    u32int outgoing2[2] = {human, mortal};
    u32int link2 = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, outgoing2, 2);
    
    /* Perform inference */
    u32int premises[2] = {link1, link2};
    u32int conclusions[10];
    int count = reasoning_infer(re, premises, 2, conclusions, 10);
    
    ASSERT(count >= 0, "Inference completed without error");
    
    /* Forward chaining */
    u32int initial[2] = {socrates, human};
    int inferred = reasoning_forward_chain(re, initial, 2, 5);
    ASSERT(inferred >= 0, "Forward chaining completed");
    
    atomspace_destroy(as);
    reasoning_destroy(re);
    
    TEST_PASS();
}

/*
 * Test: Attention stimulation
 */
int
test_attention_stimulation(void)
{
    TEST("Attention Stimulation");
    
    AtomSpace *as = atomspace_create();
    AttentionAllocator *aa = attention_create();
    aa->atomspace = as;
    
    /* Create test atoms */
    u32int atom1 = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Test1");
    u32int atom2 = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Test2");
    
    /* Get initial STI */
    Atom *a1 = atomspace_get_atom(as, atom1);
    s16int initial_sti = a1->av.sti;
    
    /* Stimulate atom */
    int result = attention_stimulate(aa, atom1, 100);
    ASSERT(result == 0, "Stimulation succeeded");
    
    /* Check STI increased */
    ASSERT(a1->av.sti == initial_sti + 100, "STI increased by 100");
    
    /* Check STI funds decreased */
    ASSERT(aa->sti_funds == DEFAULT_STI_FUNDS - 100, "STI funds decreased");
    
    attention_destroy(aa);
    atomspace_destroy(as);
    
    TEST_PASS();
}

/*
 * Test: Importance spreading
 */
int
test_attention_spreading(void)
{
    TEST("Importance Spreading");
    
    AtomSpace *as = atomspace_create();
    AttentionAllocator *aa = attention_create();
    aa->atomspace = as;
    
    /* Create test atoms and link */
    u32int atom1 = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Source");
    u32int atom2 = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Target");
    
    u32int outgoing[2] = {atom1, atom2};
    u32int link = atomspace_add_link(as, ATOM_TYPE_SIMILARITY, outgoing, 2);
    
    /* Stimulate source */
    attention_stimulate(aa, atom1, 1000);
    
    /* Get initial target STI */
    Atom *target = atomspace_get_atom(as, atom2);
    s16int initial_sti = target->av.sti;
    
    /* Spread importance */
    attention_spread_importance(aa, link);
    
    /* Check target STI increased */
    ASSERT(target->av.sti > initial_sti, "Target STI increased after spreading");
    
    attention_destroy(aa);
    atomspace_destroy(as);
    
    TEST_PASS();
}

/*
 * Test: Cognitive 9P filesystem
 */
int
test_cognitive9p_filesystem(void)
{
    TEST("Cognitive 9P Filesystem");
    
    /* Initialize filesystem */
    cognitive9p_init();
    
    /* Test filesystem tree */
    printf("  Printing filesystem tree:\n");
    print_cogfs_tree();
    
    /* Test atom creation through filesystem */
    u32int atom_id = cognitive9p_create_atom("ConceptNode", "TestAtom");
    ASSERT(atom_id != 0, "Created atom through filesystem");
    
    /* Test query */
    char buffer[1024];
    long bytes = cognitive9p_query_atoms("ConceptNode", buffer, sizeof(buffer));
    ASSERT(bytes > 0, "Query returned results");
    
    TEST_PASS();
}

/*
 * Test: Integration test
 */
int
test_integration(void)
{
    TEST("Integration Test");
    
    /* This test validates the entire cognitive stack working together */
    
    AtomSpace *as = get_global_atomspace();
    ASSERT(as != NULL, "Global AtomSpace is initialized");
    
    /* Create knowledge base */
    u32int human = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Human");
    u32int mortal = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Mortal");
    u32int socrates = atomspace_add_node(as, ATOM_TYPE_CONCEPT, "Socrates");
    
    u32int outgoing1[2] = {socrates, human};
    atomspace_add_link(as, ATOM_TYPE_INHERITANCE, outgoing1, 2);
    
    u32int outgoing2[2] = {human, mortal};
    atomspace_add_link(as, ATOM_TYPE_INHERITANCE, outgoing2, 2);
    
    /* Apply attention */
    extern AttentionAllocator *global_attention;
    if (global_attention != NULL) {
        attention_stimulate(global_attention, socrates, 200);
        attention_stimulate(global_attention, human, 150);
        attention_stimulate(global_attention, mortal, 100);
    }
    
    /* Perform reasoning */
    extern ReasoningEngine *global_reasoning;
    if (global_reasoning != NULL) {
        u32int premises[2] = {socrates, human};
        u32int conclusions[10];
        reasoning_infer(global_reasoning, premises, 2, conclusions, 10);
    }
    
    /* Print final statistics */
    print_cognitive_stats();
    
    TEST_PASS();
}
