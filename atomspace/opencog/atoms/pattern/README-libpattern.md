# libpattern.so

**Source:** `atomspace/opencog/atoms/pattern/`  
**Role:** Pattern-matching atom types — the high-level Atomese atoms that wrap and launch pattern-matching queries against the AtomSpace.

## What it does

These atoms are the user-facing query interface.  They bundle a variable declaration, a body pattern, and a rewrite formula, then delegate to `libquery-engine.so` for actual graph traversal.  Different atom types expose different query semantics: satisfaction tests, set retrieval, binding lists, rule application, etc.

## Object files

| Object | Purpose |
|--------|---------|
| `PatternLink.cc.o` | `PatternLink` — abstract base; parses variable declarations and body |
| `Pattern.cc.o` | `Pattern` — compiled internal representation of a pattern (clause sets, bound/free vars) |
| `PatternTerm.cc.o` | `PatternTerm` — single clause within a compiled pattern |
| `PatternUtils.cc.o` | Utilities: collect clauses, detect groundedness, simplify patterns |
| `PatternJit.cc.o` | `PatternJit` — JIT-style caching / specialisation of frequently used patterns |
| `SatisfactionLink.cc.o` | `SatisfactionLink` — returns `TrueTV` / `FalseTV` if a pattern is satisfied |
| `GetLink.cc.o` | `GetLink` — returns the set of all groundings of a pattern |
| `MeetLink.cc.o` | `MeetLink` — intersection / conjunction of multiple patterns |
| `BindLink.cc.o` | `BindLink` — for each grounding, applies a rewrite and collects results |
| `QueryLink.cc.o` | `QueryLink` — generalised query combining `GetLink` and `BindLink` semantics |
| `DualLink.cc.o` | `DualLink` — dual / inverse of a pattern (swap body and conclusion) |

## Key public headers

- `opencog/atoms/pattern/PatternLink.h`
- `opencog/atoms/pattern/BindLink.h`
- `opencog/atoms/pattern/GetLink.h`
