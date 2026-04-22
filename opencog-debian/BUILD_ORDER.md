# OpenCog Debian Package Build Order

## Overview

This document provides the definitive build order for all OpenCog Debian packages, including the AGI-OS integration components (cognumach and hurdcog).

## Prerequisites

### System Requirements
- Debian sid (unstable) or Ubuntu 22.04+
- 8GB+ RAM recommended
- 20GB+ free disk space
- Multi-core processor for parallel builds

### Build Tools
```bash
sudo apt-get install build-essential debhelper cmake git \
    devscripts fakeroot lintian
```

## Complete Build Order

### Stage 0: Microkernel Foundation (Optional for AGI-OS)

#### 0.1 cognumach
**Source**: `../cognumach/`  
**Dependencies**: System libraries only  
**Build Time**: ~30 minutes  

```bash
cd cognumach
./update-cognumach.sh
cd cognumach-<version>
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../cognumach_*.deb
```

**Notes**: 
- Requires 32-bit build tools for i686 target
- Kernel package, not required for userspace OpenCog

---

### Stage 1: Foundation Library

#### 1.1 libcogutil-dev
**Source**: `../cogutil/`  
**Dependencies**: None (system libraries only)  
**Build Time**: ~10 minutes  

```bash
cd cogutil
./update-cogutil.sh
cd cogutil-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../libcogutil-dev_*.deb
```

**Critical**: This must be built first as all other packages depend on it.

---

### Stage 2: Core AtomSpace

#### 2.1 opencog-atomspace
**Source**: `../atomspace/`  
**Dependencies**: libcogutil-dev  
**Build Time**: ~20 minutes  

```bash
cd atomspace
./update-atomspace.sh
cd atomspace-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-atomspace_*.deb
```

**Critical**: Core hypergraph database, required by almost all other components.

---

### Stage 3: Storage Backends (Parallel Build Possible)

These packages can be built in parallel as they only depend on atomspace.

#### 3.1 opencog-atomspace-cog
**Source**: `../atomspace-cog/`  
**Dependencies**: opencog-atomspace  
**Build Time**: ~10 minutes  

```bash
cd atomspace-cog
./update-atomspace-cog.sh
cd atomspace-cog-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-atomspace-cog_*.deb
```

#### 3.2 opencog-atomspace-rocks
**Source**: `../atomspace-rocks/`  
**Dependencies**: opencog-atomspace, librocksdb-dev  
**Build Time**: ~10 minutes  

```bash
cd atomspace-rocks
./update-atomspace-rocks.sh
cd atomspace-rocks-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-atomspace-rocks_*.deb
```

#### 3.3 opencog-atomspace-pgres (To Be Created)
**Source**: `../atomspace-pgres/`  
**Dependencies**: opencog-atomspace, libpq-dev  
**Build Time**: ~10 minutes  
**Status**: Package structure to be created

---

### Stage 4: Core Services (Parallel Build Possible)

#### 4.1 opencog-cogserver
**Source**: `../cogserver/`  
**Dependencies**: opencog-atomspace  
**Build Time**: ~15 minutes  

```bash
cd cogserver
./update-cogserver.sh
cd cogserver-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-cogserver_*.deb
```

#### 4.2 opencog-ure
**Source**: `../ure/`  
**Dependencies**: opencog-atomspace  
**Build Time**: ~15 minutes  

```bash
cd ure
./update-ure.sh
cd ure-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-ure_*.deb
```

**Critical**: URE is required by many reasoning components.

---

### Stage 4.5: Cognitive OS Layer (Optional for AGI-OS)

#### 4.5.1 hurdcog
**Source**: `../hurdcog/`  
**Dependencies**: cognumach, libcogutil-dev, opencog-atomspace  
**Build Time**: ~45 minutes  
**Status**: Package structure to be created

```bash
cd hurdcog
./update-hurdcog.sh
cd hurdcog-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../hurdcog_*.deb
```

---

### Stage 5: Cognitive Components (Parallel Build Possible)

All packages in this stage depend on atomspace and/or ure.

#### 5.1 opencog-attention
**Source**: `../attention/`  
**Dependencies**: opencog-atomspace, opencog-cogserver  
**Build Time**: ~12 minutes  

```bash
cd attention
./update-attention.sh
cd attention-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-attention_*.deb
```

#### 5.2 opencog-pln
**Source**: `../pln/`  
**Dependencies**: opencog-atomspace, opencog-ure  
**Build Time**: ~15 minutes  

```bash
cd pln
./update-pln.sh
cd pln-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-pln_*.deb
```

#### 5.3 opencog-miner
**Source**: `../miner/`  
**Dependencies**: opencog-atomspace, opencog-ure  
**Build Time**: ~12 minutes  

```bash
cd miner
./update-miner.sh
cd miner-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-miner_*.deb
```

#### 5.4 opencog-unify
**Source**: `../unify/`  
**Dependencies**: opencog-atomspace  
**Build Time**: ~10 minutes  

```bash
cd unify
./update-unify.sh
cd unify-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-unify_*.deb
```

#### 5.5 opencog-spacetime
**Source**: `../spacetime/`  
**Dependencies**: opencog-atomspace  
**Build Time**: ~10 minutes  

```bash
cd spacetime
./update-spacetime.sh
cd spacetime-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-spacetime_*.deb
```

---

### Stage 6: Learning and Generation (Parallel Build Possible)

#### 6.1 opencog-learn
**Source**: `../learn/`  
**Dependencies**: opencog-atomspace, opencog-atomspace-rocks, opencog-ure  
**Build Time**: ~15 minutes  

```bash
cd learn
./update-learn.sh
cd learn-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-learn_*.deb
```

#### 6.2 opencog-generate
**Source**: `../generate/`  
**Dependencies**: opencog-atomspace, opencog-ure  
**Build Time**: ~12 minutes  

```bash
cd generate
./update-generate.sh
cd generate-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-generate_*.deb
```

---

### Stage 7: Natural Language Processing (Parallel Build Possible)

#### 7.1 opencog-lg-atomese
**Source**: `../lg-atomese/`  
**Dependencies**: opencog-atomspace, link-grammar  
**Build Time**: ~10 minutes  

```bash
cd lg-atomese
./update-lg-atomese.sh
cd lg-atomese-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-lg-atomese_*.deb
```

#### 7.2 opencog-relex
**Source**: `../relex/`  
**Dependencies**: link-grammar, Java runtime  
**Build Time**: ~15 minutes  

```bash
cd relex
./update-relex.sh
cd relex-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-relex_*.deb
```

**Note**: Relex is Java-based and has different build requirements.

---

### Stage 8: Evolutionary and Specialized Systems (Parallel Build Possible)

#### 8.1 opencog-moses
**Source**: `../moses/`  
**Dependencies**: libcogutil-dev, opencog-atomspace  
**Build Time**: ~20 minutes  

```bash
cd moses
./update-moses.sh
cd moses-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-moses_*.deb
```

#### 8.2 opencog-asmoses
**Source**: `../asmoses/`  
**Dependencies**: opencog-moses, opencog-atomspace  
**Build Time**: ~15 minutes  

```bash
cd asmoses
./update-asmoses.sh
cd asmoses-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-asmoses_*.deb
```

#### 8.3 opencog-agi-bio
**Source**: `../agi-bio/`  
**Dependencies**: opencog-atomspace, opencog-pln, opencog-ure  
**Build Time**: ~12 minutes  

```bash
cd agi-bio
./update-agi-bio.sh
cd agi-bio-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-agi-bio_*.deb
```

#### 8.4 opencog-vision
**Source**: `../vision/`  
**Dependencies**: opencog-atomspace  
**Build Time**: ~12 minutes  

```bash
cd vision
./update-vision.sh
cd vision-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog-vision_*.deb
```

---

### Stage 9: Meta-Package

#### 9.1 opencog
**Source**: `../opencog/`  
**Dependencies**: All above OpenCog packages  
**Build Time**: ~5 minutes  

```bash
cd opencog
./update-opencog.sh
cd opencog-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../opencog_*.deb
```

**Note**: This is primarily a meta-package that pulls in all components.

---

### Stage 10: AGI-OS Unified Package (Optional)

#### 10.1 agi-os-unified
**Source**: `../agi-os-unified/`  
**Dependencies**: cognumach, hurdcog, opencog  
**Build Time**: ~5 minutes  
**Status**: Package structure to be created

```bash
cd agi-os-unified
./update-agi-os-unified.sh
cd agi-os-unified-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../agi-os-unified_*.deb
```

---

## Automated Build Script

A master build script can automate the entire process:

```bash
#!/bin/bash
# build-all-opencog.sh - Build all OpenCog packages in correct order

set -e  # Exit on error

PACKAGES=(
    "cogutil"
    "atomspace"
    "atomspace-cog"
    "atomspace-rocks"
    "cogserver"
    "ure"
    "attention"
    "pln"
    "miner"
    "unify"
    "spacetime"
    "learn"
    "generate"
    "lg-atomese"
    "relex"
    "moses"
    "asmoses"
    "agi-bio"
    "vision"
    "opencog"
)

for pkg in "${PACKAGES[@]}"; do
    echo "========================================"
    echo "Building $pkg"
    echo "========================================"
    cd "$pkg"
    ./update-$pkg.sh
    cd "$pkg"-*
    sudo apt-get build-dep -y .
    dpkg-buildpackage -rfakeroot -us -uc -j$(nproc)
    sudo dpkg -i ../*.deb || sudo apt-get install -f -y
    cd ../..
done

echo "========================================"
echo "All packages built successfully!"
echo "========================================"
```

## Parallel Build Strategy

For faster builds on multi-core systems, stages 3, 5, 6, 7, and 8 can be parallelized:

```bash
#!/bin/bash
# parallel-build-stage5.sh - Build Stage 5 packages in parallel

build_package() {
    local pkg=$1
    echo "Building $pkg in parallel..."
    cd "$pkg"
    ./update-$pkg.sh
    cd "$pkg"-*
    dpkg-buildpackage -rfakeroot -us -uc -j2
    cd ../..
}

export -f build_package

parallel -j 4 build_package ::: attention pln miner unify spacetime

# Install all built packages
sudo dpkg -i attention/*.deb pln/*.deb miner/*.deb unify/*.deb spacetime/*.deb
sudo apt-get install -f -y
```

## Troubleshooting

### Common Issues

1. **Missing Build Dependencies**
   ```bash
   sudo apt-get build-dep <package-name>
   ```

2. **Broken Dependencies After Install**
   ```bash
   sudo apt-get install -f
   ```

3. **Clean Build Required**
   ```bash
   cd <package-dir>
   debian/rules clean
   ```

4. **Lintian Warnings**
   ```bash
   lintian -i -I --show-overrides <package>.deb
   ```

### Build Logs

Build logs are essential for debugging:
```bash
dpkg-buildpackage -rfakeroot -us -uc 2>&1 | tee build.log
```

## Testing Built Packages

After building, test the installation:

```bash
# Test cogutil
pkg-config --modversion cogutil

# Test atomspace
guile -c "(use-modules (opencog))"

# Test cogserver
cogserver --version

# Run integration tests
cd tests && ./run-all-tests.sh
```

## Repository Publication

Once packages are built and tested:

1. **Sign packages**
   ```bash
   debsign <package>.changes
   ```

2. **Upload to repository**
   ```bash
   dput opencog <package>.changes
   ```

3. **Update repository metadata**
   ```bash
   reprepro -b /var/www/debian include sid <package>.changes
   ```

## Maintenance

### Updating Packages

When upstream releases new versions:

1. Update the changelog
   ```bash
   dch -v <new-version>-1 "New upstream release"
   ```

2. Rebuild the package
   ```bash
   dpkg-buildpackage -rfakeroot -us -uc
   ```

3. Test thoroughly before publishing

### Version Tracking

Keep track of component versions in a central file:
- `opencog-debian/VERSIONS.txt`

## References

- [Debian Packaging Tutorial](https://www.debian.org/doc/manuals/maint-guide/)
- [OpenCog Build Instructions](https://wiki.opencog.org/w/Building_OpenCog)
- [CMake Best Practices](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)

## Summary

**Total Build Time** (sequential): ~4-5 hours  
**Total Build Time** (parallel): ~2-3 hours  
**Disk Space Required**: ~20GB  
**RAM Required**: 8GB minimum, 16GB recommended  

The build order is critical for success. Always follow the stages in sequence, but feel free to parallelize within stages where indicated.
