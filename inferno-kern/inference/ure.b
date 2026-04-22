implement URE;

# Unified Rule Engine (URE) Kernel Module
# Implements URE as a kernel-level service in Inferno OS
# Provides flexible rule-based inference over AtomSpace

include "sys.m";
    sys: Sys;
include "draw.m";
include "math.m";
    math: Math;
include "../atomspace/atomspace.m";
    atomspace: AtomSpace;
include "../pattern/pattern.m";
    pattern: PatternMatcher;

URE: module {
    PATH: con "/dis/cog/inference/ure.dis";
    
    init: fn();
    
    # Rule representation
    Rule: adt {
        name:       string;
        pattern:    ref Pattern;        # Pattern to match
        rewrite:    ref Pattern;        # Rewrite pattern
        precond:    ref Constraint;     # Preconditions
        tv_formula: ref TVFormula;      # Truth value computation
        weight:     real;               # Rule weight/priority
        
        matches:    fn(self: self ref Rule, atom: ref Atom): int;
        apply:      fn(self: self ref Rule, bindings: ref BindingSet): ref Atom;
    };
    
    # Rule set (collection of rules)
    RuleSet: adt {
        name:       string;
        rules:      list of ref Rule;
        
        new:        fn(name: string): ref RuleSet;
        add:        fn(self: self ref RuleSet, rule: ref Rule);
        remove:     fn(self: self ref RuleSet, name: string);
        get:        fn(self: self ref RuleSet, name: string): ref Rule;
    };
    
    # URE inference engine
    Engine: adt {
        atomspace:  ref Space;
        rulesets:   list of ref RuleSet;
        max_iter:   int;
        complexity: int;
        
        new:        fn(space: ref Space): ref Engine;
        add_ruleset: fn(self: self ref Engine, rs: ref RuleSet);
        
        # Forward chaining
        forward_chain:  fn(self: self ref Engine, sources: list of ref Atom, target: ref Atom): list of ref Atom;
        
        # Backward chaining
        backward_chain: fn(self: self ref Engine, target: ref Atom): list of ref Atom;
        
        # Mixed chaining
        mixed_chain:    fn(self: self ref Engine, sources: list of ref Atom, target: ref Atom): list of ref Atom;
    };
    
    # Inference trace (for debugging/explanation)
    InferenceTrace: adt {
        steps:      list of ref InferenceStep;
        
        add_step:   fn(self: self ref InferenceTrace, step: ref InferenceStep);
        print:      fn(self: self ref InferenceTrace);
    };
    
    InferenceStep: adt {
        rule:       ref Rule;
        premises:   list of ref Atom;
        conclusion: ref Atom;
        timestamp:  int;
    };
    
    # Truth value formula
    TVFormula: adt {
        compute:    fn(self: self ref TVFormula, tvs: list of ref TruthValue): ref TruthValue;
    };
};

init()
{
    sys = load Sys Sys->PATH;
    math = load Math Math->PATH;
    atomspace = load AtomSpace AtomSpace->PATH;
    atomspace->init();
    pattern = load PatternMatcher PatternMatcher->PATH;
    pattern->init();
}

# ========================================
# RuleSet Implementation
# ========================================

RuleSet.new(name: string): ref RuleSet
{
    rs := ref RuleSet;
    rs.name = name;
    rs.rules = nil;
    return rs;
}

RuleSet.add(self: self ref RuleSet, rule: ref Rule)
{
    self.rules = rule :: self.rules;
}

RuleSet.remove(self: self ref RuleSet, name: string)
{
    newrules: list of ref Rule = nil;
    for (rules := self.rules; rules != nil; rules = tl rules) {
        rule := hd rules;
        if (rule.name != name)
            newrules = rule :: newrules;
    }
    self.rules = newrules;
}

RuleSet.get(self: self ref RuleSet, name: string): ref Rule
{
    for (rules := self.rules; rules != nil; rules = tl rules) {
        rule := hd rules;
        if (rule.name == name)
            return rule;
    }
    return nil;
}

# ========================================
# Rule Implementation
# ========================================

Rule.matches(self: self ref Rule, atom: ref Atom): int
{
    # Check if atom matches rule pattern
    bindings := pattern->match(self.pattern, atom);
    if (bindings == nil)
        return 0;
    
    # Check preconditions
    if (self.precond != nil) {
        if (!self.precond.check(bindings))
            return 0;
    }
    
    return 1;
}

Rule.apply(self: self ref Rule, bindings: ref BindingSet): ref Atom
{
    # Instantiate rewrite pattern with bindings
    conclusion := pattern->instantiate(self.rewrite, bindings);
    
    # Compute truth value
    if (self.tv_formula != nil) {
        premise_tvs := extract_truth_values(bindings);
        conclusion.tv = self.tv_formula.compute(premise_tvs);
    }
    
    return conclusion;
}

# ========================================
# URE Engine Implementation
# ========================================

Engine.new(space: ref Space): ref Engine
{
    engine := ref Engine;
    engine.atomspace = space;
    engine.rulesets = nil;
    engine.max_iter = 1000;
    engine.complexity = 10;
    return engine;
}

Engine.add_ruleset(self: self ref Engine, rs: ref RuleSet)
{
    self.rulesets = rs :: self.rulesets;
}

# Forward chaining: Start from sources, apply rules, try to reach target
Engine.forward_chain(self: self ref Engine, sources: list of ref Atom, target: ref Atom): list of ref Atom
{
    results: list of ref Atom = nil;
    queue := sources;
    visited: list of ref Atom = nil;
    iterations := 0;
    
    trace := ref InferenceTrace;
    trace.steps = nil;
    
    while (queue != nil && iterations < self.max_iter) {
        current := hd queue;
        queue = tl queue;
        
        # Skip if already visited
        if (contains(visited, current)) {
            continue;
        }
        visited = current :: visited;
        
        # Check if we reached target
        if (target != nil && atoms_equal(current, target)) {
            results = current :: results;
            continue;
        }
        
        # Try to apply rules
        for (rulesets := self.rulesets; rulesets != nil; rulesets = tl rulesets) {
            ruleset := hd rulesets;
            
            for (rules := ruleset.rules; rules != nil; rules = tl rules) {
                rule := hd rules;
                
                # Check if rule matches current atom
                if (rule.matches(current)) {
                    # Get bindings
                    bindings := pattern->match(rule.pattern, current);
                    
                    # Apply rule
                    conclusion := rule.apply(bindings);
                    
                    if (conclusion != nil) {
                        # Add to atomspace
                        self.atomspace.add_atom(conclusion);
                        
                        # Add to queue for further processing
                        queue = conclusion :: queue;
                        
                        # Record inference step
                        step := ref InferenceStep;
                        step.rule = rule;
                        step.premises = list of {current};
                        step.conclusion = conclusion;
                        step.timestamp = sys->millisec();
                        trace.add_step(step);
                        
                        # Add to results
                        results = conclusion :: results;
                    }
                }
            }
        }
        
        iterations++;
    }
    
    sys->print("Forward chaining: %d iterations, %d results\n", iterations, length(results));
    
    return results;
}

# Backward chaining: Start from target, find rules that can prove it
Engine.backward_chain(self: self ref Engine, target: ref Atom): list of ref Atom
{
    results: list of ref Atom = nil;
    goals := list of {target};
    visited: list of ref Atom = nil;
    iterations := 0;
    
    trace := ref InferenceTrace;
    trace.steps = nil;
    
    while (goals != nil && iterations < self.max_iter) {
        current_goal := hd goals;
        goals = tl goals;
        
        # Skip if already visited
        if (contains(visited, current_goal)) {
            continue;
        }
        visited = current_goal :: visited;
        
        # Check if goal is already in atomspace
        if (self.atomspace.contains(current_goal)) {
            results = current_goal :: results;
            continue;
        }
        
        # Find rules that can prove this goal
        for (rulesets := self.rulesets; rulesets != nil; rulesets = tl rulesets) {
            ruleset := hd rulesets;
            
            for (rules := ruleset.rules; rules != nil; rules = tl rules) {
                rule := hd rules;
                
                # Check if rule conclusion matches goal
                if (pattern_matches_atom(rule.rewrite, current_goal)) {
                    # Find premises needed to apply rule
                    premise_atoms := find_premises_for_rule(rule, current_goal, self.atomspace);
                    
                    if (premise_atoms != nil) {
                        # All premises found, apply rule
                        bindings := create_bindings(rule.pattern, premise_atoms);
                        conclusion := rule.apply(bindings);
                        
                        # Add to atomspace
                        self.atomspace.add_atom(conclusion);
                        
                        # Record inference step
                        step := ref InferenceStep;
                        step.rule = rule;
                        step.premises = premise_atoms;
                        step.conclusion = conclusion;
                        step.timestamp = sys->millisec();
                        trace.add_step(step);
                        
                        results = conclusion :: results;
                    } else {
                        # Add premises as subgoals
                        subgoals := extract_premise_patterns(rule.pattern);
                        for (sg := subgoals; sg != nil; sg = tl sg) {
                            goals = hd sg :: goals;
                        }
                    }
                }
            }
        }
        
        iterations++;
    }
    
    sys->print("Backward chaining: %d iterations, %d results\n", iterations, length(results));
    
    return results;
}

# Mixed chaining: Combine forward and backward chaining
Engine.mixed_chain(self: self ref Engine, sources: list of ref Atom, target: ref Atom): list of ref Atom
{
    # Start with forward chaining from sources
    forward_results := self.forward_chain(sources, target);
    
    # If target not reached, try backward chaining
    if (!contains_atom(forward_results, target)) {
        backward_results := self.backward_chain(target);
        forward_results = append_atom_lists(forward_results, backward_results);
    }
    
    return forward_results;
}

# ========================================
# InferenceTrace Implementation
# ========================================

InferenceTrace.add_step(self: self ref InferenceTrace, step: ref InferenceStep)
{
    self.steps = step :: self.steps;
}

InferenceTrace.print(self: self ref InferenceTrace)
{
    sys->print("=== Inference Trace ===\n");
    step_num := 1;
    
    for (steps := self.steps; steps != nil; steps = tl steps) {
        step := hd steps;
        sys->print("Step %d: Rule '%s'\n", step_num, step.rule.name);
        sys->print("  Premises: ");
        print_atom_list(step.premises);
        sys->print("\n  Conclusion: ");
        print_atom(step.conclusion);
        sys->print("\n");
        step_num++;
    }
}

# ========================================
# Helper Functions
# ========================================

contains(list: list of ref Atom, atom: ref Atom): int
{
    for (l := list; l != nil; l = tl l) {
        if (atoms_equal(hd l, atom))
            return 1;
    }
    return 0;
}

atoms_equal(a1: ref Atom, a2: ref Atom): int
{
    if (a1 == nil || a2 == nil)
        return 0;
    return a1.id == a2.id;
}

contains_atom(list: list of ref Atom, atom: ref Atom): int
{
    return contains(list, atom);
}

append_atom_lists(l1: list of ref Atom, l2: list of ref Atom): list of ref Atom
{
    result := l1;
    for (l := l2; l != nil; l = tl l) {
        if (!contains(result, hd l))
            result = hd l :: result;
    }
    return result;
}

length(list: list of ref Atom): int
{
    count := 0;
    for (l := list; l != nil; l = tl l)
        count++;
    return count;
}

pattern_matches_atom(pat: ref Pattern, atom: ref Atom): int
{
    # Check if pattern matches atom structure
    # (Simplified - full implementation in pattern matcher)
    return 1;
}

find_premises_for_rule(rule: ref Rule, goal: ref Atom, space: ref Space): list of ref Atom
{
    # Find atoms in atomspace that satisfy rule premises
    # (Simplified - full implementation needed)
    return nil;
}

create_bindings(pat: ref Pattern, atoms: list of ref Atom): ref BindingSet
{
    # Create binding set from pattern and atoms
    # (Simplified - full implementation needed)
    return ref BindingSet;
}

extract_premise_patterns(pat: ref Pattern): list of ref Atom
{
    # Extract premise patterns from rule pattern
    # (Simplified - full implementation needed)
    return nil;
}

extract_truth_values(bindings: ref BindingSet): list of ref TruthValue
{
    # Extract truth values from bound atoms
    # (Simplified - full implementation needed)
    return nil;
}

print_atom_list(atoms: list of ref Atom)
{
    sys->print("[");
    for (a := atoms; a != nil; a = tl a) {
        print_atom(hd a);
        if (tl a != nil)
            sys->print(", ");
    }
    sys->print("]");
}

print_atom(atom: ref Atom)
{
    if (atom == nil) {
        sys->print("nil");
        return;
    }
    sys->print("(%s %s)", atom.type, atom.name);
}
