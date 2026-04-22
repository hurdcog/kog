# libparallel.so

**Source:** `atomspace/opencog/atoms/parallel/`  
**Role:** Concurrency atom types — atoms that execute sub-expressions in parallel threads and synchronise their results inside the AtomSpace.

## What it does

Provides Atomese primitives for concurrent evaluation: spawning threads, sleeping, joining, and controlling execution.  Used by the AGI pipeline to parallelise cognitive processes without leaving the symbolic reasoning layer.

## Object files

| Object | Purpose |
|--------|---------|
| `ParallelLink.cc.o` | `ParallelLink` — evaluates each outgoing atom in a separate thread; does not join |
| `ExecuteThreadedLink.cc.o` | `ExecuteThreadedLink` — executes a thunk in a new thread and returns a future handle |
| `ThreadJoinLink.cc.o` | `ThreadJoinLink` — blocks until all threaded children have completed |
| `SleepLink.cc.o` | `SleepLink` — suspends evaluation for a duration given as a NumberNode |
| `DontExecLink.cc.o` | `DontExecLink` — prevents a nested atom from being executed/evaluated |
| `PureExecLink.cc.o` | `PureExecLink` — executes only pure (side-effect-free) atoms |
| `DefinedProcedureNode.cc.o` | `DefinedProcedureNode` — named callable procedure defined in the AtomSpace |

## Key public headers

- `opencog/atoms/parallel/ParallelLink.h`
- `opencog/atoms/parallel/ExecuteThreadedLink.h`
- `opencog/atoms/parallel/SleepLink.h`
