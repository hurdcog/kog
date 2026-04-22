# libgrounded.so

**Source:** `atomspace/opencog/atoms/grounded/`  
**Role:** Grounded atoms — atoms whose evaluation delegates to external code (Scheme procedures, Python callables, or dynamically loaded libraries).

## What it does

Provides the *grounding* mechanism that connects Atomese's symbolic layer to executable code outside the AtomSpace.  A `GroundedSchemaNode` or `GroundedPredicateNode` names a function; when evaluated, the runtime looks it up in the registered back-ends and calls it with the atom context.  The `LibraryManager` handles `.so` back-ends loaded via `dlopen`.

## Object files

| Object | Purpose |
|--------|---------|
| `GroundedSchemaNode.cc.o` | `GroundedSchemaNode` — names a schema (function returning an atom) |
| `GroundedPredicateNode.cc.o` | `GroundedPredicateNode` — names a predicate (function returning a truth value) |
| `LibraryManager.cc.o` | `LibraryManager` — `dlopen`/`dlsym` management for `.so` grounding libraries |
| `LibraryRunner.cc.o` | `LibraryRunner` — invokes a C-linkage grounding function from a loaded library |
| `SCMRunner.cc.o` | `SCMRunner` — invokes a Guile Scheme procedure as a grounding function |
| `DLScheme.cc.o` | `DLScheme` — dynamically loaded Scheme-extension helper |

## Key public headers

- `opencog/atoms/grounded/GroundedSchemaNode.h`
- `opencog/atoms/grounded/GroundedPredicateNode.h`
- `opencog/atoms/grounded/LibraryManager.h`
