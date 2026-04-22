implement Perception;

#
# Perception Application for Inferno Kernel AGI
#
# Reads sensory input from /dev/perception, extracts features,
# and creates perceptual atoms in the kernel AtomSpace.
# Stimulates attention on new perceptual atoms.
#
# Copyright (C) 2026 OpenCog Community
# Licensed under AGPL-3.0
#

include "sys.m";
    sys: Sys;

include "draw.m";

Perception: module {
    init: fn(ctxt: ref Draw->Context, args: list of string);
};

include "atomspace_kern.m";
    atomspace: AtomSpaceKern;
    Atom, TruthValue, AttentionValue: import atomspace;

init(ctxt: ref Draw->Context, args: list of string)
{
    sys = load Sys Sys->PATH;
    atomspace = load AtomSpaceKern AtomSpaceKern->PATH;
    if(atomspace == nil) {
        sys->print("perception: cannot load AtomSpaceKern\n");
        return;
    }
    atomspace->init(ctxt, nil);

    sys->print("Perception application started\n");

    # Open perception device
    perception_fd := sys->open("/dev/perception", Sys->OREAD);
    if(perception_fd == nil) {
        sys->print("perception: /dev/perception unavailable, using simulated input\n");
        simulate_perception(ctxt);
        return;
    }

    # Main perception loop
    buf := array[4096] of byte;
    for(;;) {
        n := sys->read(perception_fd, buf, len buf);
        if(n <= 0)
            break;

        # Parse percept: "feature_name confidence"
        s := string buf[0:n];
        process_percept(s);
    }

    sys->close(perception_fd);
}

process_percept(s: string)
{
    # Extract feature name and confidence
    feature := s;
    confidence := 1.0;

    # Find space separator
    for(i := 0; i < len s; i++) {
        if(s[i] == ' ') {
            feature    = s[0:i];
            confidence = real s[i+1:];
            break;
        }
    }

    # Create atom for the perceived feature
    tv := atomspace->create_truth_value(confidence, 0.9);
    atom := atomspace->global_atomspace.add_node("ConceptNode",
        "percept_" + feature, tv);

    if(atom == nil) {
        sys->print("perception: failed to create atom for %s\n", feature);
        return;
    }

    # Stimulate attention proportional to confidence
    sti := int(confidence * 200.0);
    av := ref AttentionValue;
    av.sti = sti;
    av.lti = 0;
    av.vlti = 0;
    atom.set_av(av);

    sys->print("perception: created percept atom '%s' (conf=%.2f, sti=%d)\n",
        feature, confidence, sti);
}

simulate_perception(ctxt: ref Draw->Context)
{
    # Simulate a basic set of perceptual inputs for testing
    percepts := array[] of {
        "visual_edge 0.95",
        "visual_shape_circle 0.8",
        "audio_tone_440hz 0.7",
        "tactile_pressure 0.6",
    };

    for(i := 0; i < len percepts; i++) {
        process_percept(percepts[i]);
        sys->sleep(100);
    }

    sys->print("perception: simulation complete\n");
}
