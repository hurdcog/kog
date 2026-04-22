# librule.so

**Source:** `atomspace/opencog/atoms/rule/`  
**Role:** Rule-structure atom types — atoms that represent the anatomy of an inference rule (premises, conclusion, variable declarations).

## What it does

Provides the structural atoms used by the Unified Rule Engine (URE) and PLN to represent and introspect inference rules.  A rule is stored as an atom tree; `RuleLink` is the container, and `PremiseOfLink`/`ConclusionOfLink` are accessors.

## Object files

| Object | Purpose |
|--------|---------|
| `RuleLink.cc.o` | `RuleLink` — wraps a rule's variable declarations, premises, and conclusion |
| `PremiseOfLink.cc.o` | `PremiseOfLink` — extracts the premise(s) of a rule atom |
| `ConclusionOfLink.cc.o` | `ConclusionOfLink` — extracts the conclusion of a rule atom |
| `VardeclOfLink.cc.o` | `VardeclOfLink` — extracts the variable declaration part of a scoped atom |

## Key public headers

- `opencog/atoms/rule/RuleLink.h`
