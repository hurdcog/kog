# libatomcore.so

**Source:** `atomspace/opencog/atoms/core/`  
**Role:** Core atom types — the essential building blocks of Atomese: variables, scoped forms, lambdas, type constraints, and rewrite utilities.

## What it does

Provides the workhorse atom types for representing logical and procedural knowledge in Atomese.  Most higher-level reasoning (PLN, URE, pattern matching) operates on the atom types defined here.

## Object files

| Object | Purpose |
|--------|---------|
| `AbsentLink.cc.o` | `AbsentLink` — asserts that an atom is *not* present in the AtomSpace |
| `Checkers.cc.o` | Runtime type-safety checkers for atom argument lists |
| `CondLink.cc.o` | `CondLink` — conditional branching in Atomese |
| `Context.cc.o` | `Context` — evaluation context (AtomSpace + substitution map) |
| `DefineLink.cc.o` | `DefineLink` — binds a name to an atom expression |
| `DeleteLink.cc.o` | `DeleteLink` — removes atoms from the AtomSpace at evaluation time |
| `FindUtils.cc.o` | Graph traversal helpers: find free/bound variables, collect atoms |
| `FreeLink.cc.o` | `FreeLink` — free-variable extraction |
| `FreeVariables.cc.o` | `FreeVariables` — ordered set of free variables with type constraints |
| `FunctionLink.cc.o` | `FunctionLink` — abstract base for executable link types |
| `GrantLink.cc.o` | `GrantLink` — permission/capability atom |
| `LambdaLink.cc.o` | `LambdaLink` — anonymous function over atom variables |
| `NumberNode.cc.o` | `NumberNode` — leaf atom storing a double |
| `PrenexLink.cc.o` | `PrenexLink` — pulls quantifiers to outermost scope |
| `PresentLink.cc.o` | `PresentLink` — asserts atom presence before evaluation |
| `PutLink.cc.o` | `PutLink` — beta-reduction / variable substitution |
| `Quotation.cc.o` | `QuoteLink` / `UnquoteLink` — suppress/restore evaluation |
| `RandomChoice.cc.o` | `RandomChoiceLink` — stochastic selection at evaluation time |
| `Replacement.cc.o` | Variable-substitution utilities used by rewrite rules |
| `RewriteLink.cc.o` | `RewriteLink` — abstract base for pattern-rewrite atoms |
| `ScopeLink.cc.o` | `ScopeLink` — scoped quantification (∀ / ∃ base) |
| `StateLink.cc.o` | `StateLink` — single-assignment mutable state cell |
| `TimeLink.cc.o` | `TimeLink` — time-stamped atom wrapper |
| `TypeChoice.cc.o` | `TypeChoiceLink` — disjunctive type constraint |
| `TypedAtomLink.cc.o` | `TypedAtomLink` — attaches a type signature to an atom |
| `TypedVariableLink.cc.o` | `TypedVariableLink` — variable with type annotation |
| `TypeIntersectionLink.cc.o` | `TypeIntersectionLink` — conjunctive type constraint |
| `TypeNode.cc.o` | `TypeNode` — atom whose name is a type name |
| `TypeUtils.cc.o` | Type-checking / unification utilities |
| `UniqueLink.cc.o` | `UniqueLink` — enforces uniqueness of a key→value mapping |
| `UnorderedLink.cc.o` | `UnorderedLink` — set-semantics link (order-independent) |
| `Variables.cc.o` | `Variables` class — typed variable declaration list |
| `VariableList.cc.o` | `VariableListLink` — ordered list of declared variables |
| `VariableSet.cc.o` | `VariableSetLink` — unordered set of declared variables |

## Key public headers

- `opencog/atoms/core/Variables.h`
- `opencog/atoms/core/LambdaLink.h`
- `opencog/atoms/core/TypedVariableLink.h`
- `opencog/atoms/core/RewriteLink.h`
