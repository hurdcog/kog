#
# Pattern Matcher Kernel Module for Inferno-based OpenCog AGI OS
# 
# This module provides kernel-level pattern matching and query execution
# over the AtomSpace hypergraph. It is the foundation for PLN, URE, and
# MOSES inference engines.
#
# Device Interface: /dev/cog/pattern
#
# Copyright (c) 2025 OpenCog Foundation
# License: AGPL-3.0
#

implement PatternMatcher;

include "sys.m";
    sys: Sys;
include "draw.m";
include "string.m";
    str: String;
include "bufio.m";
    bufio: Bufio;
    Iobuf: import bufio;

# Import AtomSpace module for atom operations
include "atomspace.m";
    atomspace: AtomSpace;
    Atom, TruthValue, AttentionValue, Space: import atomspace;

PatternMatcher: module {
    PATH: con "/dis/cog/pattern.dis";
    
    # =========================================================================
    # CORE DATA STRUCTURES
    # =========================================================================
    
    # Variable representation for pattern matching
    # Variables are placeholders that can bind to any atom
    Variable: adt {
        name: string;           # Variable name (e.g., "$X", "$Y")
        type_constraint: string; # Optional type restriction (e.g., "ConceptNode")
        value_constraint: string; # Optional value restriction
        glob: int;              # 1 if glob variable (matches multiple atoms)
        
        new: fn(name: string): ref Variable;
        with_type: fn(v: self ref Variable, t: string): ref Variable;
        with_value: fn(v: self ref Variable, val: string): ref Variable;
        matches: fn(v: self ref Variable, atom: ref Atom): int;
        to_string: fn(v: self ref Variable): string;
    };
    
    # Term represents either a variable or a constant atom
    Term: adt {
        is_variable: int;       # 1 if variable, 0 if constant
        variable: ref Variable; # Variable reference (if is_variable)
        atom: ref Atom;         # Atom reference (if constant)
        
        from_variable: fn(v: ref Variable): ref Term;
        from_atom: fn(a: ref Atom): ref Term;
        to_string: fn(t: self ref Term): string;
    };
    
    # Clause represents a single pattern clause (predicate with arguments)
    # Example: (InheritanceLink $X (ConceptNode "mammal"))
    Clause: adt {
        link_type: string;              # Link type (e.g., "InheritanceLink")
        arguments: list of ref Term;    # Arguments (variables or constants)
        negated: int;                   # 1 if negated clause (NOT pattern)
        optional: int;                  # 1 if optional clause
        
        new: fn(link_type: string): ref Clause;
        add_arg: fn(c: self ref Clause, arg: ref Term): ref Clause;
        negate: fn(c: self ref Clause): ref Clause;
        make_optional: fn(c: self ref Clause): ref Clause;
        to_string: fn(c: self ref Clause): string;
    };
    
    # Constraint represents additional restrictions on variables
    Constraint: adt {
        ctype: string;          # Constraint type: "type", "value", "tv", "av", "absent"
        variable: ref Variable; # Variable this constraint applies to
        operator: string;       # Comparison operator: "=", "!=", "<", ">", "<=", ">="
        value: string;          # Value to compare against
        
        new: fn(ctype: string, v: ref Variable, op: string, val: string): ref Constraint;
        check: fn(c: self ref Constraint, atom: ref Atom): int;
        to_string: fn(c: self ref Constraint): string;
    };
    
    # Pattern represents a complete pattern for matching
    Pattern: adt {
        name: string;                       # Pattern name/identifier
        variables: list of ref Variable;    # All variables in the pattern
        clauses: list of ref Clause;        # Pattern clauses
        constraints: list of ref Constraint; # Additional constraints
        conjunction: int;                   # 1 for AND, 0 for OR
        
        new: fn(name: string): ref Pattern;
        add_variable: fn(p: self ref Pattern, v: ref Variable): ref Pattern;
        add_clause: fn(p: self ref Pattern, c: ref Clause): ref Pattern;
        add_constraint: fn(p: self ref Pattern, con: ref Constraint): ref Pattern;
        set_disjunction: fn(p: self ref Pattern): ref Pattern;
        get_variables: fn(p: self ref Pattern): list of ref Variable;
        to_string: fn(p: self ref Pattern): string;
    };
    
    # Binding represents a variable-to-atom binding
    Binding: adt {
        variable: ref Variable;
        value: ref Atom;
        
        new: fn(v: ref Variable, a: ref Atom): ref Binding;
        to_string: fn(b: self ref Binding): string;
    };
    
    # BindingSet represents a complete set of bindings for a pattern match
    BindingSet: adt {
        bindings: list of ref Binding;
        complete: int;          # 1 if all variables are bound
        confidence: real;       # Confidence score for this binding
        
        new: fn(): ref BindingSet;
        add: fn(bs: self ref BindingSet, b: ref Binding): ref BindingSet;
        get: fn(bs: self ref BindingSet, varname: string): ref Atom;
        contains: fn(bs: self ref BindingSet, varname: string): int;
        merge: fn(bs: self ref BindingSet, other: ref BindingSet): ref BindingSet;
        clone: fn(bs: self ref BindingSet): ref BindingSet;
        to_string: fn(bs: self ref BindingSet): string;
    };
    
    # MatchResult represents the result of a pattern match operation
    MatchResult: adt {
        pattern: ref Pattern;
        binding_sets: list of ref BindingSet;
        match_count: int;
        execution_time_ms: int;
        
        new: fn(p: ref Pattern): ref MatchResult;
        add_binding_set: fn(mr: self ref MatchResult, bs: ref BindingSet): ref MatchResult;
        to_string: fn(mr: self ref MatchResult): string;
    };
    
    # =========================================================================
    # TOKEN TYPES FOR PARSER
    # =========================================================================
    
    # Token types
    TOK_LPAREN: con 1;      # (
    TOK_RPAREN: con 2;      # )
    TOK_VARIABLE: con 3;    # $X, $Y, etc.
    TOK_SYMBOL: con 4;      # ConceptNode, InheritanceLink, etc.
    TOK_STRING: con 5;      # "quoted string"
    TOK_NUMBER: con 6;      # 123, 3.14
    TOK_KEYWORD: con 7;     # BindLink, VariableList, AndLink, etc.
    TOK_EOF: con 8;         # End of input
    
    Token: adt {
        ttype: int;
        value: string;
        line: int;
        col: int;
        
        new: fn(ttype: int, value: string, line: int, col: int): ref Token;
        to_string: fn(t: self ref Token): string;
    };
    
    # =========================================================================
    # TOKENIZER
    # =========================================================================
    
    Tokenizer: adt {
        input: string;
        pos: int;
        line: int;
        col: int;
        tokens: list of ref Token;
        
        new: fn(input: string): ref Tokenizer;
        tokenize: fn(t: self ref Tokenizer): list of ref Token;
        next_token: fn(t: self ref Tokenizer): ref Token;
        peek_char: fn(t: self ref Tokenizer): int;
        read_char: fn(t: self ref Tokenizer): int;
        skip_whitespace: fn(t: self ref Tokenizer);
        skip_comment: fn(t: self ref Tokenizer);
        read_string: fn(t: self ref Tokenizer): string;
        read_symbol: fn(t: self ref Tokenizer): string;
        read_number: fn(t: self ref Tokenizer): string;
        read_variable: fn(t: self ref Tokenizer): string;
    };
    
    # =========================================================================
    # PARSER
    # =========================================================================
    
    Parser: adt {
        tokens: list of ref Token;
        pos: int;
        current: ref Token;
        
        new: fn(tokens: list of ref Token): ref Parser;
        parse: fn(p: self ref Parser): ref Pattern;
        parse_expression: fn(p: self ref Parser): ref Pattern;
        parse_bind_link: fn(p: self ref Parser): ref Pattern;
        parse_variable_list: fn(p: self ref Parser): list of ref Variable;
        parse_clause: fn(p: self ref Parser): ref Clause;
        parse_term: fn(p: self ref Parser): ref Term;
        parse_and_link: fn(p: self ref Parser): list of ref Clause;
        parse_or_link: fn(p: self ref Parser): list of ref Clause;
        parse_not_link: fn(p: self ref Parser): ref Clause;
        advance: fn(p: self ref Parser): ref Token;
        expect: fn(p: self ref Parser, ttype: int): ref Token;
        peek: fn(p: self ref Parser): ref Token;
    };
    
    # =========================================================================
    # PATTERN COMPILER
    # =========================================================================
    
    Compiler: adt {
        new: fn(): ref Compiler;
        compile: fn(c: self ref Compiler, source: string): ref Pattern;
        optimize: fn(c: self ref Compiler, pattern: ref Pattern): ref Pattern;
        estimate_cost: fn(c: self ref Compiler, pattern: ref Pattern): int;
        extract_variables: fn(c: self ref Compiler, pattern: ref Pattern): list of ref Variable;
        validate: fn(c: self ref Compiler, pattern: ref Pattern): (int, string);
    };
    
    # =========================================================================
    # PATTERN MATCHER ENGINE
    # =========================================================================
    
    Matcher: adt {
        space: ref Space;
        max_results: int;
        timeout_ms: int;
        use_attention: int;     # Use attention values for prioritization
        
        new: fn(space: ref Space): ref Matcher;
        match: fn(m: self ref Matcher, pattern: ref Pattern): ref MatchResult;
        match_clause: fn(m: self ref Matcher, clause: ref Clause, 
                        bindings: ref BindingSet): list of ref BindingSet;
        match_term: fn(m: self ref Matcher, term: ref Term, atom: ref Atom,
                      bindings: ref BindingSet): ref BindingSet;
        unify: fn(m: self ref Matcher, term1: ref Term, term2: ref Term,
                 bindings: ref BindingSet): ref BindingSet;
        check_constraints: fn(m: self ref Matcher, constraints: list of ref Constraint,
                             bindings: ref BindingSet): int;
        get_candidates: fn(m: self ref Matcher, clause: ref Clause): list of ref Atom;
        set_max_results: fn(m: self ref Matcher, max: int): ref Matcher;
        set_timeout: fn(m: self ref Matcher, ms: int): ref Matcher;
    };
    
    # =========================================================================
    # QUERY EXECUTOR
    # =========================================================================
    
    QueryExecutor: adt {
        matcher: ref Matcher;
        compiler: ref Compiler;
        cache: ref QueryCache;
        
        new: fn(space: ref Space): ref QueryExecutor;
        execute: fn(qe: self ref QueryExecutor, query: string): ref MatchResult;
        execute_pattern: fn(qe: self ref QueryExecutor, pattern: ref Pattern): ref MatchResult;
        explain: fn(qe: self ref QueryExecutor, query: string): string;
        enable_cache: fn(qe: self ref QueryExecutor, size: int): ref QueryExecutor;
    };
    
    # Query cache for optimization
    QueryCache: adt {
        entries: list of ref CacheEntry;
        max_size: int;
        hits: int;
        misses: int;
        
        new: fn(size: int): ref QueryCache;
        get: fn(qc: self ref QueryCache, key: string): ref MatchResult;
        put: fn(qc: self ref QueryCache, key: string, result: ref MatchResult);
        invalidate: fn(qc: self ref QueryCache);
        stats: fn(qc: self ref QueryCache): string;
    };
    
    CacheEntry: adt {
        key: string;
        result: ref MatchResult;
        timestamp: int;
        access_count: int;
    };
    
    # =========================================================================
    # MODULE INTERFACE
    # =========================================================================
    
    init: fn(ctxt: ref Draw->Context, argv: list of string);
    
    # Pattern compilation
    compile: fn(source: string): ref Pattern;
    
    # Pattern matching
    match: fn(pattern: ref Pattern, space: ref Space): ref MatchResult;
    query: fn(source: string, space: ref Space): ref MatchResult;
    
    # Utility functions
    create_variable: fn(name: string): ref Variable;
    create_clause: fn(link_type: string, args: list of ref Term): ref Clause;
    create_pattern: fn(name: string, clauses: list of ref Clause): ref Pattern;
};

# =============================================================================
# IMPLEMENTATION
# =============================================================================

init(ctxt: ref Draw->Context, argv: list of string)
{
    sys = load Sys Sys->PATH;
    str = load String String->PATH;
    bufio = load Bufio Bufio->PATH;
    atomspace = load AtomSpace AtomSpace->PATH;
    
    sys->print("Pattern Matcher Kernel Module initialized\n");
    sys->print("Device: /dev/cog/pattern\n");
}

# =============================================================================
# VARIABLE IMPLEMENTATION
# =============================================================================

Variable.new(name: string): ref Variable
{
    v := ref Variable;
    v.name = name;
    v.type_constraint = "";
    v.value_constraint = "";
    v.glob = 0;
    return v;
}

Variable.with_type(v: self ref Variable, t: string): ref Variable
{
    v.type_constraint = t;
    return v;
}

Variable.with_value(v: self ref Variable, val: string): ref Variable
{
    v.value_constraint = val;
    return v;
}

Variable.matches(v: self ref Variable, atom: ref Atom): int
{
    # Check type constraint
    if (v.type_constraint != "" && atom.atype != v.type_constraint)
        return 0;
    
    # Check value constraint
    if (v.value_constraint != "" && atom.name != v.value_constraint)
        return 0;
    
    return 1;
}

Variable.to_string(v: self ref Variable): string
{
    s := v.name;
    if (v.type_constraint != "")
        s += ":" + v.type_constraint;
    if (v.value_constraint != "")
        s += "=" + v.value_constraint;
    return s;
}

# =============================================================================
# TERM IMPLEMENTATION
# =============================================================================

Term.from_variable(v: ref Variable): ref Term
{
    t := ref Term;
    t.is_variable = 1;
    t.variable = v;
    t.atom = nil;
    return t;
}

Term.from_atom(a: ref Atom): ref Term
{
    t := ref Term;
    t.is_variable = 0;
    t.variable = nil;
    t.atom = a;
    return t;
}

Term.to_string(t: self ref Term): string
{
    if (t.is_variable)
        return t.variable.to_string();
    else if (t.atom != nil)
        return t.atom.name;
    return "<nil>";
}

# =============================================================================
# CLAUSE IMPLEMENTATION
# =============================================================================

Clause.new(link_type: string): ref Clause
{
    c := ref Clause;
    c.link_type = link_type;
    c.arguments = nil;
    c.negated = 0;
    c.optional = 0;
    return c;
}

Clause.add_arg(c: self ref Clause, arg: ref Term): ref Clause
{
    c.arguments = append(c.arguments, arg);
    return c;
}

Clause.negate(c: self ref Clause): ref Clause
{
    c.negated = 1;
    return c;
}

Clause.make_optional(c: self ref Clause): ref Clause
{
    c.optional = 1;
    return c;
}

Clause.to_string(c: self ref Clause): string
{
    s := "(";
    if (c.negated)
        s += "NOT ";
    if (c.optional)
        s += "OPTIONAL ";
    s += c.link_type;
    for (args := c.arguments; args != nil; args = tl args) {
        arg := hd args;
        s += " " + arg.to_string();
    }
    s += ")";
    return s;
}

# =============================================================================
# CONSTRAINT IMPLEMENTATION
# =============================================================================

Constraint.new(ctype: string, v: ref Variable, op: string, val: string): ref Constraint
{
    con := ref Constraint;
    con.ctype = ctype;
    con.variable = v;
    con.operator = op;
    con.value = val;
    return con;
}

Constraint.check(c: self ref Constraint, atom: ref Atom): int
{
    case c.ctype {
        "type" =>
            case c.operator {
                "=" => return atom.atype == c.value;
                "!=" => return atom.atype != c.value;
            }
        "value" =>
            case c.operator {
                "=" => return atom.name == c.value;
                "!=" => return atom.name != c.value;
            }
        "tv" =>
            # Check truth value constraints
            # Format: "strength>0.5" or "confidence>=0.8"
            return check_tv_constraint(atom, c.operator, c.value);
        "av" =>
            # Check attention value constraints
            return check_av_constraint(atom, c.operator, c.value);
        "absent" =>
            # Absent constraint - atom should NOT exist
            return atom == nil;
    }
    return 1;
}

check_tv_constraint(atom: ref Atom, op: string, val: string): int
{
    # Parse value as real
    target := real val;
    
    # Get atom's truth value strength (default comparison)
    strength := atom.tv.strength;
    
    case op {
        ">" => return strength > target;
        ">=" => return strength >= target;
        "<" => return strength < target;
        "<=" => return strength <= target;
        "=" => return strength == target;
        "!=" => return strength != target;
    }
    return 0;
}

check_av_constraint(atom: ref Atom, op: string, val: string): int
{
    # Parse value as int
    target := int val;
    
    # Get atom's attention value STI (default comparison)
    sti := atom.av.sti;
    
    case op {
        ">" => return sti > target;
        ">=" => return sti >= target;
        "<" => return sti < target;
        "<=" => return sti <= target;
        "=" => return sti == target;
        "!=" => return sti != target;
    }
    return 0;
}

Constraint.to_string(c: self ref Constraint): string
{
    return sys->sprint("(%s %s %s %s)", c.ctype, c.variable.name, c.operator, c.value);
}

# =============================================================================
# PATTERN IMPLEMENTATION
# =============================================================================

Pattern.new(name: string): ref Pattern
{
    p := ref Pattern;
    p.name = name;
    p.variables = nil;
    p.clauses = nil;
    p.constraints = nil;
    p.conjunction = 1;  # Default to AND
    return p;
}

Pattern.add_variable(p: self ref Pattern, v: ref Variable): ref Pattern
{
    p.variables = append(p.variables, v);
    return p;
}

Pattern.add_clause(p: self ref Pattern, c: ref Clause): ref Clause
{
    p.clauses = append(p.clauses, c);
    return p;
}

Pattern.add_constraint(p: self ref Pattern, con: ref Constraint): ref Pattern
{
    p.constraints = append(p.constraints, con);
    return p;
}

Pattern.set_disjunction(p: self ref Pattern): ref Pattern
{
    p.conjunction = 0;
    return p;
}

Pattern.get_variables(p: self ref Pattern): list of ref Variable
{
    return p.variables;
}

Pattern.to_string(p: self ref Pattern): string
{
    s := "(Pattern " + p.name + "\n";
    s += "  Variables: [";
    for (vars := p.variables; vars != nil; vars = tl vars) {
        v := hd vars;
        s += v.to_string();
        if (tl vars != nil)
            s += ", ";
    }
    s += "]\n";
    s += "  Clauses: [\n";
    for (clauses := p.clauses; clauses != nil; clauses = tl clauses) {
        c := hd clauses;
        s += "    " + c.to_string() + "\n";
    }
    s += "  ]\n";
    s += "  Constraints: [";
    for (cons := p.constraints; cons != nil; cons = tl cons) {
        con := hd cons;
        s += con.to_string();
        if (tl cons != nil)
            s += ", ";
    }
    s += "]\n";
    s += "  Mode: " + (p.conjunction ? "AND" : "OR") + "\n";
    s += ")";
    return s;
}

# =============================================================================
# BINDING IMPLEMENTATION
# =============================================================================

Binding.new(v: ref Variable, a: ref Atom): ref Binding
{
    b := ref Binding;
    b.variable = v;
    b.value = a;
    return b;
}

Binding.to_string(b: self ref Binding): string
{
    return sys->sprint("%s -> %s", b.variable.name, b.value.name);
}

# =============================================================================
# BINDING SET IMPLEMENTATION
# =============================================================================

BindingSet.new(): ref BindingSet
{
    bs := ref BindingSet;
    bs.bindings = nil;
    bs.complete = 0;
    bs.confidence = 1.0;
    return bs;
}

BindingSet.add(bs: self ref BindingSet, b: ref Binding): ref BindingSet
{
    bs.bindings = append(bs.bindings, b);
    return bs;
}

BindingSet.get(bs: self ref BindingSet, varname: string): ref Atom
{
    for (bindings := bs.bindings; bindings != nil; bindings = tl bindings) {
        b := hd bindings;
        if (b.variable.name == varname)
            return b.value;
    }
    return nil;
}

BindingSet.contains(bs: self ref BindingSet, varname: string): int
{
    for (bindings := bs.bindings; bindings != nil; bindings = tl bindings) {
        b := hd bindings;
        if (b.variable.name == varname)
            return 1;
    }
    return 0;
}

BindingSet.merge(bs: self ref BindingSet, other: ref BindingSet): ref BindingSet
{
    if (other == nil)
        return bs;
    
    result := bs.clone();
    for (bindings := other.bindings; bindings != nil; bindings = tl bindings) {
        b := hd bindings;
        if (!result.contains(b.variable.name))
            result.add(b);
    }
    
    # Update confidence (multiply confidences)
    result.confidence = bs.confidence * other.confidence;
    
    return result;
}

BindingSet.clone(bs: self ref BindingSet): ref BindingSet
{
    result := BindingSet.new();
    result.bindings = bs.bindings;  # Shallow copy is OK for immutable bindings
    result.complete = bs.complete;
    result.confidence = bs.confidence;
    return result;
}

BindingSet.to_string(bs: self ref BindingSet): string
{
    s := "{";
    for (bindings := bs.bindings; bindings != nil; bindings = tl bindings) {
        b := hd bindings;
        s += b.to_string();
        if (tl bindings != nil)
            s += ", ";
    }
    s += sys->sprint("} (confidence: %.3f)", bs.confidence);
    return s;
}

# =============================================================================
# MATCH RESULT IMPLEMENTATION
# =============================================================================

MatchResult.new(p: ref Pattern): ref MatchResult
{
    mr := ref MatchResult;
    mr.pattern = p;
    mr.binding_sets = nil;
    mr.match_count = 0;
    mr.execution_time_ms = 0;
    return mr;
}

MatchResult.add_binding_set(mr: self ref MatchResult, bs: ref BindingSet): ref MatchResult
{
    mr.binding_sets = append(mr.binding_sets, bs);
    mr.match_count++;
    return mr;
}

MatchResult.to_string(mr: self ref MatchResult): string
{
    s := sys->sprint("MatchResult: %d matches in %dms\n", mr.match_count, mr.execution_time_ms);
    s += "Pattern: " + mr.pattern.name + "\n";
    s += "Bindings:\n";
    i := 1;
    for (bsets := mr.binding_sets; bsets != nil; bsets = tl bsets) {
        bs := hd bsets;
        s += sys->sprint("  %d: %s\n", i, bs.to_string());
        i++;
    }
    return s;
}

# =============================================================================
# TOKEN IMPLEMENTATION
# =============================================================================

Token.new(ttype: int, value: string, line: int, col: int): ref Token
{
    t := ref Token;
    t.ttype = ttype;
    t.value = value;
    t.line = line;
    t.col = col;
    return t;
}

Token.to_string(t: self ref Token): string
{
    typename := "";
    case t.ttype {
        TOK_LPAREN => typename = "LPAREN";
        TOK_RPAREN => typename = "RPAREN";
        TOK_VARIABLE => typename = "VARIABLE";
        TOK_SYMBOL => typename = "SYMBOL";
        TOK_STRING => typename = "STRING";
        TOK_NUMBER => typename = "NUMBER";
        TOK_KEYWORD => typename = "KEYWORD";
        TOK_EOF => typename = "EOF";
    }
    return sys->sprint("Token(%s, '%s', %d:%d)", typename, t.value, t.line, t.col);
}

# =============================================================================
# TOKENIZER IMPLEMENTATION
# =============================================================================

Tokenizer.new(input: string): ref Tokenizer
{
    t := ref Tokenizer;
    t.input = input;
    t.pos = 0;
    t.line = 1;
    t.col = 1;
    t.tokens = nil;
    return t;
}

Tokenizer.peek_char(t: self ref Tokenizer): int
{
    if (t.pos >= len t.input)
        return -1;
    return t.input[t.pos];
}

Tokenizer.read_char(t: self ref Tokenizer): int
{
    if (t.pos >= len t.input)
        return -1;
    c := t.input[t.pos];
    t.pos++;
    if (c == '\n') {
        t.line++;
        t.col = 1;
    } else {
        t.col++;
    }
    return c;
}

Tokenizer.skip_whitespace(t: self ref Tokenizer)
{
    for (;;) {
        c := t.peek_char();
        if (c == -1)
            return;
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
            t.read_char();
        else if (c == ';')
            t.skip_comment();
        else
            return;
    }
}

Tokenizer.skip_comment(t: self ref Tokenizer)
{
    # Skip to end of line
    for (;;) {
        c := t.read_char();
        if (c == -1 || c == '\n')
            return;
    }
}

Tokenizer.read_string(t: self ref Tokenizer): string
{
    t.read_char();  # Skip opening quote
    s := "";
    for (;;) {
        c := t.read_char();
        if (c == -1 || c == '"')
            break;
        if (c == '\\') {
            # Handle escape sequences
            c = t.read_char();
            case c {
                'n' => s[len s] = '\n';
                't' => s[len s] = '\t';
                '\\' => s[len s] = '\\';
                '"' => s[len s] = '"';
                * => s[len s] = c;
            }
        } else {
            s[len s] = c;
        }
    }
    return s;
}

Tokenizer.read_symbol(t: self ref Tokenizer): string
{
    s := "";
    for (;;) {
        c := t.peek_char();
        if (c == -1)
            break;
        # Symbol characters: alphanumeric, -, _, :
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == ':') {
            s[len s] = t.read_char();
        } else {
            break;
        }
    }
    return s;
}

Tokenizer.read_number(t: self ref Tokenizer): string
{
    s := "";
    has_dot := 0;
    for (;;) {
        c := t.peek_char();
        if (c == -1)
            break;
        if (c >= '0' && c <= '9') {
            s[len s] = t.read_char();
        } else if (c == '.' && !has_dot) {
            s[len s] = t.read_char();
            has_dot = 1;
        } else if (c == '-' && len s == 0) {
            s[len s] = t.read_char();
        } else {
            break;
        }
    }
    return s;
}

Tokenizer.read_variable(t: self ref Tokenizer): string
{
    t.read_char();  # Skip $
    s := "$";
    for (;;) {
        c := t.peek_char();
        if (c == -1)
            break;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '_') {
            s[len s] = t.read_char();
        } else {
            break;
        }
    }
    return s;
}

Tokenizer.next_token(t: self ref Tokenizer): ref Token
{
    t.skip_whitespace();
    
    line := t.line;
    col := t.col;
    c := t.peek_char();
    
    if (c == -1)
        return Token.new(TOK_EOF, "", line, col);
    
    case c {
        '(' =>
            t.read_char();
            return Token.new(TOK_LPAREN, "(", line, col);
        ')' =>
            t.read_char();
            return Token.new(TOK_RPAREN, ")", line, col);
        '"' =>
            return Token.new(TOK_STRING, t.read_string(), line, col);
        '$' =>
            return Token.new(TOK_VARIABLE, t.read_variable(), line, col);
    }
    
    # Check for number
    if ((c >= '0' && c <= '9') || (c == '-' && t.pos + 1 < len t.input && 
        t.input[t.pos + 1] >= '0' && t.input[t.pos + 1] <= '9')) {
        return Token.new(TOK_NUMBER, t.read_number(), line, col);
    }
    
    # Must be a symbol or keyword
    sym := t.read_symbol();
    
    # Check if it's a keyword
    keywords := array[] of {
        "BindLink", "GetLink", "SatisfactionLink",
        "VariableList", "TypedVariableLink", "VariableNode",
        "AndLink", "OrLink", "NotLink",
        "PresentLink", "AbsentLink", "ChoiceLink",
        "EvaluationLink", "ExecutionOutputLink",
        "GroundedPredicateNode", "GroundedSchemaNode"
    };
    
    for (i := 0; i < len keywords; i++) {
        if (sym == keywords[i])
            return Token.new(TOK_KEYWORD, sym, line, col);
    }
    
    return Token.new(TOK_SYMBOL, sym, line, col);
}

Tokenizer.tokenize(t: self ref Tokenizer): list of ref Token
{
    tokens: list of ref Token;
    for (;;) {
        tok := t.next_token();
        tokens = append(tokens, tok);
        if (tok.ttype == TOK_EOF)
            break;
    }
    return tokens;
}

# =============================================================================
# PARSER IMPLEMENTATION
# =============================================================================

Parser.new(tokens: list of ref Token): ref Parser
{
    p := ref Parser;
    p.tokens = tokens;
    p.pos = 0;
    if (tokens != nil)
        p.current = hd tokens;
    else
        p.current = nil;
    return p;
}

Parser.advance(p: self ref Parser): ref Token
{
    prev := p.current;
    p.pos++;
    
    # Navigate to position in list
    toks := p.tokens;
    for (i := 0; i < p.pos && toks != nil; i++)
        toks = tl toks;
    
    if (toks != nil)
        p.current = hd toks;
    else
        p.current = Token.new(TOK_EOF, "", 0, 0);
    
    return prev;
}

Parser.expect(p: self ref Parser, ttype: int): ref Token
{
    if (p.current.ttype != ttype) {
        sys->print("Parse error: expected token type %d, got %d at %d:%d\n",
                   ttype, p.current.ttype, p.current.line, p.current.col);
        return nil;
    }
    return p.advance();
}

Parser.peek(p: self ref Parser): ref Token
{
    return p.current;
}

Parser.parse(p: self ref Parser): ref Pattern
{
    return p.parse_expression();
}

Parser.parse_expression(p: self ref Parser): ref Pattern
{
    p.expect(TOK_LPAREN);
    
    tok := p.peek();
    if (tok.ttype == TOK_KEYWORD) {
        case tok.value {
            "BindLink" or "GetLink" or "SatisfactionLink" =>
                return p.parse_bind_link();
        }
    }
    
    # Default: parse as simple clause pattern
    pattern := Pattern.new("anonymous");
    clause := p.parse_clause();
    pattern.add_clause(clause);
    
    p.expect(TOK_RPAREN);
    return pattern;
}

Parser.parse_bind_link(p: self ref Parser): ref Pattern
{
    keyword := p.advance();  # Consume BindLink/GetLink/SatisfactionLink
    
    pattern := Pattern.new(keyword.value);
    
    # Parse variable list
    p.expect(TOK_LPAREN);
    if (p.peek().value == "VariableList") {
        vars := p.parse_variable_list();
        for (vl := vars; vl != nil; vl = tl vl)
            pattern.add_variable(hd vl);
    } else if (p.peek().ttype == TOK_VARIABLE) {
        # Single variable
        varname := p.advance().value;
        pattern.add_variable(Variable.new(varname));
        p.expect(TOK_RPAREN);
    } else {
        p.expect(TOK_RPAREN);
    }
    
    # Parse body (clauses)
    p.expect(TOK_LPAREN);
    if (p.peek().value == "AndLink") {
        clauses := p.parse_and_link();
        for (cl := clauses; cl != nil; cl = tl cl)
            pattern.add_clause(hd cl);
    } else if (p.peek().value == "OrLink") {
        clauses := p.parse_or_link();
        for (cl := clauses; cl != nil; cl = tl cl)
            pattern.add_clause(hd cl);
        pattern.set_disjunction();
    } else {
        # Single clause
        clause := p.parse_clause();
        pattern.add_clause(clause);
        p.expect(TOK_RPAREN);
    }
    
    p.expect(TOK_RPAREN);  # Close BindLink
    return pattern;
}

Parser.parse_variable_list(p: self ref Parser): list of ref Variable
{
    p.advance();  # Consume VariableList
    
    vars: list of ref Variable;
    
    for (;;) {
        tok := p.peek();
        if (tok.ttype == TOK_RPAREN)
            break;
        
        if (tok.ttype == TOK_VARIABLE) {
            varname := p.advance().value;
            vars = append(vars, Variable.new(varname));
        } else if (tok.ttype == TOK_LPAREN) {
            # TypedVariableLink
            p.advance();  # (
            if (p.peek().value == "TypedVariableLink") {
                p.advance();  # TypedVariableLink
                p.expect(TOK_LPAREN);
                p.advance();  # VariableNode
                varname := p.expect(TOK_STRING).value;
                p.expect(TOK_RPAREN);
                p.expect(TOK_LPAREN);
                typename := p.advance().value;  # Type name
                p.expect(TOK_RPAREN);
                p.expect(TOK_RPAREN);
                
                v := Variable.new("$" + varname);
                v.with_type(typename);
                vars = append(vars, v);
            } else {
                # Skip unknown structure
                depth := 1;
                for (depth > 0) {
                    t := p.advance();
                    if (t.ttype == TOK_LPAREN)
                        depth++;
                    else if (t.ttype == TOK_RPAREN)
                        depth--;
                }
            }
        } else {
            break;
        }
    }
    
    p.expect(TOK_RPAREN);
    return vars;
}

Parser.parse_and_link(p: self ref Parser): list of ref Clause
{
    p.advance();  # Consume AndLink
    
    clauses: list of ref Clause;
    
    for (;;) {
        tok := p.peek();
        if (tok.ttype == TOK_RPAREN)
            break;
        
        if (tok.ttype == TOK_LPAREN) {
            p.advance();  # (
            
            if (p.peek().value == "NotLink") {
                clause := p.parse_not_link();
                clauses = append(clauses, clause);
            } else {
                clause := p.parse_clause();
                clauses = append(clauses, clause);
            }
            
            p.expect(TOK_RPAREN);
        } else {
            break;
        }
    }
    
    p.expect(TOK_RPAREN);
    return clauses;
}

Parser.parse_or_link(p: self ref Parser): list of ref Clause
{
    p.advance();  # Consume OrLink
    
    clauses: list of ref Clause;
    
    for (;;) {
        tok := p.peek();
        if (tok.ttype == TOK_RPAREN)
            break;
        
        if (tok.ttype == TOK_LPAREN) {
            p.advance();  # (
            clause := p.parse_clause();
            clauses = append(clauses, clause);
            p.expect(TOK_RPAREN);
        } else {
            break;
        }
    }
    
    p.expect(TOK_RPAREN);
    return clauses;
}

Parser.parse_not_link(p: self ref Parser): ref Clause
{
    p.advance();  # Consume NotLink
    
    p.expect(TOK_LPAREN);
    clause := p.parse_clause();
    clause.negate();
    p.expect(TOK_RPAREN);
    
    p.expect(TOK_RPAREN);
    return clause;
}

Parser.parse_clause(p: self ref Parser): ref Clause
{
    # Get link type
    link_type := p.advance().value;
    clause := Clause.new(link_type);
    
    # Parse arguments
    for (;;) {
        tok := p.peek();
        if (tok.ttype == TOK_RPAREN || tok.ttype == TOK_EOF)
            break;
        
        term := p.parse_term();
        clause.add_arg(term);
    }
    
    return clause;
}

Parser.parse_term(p: self ref Parser): ref Term
{
    tok := p.peek();
    
    case tok.ttype {
        TOK_VARIABLE =>
            p.advance();
            return Term.from_variable(Variable.new(tok.value));
        
        TOK_LPAREN =>
            # Nested expression - create atom reference
            p.advance();  # (
            node_type := p.advance().value;
            node_name := "";
            if (p.peek().ttype == TOK_STRING)
                node_name = p.advance().value;
            p.expect(TOK_RPAREN);
            
            # Create a placeholder atom
            atom := ref Atom;
            atom.atype = node_type;
            atom.name = node_name;
            return Term.from_atom(atom);
        
        TOK_STRING =>
            p.advance();
            atom := ref Atom;
            atom.atype = "StringValue";
            atom.name = tok.value;
            return Term.from_atom(atom);
        
        TOK_NUMBER =>
            p.advance();
            atom := ref Atom;
            atom.atype = "NumberValue";
            atom.name = tok.value;
            return Term.from_atom(atom);
        
        TOK_SYMBOL =>
            p.advance();
            atom := ref Atom;
            atom.atype = "SymbolNode";
            atom.name = tok.value;
            return Term.from_atom(atom);
    }
    
    # Default: return nil term
    return nil;
}

# =============================================================================
# COMPILER IMPLEMENTATION
# =============================================================================

Compiler.new(): ref Compiler
{
    c := ref Compiler;
    return c;
}

Compiler.compile(c: self ref Compiler, source: string): ref Pattern
{
    # Tokenize
    tokenizer := Tokenizer.new(source);
    tokens := tokenizer.tokenize();
    
    # Parse
    parser := Parser.new(tokens);
    pattern := parser.parse();
    
    # Validate
    (valid, err) := c.validate(pattern);
    if (!valid) {
        sys->print("Pattern validation error: %s\n", err);
        return nil;
    }
    
    # Optimize
    pattern = c.optimize(pattern);
    
    return pattern;
}

Compiler.optimize(c: self ref Compiler, pattern: ref Pattern): ref Pattern
{
    # Optimization 1: Reorder clauses by selectivity
    # Put most constrained clauses first
    
    # Optimization 2: Remove redundant constraints
    
    # Optimization 3: Merge compatible clauses
    
    # For now, return pattern as-is
    return pattern;
}

Compiler.estimate_cost(c: self ref Compiler, pattern: ref Pattern): int
{
    cost := 0;
    
    # Base cost per clause
    for (clauses := pattern.clauses; clauses != nil; clauses = tl clauses)
        cost += 100;
    
    # Cost per variable (unbound variables increase cost)
    for (vars := pattern.variables; vars != nil; vars = tl vars) {
        v := hd vars;
        if (v.type_constraint == "")
            cost += 50;  # Unconstrained variable
        else
            cost += 10;  # Constrained variable
    }
    
    # Disjunction is more expensive
    if (!pattern.conjunction)
        cost *= 2;
    
    return cost;
}

Compiler.extract_variables(c: self ref Compiler, pattern: ref Pattern): list of ref Variable
{
    # Extract all variables from clauses
    vars: list of ref Variable;
    seen: list of string;
    
    for (clauses := pattern.clauses; clauses != nil; clauses = tl clauses) {
        clause := hd clauses;
        for (args := clause.arguments; args != nil; args = tl args) {
            term := hd args;
            if (term.is_variable) {
                # Check if already seen
                found := 0;
                for (s := seen; s != nil; s = tl s) {
                    if (hd s == term.variable.name) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    vars = append(vars, term.variable);
                    seen = append(seen, term.variable.name);
                }
            }
        }
    }
    
    return vars;
}

Compiler.validate(c: self ref Compiler, pattern: ref Pattern): (int, string)
{
    if (pattern == nil)
        return (0, "Pattern is nil");
    
    if (pattern.clauses == nil)
        return (0, "Pattern has no clauses");
    
    # Check that all variables in clauses are declared
    declared: list of string;
    for (vars := pattern.variables; vars != nil; vars = tl vars)
        declared = append(declared, (hd vars).name);
    
    for (clauses := pattern.clauses; clauses != nil; clauses = tl clauses) {
        clause := hd clauses;
        for (args := clause.arguments; args != nil; args = tl args) {
            term := hd args;
            if (term != nil && term.is_variable) {
                found := 0;
                for (d := declared; d != nil; d = tl d) {
                    if (hd d == term.variable.name) {
                        found = 1;
                        break;
                    }
                }
                # Auto-declare undeclared variables
                if (!found) {
                    pattern.add_variable(term.variable);
                    declared = append(declared, term.variable.name);
                }
            }
        }
    }
    
    return (1, "");
}

# =============================================================================
# MATCHER IMPLEMENTATION
# =============================================================================

Matcher.new(space: ref Space): ref Matcher
{
    m := ref Matcher;
    m.space = space;
    m.max_results = 1000;
    m.timeout_ms = 10000;
    m.use_attention = 1;
    return m;
}

Matcher.set_max_results(m: self ref Matcher, max: int): ref Matcher
{
    m.max_results = max;
    return m;
}

Matcher.set_timeout(m: self ref Matcher, ms: int): ref Matcher
{
    m.timeout_ms = ms;
    return m;
}

Matcher.match(m: self ref Matcher, pattern: ref Pattern): ref MatchResult
{
    result := MatchResult.new(pattern);
    
    # Start with empty binding set
    initial_bindings := BindingSet.new();
    current_bindings: list of ref BindingSet;
    current_bindings = append(current_bindings, initial_bindings);
    
    if (pattern.conjunction) {
        # AND: all clauses must match
        for (clauses := pattern.clauses; clauses != nil; clauses = tl clauses) {
            clause := hd clauses;
            next_bindings: list of ref BindingSet;
            
            for (cb := current_bindings; cb != nil; cb = tl cb) {
                bindings := hd cb;
                new_bindings := m.match_clause(clause, bindings);
                
                for (nb := new_bindings; nb != nil; nb = tl nb)
                    next_bindings = append(next_bindings, hd nb);
            }
            
            current_bindings = next_bindings;
            
            # Early termination if no matches
            if (current_bindings == nil)
                break;
        }
    } else {
        # OR: any clause can match
        for (clauses := pattern.clauses; clauses != nil; clauses = tl clauses) {
            clause := hd clauses;
            new_bindings := m.match_clause(clause, initial_bindings);
            
            for (nb := new_bindings; nb != nil; nb = tl nb)
                current_bindings = append(current_bindings, hd nb);
        }
    }
    
    # Check constraints and add to result
    for (cb := current_bindings; cb != nil; cb = tl cb) {
        bindings := hd cb;
        if (m.check_constraints(pattern.constraints, bindings)) {
            bindings.complete = 1;
            result.add_binding_set(bindings);
            
            if (result.match_count >= m.max_results)
                break;
        }
    }
    
    return result;
}

Matcher.match_clause(m: self ref Matcher, clause: ref Clause, 
                     bindings: ref BindingSet): list of ref BindingSet
{
    results: list of ref BindingSet;
    
    # Get candidate atoms
    candidates := m.get_candidates(clause);
    
    for (cl := candidates; cl != nil; cl = tl cl) {
        candidate := hd cl;
        
        # Try to match each argument
        new_bindings := bindings.clone();
        match_success := 1;
        
        # Get outgoing set of candidate link
        outgoing := candidate.outgoing;
        args := clause.arguments;
        
        for (; args != nil && outgoing != nil; args = tl args) {
            term := hd args;
            target := hd outgoing;
            outgoing = tl outgoing;
            
            result_bindings := m.match_term(term, target, new_bindings);
            if (result_bindings == nil) {
                match_success = 0;
                break;
            }
            new_bindings = result_bindings;
        }
        
        # Handle negated clauses
        if (clause.negated)
            match_success = !match_success;
        
        if (match_success)
            results = append(results, new_bindings);
    }
    
    # Handle optional clauses
    if (clause.optional && results == nil)
        results = append(results, bindings);
    
    return results;
}

Matcher.match_term(m: self ref Matcher, term: ref Term, atom: ref Atom,
                   bindings: ref BindingSet): ref BindingSet
{
    if (term == nil || atom == nil)
        return nil;
    
    if (term.is_variable) {
        # Check if variable is already bound
        existing := bindings.get(term.variable.name);
        if (existing != nil) {
            # Variable already bound - check consistency
            if (existing.id != atom.id)
                return nil;
            return bindings;
        }
        
        # Check variable constraints
        if (!term.variable.matches(atom))
            return nil;
        
        # Bind variable
        new_bindings := bindings.clone();
        new_bindings.add(Binding.new(term.variable, atom));
        return new_bindings;
    } else {
        # Constant term - must match exactly
        if (term.atom.atype != atom.atype)
            return nil;
        if (term.atom.name != "" && term.atom.name != atom.name)
            return nil;
        return bindings;
    }
}

Matcher.unify(m: self ref Matcher, term1: ref Term, term2: ref Term,
              bindings: ref BindingSet): ref BindingSet
{
    # Unification algorithm for two terms
    
    # If both are constants, they must be equal
    if (!term1.is_variable && !term2.is_variable) {
        if (term1.atom.atype == term2.atom.atype &&
            term1.atom.name == term2.atom.name)
            return bindings;
        return nil;
    }
    
    # If term1 is a variable
    if (term1.is_variable) {
        existing := bindings.get(term1.variable.name);
        if (existing != nil) {
            # Already bound - unify with bound value
            bound_term := Term.from_atom(existing);
            return m.unify(bound_term, term2, bindings);
        }
        
        # Bind term1 to term2
        new_bindings := bindings.clone();
        if (term2.is_variable) {
            # Bind to another variable's value if bound
            val := bindings.get(term2.variable.name);
            if (val != nil)
                new_bindings.add(Binding.new(term1.variable, val));
            # Otherwise, leave unbound (will be bound later)
        } else {
            new_bindings.add(Binding.new(term1.variable, term2.atom));
        }
        return new_bindings;
    }
    
    # If term2 is a variable (and term1 is not)
    if (term2.is_variable) {
        existing := bindings.get(term2.variable.name);
        if (existing != nil) {
            # Already bound - check equality
            if (existing.atype == term1.atom.atype &&
                existing.name == term1.atom.name)
                return bindings;
            return nil;
        }
        
        # Bind term2 to term1
        new_bindings := bindings.clone();
        new_bindings.add(Binding.new(term2.variable, term1.atom));
        return new_bindings;
    }
    
    return nil;
}

Matcher.check_constraints(m: self ref Matcher, constraints: list of ref Constraint,
                          bindings: ref BindingSet): int
{
    for (cons := constraints; cons != nil; cons = tl cons) {
        constraint := hd cons;
        
        # Get bound value for constraint's variable
        atom := bindings.get(constraint.variable.name);
        
        if (!constraint.check(atom))
            return 0;
    }
    return 1;
}

Matcher.get_candidates(m: self ref Matcher, clause: ref Clause): list of ref Atom
{
    # Get all atoms of the clause's link type from AtomSpace
    candidates: list of ref Atom;
    
    if (m.space == nil)
        return candidates;
    
    # Get atoms by type
    for (atoms := m.space.atoms; atoms != nil; atoms = tl atoms) {
        atom := hd atoms;
        if (atom.atype == clause.link_type)
            candidates = append(candidates, atom);
    }
    
    # Sort by attention value if enabled
    if (m.use_attention && candidates != nil) {
        candidates = sort_by_attention(candidates);
    }
    
    return candidates;
}

sort_by_attention(atoms: list of ref Atom): list of ref Atom
{
    # Simple bubble sort by STI (descending)
    # In production, use more efficient sorting
    
    arr: array of ref Atom;
    count := 0;
    for (a := atoms; a != nil; a = tl a)
        count++;
    
    arr = array[count] of ref Atom;
    i := 0;
    for (a := atoms; a != nil; a = tl a) {
        arr[i] = hd a;
        i++;
    }
    
    # Bubble sort
    for (i = 0; i < count - 1; i++) {
        for (j := 0; j < count - i - 1; j++) {
            if (arr[j].av.sti < arr[j+1].av.sti) {
                tmp := arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
    
    # Convert back to list
    result: list of ref Atom;
    for (i = count - 1; i >= 0; i--)
        result = arr[i] :: result;
    
    return result;
}

# =============================================================================
# QUERY EXECUTOR IMPLEMENTATION
# =============================================================================

QueryExecutor.new(space: ref Space): ref QueryExecutor
{
    qe := ref QueryExecutor;
    qe.compiler = Compiler.new();
    qe.matcher = Matcher.new(space);
    qe.cache = nil;
    return qe;
}

QueryExecutor.execute(qe: self ref QueryExecutor, query: string): ref MatchResult
{
    # Check cache
    if (qe.cache != nil) {
        cached := qe.cache.get(query);
        if (cached != nil)
            return cached;
    }
    
    # Compile pattern
    pattern := qe.compiler.compile(query);
    if (pattern == nil)
        return nil;
    
    # Execute match
    result := qe.matcher.match(pattern);
    
    # Cache result
    if (qe.cache != nil)
        qe.cache.put(query, result);
    
    return result;
}

QueryExecutor.execute_pattern(qe: self ref QueryExecutor, pattern: ref Pattern): ref MatchResult
{
    return qe.matcher.match(pattern);
}

QueryExecutor.explain(qe: self ref QueryExecutor, query: string): string
{
    pattern := qe.compiler.compile(query);
    if (pattern == nil)
        return "Failed to compile query";
    
    s := "Query Execution Plan:\n";
    s += "=====================\n\n";
    s += "Pattern:\n" + pattern.to_string() + "\n\n";
    s += sys->sprint("Estimated Cost: %d\n", qe.compiler.estimate_cost(pattern));
    s += sys->sprint("Variables: %d\n", len pattern.variables);
    s += sys->sprint("Clauses: %d\n", len pattern.clauses);
    s += sys->sprint("Constraints: %d\n", len pattern.constraints);
    s += sys->sprint("Mode: %s\n", pattern.conjunction ? "Conjunction (AND)" : "Disjunction (OR)");
    
    return s;
}

QueryExecutor.enable_cache(qe: self ref QueryExecutor, size: int): ref QueryExecutor
{
    qe.cache = QueryCache.new(size);
    return qe;
}

# =============================================================================
# QUERY CACHE IMPLEMENTATION
# =============================================================================

QueryCache.new(size: int): ref QueryCache
{
    qc := ref QueryCache;
    qc.entries = nil;
    qc.max_size = size;
    qc.hits = 0;
    qc.misses = 0;
    return qc;
}

QueryCache.get(qc: self ref QueryCache, key: string): ref MatchResult
{
    for (entries := qc.entries; entries != nil; entries = tl entries) {
        entry := hd entries;
        if (entry.key == key) {
            entry.access_count++;
            qc.hits++;
            return entry.result;
        }
    }
    qc.misses++;
    return nil;
}

QueryCache.put(qc: self ref QueryCache, key: string, result: ref MatchResult)
{
    # Check if already exists
    for (entries := qc.entries; entries != nil; entries = tl entries) {
        entry := hd entries;
        if (entry.key == key) {
            entry.result = result;
            return;
        }
    }
    
    # Create new entry
    entry := ref CacheEntry;
    entry.key = key;
    entry.result = result;
    entry.timestamp = 0;  # Would use sys->millisec() in real implementation
    entry.access_count = 1;
    
    qc.entries = append(qc.entries, entry);
    
    # Evict if over capacity (LRU)
    count := 0;
    for (e := qc.entries; e != nil; e = tl e)
        count++;
    
    if (count > qc.max_size) {
        # Remove least recently used (first entry for simplicity)
        if (qc.entries != nil)
            qc.entries = tl qc.entries;
    }
}

QueryCache.invalidate(qc: self ref QueryCache)
{
    qc.entries = nil;
}

QueryCache.stats(qc: self ref QueryCache): string
{
    total := qc.hits + qc.misses;
    hit_rate := 0.0;
    if (total > 0)
        hit_rate = real qc.hits / real total * 100.0;
    
    count := 0;
    for (e := qc.entries; e != nil; e = tl e)
        count++;
    
    return sys->sprint("Cache Stats: %d entries, %d hits, %d misses, %.1f%% hit rate",
                       count, qc.hits, qc.misses, hit_rate);
}

# =============================================================================
# MODULE INTERFACE FUNCTIONS
# =============================================================================

compile(source: string): ref Pattern
{
    compiler := Compiler.new();
    return compiler.compile(source);
}

match(pattern: ref Pattern, space: ref Space): ref MatchResult
{
    matcher := Matcher.new(space);
    return matcher.match(pattern);
}

query(source: string, space: ref Space): ref MatchResult
{
    qe := QueryExecutor.new(space);
    return qe.execute(source);
}

create_variable(name: string): ref Variable
{
    return Variable.new(name);
}

create_clause(link_type: string, args: list of ref Term): ref Clause
{
    clause := Clause.new(link_type);
    for (a := args; a != nil; a = tl a)
        clause.add_arg(hd a);
    return clause;
}

create_pattern(name: string, clauses: list of ref Clause): ref Pattern
{
    pattern := Pattern.new(name);
    for (c := clauses; c != nil; c = tl c)
        pattern.add_clause(hd c);
    return pattern;
}

# =============================================================================
# HELPER FUNCTIONS
# =============================================================================

append[T](l: list of T, item: T): list of T
{
    if (l == nil)
        return item :: nil;
    return hd l :: append(tl l, item);
}
