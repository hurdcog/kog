# libatom_types.so

**Source:** `atomspace/opencog/atoms/atom_types/`  
**Role:** Atom-type registry and name server — the single authoritative mapping between integer type codes and human-readable type names throughout AtomSpace.

## What it does

Every `Atom` in the AtomSpace is tagged with an integer `Type`.  This library owns the global `NameServer` singleton that registers new types at module load time, answers type-hierarchy queries (is X a subtype of Y?), and fires signals when new types are added.  The two compiled objects correspond to generated and hand-written parts of this registration mechanism.

## Object files

| Object | Purpose |
|--------|---------|
| `NameServer.cc.o` | `NameServer` singleton — type registration, hierarchy traversal, `TypeSignal` |
| `atom_types_init.cc.o` | Auto-generated initialiser that registers all built-in atom types at startup |

## Key public headers

- `opencog/atoms/atom_types/NameServer.h`
- `opencog/atoms/atom_types/types.h`
- `opencog/atoms/atom_types/atom_types.h`
