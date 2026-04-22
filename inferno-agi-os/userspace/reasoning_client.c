/*
 * Reasoning Client Library
 *
 * High-level userspace API for PLN reasoning and URE operations.
 * Wraps the cognitive filesystem client with type-safe
 * inference-specific operations.
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Forward declarations from cogfs_client */
typedef struct CogFS CogFS;
typedef struct CogInferenceResult CogInferenceResult;

extern CogFS* cogfs_init(const char *root);
extern void   cogfs_close(CogFS *cfs);
extern int    cogfs_infer_pln(CogFS *cfs, uint32_t *premises, uint32_t count,
                              CogInferenceResult *result);
extern int    cogfs_forward_chain(CogFS *cfs, uint32_t *initial, uint32_t count,
                                  int max_steps, uint32_t *results, uint32_t max);

/* Reasoning rule types */
#define RULE_DEDUCTION       "deduction"
#define RULE_INDUCTION       "induction"
#define RULE_ABDUCTION       "abduction"
#define RULE_MODUS_PONENS    "modus_ponens"
#define RULE_MODUS_TOLLENS   "modus_tollens"
#define RULE_SIMILARITY_SUB  "similarity_substitution"

/* Reasoning client handle */
typedef struct ReasoningClient {
    CogFS *cfs;
    int    auto_close;
    int    max_chain_steps;
    float  min_confidence;
} ReasoningClient;

/*
 * Create a reasoning client
 */
ReasoningClient*
reasoning_client_create(const char *cogfs_root)
{
    ReasoningClient *rc = (ReasoningClient *)calloc(1, sizeof(ReasoningClient));
    if (rc == NULL)
        return NULL;

    rc->cfs = cogfs_init(cogfs_root);
    if (rc->cfs == NULL) {
        free(rc);
        return NULL;
    }
    rc->auto_close = 1;
    rc->max_chain_steps = 10;
    rc->min_confidence = 0.1f;

    return rc;
}

/*
 * Create a reasoning client from an existing CogFS handle
 */
ReasoningClient*
reasoning_client_from_cogfs(CogFS *cfs)
{
    ReasoningClient *rc = (ReasoningClient *)calloc(1, sizeof(ReasoningClient));
    if (rc == NULL)
        return NULL;

    rc->cfs = cfs;
    rc->auto_close = 0;
    rc->max_chain_steps = 10;
    rc->min_confidence = 0.1f;

    return rc;
}

/*
 * Destroy the reasoning client
 */
void
reasoning_client_destroy(ReasoningClient *rc)
{
    if (rc != NULL) {
        if (rc->auto_close && rc->cfs != NULL)
            cogfs_close(rc->cfs);
        free(rc);
    }
}

/*
 * Set maximum forward chain steps
 */
void
reasoning_set_max_steps(ReasoningClient *rc, int max_steps)
{
    if (rc != NULL && max_steps > 0)
        rc->max_chain_steps = max_steps;
}

/*
 * Set minimum confidence threshold for results
 */
void
reasoning_set_min_confidence(ReasoningClient *rc, float min_conf)
{
    if (rc != NULL && min_conf >= 0.0f && min_conf <= 1.0f)
        rc->min_confidence = min_conf;
}

/*
 * Perform PLN deduction: given A->B and B->C, derive A->C
 */
int
reasoning_deduction(ReasoningClient *rc, uint32_t link_ab, uint32_t link_bc,
                    CogInferenceResult *result)
{
    uint32_t premises[2] = { link_ab, link_bc };
    return cogfs_infer_pln(rc->cfs, premises, 2, result);
}

/*
 * Perform PLN induction: given A->C and B->C, derive A->B
 */
int
reasoning_induction(ReasoningClient *rc, uint32_t link_ac, uint32_t link_bc,
                    CogInferenceResult *result)
{
    uint32_t premises[2] = { link_ac, link_bc };
    return cogfs_infer_pln(rc->cfs, premises, 2, result);
}

/*
 * Perform PLN abduction: given A->B and A->C, derive B->C
 */
int
reasoning_abduction(ReasoningClient *rc, uint32_t link_ab, uint32_t link_ac,
                    CogInferenceResult *result)
{
    uint32_t premises[2] = { link_ab, link_ac };
    return cogfs_infer_pln(rc->cfs, premises, 2, result);
}

/*
 * Perform forward chaining from initial atoms
 */
int
reasoning_forward_chain(ReasoningClient *rc, uint32_t *initial_atoms,
                        uint32_t count, uint32_t *results, uint32_t max_results)
{
    return cogfs_forward_chain(rc->cfs, initial_atoms, count,
                               rc->max_chain_steps, results, max_results);
}

/*
 * Perform backward chaining to prove a goal
 * Writes goal to /reasoning/backward and reads evidence
 */
int
reasoning_backward_chain(ReasoningClient *rc, uint32_t goal_atom,
                         uint32_t *evidence, uint32_t max_evidence)
{
    /* Use forward chain infrastructure with goal as initial atom */
    return cogfs_forward_chain(rc->cfs, &goal_atom, 1,
                               rc->max_chain_steps, evidence, max_evidence);
}

/*
 * Check if a conclusion is derivable from premises
 * Returns 1 if derivable, 0 if not, negative on error
 */
int
reasoning_is_derivable(ReasoningClient *rc, uint32_t *premises,
                       uint32_t premise_count, uint32_t conclusion)
{
    CogInferenceResult result;
    int ret = cogfs_infer_pln(rc->cfs, premises, premise_count, &result);
    if (ret < 0)
        return ret;

    if (result.conclusion_id == conclusion &&
        result.tv_confidence >= rc->min_confidence)
        return 1;

    return 0;
}

/*
 * Print inference result
 */
void
reasoning_print_result(const CogInferenceResult *result)
{
    printf("Inference Result:\n");
    printf("  Conclusion: atom %u\n", result->conclusion_id);
    printf("  Truth Value: <%.4f, %.4f>\n",
           result->tv_strength, result->tv_confidence);
    printf("  Rule: %s\n", result->rule);
    printf("  Steps: %d\n", result->step_count);
}
