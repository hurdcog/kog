# ORGGML Integration: A Concrete Implementation Plan

**Date**: December 9, 2025  
**Version**: 1.0  
**Author**: Manus AI

## 1. Project Goal

This document provides a concrete, step-by-step implementation plan for **Opportunity 2: Integrate ORGGML as the Cognitive Fusion Reactor**. The goal is to create a new `cognitive-fusion-reactor/` directory within the `cogpy/occ` repository and populate it with the `orggml` architecture, effectively making it the standard framework for integrating machine learning models into the OpenCog ecosystem.

## 2. Project Scope and Deliverables

*   **Scope**: This project covers the initial integration of the `orggml` structure and the `ggml` library into the OCC repository. It includes the creation of a bridge between `ASML` (the `ggml` adaptation) and the `AtomSpace`.
*   **Deliverables**:
    1.  A new `cognitive-fusion-reactor/` directory in the `occ` repository.
    2.  The `ggml` source code integrated into `cognitive-fusion-reactor/asml/`.
    3.  A new C++ bridge library, `libasml-atomspace.so`, for converting between `ASML` tensors and `AtomSpace` atoms.
    4.  A CMake build system for the new reactor.
    5.  A demonstration program that uses the bridge to run a simple `ggml` computation on data extracted from the `AtomSpace`.

## 3. Timeline and Milestones (3-Week Sprint)

This project is estimated to take three weeks.

### Week 1: Foundation and ASML Integration

*   **Day 1-2**: Create the directory structure and CMake files. Set up the `cognitive-fusion-reactor/` directory with `asml/`, `learn.cog/`, and `sensation/` subdirectories. Create the main `CMakeLists.txt` for the reactor.
*   **Day 3-4**: Integrate the `ggml` source code into `asml/`. This is not a submodule, but a direct copy of the code. Ensure that it can be built as a static library, `libasml.a`, using the new CMake system.
*   **Day 5**: Create the initial design for the `ASML-AtomSpace` bridge. Define the C++ API for the bridge.

**Milestone**: The `ggml` library can be successfully built as `libasml.a` within the `occ` repository.

### Week 2: The ASML-AtomSpace Bridge

*   **Day 6-8**: Implement the core of the bridge. This includes functions to:
    *   Convert an `AtomSpace` `NumberNode` or `TensorNode` into an `asml` tensor (`ggml_tensor`).
    *   Convert an `asml` tensor back into an `AtomSpace` `TensorNode`.
*   **Day 9-10**: Build the bridge as a shared library, `libasml-atomspace.so`. Write unit tests for the conversion functions.

**Milestone**: The `libasml-atomspace.so` library is complete and passes all unit tests.

### Week 3: Demonstration and Documentation

*   **Day 11-13**: Create a demonstration program. This program will:
    1.  Initialize the `AtomSpace`.
    2.  Create a few `NumberNodes`.
    3.  Use the bridge to convert them into `asml` tensors.
    4.  Perform a simple `ggml` computation (e.g., add them together).
    5.  Use the bridge to convert the result back into an `AtomSpace` `TensorNode`.
    6.  Print the result from the `AtomSpace`.
*   **Day 14-15**: Write comprehensive documentation for the new reactor, the `ASML` component, and the bridge. Update the main `README.md` of the `occ` repository to reflect this new component.

**Milestone**: The demonstration program runs successfully, and all documentation is complete.

## 4. Detailed Implementation Steps

### 4.1. Directory Structure

```
occ/
├── cognitive-fusion-reactor/
│   ├── asml/              # ggml source code
│   ├── learn.cog/         # Placeholder for llama.cpp
│   ├── sensation/         # Placeholder for whisper.cpp
│   ├── bridge/            # ASML-AtomSpace bridge source
│   ├── examples/          # Demonstration program
│   └── CMakeLists.txt
└── ...
```

### 4.2. The ASML-AtomSpace Bridge API (`bridge/bridge.h`)

```cpp
#include "opencog/atomspace/AtomSpace.h"
#include "ggml.h"

namespace opencog {
namespace asml {

// Converts an AtomSpace TensorNode to a ggml_tensor
// The context `ctx` must be managed by the caller.
ggml_tensor* atom_to_tensor(ggml_context* ctx, const Handle& atom);

// Converts a ggml_tensor to an AtomSpace TensorNode
Handle tensor_to_atom(AtomSpace& atomspace, const ggml_tensor* tensor);

} // namespace asml
} // namespace opencog
```

### 4.3. CMake Build System

The main `CMakeLists.txt` in the `occ` repository will be modified to add the `cognitive-fusion-reactor/` as an optional subdirectory, controlled by a `BUILD_FUSION_REACTOR` flag.

The `CMakeLists.txt` within the reactor directory will:

1.  Build `ggml` as a static library (`asml`).
2.  Build the bridge as a shared library (`asml-atomspace`), linking against `asml` and the `AtomSpace` libraries.
3.  Build the example program, linking against the bridge.

## 5. Risks and Mitigation

*   **Risk**: The `ggml` API changes frequently. **Mitigation**: For this initial integration, we will pin to a specific, stable commit of `ggml`. A future project will address continuous upstream integration.
*   **Risk**: The conversion between `AtomSpace` types and `ggml` tensors may be complex, especially for different data types and shapes. **Mitigation**: The initial implementation will focus on simple cases (scalars, 1D vectors). Support for higher-dimensional tensors will be added incrementally.

## 6. Success Criteria

The project will be considered a success when a developer can:

1.  Build the entire `occ` repository with the `BUILD_FUSION_REACTOR` flag enabled.
2.  Run the demonstration program and see the correct computational result in the `AtomSpace`.
3.  Link their own programs against `libasml-atomspace.so` to create their own `ggml`-`AtomSpace` integrations.
