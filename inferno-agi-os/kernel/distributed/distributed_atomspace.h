/*
 * Distributed AtomSpace
 * Cross-Node Knowledge Sharing via 9P/Styx
 *
 * Implements:
 * - Node discovery and registration
 * - Remote atom operations (get, create, query)
 * - AtomSpace synchronization
 * - Distributed pattern matching
 * - Load-balanced inference scheduling
 *
 * Copyright (c) 2026 OpenCog Inferno Project
 * Licensed under AGPL-3.0
 */

#ifndef _DISTRIBUTED_ATOMSPACE_H_
#define _DISTRIBUTED_ATOMSPACE_H_

#include <stdint.h>

/* Maximum nodes in the cluster */
#define DIST_MAX_NODES 64

/* Maximum pending sync operations */
#define DIST_MAX_PENDING 1024

/* Sync modes */
typedef enum {
    SYNC_MODE_EAGER = 1,       /* Sync immediately */
    SYNC_MODE_LAZY,            /* Sync on demand */
    SYNC_MODE_PERIODIC,        /* Sync at intervals */
    SYNC_MODE_NONE             /* No sync */
} SyncMode;

/* Node status */
typedef enum {
    NODE_STATUS_UNKNOWN = 0,
    NODE_STATUS_ONLINE,
    NODE_STATUS_OFFLINE,
    NODE_STATUS_SYNCING,
    NODE_STATUS_ERROR
} NodeStatus;

/* Distribution strategy */
typedef enum {
    DIST_STRATEGY_REPLICATE = 1,   /* Full replication */
    DIST_STRATEGY_PARTITION,       /* Partition by type/domain */
    DIST_STRATEGY_ATTENTION,       /* Distribute by attention value */
    DIST_STRATEGY_HIERARCHICAL     /* Multi-level hierarchy */
} DistStrategy;

/* Remote node descriptor */
typedef struct RemoteNode {
    uint32_t id;
    char hostname[128];
    uint16_t port;
    NodeStatus status;
    uint32_t atom_count;
    uint64_t last_sync;
    uint64_t latency_us;       /* Average latency in microseconds */
    float load;                /* CPU load [0.0, 1.0] */
    DistStrategy strategy;
} RemoteNode;

/* Sync operation */
typedef struct SyncOp {
    uint32_t atom_id;
    uint32_t target_node;
    uint64_t timestamp;
    int completed;
} SyncOp;

/* Forward declaration */
typedef struct AtomSpace AtomSpace;

/* Distributed AtomSpace */
typedef struct DistributedAtomSpace {
    AtomSpace *local;
    RemoteNode nodes[DIST_MAX_NODES];
    uint32_t node_count;
    SyncMode sync_mode;
    DistStrategy default_strategy;
    SyncOp pending[DIST_MAX_PENDING];
    uint32_t pending_count;
    uint64_t total_syncs;
    uint64_t total_remote_queries;
    uint64_t total_bytes_transferred;
} DistributedAtomSpace;

/* ========================================================================
 * API Functions
 * ======================================================================== */

/* Lifecycle */
DistributedAtomSpace* dist_atomspace_create(AtomSpace *local);
void dist_atomspace_destroy(DistributedAtomSpace *das);

/* Node management */
int dist_add_node(DistributedAtomSpace *das, const char *hostname,
                  uint16_t port);
int dist_remove_node(DistributedAtomSpace *das, uint32_t node_id);
int dist_ping_node(DistributedAtomSpace *das, uint32_t node_id);
RemoteNode* dist_get_node(DistributedAtomSpace *das, uint32_t node_id);

/* Remote operations */
int dist_remote_get_atom(DistributedAtomSpace *das, uint32_t node_id,
                         uint32_t atom_id, void *buf, uint32_t bufsize);
int dist_remote_add_node_atom(DistributedAtomSpace *das, uint32_t node_id,
                              uint16_t type, const char *name);
int dist_remote_query(DistributedAtomSpace *das, uint32_t node_id,
                      uint16_t type, uint32_t *results, uint32_t max);

/* Synchronization */
int dist_sync_atom(DistributedAtomSpace *das, uint32_t atom_id);
int dist_sync_all(DistributedAtomSpace *das);
int dist_process_pending(DistributedAtomSpace *das);

/* Distributed pattern matching */
int dist_pattern_match(DistributedAtomSpace *das, uint32_t pattern_id,
                       uint32_t *results, uint32_t max);

/* Load balancing */
uint32_t dist_select_node(DistributedAtomSpace *das);
int dist_schedule_inference(DistributedAtomSpace *das,
                            uint32_t *premises, uint32_t count,
                            uint32_t *target_node);

/* Statistics */
void dist_print_stats(DistributedAtomSpace *das);

#endif /* _DISTRIBUTED_ATOMSPACE_H_ */
