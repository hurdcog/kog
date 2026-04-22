# Example Cognitive Agent Application for InfernoCog
# Demonstrates using cognitive system calls to create an intelligent agent
#
# Copyright (C) 2025 OpenCog Foundation
# License: AGPL-3.0

implement CogAgent;

include "sys.m";
    sys: Sys;
include "draw.m";
include "bufio.m";
    bufio: Bufio;
    Iobuf: import bufio;
include "string.m";
    str: String;

# Import cognitive modules
include "../modules/atomspace.m";
    atomspace: AtomSpaceKernel;
    Atom, AtomSpace, TruthValue, AttentionValue: import atomspace;
include "../modules/attention.m";
    attention: AttentionKernel;
    CognitiveScheduler, AttentionConfig: import attention;
include "../modules/pln.m";
    pln: PLNKernel;
    PLNEngine, RuleSet: import pln;

CogAgent: module {
    init: fn(ctxt: ref Draw->Context, args: list of string);
};

# Agent state
space: ref AtomSpace;
scheduler: ref CognitiveScheduler;
engine: ref PLNEngine;
running: int;

init(ctxt: ref Draw->Context, args: list of string)
{
    sys = load Sys Sys->PATH;
    bufio = load Bufio Bufio->PATH;
    str = load String String->PATH;
    
    # Load cognitive modules
    atomspace = load AtomSpaceKernel AtomSpaceKernel->PATH;
    attention = load AttentionKernel AttentionKernel->PATH;
    pln = load PLNKernel PLNKernel->PATH;
    
    sys->print("InfernoCog Cognitive Agent Starting...\n");
    
    # Initialize cognitive infrastructure
    space = AtomSpace.new();
    
    config := AttentionConfig.default();
    scheduler = CognitiveScheduler.new(space, config);
    
    rules := RuleSet.new("default");
    engine = PLNEngine.new(space, rules);
    
    sys->print("Cognitive infrastructure initialized\n");
    
    # Build initial knowledge base
    build_knowledge_base();
    
    # Start cognitive loop
    running = 1;
    cognitive_loop();
}

# Build initial knowledge base
build_knowledge_base()
{
    sys->print("Building knowledge base...\n");
    
    # Create some basic concepts
    tv_high := TruthValue.simple(0.9, 0.9);
    tv_medium := TruthValue.simple(0.7, 0.8);
    
    # Concepts
    dog := space.addnode(atomspace->CONCEPT_NODE, "dog", tv_high);
    cat := space.addnode(atomspace->CONCEPT_NODE, "cat", tv_high);
    mammal := space.addnode(atomspace->CONCEPT_NODE, "mammal", tv_high);
    animal := space.addnode(atomspace->CONCEPT_NODE, "animal", tv_high);
    
    # Relationships
    dog_mammal := space.addlink(atomspace->INHERITANCE_LINK, 
                                dog :: mammal :: nil, tv_high);
    cat_mammal := space.addlink(atomspace->INHERITANCE_LINK,
                                cat :: mammal :: nil, tv_high);
    mammal_animal := space.addlink(atomspace->INHERITANCE_LINK,
                                   mammal :: animal :: nil, tv_high);
    
    # Predicates
    has_fur := space.addnode(atomspace->PREDICATE_NODE, "has_fur", tv_high);
    barks := space.addnode(atomspace->PREDICATE_NODE, "barks", tv_medium);
    meows := space.addnode(atomspace->PREDICATE_NODE, "meows", tv_medium);
    
    # Evaluations
    dog_has_fur := space.addlink(atomspace->EVALUATION_LINK,
                                 has_fur :: dog :: nil, tv_high);
    dog_barks := space.addlink(atomspace->EVALUATION_LINK,
                               barks :: dog :: nil, tv_high);
    cat_meows := space.addlink(atomspace->EVALUATION_LINK,
                               meows :: cat :: nil, tv_high);
    
    sys->print(sys->sprint("Created %d atoms\n", space.size()));
}

# Main cognitive loop
cognitive_loop()
{
    cycle := 0;
    
    while(running) {
        cycle++;
        sys->print(sys->sprint("\n=== Cognitive Cycle %d ===\n", cycle));
        
        # Perceive
        percepts := perceive();
        sys->print(sys->sprint("Perceived %d stimuli\n", len percepts));
        
        # Reason
        decisions := reason(percepts);
        sys->print(sys->sprint("Generated %d decisions\n", len decisions));
        
        # Act
        act(decisions);
        
        # Update attention
        scheduler.cycle();
        
        # Display cognitive state
        display_state();
        
        # Sleep briefly
        sys->sleep(1000);
        
        # Stop after 10 cycles for demo
        if(cycle >= 10)
            running = 0;
    }
    
    sys->print("\nCognitive agent shutting down\n");
}

# Perception phase
perceive(): list of ref Atom
{
    # In a real agent, this would read from sensors
    # For demo, we'll stimulate random atoms
    
    percepts: list of ref Atom;
    
    # Get some atoms from attentional focus
    focus := scheduler.get_focus();
    
    # Stimulate perceived atoms
    for(atoms := focus; atoms != nil; atoms = tl atoms) {
        atom := hd atoms;
        scheduler.stimulate(atom, 100);
        percepts = atom :: percepts;
    }
    
    return percepts;
}

# Reasoning phase
reason(percepts: list of ref Atom): list of ref Atom
{
    decisions: list of ref Atom;
    
    # Try to infer new knowledge from percepts
    for(p1 := percepts; p1 != nil; p1 = tl p1) {
        for(p2 := percepts; p2 != nil; p2 = tl p2) {
            if(p1 == p2)
                continue;
            
            # Try deduction
            conclusion := engine.deduce(hd p1, hd p2);
            if(conclusion != nil) {
                sys->print("Deduced: " + conclusion.text() + "\n");
                decisions = conclusion :: decisions;
            }
        }
    }
    
    # Forward chain from high-attention atoms
    focus := scheduler.get_focus();
    if(len focus > 0) {
        conclusions := engine.forward_chain(focus, 3);
        for(c := conclusions; c != nil; c = tl c)
            decisions = hd c :: decisions;
    }
    
    return decisions;
}

# Action phase
act(decisions: list of ref Atom)
{
    # In a real agent, this would control actuators
    # For demo, we'll just log the decisions
    
    for(d := decisions; d != nil; d = tl d) {
        decision := hd d;
        sys->print("Action: " + decision.text() + "\n");
        
        # Stimulate acted-upon atoms
        scheduler.stimulate(decision, 50);
    }
}

# Display cognitive state
display_state()
{
    sys->print("\n--- Cognitive State ---\n");
    sys->print(sys->sprint("AtomSpace size: %d atoms\n", space.size()));
    sys->print(scheduler.stats());
    sys->print(engine.stats());
    
    # Show attentional focus
    sys->print("\nAttentional Focus:\n");
    focus := scheduler.get_focus();
    count := 0;
    for(f := focus; f != nil && count < 5; f = tl f) {
        atom := hd f;
        av := atom.getav();
        sys->print(sys->sprint("  [STI=%d] %s\n", av.sti, atom.text()));
        count++;
    }
}
