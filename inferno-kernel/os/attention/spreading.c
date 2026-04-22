/*
 * spreading.c - Attention Spreading for Inferno Kernel
 *
 * Implements HebbianLink-based attention spreading (ECAN).
 * Spreads short-term importance from atoms to their neighbours
 * proportional to link weight and inverse of atom fan-out.
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
 * atomspreadattn - Spread attention from atom 'id' to its outgoing neighbours.
 *
 * Spreading formula (simplified ECAN):
 *   spread_amount = source.sti * factor / noutgoing
 *
 * Returns the number of atoms that received attention.
 */
int
atomspreadattn(ulong id, float factor)
{
    Atom *source;
    int sti_spread, i, count;

    source = atomget(id);
    if(source == nil)
        return 0;

    lock(&source->lock);
    sti_spread = source->av.sti;

    if(sti_spread <= 0 || source->noutgoing == 0) {
        unlock(&source->lock);
        return 0;
    }

    count = source->noutgoing;
    /* Reduce source STI by the total amount spread */
    source->av.sti -= (short)(sti_spread * factor);
    unlock(&source->lock);

    lock(&source->lock);
    for(i = 0; i < source->noutgoing; i++) {
        Atom *target = source->outgoing[i];
        if(target == nil) continue;
        /*
         * Weighted spread: proportional to factor, divided among fan-out.
         * Cast to float first to avoid integer overflow.
         */
        short delta = (short)((float)sti_spread * factor / (float)count);
        lock(&target->lock);
        target->av.sti += delta;
        unlock(&target->lock);
    }
    unlock(&source->lock);

    return count;
}
