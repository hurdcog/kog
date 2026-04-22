# libjoin.so

**Source:** `atomspace/opencog/atoms/join/`  
**Role:** Join-pattern atom type — matches atoms against multiple independent sub-patterns and unifies the results.

## What it does

Implements `JoinLink`, which performs a relational *join* across multiple sub-patterns within a single query.  This allows queries to express conjunctive constraints that span disconnected parts of the hypergraph, complementing the sequential matching in `libpattern.so`.

## Object files

| Object | Purpose |
|--------|---------|
| `JoinLink.cc.o` | `JoinLink` — multi-arm conjunctive pattern join |

## Key public headers

- `opencog/atoms/join/JoinLink.h`
