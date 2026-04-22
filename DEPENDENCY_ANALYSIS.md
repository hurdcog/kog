# OpenCog Collection - Dependency Analysis & Build Order

## Overview

This document describes the scientifically-derived build order for the OpenCog Collection (OCC) monorepo, computed via topological sort of CMakeLists.txt dependencies.

## Analysis Methodology

The build order was determined by:

1. **Scanning** all components with CMakeLists.txt files
2. **Extracting** FIND_PACKAGE dependencies from each CMakeLists.txt
3. **Normalizing** package names to directory names (e.g., CogUtil → cogutil)
4. **Building** a dependency graph of internal project dependencies
5. **Computing** topological sort using Kahn's algorithm
6. **Validating** the result ensures all dependencies are built before dependents

## Component Dependency Graph

### Foundation Layer (No Internal Dependencies)

**Core Foundation:**
- **cogutil** - Foundation utilities library

**Independent Components:**
- agents
- aphrodite-engine
- atomspace-accelerator
- blender_api_msgs
- coggml
- cognumach
- cogself
- ghost_bridge
- gnucash
- hurdcog
- integration
- koboldcpp
- motor
- perception
- agentic-chatbots

### Layer 1: CogUtil Dependents
- **atomspace** → cogutil
- **moses** → cogutil

### Layer 2: AtomSpace Dependents
Components requiring CogUtil + AtomSpace:
- agi-bio
- atomspace-agents
- atomspace-dht
- atomspace-ipfs
- atomspace-metta
- atomspace-rpc
- **atomspace-storage** (critical for cogserver)
- atomspace-websockets
- cheminformatics
- dimensional-embedding
- generate
- matrix
- pattern-index
- sensory
- spacetime
- **unify** (required by URE)
- vision
- visualization

### Layer 3: Storage Dependents
Components requiring AtomSpace + AtomSpace-Storage:
- atomese-simd → cogutil, atomspace, sensory
- atomspace-bridge → cogutil, atomspace, atomspace-storage
- atomspace-pgres → cogutil, atomspace, atomspace-storage
- atomspace-rocks → cogutil, atomspace, atomspace-storage
- **cogserver** → cogutil, atomspace, atomspace-storage
- lg-atomese → cogutil, atomspace, atomspace-storage

### Layer 4: Unify Dependents
- **ure** → cogutil, atomspace, unify

### Layer 5: URE Dependents
- asmoses → cogutil, atomspace, ure

### Layer 6: CogServer Dependents
Components requiring CogServer:
- atomspace-cog → cogutil, atomspace, atomspace-storage, cogserver
- atomspace-restful → cogutil, atomspace, cogserver
- attention → cogutil, atomspace, cogserver
- learn → cogutil, atomspace, cogserver

### Layer 7: Advanced Cognitive Components
- **miner** → cogutil, atomspace, unify, ure
- **pln** → cogutil, atomspace, spacetime, unify, ure

### Layer 8: Top-Level Integration
- **opencog** → cogutil, atomspace, cogserver, ure, pln

### Layer 9: Benchmarking
- **benchmark** → cogutil, atomspace, atomspace-storage, ure, benchmark (circular - build last)

## Critical Build Order Rules

### Rule 1: Foundation First
```
cogutil → atomspace → atomspace-storage → cogserver
```

### Rule 2: Reasoning Chain
```
unify → ure → pln/miner → opencog
```

### Rule 3: Storage Before Server
```
atomspace-storage must precede cogserver
```

### Rule 4: Server Before Services
```
cogserver must precede atomspace-cog, atomspace-restful, attention, learn
```

### Rule 5: SpaceTime for PLN
```
spacetime must precede pln
```

## System Dependencies

Based on analysis of all CMakeLists.txt files, the following system packages are required:

```bash
sudo apt-get install -y \
  build-essential \
  cmake \
  cxxtest \
  cython3 \
  doxygen \
  guile-3.0-dev \
  libboost-all-dev \
  liboctomap-dev \
  libpq-dev \
  librocksdb-dev \
  libssl-dev \
  python3-dev \
  python3-nose \
  uuid-dev \
  valgrind
```

## Complete Build Order

The scientifically correct build sequence for `.github/workflows/ocall.yml`:

```
1. cogutil                 (foundation)
2. atomspace               (hypergraph database)
3. moses                   (evolutionary optimization)
4. agi-bio                 (bio-inspired AI)
5. atomspace-agents        (agent framework)
6. atomspace-dht           (DHT storage)
7. atomspace-ipfs          (IPFS storage)
8. atomspace-metta         (Metta integration)
9. atomspace-rpc           (RPC interface)
10. atomspace-storage      (storage API - required by cogserver!)
11. atomspace-websockets   (WebSocket interface)
12. cheminformatics        (chemistry AI)
13. dimensional-embedding  (embedding algorithms)
14. generate               (generation algorithms)
15. matrix                 (matrix operations)
16. pattern-index          (pattern indexing)
17. sensory                (sensory processing)
18. spacetime              (spatiotemporal reasoning - required by PLN!)
19. unify                  (unification - required by URE!)
20. vision                 (vision processing)
21. visualization          (visualization tools)
22. atomese-simd           (SIMD optimizations)
23. atomspace-bridge       (bridge interfaces)
24. atomspace-pgres        (PostgreSQL backend)
25. atomspace-rocks        (RocksDB backend)
26. cogserver              (networking layer)
27. lg-atomese             (link-grammar integration)
28. ure                    (unified rule engine - requires unify!)
29. asmoses                (AS-MOSES - requires ure!)
30. atomspace-cog          (CogServer storage - requires cogserver!)
31. atomspace-restful      (REST API - requires cogserver!)
32. attention              (attention allocation - requires cogserver!)
33. learn                  (learning algorithms - requires cogserver!)
34. miner                  (pattern mining - requires ure!)
35. pln                    (probabilistic logic - requires spacetime, unify, ure!)
36. opencog                (main integration - requires cogserver, ure, pln!)
37. benchmark              (benchmarking suite)

**Independent Components** (can be built anytime after foundation):
- agents (agent framework)
- agentic-chatbots (chatbot integration)
- aphrodite-engine (LLM inference engine)
- atomspace-accelerator (performance optimizations)
- blender_api_msgs (Blender integration messages)
- coggml (GGML cognitive kernel)
- cognumach (L1 microkernel)
- cogself (AGI synergy framework)
- ghost_bridge (GHOST integration)
- gnucash (cognitive accounting)
- hurdcog (L2 cognitive OS)
- integration (integration testing)
- koboldcpp (story/world modeling)
- motor (motor control)
- perception (perception framework)
```

## Validation

This build order has been validated by:
- ✅ Topological sort guarantees dependency satisfaction
- ✅ All FIND_PACKAGE references analyzed from CMakeLists.txt
- ✅ Critical paths (cogutil→atomspace→storage→server) verified
- ✅ Reasoning chain (unify→ure→pln→opencog) validated
- ✅ No circular dependencies (except benchmark self-reference)

## Notes

1. **Benchmark Circular Dependency**: The benchmark component has a self-reference in its CMakeLists.txt, which creates a circular dependency. It's built last.

2. **Optional Components**: Components without CMakeLists.txt or that don't exist are gracefully skipped by the workflow.

3. **Parallel Build Potential**: Components at the same dependency level (e.g., all Layer 2 components) could theoretically be built in parallel for faster CI builds.

4. **System Packages**: The apt-get install command includes all detected system dependencies from CMakeLists.txt analysis.

## Regenerating This Analysis

To regenerate this analysis with updated dependencies, run:

```bash
python3 /tmp/analyze_dependencies.py
```

This will re-scan all CMakeLists.txt files and compute the current dependency graph.

---

**Generated**: 2025-12-09  
**Method**: Topological sort of CMakeLists.txt FIND_PACKAGE dependencies  
**Components Analyzed**: 52 with CMakeLists.txt  
**Build Order**: Dependency-correct sequence ensuring prerequisite satisfaction
