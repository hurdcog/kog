# AtomSpace Kernel Module for InfernoCog
# Provides hypergraph knowledge representation as a kernel-level service
#
# Copyright (C) 2025 OpenCog Foundation
# License: AGPL-3.0

implement AtomSpaceKernel;

include "sys.m";
    sys: Sys;
include "draw.m";
include "bufio.m";
    bufio: Bufio;
    Iobuf: import bufio;

AtomSpaceKernel: module {
    PATH: con "/dis/cog/atomspace.dis";
    
    # Atom types
    NODE: con 1;
    LINK: con 2;
    
    # Node subtypes
    CONCEPT_NODE: con 10;
    PREDICATE_NODE: con 11;
    VARIABLE_NODE: con 12;
    NUMBER_NODE: con 13;
    
    # Link subtypes
    INHERITANCE_LINK: con 20;
    EVALUATION_LINK: con 21;
    SIMILARITY_LINK: con 22;
    IMPLICATION_LINK: con 23;
    AND_LINK: con 24;
    OR_LINK: con 25;
    NOT_LINK: con 26;
    
    # Truth value representation
    TruthValue: adt {
        strength: real;      # Probability [0.0, 1.0]
        confidence: real;    # Confidence [0.0, 1.0]
        count: real;         # Evidence count
        
        new: fn(s: real, c: real): ref TruthValue;
        simple: fn(s: real, c: real): ref TruthValue;
        merge: fn(tv1: self ref TruthValue, tv2: ref TruthValue): ref TruthValue;
        text: fn(tv: self ref TruthValue): string;
    };
    
    # Attention value representation
    AttentionValue: adt {
        sti: int;   # Short-term importance [-32768, 32767]
        lti: int;   # Long-term importance [-32768, 32767]
        vlti: int;  # Very long-term importance [0, 65535]
        
        new: fn(sti: int, lti: int, vlti: int): ref AttentionValue;
        stimulate: fn(av: self ref AttentionValue, amount: int);
        decay: fn(av: self ref AttentionValue, rate: real);
        text: fn(av: self ref AttentionValue): string;
    };
    
    # Atom representation
    Atom: adt {
        handle: int;         # Unique atom identifier
        atomtype: int;       # Atom type constant
        name: string;        # Atom name (for nodes)
        tv: ref TruthValue;  # Truth value
        av: ref AttentionValue; # Attention value
        outgoing: list of ref Atom; # Outgoing set (for links)
        incoming: list of ref Atom; # Incoming set (cached)
        
        # Constructors
        newnode: fn(atype: int, name: string, tv: ref TruthValue): ref Atom;
        newlink: fn(atype: int, outgoing: list of ref Atom, tv: ref TruthValue): ref Atom;
        
        # Accessors
        gettype: fn(a: self ref Atom): int;
        getname: fn(a: self ref Atom): string;
        gettv: fn(a: self ref Atom): ref TruthValue;
        getav: fn(a: self ref Atom): ref AttentionValue;
        getoutgoing: fn(a: self ref Atom): list of ref Atom;
        getincoming: fn(a: self ref Atom): list of ref Atom;
        
        # Mutators
        settv: fn(a: self ref Atom, tv: ref TruthValue);
        setav: fn(a: self ref Atom, av: ref AttentionValue);
        
        # Utilities
        text: fn(a: self ref Atom): string;
        equals: fn(a: self ref Atom, b: ref Atom): int;
        hash: fn(a: self ref Atom): int;
    };
    
    # AtomSpace - the hypergraph database
    AtomSpace: adt {
        atoms: array of ref Atom;  # All atoms indexed by handle
        nodetable: ref Sys->FD;    # Node lookup table
        linktable: ref Sys->FD;    # Link lookup table
        atomcount: int;            # Total atom count
        
        # Lifecycle
        new: fn(): ref AtomSpace;
        clear: fn(space: self ref AtomSpace);
        
        # Atom creation
        addnode: fn(space: self ref AtomSpace, atype: int, name: string, 
                   tv: ref TruthValue): ref Atom;
        addlink: fn(space: self ref AtomSpace, atype: int, 
                   outgoing: list of ref Atom, tv: ref TruthValue): ref Atom;
        
        # Atom retrieval
        getatom: fn(space: self ref AtomSpace, handle: int): ref Atom;
        getnode: fn(space: self ref AtomSpace, atype: int, name: string): ref Atom;
        getlink: fn(space: self ref AtomSpace, atype: int, 
                   outgoing: list of ref Atom): ref Atom;
        
        # Atom removal
        removeatom: fn(space: self ref AtomSpace, atom: ref Atom): int;
        extractatom: fn(space: self ref AtomSpace, atom: ref Atom): ref Atom;
        
        # Queries
        getatoms: fn(space: self ref AtomSpace, atype: int): list of ref Atom;
        getincoming: fn(space: self ref AtomSpace, atom: ref Atom): list of ref Atom;
        
        # Statistics
        size: fn(space: self ref AtomSpace): int;
        nodecount: fn(space: self ref AtomSpace): int;
        linkcount: fn(space: self ref AtomSpace): int;
        
        # Persistence
        save: fn(space: self ref AtomSpace, filename: string): int;
        load: fn(space: self ref AtomSpace, filename: string): int;
        
        # Attention
        getattentionalfocus: fn(space: self ref AtomSpace, size: int): list of ref Atom;
        updateattention: fn(space: self ref AtomSpace);
        
        # Utilities
        text: fn(space: self ref AtomSpace): string;
    };
    
    # Pattern matching
    Pattern: adt {
        variables: list of string;
        clauses: list of ref Atom;
        
        new: fn(vars: list of string, clauses: list of ref Atom): ref Pattern;
        match: fn(p: self ref Pattern, space: ref AtomSpace): list of ref BindLink;
        text: fn(p: self ref Pattern): string;
    };
    
    BindLink: adt {
        pattern: ref Pattern;
        bindings: list of (string, ref Atom);
        
        new: fn(pattern: ref Pattern, bindings: list of (string, ref Atom)): ref BindLink;
        get: fn(b: self ref BindLink, var: string): ref Atom;
        text: fn(b: self ref BindLink): string;
    };
    
    # Module initialization
    init: fn(ctxt: ref Draw->Context, args: list of string);
    
    # Kernel service interface
    kernelservice: fn(space: ref AtomSpace);
    
    # File system interface
    atomspacefs: fn(space: ref AtomSpace, mountpoint: string);
    
    # Distributed operations
    export: fn(space: ref AtomSpace, addr: string): int;
    import: fn(addr: string): ref AtomSpace;
    sync: fn(local: ref AtomSpace, remote: ref AtomSpace): int;
};

# Implementation follows...
# This is the interface specification. The actual implementation
# would be in atomspace.b (Limbo source file)
