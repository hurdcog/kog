implement ReasoningKernel;

#
# Reasoning Kernel Service for Inferno AGI OS
# Provides PLN (Probabilistic Logic Networks) and URE (Unified Rule Engine)
# as kernel-level services
#
# Copyright (C) 2025 OpenCog Foundation
# Licensed under AGPL-3.0
#

include "sys.m";
	sys: Sys;
	print, fprint, sprint: import sys;

include "draw.m";

include "math.m";
	math: Math;
	sqrt, log, exp: import math;

include "atomspace.m";
	atomspace: AtomSpaceKernel;
	Atom, Link, TruthValue, AtomSpace: import atomspace;

ReasoningKernel: module {
	PATH: con "/dis/cognitive/reasoning.dis";
	
	init: fn(nil: ref Draw->Context, args: list of string);
};

#
# Rule representation
#

Rule: adt {
	id: big;
	name: string;
	rtype: string;        # "deduction", "induction", "abduction", etc.
	premises: array of ref Pattern;
	conclusion: ref Pattern;
	tv: ref TruthValue;   # Rule's own truth value
	weight: real;         # Rule weight/priority
	
	# Methods
	text: fn(r: self ref Rule): string;
	apply: fn(r: self ref Rule, bindings: ref Bindings): ref Atom;
	matches: fn(r: self ref Rule, atoms: array of ref Atom): int;
};

Pattern: adt {
	ptype: string;        # Pattern type
	variables: list of string; # Variables in pattern
	template: ref Atom;   # Template atom
	constraints: list of ref Constraint; # Pattern constraints
	
	# Methods
	match: fn(p: self ref Pattern, atom: ref Atom, bindings: ref Bindings): int;
	instantiate: fn(p: self ref Pattern, bindings: ref Bindings): ref Atom;
};

Bindings: adt {
	vars: array of (string, ref Atom);
	count: int;
	
	# Methods
	create: fn(): ref Bindings;
	bind: fn(b: self ref Bindings, variable: string, atom: ref Atom);
	lookup: fn(b: self ref Bindings, variable: string): ref Atom;
	text: fn(b: self ref Bindings): string;
};

Constraint: adt {
	ctype: string;        # "type", "name", "tv", etc.
	value: string;
	
	check: fn(c: self ref Constraint, atom: ref Atom): int;
};

#
# Inference engines
#

ForwardChainer: adt {
	rules: list of ref Rule;
	maxiter: int;
	
	# Methods
	create: fn(rules: list of ref Rule, maxiter: int): ref ForwardChainer;
	infer: fn(fc: self ref ForwardChainer, premises: list of ref Atom): list of ref Atom;
};

BackwardChainer: adt {
	rules: list of ref Rule;
	maxdepth: int;
	
	# Methods
	create: fn(rules: list of ref Rule, maxdepth: int): ref BackwardChainer;
	prove: fn(bc: self ref BackwardChainer, goal: ref Atom): list of ref Atom;
};

#
# PLN-specific inference rules
#

PLN: module {
	# Deduction: (A->B), (B->C) |- (A->C)
	deduction: fn(ab: ref Link, bc: ref Link): ref Link;
	
	# Induction: (A->B), (A->C) |- (B->C)
	induction: fn(ab: ref Link, ac: ref Link): ref Link;
	
	# Abduction: (A->B), (C->B) |- (A->C)
	abduction: fn(ab: ref Link, cb: ref Link): ref Link;
	
	# Modus Ponens: (A->B), A |- B
	modus_ponens: fn(ab: ref Link, a: ref Atom): ref Atom;
	
	# And introduction: A, B |- (A AND B)
	and_intro: fn(a: ref Atom, b: ref Atom): ref Link;
	
	# Or introduction: A |- (A OR B)
	or_intro: fn(a: ref Atom, b: ref Atom): ref Link;
	
	# Truth value revision
	revision: fn(tv1: ref TruthValue, tv2: ref TruthValue): ref TruthValue;
};

pln: PLN;

#
# Global rule base
#

rulebase: list of ref Rule;
nextrid: big = 1;

#
# Initialization
#

init(nil: ref Draw->Context, args: list of string)
{
	sys = load Sys Sys->PATH;
	math = load Math Math->PATH;
	atomspace = load AtomSpaceKernel AtomSpaceKernel->PATH;
	
	if (math == nil || atomspace == nil) {
		sys->fprint(sys->fildes(2), "reasoning: cannot load required modules\n");
		raise "fail:load";
	}
	
	# Initialize PLN module
	pln = load PLN PLN->PATH;
	if (pln == nil)
		pln = self;  # Use built-in PLN
	
	# Load default inference rules
	init_default_rules();
	
	sys->print("Reasoning Kernel Service initialized\n");
	sys->print("Loaded %d inference rules\n", len rulebase);
}

#
# Initialize default PLN rules
#

init_default_rules()
{
	# Deduction rule
	r := ref Rule;
	r.id = nextrid++;
	r.name = "DeductionRule";
	r.rtype = "deduction";
	r.tv = ref TruthValue(1.0, 1.0);
	r.weight = 1.0;
	rulebase = r :: rulebase;
	
	# Induction rule
	r = ref Rule;
	r.id = nextrid++;
	r.name = "InductionRule";
	r.rtype = "induction";
	r.tv = ref TruthValue(0.9, 0.9);
	r.weight = 0.8;
	rulebase = r :: rulebase;
	
	# Abduction rule
	r = ref Rule;
	r.id = nextrid++;
	r.name = "AbductionRule";
	r.rtype = "abduction";
	r.tv = ref TruthValue(0.8, 0.8);
	r.weight = 0.7;
	rulebase = r :: rulebase;
	
	# Modus Ponens
	r = ref Rule;
	r.id = nextrid++;
	r.name = "ModusPonensRule";
	r.rtype = "modus_ponens";
	r.tv = ref TruthValue(1.0, 1.0);
	r.weight = 1.0;
	rulebase = r :: rulebase;
}

#
# PLN inference rules implementation
#

PLN.deduction(ab: ref Link, bc: ref Link): ref Link
{
	# Extract A, B, C from (A->B) and (B->C)
	if (ab == nil || bc == nil)
		return nil;
	if (len ab.outgoing < 2 || len bc.outgoing < 2)
		return nil;
	
	a := ab.outgoing[0];
	b1 := ab.outgoing[1];
	b2 := bc.outgoing[0];
	c := bc.outgoing[1];
	
	# Check that B matches
	if (!b1.equals(b2))
		return nil;
	
	# Calculate truth value for (A->C)
	# sAC = sAB * sBC
	# cAC = cAB * cBC * sBC
	sAB := ab.atom.tv.strength;
	cAB := ab.atom.tv.confidence;
	sBC := bc.atom.tv.strength;
	cBC := bc.atom.tv.confidence;
	
	sAC := sAB * sBC;
	cAC := cAB * cBC * sBC;
	
	# Create new implication link (A->C)
	# This would normally use atomspace.add_link()
	# For now, return a placeholder
	return nil;
}

PLN.induction(ab: ref Link, ac: ref Link): ref Link
{
	# Induction: (A->B), (A->C) |- (B->C)
	# This is the inverse of deduction
	
	if (ab == nil || ac == nil)
		return nil;
	if (len ab.outgoing < 2 || len ac.outgoing < 2)
		return nil;
	
	a1 := ab.outgoing[0];
	b := ab.outgoing[1];
	a2 := ac.outgoing[0];
	c := ac.outgoing[1];
	
	# Check that A matches
	if (!a1.equals(a2))
		return nil;
	
	# Calculate truth value for (B->C)
	# Induction is weaker than deduction
	sAB := ab.atom.tv.strength;
	cAB := ab.atom.tv.confidence;
	sAC := ac.atom.tv.strength;
	cAC := ac.atom.tv.confidence;
	
	sBC := sAB * sAC;
	cBC := cAB * cAC * 0.8;  # Induction discount factor
	
	return nil;
}

PLN.abduction(ab: ref Link, cb: ref Link): ref Link
{
	# Abduction: (A->B), (C->B) |- (A->C)
	# Weakest form of inference
	
	if (ab == nil || cb == nil)
		return nil;
	if (len ab.outgoing < 2 || len cb.outgoing < 2)
		return nil;
	
	a := ab.outgoing[0];
	b1 := ab.outgoing[1];
	c := cb.outgoing[0];
	b2 := cb.outgoing[1];
	
	# Check that B matches
	if (!b1.equals(b2))
		return nil;
	
	# Calculate truth value for (A->C)
	sAB := ab.atom.tv.strength;
	cAB := ab.atom.tv.confidence;
	sCB := cb.atom.tv.strength;
	cCB := cb.atom.tv.confidence;
	
	sAC := sAB * sCB;
	cAC := cAB * cCB * 0.6;  # Abduction discount factor
	
	return nil;
}

PLN.modus_ponens(ab: ref Link, a: ref Atom): ref Atom
{
	# Modus Ponens: (A->B), A |- B
	
	if (ab == nil || a == nil)
		return nil;
	if (len ab.outgoing < 2)
		return nil;
	
	a1 := ab.outgoing[0];
	b := ab.outgoing[1];
	
	# Check that A matches
	if (!a1.equals(a))
		return nil;
	
	# Calculate truth value for B
	# sB = sA * sAB
	# cB = cA * cAB
	sA := a.tv.strength;
	cA := a.tv.confidence;
	sAB := ab.atom.tv.strength;
	cAB := ab.atom.tv.confidence;
	
	sB := sA * sAB;
	cB := cA * cAB;
	
	# Update B's truth value
	b.tv = ref TruthValue(sB, cB);
	
	return b;
}

PLN.and_intro(a: ref Atom, b: ref Atom): ref Link
{
	# And introduction: A, B |- (A AND B)
	
	if (a == nil || b == nil)
		return nil;
	
	# Calculate truth value for (A AND B)
	# s(A AND B) = sA * sB
	# c(A AND B) = cA * cB
	sA := a.tv.strength;
	cA := a.tv.confidence;
	sB := b.tv.strength;
	cB := b.tv.confidence;
	
	sAnd := sA * sB;
	cAnd := cA * cB;
	
	return nil;
}

PLN.or_intro(a: ref Atom, b: ref Atom): ref Link
{
	# Or introduction: A |- (A OR B)
	
	if (a == nil || b == nil)
		return nil;
	
	# Calculate truth value for (A OR B)
	# s(A OR B) = sA + sB - sA*sB
	# c(A OR B) = min(cA, cB)
	sA := a.tv.strength;
	cA := a.tv.confidence;
	sB := b.tv.strength;
	cB := b.tv.confidence;
	
	sOr := sA + sB - sA * sB;
	cOr := cA;
	if (cB < cOr)
		cOr = cB;
	
	return nil;
}

PLN.revision(tv1: ref TruthValue, tv2: ref TruthValue): ref TruthValue
{
	# Truth value revision: combine two truth values
	# Uses Bayes' rule with independence assumption
	
	if (tv1 == nil || tv2 == nil)
		return nil;
	
	s1 := tv1.strength;
	c1 := tv1.confidence;
	s2 := tv2.strength;
	c2 := tv2.confidence;
	
	# Weighted average based on confidence
	w1 := c1 / (c1 + c2);
	w2 := c2 / (c1 + c2);
	
	s := w1 * s1 + w2 * s2;
	c := c1 + c2 - c1 * c2;  # Combine confidences
	
	return ref TruthValue(s, c);
}

#
# Forward chainer implementation
#

ForwardChainer.create(rules: list of ref Rule, maxiter: int): ref ForwardChainer
{
	fc := ref ForwardChainer;
	fc.rules = rules;
	fc.maxiter = maxiter;
	return fc;
}

ForwardChainer.infer(fc: self ref ForwardChainer, premises: list of ref Atom): list of ref Atom
{
	# Forward chaining: apply rules to premises to derive conclusions
	
	conclusions: list of ref Atom = nil;
	workingset := premises;
	
	for (iter := 0; iter < fc.maxiter; iter++) {
		newatoms: list of ref Atom = nil;
		
		# Try to apply each rule
		for (rules := fc.rules; rules != nil; rules = tl rules) {
			rule := hd rules;
			
			# Try to match rule premises with working set
			# This is simplified - real implementation would do
			# proper pattern matching and unification
			
			# For now, just add rule to demonstrate structure
		}
		
		if (newatoms == nil)
			break;  # No new inferences
		
		# Add new atoms to working set and conclusions
		for (; newatoms != nil; newatoms = tl newatoms) {
			atom := hd newatoms;
			workingset = atom :: workingset;
			conclusions = atom :: conclusions;
		}
	}
	
	return conclusions;
}

#
# Backward chainer implementation
#

BackwardChainer.create(rules: list of ref Rule, maxdepth: int): ref BackwardChainer
{
	bc := ref BackwardChainer;
	bc.rules = rules;
	bc.maxdepth = maxdepth;
	return bc;
}

BackwardChainer.prove(bc: self ref BackwardChainer, goal: ref Atom): list of ref Atom
{
	# Backward chaining: try to prove goal by finding rules
	# whose conclusion matches goal, then recursively prove premises
	
	proofs: list of ref Atom = nil;
	
	# Try each rule
	for (rules := bc.rules; rules != nil; rules = tl rules) {
		rule := hd rules;
		
		# Check if rule conclusion matches goal
		# This is simplified - real implementation would do
		# proper pattern matching and unification
		
		# If match, recursively prove premises
		# If all premises proven, add to proofs
	}
	
	return proofs;
}

#
# Bindings implementation
#

Bindings.create(): ref Bindings
{
	b := ref Bindings;
	b.vars = array[100] of (string, ref Atom);
	b.count = 0;
	return b;
}

Bindings.bind(b: self ref Bindings, variable: string, atom: ref Atom)
{
	if (b.count >= len b.vars)
		return;
	
	b.vars[b.count++] = (variable, atom);
}

Bindings.lookup(b: self ref Bindings, variable: string): ref Atom
{
	for (i := 0; i < b.count; i++) {
		(v, a) := b.vars[i];
		if (v == variable)
			return a;
	}
	return nil;
}

Bindings.text(b: self ref Bindings): string
{
	s := "{";
	for (i := 0; i < b.count; i++) {
		(v, a) := b.vars[i];
		if (i > 0)
			s += ", ";
		s += sprint("%s=%s", v, a.text());
	}
	s += "}";
	return s;
}

#
# Rule methods
#

Rule.text(r: self ref Rule): string
{
	return sprint("Rule(%s: %s)", r.name, r.rtype);
}
