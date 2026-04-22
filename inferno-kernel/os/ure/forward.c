/*
 * forward.c - URE Forward Chaining Engine
 *
 * Implements forward chaining inference for the Unified Rule Engine.
 * Starts from a set of known facts (atoms in AtomSpace) and applies
 * rules to derive new conclusions until a goal is reached or
 * max_depth is exceeded.
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
 * ureforward - Run forward chaining from source atom up to max_depth steps.
 *
 * Returns the number of new atoms derived.
 *
 * Algorithm:
 *   1. Add source to working set
 *   2. For each atom in working set:
 *      a. Find all rules whose premise matches
 *      b. Apply rule to derive conclusion atom
 *      c. Add conclusion to AtomSpace and working set
 *   3. Repeat until depth exceeded or no new atoms
 */
int
ureforward(ulong source_id, int max_depth, ulong *results, int maxresults)
{
    ulong   worklist[512];
    int     whead, wtail, depth, count;
    ulong   cur;
    Atom   *atom;

    whead = 0;
    wtail = 0;
    count = 0;

    /* Seed the working list with the source atom */
    if(atomget(source_id) == nil)
        return 0;

    worklist[wtail++ % nelem(worklist)] = source_id;

    while(whead != wtail && depth < max_depth) {
        cur = worklist[whead++ % nelem(worklist)];
        atom = atomget(cur);
        if(atom == nil)
            continue;

        /*
         * Apply InheritanceLink forward rule:
         *   If Inherit(A, B) and Inherit(B, C) then derive Inherit(A, C)
         * This is a simplified deductive closure rule.
         */
        if(atom->noutgoing >= 2 && atom->type == ATOM_TYPE_INHERITANCE_LINK) {
            Atom *b = atom->outgoing[0];
            Atom *c = atom->outgoing[1];
            if(b != nil && c != nil && c->noutgoing > 0) {
                Atom *d = c->outgoing[0];
                if(d != nil) {
                    ulong targets[2];
                    targets[0] = b->id;
                    targets[1] = d->id;
                    ulong new_id = linkcreate(ATOM_TYPE_INHERITANCE_LINK, targets, 2);
                    if(new_id != 0 && count < maxresults) {
                        results[count++] = new_id;
                        if(wtail - whead < (int)nelem(worklist) - 1)
                            worklist[wtail++ % nelem(worklist)] = new_id;
                    }
                }
            }
        }
        depth++;
    }

    return count;
}
