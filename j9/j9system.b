implement J9System;

#
# j9 - J-Surface Elementary Differentials (orgalhost)
#
# Implements distribution compute gradients for the orgalhost topology net.
# J-surface differentials track how atom importance and truth values
# change across the distributed cognitive network, enabling gradient-based
# coordination of distributed learning and inference.
#
# Copyright (C) 2026 OpenCog Community
# Licensed under AGPL-3.0
#

include "sys.m";
    sys: Sys;

include "draw.m";

J9System: module {
    PATH: con "/dis/j9/j9system.dis";

    init: fn();

    # J-surface differential for an atom
    JDifferential: adt {
        atom_id:    int;
        delta_tvs:  real;    # Change in truth value strength
        delta_tvc:  real;    # Change in truth value confidence
        delta_sti:  int;     # Change in short-term importance
        delta_imp:  real;    # Change in composite importance
        timestamp:  int;

        magnitude:  fn(j: self ref JDifferential): real;
        to_string:  fn(j: self ref JDifferential): string;
    };

    # Gradient vector over the topology net
    Gradient: adt {
        id:         int;
        dx:         real;    # x-component
        dy:         real;    # y-component
        dz:         real;    # z-component (optional depth)
        magnitude:  real;
        source_id:  int;     # Source atom ID
        target_ids: list of int;

        propagate:  fn(g: self ref Gradient, topo: ref TopologyNet): int;
        to_string:  fn(g: self ref Gradient): string;
    };

    # Organizational topology network
    TopologyNet: adt {
        nodes:    list of ref TopologyNode;
        endpoint: string;   # /net/orgalhost/...

        add_node:    fn(t: self ref TopologyNet, x: real, y: real, cap: int): ref TopologyNode;
        nearest:     fn(t: self ref TopologyNet, x: real, y: real): ref TopologyNode;
        to_string:   fn(t: self ref TopologyNet): string;
    };

    # Node in the topology network
    TopologyNode: adt {
        id:       int;
        x:        real;
        y:        real;
        capacity: int;
        load:     int;
    };

    # Create a differential for an atom
    create_differential: fn(atom_id: int, dtvs: real, dtvc: real, dsti: int): ref JDifferential;

    # Compute gradient from differential
    compute_gradient:    fn(diff: ref JDifferential): ref Gradient;

    # Create topology net
    create_topology:     fn(endpoint: string): ref TopologyNet;
};

grad_counter := 0;
node_counter := 0;
time_counter := 0;

init()
{
    sys = load Sys Sys->PATH;
}

create_differential(atom_id: int, dtvs: real, dtvc: real, dsti: int): ref JDifferential
{
    j := ref JDifferential;
    j.atom_id   = atom_id;
    j.delta_tvs = dtvs;
    j.delta_tvc = dtvc;
    j.delta_sti = dsti;
    j.delta_imp = dtvs * dtvc + real dsti / 1000.0;
    j.timestamp = time_counter++;
    return j;
}

compute_gradient(diff: ref JDifferential): ref Gradient
{
    g := ref Gradient;
    g.id        = grad_counter++;
    g.dx        = diff.delta_tvs;
    g.dy        = diff.delta_tvc;
    g.dz        = diff.delta_imp;
    g.magnitude = diff.magnitude();
    g.source_id = diff.atom_id;
    g.target_ids = nil;
    return g;
}

create_topology(endpoint: string): ref TopologyNet
{
    t := ref TopologyNet;
    t.nodes    = nil;
    t.endpoint = endpoint;
    return t;
}

JDifferential.magnitude(j: self ref JDifferential): real
{
    d := j.delta_tvs * j.delta_tvs +
         j.delta_tvc * j.delta_tvc +
         j.delta_imp * j.delta_imp;
    # Approximate sqrt
    if(d <= 0.0)
        return 0.0;
    x := d;
    x = (x + d/x) / 2.0;
    x = (x + d/x) / 2.0;
    return x;
}

JDifferential.to_string(j: self ref JDifferential): string
{
    return sys->sprint("JDiff(atom=%d, dtvs=%.3f, dtvc=%.3f, dsti=%d, t=%d)",
        j.atom_id, j.delta_tvs, j.delta_tvc, j.delta_sti, j.timestamp);
}

Gradient.propagate(g: self ref Gradient, topo: ref TopologyNet): int
{
    if(topo == nil)
        return 0;
    count := 0;
    for(ns := topo.nodes; ns != nil; ns = tl ns) {
        n := hd ns;
        if(n == nil) continue;
        sys->print("j9: propagate gradient %d → node %d (%.2f,%.2f)\n",
            g.id, n.id, n.x, n.y);
        count++;
    }
    return count;
}

Gradient.to_string(g: self ref Gradient): string
{
    return sys->sprint("Gradient(%d, src=%d, |g|=%.3f, [%.3f,%.3f,%.3f])",
        g.id, g.source_id, g.magnitude, g.dx, g.dy, g.dz);
}

TopologyNet.add_node(t: self ref TopologyNet, x: real, y: real, cap: int): ref TopologyNode
{
    n := ref TopologyNode;
    n.id       = node_counter++;
    n.x        = x;
    n.y        = y;
    n.capacity = cap;
    n.load     = 0;
    t.nodes = n :: t.nodes;
    return n;
}

TopologyNet.nearest(t: self ref TopologyNet, x: real, y: real): ref TopologyNode
{
    best: ref TopologyNode = nil;
    best_dist := 1.0e30;
    for(ns := t.nodes; ns != nil; ns = tl ns) {
        n := hd ns;
        dx := n.x - x;
        dy := n.y - y;
        dist := dx*dx + dy*dy;
        if(dist < best_dist) {
            best_dist = dist;
            best = n;
        }
    }
    return best;
}

TopologyNet.to_string(t: self ref TopologyNet): string
{
    n := 0;
    for(ns := t.nodes; ns != nil; ns = tl ns)
        n++;
    return sys->sprint("TopologyNet(nodes=%d, ep=%s)", n, t.endpoint);
}
