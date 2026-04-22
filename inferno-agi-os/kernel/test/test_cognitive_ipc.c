/*
 * Cognitive IPC & Pattern Matcher Test Suite
 *
 * Tests for inter-process communication channels and
 * the pattern matching engine.
 *
 * Copyright (c) 2026 OpenCog Inferno Project
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ========================================================================
 * External IPC API
 * ======================================================================== */

typedef struct ChannelManager ChannelManager;

typedef enum {
    CHANNEL_TYPE_LOCAL = 1,
    CHANNEL_TYPE_REMOTE,
    CHANNEL_TYPE_BROADCAST
} ChannelType;

typedef enum {
    MSG_PRIORITY_LOW = 0,
    MSG_PRIORITY_NORMAL = 1,
    MSG_PRIORITY_HIGH = 2,
    MSG_PRIORITY_URGENT = 3
} MessagePriority;

#define COG_MAX_MSG_ATOMS 256

typedef struct CogMessage {
    uint32_t sender_pid;
    uint32_t sequence;
    MessagePriority priority;
    uint32_t atom_count;
    uint32_t atom_ids[COG_MAX_MSG_ATOMS];
    uint64_t timestamp;
    uint32_t flags;
} CogMessage;

extern ChannelManager* channel_manager_create(void);
extern void channel_manager_destroy(ChannelManager *mgr);
extern int channel_create(ChannelManager *mgr, uint32_t owner_pid,
                          uint32_t peer_pid, const char *name,
                          ChannelType type);
extern int channel_close(ChannelManager *mgr, uint32_t channel_id);
extern int channel_send(ChannelManager *mgr, uint32_t channel_id,
                        uint32_t *atom_ids, uint32_t count,
                        MessagePriority priority);
extern int channel_recv(ChannelManager *mgr, uint32_t channel_id,
                        CogMessage *msg);
extern int channel_peek(ChannelManager *mgr, uint32_t channel_id);
extern int channel_broadcast(ChannelManager *mgr, uint32_t sender_pid,
                             uint32_t *atom_ids, uint32_t count);
extern void channel_print_stats(ChannelManager *mgr);

/* ========================================================================
 * External Distributed AtomSpace API
 * ======================================================================== */

typedef struct DistributedAtomSpace DistributedAtomSpace;
typedef struct AtomSpace AtomSpace;

extern AtomSpace* atomspace_create(void);
extern void atomspace_destroy(AtomSpace *as);

extern DistributedAtomSpace* dist_atomspace_create(AtomSpace *local);
extern void dist_atomspace_destroy(DistributedAtomSpace *das);
extern int dist_add_node(DistributedAtomSpace *das, const char *hostname,
                         uint16_t port);
extern int dist_ping_node(DistributedAtomSpace *das, uint32_t node_id);
extern uint32_t dist_select_node(DistributedAtomSpace *das);
extern int dist_sync_all(DistributedAtomSpace *das);
extern void dist_print_stats(DistributedAtomSpace *das);

/* ========================================================================
 * Test Framework
 * ======================================================================== */

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

/* ========================================================================
 * IPC Tests
 * ======================================================================== */

int test_channel_create_destroy(void)
{
    ChannelManager *mgr = channel_manager_create();
    if (!mgr) return 0;

    channel_manager_destroy(mgr);
    return 1;
}

int test_channel_open_close(void)
{
    ChannelManager *mgr = channel_manager_create();
    if (!mgr) return 0;

    int ch_id = channel_create(mgr, 100, 200, "test-channel",
                                CHANNEL_TYPE_LOCAL);
    if (ch_id <= 0) {
        channel_manager_destroy(mgr);
        return 0;
    }

    int result = channel_close(mgr, (uint32_t)ch_id);

    channel_manager_destroy(mgr);
    return (result == 0);
}

int test_channel_send_recv(void)
{
    ChannelManager *mgr = channel_manager_create();
    if (!mgr) return 0;

    int ch_id = channel_create(mgr, 100, 200, "data-channel",
                                CHANNEL_TYPE_LOCAL);
    if (ch_id <= 0) {
        channel_manager_destroy(mgr);
        return 0;
    }

    /* Send some atom IDs */
    uint32_t atoms[3] = { 42, 43, 44 };
    int sent = channel_send(mgr, (uint32_t)ch_id, atoms, 3,
                             MSG_PRIORITY_NORMAL);
    if (sent <= 0) {
        channel_manager_destroy(mgr);
        return 0;
    }

    /* Check pending messages */
    int pending = channel_peek(mgr, (uint32_t)ch_id);
    if (pending != 1) {
        printf("    Expected 1 pending, got %d ", pending);
        channel_manager_destroy(mgr);
        return 0;
    }

    /* Receive */
    CogMessage msg;
    int received = channel_recv(mgr, (uint32_t)ch_id, &msg);
    if (received <= 0) {
        channel_manager_destroy(mgr);
        return 0;
    }

    /* Verify message content */
    int success = (msg.atom_count == 3 &&
                   msg.atom_ids[0] == 42 &&
                   msg.atom_ids[1] == 43 &&
                   msg.atom_ids[2] == 44);

    channel_manager_destroy(mgr);
    return success;
}

int test_channel_priority(void)
{
    ChannelManager *mgr = channel_manager_create();
    if (!mgr) return 0;

    int ch_id = channel_create(mgr, 100, 200, "priority-channel",
                                CHANNEL_TYPE_LOCAL);
    if (ch_id <= 0) {
        channel_manager_destroy(mgr);
        return 0;
    }

    /* Send messages with different priorities */
    uint32_t low_atoms[1] = { 1 };
    uint32_t high_atoms[1] = { 2 };

    channel_send(mgr, (uint32_t)ch_id, low_atoms, 1, MSG_PRIORITY_LOW);
    channel_send(mgr, (uint32_t)ch_id, high_atoms, 1, MSG_PRIORITY_URGENT);

    int pending = channel_peek(mgr, (uint32_t)ch_id);

    channel_manager_destroy(mgr);
    return (pending == 2);
}

int test_channel_broadcast(void)
{
    ChannelManager *mgr = channel_manager_create();
    if (!mgr) return 0;

    /* Create multiple channels */
    channel_create(mgr, 100, 200, "ch1", CHANNEL_TYPE_LOCAL);
    channel_create(mgr, 100, 300, "ch2", CHANNEL_TYPE_LOCAL);
    channel_create(mgr, 100, 400, "ch3", CHANNEL_TYPE_LOCAL);

    /* Broadcast from pid 999 */
    uint32_t atoms[1] = { 42 };
    int sent = channel_broadcast(mgr, 999, atoms, 1);

    channel_print_stats(mgr);
    channel_manager_destroy(mgr);

    return (sent >= 0);
}

int test_channel_many_messages(void)
{
    ChannelManager *mgr = channel_manager_create();
    if (!mgr) return 0;

    int ch_id = channel_create(mgr, 100, 200, "bulk-channel",
                                CHANNEL_TYPE_LOCAL);
    if (ch_id <= 0) {
        channel_manager_destroy(mgr);
        return 0;
    }

    /* Send 100 messages */
    int i;
    for (i = 0; i < 100; i++) {
        uint32_t atoms[1] = { (uint32_t)(i + 1) };
        channel_send(mgr, (uint32_t)ch_id, atoms, 1, MSG_PRIORITY_NORMAL);
    }

    /* Receive all */
    CogMessage msg;
    int count = 0;
    while (channel_recv(mgr, (uint32_t)ch_id, &msg) > 0) {
        count++;
    }

    channel_manager_destroy(mgr);
    return (count == 100);
}

/* ========================================================================
 * Distributed AtomSpace Tests
 * ======================================================================== */

int test_dist_create_destroy(void)
{
    AtomSpace *as = atomspace_create();
    if (!as) return 0;

    DistributedAtomSpace *das = dist_atomspace_create(as);
    if (!das) {
        atomspace_destroy(as);
        return 0;
    }

    dist_atomspace_destroy(das);
    atomspace_destroy(as);
    return 1;
}

int test_dist_add_nodes(void)
{
    AtomSpace *as = atomspace_create();
    DistributedAtomSpace *das = dist_atomspace_create(as);
    if (!das) { atomspace_destroy(as); return 0; }

    int id1 = dist_add_node(das, "node1.cognitive.local", 9090);
    int id2 = dist_add_node(das, "node2.cognitive.local", 9090);
    int id3 = dist_add_node(das, "node3.cognitive.local", 9090);

    int success = (id1 > 0 && id2 > 0 && id3 > 0);

    dist_print_stats(das);
    dist_atomspace_destroy(das);
    atomspace_destroy(as);
    return success;
}

int test_dist_load_balance(void)
{
    AtomSpace *as = atomspace_create();
    DistributedAtomSpace *das = dist_atomspace_create(as);
    if (!das) { atomspace_destroy(as); return 0; }

    dist_add_node(das, "fast-node.local", 9090);
    dist_add_node(das, "slow-node.local", 9090);

    /* Ping nodes to set latency */
    dist_ping_node(das, 1);
    dist_ping_node(das, 2);

    /* Select best node */
    uint32_t selected = dist_select_node(das);

    int success = (selected > 0);

    dist_atomspace_destroy(das);
    atomspace_destroy(as);
    return success;
}

int test_dist_sync(void)
{
    AtomSpace *as = atomspace_create();
    DistributedAtomSpace *das = dist_atomspace_create(as);
    if (!das) { atomspace_destroy(as); return 0; }

    dist_add_node(das, "sync-target.local", 9090);

    int result = dist_sync_all(das);

    dist_atomspace_destroy(das);
    atomspace_destroy(as);
    return (result == 0);
}

/* ========================================================================
 * Main
 * ======================================================================== */

int main(int argc, char **argv)
{
    printf("=== Cognitive IPC & Distributed Test Suite ===\n\n");

    printf("--- IPC Channel Tests ---\n");
    TEST(channel_create_destroy);
    TEST(channel_open_close);
    TEST(channel_send_recv);
    TEST(channel_priority);
    TEST(channel_broadcast);
    TEST(channel_many_messages);

    printf("\n--- Distributed AtomSpace Tests ---\n");
    TEST(dist_create_destroy);
    TEST(dist_add_nodes);
    TEST(dist_load_balance);
    TEST(dist_sync);

    printf("\n=== Results: %d/%d tests passed ===\n", tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
