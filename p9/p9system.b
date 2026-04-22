implement P9System;

#
# p9 - P-System Nested Scopes (globalhost)
#
# Implements execution context membranes for globalhost thread pools.
# P-systems provide nested scopes with resource allocation and rule execution.
# Each membrane contains a subset of atoms, rules, and a thread pool.
#
# Copyright (C) 2026 OpenCog Community
# Licensed under AGPL-3.0
#

include "sys.m";
    sys: Sys;

include "draw.m";

P9System: module {
    PATH: con "/dis/p9/p9system.dis";

    init: fn();

    # Execution context membrane
    Membrane: adt {
        id:        int;
        name:      string;
        parent_id: int;             # -1 for root
        atom_ids:  list of int;     # Atoms inside this membrane
        pool:      ref ThreadPool;

        add_atom:  fn(m: self ref Membrane, atom_id: int);
        execute:   fn(m: self ref Membrane, task: string): int;
        to_string: fn(m: self ref Membrane): string;
    };

    # Thread pool for global execution
    ThreadPool: adt {
        id:       int;
        size:     int;
        active:   int;
        endpoint: string;   # /net/globalhost/...

        submit:   fn(p: self ref ThreadPool, task: string): int;
        is_full:  fn(p: self ref ThreadPool): int;
    };

    # Create a root membrane
    create_membrane:    fn(name: string, pool_size: int): ref Membrane;

    # Nest a child membrane inside a parent
    nest_scope:         fn(parent: ref Membrane, child: ref Membrane): int;

    # Get global thread pool
    global_pool:        fn(): ref ThreadPool;
};

mem_counter  := 0;
pool_counter := 0;

init()
{
    sys = load Sys Sys->PATH;
}

create_membrane(name: string, pool_size: int): ref Membrane
{
    m := ref Membrane;
    m.id        = mem_counter++;
    m.name      = name;
    m.parent_id = -1;
    m.atom_ids  = nil;

    pool := ref ThreadPool;
    pool.id       = pool_counter++;
    pool.size     = pool_size;
    pool.active   = 0;
    pool.endpoint = "/net/globalhost/pool_" + string pool.id;

    m.pool = pool;
    return m;
}

nest_scope(parent: ref Membrane, child: ref Membrane): int
{
    child.parent_id = parent.id;
    return 0;
}

global_pool(): ref ThreadPool
{
    pool := ref ThreadPool;
    pool.id       = 0;
    pool.size     = 64;
    pool.active   = 0;
    pool.endpoint = "/net/globalhost/global";
    return pool;
}

Membrane.add_atom(m: self ref Membrane, atom_id: int)
{
    m.atom_ids = atom_id :: m.atom_ids;
}

Membrane.execute(m: self ref Membrane, task: string): int
{
    if(m.pool == nil || m.pool.is_full())
        return -1;
    return m.pool.submit(task);
}

Membrane.to_string(m: self ref Membrane): string
{
    return sys->sprint("Membrane(%d, %s, parent=%d, atoms=%d)",
        m.id, m.name, m.parent_id, listlen(m.atom_ids));
}

ThreadPool.submit(p: self ref ThreadPool, task: string): int
{
    if(p.active >= p.size)
        return -1;
    p.active++;
    sys->print("p9: submit task '%s' to pool %d (active=%d/%d)\n",
        task, p.id, p.active, p.size);
    return 0;
}

ThreadPool.is_full(p: self ref ThreadPool): int
{
    return p.active >= p.size;
}

# Helper
listlen[T](l: list of T): int
{
    n := 0;
    for(; l != nil; l = tl l)
        n++;
    return n;
}
