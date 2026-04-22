# libexecution.so

**Source:** `atomspace/opencog/atoms/execution/`  
**Role:** Execution engine atoms — the interpreter that evaluates and executes Atomese expressions, driving grounded calls and truth-value computation.

## What it does

The *execution layer* is what turns passive atom trees into running computations.  `Instantiator` is the central evaluator: it walks an Atomese expression, applies substitutions, calls grounded procedures, and returns result atoms.  `EvaluationLink` and `ExecutionOutputLink` are the two main entry points exposed to higher-level code.

## Object files

| Object | Purpose |
|--------|---------|
| `Instantiator.cc.o` | `Instantiator` — recursive Atomese interpreter; handles grounded calls, beta-reduction, and truth-value computation |
| `EvaluationLink.cc.o` | `EvaluationLink` — evaluates a predicate and returns a truth value |
| `ExecutionOutputLink.cc.o` | `ExecutionOutputLink` — executes a schema and returns the output atom |
| `Force.cc.o` | `ForcedLink` — forces evaluation of a lazy atom |

## Key public headers

- `opencog/atoms/execution/Instantiator.h`
- `opencog/atoms/execution/EvaluationLink.h`
- `opencog/atoms/execution/ExecutionOutputLink.h`
