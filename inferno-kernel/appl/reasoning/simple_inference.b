implement SimpleInference;

#
# Simple Inference Application
# 
# Demonstrates basic reasoning using AtomSpace kernel services
# 
# Example: If "Socrates is a man" and "All men are mortal"
#          Then infer "Socrates is mortal"
# 
# Copyright (C) 2026 OpenCog Community
# Licensed under AGPL-3.0
#

include "sys.m";
    sys: Sys;
include "draw.m";
include "atomspace.m";
    atomspace: AtomSpace;

SimpleInference: module {
    init: fn(nil: ref Draw->Context, args: list of string);
};

init(nil: ref Draw->Context, args: list of string)
{
    sys = load Sys Sys->PATH;
    atomspace = load AtomSpace AtomSpace->PATH;
    
    if(atomspace == nil) {
        sys->print("Error: Cannot load AtomSpace module\n");
        return;
    }
    
    atomspace->init();
    
    sys->print("=== Simple Inference Example ===\n\n");
    
    # Create concepts
    sys->print("Creating concepts...\n");
    
    socrates := atomspace->create_atom(
        atomspace->ATOM_TYPE_CONCEPT_NODE,
        "Socrates"
    );
    
    man := atomspace->create_atom(
        atomspace->ATOM_TYPE_CONCEPT_NODE,
        "Man"
    );
    
    mortal := atomspace->create_atom(
        atomspace->ATOM_TYPE_CONCEPT_NODE,
        "Mortal"
    );
    
    if(socrates == nil || man == nil || mortal == nil) {
        sys->print("Error: Failed to create atoms\n");
        return;
    }
    
    sys->print("  Created: %s\n", socrates.to_string());
    sys->print("  Created: %s\n", man.to_string());
    sys->print("  Created: %s\n", mortal.to_string());
    sys->print("\n");
    
    # Create premise 1: "Socrates is a man"
    sys->print("Creating premise 1: Socrates is a man\n");
    
    premise1 := atomspace->create_link(
        atomspace->ATOM_TYPE_INHERITANCE_LINK,
        socrates :: man :: nil
    );
    
    # Set high truth value for premise
    premise1.atom.set_tv(ref atomspace->TruthValue(0.95, 0.95));
    
    sys->print("  Created: %s\n", premise1.to_string());
    sys->print("\n");
    
    # Create premise 2: "All men are mortal"
    sys->print("Creating premise 2: All men are mortal\n");
    
    premise2 := atomspace->create_link(
        atomspace->ATOM_TYPE_INHERITANCE_LINK,
        man :: mortal :: nil
    );
    
    # Set high truth value for premise
    premise2.atom.set_tv(ref atomspace->TruthValue(0.98, 0.98));
    
    sys->print("  Created: %s\n", premise2.to_string());
    sys->print("\n");
    
    # Perform inference: If A->B and B->C, then A->C
    sys->print("Performing inference...\n");
    
    # Create conclusion: "Socrates is mortal"
    conclusion := atomspace->create_link(
        atomspace->ATOM_TYPE_INHERITANCE_LINK,
        socrates :: mortal :: nil
    );
    
    # Calculate truth value for conclusion
    # Simple formula: min(TV1.strength, TV2.strength) for strength
    #                 min(TV1.confidence, TV2.confidence) for confidence
    tv1 := premise1.atom.get_tv();
    tv2 := premise2.atom.get_tv();
    
    conclusion_strength := tv1.strength;
    if(tv2.strength < conclusion_strength)
        conclusion_strength = tv2.strength;
    
    conclusion_confidence := tv1.confidence;
    if(tv2.confidence < conclusion_confidence)
        conclusion_confidence = tv2.confidence;
    
    conclusion_tv := ref atomspace->TruthValue(
        conclusion_strength,
        conclusion_confidence
    );
    
    conclusion.atom.set_tv(conclusion_tv);
    
    sys->print("  Inferred: %s\n", conclusion.to_string());
    sys->print("\n");
    
    # Stimulate attention on conclusion
    sys->print("Allocating attention to conclusion...\n");
    conclusion.atom.stimulate(100);
    sys->print("  Attention allocated: %s\n", conclusion.atom.av.to_string());
    sys->print("\n");
    
    # Display statistics
    sys->print("=== AtomSpace Statistics ===\n");
    stats := atomspace->get_stats();
    sys->print("%s\n", stats);
    
    sys->print("\n=== Inference Complete ===\n");
}
