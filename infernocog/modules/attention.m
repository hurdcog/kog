# Attention Allocation Kernel Module for InfernoCog
# Implements Economic Attention Networks (ECAN) as cognitive scheduler
#
# Copyright (C) 2025 OpenCog Foundation
# License: AGPL-3.0

implement AttentionKernel;

include "sys.m";
    sys: Sys;
include "atomspace.m";
    atomspace: AtomSpaceKernel;
    Atom, AtomSpace, AttentionValue: import atomspace;

AttentionKernel: module {
    PATH: con "/dis/cog/attention.dis";
    
    # Attention parameters
    AF_SIZE: con 100;              # Attentional focus size
    MIN_STI: con -32768;           # Minimum STI value
    MAX_STI: con 32767;            # Maximum STI value
    STI_FUNDS: con 100000;         # Total STI available
    LTI_FUNDS: con 100000;         # Total LTI available
    
    # Diffusion parameters
    DIFFUSION_RATE: con 0.1;       # Attention spread rate
    DECAY_RATE: con 0.95;          # Attention decay per cycle
    RENT: con 10;                  # STI rent for being in AF
    
    # Attention allocation configuration
    AttentionConfig: adt {
        af_size: int;              # Attentional focus size
        min_sti: int;              # Minimum STI threshold
        max_sti: int;              # Maximum STI threshold
        sti_funds: int;            # Total STI budget
        lti_funds: int;            # Total LTI budget
        diffusion_rate: real;      # Spread rate
        decay_rate: real;          # Decay rate
        rent: int;                 # AF rent amount
        
        default: fn(): ref AttentionConfig;
        new: fn(af_size: int, min_sti: int, max_sti: int): ref AttentionConfig;
    };
    
    # Attentional focus - high STI atoms
    AttentionalFocus: adt {
        atoms: list of ref Atom;   # Atoms in focus
        size: int;                 # Current size
        maxsize: int;              # Maximum size
        
        new: fn(maxsize: int): ref AttentionalFocus;
        add: fn(af: self ref AttentionalFocus, atom: ref Atom): int;
        remove: fn(af: self ref AttentionalFocus, atom: ref Atom): int;
        contains: fn(af: self ref AttentionalFocus, atom: ref Atom): int;
        update: fn(af: self ref AttentionalFocus, space: ref AtomSpace);
        getatoms: fn(af: self ref AttentionalFocus): list of ref Atom;
    };
    
    # Importance spreading
    ImportanceSpreading: adt {
        source: ref Atom;          # Source of spreading
        targets: list of ref Atom; # Target atoms
        amount: int;               # Amount to spread
        
        spread: fn(is: self ref ImportanceSpreading, space: ref AtomSpace);
        diffuse: fn(atom: ref Atom, space: ref AtomSpace, rate: real);
    };
    
    # Forgetting mechanism
    ForgettingAgent: adt {
        threshold: int;            # LTI threshold for forgetting
        forgotten: list of ref Atom; # Recently forgotten atoms
        
        new: fn(threshold: int): ref ForgettingAgent;
        forget: fn(fa: self ref ForgettingAgent, space: ref AtomSpace): int;
        canforget: fn(fa: self ref ForgettingAgent, atom: ref Atom): int;
    };
    
    # Hebbian learning for attention
    HebbianLearning: adt {
        learning_rate: real;       # Learning rate
        
        new: fn(rate: real): ref HebbianLearning;
        update: fn(hl: self ref HebbianLearning, atom1: ref Atom, 
                  atom2: ref Atom, space: ref AtomSpace);
        strengthen: fn(hl: self ref HebbianLearning, link: ref Atom);
    };
    
    # Attention allocation bank
    AttentionBank: adt {
        sti_funds: int;            # Available STI
        lti_funds: int;            # Available LTI
        sti_in_use: int;           # STI currently allocated
        lti_in_use: int;           # LTI currently allocated
        
        new: fn(sti: int, lti: int): ref AttentionBank;
        allocate_sti: fn(ab: self ref AttentionBank, amount: int): int;
        allocate_lti: fn(ab: self ref AttentionBank, amount: int): int;
        release_sti: fn(ab: self ref AttentionBank, amount: int);
        release_lti: fn(ab: self ref AttentionBank, amount: int);
        balance: fn(ab: self ref AttentionBank, space: ref AtomSpace);
    };
    
    # Cognitive scheduler - attention-based process scheduling
    CognitiveScheduler: adt {
        space: ref AtomSpace;      # AtomSpace to schedule
        af: ref AttentionalFocus;  # Attentional focus
        config: ref AttentionConfig; # Configuration
        bank: ref AttentionBank;   # Attention bank
        cycle: int;                # Current cycle number
        
        new: fn(space: ref AtomSpace, config: ref AttentionConfig): ref CognitiveScheduler;
        
        # Main scheduling loop
        schedule: fn(cs: self ref CognitiveScheduler): ref Atom;
        cycle: fn(cs: self ref CognitiveScheduler);
        
        # Attention operations
        stimulate: fn(cs: self ref CognitiveScheduler, atom: ref Atom, amount: int);
        spread: fn(cs: self ref CognitiveScheduler, source: ref Atom, rate: real);
        decay: fn(cs: self ref CognitiveScheduler);
        collect_rent: fn(cs: self ref CognitiveScheduler);
        
        # Focus management
        update_focus: fn(cs: self ref CognitiveScheduler);
        get_focus: fn(cs: self ref CognitiveScheduler): list of ref Atom;
        
        # Forgetting
        forget: fn(cs: self ref CognitiveScheduler, threshold: int): int;
        
        # Statistics
        stats: fn(cs: self ref CognitiveScheduler): string;
    };
    
    # Attention events for monitoring
    AttentionEvent: adt {
        eventtype: int;            # Event type
        atom: ref Atom;            # Affected atom
        old_sti: int;              # Old STI value
        new_sti: int;              # New STI value
        timestamp: int;            # Event timestamp
        
        STIMULATE: con 1;
        SPREAD: con 2;
        DECAY: con 3;
        RENT: con 4;
        FORGET: con 5;
        
        new: fn(etype: int, atom: ref Atom, old: int, new: int): ref AttentionEvent;
        text: fn(ae: self ref AttentionEvent): string;
    };
    
    # Attention monitor for debugging/visualization
    AttentionMonitor: adt {
        events: list of ref AttentionEvent; # Recent events
        maxevents: int;            # Maximum events to keep
        
        new: fn(maxevents: int): ref AttentionMonitor;
        log: fn(am: self ref AttentionMonitor, event: ref AttentionEvent);
        getevents: fn(am: self ref AttentionMonitor): list of ref AttentionEvent;
        clear: fn(am: self ref AttentionMonitor);
    };
    
    # Module initialization
    init: fn(ctxt: ref Draw->Context, args: list of string);
    
    # Kernel service interface
    kernelservice: fn(scheduler: ref CognitiveScheduler);
    
    # File system interface
    attentionfs: fn(scheduler: ref CognitiveScheduler, mountpoint: string);
    
    # Distributed attention
    sync_attention: fn(local: ref AtomSpace, remote: ref AtomSpace): int;
};
