# OpenCog Debian Packaging Architecture

## Overview

This document defines the complete Debian packaging infrastructure for the OpenCog Collection (OCC), including integration with cognumach and hurdcog for a unified AGI Operating System.

## Package Hierarchy

### Layer 1: Foundation (No Dependencies)

```
libcogutil-dev
├── Description: Low-level C++ utilities library
├── Dependencies: None (only system libraries)
└── Build Order: 1
```

### Layer 2: Core AtomSpace

```
opencog-atomspace
├── Description: Hypergraph knowledge representation system
├── Dependencies: libcogutil-dev
└── Build Order: 2
```

### Layer 3: Storage Backends

```
opencog-atomspace-cog
├── Description: CogServer storage backend for AtomSpace
├── Dependencies: opencog-atomspace
└── Build Order: 3

opencog-atomspace-rocks
├── Description: RocksDB storage backend for AtomSpace
├── Dependencies: opencog-atomspace, librocksdb-dev
└── Build Order: 3

opencog-atomspace-pgres (NEW)
├── Description: PostgreSQL storage backend for AtomSpace
├── Dependencies: opencog-atomspace, libpq-dev
└── Build Order: 3
```

### Layer 4: Core Services

```
opencog-cogserver
├── Description: Network server for AtomSpace access
├── Dependencies: opencog-atomspace
└── Build Order: 4

opencog-ure
├── Description: Unified Rule Engine
├── Dependencies: opencog-atomspace
└── Build Order: 4
```

### Layer 5: Cognitive Components

```
opencog-attention
├── Description: Economic Attention Networks (ECAN)
├── Dependencies: opencog-atomspace, opencog-cogserver
└── Build Order: 5

opencog-pln
├── Description: Probabilistic Logic Networks
├── Dependencies: opencog-atomspace, opencog-ure
└── Build Order: 5

opencog-miner
├── Description: Pattern mining system
├── Dependencies: opencog-atomspace, opencog-ure
└── Build Order: 5

opencog-unify
├── Description: Unification framework
├── Dependencies: opencog-atomspace
└── Build Order: 5

opencog-spacetime
├── Description: Spatiotemporal reasoning
├── Dependencies: opencog-atomspace
└── Build Order: 5
```

### Layer 6: Learning and Generation

```
opencog-learn
├── Description: Language learning system
├── Dependencies: opencog-atomspace, opencog-atomspace-rocks, opencog-ure
└── Build Order: 6

opencog-generate
├── Description: Natural language generation
├── Dependencies: opencog-atomspace, opencog-ure
└── Build Order: 6
```

### Layer 7: Natural Language Processing

```
opencog-lg-atomese
├── Description: Link Grammar integration
├── Dependencies: opencog-atomspace, link-grammar
└── Build Order: 7

opencog-relex
├── Description: Relation extraction system
├── Dependencies: link-grammar, Java runtime
└── Build Order: 7
```

### Layer 8: Evolutionary and Specialized Systems

```
opencog-moses
├── Description: Meta-Optimizing Semantic Evolutionary Search
├── Dependencies: libcogutil-dev, opencog-atomspace
└── Build Order: 8

opencog-asmoses
├── Description: AtomSpace-based MOSES
├── Dependencies: opencog-moses, opencog-atomspace
└── Build Order: 8

opencog-agi-bio
├── Description: Bioinformatics AGI applications
├── Dependencies: opencog-atomspace, opencog-pln, opencog-ure
└── Build Order: 8

opencog-vision
├── Description: Vision processing components
├── Dependencies: opencog-atomspace
└── Build Order: 8
```

### Layer 9: Meta-Package

```
opencog
├── Description: Complete OpenCog framework (meta-package)
├── Dependencies: All above packages
└── Build Order: 9
```

## AGI-OS Integration Packages

### Layer 0: Microkernel Foundation

```
cognumach (NEW)
├── Description: Enhanced GNU Mach microkernel with cognitive features
├── Dependencies: gcc-multilib, mig, binutils
├── Build Order: 0
└── Package Type: Binary kernel package
```

### Layer 1.5: Cognitive Operating System

```
hurdcog (NEW)
├── Description: OpenCog-powered GNU Hurd cognitive OS
├── Dependencies: cognumach, libcogutil-dev, opencog-atomspace
├── Build Order: 1.5
└── Package Type: OS infrastructure package
```

### Layer 10: Unified AGI-OS

```
agi-os-unified (NEW)
├── Description: Complete AGI Operating System stack
├── Dependencies: cognumach, hurdcog, opencog
├── Build Order: 10
└── Package Type: Meta-package with integration tools
```

## Build Dependency Graph

```
                                    ┌─────────────┐
                                    │ System Libs │
                                    └──────┬──────┘
                                           │
                        ┌──────────────────┼──────────────────┐
                        │                  │                  │
                   ┌────▼────┐      ┌─────▼──────┐    ┌─────▼─────┐
                   │cognumach│      │libcogutil  │    │link-grammar│
                   └────┬────┘      │   -dev     │    └─────┬─────┘
                        │           └─────┬──────┘          │
                   ┌────▼────┐            │                 │
                   │ hurdcog │            │                 │
                   └────┬────┘            │                 │
                        │           ┌─────▼──────┐          │
                        │           │  atomspace │          │
                        │           └─────┬──────┘          │
                        │                 │                 │
                        │        ┌────────┼────────┐        │
                        │        │        │        │        │
                        │   ┌────▼───┐ ┌──▼───┐ ┌─▼────┐   │
                        │   │ -cog   │ │-rocks│ │-pgres│   │
                        │   └────┬───┘ └──┬───┘ └─┬────┘   │
                        │        │        │       │        │
                        │   ┌────▼────────▼───────▼────┐   │
                        │   │    cogserver / ure       │   │
                        │   └────┬─────────────────────┘   │
                        │        │                         │
                        │   ┌────▼─────────────────────┐   │
                        │   │ attention/pln/miner/     │   │
                        │   │ unify/spacetime          │   │
                        │   └────┬─────────────────────┘   │
                        │        │                         │
                        │   ┌────▼─────────────────────┐   │
                        │   │   learn / generate       │   │
                        │   └────┬─────────────────────┘   │
                        │        │                         │
                        │   ┌────▼─────────┐               │
                        │   │ lg-atomese   │◄──────────────┘
                        │   │   relex      │
                        │   └────┬─────────┘
                        │        │
                        │   ┌────▼─────────────────────┐
                        │   │ moses/asmoses/agi-bio/   │
                        │   │      vision              │
                        │   └────┬─────────────────────┘
                        │        │
                        │   ┌────▼────┐
                        │   │ opencog │
                        │   └────┬────┘
                        │        │
                        └────────┼────────┐
                                 │        │
                          ┌──────▼────────▼──────┐
                          │  agi-os-unified      │
                          └──────────────────────┘
```

## Package Naming Convention

- **Library packages**: `lib<component>-dev` (e.g., `libcogutil-dev`)
- **Application packages**: `opencog-<component>` (e.g., `opencog-atomspace`)
- **Meta-packages**: `opencog` (main), `agi-os-unified` (complete system)
- **System packages**: `cognumach`, `hurdcog`

## Build Scripts

Each package directory should contain:

1. **debian/** - Standard Debian packaging files
   - `control` - Package metadata and dependencies
   - `rules` - Build instructions
   - `changelog` - Version history
   - `copyright` - License information
   - `compat` - Debhelper compatibility level
   - `source/format` - Source package format

2. **update-<component>.sh** - Script to prepare source package
   - Clone/update source repository
   - Create orig tarball
   - Copy debian/ directory
   - Prepare for build

## Testing Strategy

### Package-Level Tests
- Each package should have unit tests
- Integration tests for inter-package dependencies
- Lintian checks for package quality

### System-Level Tests
- Full stack installation test
- Component interaction verification
- Performance benchmarks
- Cognitive synergy validation

## Continuous Integration

### Build Pipeline
1. Dependency order validation
2. Parallel builds where possible
3. Package installation tests
4. Integration test suite
5. Repository publication

### Quality Gates
- All packages must build successfully
- No lintian errors (warnings acceptable with justification)
- All tests must pass
- Documentation must be complete

## Repository Structure

```
opencog-debian/
├── PACKAGING_ARCHITECTURE.md (this file)
├── BUILD_ORDER.md (detailed build instructions)
├── README.md (general information)
├── Dockerfile (build environment)
├── cogutil/
│   ├── debian/
│   └── update-cogutil.sh
├── atomspace/
│   ├── debian/
│   └── update-atomspace.sh
├── atomspace-cog/
│   ├── debian/
│   └── update-atomspace-cog.sh
├── atomspace-rocks/
│   ├── debian/
│   └── update-atomspace-rocks.sh
├── atomspace-pgres/ (NEW)
│   ├── debian/
│   └── update-atomspace-pgres.sh
├── cogserver/
│   ├── debian/
│   └── update-cogserver.sh
├── ure/
│   ├── debian/
│   └── update-ure.sh
├── attention/
│   ├── debian/
│   └── update-attention.sh
├── pln/
│   ├── debian/
│   └── update-pln.sh
├── miner/
│   ├── debian/
│   └── update-miner.sh
├── unify/
│   ├── debian/
│   └── update-unify.sh
├── spacetime/
│   ├── debian/
│   └── update-spacetime.sh
├── learn/
│   ├── debian/
│   └── update-learn.sh
├── generate/
│   ├── debian/
│   └── update-generate.sh
├── lg-atomese/
│   ├── debian/
│   └── update-lg-atomese.sh
├── relex/
│   ├── debian/
│   └── update-relex.sh
├── moses/
│   ├── debian/
│   └── update-moses.sh
├── asmoses/
│   ├── debian/
│   └── update-asmoses.sh
├── agi-bio/
│   ├── debian/
│   └── update-agi-bio.sh
├── vision/
│   ├── debian/
│   └── update-vision.sh
├── opencog/
│   ├── debian/
│   └── update-opencog.sh
├── cognumach/ (NEW)
│   ├── debian/
│   └── update-cognumach.sh
├── hurdcog/ (NEW)
│   ├── debian/
│   └── update-hurdcog.sh
└── agi-os-unified/ (NEW)
    ├── debian/
    └── update-agi-os-unified.sh
```

## Version Management

- Use semantic versioning: MAJOR.MINOR.PATCH
- Debian revision: -1, -2, etc.
- Full version example: `2.0.0-1`
- Epoch for major incompatible changes: `1:2.0.0-1`

## Maintainer Information

- Primary Maintainer: As specified in individual control files
- Uploaders: Contributors with upload rights
- VCS: Git repositories on GitHub

## License Compliance

- All packages must be DFSG-compliant
- License files in debian/copyright
- GPL-compatible licenses preferred
- Clear attribution for all components

## Future Enhancements

1. **Automated Build System**
   - Jenkins or GitLab CI integration
   - Automated dependency resolution
   - Parallel build optimization

2. **Package Repository**
   - APT repository hosting
   - GPG signing infrastructure
   - Mirror network

3. **Cross-Platform Support**
   - Ubuntu LTS versions
   - Debian stable/testing/unstable
   - Potential RPM packaging

4. **Documentation**
   - Man pages for all binaries
   - Developer documentation
   - API reference guides

## References

- [Debian Policy Manual](https://www.debian.org/doc/debian-policy/)
- [Debian New Maintainers' Guide](https://www.debian.org/doc/manuals/maint-guide/)
- [OpenCog Wiki](https://wiki.opencog.org/)
- [GNU Hurd Documentation](https://www.gnu.org/software/hurd/)
