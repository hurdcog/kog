# AGI-OS Build Guide

## Complete Build Instructions for the Three-Layer Cognitive Architecture

**Version:** 1.0.0  
**Date:** 2025-11-14  
**Status:** Production Ready

---

## Table of Contents

1. [Introduction](#introduction)
2. [Prerequisites](#prerequisites)
3. [Quick Start](#quick-start)
4. [Build Methods](#build-methods)
5. [Layer-by-Layer Build](#layer-by-layer-build)
6. [Testing](#testing)
7. [Installation](#installation)
8. [Troubleshooting](#troubleshooting)
9. [Advanced Topics](#advanced-topics)

---

## Introduction

The AGI-OS is a three-layer cognitive architecture integrating:

- **Layer 1: Cognumach** - Cognitive Microkernel (GNU Mach with cognitive enhancements)
- **Layer 2: HurdCog** - Cognitive Operating System (GNU Hurd with CogKernel)
- **Layer 3: OCC** - AGI Framework (OpenCog Collection with synergy infrastructure)

This guide provides complete instructions for building the entire stack from source.

---

## Prerequisites

### System Requirements

**Minimum:**
- CPU: x86_64 or i686 architecture
- RAM: 4 GB
- Disk: 10 GB free space
- OS: Linux (Ubuntu 20.04+ recommended)

**Recommended:**
- CPU: Multi-core x86_64 (4+ cores)
- RAM: 8 GB or more
- Disk: 20 GB free space
- OS: Ubuntu 22.04 LTS or Debian 11+

### Required Software

**Build Tools:**
```bash
sudo apt-get install -y \
    build-essential \
    gcc-multilib \
    binutils \
    autoconf \
    automake \
    libtool \
    pkg-config \
    gawk \
    bison \
    flex \
    texinfo \
    git \
    curl \
    wget
```

**Language Runtimes:**
```bash
sudo apt-get install -y \
    guile-3.0 \
    guile-3.0-dev \
    python3 \
    python3-pip \
    python3-dev \
    python3-psutil
```

**Build Systems:**
```bash
sudo apt-get install -y \
    cmake \
    make
```

**Optional (for Guix builds):**
```bash
# Install GNU Guix
curl https://git.savannah.gnu.org/cgit/guix.git/plain/etc/guix-install.sh | sudo bash
```

### Automated Setup

The easiest way to install all prerequisites:

```bash
cd /path/to/occ
make dev-setup
```

This will automatically install all required dependencies.

---

## Quick Start

### One-Command Build

For the fastest path from clone to working system:

```bash
# Clone the repository
git clone https://github.com/cogpy/occ.git
cd occ

# Complete setup, build, and test
make quickstart
```

This single command will:
1. Install all development dependencies
2. Configure all layers
3. Build the complete AGI-OS stack
4. Run integration tests
5. Display next steps

**Time:** Approximately 15-30 minutes depending on your system.

### Step-by-Step Quick Start

If you prefer more control:

```bash
# 1. Clone repository
git clone https://github.com/cogpy/occ.git
cd occ

# 2. Install dependencies
make dev-setup

# 3. Build everything
make all

# 4. Run tests
make test

# 5. View status
make status
```

---

## Build Methods

The AGI-OS supports two build methods:

### Method 1: GNU Guix Build (Recommended)

**Advantages:**
- Reproducible builds
- Automatic dependency management
- Isolated build environment
- Binary caching

**Requirements:**
- GNU Guix installed

**Usage:**
```bash
make guix-build
```

**What it does:**
- Builds using the unified Guix package definition
- Handles all dependencies automatically
- Produces reproducible outputs
- Caches build artifacts

### Method 2: Direct Build (Development)

**Advantages:**
- Faster iteration during development
- Direct access to build artifacts
- Easier debugging
- No Guix dependency

**Requirements:**
- All prerequisites installed manually

**Usage:**
```bash
make direct-build
```

**What it does:**
- Configures each layer individually
- Builds using native build systems
- Places artifacts in source directories
- Suitable for active development

---

## Layer-by-Layer Build

You can build each layer independently:

### Layer 1: Cognumach (Cognitive Microkernel)

**Build:**
```bash
make cognumach
```

**What it does:**
1. Runs `autoreconf` if needed
2. Configures with `./configure --host=i686-gnu`
3. Builds with `make -j$(nproc)`

**Output:**
- Microkernel binary: `cognumach/gnumach`
- Build artifacts in `cognumach/`

**Manual build:**
```bash
cd cognumach
autoreconf -vif
./configure --host=i686-gnu CC='gcc -m32' LD='ld -melf_i386' --enable-kdb
make -j$(nproc)
```

### Layer 2: HurdCog (Cognitive Operating System)

**Build:**
```bash
make hurdcog
```

**What it does:**
1. Configures HurdCog build system
2. Builds Hurd servers
3. Builds CogKernel (Scheme-based cognitive layer)

**Output:**
- Hurd servers in `hurdcog/*/`
- CogKernel in `hurdcog/cogkernel/`

**Manual build:**
```bash
cd hurdcog
# If Makefile exists
make -j$(nproc)

# Or build CogKernel only
cd cogkernel
make -j$(nproc)
```

### Layer 3: OCC (AGI Framework)

**Build:**
```bash
make occ
```

**What it does:**
1. Builds synergy integration infrastructure
2. Validates metamodel Scheme modules
3. Compiles Python bridges

**Components:**
- Synergy bridges: `synergy/bridges/`
- Metamodel: `metamodel/`

**Manual build:**
```bash
# Build synergy
cd synergy
python3 -m py_compile bridges/agi_os_bridge.py
python3 -m py_compile bridges/scheme_python_ffi.py

# Validate metamodel
cd ../metamodel
for file in *.scm; do
    guile --no-auto-compile -c "(load \"$file\")"
done
```

### Integration Infrastructure

**Build synergy only:**
```bash
make synergy
```

**Build metamodel only:**
```bash
make metamodel
```

---

## Testing

### Run All Tests

```bash
make test
```

This runs:
- Synergy integration tests
- Cross-layer integration tests
- Component validation

### Test Individual Layers

**Test Cognumach:**
```bash
make test-cognumach
```

**Test HurdCog:**
```bash
make test-hurdcog
```

**Test OCC:**
```bash
make test-occ
```

**Test Synergy:**
```bash
make test-synergy
```

### Integration Tests

```bash
make test-integration
```

Validates:
- Cross-layer communication
- AGI-OS bridge functionality
- Scheme-Python FFI
- Component integration

### Validation and Verification

**Validate build outputs:**
```bash
make validate
```

**Verify system integrity:**
```bash
make verify
```

Checks:
- Directory structure
- No .git directories (monorepo integrity)
- All components present

---

## Installation

### Install Complete System

```bash
make install
```

**Default installation prefix:** `/usr/local/agi-os`

This installs:
- Cognumach microkernel
- HurdCog operating system
- OCC synergy infrastructure
- Metamodel foundation

### Install Individual Layers

**Install Cognumach only:**
```bash
make install-cognumach
```

**Install HurdCog only:**
```bash
make install-hurdcog
```

**Install OCC only:**
```bash
make install-occ
```

### Custom Installation Prefix

Edit the Makefile and change:
```makefile
INSTALL_PREFIX := /your/custom/path
```

Or override at build time:
```bash
make install INSTALL_PREFIX=/opt/agi-os
```

---

## Troubleshooting

### Common Issues

#### 1. "GNU Guix not found"

**Problem:** Trying to use `make guix-build` without Guix installed.

**Solution:**
```bash
# Option 1: Install Guix
curl https://git.savannah.gnu.org/cgit/guix.git/plain/etc/guix-install.sh | sudo bash

# Option 2: Use direct build
make direct-build
```

#### 2. "configure: command not found" in Cognumach

**Problem:** Autotools not run yet.

**Solution:**
```bash
cd cognumach
autoreconf -vif
./configure --host=i686-gnu CC='gcc -m32' LD='ld -melf_i386'
```

#### 3. "gcc: error: unrecognized command line option '-m32'"

**Problem:** 32-bit compilation support not installed.

**Solution:**
```bash
sudo apt-get install gcc-multilib
```

#### 4. "guile: command not found"

**Problem:** Guile Scheme not installed.

**Solution:**
```bash
sudo apt-get install guile-3.0 guile-3.0-dev
```

#### 5. Python import errors

**Problem:** Missing Python dependencies.

**Solution:**
```bash
sudo apt-get install python3-psutil
```

#### 6. Build fails with "No space left on device"

**Problem:** Insufficient disk space.

**Solution:**
```bash
# Clean build artifacts
make clean-all

# Check disk space
df -h
```

### Getting Help

**View system status:**
```bash
make status
```

**View build information:**
```bash
make info
```

**View all available targets:**
```bash
make help
```

**Check prerequisites:**
```bash
make dev-setup
```

---

## Advanced Topics

### Parallel Builds

By default, the Makefile uses all available CPU cores. To limit:

```bash
# Use 4 cores
make -j4 all

# Or edit Makefile
NPROC := 4
```

### Cross-Compilation

Cognumach supports cross-compilation:

```bash
cd cognumach
./configure --host=i686-gnu --build=x86_64-linux-gnu \
    CC='gcc -m32' LD='ld -melf_i386'
make
```

### Development Workflow

**Typical development cycle:**

```bash
# 1. Make changes to source code
vim cognumach/kern/sched_prim.c

# 2. Rebuild affected layer
make cognumach

# 3. Test changes
make test-cognumach

# 4. Run integration tests
make test-integration

# 5. Commit changes
git add -A
git commit -m "Improve scheduler"
```

### Incremental Builds

The Makefile supports incremental builds:

```bash
# First build (full)
make all

# Make changes
vim hurdcog/cogkernel/attention.scm

# Rebuild only affected components
make hurdcog

# Or rebuild specific component
make metamodel
```

### Clean Builds

**Clean build artifacts:**
```bash
make clean
```

**Deep clean (including configure):**
```bash
make clean-all
```

**Clean specific layer:**
```bash
make clean-cognumach
make clean-hurdcog
make clean-occ
```

### Build Customization

**Enable debug symbols:**
```bash
cd cognumach
./configure --host=i686-gnu CC='gcc -m32 -g' --enable-kdb
make
```

**Optimize for size:**
```bash
cd cognumach
./configure --host=i686-gnu CC='gcc -m32 -Os'
make
```

### Continuous Integration

The Makefile is designed for CI/CD:

```bash
# CI build script
#!/bin/bash
set -e

# Setup
make dev-setup

# Build
make all

# Test
make test

# Validate
make validate

# Package (if using Guix)
make guix-build
```

---

## Build Targets Reference

### Primary Targets

| Target | Description |
|--------|-------------|
| `all` | Build complete AGI-OS stack (default) |
| `help` | Show all available targets |
| `info` | Display build system information |
| `status` | Show build status for all layers |

### Build Targets

| Target | Description |
|--------|-------------|
| `guix-build` | Build using GNU Guix (recommended) |
| `direct-build` | Build directly without Guix |
| `cognumach` | Build Layer 1: Cognitive Microkernel |
| `hurdcog` | Build Layer 2: Cognitive OS |
| `occ` | Build Layer 3: AGI Framework |
| `synergy` | Build synergy integration |
| `metamodel` | Build metamodel foundation |

### Configuration Targets

| Target | Description |
|--------|-------------|
| `configure` | Configure all layers |
| `configure-cognumach` | Configure Cognumach |
| `configure-hurdcog` | Configure HurdCog |
| `configure-occ` | Configure OCC |

### Testing Targets

| Target | Description |
|--------|-------------|
| `test` | Run all tests |
| `test-cognumach` | Test Cognumach |
| `test-hurdcog` | Test HurdCog |
| `test-occ` | Test OCC |
| `test-synergy` | Test synergy integration |
| `test-integration` | Run integration tests |
| `validate` | Validate build outputs |
| `verify` | Verify system integrity |

### Installation Targets

| Target | Description |
|--------|-------------|
| `install` | Install complete AGI-OS |
| `install-cognumach` | Install Cognumach only |
| `install-hurdcog` | Install HurdCog only |
| `install-occ` | Install OCC only |

### Maintenance Targets

| Target | Description |
|--------|-------------|
| `clean` | Clean build artifacts |
| `clean-all` | Deep clean (including configure) |
| `clean-cognumach` | Clean Cognumach |
| `clean-hurdcog` | Clean HurdCog |
| `clean-occ` | Clean OCC |

### Development Targets

| Target | Description |
|--------|-------------|
| `dev-setup` | Install development dependencies |
| `docs` | Generate documentation |
| `quickstart` | Complete setup + build + test |

---

## Build Time Estimates

Approximate build times on a modern system (4-core CPU, 8 GB RAM):

| Target | Time |
|--------|------|
| `make dev-setup` | 2-5 minutes |
| `make cognumach` | 3-10 minutes |
| `make hurdcog` | 5-15 minutes |
| `make occ` | 1-2 minutes |
| `make all` (direct-build) | 10-30 minutes |
| `make guix-build` | 15-45 minutes (first time) |
| `make test` | 1-2 minutes |
| `make quickstart` | 15-35 minutes |

**Note:** Guix builds are slower initially but benefit from caching on subsequent builds.

---

## Next Steps

After successfully building the AGI-OS:

1. **Explore the Architecture:**
   ```bash
   cat docs/AGI_OS_ARCHITECTURE.md
   ```

2. **Test Integration:**
   ```bash
   python3 synergy/bridges/agi_os_bridge.py
   python3 synergy/bridges/scheme_python_ffi.py
   ```

3. **View System Status:**
   ```bash
   make status
   ```

4. **Read Documentation:**
   - Architecture: `docs/AGI_OS_ARCHITECTURE.md`
   - Integration: `MONOREPO_INTEGRATION.md`
   - Research: `RESEARCH_FINDINGS.md`

5. **Start Development:**
   - Modify source code in any layer
   - Rebuild with `make <layer>`
   - Test with `make test-<layer>`
   - Commit changes

---

## Support and Resources

### Documentation

- **Architecture:** `/docs/AGI_OS_ARCHITECTURE.md`
- **Integration:** `/MONOREPO_INTEGRATION.md`
- **Research:** `/RESEARCH_FINDINGS.md`
- **This Guide:** `/docs/BUILD_GUIDE.md`

### Getting Help

- **GitHub Issues:** https://github.com/cogpy/occ/issues
- **GitHub Discussions:** https://github.com/cogpy/occ/discussions

### Related Projects

- **GNU Hurd:** https://hurd.gnu.org/
- **GNU Mach:** https://www.gnu.org/software/gnumach/
- **OpenCog:** https://opencog.org/
- **GNU Guix:** https://guix.gnu.org/

---

## Contributing

Contributions to the AGI-OS project are welcome! Please see:

- `CONTRIBUTING.md` (if available)
- GitHub Issues for known issues
- GitHub Discussions for questions

---

**Document Version:** 1.0.0  
**Last Updated:** 2025-11-14  
**Maintained By:** AGI-OS Development Team
