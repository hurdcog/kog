# libatomspace_accelerator.so

**Source:** `atomspace-accelerator/src/`  
**Role:** AtomSpace inference accelerator — a high-performance layer that caches, indexes, and speculatively computes inference results to reduce latency in hot reasoning paths.

## What it does

Sits between the pattern-matcher and the upper AGI layers as an optional acceleration layer.  It profiles query patterns, caches partial results, and implements a speculative pattern-miner that pre-computes likely-needed inference products before they are explicitly requested.

## Object files

| Object | Purpose |
|--------|---------|
| `accelerator.cpp.o` | `Accelerator` — main entry point; manages the cache and coordinates the other modules |
| `inference_engine.cpp.o` | `InferenceEngine` — executes cached inference plans; integrates with the query engine |
| `query_optimizer.cpp.o` | `QueryOptimizer` — rewrites and reorders pattern clauses for minimum traversal cost |
| `pattern_miner.cpp.o` | `PatternMiner` — background thread that mines frequent sub-patterns and pre-caches them |

## Key public headers

- `atomspace-accelerator/include/accelerator.h`
- `atomspace-accelerator/include/inference_engine.h`
