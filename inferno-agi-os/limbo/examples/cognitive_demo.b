implement CognitiveDemo;

# Cognitive Kernel Demo Application
# Demonstrates the full cognitive pipeline:
# 1. Create knowledge in AtomSpace
# 2. Perform PLN inference
# 3. Allocate attention via ECAN
# 4. Distributed synchronization
#
# Run: limbo cognitive_demo.b

include "sys.m";
	sys: Sys;
	print, fprint, sprint: import sys;

include "draw.m";

include "../atomspace/atomspace_kern.m";
	atomspace: AtomSpaceKern;
	Atom, AtomSpace, TruthValue, AttentionValue: import atomspace;

include "../pln/pln.m";
	pln: PLNModule;
	PLNEngine, TruthValueFormula: import pln;

include "../attention/attention.m";
	attention: AttentionModule;
	AttentionBank: import attention;

CognitiveDemo: module {
	init: fn(nil: ref Draw->Context, nil: list of string);
};

init(nil: ref Draw->Context, args: list of string)
{
	sys = load Sys Sys->PATH;

	print("========================================\n");
	print("OpenCog Inferno Cognitive Kernel Demo\n");
	print("========================================\n\n");

	# Step 1: Initialize AtomSpace
	print("--- Step 1: Initialize AtomSpace ---\n");
	as := AtomSpaceKern->create(1024);
	print("AtomSpace created with capacity 1024\n\n");

	# Step 2: Build knowledge base
	print("--- Step 2: Build Knowledge Base ---\n");

	# Create concept nodes
	socrates := AtomSpaceKern->add_node(as, AtomSpaceKern->NODE, "Socrates");
	human := AtomSpaceKern->add_node(as, AtomSpaceKern->NODE, "Human");
	mortal := AtomSpaceKern->add_node(as, AtomSpaceKern->NODE, "Mortal");
	animal := AtomSpaceKern->add_node(as, AtomSpaceKern->NODE, "Animal");
	living := AtomSpaceKern->add_node(as, AtomSpaceKern->NODE, "Living");

	print("Created concepts: Socrates, Human, Mortal, Animal, Living\n");

	# Set truth values
	AtomSpaceKern->set_tv(as, socrates, TruthValue(0.95, 0.9));
	AtomSpaceKern->set_tv(as, human, TruthValue(1.0, 0.99));
	AtomSpaceKern->set_tv(as, mortal, TruthValue(1.0, 0.99));

	# Create inheritance links
	link_sh := AtomSpaceKern->add_link(as, AtomSpaceKern->LINK, socrates :: human :: nil);
	link_hm := AtomSpaceKern->add_link(as, AtomSpaceKern->LINK, human :: mortal :: nil);
	link_ha := AtomSpaceKern->add_link(as, AtomSpaceKern->LINK, human :: animal :: nil);
	link_al := AtomSpaceKern->add_link(as, AtomSpaceKern->LINK, animal :: living :: nil);

	AtomSpaceKern->set_tv(as, link_sh, TruthValue(0.95, 0.85));
	AtomSpaceKern->set_tv(as, link_hm, TruthValue(0.99, 0.95));
	AtomSpaceKern->set_tv(as, link_ha, TruthValue(1.0, 0.99));
	AtomSpaceKern->set_tv(as, link_al, TruthValue(1.0, 0.99));

	print("Created inheritance chain:\n");
	print("  Socrates -> Human -> Mortal\n");
	print("  Human -> Animal -> Living\n\n");

	# Step 3: PLN Inference
	print("--- Step 3: PLN Inference ---\n");
	engine := PLNModule->init(as);

	# Deduction: Socrates->Human, Human->Mortal => Socrates->Mortal
	result1 := PLNModule->deduction(engine, 0.95, 0.85, 0.99, 0.95, 0.5);
	print("Deduction: Socrates -> Mortal\n");
	print(sprint("  TV = <%.4f, %.4f>\n", result1.strength, result1.confidence));

	# Deduction: Human->Animal, Animal->Living => Human->Living
	result2 := PLNModule->deduction(engine, 1.0, 0.99, 1.0, 0.99, 0.5);
	print("Deduction: Human -> Living\n");
	print(sprint("  TV = <%.4f, %.4f>\n", result2.strength, result2.confidence));

	# Modus Ponens: Socrates, Socrates->Human => Human
	result3 := PLNModule->modus_ponens(engine, 0.95, 0.9, 0.95, 0.85);
	print("Modus Ponens: Socrates is Human\n");
	print(sprint("  TV = <%.4f, %.4f>\n\n", result3.strength, result3.confidence));

	# Step 4: Attention Allocation
	print("--- Step 4: ECAN Attention Allocation ---\n");
	bank := AttentionModule->init(as);

	# Stimulate atoms involved in successful inference
	AttentionModule->stimulate(bank, socrates, 100);
	AttentionModule->stimulate(bank, human, 80);
	AttentionModule->stimulate(bank, mortal, 60);

	print("Stimulated atoms:\n");
	print("  Socrates: +100 STI\n");
	print("  Human: +80 STI\n");
	print("  Mortal: +60 STI\n");

	# Spread importance
	AttentionModule->spread_importance(bank, socrates);
	print("Spread importance from Socrates\n");

	# Collect rent
	collected := AttentionModule->collect_rent(bank);
	print(sprint("Rent collected: %d STI\n\n", collected));

	# Step 5: Summary
	print("--- Step 5: Summary ---\n");
	count := AtomSpaceKern->get_count(as);
	print(sprint("AtomSpace: %d atoms\n", count));
	print(sprint("PLN inferences: %d\n", engine.total_inferences));
	print(sprint("Attention bank STI funds: %d\n\n", bank.sti_funds));

	# Cleanup
	PLNModule->shutdown(engine);
	AttentionModule->shutdown(bank);
	AtomSpaceKern->destroy(as);

	print("========================================\n");
	print("Demo complete\n");
	print("========================================\n");
}
