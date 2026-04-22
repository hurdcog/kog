# AGI-OS Build Sequences

This document describes the build dependency sequences for all components of the AGI-OS stack.

## Table of Contents

1. [Layer 1: Cognumach Components](#layer-1-cognumach-components)
2. [Layer 2: HurdCog Components](#layer-2-hurdcog-components)
3. [Layer 3: OCC Components](#layer-3-occ-components)
4. [Complete AGI-OS Stack](#complete-agi-os-stack)
5. [Dependency Graph](#dependency-graph)

---

## Layer 1: Cognumach Components

**Build System:** GNU Autotools (wrapped in CMake)  
**Version:** 1.8.0  
**Base:** GNU Mach microkernel with cognitive extensions

### Build Sequence

1. **cognumach** (no dependencies)
   - **Location:** `cognumach/`
   - **Build Method:** Autotools → CMake wrapper
   - **Command:** 
     ```bash
     cmake -DBUILD_COGNUMACH=ON ..
     make cognumach-build
     ```
   - **Components:**
     - IPC (Inter-Process Communication)
     - VM (Virtual Memory)
     - Device drivers
     - Cognitive IPC extensions
     - Cognitive VM extensions

### Cognumach Sub-Components

```
cognumach/
├── kern/           # Kernel core
├── ipc/            # IPC mechanisms
├── vm/             # Virtual memory
├── device/         # Device drivers
├── chips/          # Platform support
├── i386/           # x86 architecture
├── x86_64/         # 64-bit port (experimental)
└── include/mach/   # Public headers
    └── cognitive/  # Cognitive extensions
```

**Build Order:**
1. Configure with autotools: `./configure`
2. Build kernel: `make`
3. Install headers and kernel image

---

## Layer 2: HurdCog Components

**Build System:** GNU Autotools + Python/Guile (wrapped in CMake)  
**Version:** 0.9.0  
**Base:** GNU Hurd with cognitive kernel extensions

### Build Sequence

1. **hurdcog** (depends on: cognumach)
   - **Location:** `hurdcog/`
   - **Build Method:** Autotools + cognitive modules
   - **Command:**
     ```bash
     cmake -DBUILD_COGNUMACH=ON -DBUILD_HURDCOG=ON ..
     make hurdcog-build
     ```

### HurdCog Sub-Components

**Core Hurd Servers (autotools):**
```
hurdcog/
├── auth/           # Authentication server
├── proc/           # Process server
├── exec/           # Execution server
├── pfinet/         # Network stack
├── ext2fs/         # Filesystem
├── boot/           # Bootstrap
└── ... (standard Hurd servers)
```

**Cognitive Extensions (Python/Guile):**
```
hurdcog/
├── cogkernel/                    # Cognitive kernel
│   ├── core/                     # Core cognitive functions
│   ├── mach-integration/         # Mach integration
│   │   └── machspace-bridge.scm # MachSpace bridge
│   ├── fusion-reactor/          # Cognitive Fusion Reactor
│   └── dashboard/               # Master Control Dashboard
├── distributed/                  # Distributed systems
│   ├── plan9/                   # Plan9 integration
│   └── inferno/                 # Inferno integration
└── cognitive/                    # Cognitive services
```

**Build Order:**
1. Build Cognumach first
2. Configure HurdCog: `./configure --with-gnumach=/path/to/cognumach`
3. Build core servers: `make`
4. Install Python dependencies: `pip install -r requirements.txt`
5. Build cognitive modules

---

## Layer 3: OCC Components

**Build System:** CMake  
**Version:** Varies by component (see version table below)  
**Base:** OpenCog Collection - AGI framework

### Core Foundation (Build First)

These components form the foundation and must be built in order:

1. **cogutil** (v2.3.1)
   - **Location:** `cogutil/`
   - **Dependencies:** None
   - **Provides:** Base utilities, configuration, logging

2. **atomspace** (v5.0.4)
   - **Location:** `atomspace/`
   - **Dependencies:** cogutil
   - **Provides:** Hypergraph knowledge representation

### Storage Backends (Build After AtomSpace)

3. **atomspace-storage** (v1.3.0)
   - **Location:** `atomspace-storage/`
   - **Dependencies:** atomspace
   - **Provides:** Storage API

4. **atomspace-rocks** (v1.3.0)
   - **Location:** `atomspace-rocks/`
   - **Dependencies:** atomspace, atomspace-storage
   - **Provides:** RocksDB backend

5. **atomspace-cog** (v0.1.4)
   - **Location:** `atomspace-cog/`
   - **Dependencies:** atomspace, atomspace-storage
   - **Provides:** Network storage via CogServer

6. **atomspace-pgres** (v1.3.0)
   - **Location:** `atomspace-pgres/`
   - **Dependencies:** atomspace, atomspace-storage
   - **Provides:** PostgreSQL backend

### Network Layer (Build After AtomSpace)

7. **cogserver** (v3.3.0)
   - **Location:** `cogserver/`
   - **Dependencies:** atomspace
   - **Provides:** Network shell, module loading

### Math & ML Foundation

8. **matrix** (v1.3.0)
   - **Location:** `matrix/`
   - **Dependencies:** atomspace
   - **Provides:** Sparse matrix operations

### Reasoning & Learning Components

9. **unify** (v0.1.7)
   - **Location:** `unify/`
   - **Dependencies:** atomspace
   - **Provides:** Unification algorithms

10. **ure** (v0.3.3)
    - **Location:** `ure/`
    - **Dependencies:** atomspace, unify
    - **Provides:** Unified Rule Engine

11. **learn** (v0.1.4)
    - **Location:** `learn/`
    - **Dependencies:** atomspace
    - **Provides:** Language learning algorithms

12. **miner** (v1.0.0)
    - **Location:** `miner/`
    - **Dependencies:** atomspace, ure
    - **Provides:** Pattern mining

13. **pln** (v0.1.1)
    - **Location:** `pln/`
    - **Dependencies:** atomspace, ure
    - **Provides:** Probabilistic Logic Networks

### Attention & Control

14. **attention** (v0.1.4)
    - **Location:** `attention/`
    - **Dependencies:** atomspace
    - **Provides:** Economic Attention Networks (ECAN)

### Agent Framework

15. **agents** (v0.1.4)
    - **Location:** `agents/`
    - **Dependencies:** atomspace
    - **Provides:** Interactive agent framework

16. **sensory** (v0.4.0)
    - **Location:** `sensory/`
    - **Dependencies:** atomspace
    - **Provides:** Dataflow system for perception

### Evolutionary Optimization

17. **asmoses** (v3.4.0)
    - **Location:** `asmoses/`
    - **Dependencies:** cogutil
    - **Provides:** Meta-Optimizing Semantic Evolutionary Search

### Cognitive Architecture Extensions

18. **coggml** (v1.0.0)
    - **Location:** `coggml/`
    - **Dependencies:** None (standalone)
    - **Provides:** Self-aware microkernel shards

19. **cogself** (v1.0.0)
    - **Location:** `cogself/`
    - **Dependencies:** coggml
    - **Provides:** AGI synergy framework

20. **atomspace-accelerator** (v1.0.0)
    - **Location:** `atomspace-accelerator/`
    - **Dependencies:** atomspace (optional)
    - **Provides:** High-performance inference engine

21. **agentic-chatbots** (v1.0.0)
    - **Location:** `agentic-chatbots/`
    - **Dependencies:** None (standalone)
    - **Provides:** Conversational AI integration

---

## Complete AGI-OS Stack

### Full Build Sequence

To build the complete AGI-OS stack from scratch:

```bash
# 1. Create build directory
mkdir build && cd build

# 2. Configure with CMake (all layers enabled)
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_COGNUMACH=ON \
  -DBUILD_HURDCOG=ON \
  -DBUILD_COGUTIL=ON \
  -DBUILD_ATOMSPACE=ON \
  -DBUILD_COGSERVER=ON \
  -DBUILD_MATRIX=ON \
  -DBUILD_LEARN=ON \
  -DBUILD_AGENTS=ON \
  -DBUILD_SENSORY=ON \
  -DBUILD_COGGML=ON \
  -DBUILD_COGSELF=ON \
  -DBUILD_ATOMSPACE_ACCELERATOR=ON \
  -DBUILD_AGENTIC_CHATBOTS=ON \
  -DBUILD_ATOMSPACE_STORAGE=ON

# 3. Build (parallel)
make -j$(nproc)

# 4. Install
sudo make install
```

### Alternative: Guix Build (Recommended)

```bash
# Build entire stack reproducibly with GNU Guix
guix build -f guix.scm

# Or use the unified Makefile
make all
```

---

## Dependency Graph

### Visual Dependency Tree

```
AGI-OS Stack
│
├─ Layer 1: Cognumach (1.8.0)
│  └─ [Microkernel] ── (no dependencies)
│
├─ Layer 2: HurdCog (0.9.0)
│  ├─ [Core Servers] ── depends on → Cognumach
│  └─ [Cognitive Kernel]
│     ├─ machspace-bridge.scm ── depends on → Cognumach IPC
│     ├─ fusion-reactor ── Python/Guile
│     └─ dashboard ── Flask
│
└─ Layer 3: OCC
   │
   ├─ Foundation
   │  ├─ cogutil (2.3.1) ── (no dependencies)
   │  └─ atomspace (5.0.4) ── depends on → cogutil
   │
   ├─ Storage Backends
   │  ├─ atomspace-storage (1.3.0) ── depends on → atomspace
   │  ├─ atomspace-rocks (1.3.0) ── depends on → atomspace, atomspace-storage
   │  ├─ atomspace-cog (0.1.4) ── depends on → atomspace, atomspace-storage
   │  └─ atomspace-pgres (1.3.0) ── depends on → atomspace, atomspace-storage
   │
   ├─ Network Layer
   │  └─ cogserver (3.3.0) ── depends on → atomspace
   │
   ├─ Math & ML
   │  └─ matrix (1.3.0) ── depends on → atomspace
   │
   ├─ Reasoning & Learning
   │  ├─ unify (0.1.7) ── depends on → atomspace
   │  ├─ ure (0.3.3) ── depends on → atomspace, unify
   │  ├─ learn (0.1.4) ── depends on → atomspace
   │  ├─ miner (1.0.0) ── depends on → atomspace, ure
   │  └─ pln (0.1.1) ── depends on → atomspace, ure
   │
   ├─ Attention & Control
   │  └─ attention (0.1.4) ── depends on → atomspace
   │
   ├─ Agent Framework
   │  ├─ agents (0.1.4) ── depends on → atomspace
   │  └─ sensory (0.4.0) ── depends on → atomspace
   │
   ├─ Evolutionary Optimization
   │  └─ asmoses (3.4.0) ── depends on → cogutil
   │
   └─ Cognitive Architecture
      ├─ coggml (1.0.0) ── (standalone)
      ├─ cogself (1.0.0) ── depends on → coggml
      ├─ atomspace-accelerator (1.0.0) ── (optional: atomspace)
      └─ agentic-chatbots (1.0.0) ── (standalone)
```

### Component Categories

| Category | Components | Build After |
|----------|-----------|------------|
| **Microkernel** | cognumach | - |
| **Operating System** | hurdcog | cognumach |
| **Foundation** | cogutil, atomspace | - |
| **Storage** | atomspace-storage, atomspace-rocks, atomspace-cog, atomspace-pgres | atomspace |
| **Network** | cogserver | atomspace |
| **Math** | matrix | atomspace |
| **Reasoning** | unify, ure, pln, miner | atomspace, (unify for ure/pln/miner) |
| **Learning** | learn | atomspace |
| **Attention** | attention | atomspace |
| **Agents** | agents, sensory | atomspace |
| **Evolution** | asmoses | cogutil |
| **Cognitive Arch** | coggml, cogself, atomspace-accelerator, agentic-chatbots | - |

---

## Version Consistency Table

| Component | Current Version | Proposed Standard | Status |
|-----------|----------------|-------------------|--------|
| cognumach | 1.8.0 | 1.8.0 | ✓ Standardized |
| hurdcog | 0.9.0 | 0.9.0 | ✓ Standardized |
| cogutil | 2.3.1 | 2.3.1 | ✓ Keep (dynamic from file) |
| atomspace | 5.0.4 | 5.0.4 | ✓ Keep (dynamic from file) |
| cogserver | 3.3.0 | 3.3.0 | ✓ Keep |
| matrix | 1.3.0 | 1.3.0 | ✓ Keep |
| atomspace-storage | 1.3.0 | 1.3.0 | ✓ Keep |
| atomspace-rocks | 1.3.0 | 1.3.0 | ✓ Keep |
| atomspace-pgres | 1.3.0 | 1.3.0 | ✓ Keep |
| atomspace-cog | 0.1.4 | 1.0.0 | ⚠ Update needed |
| unify | 0.1.7 | 0.1.7 | ✓ Keep (dynamic from file) |
| ure | 0.3.3 | 0.3.3 | ✓ Keep (dynamic from file) |
| pln | 0.1.1 | 1.0.0 | ⚠ Update needed |
| miner | 1.0.0 | 1.0.0 | ✓ Keep |
| attention | 0.1.4 | 1.0.0 | ⚠ Update needed |
| learn | - | 1.0.0 | ⚠ Add version |
| agents | - | 1.0.0 | ⚠ Add version |
| sensory | 0.4.0 | 0.4.0 | ✓ Keep |
| asmoses | 3.4.0 | 3.4.0 | ✓ Keep |
| coggml | - | 1.0.0 | ⚠ Add version |
| cogself | - | 1.0.0 | ⚠ Add version |
| atomspace-accelerator | - | 1.0.0 | ⚠ Add version |
| agentic-chatbots | - | 1.0.0 | ⚠ Add version |

---

## Build Methods Comparison

### Method 1: Direct CMake Build

**Pros:**
- Fast incremental builds
- Direct control over options
- Good for development

**Cons:**
- System dependency variations
- Less reproducible

**Usage:**
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Method 2: GNU Guix Build

**Pros:**
- Fully reproducible
- Isolated build environment
- Declarative dependencies

**Cons:**
- Slower initial build
- Requires Guix installation

**Usage:**
```bash
guix build -f guix.scm
```

### Method 3: Unified Makefile

**Pros:**
- Simple interface
- Handles Guix or direct build
- Good for CI/CD

**Cons:**
- Abstraction layer overhead

**Usage:**
```bash
make all           # Default: Guix build
make direct-build  # Direct CMake build
```

---

## Integration Bridges

Components for cross-layer integration:

### HurdCog ↔ OCC Bridge

- **Location:** `hurdcog-integration/`
- **Files:**
  - `atomspace-hurdcog-bridge.py` - Python bridge
  - `cogkernel/mach-integration/machspace-bridge.scm` - Scheme bridge
- **Function:** Connects AtomSpace to MachSpace

### Cognumach ↔ HurdCog Bridge

- **Location:** `cognumach/include/mach/cognitive/`
- **Files:**
  - `atomspace_ipc.h` - Cognitive IPC definitions
  - `cognitive_vm.h` - Cognitive VM extensions
- **Function:** Provides cognitive primitives at microkernel level

---

## Testing Strategy

### Per-Layer Testing

**Layer 1 (Cognumach):**
```bash
make cognumach-check
```

**Layer 2 (HurdCog):**
```bash
make hurdcog-check
python3 hurdcog/run-phase6-tests.py
```

**Layer 3 (OCC):**
```bash
# Per-component
cd cogutil/build && make check
cd atomspace/build && make check

# Integration
./synergy.sh
./test-integration.sh
```

### Full Stack Integration Test

```bash
make test
./test-integration.sh
```

---

## Maintenance Notes

### Version Updates

When updating component versions:

1. Update version in `CMakeLists.txt` (SET SEMANTIC_VERSION)
2. Update this document
3. Update `guix.scm` if dependencies change
4. Run integration tests
5. Update CHANGELOG.md

### Adding New Components

1. Create `CMakeLists.txt` in component directory
2. Add to root `CMakeLists.txt` with OPTION and add_subdirectory
3. Update this document with build sequence
4. Add to dependency graph
5. Update version consistency table
6. Add tests

---

## References

- **CMake Documentation:** https://cmake.org/documentation/
- **GNU Autotools:** https://www.gnu.org/software/automake/manual/
- **GNU Guix Manual:** https://guix.gnu.org/manual/
- **OpenCog Wiki:** https://wiki.opencog.org/
- **GNU Mach Manual:** https://www.gnu.org/software/hurd/gnumach-doc/
- **GNU Hurd Manual:** https://www.gnu.org/software/hurd/doc/

---

*Last Updated: 2025-12-06*  
*Document Version: 1.0.0*
