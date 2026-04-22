/*
 * atom_ops.c - Dis VM Atom Operation Instructions
 *
 * Extends the Dis virtual machine with cognitive atom instructions.
 * These instructions allow direct manipulation of AtomSpace atoms
 * from Dis bytecode, making cognitive operations first-class VM operations.
 *
 * Copyright (C) 2026 OpenCog Community
 * Licensed under AGPL-3.0
 */

#include "dat.h"
#include "fns.h"
#include "interp.h"

/*
 * atom_create instruction
 * Create a new atom in kernel AtomSpace
 *
 * Stack: (type: int, name: string) -> atom_id: int
 */
void
xatom_create(Inst *pc)
{
    int type;
    String *name;
    ulong id;

    name = (String*)R.s;
    type = R.t;

    /* Delegate to AtomSpace kernel module */
    id = atomcreate((ushort)type, name != nil ? name->buf : nil);

    R.s = (void*)(uintptr)id;
    R.PC = pc + 1;
}

/*
 * atom_link instruction
 * Create a link between atoms in kernel AtomSpace
 *
 * Stack: (type: int, targets: array of int) -> link_id: int
 */
void
xatom_link(Inst *pc)
{
    int type;
    Array *targets;
    ulong id;
    ulong *tids;
    int i;

    targets = (Array*)R.s;
    type = R.t;

    if(targets == nil || targets->len == 0) {
        R.s = (void*)(uintptr)0;
        R.PC = pc + 1;
        return;
    }

    tids = malloc(targets->len * sizeof(ulong));
    if(tids == nil) {
        R.s = (void*)(uintptr)0;
        R.PC = pc + 1;
        return;
    }

    for(i = 0; i < targets->len; i++)
        tids[i] = (ulong)((int*)targets->data)[i];

    id = linkcreate((ushort)type, tids, targets->len);
    free(tids);

    R.s = (void*)(uintptr)id;
    R.PC = pc + 1;
}

/*
 * atom_query instruction
 * Query atoms from kernel AtomSpace by type
 *
 * Stack: (type: int) -> results: array of int
 */
void
xatom_query(Inst *pc)
{
    int type;
    ulong results[4096];
    int n;
    Array *arr;
    int i;

    type = R.s;

    n = atomquery((ushort)type, results, nelem(results));
    if(n < 0) n = 0;

    arr = newarray(n, sizeof(int));
    if(arr != nil) {
        for(i = 0; i < n; i++)
            ((int*)arr->data)[i] = (int)results[i];
    }

    R.s = arr;
    R.PC = pc + 1;
}

/*
 * atom_delete instruction
 * Remove an atom from kernel AtomSpace
 *
 * Stack: (atom_id: int) -> status: int
 */
void
xatom_delete(Inst *pc)
{
    ulong id;
    int status;

    id = (ulong)(uintptr)R.s;
    status = atomdelete(id);

    R.s = (void*)(uintptr)status;
    R.PC = pc + 1;
}

/*
 * Register atom instructions with the Dis VM
 */
void
atom_ops_init(void)
{
    /* Register extended instruction handlers */
    extinst[XATOM_CREATE] = xatom_create;
    extinst[XATOM_LINK]   = xatom_link;
    extinst[XATOM_QUERY]  = xatom_query;
    extinst[XATOM_DELETE] = xatom_delete;
}
