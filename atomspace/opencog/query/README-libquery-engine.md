# libquery-engine.so

**Source:** `atomspace/opencog/query/`  
**Role:** Pattern-matching engine — the core graph-traversal algorithm that searches the AtomSpace for groundings of compiled `Pattern` objects.

## What it does

Implements the *Satisfier* algorithm: a depth-first, clause-by-clause hypergraph search with backtracking and continuation support.  Higher-level pattern atoms (`BindLink`, `GetLink`, etc.) compile themselves into `Pattern` objects and hand them to this engine.

The engine is decomposed into orthogonal *mixins* so that different match strategies (satisfying, rewriting, continuation-based) can be composed without code duplication.

## Object files

| Object | Purpose |
|--------|---------|
| `PatternMatchEngine.cc.o` | Core DFS traversal, clause dispatch, backtracking |
| `InitiateSearchMixin.cc.o` | Selects the starting point (cheapest clause to begin matching from) |
| `NextSearchMixin.cc.o` | Advances to the next candidate after a partial match or backtrack |
| `ContinuationMixin.cc.o` | Pauses a match in progress and resumes it later (coroutine-style) |
| `SatisfyMixin.cc.o` | Drives full satisfaction: iterates over all groundings |
| `Satisfier.cc.o` | `Satisfier` — collects all ground solutions into a result set |
| `TermMatchMixin.cc.o` | Per-clause term unification logic |
| `RewriteMixin.cc.o` | Applies the rewrite formula after a successful match |
| `Recognizer.cc.o` | `Recognizer` — variant that tests whether *any* grounding exists (yes/no) |

## Key public headers

- `opencog/query/SatisfyMixin.h`
- `opencog/query/PatternMatchEngine.h`
