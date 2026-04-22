/*
 * decay.c - Attention Decay for Inferno Kernel
 *
 * Implements STI decay for all atoms below the attentional focus boundary.
 * Atoms with STI above the boundary are subject to rent collection.
 * Atoms with STI below zero are candidates for forgetting.
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
 * atomdecayattn - Apply STI decay to all atoms.
 *
 * For each atom, STI is multiplied by (1 - rate).
 * Returns the number of atoms affected.
 */
int
atomdecayattn(float rate)
{
    AtomSpace *as;
    uint i, count;
    Atom *atom;

    as = kernelspace;
    if(as == nil)
        return 0;

    lock(&as->lock);
    count = 0;
    for(i = 0; i < as->natoms; i++) {
        atom = as->atoms[i];
        if(atom == nil) continue;
        lock(&atom->lock);
        atom->av.sti = (short)(atom->av.sti * (1.0f - rate));
        unlock(&atom->lock);
        count++;
    }
    unlock(&as->lock);

    return count;
}

/*
 * atomgetsti - Return the current STI of an atom.
 */
int
atomgetsti(ulong id)
{
    Atom *atom;
    int sti;

    atom = atomget(id);
    if(atom == nil)
        return 0;

    lock(&atom->lock);
    sti = atom->av.sti;
    unlock(&atom->lock);

    return sti;
}
