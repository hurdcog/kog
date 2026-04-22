implement AtomSpace;

#
# atomspace.b - Kernel-level AtomSpace hypergraph implementation
#
# This module implements the OpenCog AtomSpace as a kernel service
# in Inferno OS, making cognitive operations first-class kernel primitives.
#
# Copyright (C) 2025 OpenCog Foundation
# SPDX-License-Identifier: AGPL-3.0-or-later
#

include "sys.m";
    sys: Sys;
    print, fprint, sprint: import sys;

include "draw.m";

include "string.m";
    str: String;

include "lists.m";
    lists: Lists;

AtomSpace: module {
    PATH: con "/dev/cog/atomspace";
    
    # Atom types
    NODE: con 1;
    LINK: con 2;
    
    # Core atom data structure
    Atom: adt {
        id: int;                    # Unique atom identifier
        atype: int;                 # Atom type (NODE or LINK)
        name: string;               # Atom name (for nodes)
        tv: ref TruthValue;         # Truth value
        av: ref AttentionValue;     # Attention value
        incoming: list of ref Atom; # Incoming set (atoms pointing to this)
        outgoing: list of ref Atom; # Outgoing set (atoms this points to)
        
        # Methods
        get_type: fn(a: self ref Atom): string;
        get_name: fn(a: self ref Atom): string;
        get_tv: fn(a: self ref Atom): ref TruthValue;
        get_av: fn(a: self ref Atom): ref AttentionValue;
        set_tv: fn(a: self ref Atom, tv: ref TruthValue);
        set_av: fn(a: self ref Atom, av: ref AttentionValue);
        add_incoming: fn(a: self ref Atom, atom: ref Atom);
        add_outgoing: fn(a: self ref Atom, atom: ref Atom);
        to_string: fn(a: self ref Atom): string;
    };
    
    # Truth value representation
    TruthValue: adt {
        strength: real;    # Probability/strength [0, 1]
        confidence: real;  # Confidence/weight [0, 1]
        
        # Methods
        get_strength: fn(tv: self ref TruthValue): real;
        get_confidence: fn(tv: self ref TruthValue): real;
        get_count: fn(tv: self ref TruthValue): real;
        to_string: fn(tv: self ref TruthValue): string;
    };
    
    # Attention value representation
    AttentionValue: adt {
        sti: int;   # Short-term importance [-32768, 32767]
        lti: int;   # Long-term importance [-32768, 32767]
        vlti: int;  # Very long-term importance [0, 65535]
        
        # Methods
        get_sti: fn(av: self ref AttentionValue): int;
        get_lti: fn(av: self ref AttentionValue): int;
        get_vlti: fn(av: self ref AttentionValue): int;
        set_sti: fn(av: self ref AttentionValue, sti: int);
        set_lti: fn(av: self ref AttentionValue, lti: int);
        set_vlti: fn(av: self ref AttentionValue, vlti: int);
        to_string: fn(av: self ref AttentionValue): string;
    };
    
    # AtomSpace database
    Space: adt {
        atoms: array of ref Atom;      # All atoms in the space
        atom_count: int;                # Number of atoms
        next_id: int;                   # Next available atom ID
        index_by_name: ref HashTable;  # Index for fast name lookup
        index_by_type: ref HashTable;  # Index for fast type lookup
        
        # Methods
        create_node: fn(s: self ref Space, atype: string, name: string, 
                       tv: ref TruthValue): ref Atom;
        create_link: fn(s: self ref Space, atype: string, 
                       outgoing: list of ref Atom, tv: ref TruthValue): ref Atom;
        get_atom: fn(s: self ref Space, id: int): ref Atom;
        get_atoms_by_name: fn(s: self ref Space, name: string): list of ref Atom;
        get_atoms_by_type: fn(s: self ref Space, atype: string): list of ref Atom;
        remove_atom: fn(s: self ref Space, atom: ref Atom): int;
        get_size: fn(s: self ref Space): int;
        clear: fn(s: self ref Space);
        to_string: fn(s: self ref Space): string;
    };
    
    # Hash table for indexing
    HashTable: adt {
        buckets: array of list of (string, ref Atom);
        size: int;
        
        put: fn(h: self ref HashTable, key: string, value: ref Atom);
        get: fn(h: self ref HashTable, key: string): list of ref Atom;
        remove: fn(h: self ref HashTable, key: string);
        hash: fn(h: self ref HashTable, key: string): int;
    };
    
    # Module initialization
    init: fn();
    
    # Factory functions
    new_atom: fn(id: int, atype: int, name: string): ref Atom;
    new_truth_value: fn(strength: real, confidence: real): ref TruthValue;
    new_attention_value: fn(sti: int, lti: int, vlti: int): ref AttentionValue;
    new_space: fn(): ref Space;
    new_hash_table: fn(size: int): ref HashTable;
    
    # Utility functions
    simple_tv: fn(): ref TruthValue;  # Default truth value (0.5, 0.5)
    default_av: fn(): ref AttentionValue;  # Default attention value (0, 0, 0)
};

# Global state
space: ref AtomSpace->Space;
next_atom_id: int;

# Initialize module
init()
{
    sys = load Sys Sys->PATH;
    str = load String String->PATH;
    lists = load Lists Lists->PATH;
    
    # Initialize global AtomSpace
    space = new_space();
    next_atom_id = 1;
    
    print("AtomSpace kernel module initialized\n");
}

# Atom implementation
Atom.get_type(a: self ref Atom): string
{
    if (a.atype == NODE)
        return "Node";
    return "Link";
}

Atom.get_name(a: self ref Atom): string
{
    return a.name;
}

Atom.get_tv(a: self ref Atom): ref TruthValue
{
    return a.tv;
}

Atom.get_av(a: self ref Atom): ref AttentionValue
{
    return a.av;
}

Atom.set_tv(a: self ref Atom, tv: ref TruthValue)
{
    a.tv = tv;
}

Atom.set_av(a: self ref Atom, av: ref AttentionValue)
{
    a.av = av;
}

Atom.add_incoming(a: self ref Atom, atom: ref Atom)
{
    a.incoming = atom :: a.incoming;
}

Atom.add_outgoing(a: self ref Atom, atom: ref Atom)
{
    a.outgoing = atom :: a.outgoing;
}

Atom.to_string(a: self ref Atom): string
{
    s := sprint("Atom[%d]: %s '%s' %s %s", 
                a.id, a.get_type(), a.name, 
                a.tv.to_string(), a.av.to_string());
    return s;
}

# TruthValue implementation
TruthValue.get_strength(tv: self ref TruthValue): real
{
    return tv.strength;
}

TruthValue.get_confidence(tv: self ref TruthValue): real
{
    return tv.confidence;
}

TruthValue.get_count(tv: self ref TruthValue): real
{
    # Convert confidence to count using standard formula
    # count = confidence / (1 - confidence)
    if (tv.confidence >= 1.0)
        return 1000000.0;  # Large number for maximum confidence
    return tv.confidence / (1.0 - tv.confidence);
}

TruthValue.to_string(tv: self ref TruthValue): string
{
    return sprint("TV(%.3f, %.3f)", tv.strength, tv.confidence);
}

# AttentionValue implementation
AttentionValue.get_sti(av: self ref AttentionValue): int
{
    return av.sti;
}

AttentionValue.get_lti(av: self ref AttentionValue): int
{
    return av.lti;
}

AttentionValue.get_vlti(av: self ref AttentionValue): int
{
    return av.vlti;
}

AttentionValue.set_sti(av: self ref AttentionValue, sti: int)
{
    av.sti = sti;
}

AttentionValue.set_lti(av: self ref AttentionValue, lti: int)
{
    av.lti = lti;
}

AttentionValue.set_vlti(av: self ref AttentionValue, vlti: int)
{
    av.vlti = vlti;
}

AttentionValue.to_string(av: self ref AttentionValue): string
{
    return sprint("AV(%d, %d, %d)", av.sti, av.lti, av.vlti);
}

# Space implementation
Space.create_node(s: self ref Space, atype: string, name: string, 
                  tv: ref TruthValue): ref Atom
{
    atom := new_atom(s.next_id++, NODE, name);
    atom.tv = tv;
    atom.av = default_av();
    
    # Add to space
    s.atoms = (s.atoms, atom);
    s.atom_count++;
    
    # Index by name and type
    s.index_by_name.put(name, atom);
    s.index_by_type.put(atype, atom);
    
    return atom;
}

Space.create_link(s: self ref Space, atype: string, 
                  outgoing: list of ref Atom, tv: ref TruthValue): ref Atom
{
    atom := new_atom(s.next_id++, LINK, "");
    atom.tv = tv;
    atom.av = default_av();
    atom.outgoing = outgoing;
    
    # Update incoming sets of outgoing atoms
    for (ol := outgoing; ol != nil; ol = tl ol) {
        out := hd ol;
        out.add_incoming(atom);
    }
    
    # Add to space
    s.atoms = (s.atoms, atom);
    s.atom_count++;
    
    # Index by type
    s.index_by_type.put(atype, atom);
    
    return atom;
}

Space.get_atom(s: self ref Space, id: int): ref Atom
{
    for (i := 0; i < len s.atoms; i++) {
        if (s.atoms[i].id == id)
            return s.atoms[i];
    }
    return nil;
}

Space.get_atoms_by_name(s: self ref Space, name: string): list of ref Atom
{
    return s.index_by_name.get(name);
}

Space.get_atoms_by_type(s: self ref Space, atype: string): list of ref Atom
{
    return s.index_by_type.get(atype);
}

Space.remove_atom(s: self ref Space, atom: ref Atom): int
{
    # Remove from incoming sets
    for (il := atom.incoming; il != nil; il = tl il) {
        inc := hd il;
        # Remove atom from inc.outgoing
    }
    
    # Remove from outgoing sets
    for (ol := atom.outgoing; ol != nil; ol = tl ol) {
        out := hd ol;
        # Remove atom from out.incoming
    }
    
    # Remove from indexes
    s.index_by_name.remove(atom.name);
    
    s.atom_count--;
    return 1;
}

Space.get_size(s: self ref Space): int
{
    return s.atom_count;
}

Space.clear(s: self ref Space)
{
    s.atoms = array[0] of ref Atom;
    s.atom_count = 0;
    s.next_id = 1;
    s.index_by_name = new_hash_table(1024);
    s.index_by_type = new_hash_table(256);
}

Space.to_string(s: self ref Space): string
{
    return sprint("AtomSpace[%d atoms]", s.atom_count);
}

# HashTable implementation
HashTable.put(h: self ref HashTable, key: string, value: ref Atom)
{
    bucket := h.hash(key);
    h.buckets[bucket] = (key, value) :: h.buckets[bucket];
}

HashTable.get(h: self ref HashTable, key: string): list of ref Atom
{
    bucket := h.hash(key);
    result: list of ref Atom;
    
    for (l := h.buckets[bucket]; l != nil; l = tl l) {
        (k, v) := hd l;
        if (k == key)
            result = v :: result;
    }
    
    return result;
}

HashTable.remove(h: self ref HashTable, key: string)
{
    bucket := h.hash(key);
    h.buckets[bucket] = nil;
}

HashTable.hash(h: self ref HashTable, key: string): int
{
    hash := 0;
    for (i := 0; i < len key; i++) {
        hash = (hash * 31 + key[i]) % h.size;
    }
    return hash;
}

# Factory functions
new_atom(id: int, atype: int, name: string): ref Atom
{
    return ref Atom(id, atype, name, nil, nil, nil, nil);
}

new_truth_value(strength: real, confidence: real): ref TruthValue
{
    return ref TruthValue(strength, confidence);
}

new_attention_value(sti: int, lti: int, vlti: int): ref AttentionValue
{
    return ref AttentionValue(sti, lti, vlti);
}

new_space(): ref Space
{
    s := ref Space;
    s.atoms = array[0] of ref Atom;
    s.atom_count = 0;
    s.next_id = 1;
    s.index_by_name = new_hash_table(1024);
    s.index_by_type = new_hash_table(256);
    return s;
}

new_hash_table(size: int): ref HashTable
{
    h := ref HashTable;
    h.buckets = array[size] of list of (string, ref Atom);
    h.size = size;
    return h;
}

simple_tv(): ref TruthValue
{
    return new_truth_value(0.5, 0.5);
}

default_av(): ref AttentionValue
{
    return new_attention_value(0, 0, 0);
}
