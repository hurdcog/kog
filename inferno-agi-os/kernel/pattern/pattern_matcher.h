/*
 * Pattern Matching Engine
 * Full Pattern Matching with Variable Binding
 *
 * Implements:
 * - Type-based pattern matching
 * - Variable binding and unification
 * - Recursive link pattern matching
 * - Query optimization with index hints
 * - Incremental pattern matching
 *
 * Copyright (c) 2026 OpenCog Inferno Project
 * Licensed under AGPL-3.0
 */

#ifndef _PATTERN_MATCHER_H_
#define _PATTERN_MATCHER_H_

#include <stdint.h>

/* Maximum variables in a pattern */
#define PM_MAX_VARIABLES 32

/* Maximum matches returned */
#define PM_MAX_MATCHES 4096

/* Maximum pattern depth */
#define PM_MAX_DEPTH 16

/* Variable binding */
typedef struct VarBinding {
    uint32_t var_id;        /* Variable atom ID */
    uint32_t bound_id;      /* Bound atom ID */
    int is_bound;           /* Whether this variable is bound */
} VarBinding;

/* Binding set (a complete set of variable bindings for one match) */
typedef struct BindingSet {
    VarBinding bindings[PM_MAX_VARIABLES];
    uint32_t binding_count;
} BindingSet;

/* Pattern match result */
typedef struct MatchResult {
    BindingSet *results;
    uint32_t result_count;
    uint32_t max_results;
    uint32_t atoms_examined;
    uint64_t time_us;       /* Time in microseconds */
} MatchResult;

/* Pattern query */
typedef struct PatternQuery {
    uint32_t pattern_id;        /* Root atom of the pattern */
    uint32_t *variables;        /* Array of variable atom IDs */
    uint32_t variable_count;
    uint32_t max_results;
    int use_type_index;         /* Optimization hint */
} PatternQuery;

/* Forward declaration */
typedef struct AtomSpace AtomSpace;

/* ========================================================================
 * API Functions
 * ======================================================================== */

/* Pattern matching */
MatchResult* pattern_match(AtomSpace *as, PatternQuery *query);
void match_result_destroy(MatchResult *result);

/* Convenience functions */
int pattern_match_type(AtomSpace *as, uint16_t type,
                       uint32_t *results, uint32_t max);
int pattern_match_name(AtomSpace *as, const char *name,
                       uint32_t *results, uint32_t max);
int pattern_match_link(AtomSpace *as, uint16_t link_type,
                       uint32_t source, uint32_t target,
                       uint32_t *results, uint32_t max);

/* Binding operations */
BindingSet* binding_set_create(void);
void binding_set_destroy(BindingSet *bs);
int binding_set_bind(BindingSet *bs, uint32_t var_id, uint32_t atom_id);
uint32_t binding_set_lookup(BindingSet *bs, uint32_t var_id);
BindingSet* binding_set_copy(BindingSet *bs);

/* Statistics */
void pattern_match_print_stats(MatchResult *result);

#endif /* _PATTERN_MATCHER_H_ */
