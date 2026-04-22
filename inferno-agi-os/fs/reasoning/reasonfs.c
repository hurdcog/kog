/*
 * Reasoning Filesystem (reasonfs)
 *
 * Exposes PLN reasoning and URE inference as a 9P filesystem.
 *
 * Filesystem layout:
 *   /reasoning/
 *     pln          - Write premises (atom IDs), read conclusions
 *     forward      - Write initial atoms + max_steps, read derived atoms
 *     backward     - Write goal atom, read evidence chain
 *     rules/       - Directory of available inference rules
 *       deduction  - Read rule description
 *       induction  - Read rule description
 *       abduction  - Read rule description
 *     results/     - Recent inference results
 *       latest     - Read most recent result
 *     stats        - Read reasoning engine statistics
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#endif

/* Maximum inference results */
#define REASONFS_MAX_RESULTS   256
#define REASONFS_MAX_RULES     32
#define REASONFS_MAX_BUF       4096

/* Inference rule */
typedef struct ReasonFSRule {
    char     name[64];
    char     description[256];
    int      premise_count;
    int      active;
} ReasonFSRule;

/* Inference result */
typedef struct ReasonFSResult {
    uint32_t conclusion_id;
    float    tv_strength;
    float    tv_confidence;
    char     rule_name[64];
    int      step_count;
    int64_t  timestamp;
} ReasonFSResult;

/* ReasonFS state */
typedef struct ReasonFS {
    ReasonFSRule   rules[REASONFS_MAX_RULES];
    int            rule_count;
    ReasonFSResult results[REASONFS_MAX_RESULTS];
    int            result_count;
    int            result_head;
    int            total_inferences;
    int            initialized;
#ifdef PLATFORM_WINDOWS
    CRITICAL_SECTION lock;
#else
    pthread_mutex_t  lock;
#endif
} ReasonFS;

static ReasonFS g_reasonfs;

/*
 * Initialize the reasoning filesystem
 */
int
reasonfs_init(void)
{
    memset(&g_reasonfs, 0, sizeof(ReasonFS));
    g_reasonfs.initialized = 1;

#ifdef PLATFORM_WINDOWS
    InitializeCriticalSection(&g_reasonfs.lock);
#else
    pthread_mutex_init(&g_reasonfs.lock, NULL);
#endif

    /* Register built-in PLN rules */
    struct { const char *name; const char *desc; int premises; } builtin[] = {
        {"deduction",    "A->B, B->C => A->C with PLN truth value formula", 2},
        {"induction",    "A->C, B->C => A->B with PLN induction formula",   2},
        {"abduction",    "A->B, A->C => B->C with PLN abduction formula",   2},
        {"modus_ponens", "A, A->B => B with modus ponens truth value",      2},
        {"modus_tollens","~B, A->B => ~A with modus tollens truth value",   2},
        {"similarity_substitution", "A~B, P(A) => P(B) with similarity substitution", 2},
    };

    for (int i = 0; i < 6; i++) {
        strncpy(g_reasonfs.rules[i].name, builtin[i].name, 63);
        strncpy(g_reasonfs.rules[i].description, builtin[i].desc, 255);
        g_reasonfs.rules[i].premise_count = builtin[i].premises;
        g_reasonfs.rules[i].active = 1;
    }
    g_reasonfs.rule_count = 6;

    return 0;
}

/*
 * Shutdown the reasoning filesystem
 */
void
reasonfs_shutdown(void)
{
    g_reasonfs.initialized = 0;

#ifdef PLATFORM_WINDOWS
    DeleteCriticalSection(&g_reasonfs.lock);
#else
    pthread_mutex_destroy(&g_reasonfs.lock);
#endif
}

/*
 * PLN deduction truth value formula
 * sAC = sAB * sBC + (1 - sAB) * sC
 * cAC = min(cAB, cBC) * (sAB * sBC)
 */
static void
pln_deduction_tv(float sAB, float cAB, float sBC, float cBC,
                 float sC, float *sAC, float *cAC)
{
    *sAC = sAB * sBC + (1.0f - sAB) * sC;
    *cAC = fminf(cAB, cBC) * (sAB * sBC);
    if (*cAC < 0.0f) *cAC = 0.0f;
    if (*cAC > 1.0f) *cAC = 1.0f;
    if (*sAC < 0.0f) *sAC = 0.0f;
    if (*sAC > 1.0f) *sAC = 1.0f;
}

/*
 * PLN induction truth value formula
 */
static void
pln_induction_tv(float sAC, float cAC, float sBC, float cBC,
                 float *sAB, float *cAB)
{
    if (sBC > 0.001f) {
        *sAB = sAC / sBC;
        if (*sAB > 1.0f) *sAB = 1.0f;
    } else {
        *sAB = 0.0f;
    }
    *cAB = fminf(cAC, cBC) * 0.5f; /* Lower confidence for induction */
}

/*
 * Record an inference result
 */
static void
reasonfs_record_result(uint32_t conclusion_id, float strength, float confidence,
                       const char *rule_name, int steps)
{
#ifdef PLATFORM_WINDOWS
    EnterCriticalSection(&g_reasonfs.lock);
#else
    pthread_mutex_lock(&g_reasonfs.lock);
#endif

    int idx = g_reasonfs.result_head;
    g_reasonfs.results[idx].conclusion_id = conclusion_id;
    g_reasonfs.results[idx].tv_strength = strength;
    g_reasonfs.results[idx].tv_confidence = confidence;
    strncpy(g_reasonfs.results[idx].rule_name, rule_name, 63);
    g_reasonfs.results[idx].step_count = steps;
    g_reasonfs.results[idx].timestamp = 0; /* Would use time() in real system */

    g_reasonfs.result_head = (g_reasonfs.result_head + 1) % REASONFS_MAX_RESULTS;
    if (g_reasonfs.result_count < REASONFS_MAX_RESULTS)
        g_reasonfs.result_count++;
    g_reasonfs.total_inferences++;

#ifdef PLATFORM_WINDOWS
    LeaveCriticalSection(&g_reasonfs.lock);
#else
    pthread_mutex_unlock(&g_reasonfs.lock);
#endif
}

/*
 * Handle write to /reasoning/pln
 * Input: space-separated premise atom IDs
 * Performs deduction and records result
 */
int
reasonfs_infer_pln(uint32_t *premises, uint32_t count,
                   uint32_t *conclusion_id, float *strength, float *confidence)
{
    if (!g_reasonfs.initialized || count < 2)
        return -1;

    /* Simplified: use deduction formula with default TVs */
    float sAB = 0.9f, cAB = 0.8f;
    float sBC = 0.85f, cBC = 0.75f;
    float sC = 0.5f;

    pln_deduction_tv(sAB, cAB, sBC, cBC, sC, strength, confidence);

    /* Generate a conclusion ID (would normally create an atom) */
    *conclusion_id = premises[0] + premises[1] + 1000;

    reasonfs_record_result(*conclusion_id, *strength, *confidence, "deduction", 1);

    return 0;
}

/*
 * Handle read from /reasoning/results/latest
 */
int
reasonfs_get_latest(char *buf, size_t maxlen)
{
    if (!g_reasonfs.initialized || g_reasonfs.result_count == 0)
        return -1;

#ifdef PLATFORM_WINDOWS
    EnterCriticalSection(&g_reasonfs.lock);
#else
    pthread_mutex_lock(&g_reasonfs.lock);
#endif

    int idx = (g_reasonfs.result_head - 1 + REASONFS_MAX_RESULTS) % REASONFS_MAX_RESULTS;
    ReasonFSResult *r = &g_reasonfs.results[idx];

    int n = snprintf(buf, maxlen,
        "%u %.4f %.4f %s %d",
        r->conclusion_id, r->tv_strength, r->tv_confidence,
        r->rule_name, r->step_count);

#ifdef PLATFORM_WINDOWS
    LeaveCriticalSection(&g_reasonfs.lock);
#else
    pthread_mutex_unlock(&g_reasonfs.lock);
#endif

    return n;
}

/*
 * Handle read from /reasoning/rules/<name>
 */
int
reasonfs_get_rule(const char *name, char *buf, size_t maxlen)
{
    if (!g_reasonfs.initialized)
        return -1;

    for (int i = 0; i < g_reasonfs.rule_count; i++) {
        if (strcmp(g_reasonfs.rules[i].name, name) == 0) {
            return snprintf(buf, maxlen, "%s: %s (premises: %d)",
                g_reasonfs.rules[i].name,
                g_reasonfs.rules[i].description,
                g_reasonfs.rules[i].premise_count);
        }
    }

    return -1;
}

/*
 * Handle read from /reasoning/stats
 */
int
reasonfs_stats(char *buf, size_t maxlen)
{
    if (!g_reasonfs.initialized)
        return -1;

    return snprintf(buf, maxlen,
        "Reasoning Filesystem Statistics\n"
        "  Active rules: %d\n"
        "  Total inferences: %d\n"
        "  Cached results: %d\n",
        g_reasonfs.rule_count,
        g_reasonfs.total_inferences,
        g_reasonfs.result_count);
}
