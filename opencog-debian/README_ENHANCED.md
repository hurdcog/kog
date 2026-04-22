# OpenCog Debian Packaging System

## Overview

This directory contains the complete Debian packaging infrastructure for the **OpenCog Collection (OCC)** and the **AGI Operating System (AGI-OS)** stack. The packaging system is production-ready and provides automated builds, dependency management, and continuous integration support.

## Architecture

The packaging system supports a **three-layer AGI-OS architecture**:

### Layer 0: Cognumach Microkernel
Enhanced GNU Mach microkernel with cognitive features for advanced memory management, SMP support, and optimized IPC.

### Layer 1: HurdCog Cognitive OS
OpenCog-powered GNU Hurd system with cognitive translators, MachSpace integration, and the Master Control Dashboard.

### Layer 2: OpenCog Collection
Complete AGI research platform with AtomSpace, PLN, ECAN, pattern mining, language learning, and evolutionary optimization.

## Package Structure

```
opencog-debian/
├── cogutil/              # Foundation library (Layer 1)
├── atomspace/            # Core hypergraph database (Layer 2)
├── atomspace-cog/        # CogServer storage backend
├── atomspace-rocks/      # RocksDB storage backend
├── atomspace-pgres/      # PostgreSQL storage backend
├── cogserver/            # Network server
├── ure/                  # Unified Rule Engine
├── pln/                  # Probabilistic Logic Networks
├── attention/            # Economic Attention Networks
├── miner/                # Pattern mining
├── unify/                # Unification framework
├── spacetime/            # Spatiotemporal reasoning
├── learn/                # Language learning
├── generate/             # Natural language generation
├── lg-atomese/           # Link Grammar integration
├── relex/                # Relation extraction
├── moses/                # Evolutionary optimization
├── asmoses/              # AtomSpace-based MOSES
├── agi-bio/              # Bioinformatics applications
├── vision/               # Vision processing
├── opencog/              # Meta-package (all OpenCog)
├── cognumach/            # Microkernel (AGI-OS Layer 0)
├── hurdcog/              # Cognitive OS (AGI-OS Layer 1)
└── agi-os-unified/       # Complete AGI-OS stack
```

## Quick Start

### Prerequisites

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    debhelper \
    cmake \
    devscripts \
    fakeroot \
    lintian \
    git
```

### Build All Packages

```bash
cd opencog-debian
./build-all-packages.sh
```

This will:
1. Build packages in correct dependency order
2. Install build dependencies automatically
3. Run parallel builds where possible
4. Generate detailed logs
5. Install packages after building (optional)

### Build Options

```bash
# Custom parallel jobs
PARALLEL_JOBS=8 ./build-all-packages.sh

# Build without installing
INSTALL_PACKAGES=no ./build-all-packages.sh

# Build including AGI-OS components
BUILD_AGI_OS=yes ./build-all-packages.sh
```

### Build Individual Package

```bash
cd opencog-debian/cogutil
./update-cogutil.sh
cd cogutil-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../libcogutil-dev_*.deb
```

## Build Order

Packages must be built in dependency order:

1. **Stage 1**: `cogutil` (foundation)
2. **Stage 2**: `atomspace` (core)
3. **Stage 3**: `atomspace-cog`, `atomspace-rocks`, `atomspace-pgres` (parallel)
4. **Stage 4**: `cogserver`, `ure` (parallel)
5. **Stage 5**: `attention`, `pln`, `miner`, `unify`, `spacetime` (parallel)
6. **Stage 6**: `learn`, `generate` (parallel)
7. **Stage 7**: `lg-atomese`, `relex` (parallel)
8. **Stage 8**: `moses`, `asmoses`, `agi-bio`, `vision` (parallel)
9. **Stage 9**: `opencog` (meta-package)

For AGI-OS:
- **Stage 0**: `cognumach` (microkernel)
- **Stage 1.5**: `hurdcog` (cognitive OS)
- **Stage 10**: `agi-os-unified` (unified stack)

See [BUILD_ORDER.md](BUILD_ORDER.md) for detailed instructions.

## Validation

Validate all package structures:

```bash
cd opencog-debian
./validate-packaging.sh
```

This checks:
- Debian control files
- Changelog format
- Rules executability
- Update scripts
- Source format
- Copyright files

## Continuous Integration

The repository includes GitHub Actions workflows for automated builds:

```yaml
.github/workflows/debian-packaging.yml
```

The CI pipeline:
1. Validates package structure
2. Builds packages in stages
3. Runs lintian checks
4. Tests installation
5. Creates release artifacts
6. Publishes to package repository (on main branch)

### Running CI Locally

```bash
cd opencog-debian
./ci-build.sh
```

## Installation

### Install Complete OpenCog Collection

```bash
sudo apt-get install opencog
```

This installs all OpenCog components.

### Install AGI-OS Stack

```bash
sudo apt-get install agi-os-unified
```

This installs:
- Cognumach microkernel
- HurdCog cognitive OS
- Complete OpenCog Collection
- Integration tools

### Install Individual Components

```bash
# Foundation
sudo apt-get install libcogutil-dev

# AtomSpace with storage
sudo apt-get install opencog-atomspace opencog-atomspace-rocks

# Reasoning
sudo apt-get install opencog-pln opencog-ure

# Learning
sudo apt-get install opencog-learn opencog-generate
```

## Configuration

### Initialize AGI-OS

After installing `agi-os-unified`:

```bash
sudo agi-os-init
```

This creates:
- `/etc/agi-os/` - Configuration directory
- `/var/lib/agi-os/` - Data directory
- `/var/log/agi-os/` - Log directory

### Check Status

```bash
agi-os-status
```

Shows status of all AGI-OS layers and components.

### Configure AtomSpace

Edit `/etc/agi-os/config.scm`:

```scheme
;; Storage backend
(define atomspace-config
  '((storage-type . "rocksdb")
    (storage-path . "/var/lib/agi-os/atomspace")
    (cache-size . 1000000)))
```

## Development

### Package Maintenance

Each package directory contains:

```
package-name/
├── debian/
│   ├── control          # Package metadata
│   ├── rules            # Build instructions
│   ├── changelog        # Version history
│   ├── copyright        # License information
│   ├── compat           # Debhelper version
│   └── source/format    # Source format
└── update-package.sh    # Source preparation script
```

### Adding New Package

1. Create package directory:
   ```bash
   mkdir opencog-debian/new-component
   ```

2. Create `debian/` subdirectory with required files

3. Create `update-new-component.sh` script:
   ```bash
   #!/bin/bash
   set -e
   PACKAGE_NAME="new-component"
   VERSION="1.0.0"
   # ... preparation logic
   ```

4. Update `BUILD_ORDER.md` with dependencies

5. Add to `build-all-packages.sh` in appropriate stage

6. Validate:
   ```bash
   ./validate-packaging.sh
   ```

### Testing Changes

Test package build:

```bash
cd opencog-debian/package-name
./update-package-name.sh
cd package-name-<version>
dpkg-buildpackage -rfakeroot -us -uc
```

Test installation:

```bash
sudo dpkg -i ../package-name_*.deb
```

## Documentation

### Core Documentation
- [PACKAGING_ARCHITECTURE.md](PACKAGING_ARCHITECTURE.md) - Package hierarchy and structure
- [BUILD_ORDER.md](BUILD_ORDER.md) - Detailed build instructions
- [AGI_OS_INTEGRATION.md](AGI_OS_INTEGRATION.md) - AGI-OS integration guide

### External Resources
- [Debian Policy Manual](https://www.debian.org/doc/debian-policy/)
- [Debian New Maintainers' Guide](https://www.debian.org/doc/manuals/maint-guide/)
- [OpenCog Wiki](https://wiki.opencog.org/)

## Troubleshooting

### Build Failures

**Missing dependencies:**
```bash
sudo apt-get build-dep .
```

**Parallel build issues:**
```bash
PARALLEL_JOBS=1 ./build-all-packages.sh
```

**Clean build:**
```bash
rm -rf package-name-*/
./update-package-name.sh
```

### Installation Issues

**Dependency conflicts:**
```bash
sudo apt-get install -f
```

**Package removal:**
```bash
sudo apt-get remove --purge package-name
```

### Runtime Issues

**Check logs:**
```bash
tail -f /var/log/agi-os/system.log
```

**Verify services:**
```bash
systemctl status cogserver
systemctl status hurdcog-dashboard
```

## Contributing

We welcome contributions! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

### Code Style

- Follow Debian packaging guidelines
- Use consistent naming conventions
- Document all changes
- Update relevant documentation files

### Testing

Before submitting:

```bash
# Validate packaging
./validate-packaging.sh

# Test build
./build-all-packages.sh

# Run CI locally
./ci-build.sh
```

## Support

For support and questions:

- **GitHub Issues**: https://github.com/cogpy/occ/issues
- **Discussions**: https://github.com/cogpy/occ/discussions
- **Mailing List**: opencog@googlegroups.com
- **IRC**: #opencog on Libera.Chat

## License

The packaging infrastructure is released under AGPL-3.0. Individual packages may have different licenses - see each package's `debian/copyright` file.

## Maintainers

- Primary Maintainer: OpenCog Developers <opencog@googlegroups.com>
- VCS: https://github.com/cogpy/occ

## Version History

### Current Release
- Complete packaging for all OpenCog components
- AGI-OS integration (Cognumach, HurdCog)
- Automated build system
- CI/CD integration
- Comprehensive documentation

### Recent Improvements
- Added `agi-os-unified` meta-package
- Enhanced package descriptions
- Automated build scripts
- GitHub Actions workflows
- Integration documentation

## Roadmap

### Short Term
- [ ] APT repository hosting
- [ ] GPG signing infrastructure
- [ ] Additional storage backends
- [ ] Performance optimizations

### Long Term
- [ ] Cross-platform support (RPM packaging)
- [ ] Docker container images
- [ ] Kubernetes deployment
- [ ] Cloud-native integration

---

**OpenCog Collection** - Advancing Artificial General Intelligence through Cognitive Synergy
