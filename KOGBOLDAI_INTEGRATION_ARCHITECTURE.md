# KogboldAI Kernel: AGI-OS Integration Architecture

**Version**: 1.0  
**Date**: December 6, 2025  
**Status**: Design Specification

## 1. Introduction

This document provides the detailed integration architecture for incorporating the **KogboldAI Kernel** into the AGI-OS. The integration is designed to be modular, performant, and synergistic, leveraging the strengths of both the KogboldAI Kernel and the existing OpenCog components.

## 2. Architectural Placement and Rationale

The KogboldAI Kernel will be integrated as a **Layer 8 Specialized System**, similar to `vision`, `moses`, and `agi-bio`. This placement is chosen because:

- It is a specialized cognitive module with a well-defined function (narrative generation).
- It has its own performance characteristics and dependencies (GGML).
- It can be exposed as a high-level service to other AGI-OS components.

## 3. New Debian Packages

Two new Debian packages will be created to manage the integration:

### 3.1. `kogboldai-kernel`

- **Purpose**: Provides the core KogboldAI Kernel library, headers, and Python FFI bindings.
- **Layer**: 8
- **Dependencies**: `libcogutil-dev`, `libggml-dev`, `build-essential`, `cmake`
- **Contents**:
  - `/usr/lib/libkoboldkern.so`
  - `/usr/include/kobold_kernel.h`
  - `/usr/lib/python3/dist-packages/kobold_kernel/`

### 3.2. `kogboldai-atomspace-bridge`

- **Purpose**: Provides the integration logic between the KogboldAI Kernel and the OpenCog AtomSpace.
- **Layer**: 8
- **Dependencies**: `kogboldai-kernel`, `opencog-atomspace`, `python3-opencog`
- **Contents**:
  - Python modules for story-to-atom conversion
  - PLN rules for narrative reasoning
  - Attention synchronization service

## 4. Integration Architecture Diagram

```
+--------------------------------------------------------------------+
|                            AGI-OS Application (Layer 9+)             |
+--------------------------------------------------------------------+
|                                 ▲                                    |
|                                 │ (Python API)                       |
|                                 ▼                                    |
+--------------------------------------------------------------------+
|                    kogboldai-atomspace-bridge (Layer 8)              |
|  +-------------------------+      +------------------------------+   |
|  | Story-to-Atom Converter |----->|      AtomSpace (Layer 2)     |   |
|  +-------------------------+      +------------------------------+   |
|  | Narrative Reasoner (PLN)|<-----|                              |   |
|  +-------------------------+      +------------------------------+   |
|  | Attention Sync Service  |      |                              |   |
|  +-------------------------+      +------------------------------+   |
|              │ (Python FFI)                                          |
|              ▼                                                       |
+--------------------------------------------------------------------+
|                         kogboldai-kernel (Layer 8)                   |
|  +-------------------------+      +------------------------------+   |
|  | Story Management        |      |      GGML Backend            |   |
|  +-------------------------+      +------------------------------+   |
|  | Token Sampling          |      |                              |   |
|  +-------------------------+      +------------------------------+   |
|  | World Info              |      |                              |   |
|  +-------------------------+      +------------------------------+   |
+--------------------------------------------------------------------+
```

## 5. Detailed Component Design

### 5.1. `kogboldai-kernel` Package

This package will be a straightforward adaptation of the existing `kernel` directory from the KogboldAI repository. The `CMakeLists.txt` will be used to build the shared library and Python bindings. A new `debian` directory will be created with the necessary control files.

### 5.2. `kogboldai-atomspace-bridge` Package

This package will be a new Python project with the following structure:

```
kogboldai_atomspace_bridge/
├── __init__.py
├── converter.py      # Story-to-Atom conversion logic
├── reasoner.py       # PLN narrative reasoning rules
└── attention.py      # Attention synchronization service
```

#### 5.2.1. `converter.py`

```python
class StoryAtomConverter:
    def __init__(self, atomspace):
        self.atomspace = atomspace

    def story_to_atoms(self, story_handle):
        # ... logic to convert story chunks to ConceptNodes and ListLinks ...
        story_node = self.atomspace.add_node(types.ConceptNode, "Story-{}".format(story_handle))
        return story_node

    def atoms_to_story(self, story_node):
        # ... logic to reconstruct a story from AtomSpace ...
        pass
```

#### 5.2.2. `reasoner.py`

```python
class NarrativeReasoner:
    def __init__(self, atomspace):
        self.atomspace = atomspace

    def evaluate_coherence(self, story_node):
        # Define and execute PLN rules to check for narrative coherence
        # e.g., check for contradictions, plot holes, character consistency
        rules = "(BindLink (VariableNode \"$X\") (Evaluation (PredicateNode \"is_coherent\") (VariableNode \"$X\")))"
        # ...
```

## 6. Build System Updates

### 6.1. New `kogboldai-kernel` Directory

A new directory `opencog-debian/kogboldai-kernel` will be created in the `occ` repository. This will contain the `debian` directory and an `update-kogboldai-kernel.sh` script.

### 6.2. `update-kogboldai-kernel.sh`

This script will be responsible for:
1. Cloning the `cogpy/KogboldAI` repository.
2. Checking out the `united` branch.
3. Copying the `kernel` directory into the build environment.
4. Running CMake and building the library.
5. Creating the source tarball for the Debian package.

### 6.3. `BUILD_ORDER_ENHANCED.md` Update

The `kogboldai-kernel` package will be added to **Stage 8** of the build order. The `kogboldai-atomspace-bridge` will be added to **Stage 9**.

## 7. Conclusion

This integration architecture provides a clear and modular path for incorporating the KogboldAI Kernel into the AGI-OS. By creating two new dedicated packages, we maintain a clean separation of concerns and ensure that the integration is both robust and maintainable. This approach allows the AGI-OS to leverage the high-performance narrative generation capabilities of the KogboldAI Kernel while enriching it with the advanced reasoning and knowledge representation of OpenCog.
