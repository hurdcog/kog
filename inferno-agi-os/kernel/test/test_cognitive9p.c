/*
 * Test suite for the Cognitive 9P Filesystem kernel module
 *
 * Tests the 9P protocol interface to cognitive services via
 * the portable cogfs API (cogfs_read, cogfs_write, cogfs_readdir).
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Forward declarations for portable build */
typedef struct AtomSpace AtomSpace;

extern void atomspace_init(void);
extern void atomspace_shutdown(void);
extern AtomSpace* get_global_atomspace(void);
extern uint32_t atomspace_add_node(AtomSpace *as, uint16_t type, const char *name);

/* Cognitive 9P portable API */
extern void cognitive9p_init(void);
extern void cognitive9p_shutdown(void);
extern long cogfs_read(const char *path, void *buf, long n, long offset);
extern long cogfs_write(const char *path, const void *buf, long n, long offset);
extern int cogfs_readdir(const char *path, char **names, int max_names);
extern void cogfs_print_tree(void);

#define ATOM_TYPE_CONCEPT   0x0002

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

/*
 * Test 1: Cognitive 9P initialization
 */
static void
test_cognitive9p_init_test(void)
{
    printf("\n=== Test: Cognitive 9P Initialization ===\n");

    atomspace_init();
    AtomSpace *as = get_global_atomspace();
    TEST_ASSERT(as != NULL, "AtomSpace initialized");

    cognitive9p_init();
    TEST_ASSERT(1, "Cognitive 9P initialized without crash");
}

/*
 * Test 2: Print filesystem tree
 */
static void
test_cogfs_tree(void)
{
    printf("\n=== Test: Cognitive Filesystem Tree ===\n");

    cogfs_print_tree();
    TEST_ASSERT(1, "Filesystem tree printed without crash");
}

/*
 * Test 3: Read from cognitive filesystem
 */
static void
test_cogfs_read(void)
{
    char buf[1024];
    long n;

    printf("\n=== Test: CogFS Read Operations ===\n");

    /* Read stats file (a FILE node, not a directory) */
    n = cogfs_read("/atomspace/stats", buf, sizeof(buf), 0);
    TEST_ASSERT(n > 0, "Read from /atomspace/stats");
    if (n > 0) printf("  stats: %.*s", (int)n, buf);

    /* Read count file */
    n = cogfs_read("/atomspace/count", buf, sizeof(buf), 0);
    TEST_ASSERT(n > 0, "Read from /atomspace/count");

    /* Read attention allocation file */
    n = cogfs_read("/attention/allocation", buf, sizeof(buf), 0);
    TEST_ASSERT(n > 0, "Read from /attention/allocation");

    /* Read working memory capacity */
    n = cogfs_read("/memory/working/capacity", buf, sizeof(buf), 0);
    TEST_ASSERT(n > 0, "Read from /memory/working/capacity");
}

/*
 * Test 4: Write to cognitive filesystem (create atoms)
 */
static void
test_cogfs_write(void)
{
    long n;

    printf("\n=== Test: CogFS Write Operations ===\n");

    /* Write to working memory capacity (a writable file) */
    const char *cmd = "1024";
    n = cogfs_write("/memory/working/capacity", cmd, (long)strlen(cmd), 0);
    TEST_ASSERT(n > 0, "Write to /memory/working/capacity");

    /* Write to STI funds */
    const char *funds = "50000";
    n = cogfs_write("/attention/sti_funds", funds, (long)strlen(funds), 0);
    TEST_ASSERT(n > 0, "Write to /attention/sti_funds");
}

/*
 * Test 5: Directory listing
 */
static void
test_cogfs_readdir(void)
{
    char *names[32];
    int count;

    printf("\n=== Test: CogFS Directory Listing ===\n");

    count = cogfs_readdir("/", names, 32);
    TEST_ASSERT(count >= 0, "Read root directory listing");
    printf("  Root directory has %d entries\n", count);

    /* Free names if allocated */
    int i;
    for (i = 0; i < count; i++) {
        if (names[i]) {
            printf("    /%s\n", names[i]);
        }
    }
}

/*
 * Main test runner
 */
int
main(int argc, char *argv[])
{
    printf("========================================\n");
    printf("Cognitive 9P Filesystem Test Suite\n");
    printf("========================================\n");

    test_cognitive9p_init_test();
    test_cogfs_tree();
    test_cogfs_read();
    test_cogfs_write();
    test_cogfs_readdir();

    printf("\n========================================\n");
    printf("Results: %d/%d passed, %d failed\n",
           tests_passed, tests_total, tests_failed);
    printf("========================================\n");

    /* Clean up */
    atomspace_shutdown();

    return tests_failed > 0 ? 1 : 0;
}
