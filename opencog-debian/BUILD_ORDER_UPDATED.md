# OpenCog Debian Package Build Order (Updated with Phase 1 Packages)

This document specifies the correct build order for all OpenCog and AGI-OS packages, including the new Phase 1 HurdCog subcomponents.

## Overview

The build process is organized into **13 stages** with clear dependency relationships. Packages within parallel stages can be built simultaneously to reduce total build time.

**Total Packages**: 27 (24 existing + 3 new Phase 1 packages)

---

## Build Stages

### Stage 0: Microkernel (Optional - AGI-OS only)

**Build Type**: Sequential  
**Estimated Time**: 30 minutes

| Package | Description | Dependencies |
|---------|-------------|--------------|
| `cognumach` | Enhanced GNU Mach microkernel | System libraries |

**Build Commands**:
```bash
cd opencog-debian/cognumach
./update-cognumach.sh
cd cognumach-*
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../*.deb
```

---

### Stage 1: Foundation

**Build Type**: Sequential  
**Estimated Time**: 10 minutes

| Package | Description | Dependencies |
|---------|-------------|--------------|
| `cogutil` | Foundation library with utilities | System libraries |

**Build Commands**:
```bash
cd opencog-debian/cogutil
./update-cogutil.sh
cd cogutil-*
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../libcogutil-dev_*.deb
```

---

### Stage 2: Core

**Build Type**: Sequential  
**Estimated Time**: 20 minutes

| Package | Description | Dependencies |
|---------|-------------|--------------|
| `atomspace` | Hypergraph database | cogutil |

**Build Commands**:
```bash
cd opencog-debian/atomspace
./update-atomspace.sh
cd atomspace-*
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-atomspace_*.deb
```

---

### Stage 3: Storage Backends

**Build Type**: **Parallel** (can build simultaneously)  
**Estimated Time**: 10 minutes (parallel) / 30 minutes (sequential)

| Package | Description | Dependencies |
|---------|-------------|--------------|
| `atomspace-cog` | CogServer storage backend | atomspace |
| `atomspace-rocks` | RocksDB storage backend | atomspace |
| `atomspace-pgres` | PostgreSQL storage backend | atomspace |

**Build Commands** (parallel):
```bash
# Terminal 1
cd opencog-debian/atomspace-cog && ./update-atomspace-cog.sh && cd atomspace-cog-* && dpkg-buildpackage -rfakeroot -us -uc

# Terminal 2
cd opencog-debian/atomspace-rocks && ./update-atomspace-rocks.sh && cd atomspace-rocks-* && dpkg-buildpackage -rfakeroot -us -uc

# Terminal 3
cd opencog-debian/atomspace-pgres && ./update-atomspace-pgres.sh && cd atomspace-pgres-* && dpkg-buildpackage -rfakeroot -us -uc
```

---

### Stage 4: Core Services

**Build Type**: **Parallel** (can build simultaneously)  
**Estimated Time**: 15 minutes (parallel) / 30 minutes (sequential)

| Package | Description | Dependencies |
|---------|-------------|--------------|
| `cogserver` | Network server | atomspace |
| `ure` | Unified Rule Engine | atomspace |

---

### Stage 5: Cognitive Components

**Build Type**: **Parallel** (can build simultaneously)  
**Estimated Time**: 15 minutes (parallel) / 60 minutes (sequential)

| Package | Description | Dependencies |
|---------|-------------|--------------|
| `attention` | ECAN attention allocation | atomspace, ure |
| `pln` | Probabilistic Logic Networks | atomspace, ure |
| `miner` | Pattern mining | atomspace, ure |
| `unify` | Unification framework | atomspace |
| `spacetime` | Spatiotemporal reasoning | atomspace |

---

### Stage 6: Learning and Generation

**Build Type**: **Parallel** (can build simultaneously)  
**Estimated Time**: 15 minutes (parallel) / 27 minutes (sequential)

| Package | Description | Dependencies |
|---------|-------------|--------------|
| `learn` | Language learning | atomspace |
| `generate` | Natural language generation | atomspace |

---

### Stage 7: NLP Components

**Build Type**: **Parallel** (can build simultaneously)  
**Estimated Time**: 15 minutes (parallel) / 25 minutes (sequential)

| Package | Description | Dependencies |
|---------|-------------|--------------|
| `lg-atomese` | Link Grammar integration | atomspace |
| `relex` | Relation extraction | atomspace |

---

### Stage 8: Specialized Systems

**Build Type**: **Parallel** (can build simultaneously)  
**Estimated Time**: 20 minutes (parallel) / 48 minutes (sequential)

| Package | Description | Dependencies |
|---------|-------------|--------------|
| `moses` | Evolutionary optimization | atomspace |
| `asmoses` | AtomSpace-based MOSES | atomspace, moses |
| `agi-bio` | Bioinformatics applications | atomspace |
| `vision` | Vision processing | atomspace |

---

### Stage 9: OpenCog Meta-Package

**Build Type**: Sequential  
**Estimated Time**: 5 minutes

| Package | Description | Dependencies |
|---------|-------------|--------------|
| `opencog` | Meta-package for all OpenCog | All above packages |

**Build Commands**:
```bash
cd opencog-debian/opencog
./update-opencog.sh
cd opencog-*
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog_*.deb
```

---

### Stage 10: HurdCog Layer 1 Core (Optional - AGI-OS only) 🆕

**Build Type**: Sequential (strict dependency order)  
**Estimated Time**: 25 minutes

| Package | Description | Dependencies |
|---------|-------------|--------------|
| `hurdcog` | Base HurdCog OS | cognumach |
| `hurdcog-machspace` | OS-level AtomSpace integration | hurdcog, atomspace, cognumach |
| `hurdcog-cogkernel-core` | Core cognitive kernel | hurdcog-machspace, atomspace |
| `hurdcog-occ-bridge` | OCC-HurdCog integration bridge | hurdcog-machspace, hurdcog-cogkernel-core, atomspace |

**Build Commands** (must be sequential):
```bash
# 1. Base HurdCog OS
cd opencog-debian/hurdcog
./update-hurdcog.sh
cd hurdcog-*
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../*.deb

# 2. MachSpace (depends on hurdcog)
cd ../../hurdcog-machspace
./update-hurdcog-machspace.sh
cd hurdcog-machspace-*
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../*.deb

# 3. CogKernel Core (depends on machspace)
cd ../../hurdcog-cogkernel-core
./update-hurdcog-cogkernel-core.sh
cd hurdcog-cogkernel-core-*
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../*.deb

# 4. OCC Bridge (depends on cogkernel-core)
cd ../../hurdcog-occ-bridge
./update-hurdcog-occ-bridge.sh
cd hurdcog-occ-bridge-*
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../*.deb
```

**Package Details**:

#### hurdcog-machspace
- **Purpose**: Distributed hypergraph memory for OS-level cognition
- **Key Components**:
  - `machspace.scm` - Distributed hypergraph implementation
  - `cognitive-grip.scm` - 5-finger cognitive grip mechanism
  - `machspace-bridge.scm` - Mach IPC integration
- **Solves**: GNU Hurd's 5 fundamental problems through cognitive architecture

#### hurdcog-cogkernel-core
- **Purpose**: Core cognitive kernel infrastructure
- **Key Components**:
  - `hurdcog-bootstrap.scm` - Minimal bootstrap system
  - `cognitive-interface/` - Decision-making and workflow
  - Phase integration modules (phase1, phase2, phase3)
- **Provides**: Foundation for all cognitive subsystems

#### hurdcog-occ-bridge
- **Purpose**: Three-layer cognitive synergy integration
- **Key Components**:
  - `atomspace-hurdcog-bridge.py` - Python bridge
  - `cognumach.scm` - Microkernel interface
  - `hurdcog.scm` - OS interface
  - `agi-os-synergy.scm` - Unified integration framework
- **Enables**: Cognitive fusion across all three layers

---

### Stage 13: AGI-OS Unified (Optional - AGI-OS only)

**Build Type**: Sequential  
**Estimated Time**: 2 minutes

| Package | Description | Dependencies |
|---------|-------------|--------------|
| `agi-os-unified` | Complete AGI-OS meta-package | All packages |

**Build Commands**:
```bash
cd opencog-debian/agi-os-unified
./update-agi-os-unified.sh
cd agi-os-unified-*
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../agi-os-unified_*.deb
```

---

## Automated Build

Use the provided build script to automate the entire process:

```bash
cd opencog-debian

# Build OpenCog Collection only (24 packages)
./build-all-packages.sh

# Build complete AGI-OS stack (27 packages)
BUILD_AGI_OS=yes ./build-all-packages.sh

# Custom parallel jobs
PARALLEL_JOBS=8 BUILD_AGI_OS=yes ./build-all-packages.sh

# Build without installing
INSTALL_PACKAGES=no ./build-all-packages.sh
```

---

## Build Time Estimates

### OpenCog Collection Only (24 packages)

| Configuration | Total Time |
|---------------|------------|
| Sequential | ~4 hours |
| Parallel (4 cores) | ~2 hours |
| Parallel (8 cores) | ~1.5 hours |

### Complete AGI-OS Stack (27 packages)

| Configuration | Total Time |
|---------------|------------|
| Sequential | ~4.5 hours |
| Parallel (4 cores) | ~2.5 hours |
| Parallel (8 cores) | ~2 hours |

---

## Dependency Graph

```
System Libraries
│
├─→ cognumach (Stage 0) [AGI-OS]
│   └─→ hurdcog (Stage 10) [AGI-OS]
│       └─→ hurdcog-machspace (Stage 10) [AGI-OS] 🆕
│           └─→ hurdcog-cogkernel-core (Stage 10) [AGI-OS] 🆕
│               └─→ hurdcog-occ-bridge (Stage 10) [AGI-OS] 🆕
│
└─→ libcogutil-dev (Stage 1)
    └─→ opencog-atomspace (Stage 2)
        ├─→ Storage Backends (Stage 3) [parallel]
        │   ├─→ opencog-atomspace-cog
        │   ├─→ opencog-atomspace-rocks
        │   └─→ opencog-atomspace-pgres
        │
        ├─→ Core Services (Stage 4) [parallel]
        │   ├─→ opencog-cogserver
        │   └─→ opencog-ure
        │
        ├─→ Cognitive Components (Stage 5) [parallel]
        │   ├─→ opencog-attention
        │   ├─→ opencog-pln
        │   ├─→ opencog-miner
        │   ├─→ opencog-unify
        │   └─→ opencog-spacetime
        │
        ├─→ Learning & Generation (Stage 6) [parallel]
        │   ├─→ opencog-learn
        │   └─→ opencog-generate
        │
        ├─→ NLP Components (Stage 7) [parallel]
        │   ├─→ opencog-lg-atomese
        │   └─→ opencog-relex
        │
        └─→ Specialized Systems (Stage 8) [parallel]
            ├─→ opencog-moses
            ├─→ opencog-asmoses
            ├─→ opencog-agi-bio
            └─→ opencog-vision
                └─→ opencog (Stage 9)
                    └─→ agi-os-unified (Stage 13) [AGI-OS]
```

---

## Installation

After building all packages, install the complete stack:

```bash
# Install OpenCog Collection
sudo apt-get install opencog

# Install complete AGI-OS
sudo apt-get install agi-os-unified

# Initialize AGI-OS
sudo agi-os-init

# Check status
agi-os-status
```

---

## Validation

Validate all package structures before building:

```bash
cd opencog-debian
./validate-packaging.sh
```

Expected output:
```
Total Packages: 27
Valid Packages: 27
Invalid Packages: 0
✓ All packages have valid structure
```

---

## Troubleshooting

### Missing Dependencies

If a package fails due to missing dependencies:
```bash
cd package-name-*
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
```

### Clean Build

To start fresh:
```bash
cd opencog-debian/package-name
rm -rf package-name-*/
./update-package-name.sh
```

### Check Logs

Build logs are stored in `opencog-debian/build-logs/`:
```bash
tail -f build-logs/build-all-*.log
tail -f build-logs/package-name-*.log
```

---

## Notes

1. **Stage 10 is critical**: The HurdCog Layer 1 Core packages must be built in strict sequential order due to tight dependencies.

2. **Phase 1 packages are foundational**: hurdcog-machspace, hurdcog-cogkernel-core, and hurdcog-occ-bridge provide the essential infrastructure for all future HurdCog cognitive subsystems.

3. **Future phases**: Stages 11 and 12 are reserved for Phase 2 (cognitive services) and Phase 3 (advanced features) packages when implemented.

4. **Parallel builds**: Use `PARALLEL_JOBS` environment variable to control parallelism. Default is `$(nproc)`.

5. **AGI-OS optional**: Set `BUILD_AGI_OS=yes` to include cognumach, hurdcog, and all cognitive subsystems. Otherwise, only OpenCog Collection is built.

---

**Last Updated**: November 30, 2025  
**Total Packages**: 27 (24 existing + 3 new Phase 1)  
**New in This Version**: Phase 1 HurdCog subcomponent packages
