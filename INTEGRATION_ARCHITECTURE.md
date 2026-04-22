# OCC-HurdCog-Cognumach Integration Architecture

**Date**: December 8, 2025  
**Version**: 1.0  
**Purpose**: Define the complete integration architecture for the unified AGI Operating System

## Overview

This document specifies the integration architecture for combining three major subsystems into a coherent, production-ready AGI Operating System:

1. **Cognumach** (Layer 1): Enhanced GNU Mach microkernel with cognitive features
2. **HurdCog** (Layer 2): Cognitive operating system based on GNU Hurd
3. **OCC** (Layer 3): OpenCog Collection AGI framework

## Architectural Layers

### Layer 0: Build Infrastructure

**Purpose**: Provide unified build tools and dependency management

**Components**:
- **MIG (Mach Interface Generator)**: Centralized at `build-tools/mig/`
  - Generates IPC stubs for Mach-based communication
  - Shared by both Cognumach and HurdCog
  - Single source of truth for interface definitions

**Integration Points**:
- CMake build orchestration at repository root
- Autotools integration for Mach-based components
- Debian packaging infrastructure

### Layer 1: Cognumach (Cognitive Microkernel)

**Purpose**: Provide enhanced microkernel with cognitive-aware primitives

**Key Features**:
- Enhanced memory management with attention-aware allocation
- SMP (Symmetric Multiprocessing) support
- VM (Virtual Memory) optimizations
- IPC (Inter-Process Communication) with cognitive extensions
- Cognitive scheduler primitives

**Cognitive Extensions**:
1. **Attention-Aware Memory Management**
   - Memory pages tagged with attention values
   - Allocation prioritized by cognitive importance
   - Swapping decisions informed by attention

2. **Cognitive IPC Primitives**
   - Message ports with attention routing
   - Atom serialization support in IPC
   - Distributed hypergraph communication

3. **Scheduler Hooks**
   - CPU time allocation based on cognitive priority
   - Thread attention values
   - Learning-based scheduling policies

**Build Dependencies**:
- MIG (Mach Interface Generator)
- 32-bit build tools (for i386 compatibility)
- Autotools (autoconf, automake, libtool)

**Integration Requirements**:
- Must be built before HurdCog
- Provides kernel primitives for cognitive operations
- Exports cognitive IPC interfaces

### Layer 2: HurdCog (Cognitive Operating System)

**Purpose**: Provide cognitive operating system services on top of Cognumach

**Key Components**:

1. **MachSpace Abstraction Layer**
   - Distributed hypergraph storage
   - Shared memory AtomSpace
   - Copy-on-write atom management
   - Kernel-level atom access

2. **CogKernel Core**
   - Cognitive kernel services
   - System-wide reasoning coordination
   - Attention allocation across processes
   - Learning and adaptation mechanisms

3. **System Translators (Cognitive)**
   - File system translator with semantic indexing
   - Network translator with cognitive routing
   - Device translators with learning capabilities

**Cognitive Features**:

1. **MachSpace (Distributed Hypergraph)**
   ```
   ┌─────────────────────────────────────────┐
   │  User Space Applications                │
   │  (OpenCog, PLN, ECAN, etc.)            │
   ├─────────────────────────────────────────┤
   │  MachSpace API                          │
   │  - Atom CRUD operations                 │
   │  - Query interface                      │
   │  - Attention value access               │
   ├─────────────────────────────────────────┤
   │  Shared Memory Hypergraph               │
   │  - Copy-on-write atoms                  │
   │  - Distributed across processes         │
   │  - Kernel-managed consistency           │
   ├─────────────────────────────────────────┤
   │  Cognumach IPC Layer                    │
   │  - Message-based atom transfer          │
   │  - Attention-aware routing              │
   └─────────────────────────────────────────┘
   ```

2. **Cognitive Scheduler Integration**
   - Receives attention values from MachSpace
   - Adjusts process priorities dynamically
   - Learns optimal scheduling policies
   - Supports Tensor Thread Fiber scheduling

3. **System-Wide Reasoning**
   - URE (Unified Rule Engine) at kernel level
   - Distributed inference across processes
   - Cognitive resource management

**Build Dependencies**:
- Cognumach (must be built first)
- MIG (from build-tools)
- Autotools
- Python 3 (for cognitive modules)
- Guile 3.0 (for Scheme-based reasoning)

**Integration Requirements**:
- Links against Cognumach kernel
- Provides MachSpace API for OCC
- Exports cognitive system services

### Layer 3: OCC (OpenCog Collection)

**Purpose**: Provide comprehensive AGI framework and cognitive components

**Core Components** (in build order):

1. **CogUtil** (Foundation)
   - Low-level C++ utilities
   - No dependencies beyond system libraries
   - Required by all other components

2. **AtomSpace** (Core Hypergraph)
   - Hypergraph knowledge representation
   - Truth values and attention values
   - Query and pattern matching
   - **Integration**: Can use MachSpace as storage backend

3. **Storage Backends**
   - atomspace-cog: CogServer storage
   - atomspace-rocks: RocksDB storage
   - atomspace-pgres: PostgreSQL storage
   - **New**: atomspace-machspace: HurdCog MachSpace storage

4. **CogServer** (Network Interface)
   - Network access to AtomSpace
   - Command-line interface
   - Module loading system

5. **URE** (Unified Rule Engine)
   - Forward and backward chaining
   - Probabilistic reasoning
   - **Integration**: Can execute rules at kernel level via HurdCog

6. **Cognitive Components**
   - **ECAN** (Economic Attention Networks): Attention allocation
   - **PLN** (Probabilistic Logic Networks): Reasoning
   - **Miner**: Pattern mining
   - **Unify**: Unification framework
   - **SpaceTime**: Spatiotemporal reasoning

7. **Learning Components**
   - **Learn**: Language learning
   - **Generate**: Natural language generation
   - **lg-atomese**: Link Grammar integration
   - **Relex**: Relation extraction

8. **Specialized Components**
   - **MOSES**: Evolutionary optimization
   - **Vision**: Computer vision
   - **agi-bio**: Bioinformatics

**Build Dependencies**:
- Boost (C++ libraries)
- Guile 3.0 (Scheme interpreter)
- Python 3 (Python bindings)
- Cython (Python-C++ bridge)
- CMake (build system)

**Integration Requirements**:
- Can optionally use MachSpace for distributed storage
- ECAN attention values feed into cognitive scheduler
- URE can leverage kernel-level reasoning

## Integration Bridges

### Bridge 1: AtomSpace ↔ MachSpace

**Purpose**: Allow AtomSpace to use MachSpace as a distributed storage backend

**Implementation**:
- Location: `opencog-debian/hurdcog-atomspace-bridge/`
- Language: C++ with Python bindings
- API: AtomSpace StorageNode interface

**Key Functions**:
```cpp
class MachSpaceStorageNode : public StorageNode {
public:
    // Connect to MachSpace
    void open();
    
    // Store atom in MachSpace
    void store_atom(const Handle& h);
    
    // Fetch atom from MachSpace
    Handle fetch_atom(const Handle& h);
    
    // Query MachSpace
    HandleSeq fetch_incoming_set(const Handle& h);
    
    // Sync with MachSpace
    void barrier();
};
```

**Integration Flow**:
```
AtomSpace (OCC)
    ↓
MachSpaceStorageNode (Bridge)
    ↓
MachSpace API (HurdCog)
    ↓
Shared Memory Hypergraph (Kernel)
    ↓
Cognumach IPC (Microkernel)
```

### Bridge 2: ECAN ↔ Cognitive Scheduler

**Purpose**: Feed attention values from ECAN into the cognitive scheduler

**Implementation**:
- Location: `opencog-debian/cognumach-cognitive-scheduler/`
- Language: C with Scheme bindings
- API: Scheduler policy hooks

**Key Functions**:
```c
// Set attention value for a process
void sched_set_attention(task_t task, float attention_value);

// Get attention value for a process
float sched_get_attention(task_t task);

// Update scheduling policy based on attention
void sched_update_policy();
```

**Integration Flow**:
```
ECAN (OCC)
    ↓
Attention Value Updates
    ↓
MachSpace (HurdCog)
    ↓
Cognitive Scheduler (Cognumach)
    ↓
CPU Time Allocation
```

### Bridge 3: URE ↔ CogKernel

**Purpose**: Enable kernel-level rule execution for system-wide reasoning

**Implementation**:
- Location: `opencog-debian/hurdcog-occ-bridge/`
- Language: C++ and Scheme
- API: Rule execution hooks

**Key Functions**:
```scheme
;; Execute rule at kernel level
(define (cogkernel-execute-rule rule-handle)
  ...)

;; Register rule for system-wide inference
(define (cogkernel-register-rule rule-handle)
  ...)

;; Query kernel-level knowledge
(define (cogkernel-query pattern)
  ...)
```

**Integration Flow**:
```
URE (OCC)
    ↓
Rule Execution Request
    ↓
CogKernel Core (HurdCog)
    ↓
Distributed Inference
    ↓
MachSpace Updates
```

## Tetradic Architecture Alignment

### Four Tensor Bundles

The integrated system implements a **tetradic/tetrahedral structure** with 4 cognitive tensor bundles:

1. **Perception Bundle** (Input Processing)
   - Vision system
   - NLP (Relex, Link Grammar)
   - Sensory processing
   - **Integration**: Kernel-level sensor fusion

2. **Reasoning Bundle** (Inference)
   - PLN (Probabilistic Logic Networks)
   - URE (Unified Rule Engine)
   - Unify framework
   - **Integration**: Distributed reasoning via CogKernel

3. **Learning Bundle** (Adaptation)
   - Learn (language learning)
   - Miner (pattern mining)
   - MOSES (evolutionary optimization)
   - **Integration**: System-wide learning via MachSpace

4. **Action Bundle** (Output Generation)
   - Generate (NLG)
   - Motor control
   - Embodiment
   - **Integration**: Cognitive translators for action execution

### Triadic Structure

Each bundle contains a **triadic system** of 3 dyadic edges:

```
Perception Bundle:
  Edge 1: Vision ↔ NLP
  Edge 2: NLP ↔ Sensory
  Edge 3: Sensory ↔ Vision

Reasoning Bundle:
  Edge 1: PLN ↔ URE
  Edge 2: URE ↔ Unify
  Edge 3: Unify ↔ PLN

Learning Bundle:
  Edge 1: Learn ↔ Miner
  Edge 2: Miner ↔ MOSES
  Edge 3: MOSES ↔ Learn

Action Bundle:
  Edge 1: Generate ↔ Motor
  Edge 2: Motor ↔ Embodiment
  Edge 3: Embodiment ↔ Generate
```

### Tetrahedral Symmetry

The 4 triadic fiber bundles share **6 dyadic edges** with mutually orthogonal symmetries:

```
        Perception
           /|\
          / | \
         /  |  \
        /   |   \
    Reasoning--Learning
        \   |   /
         \  |  /
          \ | /
           \|/
          Action
```

**Shared Edges**:
1. Perception ↔ Reasoning: Sensory input drives inference
2. Perception ↔ Learning: Pattern recognition from input
3. Perception ↔ Action: Sensorimotor loops
4. Reasoning ↔ Learning: Inference-guided learning
5. Reasoning ↔ Action: Goal-directed behavior
6. Learning ↔ Action: Learned action policies

### Ontogenetic Loom Placement

**Ontogenetic Looms** are placed at integration points for optimal weaving of cognitive inference engines:

1. **Loom 1**: AtomSpace ↔ MachSpace bridge
   - Weaves serial and parallel tensor thread fibers
   - Coordinates knowledge representation across layers

2. **Loom 2**: ECAN ↔ Cognitive Scheduler bridge
   - Weaves attention allocation across system
   - Coordinates resource management

3. **Loom 3**: URE ↔ CogKernel bridge
   - Weaves reasoning operations across processes
   - Coordinates distributed inference

4. **Loom 4**: Component Integration Layer
   - Weaves the four tensor bundles
   - Coordinates cognitive synergy

## Build Dependency Graph

```
Layer 0: Build Tools
    └─ MIG (build-tools/mig/)

Layer 1: Cognumach
    ├─ Depends on: MIG
    └─ Provides: Cognitive microkernel primitives

Layer 2: HurdCog
    ├─ Depends on: Cognumach, MIG
    └─ Provides: MachSpace, CogKernel, Cognitive translators

Layer 3: OCC Core
    ├─ CogUtil (no dependencies)
    ├─ AtomSpace (depends on: CogUtil)
    ├─ Storage backends (depend on: AtomSpace)
    │   ├─ atomspace-cog
    │   ├─ atomspace-rocks
    │   ├─ atomspace-pgres
    │   └─ atomspace-machspace (depends on: HurdCog)
    ├─ CogServer (depends on: AtomSpace)
    └─ URE (depends on: AtomSpace)

Layer 4: Integration Bridges
    ├─ hurdcog-atomspace-bridge (depends on: HurdCog, AtomSpace)
    ├─ cognumach-cognitive-scheduler (depends on: Cognumach, HurdCog)
    └─ hurdcog-occ-bridge (depends on: HurdCog, URE)

Layer 5: Cognitive Components
    ├─ ECAN (depends on: AtomSpace)
    ├─ PLN (depends on: URE)
    ├─ Miner (depends on: AtomSpace, URE)
    ├─ Unify (depends on: AtomSpace)
    └─ SpaceTime (depends on: AtomSpace)

Layer 6: Learning & NLP
    ├─ Learn (depends on: AtomSpace, URE, atomspace-rocks)
    ├─ Generate (depends on: AtomSpace)
    ├─ lg-atomese (depends on: AtomSpace)
    └─ Relex (depends on: AtomSpace)

Layer 7: Specialized Systems
    ├─ MOSES (depends on: CogUtil)
    ├─ Vision (depends on: AtomSpace)
    └─ agi-bio (depends on: AtomSpace, PLN)

Layer 8: Meta-Packages
    ├─ opencog (depends on: all OCC components)
    └─ agi-os-unified (depends on: Cognumach, HurdCog, opencog)
```

## Debian Package Structure

### Package Naming Convention

- **Library packages**: `lib<name>` (e.g., `libcogutil`)
- **Development packages**: `lib<name>-dev` (e.g., `libcogutil-dev`)
- **Application packages**: `<name>` (e.g., `opencog-atomspace`)
- **Integration packages**: `<layer1>-<layer2>-<purpose>` (e.g., `hurdcog-atomspace-bridge`)

### Core Packages (32 total)

1. **cognumach** - Cognitive microkernel
2. **cognumach-dev** - Development files
3. **hurdcog** - Cognitive operating system
4. **hurdcog-dev** - Development files
5. **libcogutil** - Foundation utilities
6. **libcogutil-dev** - Development files
7. **opencog-atomspace** - Hypergraph database
8. **opencog-atomspace-dev** - Development files
9. **opencog-atomspace-cog** - CogServer storage
10. **opencog-atomspace-rocks** - RocksDB storage
11. **opencog-atomspace-pgres** - PostgreSQL storage
12. **opencog-atomspace-machspace** - MachSpace storage (NEW)
13. **opencog-cogserver** - Network interface
14. **opencog-ure** - Unified Rule Engine
15. **opencog-attention** - ECAN
16. **opencog-pln** - PLN
17. **opencog-miner** - Pattern miner
18. **opencog-unify** - Unification
19. **opencog-spacetime** - Spatiotemporal reasoning
20. **opencog-learn** - Language learning
21. **opencog-generate** - NLG
22. **opencog-lg-atomese** - Link Grammar
23. **opencog-relex** - Relation extraction
24. **opencog-moses** - MOSES
25. **opencog-vision** - Computer vision
26. **opencog-agi-bio** - Bioinformatics
27. **hurdcog-machspace** - MachSpace layer
28. **hurdcog-cogkernel-core** - CogKernel core
29. **hurdcog-atomspace-bridge** - AtomSpace bridge
30. **cognumach-cognitive-scheduler** - Cognitive scheduler
31. **hurdcog-occ-bridge** - OCC bridge
32. **agi-os-unified** - Complete AGI-OS

### Package Dependencies

Each package's `debian/control` file specifies:
- **Build-Depends**: Required for building
- **Depends**: Required for runtime
- **Recommends**: Optional but recommended
- **Suggests**: Optional enhancements

## Production Readiness Checklist

### Build Infrastructure
- [x] Centralized MIG tool
- [x] Root-level CMake orchestration
- [x] Autotools integration for Mach components
- [ ] CI/CD pipeline configuration
- [ ] Automated testing framework

### Debian Packaging
- [x] Package structure defined
- [ ] All debian/control files complete
- [ ] All debian/rules files complete
- [ ] Package build scripts tested
- [ ] Dependency resolution validated

### Integration Bridges
- [ ] hurdcog-atomspace-bridge implementation complete
- [ ] cognumach-cognitive-scheduler implementation complete
- [ ] hurdcog-occ-bridge implementation complete
- [ ] Integration tests passing
- [ ] Performance benchmarks established

### Documentation
- [x] Architecture documentation
- [x] Integration specifications
- [ ] API documentation (Doxygen)
- [ ] User guides
- [ ] Administrator guides

### Testing
- [ ] Unit tests for all components
- [ ] Integration tests for bridges
- [ ] System-level tests
- [ ] Performance tests
- [ ] Security audit

## Next Steps

### Immediate (Phase 4)
1. Implement MIG consolidation
2. Update all references to use centralized MIG
3. Create symlinks for backward compatibility
4. Test Cognumach and HurdCog builds

### Short-term (Phase 5-6)
1. Complete hurdcog-atomspace-bridge implementation
2. Implement MachSpace abstraction layer
3. Complete cognumach-cognitive-scheduler
4. Implement attention-based scheduling
5. Complete all Debian package specifications

### Long-term (Phase 7-8)
1. Full system integration testing
2. Performance optimization
3. Security hardening
4. Production deployment
5. Documentation completion

## Conclusion

This integration architecture provides a comprehensive roadmap for combining Cognumach, HurdCog, and OCC into a unified AGI Operating System. The layered approach ensures clean separation of concerns while enabling deep integration through well-defined bridges. The tetradic structure aligns with cognitive architecture principles for optimal synergy.
