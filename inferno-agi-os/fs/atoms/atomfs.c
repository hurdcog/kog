/*
 * AtomSpace Filesystem (atomfs)
 *
 * Exposes the AtomSpace hypergraph as a 9P filesystem.
 * Following Inferno/Plan 9 philosophy: everything is a file.
 *
 * Filesystem layout:
 *   /atoms/
 *     new          - Write "type name strength confidence" to create atom
 *     delete       - Write atom ID to delete
 *     stats        - Read for AtomSpace statistics
 *     query        - Write type filter, read matching atom IDs
 *     concepts/    - Directory of ConceptNodes
 *       <name>     - Read for atom details, write to set TV
 *     predicates/  - Directory of PredicateNodes
 *     schemas/     - Directory of SchemaNodes
 *     links/       - Directory of Links
 *     truth/       - Truth value access
 *       <id>       - Read/write truth value for atom
 *     attention/   - Attention value access
 *       <id>       - Read/write attention value for atom
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "../atoms/atomfs.h"

/* Maximum atoms in filesystem view */
#define ATOMFS_MAX_ATOMS    65536
#define ATOMFS_MAX_PATH     512
#define ATOMFS_MAX_BUF      4096

/* Atom types for filesystem categorization */
#define ATOMFS_TYPE_CONCEPT     1
#define ATOMFS_TYPE_PREDICATE   2
#define ATOMFS_TYPE_SCHEMA      3
#define ATOMFS_TYPE_NUMBER      4
#define ATOMFS_TYPE_VARIABLE    5
#define ATOMFS_TYPE_LINK        10
#define ATOMFS_TYPE_INHERITANCE 11
#define ATOMFS_TYPE_EVALUATION  12
#define ATOMFS_TYPE_LIST        13
#define ATOMFS_TYPE_IMPLICATION 14

/* Filesystem atom entry */
typedef struct AtomFSEntry {
    uint32_t id;
    int      type;
    char     name[256];
    float    tv_strength;
    float    tv_confidence;
    int16_t  av_sti;
    int16_t  av_lti;
    uint32_t outgoing[16];
    int      outgoing_count;
    int      active;
} AtomFSEntry;

/* AtomFS state */
typedef struct AtomFS {
    AtomFSEntry atoms[ATOMFS_MAX_ATOMS];
    uint32_t    next_id;
    uint32_t    atom_count;
    int         initialized;
#ifdef PLATFORM_WINDOWS
    CRITICAL_SECTION lock;
#else
    pthread_mutex_t  lock;
#endif
} AtomFS;

static AtomFS g_atomfs;

/*
 * Initialize the AtomSpace filesystem
 */
int
atomfs_init(void)
{
    memset(&g_atomfs, 0, sizeof(AtomFS));
    g_atomfs.next_id = 1;
    g_atomfs.initialized = 1;

#ifdef PLATFORM_WINDOWS
    InitializeCriticalSection(&g_atomfs.lock);
#else
    pthread_mutex_init(&g_atomfs.lock, NULL);
#endif

    return 0;
}

/*
 * Shutdown the AtomSpace filesystem
 */
void
atomfs_shutdown(void)
{
    g_atomfs.initialized = 0;

#ifdef PLATFORM_WINDOWS
    DeleteCriticalSection(&g_atomfs.lock);
#else
    pthread_mutex_destroy(&g_atomfs.lock);
#endif
}

/*
 * Lock the filesystem
 */
static void
atomfs_lock(void)
{
#ifdef PLATFORM_WINDOWS
    EnterCriticalSection(&g_atomfs.lock);
#else
    pthread_mutex_lock(&g_atomfs.lock);
#endif
}

/*
 * Unlock the filesystem
 */
static void
atomfs_unlock(void)
{
#ifdef PLATFORM_WINDOWS
    LeaveCriticalSection(&g_atomfs.lock);
#else
    pthread_mutex_unlock(&g_atomfs.lock);
#endif
}

/*
 * Parse atom type string to integer
 */
static int
atomfs_parse_type(const char *type_str)
{
    if (strcmp(type_str, "ConceptNode") == 0)      return ATOMFS_TYPE_CONCEPT;
    if (strcmp(type_str, "PredicateNode") == 0)     return ATOMFS_TYPE_PREDICATE;
    if (strcmp(type_str, "SchemaNode") == 0)        return ATOMFS_TYPE_SCHEMA;
    if (strcmp(type_str, "NumberNode") == 0)        return ATOMFS_TYPE_NUMBER;
    if (strcmp(type_str, "VariableNode") == 0)      return ATOMFS_TYPE_VARIABLE;
    if (strcmp(type_str, "InheritanceLink") == 0)   return ATOMFS_TYPE_INHERITANCE;
    if (strcmp(type_str, "EvaluationLink") == 0)    return ATOMFS_TYPE_EVALUATION;
    if (strcmp(type_str, "ListLink") == 0)          return ATOMFS_TYPE_LIST;
    if (strcmp(type_str, "ImplicationLink") == 0)   return ATOMFS_TYPE_IMPLICATION;
    return ATOMFS_TYPE_CONCEPT; /* default */
}

/*
 * Get atom type string from integer
 */
static const char*
atomfs_type_str(int type)
{
    switch (type) {
    case ATOMFS_TYPE_CONCEPT:     return "ConceptNode";
    case ATOMFS_TYPE_PREDICATE:   return "PredicateNode";
    case ATOMFS_TYPE_SCHEMA:      return "SchemaNode";
    case ATOMFS_TYPE_NUMBER:      return "NumberNode";
    case ATOMFS_TYPE_VARIABLE:    return "VariableNode";
    case ATOMFS_TYPE_INHERITANCE: return "InheritanceLink";
    case ATOMFS_TYPE_EVALUATION:  return "EvaluationLink";
    case ATOMFS_TYPE_LIST:        return "ListLink";
    case ATOMFS_TYPE_IMPLICATION: return "ImplicationLink";
    default:                      return "UnknownType";
    }
}

/*
 * Handle write to /atoms/new
 * Format: "type name strength confidence"
 * Returns: atom ID
 */
uint32_t
atomfs_create(const char *type_str, const char *name,
              float strength, float confidence)
{
    if (!g_atomfs.initialized)
        return 0;

    atomfs_lock();

    if (g_atomfs.atom_count >= ATOMFS_MAX_ATOMS) {
        atomfs_unlock();
        return 0;
    }

    /* Check for duplicate */
    int parsed_type = atomfs_parse_type(type_str);
    for (uint32_t i = 0; i < ATOMFS_MAX_ATOMS; i++) {
        if (g_atomfs.atoms[i].active &&
            g_atomfs.atoms[i].type == parsed_type &&
            strcmp(g_atomfs.atoms[i].name, name) == 0) {
            /* Update existing atom's TV */
            g_atomfs.atoms[i].tv_strength = strength;
            g_atomfs.atoms[i].tv_confidence = confidence;
            uint32_t id = g_atomfs.atoms[i].id;
            atomfs_unlock();
            return id;
        }
    }

    /* Find empty slot */
    uint32_t slot = 0;
    for (slot = 0; slot < ATOMFS_MAX_ATOMS; slot++) {
        if (!g_atomfs.atoms[slot].active)
            break;
    }

    AtomFSEntry *entry = &g_atomfs.atoms[slot];
    entry->id = g_atomfs.next_id++;
    entry->type = parsed_type;
    strncpy(entry->name, name, sizeof(entry->name) - 1);
    entry->tv_strength = strength;
    entry->tv_confidence = confidence;
    entry->av_sti = 0;
    entry->av_lti = 0;
    entry->outgoing_count = 0;
    entry->active = 1;
    g_atomfs.atom_count++;

    uint32_t id = entry->id;
    atomfs_unlock();

    return id;
}

/*
 * Handle read from /atoms/<id>
 * Returns atom data as string
 */
int
atomfs_read(uint32_t atom_id, char *buf, size_t maxlen)
{
    if (!g_atomfs.initialized)
        return -1;

    atomfs_lock();

    for (uint32_t i = 0; i < ATOMFS_MAX_ATOMS; i++) {
        if (g_atomfs.atoms[i].active && g_atomfs.atoms[i].id == atom_id) {
            AtomFSEntry *e = &g_atomfs.atoms[i];
            int n = snprintf(buf, maxlen,
                "%u %s %s %.4f %.4f %d %d",
                e->id, atomfs_type_str(e->type), e->name,
                e->tv_strength, e->tv_confidence,
                e->av_sti, e->av_lti);
            atomfs_unlock();
            return n;
        }
    }

    atomfs_unlock();
    return -1;
}

/*
 * Handle write to /atoms/delete
 */
int
atomfs_delete(uint32_t atom_id)
{
    if (!g_atomfs.initialized)
        return -1;

    atomfs_lock();

    for (uint32_t i = 0; i < ATOMFS_MAX_ATOMS; i++) {
        if (g_atomfs.atoms[i].active && g_atomfs.atoms[i].id == atom_id) {
            g_atomfs.atoms[i].active = 0;
            g_atomfs.atom_count--;
            atomfs_unlock();
            return 0;
        }
    }

    atomfs_unlock();
    return -1;
}

/*
 * Handle read from /atoms/stats
 */
int
atomfs_stats(char *buf, size_t maxlen)
{
    if (!g_atomfs.initialized)
        return -1;

    atomfs_lock();

    int nodes = 0, links = 0;
    float avg_sti = 0.0f;

    for (uint32_t i = 0; i < ATOMFS_MAX_ATOMS; i++) {
        if (g_atomfs.atoms[i].active) {
            if (g_atomfs.atoms[i].type < ATOMFS_TYPE_LINK)
                nodes++;
            else
                links++;
            avg_sti += g_atomfs.atoms[i].av_sti;
        }
    }

    if (g_atomfs.atom_count > 0)
        avg_sti /= (float)g_atomfs.atom_count;

    int n = snprintf(buf, maxlen,
        "AtomSpace Filesystem Statistics\n"
        "  Total atoms: %u\n"
        "  Nodes: %d\n"
        "  Links: %d\n"
        "  Next ID: %u\n"
        "  Average STI: %.2f\n",
        g_atomfs.atom_count, nodes, links,
        g_atomfs.next_id, avg_sti);

    atomfs_unlock();
    return n;
}

/*
 * Handle read from /atoms/query
 * Returns space-separated atom IDs matching type filter
 */
int
atomfs_query(const char *type_filter, uint32_t *ids, uint32_t max_results)
{
    if (!g_atomfs.initialized)
        return 0;

    int filter_type = atomfs_parse_type(type_filter);

    atomfs_lock();

    uint32_t count = 0;
    for (uint32_t i = 0; i < ATOMFS_MAX_ATOMS && count < max_results; i++) {
        if (g_atomfs.atoms[i].active && g_atomfs.atoms[i].type == filter_type) {
            ids[count++] = g_atomfs.atoms[i].id;
        }
    }

    atomfs_unlock();
    return (int)count;
}

/*
 * Handle write to /atoms/truth/<id>
 */
int
atomfs_set_tv(uint32_t atom_id, float strength, float confidence)
{
    if (!g_atomfs.initialized)
        return -1;

    atomfs_lock();

    for (uint32_t i = 0; i < ATOMFS_MAX_ATOMS; i++) {
        if (g_atomfs.atoms[i].active && g_atomfs.atoms[i].id == atom_id) {
            g_atomfs.atoms[i].tv_strength = strength;
            g_atomfs.atoms[i].tv_confidence = confidence;
            atomfs_unlock();
            return 0;
        }
    }

    atomfs_unlock();
    return -1;
}

/*
 * Handle write to /atoms/attention/<id>
 */
int
atomfs_set_av(uint32_t atom_id, int16_t sti, int16_t lti)
{
    if (!g_atomfs.initialized)
        return -1;

    atomfs_lock();

    for (uint32_t i = 0; i < ATOMFS_MAX_ATOMS; i++) {
        if (g_atomfs.atoms[i].active && g_atomfs.atoms[i].id == atom_id) {
            g_atomfs.atoms[i].av_sti = sti;
            g_atomfs.atoms[i].av_lti = lti;
            atomfs_unlock();
            return 0;
        }
    }

    atomfs_unlock();
    return -1;
}
