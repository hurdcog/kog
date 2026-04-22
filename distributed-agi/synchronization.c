/*
 * synchronization.c - Distributed AtomSpace Synchronization
 *
 * Manages consistency of the distributed AtomSpace across multiple
 * cognitive nodes. Implements eventual consistency via Lamport timestamps
 * and a gossip-style propagation protocol.
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long  ulong;
typedef unsigned short ushort;

/* Maximum cluster size */
#define MAX_NODES 64

/* Lamport timestamp */
typedef struct Timestamp {
    ulong logical;   /* Logical clock value */
    int   node_id;   /* Originating node */
} Timestamp;

/* Sync record for a single atom update */
typedef struct SyncRecord {
    ulong     atom_id;
    Timestamp ts;
    float     tv_strength;
    float     tv_confidence;
    short     av_sti;
    int       applied;   /* 1 if applied locally */
} SyncRecord;

#define MAX_SYNC_QUEUE 4096

static SyncRecord sync_queue[MAX_SYNC_QUEUE];
static int sync_head = 0;
static int sync_tail = 0;

static ulong local_clock = 0;
static int   local_node  = 0;

/*
 * sync_init - Initialize the sync subsystem.
 */
void
sync_init(int node_id)
{
    local_clock = 0;
    local_node  = node_id;
    sync_head   = 0;
    sync_tail   = 0;
    printf("sync: initialized node %d\n", node_id);
}

/*
 * sync_tick - Advance the local Lamport clock.
 */
Timestamp
sync_tick(void)
{
    Timestamp ts;
    ts.logical = ++local_clock;
    ts.node_id = local_node;
    return ts;
}

/*
 * sync_update - Merge an incoming clock value (Lamport receive rule).
 * Correct Lamport rule: local_clock = max(local_clock, remote_clock) + 1
 */
void
sync_update(ulong remote_clock)
{
    if(remote_clock > local_clock)
        local_clock = remote_clock + 1;
    else
        local_clock++;
}

/*
 * sync_enqueue - Enqueue an atom update for propagation.
 *
 * Returns 0 on success, -1 if queue is full.
 */
int
sync_enqueue(ulong atom_id, float tvs, float tvc, short sti)
{
    SyncRecord *r;
    int next;

    next = (sync_tail + 1) % MAX_SYNC_QUEUE;
    if(next == sync_head)
        return -1;  /* Queue full */

    r = &sync_queue[sync_tail];
    r->atom_id      = atom_id;
    r->ts           = sync_tick();
    r->tv_strength  = tvs;
    r->tv_confidence = tvc;
    r->av_sti       = sti;
    r->applied      = 0;

    sync_tail = next;
    return 0;
}

/*
 * sync_process - Process pending sync queue entries.
 *
 * For each pending update, attempt to apply it locally and broadcast
 * to peer nodes. Returns the number of updates processed.
 */
int
sync_process(int *peer_nodes, int npeers)
{
    int processed = 0;
    int i;

    while(sync_head != sync_tail) {
        SyncRecord *r = &sync_queue[sync_head];

        if(!r->applied) {
            /* Apply update locally - in real impl, calls into atomspace kernel */
            printf("sync: apply atom %lu <%.3f, %.3f> ts(%lu,%d)\n",
                r->atom_id, r->tv_strength, r->tv_confidence,
                r->ts.logical, r->ts.node_id);
            r->applied = 1;

            /* Broadcast to peers via 9P-Cog */
            for(i = 0; i < npeers; i++) {
                if(peer_nodes[i] == local_node) continue;
                printf("sync: broadcast atom %lu → node %d\n",
                    r->atom_id, peer_nodes[i]);
            }
        }

        sync_head = (sync_head + 1) % MAX_SYNC_QUEUE;
        processed++;
    }

    return processed;
}

/*
 * sync_stats - Print synchronization statistics.
 */
void
sync_stats(void)
{
    int pending = (sync_tail - sync_head + MAX_SYNC_QUEUE) % MAX_SYNC_QUEUE;
    printf("sync: node=%d clock=%lu pending=%d\n",
        local_node, local_clock, pending);
}
