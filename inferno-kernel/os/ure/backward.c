/*
 * backward.c - URE Backward Chaining Engine
 *
 * Implements backward chaining (goal-directed) inference for the URE.
 * Starts from a goal atom and recursively searches for supporting evidence
 * in the AtomSpace until the goal is proven or max_depth is exceeded.
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/error.h"

/*
 * urebackward - Run backward chaining from a goal atom.
 *
 * Returns the computed truth value strength for the goal, or -1.0 on failure.
 *
 * Algorithm:
 *   1. If the goal atom exists with high confidence, return its TV
 *   2. Otherwise, find rules whose conclusion unifies with goal
 *   3. Recursively prove each premise of each matching rule
 *   4. Combine premise TVs using PLN deduction formula
 *   5. Update goal atom TV with the derived value
 */
float
urebackward(ulong goal_id, int max_depth, float *confidence_out)
{
    Atom  *goal;
    float  strength, confidence;
    uint   i;

    if(max_depth <= 0)
        return -1.0f;

    goal = atomget(goal_id);
    if(goal == nil)
        return -1.0f;

    lock(&goal->lock);
    strength   = goal->tv.strength;
    confidence = goal->tv.confidence;
    unlock(&goal->lock);

    /* If already known with sufficient confidence, return directly */
    if(confidence > 0.5f) {
        if(confidence_out != nil)
            *confidence_out = confidence;
        return strength;
    }

    /*
     * Search incoming links for rules that could prove this atom.
     * For InheritanceLink(A, B): if we need to prove B, look for A
     * that inherits to B and recursively prove A.
     */
    lock(&goal->lock);
    for(i = 0; i < goal->nincoming; i++) {
        Link *rule = (Link*)goal->incoming[i];
        if(rule == nil) continue;

        if(rule->atom.type == ATOM_TYPE_INHERITANCE_LINK && rule->ntargets >= 2) {
            Atom *premise = rule->targets[0];
            if(premise == nil) continue;

            float prem_conf = 0.0f;
            float prem_str  = urebackward(premise->id, max_depth - 1, &prem_conf);

            if(prem_str >= 0.0f && prem_conf > confidence) {
                /* PLN simple deduction: s_BC = s_AB * s_AC (simplified) */
                float link_str  = rule->atom.tv.strength;
                float link_conf = rule->atom.tv.confidence;
                float derived_str  = prem_str  * link_str;
                float derived_conf = prem_conf  * link_conf;

                if(derived_conf > confidence) {
                    strength   = derived_str;
                    confidence = derived_conf;
                }
            }
        }
    }
    unlock(&goal->lock);

    /* Update goal atom with derived TV */
    if(confidence > goal->tv.confidence)
        atomsettv(goal_id, strength, confidence);

    if(confidence_out != nil)
        *confidence_out = confidence;

    return strength;
}
