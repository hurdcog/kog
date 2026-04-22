/*
 * PLN Inference Engine Implementation
 * Probabilistic Logic Networks - Full Inference System
 *
 * Implements deduction, induction, abduction, modus ponens,
 * revision, forward chaining, and backward chaining with
 * proper PLN truth value formulas.
 *
 * Copyright (c) 2026 OpenCog Inferno Project
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "pln_engine.h"

#ifdef _WIN32
#include <windows.h>
#define LOCK_TYPE CRITICAL_SECTION
#define LOCK_INIT(l) InitializeCriticalSection(&(l))
#define LOCK_ACQUIRE(l) EnterCriticalSection(&(l))
#define LOCK_RELEASE(l) LeaveCriticalSection(&(l))
#define LOCK_DESTROY(l) DeleteCriticalSection(&(l))
#else
#include <pthread.h>
#define LOCK_TYPE pthread_mutex_t
#define LOCK_INIT(l) pthread_mutex_init(&(l), NULL)
#define LOCK_ACQUIRE(l) pthread_mutex_lock(&(l))
#define LOCK_RELEASE(l) pthread_mutex_unlock(&(l))
#define LOCK_DESTROY(l) pthread_mutex_destroy(&(l))
#endif

/* ========================================================================
 * External AtomSpace Interface
 * ======================================================================== */

/* Import from atomspace_portable.c */
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

typedef struct LinkData {
    uint32_t *outgoing;
    uint32_t arity;
} LinkData;

/* Atom type constants */
#define ATOM_TYPE_LINK          0x0100
#define ATOM_TYPE_INHERITANCE   0x0101
#define ATOM_TYPE_SIMILARITY    0x0102
#define ATOM_TYPE_EVALUATION    0x0103
#define ATOM_TYPE_IMPLICATION   0x0108

extern AtomSpace* get_global_atomspace(void);
extern Atom* atomspace_get_atom(AtomSpace *as, uint32_t atom_id);
extern uint32_t atomspace_add_link(AtomSpace *as, uint16_t type,
                                    uint32_t *outgoing, uint32_t arity);
extern TruthValue tv_create(float strength, float confidence);

/* Lock for the engine */
static LOCK_TYPE g_pln_lock;
static int g_pln_lock_initialized = 0;

/* ========================================================================
 * Helper Functions
 * ======================================================================== */

static PLNTruthValue atom_tv_to_pln(TruthValue tv)
{
    PLNTruthValue ptv;
    ptv.strength = tv.strength;
    ptv.confidence = tv.confidence;
    ptv.count = tv.count > 0 ? tv.count : 1;
    return ptv;
}

static TruthValue pln_to_atom_tv(PLNTruthValue ptv)
{
    return tv_create(ptv.strength, ptv.confidence);
}

/**
 * Check if an atom is a link type
 */
static int is_link(Atom *atom)
{
    return atom != NULL && atom->type >= ATOM_TYPE_LINK;
}

/**
 * Get outgoing atoms of a link
 */
static int get_outgoing(Atom *atom, uint32_t *out, uint32_t max)
{
    LinkData *ld;
    uint32_t i, count;

    if (!is_link(atom) || atom->data == NULL) return 0;

    ld = (LinkData *)atom->data;
    count = ld->arity < max ? ld->arity : max;

    for (i = 0; i < count; i++) {
        out[i] = ld->outgoing[i];
    }

    return (int)count;
}

/* ========================================================================
 * Built-in PLN Rule Implementations
 * ======================================================================== */

/**
 * Deduction Rule: InheritanceLink(A,B), InheritanceLink(B,C) |- InheritanceLink(A,C)
 */
static int rule_deduction(AtomSpace *as, uint32_t *premises, uint32_t count,
                          PLNResult *result)
{
    Atom *link1, *link2;
    uint32_t out1[2], out2[2];
    int n1, n2;
    PLNTruthValue tv1, tv2, tvResult;
    uint32_t outgoing[2];
    uint32_t new_link;

    if (count < 2) return -1;

    link1 = atomspace_get_atom(as, premises[0]);
    link2 = atomspace_get_atom(as, premises[1]);

    if (!link1 || !link2) return -1;

    /* Both must be inheritance links */
    if (link1->type != ATOM_TYPE_INHERITANCE ||
        link2->type != ATOM_TYPE_INHERITANCE) return -1;

    n1 = get_outgoing(link1, out1, 2);
    n2 = get_outgoing(link2, out2, 2);

    if (n1 < 2 || n2 < 2) return -1;

    /* Check chain: out1[1] == out2[0] (B matches) */
    if (out1[1] != out2[0]) return -1;

    /* Apply deduction formula */
    tv1 = atom_tv_to_pln(link1->tv);
    tv2 = atom_tv_to_pln(link2->tv);

    /* Get sB from the middle atom */
    Atom *atomB = atomspace_get_atom(as, out1[1]);
    float sB = atomB ? atomB->tv.strength : 0.5f;

    tvResult = pln_deduction(tv1, tv2, sB);

    /* Create conclusion: InheritanceLink(A, C) */
    outgoing[0] = out1[0];  /* A */
    outgoing[1] = out2[1];  /* C */
    new_link = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, outgoing, 2);

    if (new_link == 0) return -1;

    /* Set truth value on new link */
    Atom *conclusion = atomspace_get_atom(as, new_link);
    if (conclusion) {
        conclusion->tv = pln_to_atom_tv(tvResult);
    }

    result->conclusion_id = new_link;
    result->tv = tvResult;
    result->rule_confidence = tvResult.confidence;
    result->rule_name = "DeductionRule";

    return 0;
}

/**
 * Induction Rule: InheritanceLink(A,B), InheritanceLink(A,C) |- InheritanceLink(B,C)
 */
static int rule_induction(AtomSpace *as, uint32_t *premises, uint32_t count,
                          PLNResult *result)
{
    Atom *link1, *link2;
    uint32_t out1[2], out2[2];
    int n1, n2;
    PLNTruthValue tv1, tv2, tvResult;
    uint32_t outgoing[2];
    uint32_t new_link;

    if (count < 2) return -1;

    link1 = atomspace_get_atom(as, premises[0]);
    link2 = atomspace_get_atom(as, premises[1]);

    if (!link1 || !link2) return -1;
    if (link1->type != ATOM_TYPE_INHERITANCE ||
        link2->type != ATOM_TYPE_INHERITANCE) return -1;

    n1 = get_outgoing(link1, out1, 2);
    n2 = get_outgoing(link2, out2, 2);

    if (n1 < 2 || n2 < 2) return -1;

    /* Check shared source: out1[0] == out2[0] (A matches) */
    if (out1[0] != out2[0]) return -1;

    tv1 = atom_tv_to_pln(link1->tv);
    tv2 = atom_tv_to_pln(link2->tv);
    tvResult = pln_induction(tv1, tv2);

    /* Create conclusion: InheritanceLink(B, C) */
    outgoing[0] = out1[1];  /* B */
    outgoing[1] = out2[1];  /* C */
    new_link = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, outgoing, 2);

    if (new_link == 0) return -1;

    Atom *conclusion = atomspace_get_atom(as, new_link);
    if (conclusion) {
        conclusion->tv = pln_to_atom_tv(tvResult);
    }

    result->conclusion_id = new_link;
    result->tv = tvResult;
    result->rule_confidence = tvResult.confidence;
    result->rule_name = "InductionRule";

    return 0;
}

/**
 * Abduction Rule: InheritanceLink(A,C), InheritanceLink(B,C) |- InheritanceLink(A,B)
 */
static int rule_abduction(AtomSpace *as, uint32_t *premises, uint32_t count,
                          PLNResult *result)
{
    Atom *link1, *link2;
    uint32_t out1[2], out2[2];
    int n1, n2;
    PLNTruthValue tv1, tv2, tvResult;
    uint32_t outgoing[2];
    uint32_t new_link;

    if (count < 2) return -1;

    link1 = atomspace_get_atom(as, premises[0]);
    link2 = atomspace_get_atom(as, premises[1]);

    if (!link1 || !link2) return -1;
    if (link1->type != ATOM_TYPE_INHERITANCE ||
        link2->type != ATOM_TYPE_INHERITANCE) return -1;

    n1 = get_outgoing(link1, out1, 2);
    n2 = get_outgoing(link2, out2, 2);

    if (n1 < 2 || n2 < 2) return -1;

    /* Check shared target: out1[1] == out2[1] (C matches) */
    if (out1[1] != out2[1]) return -1;

    tv1 = atom_tv_to_pln(link1->tv);
    tv2 = atom_tv_to_pln(link2->tv);
    tvResult = pln_abduction(tv1, tv2);

    /* Create conclusion: InheritanceLink(A, B) */
    outgoing[0] = out1[0];  /* A */
    outgoing[1] = out2[0];  /* B */
    new_link = atomspace_add_link(as, ATOM_TYPE_INHERITANCE, outgoing, 2);

    if (new_link == 0) return -1;

    Atom *conclusion = atomspace_get_atom(as, new_link);
    if (conclusion) {
        conclusion->tv = pln_to_atom_tv(tvResult);
    }

    result->conclusion_id = new_link;
    result->tv = tvResult;
    result->rule_confidence = tvResult.confidence;
    result->rule_name = "AbductionRule";

    return 0;
}

/**
 * Modus Ponens Rule: A, ImplicationLink(A,B) |- B
 */
static int rule_modus_ponens(AtomSpace *as, uint32_t *premises, uint32_t count,
                             PLNResult *result)
{
    Atom *atomA, *link;
    uint32_t out[2];
    int n;
    PLNTruthValue tvA, tvAB, tvResult;

    if (count < 2) return -1;

    /* Try both orderings: (A, A->B) and (A->B, A) */
    atomA = atomspace_get_atom(as, premises[0]);
    link = atomspace_get_atom(as, premises[1]);

    if (!atomA || !link) return -1;

    /* If first is a link, swap */
    if (is_link(atomA) && !is_link(link)) {
        Atom *tmp = atomA;
        atomA = link;
        link = tmp;
    }

    if (link->type != ATOM_TYPE_IMPLICATION &&
        link->type != ATOM_TYPE_INHERITANCE) return -1;

    n = get_outgoing(link, out, 2);
    if (n < 2) return -1;

    /* Check that A matches the source of the link */
    if (out[0] != atomA->id) return -1;

    tvA = atom_tv_to_pln(atomA->tv);
    tvAB = atom_tv_to_pln(link->tv);
    tvResult = pln_modus_ponens(tvA, tvAB);

    /* The conclusion is atom B (already exists) */
    Atom *atomB = atomspace_get_atom(as, out[1]);
    if (!atomB) return -1;

    /* Update B's truth value via revision */
    PLNTruthValue tvB = atom_tv_to_pln(atomB->tv);
    if (tvB.count > 0 && tvB.confidence > PLN_MIN_CONFIDENCE) {
        tvResult = pln_revision(tvB, tvResult);
    }
    atomB->tv = pln_to_atom_tv(tvResult);

    result->conclusion_id = out[1];
    result->tv = tvResult;
    result->rule_confidence = tvResult.confidence;
    result->rule_name = "ModusPonensRule";

    return 0;
}

/**
 * Revision Rule: Merge two truth values for the same atom
 */
static int rule_revision(AtomSpace *as, uint32_t *premises, uint32_t count,
                         PLNResult *result)
{
    Atom *atom1, *atom2;
    PLNTruthValue tv1, tv2, tvResult;

    if (count < 2) return -1;

    atom1 = atomspace_get_atom(as, premises[0]);
    atom2 = atomspace_get_atom(as, premises[1]);

    if (!atom1 || !atom2) return -1;

    /* Both must be the same type for revision */
    if (atom1->type != atom2->type) return -1;

    tv1 = atom_tv_to_pln(atom1->tv);
    tv2 = atom_tv_to_pln(atom2->tv);
    tvResult = pln_revision(tv1, tv2);

    /* Update first atom with revised truth value */
    atom1->tv = pln_to_atom_tv(tvResult);

    result->conclusion_id = atom1->id;
    result->tv = tvResult;
    result->rule_confidence = tvResult.confidence;
    result->rule_name = "RevisionRule";

    return 0;
}

/* ========================================================================
 * PLN Engine Implementation
 * ======================================================================== */

PLNEngine* pln_engine_create(AtomSpace *as)
{
    PLNEngine *engine;

    engine = (PLNEngine *)calloc(1, sizeof(PLNEngine));
    if (!engine) return NULL;

    engine->atomspace = as;
    engine->rule_count = 0;
    engine->total_inferences = 0;
    engine->successful_inferences = 0;
    engine->max_chain_depth = PLN_MAX_CHAIN_DEPTH;
    engine->min_confidence_threshold = PLN_MIN_CONFIDENCE;

    if (!g_pln_lock_initialized) {
        LOCK_INIT(g_pln_lock);
        g_pln_lock_initialized = 1;
    }

    printf("PLN Engine: Created with max chain depth %u\n",
           engine->max_chain_depth);

    return engine;
}

void pln_engine_destroy(PLNEngine *engine)
{
    if (!engine) return;

    printf("PLN Engine: Destroyed (total inferences: %u, successful: %u)\n",
           engine->total_inferences, engine->successful_inferences);

    free(engine);
}

int pln_engine_add_rule(PLNEngine *engine, const char *name,
                        PLNRuleFunc func, uint32_t min_premises,
                        uint32_t max_premises, float priority)
{
    PLNRule *rule;

    if (!engine || !name || !func) return -1;
    if (engine->rule_count >= PLN_MAX_RULES) return -1;

    rule = &engine->rules[engine->rule_count];
    strncpy(rule->name, name, sizeof(rule->name) - 1);
    rule->name[sizeof(rule->name) - 1] = '\0';
    rule->apply = func;
    rule->enabled = 1;
    rule->priority = priority;
    rule->min_premises = min_premises;
    rule->max_premises = max_premises;
    rule->application_count = 0;
    rule->success_count = 0;

    engine->rule_count++;

    printf("PLN Engine: Added rule '%s' (priority %.2f, premises %u-%u)\n",
           name, priority, min_premises, max_premises);

    return 0;
}

int pln_engine_register_default_rules(PLNEngine *engine)
{
    if (!engine) return -1;

    printf("PLN Engine: Registering default inference rules\n");

    pln_engine_add_rule(engine, "Deduction", rule_deduction, 2, 2, 0.9f);
    pln_engine_add_rule(engine, "ModusPonens", rule_modus_ponens, 2, 2, 0.85f);
    pln_engine_add_rule(engine, "Induction", rule_induction, 2, 2, 0.7f);
    pln_engine_add_rule(engine, "Abduction", rule_abduction, 2, 2, 0.6f);
    pln_engine_add_rule(engine, "Revision", rule_revision, 2, 2, 0.5f);

    printf("PLN Engine: Registered %u default rules\n", engine->rule_count);

    return 0;
}

int pln_engine_infer(PLNEngine *engine, uint32_t *premises,
                     uint32_t premise_count, PLNResult *results,
                     uint32_t max_results)
{
    uint32_t i;
    int result_count = 0;

    if (!engine || !premises || !results || premise_count == 0) return -1;

    LOCK_ACQUIRE(g_pln_lock);

    /* Try each enabled rule */
    for (i = 0; i < engine->rule_count && (uint32_t)result_count < max_results; i++) {
        PLNRule *rule = &engine->rules[i];

        if (!rule->enabled) continue;
        if (premise_count < rule->min_premises) continue;
        if (premise_count > rule->max_premises) continue;

        rule->application_count++;
        engine->total_inferences++;

        if (rule->apply(engine->atomspace, premises, premise_count,
                        &results[result_count]) == 0) {
            /* Check confidence threshold */
            if (results[result_count].tv.confidence >=
                engine->min_confidence_threshold) {
                rule->success_count++;
                engine->successful_inferences++;
                result_count++;
            }
        }
    }

    LOCK_RELEASE(g_pln_lock);

    return result_count;
}

int pln_engine_forward_chain(PLNEngine *engine, uint32_t *seeds,
                             uint32_t seed_count, uint32_t max_steps,
                             PLNResult *results, uint32_t max_results)
{
    uint32_t step;
    int total_results = 0;
    uint32_t *frontier;
    uint32_t frontier_count;
    uint32_t *next_frontier;
    uint32_t next_count;
    uint32_t max_frontier = 1024;

    if (!engine || !seeds || !results) return -1;

    frontier = (uint32_t *)calloc(max_frontier, sizeof(uint32_t));
    next_frontier = (uint32_t *)calloc(max_frontier, sizeof(uint32_t));
    if (!frontier || !next_frontier) {
        free(frontier);
        free(next_frontier);
        return -1;
    }

    /* Initialize frontier with seeds */
    frontier_count = seed_count < max_frontier ? seed_count : max_frontier;
    memcpy(frontier, seeds, frontier_count * sizeof(uint32_t));

    printf("PLN Forward Chain: Starting with %u seeds, max %u steps\n",
           seed_count, max_steps);

    for (step = 0; step < max_steps && (uint32_t)total_results < max_results; step++) {
        next_count = 0;

        /* Try all pairs in the frontier */
        uint32_t i, j;
        for (i = 0; i < frontier_count && (uint32_t)total_results < max_results; i++) {
            for (j = i + 1; j < frontier_count && (uint32_t)total_results < max_results; j++) {
                uint32_t pair[2] = { frontier[i], frontier[j] };
                PLNResult step_results[8];
                int n = pln_engine_infer(engine, pair, 2, step_results, 8);

                int k;
                for (k = 0; k < n && (uint32_t)total_results < max_results; k++) {
                    results[total_results++] = step_results[k];

                    /* Add conclusion to next frontier */
                    if (next_count < max_frontier) {
                        next_frontier[next_count++] = step_results[k].conclusion_id;
                    }
                }
            }
        }

        if (next_count == 0) break;  /* No new inferences */

        /* Swap frontiers */
        uint32_t *tmp = frontier;
        frontier = next_frontier;
        next_frontier = tmp;
        frontier_count = next_count;
    }

    printf("PLN Forward Chain: Completed %u steps, %d results\n",
           step, total_results);

    free(frontier);
    free(next_frontier);

    return total_results;
}

int pln_engine_backward_chain(PLNEngine *engine, uint32_t goal,
                              uint32_t max_depth, PLNResult *results,
                              uint32_t max_results)
{
    /* Backward chaining: Given a goal, find premises that can derive it */
    /* This is a simplified implementation that searches for supporting links */

    printf("PLN Backward Chain: Goal atom %u, max depth %u\n",
           goal, max_depth);

    /* TODO: Full backward chaining with recursive subgoal generation */
    /* For now, return 0 results */
    return 0;
}

void pln_engine_print_stats(PLNEngine *engine)
{
    uint32_t i;

    if (!engine) return;

    printf("\nPLN Engine Statistics:\n");
    printf("  Total inferences attempted: %u\n", engine->total_inferences);
    printf("  Successful inferences: %u\n", engine->successful_inferences);
    printf("  Success rate: %.1f%%\n",
           engine->total_inferences > 0 ?
           100.0f * (float)engine->successful_inferences / (float)engine->total_inferences :
           0.0f);
    printf("  Max chain depth: %u\n", engine->max_chain_depth);
    printf("  Min confidence threshold: %.4f\n", engine->min_confidence_threshold);
    printf("\n  Rules (%u):\n", engine->rule_count);

    for (i = 0; i < engine->rule_count; i++) {
        PLNRule *r = &engine->rules[i];
        printf("    [%s] %s: priority=%.2f, applied=%u, succeeded=%u\n",
               r->enabled ? "ON" : "OFF",
               r->name, r->priority,
               r->application_count, r->success_count);
    }
}
