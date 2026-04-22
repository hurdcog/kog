# AGI-OS/OCC Documentation Index

This directory contains comprehensive documentation for the AGI-OS/OCC project.

## Core Documentation

### [BUILD_SEQUENCES.md](BUILD_SEQUENCES.md)
**Complete build dependency sequences and order**

Detailed documentation of the build dependency graph for all three layers:
- Layer 1: Cognumach components and build sequence
- Layer 2: HurdCog components and build sequence  
- Layer 3: OCC components with dependency tree
- Complete AGI-OS stack build order
- Visual dependency graphs
- Version consistency table
- Build method comparisons
- Integration bridges

**When to use:** 
- Planning builds
- Understanding component dependencies
- Troubleshooting build order issues
- Adding new components

### [VERSION_MANAGEMENT.md](VERSION_MANAGEMENT.md)
**Version standardization and management guide**

Comprehensive guide to version management across the repository:
- Version standardization policy
- Version declaration methods
- Version checking and verification
- Update workflows
- Synchronization groups
- Semantic versioning guidelines
- Tools and automation

**When to use:**
- Updating component versions
- Checking version consistency
- Planning releases
- Understanding version strategy

## Build System Documentation

### Root Directory Files

#### [CMakeLists.txt](../CMakeLists.txt)
Root CMake configuration with:
- Three-layer architecture integration
- Build options for all components
- Proper build dependency order
- Configuration summary

#### [Makefile](../Makefile)
Unified build interface with:
- Guix build support
- Direct build support
- CMake build sequences (via Makefile.build-sequences)
- Test targets
- Installation targets

#### [Makefile.build-sequences](../Makefile.build-sequences)
CMake build sequences including:
- Layer-by-layer builds
- Component-specific builds
- Dependency-aware builds
- Complete stack build

## Component-Specific Documentation

### Layer 1: Cognumach
- [cognumach/CMakeLists.txt](../cognumach/CMakeLists.txt) - CMake wrapper for autotools build
- Version: 1.8.0 (GNU Mach)

### Layer 2: HurdCog
- [hurdcog/CMakeLists.txt](../hurdcog/CMakeLists.txt) - CMake wrapper for autotools + cognitive extensions
- Version: 0.9.0 (GNU Hurd)

### Layer 3: OCC
All OCC components have individual CMakeLists.txt files with standardized versions.
See [BUILD_SEQUENCES.md](BUILD_SEQUENCES.md) for complete list.

## Tools and Scripts

### [scripts/standardize-versions.py](../scripts/standardize-versions.py)
**Version standardization tool**

Python script to:
- Report current versions across all components
- Identify version inconsistencies
- Apply version updates according to policy
- Generate version reports

Usage:
```bash
# Generate version report
python3 scripts/standardize-versions.py --report

# Dry run (show what would change)
python3 scripts/standardize-versions.py

# Apply changes interactively
python3 scripts/standardize-versions.py --apply
```

## Quick Reference

### Building the Complete Stack

**Option 1: Guix (Recommended)**
```bash
make all
# or
guix build -f guix.scm
```

**Option 2: CMake Direct**
```bash
make cmake-agi-os-stack
```

**Option 3: Traditional Autotools**
```bash
make direct-build
```

### Common Tasks

**Check build status:**
```bash
make status
```

**Run tests:**
```bash
make test
./synergy.sh
```

**Check versions:**
```bash
python3 scripts/standardize-versions.py --report
```

**Build specific layer:**
```bash
make cmake-cognumach  # Layer 1
make cmake-hurdcog    # Layer 2
make cmake-occ-full   # Layer 3
```

**Build specific component:**
```bash
make cmake-foundation  # cogutil + atomspace
make cmake-reasoning   # unify, ure, pln, miner
make cmake-agents      # agents + sensory
```

## Architecture Overview

```
AGI-OS
├── Layer 1: Cognumach (v1.8.0)
│   └── Cognitive Microkernel (GNU Mach)
│
├── Layer 2: HurdCog (v0.9.0)
│   ├── Core Servers (GNU Hurd)
│   └── Cognitive Kernel Extensions
│
└── Layer 3: OCC (various versions)
    ├── Foundation (cogutil, atomspace)
    ├── Storage Backends
    ├── Network Layer (cogserver)
    ├── Math Layer (matrix)
    ├── Reasoning (unify, ure, pln, miner)
    ├── Learning (learn)
    ├── Attention (ECAN)
    ├── Agents (agents, sensory)
    └── Cognitive Architecture (coggml, cogself, etc.)
```

## Additional Resources

### External Documentation
- [OpenCog Wiki](https://wiki.opencog.org/)
- [GNU Mach Manual](https://www.gnu.org/software/hurd/gnumach-doc/)
- [GNU Hurd Manual](https://www.gnu.org/software/hurd/doc/)
- [CMake Documentation](https://cmake.org/documentation/)
- [GNU Guix Manual](https://guix.gnu.org/manual/)

### Repository Documentation
- [README.md](../README.md) - Project overview
- [CLAUDE.md](../CLAUDE.md) - AI assistant guide
- [CONTRIBUTING.md](../CONTRIBUTING.md) - Contribution guidelines

## Maintenance

### Updating This Index

When adding new documentation:

1. Add entry to appropriate section
2. Include brief description
3. Add "When to use" guidance
4. Update Quick Reference if needed
5. Test all links

### Documentation Standards

- Use Markdown format
- Include table of contents for long documents
- Add code examples where applicable
- Keep version information current
- Cross-reference related documents
- Include last updated date

---

**Last Updated:** 2025-12-06  
**Documentation Version:** 1.0.0  
**Maintainer:** AGI-OS Development Team
