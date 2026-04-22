implement AtomSpace;

#
# AtomSpace Module for Limbo
# 
# Provides high-level interface to kernel AtomSpace services
# 
# Copyright (C) 2026 OpenCog Community
# Licensed under AGPL-3.0
#

include "sys.m";
    sys: Sys;
include "draw.m";

AtomSpace: module {
    PATH: con "/dev/atomspace";
    
    init: fn();
    
    # Atom types
    ATOM_TYPE_NODE: con 0;
    ATOM_TYPE_LINK: con 1;
    ATOM_TYPE_CONCEPT_NODE: con 2;
    ATOM_TYPE_PREDICATE_NODE: con 3;
    ATOM_TYPE_VARIABLE_NODE: con 4;
    ATOM_TYPE_SCHEMA_NODE: con 5;
    ATOM_TYPE_ANCHOR_NODE: con 6;
    
    ATOM_TYPE_INHERITANCE_LINK: con 10;
    ATOM_TYPE_SIMILARITY_LINK: con 11;
    ATOM_TYPE_EVALUATION_LINK: con 12;
    ATOM_TYPE_IMPLICATION_LINK: con 13;
    ATOM_TYPE_EQUIVALENCE_LINK: con 14;
    ATOM_TYPE_AND_LINK: con 15;
    ATOM_TYPE_OR_LINK: con 16;
    ATOM_TYPE_NOT_LINK: con 17;
    
    # Atom ADT
    Atom: adt {
        id: int;
        atype: int;
        name: string;
        tv: ref TruthValue;
        av: ref AttentionValue;
        
        # Methods
        set_tv: fn(a: self ref Atom, tv: ref TruthValue): int;
        get_tv: fn(a: self ref Atom): ref TruthValue;
        stimulate: fn(a: self ref Atom, amount: int): int;
        to_string: fn(a: self ref Atom): string;
    };
    
    # TruthValue ADT
    TruthValue: adt {
        strength: real;
        confidence: real;
        
        # Methods
        merge: fn(tv1: self ref TruthValue, tv2: ref TruthValue): ref TruthValue;
        to_string: fn(tv: self ref TruthValue): string;
    };
    
    # AttentionValue ADT
    AttentionValue: adt {
        sti: int;   # Short-term importance
        lti: int;   # Long-term importance
        vlti: int;  # Very long-term importance
        
        # Methods
        decay: fn(av: self ref AttentionValue, rate: real): ref AttentionValue;
        to_string: fn(av: self ref AttentionValue): string;
    };
    
    # Link ADT
    Link: adt {
        atom: ref Atom;
        targets: list of ref Atom;
        
        # Methods
        get_targets: fn(l: self ref Link): list of ref Atom;
        to_string: fn(l: self ref Link): string;
    };
    
    # AtomSpace operations
    create_atom: fn(atype: int, name: string): ref Atom;
    create_link: fn(atype: int, targets: list of ref Atom): ref Link;
    get_atom: fn(id: int): ref Atom;
    delete_atom: fn(id: int): int;
    
    # Query operations
    query_by_type: fn(atype: int): list of ref Atom;
    query_by_name: fn(name: string): list of ref Atom;
    query_by_pattern: fn(pattern: string): list of ref Atom;
    
    # Statistics
    get_stats: fn(): string;
};

init()
{
    sys = load Sys Sys->PATH;
}

#
# Atom methods
#

Atom.set_tv(a: self ref Atom, tv: ref TruthValue): int
{
    fd := sys->open("/dev/atomspace/ctl", Sys->OWRITE);
    if(fd == nil)
        return -1;
    
    cmd := sys->sprint("settv %d %f %f", a.id, tv.strength, tv.confidence);
    sys->fprint(fd, "%s", cmd);
    sys->close(fd);
    
    a.tv = tv;
    return 0;
}

Atom.get_tv(a: self ref Atom): ref TruthValue
{
    return a.tv;
}

Atom.stimulate(a: self ref Atom, amount: int): int
{
    fd := sys->open("/dev/atomspace/ctl", Sys->OWRITE);
    if(fd == nil)
        return -1;
    
    cmd := sys->sprint("stimulate %d %d", a.id, amount);
    sys->fprint(fd, "%s", cmd);
    sys->close(fd);
    
    a.av.sti += amount;
    return 0;
}

Atom.to_string(a: self ref Atom): string
{
    type_name := "Unknown";
    case a.atype {
        ATOM_TYPE_CONCEPT_NODE => type_name = "ConceptNode";
        ATOM_TYPE_PREDICATE_NODE => type_name = "PredicateNode";
        ATOM_TYPE_VARIABLE_NODE => type_name = "VariableNode";
        ATOM_TYPE_SCHEMA_NODE => type_name = "SchemaNode";
        ATOM_TYPE_INHERITANCE_LINK => type_name = "InheritanceLink";
        ATOM_TYPE_SIMILARITY_LINK => type_name = "SimilarityLink";
        ATOM_TYPE_EVALUATION_LINK => type_name = "EvaluationLink";
    }
    
    if(a.name != nil)
        return sys->sprint("(%s \"%s\" %s)", type_name, a.name, a.tv.to_string());
    else
        return sys->sprint("(%s %s)", type_name, a.tv.to_string());
}

#
# TruthValue methods
#

TruthValue.merge(tv1: self ref TruthValue, tv2: ref TruthValue): ref TruthValue
{
    # Simple averaging merge (can be improved with proper formula)
    merged := ref TruthValue;
    merged.strength = (tv1.strength + tv2.strength) / 2.0;
    merged.confidence = (tv1.confidence + tv2.confidence) / 2.0;
    return merged;
}

TruthValue.to_string(tv: self ref TruthValue): string
{
    return sys->sprint("<%.3f, %.3f>", tv.strength, tv.confidence);
}

#
# AttentionValue methods
#

AttentionValue.decay(av: self ref AttentionValue, rate: real): ref AttentionValue
{
    decayed := ref AttentionValue;
    decayed.sti = int(real(av.sti) * (1.0 - rate));
    decayed.lti = av.lti;
    decayed.vlti = av.vlti;
    return decayed;
}

AttentionValue.to_string(av: self ref AttentionValue): string
{
    return sys->sprint("[%d, %d, %d]", av.sti, av.lti, av.vlti);
}

#
# Link methods
#

Link.get_targets(l: self ref Link): list of ref Atom
{
    return l.targets;
}

Link.to_string(l: self ref Link): string
{
    s := l.atom.to_string();
    s += " -> (";
    for(targets := l.targets; targets != nil; targets = tl targets) {
        target := hd targets;
        s += target.to_string();
        if(tl targets != nil)
            s += ", ";
    }
    s += ")";
    return s;
}

#
# AtomSpace operations
#

create_atom(atype: int, name: string): ref Atom
{
    fd := sys->open("/dev/atomspace/ctl", Sys->OWRITE);
    if(fd == nil)
        return nil;
    
    cmd := sys->sprint("create %d %s", atype, name);
    sys->fprint(fd, "%s", cmd);
    sys->close(fd);
    
    # TODO: Read back atom ID from kernel
    atom := ref Atom;
    atom.id = 0;  # Placeholder
    atom.atype = atype;
    atom.name = name;
    atom.tv = ref TruthValue(0.5, 0.0);
    atom.av = ref AttentionValue(0, 0, 0);
    
    return atom;
}

create_link(atype: int, targets: list of ref Atom): ref Link
{
    fd := sys->open("/dev/atomspace/ctl", Sys->OWRITE);
    if(fd == nil)
        return nil;
    
    # Build target ID list
    target_ids := "";
    for(t := targets; t != nil; t = tl t) {
        target := hd t;
        target_ids += sys->sprint("%d", target.id);
        if(tl t != nil)
            target_ids += ",";
    }
    
    cmd := sys->sprint("link %d %s", atype, target_ids);
    sys->fprint(fd, "%s", cmd);
    sys->close(fd);
    
    # TODO: Read back link ID from kernel
    link := ref Link;
    link.atom = ref Atom;
    link.atom.id = 0;  # Placeholder
    link.atom.atype = atype;
    link.atom.tv = ref TruthValue(0.5, 0.0);
    link.atom.av = ref AttentionValue(0, 0, 0);
    link.targets = targets;
    
    return link;
}

get_atom(id: int): ref Atom
{
    # TODO: Query kernel for atom by ID
    return nil;
}

delete_atom(id: int): int
{
    fd := sys->open("/dev/atomspace/ctl", Sys->OWRITE);
    if(fd == nil)
        return -1;
    
    cmd := sys->sprint("delete %d", id);
    sys->fprint(fd, "%s", cmd);
    sys->close(fd);
    
    return 0;
}

#
# Query operations
#

query_by_type(atype: int): list of ref Atom
{
    # TODO: Query kernel for atoms by type
    return nil;
}

query_by_name(name: string): list of ref Atom
{
    # TODO: Query kernel for atoms by name
    return nil;
}

query_by_pattern(pattern: string): list of ref Atom
{
    # TODO: Query kernel with pattern
    return nil;
}

#
# Statistics
#

get_stats(): string
{
    fd := sys->open("/dev/atomspace/stats", Sys->OREAD);
    if(fd == nil)
        return "Error: Cannot open stats";
    
    buf := array[1024] of byte;
    n := sys->read(fd, buf, len buf);
    sys->close(fd);
    
    if(n <= 0)
        return "Error: Cannot read stats";
    
    return string buf[0:n];
}
