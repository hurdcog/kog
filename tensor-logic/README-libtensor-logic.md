# libtensor-logic.so

**Source:** `tensor-logic/opencog/tensor-logic/`  
**Role:** Tensor Logic integration layer — bridges symbolic AtomSpace reasoning with neural tensor operations, enabling multi-entity management, multi-scale temporal analysis, network-aware distributed reasoning, and reservoir computing.

## What it does

`TensorLogic` is the top-level integration class that composes five specialised sub-systems:

1. **Multi-Entity AtomSpace** — tracks multiple cognitive entities, each with a tensor-valued state embedding.
2. **Multi-Scale Analysis** — decomposes and reconstructs signals across configurable temporal scales using wavelet-style decomposition.
3. **Network-Aware DAS** — records real-time atom-flow tensors across distributed AtomSpace nodes; predicts optimal routing and node placement.
4. **ESN Reservoir** — Echo State Network reservoir computing for non-linear temporal processing.
5. **TensorAtomSpace** — bridges symbolic atoms to dense float tensors for embedding-based similarity and retrieval.

## Object files

| Object | Purpose |
|--------|---------|
| `TensorLogic.cpp.o` | `TensorLogic` — top-level coordinator; exposes the unified public API |
| `MultiEntityAtomSpace.cpp.o` | `MultiEntityAtomSpace` — entity registry with tensor-state CRUD |
| `MultiScaleAnalysis.cpp.o` | `MultiScaleAnalysis` — scale-indexed decomposition / reconstruction |
| `NetworkAwareDAS.cpp.o` | `NetworkAwareDAS` — flow-record ledger, routing scoring, node placement |
| `ESNReservoir.cpp.o` | `ESNReservoir` — sparse recurrent reservoir with online training |
| `TensorAtomSpace.cpp.o` | `TensorAtomSpace` — atom ↔ tensor conversion and embedding lookup |

## Key public headers

- `tensor-logic/opencog/tensor-logic/TensorLogic.h`
- `tensor-logic/opencog/tensor-logic/NetworkAwareDAS.h`
- `tensor-logic/opencog/tensor-logic/ESNReservoir.h`
