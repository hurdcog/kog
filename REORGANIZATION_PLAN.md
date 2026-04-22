# OCC Repository Reorganization Plan

**Date**: 2025-11-30  
**Objective**: Optimize repository structure for AGI-OS implementation  
**Current Size**: 2.8GB, 38,166 files, 4,857 directories

---

## Current State Analysis

### Repository Bloat Issues

The repository currently contains **2.8GB** of data across **38,166 files**, making it unwieldy for AGI-OS development. Major contributors to bloat include:

**Large Legacy Components**:
- `learn/` (712MB) - Learning algorithms, many outdated
- `opencog_rpi/` (297MB) - Raspberry Pi specific tools
- `test-datasets/` (295MB) - Large test datasets
- `TinyCog/` (147MB) - Experimental chatbot
- `unity3d-opencog-game/` (139MB) - Game integration
- `benchmark/` (76MB) - Performance benchmarks
- `loving-ai/` (59MB) - Specific application
- `external-tools/` (59MB) - Third-party tools

**Redundant Components**:
- Multiple Minecraft integrations
- Duplicate Python bindings
- Legacy Python 2 code in `python-attic/`
- Obsolete ROS integrations
- Old Docker configurations

**Non-Essential for AGI-OS**:
- Game integrations (Unity3D, Minecraft)
- Hardware-specific code (Raspberry Pi, robotics)
- Application-specific code (chatbots, loving-ai)
- Large benchmark datasets
- Legacy experimental projects

---

## Optimal AGI-OS Repository Structure

### Core Principle: Metamodel-Driven Architecture

Following the forensic study framework, the reorganized structure maps components to the MetaModel with focus on:
- **Tensor Thread Fibers**: Serial and parallel cognitive processing paths
- **Ontogenetic Looms**: Pattern weaving and emergence mechanisms
- **Cognitive Inference Engines**: PLN, URE, ECAN integration
- **Scheme Foundation**: Primary implementation language for metamodel

### Proposed Structure

```
occ/
├── core/                          # Core AGI-OS Components
│   ├── atomspace/                 # Hypergraph knowledge representation
│   ├── pln/                       # Probabilistic Logic Networks
│   ├── ure/                       # Unified Rule Engine
│   ├── ecan/                      # Economic Attention Networks
│   ├── cogserver/                 # Cognitive server infrastructure
│   └── scheme/                    # Scheme-based metamodel foundation
│
├── integration/                   # AGI-OS Integration Layer
│   ├── hurdcog-bridge/           # AtomSpace-HurdCog bridge
│   ├── cognumach-interface/      # Cognumach kernel interface
│   ├── cognitive-scheduler/      # PLN-based process scheduler
│   ├── distributed-atomspace/    # Multi-node synchronization
│   └── ipc/                      # Inter-process communication
│
├── cognitive-services/            # High-Level Cognitive Services
│   ├── reasoning/                # Reasoning engines
│   ├── learning/                 # Learning algorithms (curated)
│   ├── pattern-mining/           # Pattern recognition
│   ├── attention/                # Attention allocation
│   └── memory/                   # Memory management
│
├── metamodel/                     # Metamodel Implementation
│   ├── tensor-threads/           # Serial/parallel processing fibers
│   ├── ontogenetic-looms/        # Pattern weaving mechanisms
│   ├── inference-engines/        # Cognitive inference coordination
│   └── scheme-core/              # Scheme foundation (primary)
│
├── monitoring/                    # System Monitoring & Introspection
│   ├── cognitive-dashboard/      # Real-time visualization
│   ├── metrics/                  # Performance metrics
│   ├── profiling/                # Cognitive profiling tools
│   └── logging/                  # Structured logging
│
├── autogenesis/                   # Autonomous Evolution
│   ├── evolution-manager/        # Self-improvement orchestration
│   ├── code-generation/          # Automated code synthesis
│   ├── safety-validation/        # Safety checks
│   └── rollback/                 # Rollback mechanisms
│
├── build/                         # Build System
│   ├── guix/                     # GNU Guix packages
│   ├── cmake/                    # CMake configurations
│   ├── docker/                   # Containerization (minimal)
│   └── ci/                       # CI/CD workflows
│
├── docs/                          # Documentation
│   ├── architecture/             # Architecture guides
│   ├── integration/              # Integration documentation
│   ├── api/                      # API reference
│   ├── tutorials/                # Tutorials and examples
│   └── research/                 # Research papers and notes
│
├── tests/                         # Test Suite
│   ├── unit/                     # Unit tests
│   ├── integration/              # Integration tests
│   ├── cognitive/                # Cognitive synergy tests
│   └── performance/              # Performance benchmarks (minimal)
│
├── tools/                         # Development Tools
│   ├── scripts/                  # Utility scripts
│   ├── analyzers/                # Code analyzers
│   └── generators/               # Code generators
│
└── archive/                       # Archived Components
    ├── legacy-python2/           # Python 2 code (archived)
    ├── experimental/             # Experimental projects
    ├── applications/             # Specific applications
    └── hardware-specific/        # Hardware-specific code
```

---

## Components to Archive/Remove

### Archive (Move to `archive/`)

**Legacy Code**:
- `python-attic/` → `archive/legacy-python2/python-attic/`
- Obsolete Python 2 scripts throughout repository

**Experimental Projects**:
- `TinyCog/` → `archive/experimental/tinycog/`
- `unity3d-opencog-game/` → `archive/experimental/unity3d-game/`
- `opencog-to-minecraft/` → `archive/experimental/minecraft/`
- `rocca/` → `archive/experimental/rocca/`

**Application-Specific**:
- `loving-ai/` → `archive/applications/loving-ai/`
- `loving-ai-ghost/` → `archive/applications/loving-ai-ghost/`

**Hardware-Specific**:
- `opencog_rpi/` → `archive/hardware-specific/raspberry-pi/`
- `pau2motors/` → `archive/hardware-specific/motors/`
- `pi_vision/` → `archive/hardware-specific/pi-vision/`
- `ros-behavior-scripting/` → `archive/hardware-specific/ros/`
- `robots_config/` → `archive/hardware-specific/robots/`

**Large Datasets**:
- `test-datasets/` → Move to external storage or LFS

### Remove Completely

**Redundant/Obsolete**:
- Duplicate Docker configurations
- Old benchmark data files
- Compiled binaries and build artifacts
- Temporary files and logs
- `.pyc` files and `__pycache__` directories

---

## Core Components to Retain and Reorganize

### Essential AGI-OS Components

**Knowledge Representation**:
- `atomspace/` → `core/atomspace/`
- `atomspace-storage/` → `core/atomspace/storage/`
- `matrix/` → `cognitive-services/learning/matrix/`

**Reasoning & Inference**:
- `pln/` → `core/pln/`
- `ure/` → `core/ure/`
- `unify/` → `core/ure/unify/`

**Attention & Memory**:
- `attention/` → `core/ecan/`
- `spacetime/` → `cognitive-services/memory/spacetime/`

**Learning**:
- `learn/` → `cognitive-services/learning/` (curated subset)
- `miner/` → `cognitive-services/pattern-mining/`

**Infrastructure**:
- `cogserver/` → `core/cogserver/`
- `cogutil/` → `core/cogutil/`

**Integration** (New):
- `src/atomspace-hurdcog-bridge/` → `integration/hurdcog-bridge/`
- `src/cognitive-dashboard/` → `monitoring/cognitive-dashboard/`
- `src/autogenesis-enhanced/` → `autogenesis/evolution-manager/`

**Scheme Foundation**:
- Scheme-based components → `metamodel/scheme-core/`
- Cognitive grammar → `metamodel/inference-engines/`

---

## Reorganization Strategy

### Phase 1: Create New Structure

1. Create new directory hierarchy
2. Set up proper `.gitignore` for build artifacts
3. Create README files for each major directory

### Phase 2: Move Core Components

1. Move essential components to new locations
2. Update import paths and references
3. Update build system configurations

### Phase 3: Archive Legacy Components

1. Create `archive/` directory structure
2. Move non-essential components to archive
3. Add archive README with component descriptions

### Phase 4: Remove Bloat

1. Remove compiled binaries and build artifacts
2. Clean up temporary files
3. Remove duplicate configurations
4. Move large datasets to external storage

### Phase 5: Update Documentation

1. Update all documentation for new structure
2. Create migration guide
3. Update build instructions
4. Update integration guides

### Phase 6: Update Build System

1. Update CMakeLists.txt files
2. Update Guix package definitions
3. Update CI/CD workflows
4. Test build process

---

## Expected Benefits

### Size Reduction

**Target**: Reduce repository from 2.8GB to ~500MB (82% reduction)
- Remove large datasets: -295MB
- Archive experimental projects: -400MB
- Archive hardware-specific: -500MB
- Remove build artifacts: -100MB
- Archive legacy applications: -200MB
- Remaining bloat cleanup: -800MB

### Improved Organization

**Clarity**: Clear separation of concerns
- Core AGI-OS components in `core/`
- Integration layer in `integration/`
- Metamodel implementation in `metamodel/`
- Monitoring and evolution separate

**Discoverability**: Easier to find relevant code
- Logical hierarchy based on function
- Consistent naming conventions
- Comprehensive README files

**Maintainability**: Easier to maintain and evolve
- Reduced complexity
- Clear dependencies
- Better modularity

### Development Velocity

**Faster Cloning**: Smaller repository size
**Faster Builds**: Fewer components to build
**Clearer Focus**: AGI-OS implementation priorities clear
**Better Testing**: Focused test suite on essential components

---

## Migration Guide for Developers

### Import Path Changes

Old paths will be updated to new structure:

```python
# Old
from opencog.atomspace import AtomSpace

# New
from core.atomspace import AtomSpace
```

```scheme
;; Old
(use-modules (opencog))

;; New
(use-modules (core atomspace))
```

### Build System Changes

CMake targets will be reorganized:

```bash
# Old
cmake -DBUILD_ATOMSPACE=ON ..

# New
cmake -DBUILD_CORE_ATOMSPACE=ON ..
```

### Documentation Updates

All documentation will be updated to reflect new structure with migration examples provided.

---

## Risk Mitigation

### Backup Strategy

1. Create full repository backup before reorganization
2. Tag current state: `v0.1.0-pre-reorganization`
3. Create reorganization branch: `feature/repository-reorganization`
4. Test thoroughly before merging to main

### Rollback Plan

If reorganization causes issues:
1. Revert to tagged version
2. Restore from backup
3. Incremental reorganization instead of full

### Testing Strategy

1. Build all core components in new structure
2. Run full test suite
3. Verify integration with HurdCog/Cognumach
4. Performance benchmarks to ensure no regression

---

## Implementation Timeline

### Week 1: Preparation
- Create backup and tag
- Create new directory structure
- Document all component dependencies

### Week 2: Core Migration
- Move core components to new locations
- Update import paths
- Update build system

### Week 3: Archive & Cleanup
- Archive non-essential components
- Remove bloat
- Clean up build artifacts

### Week 4: Testing & Documentation
- Comprehensive testing
- Update all documentation
- Create migration guide

### Week 5: Deployment
- Merge to main branch
- Update CI/CD
- Announce changes to community

---

## Success Metrics

### Quantitative

- Repository size: < 500MB (from 2.8GB)
- File count: < 10,000 (from 38,166)
- Directory count: < 500 (from 4,857)
- Build time: < 10 minutes (baseline TBD)
- Clone time: < 2 minutes (baseline TBD)

### Qualitative

- Clear separation of AGI-OS components
- Easy navigation for new developers
- Comprehensive documentation
- Successful builds on all platforms
- No regression in functionality

---

## Conclusion

This reorganization will transform the OCC repository from a sprawling collection of experiments into a focused, efficient AGI-OS implementation platform optimized for integration with HurdCog and Cognumach.

The metamodel-driven structure ensures proper mapping of cognitive components to tensor thread fibers and ontogenetic looms, enabling optimal weaving of cognitive inference engines.
