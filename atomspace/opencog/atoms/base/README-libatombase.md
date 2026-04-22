# libatombase.so

**Source:** `atomspace/opencog/atoms/base/`  
**Role:** Core atom object model — the `Atom`, `Node`, `Link`, and `Handle` classes that every higher-level AtomSpace library builds on.

## What it does

Defines the fundamental building blocks of the hypergraph:

- **`Atom`** is the abstract base for all knowledge-representation objects.
- **`Node`** is a leaf atom identified by a name string; represents concepts.
- **`Link`** is a composite atom that holds an ordered list of outgoing `Handle`s; represents relationships.
- **`Handle`** is a smart pointer / reference to an Atom; the currency passed between all AtomSpace APIs.
- **`ClassServer`** (thin wrapper around `NameServer`) resolves type codes for newly loaded shared libraries.

## Object files

| Object | Purpose |
|--------|---------|
| `Atom.cc.o` | `Atom` abstract base — truth-value storage, attention value, incoming set |
| `Node.cc.o` | `Node` leaf atom — name-keyed identity |
| `Link.cc.o` | `Link` composite atom — outgoing set management |
| `Handle.cc.o` | `Handle` smart-pointer and comparison operators |
| `ClassServer.cc.o` | `ClassServer` — thin facade over `NameServer` for dynamic type loading |

## Key public headers

- `opencog/atoms/base/Atom.h`
- `opencog/atoms/base/Handle.h`
- `opencog/atoms/base/Node.h`
- `opencog/atoms/base/Link.h`
