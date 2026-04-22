implement HelloCognition;

#
# Hello Cognition - First Cognitive Application in OpenCog Inferno
#
# This example demonstrates basic cognitive operations using the
# AtomSpace kernel module through Limbo bindings.
#

include "sys.m";
    sys: Sys;
    print: import sys;

include "draw.m";

include "atomspace.m";
    atomspace: AtomSpace;
    Space, Atom, TruthValue, AttentionValue: import atomspace;

HelloCognition: module
{
    init: fn(ctxt: ref Draw->Context, args: list of string);
};

init(ctxt: ref Draw->Context, args: list of string)
{
    sys = load Sys Sys->PATH;
    atomspace = load AtomSpace AtomSpace->PATH;
    
    if (atomspace == nil) {
        print("Failed to load AtomSpace module\n");
        raise "fail:load";
    }
    
    atomspace->init();
    
    print("=== Hello Cognition ==>\n");
    print("OpenCog Inferno - Cognitive Operating System\n\n");
    
    # Open the AtomSpace
    space := Space.open();
    if (space == nil) {
        print("Failed to open AtomSpace\n");
        raise "fail:open";
    }
    
    print("Step 1: Creating cognitive concepts\n");
    
    # Create concept nodes
    self := space.concept("self");
    agent := space.concept("agent");
    intelligence := space.concept("intelligence");
    cognition := space.concept("cognition");
    
    print("  Created: ConceptNode \"self\" [%d]\n", self.id);
    print("  Created: ConceptNode \"agent\" [%d]\n", agent.id);
    print("  Created: ConceptNode \"intelligence\" [%d]\n", intelligence.id);
    print("  Created: ConceptNode \"cognition\" [%d]\n", cognition.id);
    print("\n");
    
    print("Step 2: Establishing relationships\n");
    
    # Create inheritance links
    link1 := space.inheritance(self, agent);
    link2 := space.inheritance(agent, intelligence);
    link3 := space.inheritance(intelligence, cognition);
    
    print("  Created: InheritanceLink [%d]\n", link1.id);
    print("    ConceptNode \"self\" -> ConceptNode \"agent\"\n");
    print("  Created: InheritanceLink [%d]\n", link2.id);
    print("    ConceptNode \"agent\" -> ConceptNode \"intelligence\"\n");
    print("  Created: InheritanceLink [%d]\n", link3.id);
    print("    ConceptNode \"intelligence\" -> ConceptNode \"cognition\"\n");
    print("\n");
    
    print("Step 3: Assigning truth values\n");
    
    # Set truth values (strength, confidence)
    tv1 := TruthValue.mk(0.95, 0.90);
    tv2 := TruthValue.mk(0.85, 0.80);
    tv3 := TruthValue.mk(0.90, 0.85);
    
    link1.settv(tv1);
    link2.settv(tv2);
    link3.settv(tv3);
    
    print("  Set TV for link1: %s\n", tv1.text());
    print("  Set TV for link2: %s\n", tv2.text());
    print("  Set TV for link3: %s\n", tv3.text());
    print("\n");
    
    print("Step 4: Allocating attention\n");
    
    # Set attention values
    av1 := AttentionValue.mk(100, 50, 10);
    av2 := AttentionValue.mk(80, 40, 5);
    av3 := AttentionValue.mk(60, 30, 0);
    
    self.setav(av1);
    agent.setav(av2);
    intelligence.setav(av3);
    
    print("  Set AV for self: %s\n", av1.text());
    print("  Set AV for agent: %s\n", av2.text());
    print("  Set AV for intelligence: %s\n", av3.text());
    print("\n");
    
    print("Step 5: Querying the AtomSpace\n");
    
    # Query by type
    concepts := space.querytype(atomspace->CONCEPT);
    print("  Found %d concept nodes:\n", len concepts);
    for (i := 0; i < len concepts; i++) {
        print("    [%d] %s\n", concepts[i].id, concepts[i].name());
    }
    print("\n");
    
    # Query by name
    results := space.queryname("self");
    if (len results > 0) {
        print("  Query for 'self' returned:\n");
        for (i := 0; i < len results; i++) {
            atom := results[i];
            tv := atom.gettv();
            av := atom.getav();
            print("    %s\n", atom.text());
            print("      TV: %s\n", tv.text());
            print("      AV: %s\n", av.text());
        }
    }
    print("\n");
    
    print("Step 6: Cognitive reasoning (simulated)\n");
    
    # In a full implementation, this would invoke PLN
    print("  Inferring: self -> agent -> intelligence -> cognition\n");
    print("  Conclusion: self is a form of cognition\n");
    print("  Confidence: 0.73 (computed from truth value propagation)\n");
    print("\n");
    
    print("Step 7: AtomSpace statistics\n");
    
    (total, nodes, links) := space.stats();
    print("  Total atoms: %d\n", total);
    print("  Nodes: %d\n", nodes);
    print("  Links: %d\n", links);
    print("\n");
    
    # Close the AtomSpace
    space.close();
    
    print("=== Cognition Complete ===\n");
    print("\nThis demonstrates:\n");
    print("  1. Cognitive concepts as kernel-level data structures\n");
    print("  2. Relationships with probabilistic truth values\n");
    print("  3. Attention allocation for cognitive focus\n");
    print("  4. Query operations on the hypergraph\n");
    print("  5. Native OS-level cognitive processing\n");
    print("\nWelcome to the future of AGI!\n");
}
