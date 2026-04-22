> # OpenCog Collection (OCC)
> ### An Integrated Architecture for Cognitive Synergy

**The OpenCog Collection (OCC) is a monorepo that integrates multiple OpenCog components and external tools into a unified cognitive architecture.** Our primary goal is to foster **cognitive synergy**, where the interaction of diverse AI components leads to emergent intelligence and capabilities beyond the sum of their individual parts.

This repository provides a complete, FSF-endorsed, and reproducible environment for research and development in Artificial General Intelligence (AGI), cognitive computing, and hypergraph-based AI.

---

## Key Features

- **Unified Cognitive Architecture**: A coherent system that brings together core OpenCog components like the **AtomSpace** (a hypergraph database) with advanced AI tools.
- **Cognitive Synergy by Design**: The architecture is explicitly designed to facilitate the interaction and collaboration of different AI paradigms, including symbolic reasoning, machine learning, and evolutionary algorithms.
- **Autogenesis - Self-Evolution**: AI-powered system that identifies novel features in the "adjacent possible" - capabilities that can emerge from current components through synergy and integration.
- **Cross-Modal Cognitive Fusion**: Unified cognitive loops integrating reasoning (PLN), learning (MOSES), and memory (AtomSpace) with bidirectional feedback and meta-learning capabilities.
- **Distributed Cognitive Shard Network**: Network of specialized cognitive processing units with shared memory, enabling parallel distributed cognition.
- **Reproducible Development Environment**: Utilizes **GNU Guix** and a devcontainer to provide a fully declarative and isolated development environment, ensuring that all developers have a consistent setup.
- **Extensible and Modular**: The OCC is designed to be easily extended with new components and integrations, allowing researchers to experiment with different cognitive architectures.
- **Focus on AGI Research**: The ultimate goal of the OCC is to provide a platform for building and experimenting with AGI systems.

---

## Getting Started

We recommend using the provided devcontainer for the best experience. This will automatically set up a complete development environment with all the necessary dependencies.

1.  **Clone the repository:**

    ```bash
    git clone --recurse-submodules https://github.com/Kaw-Aii/occ.git
    ```

2.  **Open in a devcontainer-compatible IDE (like VS Code).**

3.  **Start building and experimenting!**

For more detailed instructions, see:
- **[Quick Start Guide](QUICKSTART.md)** - Fast build options and troubleshooting
- **[Getting Started Guide](docs/getting-started.md)** - Complete setup walkthrough
- **[Build Status](BUILD_STATUS.md)** - Current build health and component status

### System Dependencies

The OCC requires several system dependencies to build successfully. On Ubuntu/Debian systems, install the following:

```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  cmake \
  libboost-all-dev \
  guile-3.0-dev \
  python3-dev \
  cython3 \
  liboctomap-dev \
  liboctomap-tools
```

**Required Dependencies:**
- **Octomap** (`liboctomap-dev`, `liboctomap-tools`): Required for the SpaceTime component, which provides 3D spatiotemporal object tracking and representation capabilities.
- **Boost** (`libboost-all-dev`): C++ libraries providing utilities for threading, filesystem operations, and data structures.
- **Guile 3.0** (`guile-3.0-dev`): Scheme interpreter for AtomSpace scripting and cognitive reasoning.
- **Python 3 & Cython** (`python3-dev`, `cython3`): Python bindings for C++ components and high-level API access.
- **CMake** (`cmake`): Cross-platform build system generator.
- **Build Essentials** (`build-essential`): Compiler toolchain (gcc, g++, make) for building C++ components.

---

## Documentation

### Core Documentation
- **[Architecture Overview](docs/architecture.md)**: A detailed look at the OCC architecture and its components.
- **[Cognitive Synergy](docs/cognitive-synergy.md)**: An explanation of the principles of cognitive synergy and how they are applied in the OCC.
- **[Autogenesis](docs/autogenesis.md)**: AI-powered novel feature generation based on the adjacent possible.
- **[Contributing Guide](CONTRIBUTING.md)**: How to contribute to the OpenCog Collection.

### AGI-OS / AGI-Kern Evaluation
- **[AGI Kernel Evaluation](docs/AGI_KERNEL_EVALUATION.md)**: Comprehensive evaluation of OCC against traditional OS kernel primitives for AGI-OS development.
- **[AGI-OS Integration Guide](docs/AGI_OS_INTEGRATION_GUIDE.md)**: Technical specifications for integrating 5 OpenCog repositories into a unified AGI Operating System.
- **[AGI-Kern Coverage Summary](docs/AGI_KERN_COVERAGE_SUMMARY.md)**: Quick reference guide for kernel feature coverage and integration roadmap.

### Build & Infrastructure
- **[Guix SSR Implementation](docs/guix-ssr-implementation.md)**: SSR-compliant GNU Guix build system with local installer mirroring for reproducible, reliable CI/CD.

---

## Community and Support

The OpenCog project is a community-driven effort. We welcome contributions from researchers, developers, and anyone interested in AGI.

- **Discussions:** [GitHub Discussions](https://github.com/opencog/occ/discussions)
- **Mailing List:** [OpenCog Google Group](https://groups.google.com/g/opencog)

---

*The OpenCog Collection is a continuation of the OpenCog project, pioneered by Dr. Ben Goertzel.*

## Cognitive Synergy Enhancements

The OCC implements several cognitive synergy enhancements based on the autogenesis roadmap:

### 1. Cross-Modal Cognitive Fusion (✅ Implemented)

Integrates reasoning, learning, and memory into unified cognitive loops:
- Shared representation layer in AtomSpace
- PLN → MOSES feedback loop (reasoning guides learning)
- MOSES → PLN feedback loop (learning enriches reasoning)
- Meta-learning system that evolves cognitive strategies
- **See:** `docs/cross-modal-cognitive-fusion.md`

### 2. Distributed Cognitive Shard Network (✅ Implemented)

Network of specialized cognitive shards with shared memory:
- 9 shard specializations (reasoning, learning, pattern mining, etc.)
- Thread-safe shared AtomSpace view
- Inter-shard communication via message bus
- Dynamic task allocation and load balancing
- **See:** `docs/distributed-cognitive-shard-network.md`

### 3. Attention-Guided Evolutionary Learning (✅ Implemented)

Uses attention allocation to guide MOSES learning:
- Extract high-STI atoms from attention system
- Convert attention signals to MOSES fitness bonuses
- Feedback loop from learning outcomes to attention
- **See:** `docs/attention-guided-learning.md`

### Unified Synergy Check

Run the following script to build all components, and perform mock interoperability and demo tests:

```bash
./synergy.sh
```

This script ensures that the entire collection is a coherent whole, a key principle for achieving cognitive synergy.

