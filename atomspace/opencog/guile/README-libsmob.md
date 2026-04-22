# libsmob.so

**Source:** `atomspace/opencog/guile/`  
**Role:** Guile Scheme bindings for the AtomSpace — exposes the full AtomSpace C++ API to interactive Scheme sessions and `.scm` module files.

## What it does

Implements Guile *smobs* (small objects) that wrap `Handle`, `AtomSpace`, `TruthValue`, and `Value` C++ objects so they can be manipulated from Scheme.  Also provides `SchemeEval`, the C++ class that runs a Guile interpreter with full AtomSpace access, used by the CogServer's Scheme shell.

## Object files

| Object | Purpose |
|--------|---------|
| `SchemeEval.cc.o` | `SchemeEval` — evaluates Scheme strings/files in an AtomSpace context |
| `SchemeModule.cc.o` | `SchemeModule` — loads OpenCog Guile modules into the interpreter |
| `SchemePrimitive.cc.o` | `SchemePrimitive` — registers C++ functions as callable Scheme primitives |
| `SchemeSmob.cc.o` | Smob type registration and GC hooks for all OpenCog smob types |
| `SchemeSmobAtom.cc.o` | Atom smob: `cog-atom`, type predicates, outgoing/incoming set accessors |
| `SchemeSmobAS.cc.o` | AtomSpace smob: `cog-atomspace`, `cog-set-atomspace!`, frame management |
| `SchemeSmobGC.cc.o` | Garbage-collection finalisers for smob objects |
| `SchemeSmobNew.cc.o` | `cog-new-*` constructors: creates atoms from Scheme |
| `SchemeSmobPrint.cc.o` | Scheme `display`/`write` output for smob objects |
| `SchemeSmobTV.cc.o` | TruthValue smob: `cog-tv`, `cog-stv`, `cog-ctv` |
| `SchemeSmobValue.cc.o` | Value smob: `cog-value`, `cog-float-value`, `cog-string-value` |
| `SchemeSmobLogger.cc.o` | `cog-logger-*` Scheme interface to `Logger` |

## Key public headers

- `opencog/guile/SchemeEval.h`
- `opencog/guile/SchemePrimitive.h`
- `opencog/guile/SchemeModule.h`
