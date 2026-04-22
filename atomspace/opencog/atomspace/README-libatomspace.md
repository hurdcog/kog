# libatomspace.so

**Source:** `atomspace/opencog/atomspace/`  
**Role:** The AtomSpace — the hypergraph database that stores all atoms and serves as the shared working memory of the entire OCC cognitive system.

## What it does

The AtomSpace is the central data structure of OpenCog: a concurrent, transactional hypergraph where Atoms live, relationships are expressed as Links, and every piece of knowledge is retrievable by type, name, or pattern.

- **`AtomSpace`** provides the public API — `add_atom`, `get_atom`, `remove_atom`, outgoing/incoming set access, and truth-value/value access.
- **`AtomTable`** is the internal concurrent hash table and type index that backs the AtomSpace.
- **`Frame`** implements copy-on-write *frames*: transient sub-spaces that overlay the main space for hypothetical or scoped reasoning.
- **`Transient`** is a lightweight ephemeral sub-space used during pattern-match variable binding.
- **`TypeIndex`** is the per-type hash bucket structure that makes typed queries O(1).

## Object files

| Object | Purpose |
|--------|---------|
| `AtomSpace.cc.o` | `AtomSpace` public API — CRUD operations, frame management, event signals |
| `AtomTable.cc.o` | `AtomTable` — concurrent storage, deduplication, incoming-set maintenance |
| `Frame.cc.o` | `Frame` — copy-on-write overlay space for scoped or hypothetical knowledge |
| `Transient.cc.o` | `Transient` — minimal ephemeral space for pattern-matcher scratch storage |
| `TypeIndex.cc.o` | `TypeIndex` — per-type hash bucket for O(1) typed atom retrieval |

## Key public headers

- `opencog/atomspace/AtomSpace.h`
- `opencog/atomspace/AtomTable.h`
- `opencog/atomspace/Frame.h`
