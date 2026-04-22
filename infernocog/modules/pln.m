# Probabilistic Logic Networks (PLN) Kernel Module for InfernoCog
# Implements uncertain reasoning as a kernel-level service
#
# Copyright (C) 2025 OpenCog Foundation
# License: AGPL-3.0

implement PLNKernel;

include "sys.m";
    sys: Sys;
include "atomspace.m";
    atomspace: AtomSpaceKernel;
    Atom, AtomSpace, TruthValue: import atomspace;

PLNKernel: module {
    PATH: con "/dis/cog/pln.dis";
    
    # Inference rule types
    DEDUCTION: con 1;
    INDUCTION: con 2;
    ABDUCTION: con 3;
    MODUS_PONENS: con 4;
    MODUS_TOLLENS: con 5;
    AND_INTRODUCTION: con 6;
    OR_INTRODUCTION: con 7;
    NEGATION: con 8;
    
    # Inference rule
    InferenceRule: adt {
        ruletype: int;             # Rule type constant
        name: string;              # Rule name
        premises: list of ref Atom; # Required premises
        conclusion: ref Atom;      # Conclusion pattern
        tv_formula: string;        # Truth value formula
        weight: real;              # Rule weight/priority
        
        new: fn(rtype: int, name: string): ref InferenceRule;
        apply: fn(rule: self ref InferenceRule, space: ref AtomSpace): ref Atom;
        applicable: fn(rule: self ref InferenceRule, space: ref AtomSpace): int;
        text: fn(rule: self ref InferenceRule): string;
    };
    
    # Rule set
    RuleSet: adt {
        rules: list of ref InferenceRule; # Collection of rules
        name: string;              # Rule set name
        
        new: fn(name: string): ref RuleSet;
        add: fn(rs: self ref RuleSet, rule: ref InferenceRule);
        remove: fn(rs: self ref RuleSet, rule: ref InferenceRule);
        find: fn(rs: self ref RuleSet, name: string): ref InferenceRule;
        applicable: fn(rs: self ref RuleSet, space: ref AtomSpace): list of ref InferenceRule;
    };
    
    # Deduction: A→B, B→C ⊢ A→C
    Deduction: adt {
        implication1: ref Atom;    # A→B
        implication2: ref Atom;    # B→C
        
        new: fn(impl1: ref Atom, impl2: ref Atom): ref Deduction;
        infer: fn(d: self ref Deduction, space: ref AtomSpace): ref Atom;
        compute_tv: fn(d: self ref Deduction): ref TruthValue;
    };
    
    # Induction: Multiple instances ⊢ Generalization
    Induction: adt {
        instances: list of ref Atom; # Observed instances
        pattern: ref Atom;         # Pattern to generalize
        
        new: fn(instances: list of ref Atom, pattern: ref Atom): ref Induction;
        infer: fn(i: self ref Induction, space: ref AtomSpace): ref Atom;
        compute_tv: fn(i: self ref Induction): ref TruthValue;
    };
    
    # Abduction: A→B, B ⊢ A (hypothesis)
    Abduction: adt {
        implication: ref Atom;     # A→B
        observation: ref Atom;     # B
        
        new: fn(impl: ref Atom, obs: ref Atom): ref Abduction;
        infer: fn(a: self ref Abduction, space: ref AtomSpace): ref Atom;
        compute_tv: fn(a: self ref Abduction): ref TruthValue;
    };
    
    # Modus Ponens: A→B, A ⊢ B
    ModusPonens: adt {
        implication: ref Atom;     # A→B
        antecedent: ref Atom;      # A
        
        new: fn(impl: ref Atom, ant: ref Atom): ref ModusPonens;
        infer: fn(mp: self ref ModusPonens, space: ref AtomSpace): ref Atom;
        compute_tv: fn(mp: self ref ModusPonens): ref TruthValue;
    };
    
    # Truth value formulas
    TVFormula: adt {
        # Deduction formula: sAC = sAB * sBC * (1 - sAB + sBC) / (sAB + sBC)
        deduction: fn(tvAB: ref TruthValue, tvBC: ref TruthValue): ref TruthValue;
        
        # Induction formula
        induction: fn(instances: list of ref TruthValue): ref TruthValue;
        
        # Abduction formula
        abduction: fn(tvAB: ref TruthValue, tvB: ref TruthValue): ref TruthValue;
        
        # Modus ponens formula
        modus_ponens: fn(tvAB: ref TruthValue, tvA: ref TruthValue): ref TruthValue;
        
        # Revision (merge two truth values)
        revision: fn(tv1: ref TruthValue, tv2: ref TruthValue): ref TruthValue;
        
        # AND operation
        and_op: fn(tv1: ref TruthValue, tv2: ref TruthValue): ref TruthValue;
        
        # OR operation
        or_op: fn(tv1: ref TruthValue, tv2: ref TruthValue): ref TruthValue;
        
        # NOT operation
        not_op: fn(tv: ref TruthValue): ref TruthValue;
    };
    
    # Inference context
    InferenceContext: adt {
        space: ref AtomSpace;      # Working AtomSpace
        rules: ref RuleSet;        # Available rules
        max_steps: int;            # Maximum inference steps
        current_step: int;         # Current step
        trace: list of ref Atom;   # Inference trace
        
        new: fn(space: ref AtomSpace, rules: ref RuleSet, max_steps: int): ref InferenceContext;
        step: fn(ctx: self ref InferenceContext): ref Atom;
        infer: fn(ctx: self ref InferenceContext, target: ref Atom): ref Atom;
        reset: fn(ctx: self ref InferenceContext);
    };
    
    # Forward chaining
    ForwardChainer: adt {
        context: ref InferenceContext; # Inference context
        premises: list of ref Atom; # Starting premises
        conclusions: list of ref Atom; # Derived conclusions
        
        new: fn(ctx: ref InferenceContext, premises: list of ref Atom): ref ForwardChainer;
        chain: fn(fc: self ref ForwardChainer, steps: int): list of ref Atom;
        step: fn(fc: self ref ForwardChainer): ref Atom;
    };
    
    # Backward chaining
    BackwardChainer: adt {
        context: ref InferenceContext; # Inference context
        goal: ref Atom;            # Goal to prove
        subgoals: list of ref Atom; # Current subgoals
        proofs: list of ref Atom;  # Found proofs
        
        new: fn(ctx: ref InferenceContext, goal: ref Atom): ref BackwardChainer;
        chain: fn(bc: self ref BackwardChainer, steps: int): list of ref Atom;
        step: fn(bc: self ref BackwardChainer): ref Atom;
        prove: fn(bc: self ref BackwardChainer): int;
    };
    
    # PLN inference engine
    PLNEngine: adt {
        space: ref AtomSpace;      # AtomSpace to reason over
        rules: ref RuleSet;        # Inference rules
        max_steps: int;            # Maximum steps
        trace_enabled: int;        # Enable inference tracing
        
        new: fn(space: ref AtomSpace, rules: ref RuleSet): ref PLNEngine;
        
        # High-level inference
        deduce: fn(engine: self ref PLNEngine, p1: ref Atom, p2: ref Atom): ref Atom;
        induce: fn(engine: self ref PLNEngine, instances: list of ref Atom): ref Atom;
        abduce: fn(engine: self ref PLNEngine, impl: ref Atom, obs: ref Atom): ref Atom;
        
        # Chaining
        forward_chain: fn(engine: self ref PLNEngine, premises: list of ref Atom, 
                         steps: int): list of ref Atom;
        backward_chain: fn(engine: self ref PLNEngine, goal: ref Atom, 
                          steps: int): list of ref Atom;
        
        # Rule management
        add_rule: fn(engine: self ref PLNEngine, rule: ref InferenceRule);
        remove_rule: fn(engine: self ref PLNEngine, rule: ref InferenceRule);
        
        # Configuration
        set_max_steps: fn(engine: self ref PLNEngine, steps: int);
        enable_trace: fn(engine: self ref PLNEngine, enable: int);
        
        # Statistics
        stats: fn(engine: self ref PLNEngine): string;
    };
    
    # Inference trace for debugging
    InferenceTrace: adt {
        steps: list of ref InferenceStep; # Inference steps
        
        new: fn(): ref InferenceTrace;
        add: fn(trace: self ref InferenceTrace, step: ref InferenceStep);
        text: fn(trace: self ref InferenceTrace): string;
    };
    
    InferenceStep: adt {
        rule: ref InferenceRule;   # Rule applied
        premises: list of ref Atom; # Input premises
        conclusion: ref Atom;      # Output conclusion
        timestamp: int;            # Step timestamp
        
        new: fn(rule: ref InferenceRule, premises: list of ref Atom, 
               conclusion: ref Atom): ref InferenceStep;
        text: fn(step: self ref InferenceStep): string;
    };
    
    # Module initialization
    init: fn(ctxt: ref Draw->Context, args: list of string);
    
    # Kernel service interface
    kernelservice: fn(engine: ref PLNEngine);
    
    # File system interface
    plnfs: fn(engine: ref PLNEngine, mountpoint: string);
    
    # Distributed inference
    distribute_inference: fn(engine: ref PLNEngine, nodes: list of string): int;
};
