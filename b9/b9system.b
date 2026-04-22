implement B9System;

#
# b9 - B-Series Rooted Trees (localhost)
#
# Implements connection edge patterns to localhost terminal nodes.
# B-series rooted trees encode local atom connectivity using
# binary serialization for efficient storage and retrieval.
#
# Copyright (C) 2026 OpenCog Community
# Licensed under AGPL-3.0
#

include "sys.m";
    sys: Sys;

include "draw.m";

B9System: module {
    PATH: con "/dis/b9/b9system.dis";

    init: fn();

    # B-tree rooted at a single atom
    BTree: adt {
        root:      int;              # Root atom ID
        edges:     list of ref BEdge;
        terminals: list of ref TerminalNode;

        add_edge:     fn(bt: self ref BTree,
                          from_id: int, to_id: int,
                          pattern: string, weight: real): int;
        route_local:  fn(bt: self ref BTree, atom_id: int): ref TerminalNode;
        serialize:    fn(bt: self ref BTree): array of byte;
        to_string:    fn(bt: self ref BTree): string;
    };

    # Edge connecting two atoms in the b-tree
    BEdge: adt {
        from_id:  int;
        to_id:    int;
        pattern:  string;
        weight:   real;
    };

    # Localhost terminal node
    TerminalNode: adt {
        id:       int;
        endpoint: string;   # /net/localhost/...
        capacity: int;
        load:     int;

        is_available: fn(t: self ref TerminalNode): int;
    };

    # Create a new b-tree
    create_tree:    fn(root_id: int): ref BTree;

    # Add a terminal node
    add_terminal:   fn(tree: ref BTree, endpoint: string, capacity: int): ref TerminalNode;

    # Serialize/deserialize
    serialize:      fn(tree: ref BTree): array of byte;
    deserialize:    fn(data: array of byte): ref BTree;
};

node_counter := 0;

init()
{
    sys = load Sys Sys->PATH;
}

create_tree(root_id: int): ref BTree
{
    bt := ref BTree;
    bt.root      = root_id;
    bt.edges     = nil;
    bt.terminals = nil;
    return bt;
}

add_terminal(tree: ref BTree, endpoint: string, capacity: int): ref TerminalNode
{
    t := ref TerminalNode;
    t.id       = node_counter++;
    t.endpoint = endpoint;
    t.capacity = capacity;
    t.load     = 0;
    tree.terminals = t :: tree.terminals;
    return t;
}

BTree.add_edge(bt: self ref BTree, from_id: int, to_id: int,
               pattern: string, weight: real): int
{
    e := ref BEdge;
    e.from_id  = from_id;
    e.to_id    = to_id;
    e.pattern  = pattern;
    e.weight   = weight;
    bt.edges = e :: bt.edges;
    return 0;
}

BTree.route_local(bt: self ref BTree, atom_id: int): ref TerminalNode
{
    # Find least-loaded terminal with capacity
    best: ref TerminalNode = nil;
    for(ts := bt.terminals; ts != nil; ts = tl ts) {
        t := hd ts;
        if(t.is_available() && (best == nil || t.load < best.load))
            best = t;
    }
    USED(atom_id);
    return best;
}

BTree.serialize(bt: self ref BTree): array of byte
{
    s := bt.to_string();
    return array of byte s;
}

BTree.to_string(bt: self ref BTree): string
{
    s := sys->sprint("BTree(root=%d, edges=%d, terminals=%d)",
        bt.root, listlen(bt.edges), listlen(bt.terminals));
    return s;
}

TerminalNode.is_available(t: self ref TerminalNode): int
{
    return t.load < t.capacity;
}

serialize(tree: ref BTree): array of byte
{
    return tree.serialize();
}

deserialize(data: array of byte): ref BTree
{
    # Simplified: create empty tree
    # A production implementation would parse the serialized format
    s := string data;
    root_id := 0;
    # Find "root=" in the string and parse the integer that follows
    for(i := 0; i + 5 < len s; i++) {
        if(s[i:i+5] == "root=") {
            root_id = int s[i+5:];
            break;
        }
    }
    return create_tree(root_id);
}

# Helper
listlen[T](l: list of T): int
{
    n := 0;
    for(; l != nil; l = tl l)
        n++;
    return n;
}
