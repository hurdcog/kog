# Guile module shared libraries

**Source:** `atomspace/opencog/guile/modules/`  
**Role:** Thin Guile Scheme extension modules — each exposes one domain of C++ functionality as a first-class Scheme module loadable with `(use-modules (opencog ...))`.

---

## libexec.so

Exposes the Atomese execution layer to Scheme.

| Object | Purpose |
|--------|---------|
| `ExecSCM.cc.o` | Registers `cog-execute!`, `cog-evaluate!` in Guile |

---

## liblogger.so

Exposes `cogutil`'s `Logger` to Scheme.

| Object | Purpose |
|--------|---------|
| `LoggerSCM.cc.o` | Registers `cog-logger-set-level!`, `cog-logger-info`, etc. in Guile |

---

## librandgen.so

Exposes the random-number generator to Scheme.

| Object | Purpose |
|--------|---------|
| `RandGenSCM.cc.o` | Registers `cog-randgen-set-seed!` and related primitives in Guile |

---

## libtype-utils.so

Exposes type-system utilities to Scheme.

| Object | Purpose |
|--------|---------|
| `TypeUtilsSCM.cc.o` | Registers `cog-type->int`, `cog-int->type`, `cog-subtype?` in Guile |
