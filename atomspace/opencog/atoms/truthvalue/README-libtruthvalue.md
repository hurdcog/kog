# libtruthvalue.so

**Source:** `atomspace/opencog/atoms/truthvalue/`  
**Role:** Truth-value representations — probabilistic confidence annotations on atoms that drive PLN inference and attention allocation.

## What it does

Every atom carries a *truth value* that encodes the system's degree of belief.  This library implements the concrete truth-value types used across OCC: from the simple `(strength, confidence)` pair to formula-driven and count-based variants.

## Object files

| Object | Purpose |
|--------|---------|
| `TruthValue.cc.o` | `TruthValue` abstract base — `getMean()`, `getConfidence()`, serialisation |
| `SimpleTruthValue.cc.o` | `SimpleTruthValue` — `(strength ∈ [0,1], confidence ∈ [0,1])` pair |
| `CountTruthValue.cc.o` | `CountTruthValue` — derived from an observation count; confidence = count/(count+k) |
| `FormulaTruthValue.cc.o` | `FormulaTruthValue` — truth values computed dynamically by an Atomese formula |

## Key public headers

- `opencog/atoms/truthvalue/TruthValue.h`
- `opencog/atoms/truthvalue/SimpleTruthValue.h`
- `opencog/atoms/truthvalue/CountTruthValue.h`
