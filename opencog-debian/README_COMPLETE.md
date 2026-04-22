# OpenCog Debian Packaging Infrastructure - Complete Guide

## Overview

This directory contains the complete, production-ready Debian packaging infrastructure for the **OpenCog Collection (OCC)**, including integration with **cognumach** (microkernel) and **hurdcog** (cognitive OS) to create a unified **AGI Operating System**.

## Quick Start

### Build All Packages

```bash
# Simple build (OpenCog components only)
./build-all-packages.sh

# Enhanced build with parallel support
./build-all-enhanced.sh -j 8

# Build including AGI-OS components
./build-all-enhanced.sh --agi-os
```

### Build Individual Package

```bash
cd cogutil
./update-cogutil.sh
cd cogutil-<version>
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../libcogutil-dev_*.deb
```

## Infrastructure Components

### 1. Package Directories

Each package has its own directory with:
- `debian/` - Standard Debian packaging files
- `update-<package>.sh` - Source preparation script
- `README.md` - Package-specific documentation

### 2. Build Scripts

#### build-all-packages.sh
Original build script with sequential processing.

```bash
./build-all-packages.sh
```

#### build-all-enhanced.sh
Enhanced build script with:
- Parallel builds per stage
- Colored output and logging
- Error handling and recovery
- Build reports

```bash
# Build with 8 parallel jobs
./build-all-enhanced.sh -j 8

# Build AGI-OS components
./build-all-enhanced.sh --agi-os

# Skip tests
./build-all-enhanced.sh --skip-tests

# Custom log directory
./build-all-enhanced.sh --log-dir /tmp/build-logs
```

### 3. Validation Tools

#### validate-packaging.sh
Validates all package structures.

```bash
./validate-packaging.sh
```

Checks:
- ✓ debian/control exists
- ✓ debian/rules exists and is executable
- ✓ debian/changelog exists
- ✓ debian/compat exists
- ✓ debian/copyright exists
- ✓ debian/source/format exists
- ✓ update script exists and is executable

### 4. Dependency Management

#### resolve-dependencies.sh
Analyzes and manages package dependencies.

```bash
# Display build order
./resolve-dependencies.sh order

# Generate Makefile
./resolve-dependencies.sh makefile

# Generate CMakeLists.txt
./resolve-dependencies.sh cmake

# Generate dependency graph
./resolve-dependencies.sh graph
dot -Tpng dependencies.dot -o dependencies.png

# Check for missing packages
./resolve-dependencies.sh check
```

## Package Architecture

### Three-Layer AGI-OS Stack

```
┌─────────────────────────────────────┐
│   Layer 3: OpenCog Collection      │
│   (AGI Framework)                   │
├─────────────────────────────────────┤
│   Layer 2: HurdCog                  │
│   (Cognitive Operating System)      │
├─────────────────────────────────────┤
│   Layer 1: Cognumach                │
│   (Enhanced Microkernel)            │
├─────────────────────────────────────┤
│   Layer 0: System Libraries         │
└─────────────────────────────────────┘
```

### Build Stages

#### Stage 0: Microkernel Foundation
- **cognumach** - Enhanced GNU Mach microkernel

#### Stage 1: Foundation Library
- **cogutil** - Low-level C++ utilities

#### Stage 2: Core AtomSpace
- **atomspace** - Hypergraph knowledge representation

#### Stage 3: Storage Backends (Parallel)
- **atomspace-cog** - CogServer storage backend
- **atomspace-rocks** - RocksDB storage backend
- **atomspace-pgres** - PostgreSQL storage backend

#### Stage 4: Core Services (Parallel)
- **cogserver** - Network server for AtomSpace
- **ure** - Unified Rule Engine

#### Stage 4.5: Cognitive OS Layer (Parallel)
- **hurdcog** - Main cognitive OS package
- **hurdcog-cogkernel-core** - Core cognitive kernel services
- **hurdcog-machspace** - AtomSpace-microkernel bridge
- **hurdcog-occ-bridge** - OpenCog Collection bridge

#### Stage 5: Cognitive Components (Parallel)
- **attention** - Economic Attention Networks (ECAN)
- **pln** - Probabilistic Logic Networks
- **miner** - Pattern mining system
- **unify** - Unification framework
- **spacetime** - Spatiotemporal reasoning

#### Stage 6: Learning and Generation (Parallel)
- **learn** - Language learning system
- **generate** - Natural language generation

#### Stage 7: Natural Language Processing (Parallel)
- **lg-atomese** - Link Grammar integration
- **relex** - Relation extraction system

#### Stage 8: Evolutionary and Specialized (Parallel)
- **moses** - Meta-Optimizing Semantic Evolutionary Search
- **asmoses** - AtomSpace-based MOSES
- **agi-bio** - Bioinformatics AGI applications
- **vision** - Vision processing components

#### Stage 9: Meta-Package
- **opencog** - Complete OpenCog framework

#### Stage 10: AGI-OS Unified
- **agi-os-unified** - Complete AGI Operating System stack

## Documentation

### Core Documentation Files

1. **PACKAGING_ARCHITECTURE.md** - Overall architecture and design
2. **BUILD_ORDER.md** - Detailed build instructions
3. **AGI_OS_INTEGRATION.md** - AGI-OS integration details
4. **AGI_OS_IMPLEMENTATION.md** - Implementation guide
5. **README.md** - General information
6. **README_ENHANCED.md** - Enhanced features
7. **README_COMPLETE.md** - This file

### Package-Specific Documentation

Each package directory contains:
- Package README
- Debian control file with detailed description
- Changelog with version history

## Continuous Integration

### GitHub Actions Workflow

Located at `.github/workflows/debian-packaging.yml`

Features:
- Automated validation on push/PR
- Multi-stage parallel builds
- Integration testing
- Package artifact uploads
- Automatic release publishing

### Local CI Testing

```bash
# Run validation
./validate-packaging.sh

# Run dependency checks
./resolve-dependencies.sh check

# Test build order
./resolve-dependencies.sh order
```

## Advanced Usage

### Parallel Builds

Build multiple packages simultaneously within each stage:

```bash
# Use all CPU cores
./build-all-enhanced.sh -j $(nproc)

# Use specific number of cores
./build-all-enhanced.sh -j 4
```

### Custom Build Order

Generate and use custom Makefile:

```bash
./resolve-dependencies.sh makefile
make -f Makefile.packages -j4
```

### Dependency Visualization

Create visual dependency graph:

```bash
./resolve-dependencies.sh graph
dot -Tpng dependencies.dot -o dependencies.png
xdg-open dependencies.png
```

### Docker Builds

Use the provided Dockerfile:

```bash
docker build -t opencog-builder .
docker run -v $(pwd):/workspace opencog-builder ./build-all-packages.sh
```

## Troubleshooting

### Common Issues

#### 1. Missing Build Dependencies

**Error**: `dpkg-checkbuilddeps: error: Unmet build dependencies`

**Solution**:
```bash
cd <package>/<package>-<version>
sudo apt-get build-dep .
```

#### 2. Source Directory Not Found

**Error**: Update script fails to create source directory

**Solution**:
```bash
cd <package>
./update-<package>.sh
# Check for errors in git clone or tarball extraction
```

#### 3. Build Failures

**Error**: Package build fails

**Solution**:
```bash
# Check build log
cat logs/<package>.log

# Try building manually with verbose output
cd <package>/<package>-<version>
dpkg-buildpackage -rfakeroot -us -uc -j1 2>&1 | tee build.log
```

#### 4. Dependency Installation Failures

**Error**: `dpkg: dependency problems prevent installation`

**Solution**:
```bash
sudo apt-get install -f
```

### Debug Mode

Enable verbose logging:

```bash
export DEBUG=1
./build-all-enhanced.sh
```

## Testing

### Package Testing

Each package should include tests:

```bash
cd <package>/<package>-<version>
dpkg-buildpackage -rfakeroot -us -uc
# Tests run automatically during build
```

### Integration Testing

Test complete stack:

```bash
./test-integration.sh
```

### Lintian Checks

Check package quality:

```bash
cd <package>
lintian ../*.deb
```

## Maintenance

### Updating Packages

1. Update source repository reference in `update-<package>.sh`
2. Update version in `debian/changelog`
3. Test build:
   ```bash
   cd <package>
   ./update-<package>.sh
   cd <package>-<version>
   dpkg-buildpackage -rfakeroot -us -uc
   ```
4. Commit changes

### Adding New Packages

1. Create package directory:
   ```bash
   mkdir new-package
   cd new-package
   ```

2. Create debian/ structure:
   ```bash
   mkdir -p debian/source
   touch debian/{control,rules,changelog,compat,copyright}
   echo "3.0 (quilt)" > debian/source/format
   chmod +x debian/rules
   ```

3. Create update script:
   ```bash
   cat > update-new-package.sh << 'EOF'
   #!/bin/bash
   # Update script for new-package
   git clone https://github.com/opencog/new-package.git
   cd new-package
   VERSION=$(git describe --tags --abbrev=0)
   # ... rest of script
   EOF
   chmod +x update-new-package.sh
   ```

4. Update dependency resolver:
   Edit `resolve-dependencies.sh` and add package to dependency map

5. Test and validate:
   ```bash
   ./validate-packaging.sh
   ./resolve-dependencies.sh check
   ```

## Performance Optimization

### Build Performance

- Use parallel builds: `-j $(nproc)`
- Use ccache for faster rebuilds
- Use tmpfs for build directory (if enough RAM)

### Package Size Optimization

- Strip debug symbols (done automatically)
- Use appropriate compression (xz for .deb files)
- Split large packages into -dev, -doc, -data subpackages

## Security

### Package Signing

Sign packages for distribution:

```bash
# Generate GPG key
gpg --gen-key

# Build and sign
dpkg-buildpackage -rfakeroot
```

### Vulnerability Scanning

Scan packages for known vulnerabilities:

```bash
# Install scanner
sudo apt-get install clamav

# Scan packages
clamscan *.deb
```

## Distribution

### Creating Repository

Set up APT repository:

```bash
# Install repository tools
sudo apt-get install reprepro

# Create repository structure
mkdir -p repo/conf
cat > repo/conf/distributions << EOF
Codename: opencog
Components: main
Architectures: i386 amd64 source
EOF

# Add packages
reprepro -b repo includedeb opencog *.deb
```

### Publishing

1. Upload to package hosting service
2. Configure APT sources:
   ```bash
   echo "deb https://packages.opencog.org/debian opencog main" | \
       sudo tee /etc/apt/sources.list.d/opencog.list
   ```

## Contributing

### Guidelines

1. Follow Debian Policy Manual
2. Test all changes
3. Update documentation
4. Run validation before committing
5. Use semantic versioning

### Pull Request Process

1. Fork repository
2. Create feature branch
3. Make changes
4. Run tests and validation
5. Submit pull request
6. Address review comments

## Resources

### Documentation
- [Debian Policy Manual](https://www.debian.org/doc/debian-policy/)
- [Debian New Maintainers' Guide](https://www.debian.org/doc/manuals/maint-guide/)
- [OpenCog Wiki](https://wiki.opencog.org/)

### Tools
- debhelper - Debian packaging helper
- lintian - Package quality checker
- pbuilder - Clean build environment
- sbuild - Schroot-based builder

### Community
- GitHub: https://github.com/cogpy/occ
- Mailing List: opencog@googlegroups.com
- IRC: #opencog on Libera.Chat

## License

All packaging infrastructure is licensed under AGPL-3.0, consistent with OpenCog licensing.

## Status

**Current Status**: Production-ready ✓

- ✓ All 27 packages have complete debian/ directories
- ✓ All packages validated successfully
- ✓ Build order resolved and tested
- ✓ Dependency graph generated
- ✓ CI/CD pipeline configured
- ✓ Documentation complete
- ✓ AGI-OS integration implemented

**Last Updated**: 2025-12-01

## Changelog

### Version 2.0.0 (2025-12-01)
- Complete packaging infrastructure for all OpenCog components
- AGI-OS integration (cognumach, hurdcog)
- Enhanced build scripts with parallel support
- Comprehensive documentation
- CI/CD pipeline
- Dependency resolution tools
- Validation framework

### Version 1.0.0 (Previous)
- Initial packaging structure
- Basic build scripts
- Core component packages

---

**For questions or issues, please open an issue on GitHub or contact the maintainers.**
