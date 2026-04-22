/*
 * partitioning.c - Atom Partitioning for Distributed AtomSpace
 *
 * Implements atom distribution strategy for the distributed cognitive network.
 * Atoms are partitioned across nodes based on their type and attention value.
 *
 * Partitioning Rules:
 *   - Perceptual atoms  → Perception nodes
 *   - Conceptual atoms  → Reasoning nodes
 *   - Procedural atoms  → Action nodes
 *   - Episodic atoms    → Memory nodes
 *   - High-attention    → Replicated across all nodes
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long  ulong;
typedef unsigned short ushort;

/* Node type codes */
enum {
    NODE_PERCEPTION = 0,
    NODE_REASONING  = 1,
    NODE_LEARNING   = 2,
    NODE_ACTION     = 3,
    NODE_MEMORY     = 4,
    NODE_COUNT,
};

/* Atom type codes (mirrors atomspace.h) */
enum {
    ATOM_TYPE_NODE              = 0,
    ATOM_TYPE_LINK              = 1,
    ATOM_TYPE_CONCEPT_NODE      = 2,
    ATOM_TYPE_PREDICATE_NODE    = 3,
    ATOM_TYPE_VARIABLE_NODE     = 4,
    ATOM_TYPE_INHERITANCE_LINK  = 5,
    ATOM_TYPE_SIMILARITY_LINK   = 6,
    ATOM_TYPE_EVALUATION_LINK   = 7,
    ATOM_TYPE_PERCEPTION_NODE   = 20,
    ATOM_TYPE_ACTION_NODE       = 21,
    ATOM_TYPE_PROCEDURE_NODE    = 22,
    ATOM_TYPE_EPISODE_NODE      = 23,
};

/* High-attention threshold for replication */
#define REPLICATION_STI_THRESHOLD 500

/* Long-term importance threshold for replication */
#define REPLICATION_LTI_THRESHOLD 10

/*
 * atom_home_node - Determine the primary home node for an atom.
 *
 * Returns NODE_* constant indicating which cluster should own this atom.
 */
int
atom_home_node(ushort atom_type, short sti)
{
    /* High-attention atoms are replicated everywhere; primary home = reasoning */
    if(sti >= REPLICATION_STI_THRESHOLD)
        return NODE_REASONING;

    switch(atom_type) {
    case ATOM_TYPE_PERCEPTION_NODE:
        return NODE_PERCEPTION;

    case ATOM_TYPE_ACTION_NODE:
    case ATOM_TYPE_PROCEDURE_NODE:
        return NODE_ACTION;

    case ATOM_TYPE_EPISODE_NODE:
        return NODE_MEMORY;

    case ATOM_TYPE_INHERITANCE_LINK:
    case ATOM_TYPE_SIMILARITY_LINK:
    case ATOM_TYPE_EVALUATION_LINK:
    case ATOM_TYPE_CONCEPT_NODE:
    case ATOM_TYPE_PREDICATE_NODE:
    default:
        return NODE_REASONING;
    }
}

/*
 * atom_should_replicate - Return 1 if the atom should be replicated to all nodes.
 */
int
atom_should_replicate(short sti, int lti)
{
    return (sti >= REPLICATION_STI_THRESHOLD || lti > REPLICATION_LTI_THRESHOLD);
}

/*
 * partition_atoms - Partition a set of atom IDs into per-node assignment lists.
 *
 * atoms:      Array of atom IDs to partition
 * natoms:     Number of atoms
 * sti_vals:   Corresponding STI values
 * types:      Corresponding type values
 * assignment: Output array [natoms] filled with NODE_* values
 *
 * Returns the number of atoms assigned to replication (all nodes).
 */
int
partition_atoms(ulong *atoms, int natoms, short *sti_vals, ushort *types,
                int *assignment)
{
    int i, replicated = 0;

    for(i = 0; i < natoms; i++) {
        int home = atom_home_node(types[i], sti_vals[i]);
        assignment[i] = home;
        if(atom_should_replicate(sti_vals[i], 0)) {
            assignment[i] = -1;  /* -1 = replicate to all */
            replicated++;
        }
    }

    return replicated;
}

/*
 * print_partition_summary - Print partition statistics.
 */
void
print_partition_summary(int *assignment, int natoms)
{
    int counts[NODE_COUNT + 1] = {0};  /* +1 for replicated */
    int i;
    const char *names[] = {
        "Perception", "Reasoning", "Learning", "Action", "Memory", "Replicated"
    };

    for(i = 0; i < natoms; i++) {
        if(assignment[i] < 0)
            counts[NODE_COUNT]++;
        else if(assignment[i] < NODE_COUNT)
            counts[assignment[i]]++;
    }

    printf("Atom Partition Summary (%d atoms):\n", natoms);
    for(i = 0; i <= NODE_COUNT; i++)
        printf("  %-12s: %d\n", names[i], counts[i]);
}
