/*
 * tv_ops.c - Dis VM Truth Value Operation Instructions
 *
 * Extends the Dis virtual machine with truth value instructions.
 * Truth value operations become first-class VM operations enabling
 * efficient probabilistic reasoning at the bytecode level.
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include "dat.h"
#include "fns.h"
#include "interp.h"

/*
 * tv_merge instruction
 * Merge two truth values using PLN revision rule
 *
 * Stack: (tv1: TruthValue, tv2: TruthValue) -> merged: TruthValue
 *
 * Revision rule: strength = (s1*c1 + s2*c2) / (c1 + c2)
 *                confidence = c1 + c2 - c1*c2
 */
void
xtv_merge(Inst *pc)
{
    float s1, c1, s2, c2;
    float s_merged, c_merged;
    /* TruthValue is represented as two floats packed into R.s/R.t */
    s1 = R.s_f;
    c1 = R.t_f;
    s2 = R.u_f;
    c2 = R.v_f;

    if(c1 + c2 > 0.0f)
        s_merged = (s1 * c1 + s2 * c2) / (c1 + c2);
    else
        s_merged = 0.5f;

    c_merged = c1 + c2 - c1 * c2;
    if(c_merged > 1.0f) c_merged = 1.0f;

    R.s_f = s_merged;
    R.t_f = c_merged;
    R.PC = pc + 1;
}

/*
 * tv_update instruction
 * Update truth value of an atom in kernel AtomSpace
 *
 * Stack: (atom_id: int, strength: float, confidence: float) -> status: int
 */
void
xtv_update(Inst *pc)
{
    ulong id;
    float strength, confidence;
    int status;

    id = (ulong)(uintptr)R.s;
    strength = R.t_f;
    confidence = R.u_f;

    status = atomsettv(id, strength, confidence);

    R.s = (void*)(uintptr)status;
    R.PC = pc + 1;
}

/*
 * tv_propagate instruction
 * Propagate truth values through atom links
 *
 * Stack: (atom_id: int, depth: int) -> count: int
 *
 * Propagates truth values to all atoms reachable within depth hops.
 */
void
xtv_propagate(Inst *pc)
{
    ulong id;
    int depth;
    int count;

    id = (ulong)(uintptr)R.s;
    depth = (int)(uintptr)R.t;

    count = atomtvpropagate(id, depth);

    R.s = (void*)(uintptr)count;
    R.PC = pc + 1;
}

/*
 * Register truth value instructions with the Dis VM
 */
void
tv_ops_init(void)
{
    extinst[XTV_MERGE]     = xtv_merge;
    extinst[XTV_UPDATE]    = xtv_update;
    extinst[XTV_PROPAGATE] = xtv_propagate;
}
