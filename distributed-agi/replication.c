/*
 * replication.c - Atom Replication for Distributed AtomSpace
 *
 * Manages replication of high-attention atoms across multiple cognitive nodes.
 * Ensures that atoms above the replication threshold are kept in sync
 * across all nodes in the cluster.
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long  ulong;
typedef unsigned short ushort;

#define MAX_NODES  64
#define MAX_REPLICAS 1024

/* Replica record */
typedef struct Replica {
    ulong  atom_id;
    int    nodes[MAX_NODES];  /* Nodes holding this replica (1 = yes, 0 = no) */
    int    nnodes;
    float  tv_strength;
    float  tv_confidence;
    short  av_sti;
} Replica;

static Replica replicas[MAX_REPLICAS];
static int nreplicas = 0;

/*
 * replication_add - Register an atom for replication to a set of nodes.
 *
 * Returns the replica index, or -1 on failure.
 */
int
replication_add(ulong atom_id, int *node_ids, int nnodes,
                float tvs, float tvc, short sti)
{
    Replica *r;
    int i;

    if(nreplicas >= MAX_REPLICAS)
        return -1;

    r = &replicas[nreplicas];
    r->atom_id      = atom_id;
    r->nnodes       = nnodes < MAX_NODES ? nnodes : MAX_NODES;
    r->tv_strength  = tvs;
    r->tv_confidence = tvc;
    r->av_sti       = sti;

    memset(r->nodes, 0, sizeof(r->nodes));
    for(i = 0; i < r->nnodes; i++)
        if(node_ids[i] < MAX_NODES)
            r->nodes[node_ids[i]] = 1;

    return nreplicas++;
}

/*
 * replication_sync - Propagate a truth value update to all replica nodes.
 *
 * In a real implementation this would use 9P-Cog to push updates.
 * Here we log the sync operation.
 *
 * Returns the number of nodes notified.
 */
int
replication_sync(ulong atom_id, float new_tvs, float new_tvc)
{
    int i, j, notified = 0;

    for(i = 0; i < nreplicas; i++) {
        if(replicas[i].atom_id != atom_id)
            continue;

        replicas[i].tv_strength   = new_tvs;
        replicas[i].tv_confidence = new_tvc;

        for(j = 0; j < MAX_NODES; j++) {
            if(!replicas[i].nodes[j]) continue;
            /* Send update to node j via 9P-Cog */
            printf("replication: sync atom %lu → node %d  <%.3f, %.3f>\n",
                atom_id, j, new_tvs, new_tvc);
            notified++;
        }
        break;
    }

    return notified;
}

/*
 * replication_evict - Remove an atom from replication (e.g. after forgetting).
 */
int
replication_evict(ulong atom_id)
{
    int i;

    for(i = 0; i < nreplicas; i++) {
        if(replicas[i].atom_id == atom_id) {
            /* Compact the replica list */
            replicas[i] = replicas[--nreplicas];
            return 0;
        }
    }

    return -1;
}

/*
 * replication_stats - Print replication statistics.
 */
void
replication_stats(void)
{
    printf("Replication stats: %d replicated atoms\n", nreplicas);
}
