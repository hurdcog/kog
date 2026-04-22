# libatomflow.so

**Source:** `atomspace/opencog/atoms/flow/`  
**Role:** Data-flow atom types — atoms that read, write, transform, and stream Values to and from the AtomSpace at evaluation time.

## What it does

Implements the *Value flow* layer of Atomese: atoms that act as functional operators over the Value type system.  They connect the structural (graph) layer to the numeric/streaming layer, enabling reactive data pipelines inside the AtomSpace without external code.

## Object files

| Object | Purpose |
|--------|---------|
| `ValueOfLink.cc.o` | `ValueOfLink` — reads a named Value key from an atom |
| `SetValueLink.cc.o` | `SetValueLink` — writes a Value to an atom's key |
| `IncrementValueLink.cc.o` | `IncrementValueLink` — atomic numeric increment on a FloatValue |
| `TruthValueOfLink.cc.o` | `TruthValueOfLink` — extracts an atom's truth value as a FloatValue |
| `SetTVLink.cc.o` | `SetTVLink` — sets an atom's truth value from a FloatValue |
| `StreamValueOfLink.cc.o` | `StreamValueOfLink` — reads from a streaming Value source |
| `FormulaPredicateLink.cc.o` | `FormulaPredicateLink` — evaluates a formula to produce a truth value |
| `FilterLink.cc.o` | `FilterLink` — filters a set of atoms by a predicate |
| `CollectionOfLink.cc.o` | `CollectionOfLink` — collects all atoms satisfying a pattern into a LinkValue |
| `ConcatenateLink.cc.o` | `ConcatenateLink` — concatenates two LinkValues |
| `SplitLink.cc.o` | `SplitLink` — splits a string Value on a delimiter |
| `StringOfLink.cc.o` | `StringOfLink` — converts an atom to its string representation |
| `NumberOfLink.cc.o` | `NumberOfLink` — returns the numeric value of a NumberNode |
| `SizeOfLink.cc.o` | `SizeOfLink` — returns the arity or length of an atom/value |
| `TypeOfLink.cc.o` | `TypeOfLink` — returns the type of an atom as a TypeNode |
| `IncomingOfLink.cc.o` | `IncomingOfLink` — returns the incoming set of an atom |
| `LinkSignatureLink.cc.o` | `LinkSignatureLink` — typed link signature matcher |
| `PromiseLink.cc.o` | `PromiseLink` — lazy thunk that evaluates on demand |
| `ValueShimLink.cc.o` | Shim bridging legacy value access patterns |

## Key public headers

- `opencog/atoms/flow/ValueOfLink.h`
- `opencog/atoms/flow/SetValueLink.h`
- `opencog/atoms/flow/FilterLink.h`
