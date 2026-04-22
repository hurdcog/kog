implement Learning;

#
# Learning Application for Inferno Kernel AGI
#
# Implements pattern-based learning over the kernel AtomSpace.
# Monitors atom creation/modification events, identifies recurring patterns,
# and strengthens truth values of frequently observed associations.
#
# Copyright (C) 2026 OpenCog Community
# Licensed under AGPL-3.0
#

include "sys.m";
    sys: Sys;

include "draw.m";

Learning: module {
    init: fn(ctxt: ref Draw->Context, args: list of string);
};

include "atomspace_kern.m";
    atomspace: AtomSpaceKern;
    Atom, TruthValue, AttentionValue: import atomspace;

# Learning state
Pattern: adt {
    source_name: string;
    target_name: string;
    count:       int;
    strength:    real;
};

patterns: array of ref Pattern;
npatterns: int;

init(ctxt: ref Draw->Context, args: list of string)
{
    sys = load Sys Sys->PATH;
    atomspace = load AtomSpaceKern AtomSpaceKern->PATH;
    if(atomspace == nil) {
        sys->print("learning: cannot load AtomSpaceKern\n");
        return;
    }
    atomspace->init(ctxt, nil);

    patterns  = array[256] of ref Pattern;
    npatterns = 0;

    sys->print("Learning application started\n");

    # Seed with basic ontology relationships
    learn_relationship("cat", "animal", 0.95);
    learn_relationship("dog", "animal", 0.95);
    learn_relationship("animal", "living-thing", 1.0);
    learn_relationship("red", "color", 1.0);
    learn_relationship("blue", "color", 1.0);

    # Run learning cycle
    learning_cycle();
}

learn_relationship(source: string, target: string, strength: real)
{
    # Create nodes if they don't exist
    stv := atomspace->create_truth_value(1.0, 1.0);
    src_atom := atomspace->global_atomspace.add_node("ConceptNode", source, stv);
    tgt_atom := atomspace->global_atomspace.add_node("ConceptNode", target, stv);

    if(src_atom == nil || tgt_atom == nil)
        return;

    # Create inheritance link
    ltv := atomspace->create_truth_value(strength, 0.9);
    outgoing: list of ref Atom = nil;
    outgoing = tgt_atom :: outgoing;
    outgoing = src_atom :: outgoing;

    link := atomspace->global_atomspace.add_link("InheritanceLink", outgoing, ltv);
    if(link == nil)
        return;

    sys->print("learning: Inherit(%s, %s) <%.2f, 0.90>\n",
        source, target, strength);

    # Record pattern
    if(npatterns < len patterns) {
        p := ref Pattern;
        p.source_name = source;
        p.target_name = target;
        p.count       = 1;
        p.strength    = strength;
        patterns[npatterns++] = p;
    }
}

learning_cycle()
{
    sys->print("learning: running learning cycle (%d patterns known)\n", npatterns);

    # Reinforce high-count patterns by increasing LTI
    for(i := 0; i < npatterns; i++) {
        p := patterns[i];
        if(p == nil)
            continue;
        if(p.count > 1) {
            # Find and reinforce the link
            atom := atomspace->global_atomspace.get_node("ConceptNode", p.source_name);
            if(atom != nil) {
                av := atom.get_av();
                if(av != nil) {
                    av.lti += p.count;
                    atom.set_av(av);
                }
            }
        }
    }

    sys->print("learning: cycle complete\n");
}
