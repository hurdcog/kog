/*
 * Distributed AtomSpace Implementation
 * Cross-Node Knowledge Sharing
 *
 * Provides node management, remote operations, synchronization,
 * distributed pattern matching, and load-balanced inference scheduling.
 *
 * In a full Inferno deployment, remote operations use 9P/Styx.
 * This portable implementation simulates the distributed protocol
 * for testing and development.
 *
 * Copyright (c) 2026 OpenCog Inferno Project
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "distributed_atomspace.h"

#ifdef _WIN32
#include <windows.h>
static uint64_t dist_get_time_us(void) {
    static LARGE_INTEGER freq = {0};
    LARGE_INTEGER t;
    if (freq.QuadPart == 0) QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t);
    return (uint64_t)((t.QuadPart * 1000000) / freq.QuadPart);
}
#else
#include <sys/time.h>
static uint64_t dist_get_time_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
}
#endif

/* ========================================================================
 * Lifecycle
 * ======================================================================== */

DistributedAtomSpace* dist_atomspace_create(AtomSpace *local)
{
    DistributedAtomSpace *das;

    das = (DistributedAtomSpace *)calloc(1, sizeof(DistributedAtomSpace));
    if (!das) return NULL;

    das->local = local;
    das->node_count = 0;
    das->sync_mode = SYNC_MODE_LAZY;
    das->default_strategy = DIST_STRATEGY_REPLICATE;
    das->pending_count = 0;
    das->total_syncs = 0;
    das->total_remote_queries = 0;
    das->total_bytes_transferred = 0;

    printf("Distributed AtomSpace: Created (sync mode: lazy)\n");

    return das;
}

void dist_atomspace_destroy(DistributedAtomSpace *das)
{
    if (!das) return;

    printf("Distributed AtomSpace: Destroyed\n");
    printf("  Total syncs: %lu\n", (unsigned long)das->total_syncs);
    printf("  Total remote queries: %lu\n",
           (unsigned long)das->total_remote_queries);
    printf("  Total bytes transferred: %lu\n",
           (unsigned long)das->total_bytes_transferred);

    free(das);
}

/* ========================================================================
 * Node Management
 * ======================================================================== */

int dist_add_node(DistributedAtomSpace *das, const char *hostname,
                  uint16_t port)
{
    RemoteNode *node;

    if (!das || !hostname) return -1;
    if (das->node_count >= DIST_MAX_NODES) return -1;

    node = &das->nodes[das->node_count];
    node->id = das->node_count + 1;
    strncpy(node->hostname, hostname, sizeof(node->hostname) - 1);
    node->hostname[sizeof(node->hostname) - 1] = '\0';
    node->port = port;
    node->status = NODE_STATUS_ONLINE;
    node->atom_count = 0;
    node->last_sync = 0;
    node->latency_us = 0;
    node->load = 0.0f;
    node->strategy = das->default_strategy;

    das->node_count++;

    printf("Distributed AtomSpace: Added node %u '%s:%u'\n",
           node->id, hostname, port);

    return (int)node->id;
}

int dist_remove_node(DistributedAtomSpace *das, uint32_t node_id)
{
    RemoteNode *node = dist_get_node(das, node_id);
    if (!node) return -1;

    node->status = NODE_STATUS_OFFLINE;
    printf("Distributed AtomSpace: Removed node %u '%s'\n",
           node->id, node->hostname);

    return 0;
}

int dist_ping_node(DistributedAtomSpace *das, uint32_t node_id)
{
    RemoteNode *node = dist_get_node(das, node_id);
    if (!node) return -1;

    /* In a real implementation, this would send a 9P Tversion/Rversion */
    /* For now, simulate with a fixed latency */
    node->latency_us = 1000;  /* 1ms simulated */
    node->status = NODE_STATUS_ONLINE;

    return 0;
}

RemoteNode* dist_get_node(DistributedAtomSpace *das, uint32_t node_id)
{
    uint32_t i;
    if (!das) return NULL;

    for (i = 0; i < das->node_count; i++) {
        if (das->nodes[i].id == node_id) {
            return &das->nodes[i];
        }
    }
    return NULL;
}

/* ========================================================================
 * Remote Operations (Simulated for Portable Build)
 * ======================================================================== */

int dist_remote_get_atom(DistributedAtomSpace *das, uint32_t node_id,
                         uint32_t atom_id, void *buf, uint32_t bufsize)
{
    RemoteNode *node;

    if (!das || !buf) return -1;

    node = dist_get_node(das, node_id);
    if (!node || node->status != NODE_STATUS_ONLINE) return -1;

    das->total_remote_queries++;
    das->total_bytes_transferred += bufsize;

    /* In a real implementation:
     * 1. Open 9P connection to node->hostname:node->port
     * 2. Walk to /atoms/<atom_id>
     * 3. Read atom data
     * 4. Parse and return
     *
     * For portable build, return -1 (no remote atoms available)
     */

    printf("Distributed AtomSpace: Remote get atom %u from node %u (simulated)\n",
           atom_id, node_id);

    return -1;  /* Not available in portable mode */
}

int dist_remote_add_node_atom(DistributedAtomSpace *das, uint32_t node_id,
                              uint16_t type, const char *name)
{
    RemoteNode *node;

    if (!das || !name) return -1;

    node = dist_get_node(das, node_id);
    if (!node || node->status != NODE_STATUS_ONLINE) return -1;

    das->total_remote_queries++;
    das->total_bytes_transferred += strlen(name) + 16;

    /* In a real implementation:
     * echo "ConceptNode <name> 0.0 0.0" > /net/cognitive/<host>/atoms/new
     */

    printf("Distributed AtomSpace: Remote add node '%s' to node %u (simulated)\n",
           name, node_id);

    node->atom_count++;
    return 0;
}

int dist_remote_query(DistributedAtomSpace *das, uint32_t node_id,
                      uint16_t type, uint32_t *results, uint32_t max)
{
    RemoteNode *node;

    if (!das || !results) return -1;

    node = dist_get_node(das, node_id);
    if (!node || node->status != NODE_STATUS_ONLINE) return -1;

    das->total_remote_queries++;

    /* In a real implementation:
     * echo "<type>" > /net/cognitive/<host>/atoms/query
     * cat /net/cognitive/<host>/atoms/query
     */

    printf("Distributed AtomSpace: Remote query type %u on node %u (simulated)\n",
           type, node_id);

    return 0;  /* No results in portable mode */
}

/* ========================================================================
 * Synchronization
 * ======================================================================== */

int dist_sync_atom(DistributedAtomSpace *das, uint32_t atom_id)
{
    uint32_t i;

    if (!das) return -1;

    /* Queue sync to all online nodes */
    for (i = 0; i < das->node_count; i++) {
        if (das->nodes[i].status == NODE_STATUS_ONLINE &&
            das->pending_count < DIST_MAX_PENDING) {
            SyncOp *op = &das->pending[das->pending_count];
            op->atom_id = atom_id;
            op->target_node = das->nodes[i].id;
            op->timestamp = dist_get_time_us();
            op->completed = 0;
            das->pending_count++;
        }
    }

    return 0;
}

int dist_sync_all(DistributedAtomSpace *das)
{
    if (!das) return -1;

    printf("Distributed AtomSpace: Full sync requested (%u nodes)\n",
           das->node_count);

    /* In a real implementation, this would:
     * 1. Compare version vectors with each node
     * 2. Send/receive delta updates
     * 3. Resolve conflicts using truth value revision
     */

    das->total_syncs++;
    return 0;
}

int dist_process_pending(DistributedAtomSpace *das)
{
    uint32_t i;
    int processed = 0;

    if (!das) return -1;

    for (i = 0; i < das->pending_count; i++) {
        if (!das->pending[i].completed) {
            /* Process this sync operation */
            das->pending[i].completed = 1;
            das->total_syncs++;
            processed++;
        }
    }

    /* Compact the pending array */
    if (processed > 0) {
        uint32_t write = 0;
        for (i = 0; i < das->pending_count; i++) {
            if (!das->pending[i].completed) {
                if (write != i) {
                    das->pending[write] = das->pending[i];
                }
                write++;
            }
        }
        das->pending_count = write;
    }

    return processed;
}

/* ========================================================================
 * Distributed Pattern Matching
 * ======================================================================== */

int dist_pattern_match(DistributedAtomSpace *das, uint32_t pattern_id,
                       uint32_t *results, uint32_t max)
{
    uint32_t i;
    int total = 0;

    if (!das || !results) return -1;

    printf("Distributed AtomSpace: Distributed pattern match for pattern %u\n",
           pattern_id);

    /* Query all online nodes in parallel (simulated sequentially) */
    for (i = 0; i < das->node_count && (uint32_t)total < max; i++) {
        if (das->nodes[i].status == NODE_STATUS_ONLINE) {
            int n = dist_remote_query(das, das->nodes[i].id,
                                       0, &results[total], max - total);
            if (n > 0) total += n;
        }
    }

    return total;
}

/* ========================================================================
 * Load Balancing
 * ======================================================================== */

uint32_t dist_select_node(DistributedAtomSpace *das)
{
    uint32_t i;
    uint32_t best_id = 0;
    float best_score = -1.0f;

    if (!das) return 0;

    /* Select node with lowest load and latency */
    for (i = 0; i < das->node_count; i++) {
        RemoteNode *node = &das->nodes[i];
        if (node->status == NODE_STATUS_ONLINE) {
            /* Score = (1 - load) / (1 + latency_ms) */
            float latency_ms = (float)node->latency_us / 1000.0f;
            float score = (1.0f - node->load) / (1.0f + latency_ms);

            if (score > best_score) {
                best_score = score;
                best_id = node->id;
            }
        }
    }

    return best_id;
}

int dist_schedule_inference(DistributedAtomSpace *das,
                            uint32_t *premises, uint32_t count,
                            uint32_t *target_node)
{
    if (!das || !premises || !target_node) return -1;

    *target_node = dist_select_node(das);
    if (*target_node == 0) return -1;

    printf("Distributed AtomSpace: Scheduled inference (%u premises) on node %u\n",
           count, *target_node);

    return 0;
}

/* ========================================================================
 * Statistics
 * ======================================================================== */

void dist_print_stats(DistributedAtomSpace *das)
{
    uint32_t i;

    if (!das) return;

    printf("\nDistributed AtomSpace Statistics:\n");
    printf("  Nodes: %u\n", das->node_count);
    printf("  Sync mode: %s\n",
           das->sync_mode == SYNC_MODE_EAGER ? "eager" :
           das->sync_mode == SYNC_MODE_LAZY ? "lazy" :
           das->sync_mode == SYNC_MODE_PERIODIC ? "periodic" : "none");
    printf("  Total syncs: %lu\n", (unsigned long)das->total_syncs);
    printf("  Total remote queries: %lu\n",
           (unsigned long)das->total_remote_queries);
    printf("  Total bytes transferred: %lu\n",
           (unsigned long)das->total_bytes_transferred);
    printf("  Pending operations: %u\n", das->pending_count);

    printf("\n  Nodes:\n");
    for (i = 0; i < das->node_count; i++) {
        RemoteNode *n = &das->nodes[i];
        printf("    [%u] %s:%u - %s, atoms=%u, load=%.1f%%, latency=%lu us\n",
               n->id, n->hostname, n->port,
               n->status == NODE_STATUS_ONLINE ? "ONLINE" :
               n->status == NODE_STATUS_OFFLINE ? "OFFLINE" : "ERROR",
               n->atom_count, n->load * 100.0f,
               (unsigned long)n->latency_us);
    }
}
