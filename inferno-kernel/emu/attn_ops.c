/*
 * attn_ops.c - Dis VM Attention Operation Instructions
 *
 * Extends the Dis virtual machine with attention allocation instructions.
 * Attention operations become first-class VM operations, enabling
 * ECAN-style attention dynamics directly in bytecode.
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include "dat.h"
#include "fns.h"
#include "interp.h"

/*
 * attn_allocate instruction
 * Allocate attention (STI) to an atom
 *
 * Stack: (atom_id: int, amount: int) -> new_sti: int
 */
void
xattn_allocate(Inst *pc)
{
    ulong id;
    short amount;
    int new_sti;

    id = (ulong)(uintptr)R.s;
    amount = (short)(int)(uintptr)R.t;

    atomstimulate(id, amount);
    new_sti = atomgetsti(id);

    R.s = (void*)(uintptr)new_sti;
    R.PC = pc + 1;
}

/*
 * attn_spread instruction
 * Spread attention from an atom to its neighbours
 *
 * Stack: (atom_id: int, spread_factor: float) -> count: int
 *
 * Implements HebbianLink-based attention spreading (ECAN).
 */
void
xattn_spread(Inst *pc)
{
    ulong id;
    float factor;
    int count;

    id = (ulong)(uintptr)R.s;
    factor = R.t_f;

    count = atomspreadattn(id, factor);

    R.s = (void*)(uintptr)count;
    R.PC = pc + 1;
}

/*
 * attn_decay instruction
 * Apply attention decay to all atoms below the attentional focus boundary
 *
 * Stack: (decay_rate: float) -> count: int
 */
void
xattn_decay(Inst *pc)
{
    float rate;
    int count;

    rate = R.s_f;
    count = atomdecayattn(rate);

    R.s = (void*)(uintptr)count;
    R.PC = pc + 1;
}

/*
 * Register attention instructions with the Dis VM
 */
void
attn_ops_init(void)
{
    extinst[XATTN_ALLOCATE] = xattn_allocate;
    extinst[XATTN_SPREAD]   = xattn_spread;
    extinst[XATTN_DECAY]    = xattn_decay;
}
