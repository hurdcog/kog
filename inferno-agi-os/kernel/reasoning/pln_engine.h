/*
 * PLN Inference Engine
 * Probabilistic Logic Networks - Core Inference Rules
 *
 * Implements the fundamental PLN truth value formulas for:
 * - Deduction: A->B, B->C |- A->C
 * - Induction: A->B, A->C |- B->C
 * - Abduction: A->C, B->C |- A->B
 * - Modus Ponens: A, A->B |- B
 * - Revision: Merge evidence from multiple sources
 * - Similarity: Convert between inheritance and similarity
 *
 * Copyright (c) 2026 OpenCog Inferno Project
 * Licensed under AGPL-3.0
 */

#ifndef _PLN_ENGINE_H_
#define _PLN_ENGINE_H_

#include <stdint.h>
#include <math.h>

/* ========================================================================
 * PLN Truth Value Formulas
 * ======================================================================== */

/* Default confidence parameter (k) for PLN */
#define PLN_DEFAULT_K 800.0f

/* Minimum confidence threshold for valid inference */
#define PLN_MIN_CONFIDENCE 0.001f

/* Maximum inference chain depth */
#define PLN_MAX_CHAIN_DEPTH 20

/* Maximum rules per engine */
#define PLN_MAX_RULES 64

/* Maximum premises per inference step */
#define PLN_MAX_PREMISES 16

/* Truth value for PLN operations */
typedef struct PLNTruthValue {
    float strength;     /* [0.0, 1.0] probability estimate */
    float confidence;   /* [0.0, 1.0] confidence in strength */
    uint32_t count;     /* Evidence count */
} PLNTruthValue;

/* Inference result */
typedef struct PLNResult {
    uint32_t conclusion_id;     /* Atom ID of conclusion */
    PLNTruthValue tv;           /* Computed truth value */
    float rule_confidence;      /* Confidence from rule application */
    const char *rule_name;      /* Name of rule that produced this */
} PLNResult;

/* Forward declarations */
typedef struct AtomSpace AtomSpace;

/* ========================================================================
 * PLN Truth Value Computation Functions
 * ======================================================================== */

/**
 * Convert count to confidence: c = n / (n + k)
 */
static inline float pln_count_to_confidence(uint32_t count, float k)
{
    return (float)count / ((float)count + k);
}

/**
 * Convert confidence to count: n = k * c / (1 - c)
 */
static inline uint32_t pln_confidence_to_count(float confidence, float k)
{
    if (confidence >= 1.0f) return (uint32_t)(k * 1000.0f);
    if (confidence <= 0.0f) return 0;
    return (uint32_t)(k * confidence / (1.0f - confidence));
}

/**
 * PLN Deduction Formula
 *
 * Given: A->B with (sAB, cAB) and B->C with (sBC, cBC)
 * Derive: A->C with (sAC, cAC)
 *
 * sAC = sAB * sBC + (1 - sAB) * (sC - sB * sBC) / (1 - sB)
 * Simplified (when sB unknown): sAC = sAB * sBC
 * cAC = min(cAB, cBC) * sBC * sAB
 */
static inline PLNTruthValue pln_deduction(
    PLNTruthValue tvAB, PLNTruthValue tvBC,
    float sB)
{
    PLNTruthValue result;
    float sAB = tvAB.strength;
    float sBC = tvBC.strength;

    /* Full deduction formula */
    if (sB > 0.001f && sB < 0.999f) {
        float sC = sBC;  /* Approximation when sC unknown */
        result.strength = sAB * sBC + (1.0f - sAB) * (sC - sB * sBC) / (1.0f - sB);
    } else {
        /* Simplified formula */
        result.strength = sAB * sBC;
    }

    /* Clamp strength */
    if (result.strength < 0.0f) result.strength = 0.0f;
    if (result.strength > 1.0f) result.strength = 1.0f;

    /* Confidence: product of input confidences scaled by strengths */
    result.confidence = tvAB.confidence * tvBC.confidence *
                        fminf(sAB, sBC);
    if (result.confidence < PLN_MIN_CONFIDENCE) {
        result.confidence = PLN_MIN_CONFIDENCE;
    }

    result.count = (tvAB.count < tvBC.count) ? tvAB.count : tvBC.count;

    return result;
}

/**
 * PLN Induction Formula
 *
 * Given: A->B with (sAB, cAB) and A->C with (sAC, cAC)
 * Derive: B->C with (sBC, cBC)
 *
 * sBC = sAB * sAC + (1 - sAB) * sC
 * Simplified: sBC = sAC / sAB (when sAB > 0)
 * cBC = cAB * cAC * sAB^2
 */
static inline PLNTruthValue pln_induction(
    PLNTruthValue tvAB, PLNTruthValue tvAC)
{
    PLNTruthValue result;
    float sAB = tvAB.strength;
    float sAC = tvAC.strength;

    if (sAB > 0.001f) {
        result.strength = sAC * sAB;
    } else {
        result.strength = 0.0f;
    }

    /* Clamp */
    if (result.strength > 1.0f) result.strength = 1.0f;

    /* Induction has lower confidence than deduction */
    result.confidence = tvAB.confidence * tvAC.confidence * sAB * sAB;
    if (result.confidence < PLN_MIN_CONFIDENCE) {
        result.confidence = PLN_MIN_CONFIDENCE;
    }

    result.count = (tvAB.count < tvAC.count) ? tvAB.count : tvAC.count;

    return result;
}

/**
 * PLN Abduction Formula
 *
 * Given: A->C with (sAC, cAC) and B->C with (sBC, cBC)
 * Derive: A->B with (sAB, cAB)
 *
 * sAB = sAC * sBC + (1 - sAC) * (1 - sBC)
 * cAB = cAC * cBC * sBC * sAC
 */
static inline PLNTruthValue pln_abduction(
    PLNTruthValue tvAC, PLNTruthValue tvBC)
{
    PLNTruthValue result;
    float sAC = tvAC.strength;
    float sBC = tvBC.strength;

    /* Abduction formula based on shared consequence */
    result.strength = sAC * sBC + (1.0f - sAC) * (1.0f - sBC);

    /* Clamp */
    if (result.strength < 0.0f) result.strength = 0.0f;
    if (result.strength > 1.0f) result.strength = 1.0f;

    /* Abduction has lowest confidence */
    result.confidence = tvAC.confidence * tvBC.confidence * sBC * sAC;
    if (result.confidence < PLN_MIN_CONFIDENCE) {
        result.confidence = PLN_MIN_CONFIDENCE;
    }

    result.count = (tvAC.count < tvBC.count) ? tvAC.count : tvBC.count;

    return result;
}

/**
 * PLN Modus Ponens Formula
 *
 * Given: A with (sA, cA) and A->B with (sAB, cAB)
 * Derive: B with (sB, cB)
 *
 * sB = sA * sAB + (1 - sA) * sAB * (1 - sAB)
 * Simplified: sB = sA * sAB
 * cB = cA * cAB * sA
 */
static inline PLNTruthValue pln_modus_ponens(
    PLNTruthValue tvA, PLNTruthValue tvAB)
{
    PLNTruthValue result;
    float sA = tvA.strength;
    float sAB = tvAB.strength;

    result.strength = sA * sAB;

    /* Clamp */
    if (result.strength > 1.0f) result.strength = 1.0f;

    result.confidence = tvA.confidence * tvAB.confidence * sA;
    if (result.confidence < PLN_MIN_CONFIDENCE) {
        result.confidence = PLN_MIN_CONFIDENCE;
    }

    result.count = (tvA.count < tvAB.count) ? tvA.count : tvAB.count;

    return result;
}

/**
 * PLN Revision Formula
 *
 * Merge two truth values from independent evidence sources.
 *
 * s = (s1 * n1 + s2 * n2) / (n1 + n2)
 * n = n1 + n2
 * c = n / (n + k)
 */
static inline PLNTruthValue pln_revision(
    PLNTruthValue tv1, PLNTruthValue tv2)
{
    PLNTruthValue result;
    uint32_t n1 = tv1.count > 0 ? tv1.count : 1;
    uint32_t n2 = tv2.count > 0 ? tv2.count : 1;
    uint32_t n = n1 + n2;

    result.strength = (tv1.strength * (float)n1 + tv2.strength * (float)n2) / (float)n;
    result.count = n;
    result.confidence = pln_count_to_confidence(n, PLN_DEFAULT_K);

    return result;
}

/**
 * PLN Similarity from Inheritance
 *
 * Given: A->B with (sAB, cAB) and B->A with (sBA, cBA)
 * Derive: SimilarityLink(A,B) with (sSim, cSim)
 *
 * sSim = (sAB + sBA) / 2
 * cSim = min(cAB, cBA)
 */
static inline PLNTruthValue pln_similarity_from_inheritance(
    PLNTruthValue tvAB, PLNTruthValue tvBA)
{
    PLNTruthValue result;

    result.strength = (tvAB.strength + tvBA.strength) / 2.0f;
    result.confidence = fminf(tvAB.confidence, tvBA.confidence);
    result.count = (tvAB.count < tvBA.count) ? tvAB.count : tvBA.count;

    return result;
}

/* ========================================================================
 * PLN Engine API
 * ======================================================================== */

/* Rule function type for PLN engine */
typedef int (*PLNRuleFunc)(AtomSpace *as, uint32_t *premises, uint32_t count,
                           PLNResult *result);

/* PLN Rule descriptor */
typedef struct PLNRule {
    char name[64];
    PLNRuleFunc apply;
    int enabled;
    float priority;
    uint32_t min_premises;
    uint32_t max_premises;
    uint32_t application_count;
    uint32_t success_count;
} PLNRule;

/* PLN Engine */
typedef struct PLNEngine {
    AtomSpace *atomspace;
    PLNRule rules[PLN_MAX_RULES];
    uint32_t rule_count;
    uint32_t total_inferences;
    uint32_t successful_inferences;
    uint32_t max_chain_depth;
    float min_confidence_threshold;
} PLNEngine;

/* Engine lifecycle */
PLNEngine* pln_engine_create(AtomSpace *as);
void pln_engine_destroy(PLNEngine *engine);

/* Rule management */
int pln_engine_add_rule(PLNEngine *engine, const char *name,
                        PLNRuleFunc func, uint32_t min_premises,
                        uint32_t max_premises, float priority);
int pln_engine_register_default_rules(PLNEngine *engine);

/* Inference operations */
int pln_engine_infer(PLNEngine *engine, uint32_t *premises,
                     uint32_t premise_count, PLNResult *results,
                     uint32_t max_results);
int pln_engine_forward_chain(PLNEngine *engine, uint32_t *seeds,
                             uint32_t seed_count, uint32_t max_steps,
                             PLNResult *results, uint32_t max_results);
int pln_engine_backward_chain(PLNEngine *engine, uint32_t goal,
                              uint32_t max_depth, PLNResult *results,
                              uint32_t max_results);

/* Statistics */
void pln_engine_print_stats(PLNEngine *engine);

#endif /* _PLN_ENGINE_H_ */
