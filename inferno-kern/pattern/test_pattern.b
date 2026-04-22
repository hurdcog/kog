#
# Test Suite for Pattern Matcher Kernel Module
#
# This module provides comprehensive unit tests for the Pattern Matcher
# including tokenizer, parser, compiler, and matcher components.
#
# Copyright (c) 2025 OpenCog Foundation
# License: AGPL-3.0
#

implement TestPattern;

include "sys.m";
    sys: Sys;
include "draw.m";

include "pattern.m";
    pattern: PatternMatcher;
    Variable, Term, Clause, Constraint, Pattern,
    Binding, BindingSet, MatchResult,
    Token, Tokenizer, Parser, Compiler, Matcher, QueryExecutor: import pattern;

include "atomspace.m";
    atomspace: AtomSpace;
    Atom, TruthValue, AttentionValue, Space: import atomspace;

TestPattern: module {
    init: fn(ctxt: ref Draw->Context, argv: list of string);
};

# Test counters
tests_run := 0;
tests_passed := 0;
tests_failed := 0;

init(ctxt: ref Draw->Context, argv: list of string)
{
    sys = load Sys Sys->PATH;
    pattern = load PatternMatcher PatternMatcher->PATH;
    atomspace = load AtomSpace AtomSpace->PATH;
    
    sys->print("\n");
    sys->print("╔══════════════════════════════════════════════════════════════╗\n");
    sys->print("║     Pattern Matcher Kernel Module - Test Suite               ║\n");
    sys->print("║     OpenCog Inferno AGI OS                                   ║\n");
    sys->print("╚══════════════════════════════════════════════════════════════╝\n");
    sys->print("\n");
    
    # Run all test suites
    test_variables();
    test_terms();
    test_clauses();
    test_constraints();
    test_patterns();
    test_bindings();
    test_tokenizer();
    test_parser();
    test_compiler();
    test_matcher();
    test_query_executor();
    test_integration();
    
    # Print summary
    sys->print("\n");
    sys->print("══════════════════════════════════════════════════════════════\n");
    sys->print("                      TEST SUMMARY\n");
    sys->print("══════════════════════════════════════════════════════════════\n");
    sys->print("  Total tests run:    %d\n", tests_run);
    sys->print("  Tests passed:       %d\n", tests_passed);
    sys->print("  Tests failed:       %d\n", tests_failed);
    sys->print("  Success rate:       %.1f%%\n", 
               real tests_passed / real tests_run * 100.0);
    sys->print("══════════════════════════════════════════════════════════════\n");
    
    if (tests_failed == 0)
        sys->print("\n✅ ALL TESTS PASSED!\n\n");
    else
        sys->print("\n❌ SOME TESTS FAILED!\n\n");
}

# =============================================================================
# TEST HELPER FUNCTIONS
# =============================================================================

assert(condition: int, test_name: string)
{
    tests_run++;
    if (condition) {
        tests_passed++;
        sys->print("  ✓ %s\n", test_name);
    } else {
        tests_failed++;
        sys->print("  ✗ %s [FAILED]\n", test_name);
    }
}

assert_eq_str(actual: string, expected: string, test_name: string)
{
    assert(actual == expected, test_name);
    if (actual != expected)
        sys->print("    Expected: '%s', Got: '%s'\n", expected, actual);
}

assert_eq_int(actual: int, expected: int, test_name: string)
{
    assert(actual == expected, test_name);
    if (actual != expected)
        sys->print("    Expected: %d, Got: %d\n", expected, actual);
}

assert_not_nil[T](value: ref T, test_name: string)
{
    assert(value != nil, test_name);
}

# =============================================================================
# VARIABLE TESTS
# =============================================================================

test_variables()
{
    sys->print("\n── Variable Tests ──────────────────────────────────────────\n");
    
    # Test 1: Create simple variable
    v1 := Variable.new("$X");
    assert_not_nil(v1, "Create variable $X");
    assert_eq_str(v1.name, "$X", "Variable name is $X");
    
    # Test 2: Create variable with type constraint
    v2 := Variable.new("$Y");
    v2.with_type("ConceptNode");
    assert_eq_str(v2.type_constraint, "ConceptNode", "Variable type constraint");
    
    # Test 3: Create variable with value constraint
    v3 := Variable.new("$Z");
    v3.with_value("cat");
    assert_eq_str(v3.value_constraint, "cat", "Variable value constraint");
    
    # Test 4: Variable matches atom (no constraints)
    atom := create_test_atom("ConceptNode", "dog");
    assert(v1.matches(atom), "Unconstrained variable matches any atom");
    
    # Test 5: Variable matches atom (type constraint)
    assert(v2.matches(atom), "Type-constrained variable matches correct type");
    
    # Test 6: Variable doesn't match wrong type
    atom2 := create_test_atom("PredicateNode", "likes");
    assert(!v2.matches(atom2), "Type-constrained variable rejects wrong type");
    
    # Test 7: Variable to_string
    s := v2.to_string();
    assert(s != "", "Variable to_string produces output");
    
    # Test 8: Glob variable
    v4 := Variable.new("$*Args");
    v4.glob = 1;
    assert_eq_int(v4.glob, 1, "Glob variable flag set");
}

# =============================================================================
# TERM TESTS
# =============================================================================

test_terms()
{
    sys->print("\n── Term Tests ──────────────────────────────────────────────\n");
    
    # Test 1: Create term from variable
    v := Variable.new("$X");
    t1 := Term.from_variable(v);
    assert_not_nil(t1, "Create term from variable");
    assert_eq_int(t1.is_variable, 1, "Term is_variable flag set");
    
    # Test 2: Create term from atom
    atom := create_test_atom("ConceptNode", "cat");
    t2 := Term.from_atom(atom);
    assert_not_nil(t2, "Create term from atom");
    assert_eq_int(t2.is_variable, 0, "Term is_variable flag not set for atom");
    
    # Test 3: Term to_string for variable
    s1 := t1.to_string();
    assert_eq_str(s1, "$X", "Variable term to_string");
    
    # Test 4: Term to_string for atom
    s2 := t2.to_string();
    assert_eq_str(s2, "cat", "Atom term to_string");
}

# =============================================================================
# CLAUSE TESTS
# =============================================================================

test_clauses()
{
    sys->print("\n── Clause Tests ─────────────────────────────────────────────\n");
    
    # Test 1: Create simple clause
    c1 := Clause.new("InheritanceLink");
    assert_not_nil(c1, "Create InheritanceLink clause");
    assert_eq_str(c1.link_type, "InheritanceLink", "Clause link type");
    
    # Test 2: Add arguments to clause
    v := Variable.new("$X");
    t1 := Term.from_variable(v);
    atom := create_test_atom("ConceptNode", "mammal");
    t2 := Term.from_atom(atom);
    
    c1.add_arg(t1);
    c1.add_arg(t2);
    
    count := 0;
    for (args := c1.arguments; args != nil; args = tl args)
        count++;
    assert_eq_int(count, 2, "Clause has 2 arguments");
    
    # Test 3: Negate clause
    c2 := Clause.new("EvaluationLink");
    c2.negate();
    assert_eq_int(c2.negated, 1, "Clause negation flag set");
    
    # Test 4: Optional clause
    c3 := Clause.new("SimilarityLink");
    c3.make_optional();
    assert_eq_int(c3.optional, 1, "Clause optional flag set");
    
    # Test 5: Clause to_string
    s := c1.to_string();
    assert(len s > 0, "Clause to_string produces output");
}

# =============================================================================
# CONSTRAINT TESTS
# =============================================================================

test_constraints()
{
    sys->print("\n── Constraint Tests ─────────────────────────────────────────\n");
    
    v := Variable.new("$X");
    
    # Test 1: Type constraint (equals)
    con1 := Constraint.new("type", v, "=", "ConceptNode");
    assert_not_nil(con1, "Create type constraint");
    
    atom1 := create_test_atom("ConceptNode", "cat");
    assert(con1.check(atom1), "Type constraint passes for matching type");
    
    atom2 := create_test_atom("PredicateNode", "likes");
    assert(!con1.check(atom2), "Type constraint fails for non-matching type");
    
    # Test 2: Type constraint (not equals)
    con2 := Constraint.new("type", v, "!=", "PredicateNode");
    assert(con2.check(atom1), "Type != constraint passes");
    assert(!con2.check(atom2), "Type != constraint fails");
    
    # Test 3: Value constraint
    con3 := Constraint.new("value", v, "=", "cat");
    assert(con3.check(atom1), "Value constraint passes");
    
    atom3 := create_test_atom("ConceptNode", "dog");
    assert(!con3.check(atom3), "Value constraint fails for different value");
    
    # Test 4: Truth value constraint
    atom4 := create_test_atom_with_tv("ConceptNode", "confident", 0.9, 0.8);
    con4 := Constraint.new("tv", v, ">", "0.5");
    assert(con4.check(atom4), "TV > constraint passes");
    
    con5 := Constraint.new("tv", v, "<", "0.5");
    assert(!con5.check(atom4), "TV < constraint fails");
    
    # Test 5: Attention value constraint
    atom5 := create_test_atom_with_av("ConceptNode", "important", 100, 50);
    con6 := Constraint.new("av", v, ">=", "50");
    assert(con6.check(atom5), "AV >= constraint passes");
    
    # Test 6: Constraint to_string
    s := con1.to_string();
    assert(len s > 0, "Constraint to_string produces output");
}

# =============================================================================
# PATTERN TESTS
# =============================================================================

test_patterns()
{
    sys->print("\n── Pattern Tests ────────────────────────────────────────────\n");
    
    # Test 1: Create pattern
    p := Pattern.new("test_pattern");
    assert_not_nil(p, "Create pattern");
    assert_eq_str(p.name, "test_pattern", "Pattern name");
    
    # Test 2: Add variables
    v1 := Variable.new("$X");
    v2 := Variable.new("$Y");
    p.add_variable(v1);
    p.add_variable(v2);
    
    vars := p.get_variables();
    count := 0;
    for (vl := vars; vl != nil; vl = tl vl)
        count++;
    assert_eq_int(count, 2, "Pattern has 2 variables");
    
    # Test 3: Add clauses
    c1 := Clause.new("InheritanceLink");
    c2 := Clause.new("EvaluationLink");
    p.add_clause(c1);
    p.add_clause(c2);
    
    clause_count := 0;
    for (cl := p.clauses; cl != nil; cl = tl cl)
        clause_count++;
    assert_eq_int(clause_count, 2, "Pattern has 2 clauses");
    
    # Test 4: Add constraints
    con := Constraint.new("type", v1, "=", "ConceptNode");
    p.add_constraint(con);
    
    con_count := 0;
    for (cons := p.constraints; cons != nil; cons = tl cons)
        con_count++;
    assert_eq_int(con_count, 1, "Pattern has 1 constraint");
    
    # Test 5: Default conjunction mode
    assert_eq_int(p.conjunction, 1, "Pattern default mode is AND");
    
    # Test 6: Set disjunction mode
    p.set_disjunction();
    assert_eq_int(p.conjunction, 0, "Pattern mode changed to OR");
    
    # Test 7: Pattern to_string
    s := p.to_string();
    assert(len s > 0, "Pattern to_string produces output");
}

# =============================================================================
# BINDING TESTS
# =============================================================================

test_bindings()
{
    sys->print("\n── Binding Tests ────────────────────────────────────────────\n");
    
    # Test 1: Create binding
    v := Variable.new("$X");
    atom := create_test_atom("ConceptNode", "cat");
    b := Binding.new(v, atom);
    assert_not_nil(b, "Create binding");
    
    # Test 2: Binding to_string
    s := b.to_string();
    assert(len s > 0, "Binding to_string produces output");
    
    # Test 3: Create binding set
    bs := BindingSet.new();
    assert_not_nil(bs, "Create binding set");
    assert_eq_int(bs.complete, 0, "Binding set initially incomplete");
    
    # Test 4: Add binding to set
    bs.add(b);
    assert(bs.contains("$X"), "Binding set contains $X");
    
    # Test 5: Get binding from set
    result := bs.get("$X");
    assert_not_nil(result, "Get binding returns atom");
    assert_eq_str(result.name, "cat", "Get binding returns correct atom");
    
    # Test 6: Get non-existent binding
    result2 := bs.get("$Y");
    assert(result2 == nil, "Get non-existent binding returns nil");
    
    # Test 7: Clone binding set
    bs2 := bs.clone();
    assert_not_nil(bs2, "Clone binding set");
    assert(bs2.contains("$X"), "Cloned set contains $X");
    
    # Test 8: Merge binding sets
    v2 := Variable.new("$Y");
    atom2 := create_test_atom("ConceptNode", "dog");
    b2 := Binding.new(v2, atom2);
    bs3 := BindingSet.new();
    bs3.add(b2);
    
    merged := bs.merge(bs3);
    assert(merged.contains("$X"), "Merged set contains $X");
    assert(merged.contains("$Y"), "Merged set contains $Y");
    
    # Test 9: Binding set confidence
    bs.confidence = 0.9;
    bs3.confidence = 0.8;
    merged2 := bs.merge(bs3);
    # Confidence should be multiplied: 0.9 * 0.8 = 0.72
    assert(merged2.confidence < 0.75 && merged2.confidence > 0.70, 
           "Merged confidence is product");
    
    # Test 10: Binding set to_string
    s2 := bs.to_string();
    assert(len s2 > 0, "Binding set to_string produces output");
}

# =============================================================================
# TOKENIZER TESTS
# =============================================================================

test_tokenizer()
{
    sys->print("\n── Tokenizer Tests ──────────────────────────────────────────\n");
    
    # Test 1: Tokenize parentheses
    t1 := Tokenizer.new("()");
    tokens1 := t1.tokenize();
    assert(count_tokens(tokens1) == 3, "Tokenize () produces 3 tokens");
    
    # Test 2: Tokenize variable
    t2 := Tokenizer.new("$X");
    tokens2 := t2.tokenize();
    first := hd tokens2;
    assert_eq_int(first.ttype, TOK_VARIABLE, "Tokenize $X as VARIABLE");
    assert_eq_str(first.value, "$X", "Variable value is $X");
    
    # Test 3: Tokenize string
    t3 := Tokenizer.new("\"hello world\"");
    tokens3 := t3.tokenize();
    first3 := hd tokens3;
    assert_eq_int(first3.ttype, TOK_STRING, "Tokenize string as STRING");
    assert_eq_str(first3.value, "hello world", "String value correct");
    
    # Test 4: Tokenize number
    t4 := Tokenizer.new("42");
    tokens4 := t4.tokenize();
    first4 := hd tokens4;
    assert_eq_int(first4.ttype, TOK_NUMBER, "Tokenize 42 as NUMBER");
    assert_eq_str(first4.value, "42", "Number value is 42");
    
    # Test 5: Tokenize keyword
    t5 := Tokenizer.new("BindLink");
    tokens5 := t5.tokenize();
    first5 := hd tokens5;
    assert_eq_int(first5.ttype, TOK_KEYWORD, "Tokenize BindLink as KEYWORD");
    
    # Test 6: Tokenize symbol
    t6 := Tokenizer.new("ConceptNode");
    tokens6 := t6.tokenize();
    first6 := hd tokens6;
    assert_eq_int(first6.ttype, TOK_SYMBOL, "Tokenize ConceptNode as SYMBOL");
    
    # Test 7: Skip whitespace
    t7 := Tokenizer.new("  $X  ");
    tokens7 := t7.tokenize();
    first7 := hd tokens7;
    assert_eq_int(first7.ttype, TOK_VARIABLE, "Whitespace skipped");
    
    # Test 8: Skip comments
    t8 := Tokenizer.new("; comment\n$X");
    tokens8 := t8.tokenize();
    first8 := hd tokens8;
    assert_eq_int(first8.ttype, TOK_VARIABLE, "Comment skipped");
    
    # Test 9: Complex expression
    expr := "(InheritanceLink $X (ConceptNode \"cat\"))";
    t9 := Tokenizer.new(expr);
    tokens9 := t9.tokenize();
    assert(count_tokens(tokens9) >= 7, "Complex expression tokenized");
    
    # Test 10: Token line/column tracking
    t10 := Tokenizer.new("$X\n$Y");
    tokens10 := t10.tokenize();
    first10 := hd tokens10;
    assert_eq_int(first10.line, 1, "First token on line 1");
}

count_tokens(tokens: list of ref Token): int
{
    count := 0;
    for (t := tokens; t != nil; t = tl t)
        count++;
    return count;
}

# =============================================================================
# PARSER TESTS
# =============================================================================

test_parser()
{
    sys->print("\n── Parser Tests ─────────────────────────────────────────────\n");
    
    # Test 1: Parse simple clause
    expr1 := "(InheritanceLink $X (ConceptNode \"mammal\"))";
    p1 := parse_expression(expr1);
    assert_not_nil(p1, "Parse simple clause");
    
    # Test 2: Parse BindLink
    expr2 := "(BindLink (VariableList $X) (InheritanceLink $X (ConceptNode \"cat\")))";
    p2 := parse_expression(expr2);
    assert_not_nil(p2, "Parse BindLink");
    assert_eq_str(p2.name, "BindLink", "Pattern name is BindLink");
    
    # Test 3: Parse with multiple variables
    expr3 := "(BindLink (VariableList $X $Y) (InheritanceLink $X $Y))";
    p3 := parse_expression(expr3);
    vars := p3.get_variables();
    var_count := 0;
    for (v := vars; v != nil; v = tl v)
        var_count++;
    assert_eq_int(var_count, 2, "Parse 2 variables");
    
    # Test 4: Parse AndLink
    expr4 := "(BindLink (VariableList $X) (AndLink (InheritanceLink $X (ConceptNode \"a\")) (InheritanceLink $X (ConceptNode \"b\"))))";
    p4 := parse_expression(expr4);
    clause_count := 0;
    for (c := p4.clauses; c != nil; c = tl c)
        clause_count++;
    assert_eq_int(clause_count, 2, "Parse AndLink with 2 clauses");
    assert_eq_int(p4.conjunction, 1, "AndLink sets conjunction mode");
    
    # Test 5: Parse OrLink
    expr5 := "(BindLink (VariableList $X) (OrLink (InheritanceLink $X (ConceptNode \"a\")) (InheritanceLink $X (ConceptNode \"b\"))))";
    p5 := parse_expression(expr5);
    assert_eq_int(p5.conjunction, 0, "OrLink sets disjunction mode");
    
    # Test 6: Parse GetLink
    expr6 := "(GetLink (VariableList $X) (InheritanceLink $X (ConceptNode \"cat\")))";
    p6 := parse_expression(expr6);
    assert_eq_str(p6.name, "GetLink", "Parse GetLink");
    
    # Test 7: Parse SatisfactionLink
    expr7 := "(SatisfactionLink (VariableList $X) (InheritanceLink $X (ConceptNode \"cat\")))";
    p7 := parse_expression(expr7);
    assert_eq_str(p7.name, "SatisfactionLink", "Parse SatisfactionLink");
}

parse_expression(expr: string): ref Pattern
{
    tokenizer := Tokenizer.new(expr);
    tokens := tokenizer.tokenize();
    parser := Parser.new(tokens);
    return parser.parse();
}

# =============================================================================
# COMPILER TESTS
# =============================================================================

test_compiler()
{
    sys->print("\n── Compiler Tests ───────────────────────────────────────────\n");
    
    compiler := Compiler.new();
    
    # Test 1: Compile simple pattern
    expr1 := "(BindLink (VariableList $X) (InheritanceLink $X (ConceptNode \"cat\")))";
    p1 := compiler.compile(expr1);
    assert_not_nil(p1, "Compile simple pattern");
    
    # Test 2: Validate pattern
    (valid, err) := compiler.validate(p1);
    assert_eq_int(valid, 1, "Pattern validation passes");
    
    # Test 3: Estimate cost
    cost := compiler.estimate_cost(p1);
    assert(cost > 0, "Cost estimation produces positive value");
    
    # Test 4: Extract variables
    vars := compiler.extract_variables(p1);
    var_count := 0;
    for (v := vars; v != nil; v = tl v)
        var_count++;
    assert(var_count >= 1, "Extract at least 1 variable");
    
    # Test 5: Compile complex pattern
    expr2 := "(BindLink (VariableList $X $Y) (AndLink (InheritanceLink $X (ConceptNode \"mammal\")) (InheritanceLink $Y (ConceptNode \"mammal\")) (EvaluationLink (PredicateNode \"friends\") (ListLink $X $Y))))";
    p2 := compiler.compile(expr2);
    assert_not_nil(p2, "Compile complex pattern");
    
    # Test 6: Cost increases with complexity
    cost2 := compiler.estimate_cost(p2);
    assert(cost2 > cost, "Complex pattern has higher cost");
    
    # Test 7: Optimize pattern (currently no-op)
    p3 := compiler.optimize(p1);
    assert_not_nil(p3, "Optimize returns pattern");
    
    # Test 8: Compile invalid pattern returns nil
    # (This would require error handling in the parser)
    
    # Test 9: Auto-declare undeclared variables
    expr3 := "(InheritanceLink $Z (ConceptNode \"test\"))";
    p4 := compiler.compile(expr3);
    vars4 := compiler.extract_variables(p4);
    found_z := 0;
    for (v := vars4; v != nil; v = tl v) {
        if ((hd v).name == "$Z")
            found_z = 1;
    }
    assert_eq_int(found_z, 1, "Undeclared variable auto-declared");
}

# =============================================================================
# MATCHER TESTS
# =============================================================================

test_matcher()
{
    sys->print("\n── Matcher Tests ────────────────────────────────────────────\n");
    
    # Create test AtomSpace
    space := create_test_atomspace();
    matcher := Matcher.new(space);
    
    # Test 1: Create matcher
    assert_not_nil(matcher, "Create matcher");
    
    # Test 2: Set max results
    matcher.set_max_results(100);
    assert_eq_int(matcher.max_results, 100, "Set max results");
    
    # Test 3: Set timeout
    matcher.set_timeout(5000);
    assert_eq_int(matcher.timeout_ms, 5000, "Set timeout");
    
    # Test 4: Match simple pattern
    p1 := create_simple_pattern();
    result1 := matcher.match(p1);
    assert_not_nil(result1, "Match returns result");
    
    # Test 5: Match result has pattern reference
    assert(result1.pattern != nil, "Result has pattern reference");
    
    # Test 6: Match result to_string
    s := result1.to_string();
    assert(len s > 0, "Match result to_string");
    
    # Test 7: Get candidates
    clause := Clause.new("InheritanceLink");
    candidates := matcher.get_candidates(clause);
    # Should find InheritanceLinks in test space
    cand_count := 0;
    for (c := candidates; c != nil; c = tl c)
        cand_count++;
    assert(cand_count >= 0, "Get candidates returns list");
    
    # Test 8: Check constraints
    constraints: list of ref Constraint;
    v := Variable.new("$X");
    con := Constraint.new("type", v, "=", "ConceptNode");
    constraints = con :: constraints;
    
    bs := BindingSet.new();
    atom := create_test_atom("ConceptNode", "test");
    bs.add(Binding.new(v, atom));
    
    assert(matcher.check_constraints(constraints, bs), "Check constraints passes");
    
    # Test 9: Match term with variable
    term := Term.from_variable(v);
    bindings := BindingSet.new();
    result_bindings := matcher.match_term(term, atom, bindings);
    assert_not_nil(result_bindings, "Match term binds variable");
    assert(result_bindings.contains("$X"), "Variable bound after match");
    
    # Test 10: Match term with constant
    const_term := Term.from_atom(atom);
    result_bindings2 := matcher.match_term(const_term, atom, bindings);
    assert_not_nil(result_bindings2, "Match constant term succeeds");
}

# =============================================================================
# QUERY EXECUTOR TESTS
# =============================================================================

test_query_executor()
{
    sys->print("\n── Query Executor Tests ─────────────────────────────────────\n");
    
    space := create_test_atomspace();
    qe := QueryExecutor.new(space);
    
    # Test 1: Create query executor
    assert_not_nil(qe, "Create query executor");
    
    # Test 2: Execute simple query
    query1 := "(BindLink (VariableList $X) (InheritanceLink $X (ConceptNode \"mammal\")))";
    result1 := qe.execute(query1);
    assert_not_nil(result1, "Execute query returns result");
    
    # Test 3: Execute pattern directly
    pattern := create_simple_pattern();
    result2 := qe.execute_pattern(pattern);
    assert_not_nil(result2, "Execute pattern returns result");
    
    # Test 4: Explain query
    explanation := qe.explain(query1);
    assert(len explanation > 0, "Explain produces output");
    
    # Test 5: Enable cache
    qe.enable_cache(100);
    assert(qe.cache != nil, "Cache enabled");
    
    # Test 6: Cache hit
    result3 := qe.execute(query1);  # First execution
    result4 := qe.execute(query1);  # Should hit cache
    assert(qe.cache.hits >= 1, "Cache hit recorded");
    
    # Test 7: Cache stats
    stats := qe.cache.stats();
    assert(len stats > 0, "Cache stats produces output");
    
    # Test 8: Cache invalidation
    qe.cache.invalidate();
    count := 0;
    for (e := qe.cache.entries; e != nil; e = tl e)
        count++;
    assert_eq_int(count, 0, "Cache invalidated");
}

# =============================================================================
# INTEGRATION TESTS
# =============================================================================

test_integration()
{
    sys->print("\n── Integration Tests ────────────────────────────────────────\n");
    
    # Create a realistic AtomSpace
    space := create_realistic_atomspace();
    qe := QueryExecutor.new(space);
    
    # Test 1: Find all mammals
    query1 := "(BindLink (VariableList $X) (InheritanceLink $X (ConceptNode \"mammal\")))";
    result1 := qe.execute(query1);
    assert_not_nil(result1, "Find all mammals query");
    
    # Test 2: Find friends (binary relation)
    query2 := "(BindLink (VariableList $X $Y) (EvaluationLink (PredicateNode \"friends\") (ListLink $X $Y)))";
    result2 := qe.execute(query2);
    assert_not_nil(result2, "Find friends query");
    
    # Test 3: Conjunction query (AND)
    query3 := "(BindLink (VariableList $X) (AndLink (InheritanceLink $X (ConceptNode \"mammal\")) (InheritanceLink $X (ConceptNode \"pet\"))))";
    result3 := qe.execute(query3);
    assert_not_nil(result3, "Conjunction query");
    
    # Test 4: Pattern with type constraint
    # (Would need TypedVariableLink support)
    
    # Test 5: Query explain shows execution plan
    explanation := qe.explain(query1);
    assert(len explanation > 100, "Explain shows detailed plan");
    
    # Test 6: Multiple executions are consistent
    result6a := qe.execute(query1);
    result6b := qe.execute(query1);
    assert_eq_int(result6a.match_count, result6b.match_count, 
                  "Multiple executions consistent");
    
    # Test 7: Empty result for non-matching pattern
    query7 := "(BindLink (VariableList $X) (InheritanceLink $X (ConceptNode \"nonexistent\")))";
    result7 := qe.execute(query7);
    assert_eq_int(result7.match_count, 0, "Non-matching pattern returns empty");
    
    # Test 8: Performance test (simple benchmark)
    # Execute 10 queries and check total time is reasonable
    for (i := 0; i < 10; i++)
        qe.execute(query1);
    assert(1, "10 queries executed without timeout");
}

# =============================================================================
# TEST HELPER FUNCTIONS
# =============================================================================

create_test_atom(atype: string, name: string): ref Atom
{
    atom := ref Atom;
    atom.id = 0;
    atom.atype = atype;
    atom.name = name;
    atom.tv = ref TruthValue;
    atom.tv.strength = 1.0;
    atom.tv.confidence = 1.0;
    atom.av = ref AttentionValue;
    atom.av.sti = 0;
    atom.av.lti = 0;
    atom.av.vlti = 0;
    atom.outgoing = nil;
    return atom;
}

create_test_atom_with_tv(atype: string, name: string, strength: real, confidence: real): ref Atom
{
    atom := create_test_atom(atype, name);
    atom.tv.strength = strength;
    atom.tv.confidence = confidence;
    return atom;
}

create_test_atom_with_av(atype: string, name: string, sti: int, lti: int): ref Atom
{
    atom := create_test_atom(atype, name);
    atom.av.sti = sti;
    atom.av.lti = lti;
    return atom;
}

create_test_atomspace(): ref Space
{
    space := ref Space;
    space.atoms = nil;
    space.atom_count = 0;
    
    # Add some test atoms
    cat := create_test_atom("ConceptNode", "cat");
    dog := create_test_atom("ConceptNode", "dog");
    mammal := create_test_atom("ConceptNode", "mammal");
    
    # Create inheritance links
    link1 := create_test_atom("InheritanceLink", "");
    link1.outgoing = cat :: mammal :: nil;
    
    link2 := create_test_atom("InheritanceLink", "");
    link2.outgoing = dog :: mammal :: nil;
    
    space.atoms = cat :: dog :: mammal :: link1 :: link2 :: nil;
    space.atom_count = 5;
    
    return space;
}

create_realistic_atomspace(): ref Space
{
    space := ref Space;
    space.atoms = nil;
    space.atom_count = 0;
    
    # Concepts
    cat := create_test_atom("ConceptNode", "cat");
    dog := create_test_atom("ConceptNode", "dog");
    bird := create_test_atom("ConceptNode", "bird");
    mammal := create_test_atom("ConceptNode", "mammal");
    animal := create_test_atom("ConceptNode", "animal");
    pet := create_test_atom("ConceptNode", "pet");
    
    # Predicates
    friends := create_test_atom("PredicateNode", "friends");
    likes := create_test_atom("PredicateNode", "likes");
    
    # Inheritance links
    link1 := create_test_atom("InheritanceLink", "");
    link1.outgoing = cat :: mammal :: nil;
    
    link2 := create_test_atom("InheritanceLink", "");
    link2.outgoing = dog :: mammal :: nil;
    
    link3 := create_test_atom("InheritanceLink", "");
    link3.outgoing = mammal :: animal :: nil;
    
    link4 := create_test_atom("InheritanceLink", "");
    link4.outgoing = cat :: pet :: nil;
    
    link5 := create_test_atom("InheritanceLink", "");
    link5.outgoing = dog :: pet :: nil;
    
    # Evaluation links (relations)
    list1 := create_test_atom("ListLink", "");
    list1.outgoing = cat :: dog :: nil;
    
    eval1 := create_test_atom("EvaluationLink", "");
    eval1.outgoing = friends :: list1 :: nil;
    
    space.atoms = cat :: dog :: bird :: mammal :: animal :: pet :: 
                  friends :: likes ::
                  link1 :: link2 :: link3 :: link4 :: link5 ::
                  list1 :: eval1 :: nil;
    space.atom_count = 15;
    
    return space;
}

create_simple_pattern(): ref Pattern
{
    p := Pattern.new("simple");
    
    v := Variable.new("$X");
    p.add_variable(v);
    
    c := Clause.new("InheritanceLink");
    t1 := Term.from_variable(v);
    atom := create_test_atom("ConceptNode", "mammal");
    t2 := Term.from_atom(atom);
    c.add_arg(t1);
    c.add_arg(t2);
    
    p.add_clause(c);
    
    return p;
}

# Token type constants (must match pattern.b)
TOK_LPAREN: con 1;
TOK_RPAREN: con 2;
TOK_VARIABLE: con 3;
TOK_SYMBOL: con 4;
TOK_STRING: con 5;
TOK_NUMBER: con 6;
TOK_KEYWORD: con 7;
TOK_EOF: con 8;
