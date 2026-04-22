/*
 * Pattern Matching Engine Implementation
 * Full Pattern Matching with Variable Binding
 *
 * Copyright (c) 2026 OpenCog Inferno Project
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "pattern_matcher.h"

#ifdef _WIN32
#include <windows.h>
static uint64_t pm_get_time_us(void) {
    static LARGE_INTEGER freq = {0};
    LARGE_INTEGER t;
    if (freq.QuadPart == 0) QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t);
    return (uint64_t)((t.QuadPart * 1000000) / freq.QuadPart);
}
#else
#include <sys/time.h>
static uint64_t pm_get_time_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
}
#endif

/* ========================================================================
 * External AtomSpace Interface
 * ======================================================================== */

typedef struct TruthValue {
    float strength;
    float confidence;
    uint32_t count;
} TruthValue;

typedef struct Atom {
    uint32_t id;
    uint16_t type;
    uint16_t flags;
    TruthValue tv;
    void *data;
    uint32_t datalen;
} Atom;

typedef struct NodeData {
    char *name;
} NodeData;

typedef struct LinkData {
    uint32_t *outgoing;
    uint32_t arity;
} LinkData;

typedef struct AtomTableEntry {
    uint32_t atom_id;
    Atom *atom;
    struct AtomTableEntry *next;
} AtomTableEntry;

#define ATOM_TABLE_SIZE 65536
#define ATOM_TYPE_VARIABLE 0x0004
#define ATOM_TYPE_LINK     0x0100

typedef struct AtomSpace {
    uint32_t next_id;
    uint32_t atom_count;
    uint32_t node_count;
    uint32_t link_count;
    AtomTableEntry *buckets[ATOM_TABLE_SIZE];
} AtomSpace;

extern Atom* atomspace_get_atom(AtomSpace *as, uint32_t atom_id);
extern const char* atomspace_get_name(AtomSpace *as, uint32_t atom_id);
extern uint16_t atomspace_get_type(AtomSpace *as, uint32_t atom_id);

/* ========================================================================
 * Binding Set Operations
 * ======================================================================== */

BindingSet* binding_set_create(void)
{
    BindingSet *bs = (BindingSet *)calloc(1, sizeof(BindingSet));
    return bs;
}

void binding_set_destroy(BindingSet *bs)
{
    if (bs) free(bs);
}

int binding_set_bind(BindingSet *bs, uint32_t var_id, uint32_t atom_id)
{
    uint32_t i;

    if (!bs) return -1;

    /* Check if already bound */
    for (i = 0; i < bs->binding_count; i++) {
        if (bs->bindings[i].var_id == var_id) {
            if (bs->bindings[i].is_bound) {
                /* Already bound - check consistency */
                return (bs->bindings[i].bound_id == atom_id) ? 0 : -1;
            }
            bs->bindings[i].bound_id = atom_id;
            bs->bindings[i].is_bound = 1;
            return 0;
        }
    }

    /* New binding */
    if (bs->binding_count >= PM_MAX_VARIABLES) return -1;

    bs->bindings[bs->binding_count].var_id = var_id;
    bs->bindings[bs->binding_count].bound_id = atom_id;
    bs->bindings[bs->binding_count].is_bound = 1;
    bs->binding_count++;

    return 0;
}

uint32_t binding_set_lookup(BindingSet *bs, uint32_t var_id)
{
    uint32_t i;

    if (!bs) return 0;

    for (i = 0; i < bs->binding_count; i++) {
        if (bs->bindings[i].var_id == var_id && bs->bindings[i].is_bound) {
            return bs->bindings[i].bound_id;
        }
    }

    return 0;  /* Not found */
}

BindingSet* binding_set_copy(BindingSet *bs)
{
    BindingSet *copy;

    if (!bs) return NULL;

    copy = (BindingSet *)malloc(sizeof(BindingSet));
    if (!copy) return NULL;

    memcpy(copy, bs, sizeof(BindingSet));
    return copy;
}

/* ========================================================================
 * Pattern Matching Core
 * ======================================================================== */

/**
 * Check if an atom is a variable
 */
static int is_variable(AtomSpace *as, uint32_t atom_id, uint32_t *variables,
                       uint32_t var_count)
{
    uint32_t i;

    /* Check explicit variable list */
    for (i = 0; i < var_count; i++) {
        if (variables[i] == atom_id) return 1;
    }

    /* Check type */
    Atom *atom = atomspace_get_atom(as, atom_id);
    if (atom && atom->type == ATOM_TYPE_VARIABLE) return 1;

    return 0;
}

/**
 * Recursive pattern matching: Check if candidate matches pattern
 * with variable binding support
 */
static int match_recursive(AtomSpace *as, uint32_t pattern_id,
                           uint32_t candidate_id, BindingSet *bindings,
                           uint32_t *variables, uint32_t var_count,
                           uint32_t depth, uint32_t *examined)
{
    Atom *pattern, *candidate;
    LinkData *pld, *cld;
    uint32_t i;

    if (depth > PM_MAX_DEPTH) return 0;
    (*examined)++;

    /* If pattern is a variable, try to bind it */
    if (is_variable(as, pattern_id, variables, var_count)) {
        uint32_t existing = binding_set_lookup(bindings, pattern_id);
        if (existing != 0) {
            /* Already bound - must match */
            return (existing == candidate_id) ? 1 : 0;
        }
        /* Bind the variable */
        return (binding_set_bind(bindings, pattern_id, candidate_id) == 0) ? 1 : 0;
    }

    pattern = atomspace_get_atom(as, pattern_id);
    candidate = atomspace_get_atom(as, candidate_id);

    if (!pattern || !candidate) return 0;

    /* Types must match */
    if (pattern->type != candidate->type) return 0;

    /* For nodes: names must match */
    if (pattern->type < ATOM_TYPE_LINK) {
        const char *pname = atomspace_get_name(as, pattern_id);
        const char *cname = atomspace_get_name(as, candidate_id);
        if (pname && cname) {
            return (strcmp(pname, cname) == 0) ? 1 : 0;
        }
        return (pname == NULL && cname == NULL) ? 1 : 0;
    }

    /* For links: recursively match outgoing sets */
    if (!pattern->data || !candidate->data) return 0;

    pld = (LinkData *)pattern->data;
    cld = (LinkData *)candidate->data;

    /* Arity must match */
    if (pld->arity != cld->arity) return 0;

    /* Match each outgoing atom */
    for (i = 0; i < pld->arity; i++) {
        if (!match_recursive(as, pld->outgoing[i], cld->outgoing[i],
                             bindings, variables, var_count,
                             depth + 1, examined)) {
            return 0;
        }
    }

    return 1;  /* All outgoing atoms matched */
}

/* ========================================================================
 * Public API Implementation
 * ======================================================================== */

MatchResult* pattern_match(AtomSpace *as, PatternQuery *query)
{
    MatchResult *result;
    Atom *pattern;
    uint32_t i;
    uint64_t start_time;

    if (!as || !query) return NULL;

    start_time = pm_get_time_us();

    result = (MatchResult *)calloc(1, sizeof(MatchResult));
    if (!result) return NULL;

    result->max_results = query->max_results > 0 ?
                          query->max_results : PM_MAX_MATCHES;
    result->results = (BindingSet *)calloc(result->max_results,
                                            sizeof(BindingSet));
    if (!result->results) {
        free(result);
        return NULL;
    }

    pattern = atomspace_get_atom(as, query->pattern_id);
    if (!pattern) {
        free(result->results);
        free(result);
        return NULL;
    }

    /* Iterate through all atoms and try to match */
    for (i = 0; i < ATOM_TABLE_SIZE && result->result_count < result->max_results; i++) {
        AtomTableEntry *entry = as->buckets[i];
        while (entry && result->result_count < result->max_results) {
            if (entry->atom && entry->atom_id != query->pattern_id) {
                /* Only try atoms of the same type as the pattern root */
                if (entry->atom->type == pattern->type) {
                    BindingSet bindings;
                    memset(&bindings, 0, sizeof(bindings));

                    if (match_recursive(as, query->pattern_id, entry->atom_id,
                                        &bindings, query->variables,
                                        query->variable_count, 0,
                                        &result->atoms_examined)) {
                        result->results[result->result_count] = bindings;
                        result->result_count++;
                    }
                }
            }
            entry = entry->next;
        }
    }

    result->time_us = pm_get_time_us() - start_time;

    return result;
}

void match_result_destroy(MatchResult *result)
{
    if (!result) return;
    if (result->results) free(result->results);
    free(result);
}

int pattern_match_type(AtomSpace *as, uint16_t type,
                       uint32_t *results, uint32_t max)
{
    uint32_t i;
    int count = 0;

    if (!as || !results) return -1;

    for (i = 0; i < ATOM_TABLE_SIZE && (uint32_t)count < max; i++) {
        AtomTableEntry *entry = as->buckets[i];
        while (entry && (uint32_t)count < max) {
            if (entry->atom && entry->atom->type == type) {
                results[count++] = entry->atom_id;
            }
            entry = entry->next;
        }
    }

    return count;
}

int pattern_match_name(AtomSpace *as, const char *name,
                       uint32_t *results, uint32_t max)
{
    uint32_t i;
    int count = 0;

    if (!as || !name || !results) return -1;

    for (i = 0; i < ATOM_TABLE_SIZE && (uint32_t)count < max; i++) {
        AtomTableEntry *entry = as->buckets[i];
        while (entry && (uint32_t)count < max) {
            if (entry->atom && entry->atom->type < ATOM_TYPE_LINK &&
                entry->atom->data) {
                NodeData *nd = (NodeData *)entry->atom->data;
                if (nd->name && strcmp(nd->name, name) == 0) {
                    results[count++] = entry->atom_id;
                }
            }
            entry = entry->next;
        }
    }

    return count;
}

int pattern_match_link(AtomSpace *as, uint16_t link_type,
                       uint32_t source, uint32_t target,
                       uint32_t *results, uint32_t max)
{
    uint32_t i;
    int count = 0;

    if (!as || !results) return -1;

    for (i = 0; i < ATOM_TABLE_SIZE && (uint32_t)count < max; i++) {
        AtomTableEntry *entry = as->buckets[i];
        while (entry && (uint32_t)count < max) {
            if (entry->atom && entry->atom->type == link_type &&
                entry->atom->data) {
                LinkData *ld = (LinkData *)entry->atom->data;
                if (ld->arity >= 2) {
                    int match = 1;
                    if (source != 0 && ld->outgoing[0] != source) match = 0;
                    if (target != 0 && ld->outgoing[1] != target) match = 0;
                    if (match) {
                        results[count++] = entry->atom_id;
                    }
                }
            }
            entry = entry->next;
        }
    }

    return count;
}

void pattern_match_print_stats(MatchResult *result)
{
    if (!result) return;

    printf("\nPattern Match Results:\n");
    printf("  Matches found: %u\n", result->result_count);
    printf("  Atoms examined: %u\n", result->atoms_examined);
    printf("  Time: %lu us\n", (unsigned long)result->time_us);

    if (result->result_count > 0 && result->atoms_examined > 0) {
        printf("  Selectivity: %.2f%%\n",
               100.0f * (float)result->result_count / (float)result->atoms_examined);
    }
}
