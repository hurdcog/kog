# AGI-OS Integration Analysis

**Date**: December 25, 2025
**Repository**: cogpy/occ
**Purpose**: Comprehensive analysis of hurdcog and cognumach integration requirements

## Current State

### Repository Structure

The occ repository contains three major subsystems:

1. **Cognumach** (`/cognumach/`) - GNU Mach microkernel with cognitive extensions
   - Autotools-based build system (configure.ac, Makefile.am)
   - CMake wrapper for integration
   - Symlink to build-tools/mig for MIG (Mach Interface Generator)
   - Version: 1.8.0

2. **HurdCog** (`/hurdcog/`) - GNU Hurd OS with cognitive capabilities
   - Autotools-based build system
   - CMake wrapper with cognitive extensions
   - Symlink to build-tools/mig
   - Cognitive kernel components (Python/Scheme)
   - Version: 0.9.0

3. **OCC** (OpenCog Collection) - Distributed across multiple directories
   - Core: cogutil, atomspace, cogserver, ure, pln, etc.
   - Storage backends: atomspace-storage, atomspace-rocks, atomspace-pgres
   - Cognitive components: attention, learn, generate, miner
   - Specialized: moses, agi-bio, vision

### Integration Layer

Located in `/integration/` directory:

- **atomspace-machspace**: Bridge between AtomSpace and MachSpace
- **cognitive-scheduler**: Attention-aware CPU scheduling
- **hurdcog-bridge**: Bridge between HurdCog and OCC layers

### Debian Packaging

Located in `/opencog-debian/` directory with 32 packages:

- Layer 0: cognumach
- Layer 1: cogutil
- Layer 2: atomspace
- Layer 2.5: atomspace-storage (CRITICAL - must be built before cogserver)
- Layer 3: Storage backends (atomspace-cog, atomspace-rocks, atomspace-pgres)
- Layer 4: cogserver, ure
- Layer 4.5: hurdcog, hurdcog-machspace, hurdcog-cogkernel-core
- Layer 4.6: Integration components (hurdcog-atomspace-bridge, cognumach-cognitive-scheduler, hurdcog-occ-bridge)
- Layers 5-11: Higher-level cognitive components

## Key Issues Identified

### 1. MIG Build Tool Management

**Issue**: MIG (Mach Interface Generator) is duplicated across multiple locations:
- `/build-tools/mig/` (canonical location)
- `/cognumach/mig` -> symlink to ../build-tools/mig
- `/hurdcog/mig` -> symlink to ../build-tools/mig
- `/hurdcog/external/hurd-repos/mig/` (full copy)
- `/hurdcog/external/unicorn-forest-repos/mig/` (full copy)

**Solution**: Consolidate MIG to a single canonical location and ensure all build systems reference it correctly.

### 2. Build Dependency Order

**Current Status**: atomspace-storage is correctly positioned as a dependency of cogserver.

**Verification Needed**: Ensure all packages in the build chain respect this order.

### 3. Integration Component Placement

**Issue**: Integration components are split between:
- `/integration/` (source implementations)
- `/opencog-debian/` (packaging metadata)
- `/hurdcog-integration/` (additional bridge scripts)

**Solution**: Consolidate integration components into a unified structure.

### 4. Root-Level Build System

**Missing**: A top-level CMakeLists.txt that orchestrates the entire AGI-OS build.

**Solution**: Create a master build system that:
- Builds cognumach first
- Builds hurdcog second
- Builds OCC core components
- Builds integration bridges
- Builds higher-level cognitive components

## Integration Requirements

### Phase 1: Microkernel Foundation (Cognumach)

**Dependencies**: None (system libraries only)

**Build Requirements**:
- 32-bit build tools (gcc-multilib)
- GNU autotools (autoconf, automake, libtool)
- MIG (Mach Interface Generator)
- Texinfo (for documentation)

**Build Time**: ~30 minutes

**Outputs**:
- gnumach kernel binary
- Development headers
- MIG tool

### Phase 2: Operating System Layer (HurdCog)

**Dependencies**: Cognumach

**Build Requirements**:
- Cognumach headers and libraries
- MIG tool
- Python 3 (for cognitive kernel)
- Guile 3.0 (for Scheme-based cognitive modules)
- GNU autotools

**Build Time**: ~45 minutes

**Outputs**:
- HurdCog system servers
- Cognitive kernel modules
- MachSpace implementation
- Development headers

### Phase 3: OCC Foundation

**Dependencies**: System libraries only (for cogutil and atomspace)

**Build Requirements**:
- CMake 3.12+
- C++17 compiler
- Boost libraries
- Guile 3.0
- Python 3

**Build Time**: ~30 minutes

**Outputs**:
- libcogutil
- libatomspace
- atomspace-storage

### Phase 4: Integration Bridges

**Dependencies**: Cognumach, HurdCog, OCC Foundation

**Components**:
1. **atomspace-machspace**: AtomSpace ↔ MachSpace bridge
2. **cognitive-scheduler**: ECAN ↔ CPU scheduler bridge
3. **hurdcog-atomspace-bridge**: Direct kernel-level AtomSpace integration
4. **cognumach-cognitive-scheduler**: Microkernel scheduler with attention awareness
5. **hurdcog-occ-bridge**: HurdCog ↔ OCC layer bridge

**Build Time**: ~20 minutes per component

### Phase 5: Higher-Level Components

**Dependencies**: OCC Foundation + Integration Bridges

**Components**: cogserver, ure, pln, attention, learn, generate, etc.

**Build Time**: ~2-3 hours total

## Optimal Build Dependency Order

```
Layer 0: Microkernel Foundation
  └─ cognumach (1.8.0)

Layer 1: Foundation Library
  └─ cogutil (libcogutil-dev)

Layer 2: Core AtomSpace
  └─ atomspace (opencog-atomspace)

Layer 2.5: Storage Foundation
  └─ atomspace-storage ⚠️ CRITICAL: Must be built before cogserver

Layer 3: Storage Backends (parallel build possible)
  ├─ atomspace-cog
  ├─ atomspace-rocks
  └─ atomspace-pgres

Layer 4: Core Services (parallel build possible)
  ├─ cogserver (depends on atomspace-storage)
  └─ ure

Layer 4.5: Cognitive Operating System Base
  ├─ hurdcog (depends on cognumach)
  ├─ hurdcog-machspace (depends on hurdcog, atomspace)
  └─ hurdcog-cogkernel-core (depends on hurdcog, atomspace)

Layer 4.6: AGI-OS Integration Components
  ├─ hurdcog-atomspace-bridge (depends on hurdcog, atomspace, atomspace-storage)
  ├─ cognumach-cognitive-scheduler (depends on cognumach, hurdcog-machspace)
  └─ hurdcog-occ-bridge (depends on hurdcog-machspace, hurdcog-cogkernel-core, atomspace)

Layer 5: Cognitive Components (parallel build possible)
  ├─ attention (depends on atomspace, ure)
  ├─ pln (depends on atomspace, ure)
  ├─ miner (depends on atomspace, ure)
  ├─ unify (depends on atomspace)
  └─ spacetime (depends on atomspace)

Layer 6: Learning and Generation (parallel build possible)
  ├─ learn (depends on atomspace, atomspace-rocks, ure)
  └─ generate (depends on atomspace, ure)

Layer 7: Natural Language Processing (parallel build possible)
  ├─ lg-atomese (depends on atomspace)
  └─ relex (depends on atomspace)

Layer 8: Specialized Systems (parallel build possible)
  ├─ moses (depends on cogutil)
  ├─ asmoses (depends on atomspace, moses)
  ├─ agi-bio (depends on atomspace, pln)
  ├─ vision (depends on atomspace)
  └─ kogboldai-kernel (depends on atomspace)

Layer 9: Meta-Package
  └─ opencog (meta-package pulling in all components)

Layer 10: AGI-OS Unified Stack
  └─ agi-os-unified (depends on cognumach, hurdcog, opencog)

Layer 11: AGI-OS Services
  ├─ agi-os-monitoring (depends on agi-os-unified)
  └─ agi-os-cognitive-init (depends on agi-os-unified)
```

## Recommended Actions

### 1. Consolidate MIG Tool

- Keep canonical MIG in `/build-tools/mig/`
- Maintain symlinks in cognumach and hurdcog
- Remove duplicate copies in hurdcog/external/
- Update all build scripts to reference the canonical location

### 2. Create Master Build System

Create `/CMakeLists.txt` that:
- Detects available components
- Builds in correct dependency order
- Supports parallel builds where possible
- Integrates autotools-based components (cognumach, hurdcog)
- Provides unified configuration options

### 3. Enhance Integration Layer

- Move integration components from scattered locations to `/integration/`
- Create proper CMake build system for integration layer
- Ensure integration components are built after their dependencies
- Add comprehensive testing for integration bridges

### 4. Complete Debian Packaging

- Verify all 32 packages have complete packaging metadata
- Ensure build dependencies are correctly specified
- Add package interdependencies
- Create automated build and test scripts
- Verify atomspace-storage is built before cogserver

### 5. Documentation

- Update BUILD_ORDER_ENHANCED.md with latest changes
- Create INTEGRATION_GUIDE.md for developers
- Document MIG tool usage and location
- Add architecture diagrams

## Cognitive Synergy Architecture

The integrated AGI-OS achieves cognitive synergy through:

1. **Kernel-Level Hypergraph**: MachSpace provides distributed hypergraph storage at the microkernel level
2. **Attention-Aware Scheduling**: CPU scheduler allocates resources based on ECAN attention values
3. **System-Wide Reasoning**: URE rules can execute at kernel level via CogKernel
4. **Distributed Knowledge**: AtomSpace atoms can be shared across system processes via IPC
5. **Cognitive Initialization**: System boot process is guided by reasoning and attention
6. **Unified Monitoring**: Telemetry from all three layers (Cognumach, HurdCog, OCC) is integrated

## Next Steps

1. ✅ Consolidate MIG tool references
2. ✅ Create master CMakeLists.txt
3. ✅ Enhance integration layer build system
4. ✅ Verify and update Debian packaging
5. ✅ Add comprehensive tests
6. ✅ Update documentation
7. ✅ Commit and push changes

