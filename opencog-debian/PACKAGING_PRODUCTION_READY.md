# OpenCog Debian Packaging - Production Ready

## Overview

This directory contains production-ready Debian packaging for the complete OpenCog Collection (OCC), including the AGI-OS stack (Cognumach microkernel and HurdCog operating system).

**Last Updated**: 2025-12-16  
**Status**: Production Ready  
**Maintainer**: OpenCog Developers

## Architecture

The packaging follows a three-layer AGI-OS architecture:

```
Layer 0: Build Tools
  └── MIG (Mach Interface Generator) - centralized in build-tools/mig/

Layer 1: Cognumach (Microkernel)
  ├── cognumach - GNU Mach microkernel with cognitive extensions
  └── cognumach-cognitive-scheduler - Cognitive process scheduler

Layer 2: HurdCog (Operating System)
  ├── hurdcog - GNU Hurd + cognitive extensions
  ├── hurdcog-cogkernel-core - Core cognitive kernel services
  ├── hurdcog-atomspace-bridge - AtomSpace integration
  ├── hurdcog-machspace - MachSpace (AtomSpace at OS level)
  └── hurdcog-occ-bridge - OCC integration bridge

Layer 3: OCC (AGI Framework)
  ├── Core Foundation
  │   ├── cogutil - Utilities library
  │   └── atomspace - Hypergraph knowledge representation
  │
  ├── Storage Backends (CRITICAL: Built before CogServer)
  │   ├── atomspace-storage - Base storage interface
  │   ├── atomspace-rocks - RocksDB backend
  │   ├── atomspace-cog - CogServer storage backend
  │   ├── atomspace-pgres - PostgreSQL backend
  │   └── atomspace-machspace - MachSpace integration
  │
  ├── Core Services
  │   ├── cogserver - Network server
  │   └── ure - Unified Rule Engine
  │
  ├── Cognitive Components
  │   ├── attention - Attention allocation
  │   ├── pln - Probabilistic Logic Networks
  │   ├── miner - Pattern mining
  │   ├── unify - Unification framework
  │   └── spacetime - Spatiotemporal reasoning
  │
  ├── Learning & Generation
  │   ├── learn - Symbolic learning
  │   └── generate - Language generation
  │
  ├── Natural Language Processing
  │   ├── lg-atomese - Link Grammar integration
  │   └── relex - Relationship extraction
  │
  ├── Specialized Systems
  │   ├── moses - Evolutionary program learning
  │   ├── asmoses - AtomSpace MOSES
  │   ├── agi-bio - Bioinformatics
  │   └── vision - Computer vision
  │
  └── Meta-Packages
      ├── opencog - Complete OCC installation
      ├── agi-os-unified - Full AGI-OS stack
      ├── agi-os-cognitive-init - Cognitive initialization
      └── agi-os-monitoring - System monitoring
```

## Critical Build Order

### Dependency Chain

The build order is **strictly enforced** to ensure correct dependency resolution:

1. **cogutil** (no dependencies)
2. **atomspace** (depends: cogutil)
3. **atomspace-storage** (depends: atomspace) ← **MUST BUILD BEFORE COGSERVER**
4. **atomspace-rocks, atomspace-cog, atomspace-pgres** (depends: atomspace-storage)
5. **cogserver** (depends: atomspace, atomspace-storage)
6. All other components (depend on atomspace and/or cogserver)

### Why atomspace-storage Must Build Before cogserver

The `cogserver` package has a **hard dependency** on `atomspace-storage` because:

1. CogServer uses storage backends for persistence
2. The storage interface is required at compile time
3. Runtime linking requires storage libraries to be present

**Violation of this order will cause build failures.**

## Quick Start

### Build All Packages (Userspace Only)

```bash
cd opencog-debian
./build-all-production.sh
```

### Build Full AGI-OS Stack

```bash
cd opencog-debian
BUILD_AGI_OS=yes ./build-all-production.sh
```

### Build Specific Package

```bash
cd opencog-debian/cogutil
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../*.deb
```

## Build Scripts

### build-all-production.sh (NEW - Recommended)

**Features**:
- Enforces correct build order
- Supports full AGI-OS stack
- Colored output for clarity
- Automatic dependency installation
- Error handling and recovery
- Build artifact organization

**Usage**:
```bash
# Userspace only (default)
./build-all-production.sh

# Full AGI-OS stack
BUILD_AGI_OS=yes ./build-all-production.sh

# Custom build directory
BUILD_DIR=/custom/path ./build-all-production.sh

# Don't auto-install
INSTALL_DEBS=no ./build-all-production.sh

# Parallel jobs
PARALLEL_JOBS=8 ./build-all-production.sh
```

### build-all-enhanced.sh (Legacy)

Original enhanced build script, still functional but superseded by `build-all-production.sh`.

### build-all-packages.sh (Legacy)

Original build script, kept for compatibility.

## Package Status

### Production Ready ✓

All core packages are production-ready with complete debian packaging:

- ✓ cogutil
- ✓ atomspace
- ✓ atomspace-storage
- ✓ atomspace-rocks
- ✓ atomspace-cog
- ✓ atomspace-pgres
- ✓ cogserver
- ✓ ure
- ✓ pln
- ✓ attention
- ✓ miner
- ✓ unify
- ✓ spacetime
- ✓ learn
- ✓ generate
- ✓ lg-atomese
- ✓ relex
- ✓ moses
- ✓ asmoses
- ✓ agi-bio
- ✓ vision
- ✓ opencog (meta-package)

### AGI-OS Packages ✓

Full AGI-OS stack packages are complete:

- ✓ cognumach
- ✓ cognumach-cognitive-scheduler
- ✓ hurdcog
- ✓ hurdcog-cogkernel-core
- ✓ hurdcog-atomspace-bridge
- ✓ hurdcog-machspace
- ✓ hurdcog-occ-bridge
- ✓ atomspace-machspace
- ✓ agi-os-unified
- ✓ agi-os-cognitive-init
- ✓ agi-os-monitoring

## Integration with Main Repository

### MIG Centralization

The Mach Interface Generator (MIG) has been centralized:

**Location**: `../build-tools/mig/`

**Symlinks**:
- `../cognumach/mig -> ../build-tools/mig`
- `../hurdcog/mig -> ../build-tools/mig`

**Benefits**:
- Single source of truth
- Consistent version across all components
- Simplified maintenance
- Follows layered architecture

### CMakeLists.txt Integration

The root `CMakeLists.txt` has been updated to:

1. **Enforce atomspace-storage before cogserver**:
   ```cmake
   # AtomSpace Storage - MUST be built before CogServer
   OPTION(BUILD_ATOMSPACE_STORAGE "Build AtomSpace storage backends" ON)
   IF(BUILD_ATOMSPACE_STORAGE AND BUILD_ATOMSPACE)
       add_subdirectory(atomspace-storage)
       add_subdirectory(atomspace-rocks)
       add_subdirectory(atomspace-cog)
   ENDIF()
   
   IF(BUILD_COGSERVER AND BUILD_ATOMSPACE AND BUILD_ATOMSPACE_STORAGE)
       add_subdirectory(cogserver)
   ENDIF()
   ```

2. **Reference centralized MIG**:
   ```cmake
   # Layer 0: Build Tools - MIG (Mach Interface Generator)
   IF(BUILD_COGNUMACH OR BUILD_HURDCOG)
       MESSAGE(STATUS "Building Layer 0: MIG (Mach Interface Generator)...")
       MESSAGE(STATUS "  Location: build-tools/mig/")
   ENDIF()
   ```

3. **Support AGI-OS stack**:
   ```cmake
   OPTION(BUILD_COGNUMACH "Build Cognumach microkernel - Layer 1" OFF)
   OPTION(BUILD_HURDCOG "Build HurdCog OS - Layer 2" OFF)
   ```

## Continuous Integration

### GitHub Actions

The repository includes CI workflows for:

1. **Debian Package Building** (`.github/workflows/debian-packages.yml`)
   - Builds all packages in correct order
   - Tests installation
   - Uploads artifacts

2. **Windows Builds** (`.github/workflows/chocolatey-package.yml`)
   - Builds Windows binaries
   - Creates Chocolatey packages
   - Publishes to Chocolatey (on release)

### Local Testing

```bash
# Test package build
cd opencog-debian/cogutil
dpkg-buildpackage -rfakeroot -us -uc

# Test package installation
sudo dpkg -i ../libcogutil-dev_*.deb

# Validate package
lintian ../libcogutil-dev_*.deb
```

## Troubleshooting

### Build Dependency Issues

```bash
# Install all build dependencies
cd opencog-debian/cogutil
sudo apt-get build-dep .

# If build-dep fails, install manually
sudo apt-get install debhelper cmake libboost-dev ...
```

### Package Conflicts

```bash
# Remove old packages
sudo apt-get remove --purge opencog-*

# Clean build directory
rm -rf /tmp/opencog-build/*

# Rebuild from scratch
./build-all-production.sh
```

### AGI-OS Build Issues

```bash
# Ensure 32-bit support (for i386 packages)
sudo dpkg --add-architecture i386
sudo apt-get update

# Install cross-compilation tools
sudo apt-get install gcc-multilib g++-multilib

# Build MIG first
cd ../build-tools/mig
./configure
make
sudo make install
```

## Contributing

### Adding New Packages

1. Create package directory: `opencog-debian/new-package/`
2. Create debian files:
   ```bash
   cd opencog-debian/new-package
   dh_make --createorig
   ```
3. Edit `debian/control` with dependencies
4. Add to `build-all-production.sh` in correct stage
5. Test build:
   ```bash
   dpkg-buildpackage -rfakeroot -us -uc
   ```

### Updating Existing Packages

1. Update source in main repository
2. Update `debian/changelog`:
   ```bash
   dch -i "Description of changes"
   ```
3. Rebuild package
4. Test installation

## References

- [Debian New Maintainers' Guide](https://www.debian.org/doc/manuals/maint-guide/)
- [Debian Policy Manual](https://www.debian.org/doc/debian-policy/)
- [OpenCog Wiki](https://wiki.opencog.org/)
- [AGI-OS Architecture](./AGI_OS_INTEGRATION.md)
- [Build Order Documentation](./BUILD_ORDER.md)

## Support

For issues or questions:
- GitHub Issues: https://github.com/cogpy/occ/issues
- Mailing List: opencog@googlegroups.com
- IRC: #opencog on Libera.Chat

---

**Production Ready**: All core components have complete packaging and are ready for deployment.
