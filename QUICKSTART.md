# Quick Start Build Guide

**Last updated:** January 24, 2026

This guide helps you quickly build and test the OpenCog Collection (OCC) components.

## Prerequisites

### Minimal Build (CogUtil + CogGML only)
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y build-essential cmake python3-dev

# macOS
brew install cmake python3

# Windows
# Install Visual Studio 2022 with C++ support
# Install CMake from cmake.org
```

### Full Build (All Components)
```bash
# Ubuntu/Debian
sudo apt-get install -y \
  build-essential \
  cmake \
  libboost-all-dev \
  guile-3.0-dev \
  python3-dev \
  cython3 \
  liboctomap-dev \
  liboctomap-tools \
  cxxtest \
  doxygen
```

## Quick Start Options

### Option 1: Minimal Build (5 minutes, no dependencies)

Build just the core utilities and cognitive microkernel:

```bash
# Clone repository
git clone --recurse-submodules https://github.com/o9nn/occ.git
cd occ

# Create build directory
mkdir build && cd build

# Configure minimal build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_ATOMSPACE=OFF \
  -DBUILD_COGSERVER=OFF \
  -DBUILD_MATRIX=OFF \
  -DBUILD_LEARN=OFF \
  -DBUILD_AGENTS=OFF \
  -DBUILD_SENSORY=OFF \
  -DBUILD_ATOMSPACE_ACCELERATOR=OFF \
  -DBUILD_AGENTIC_CHATBOTS=OFF \
  -DBUILD_COGSELF=OFF \
  -DBUILD_INTEGRATION_LAYER=OFF

# Build
make -j$(nproc)

# Verify
./test-basic-build.sh
```

**Result:** You'll have:
- `libcogutil.so` (1.6MB) - Foundation utilities
- `libcoggml.so` (131KB) - Cognitive microkernel

### Option 2: Full Build (30+ minutes, requires dependencies)

Build all components with full cognitive capabilities:

```bash
# Clone repository
git clone --recurse-submodules https://github.com/o9nn/occ.git
cd occ

# Create build directory
mkdir build && cd build

# Configure full build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build (this may take 30+ minutes on first build)
make -j$(nproc)

# Run synergy check
cd ..
./synergy.sh
```

**Result:** You'll have all OCC components including AtomSpace, CogServer, PLN, MOSES, etc.

### Option 3: Using GNU Guix (Recommended for reproducibility)

```bash
# Clone repository
git clone --recurse-submodules https://github.com/o9nn/occ.git
cd occ

# Build with Guix (fully reproducible)
guix build -f guix.scm

# Or enter development environment
guix shell -D -f guix.scm
```

### Option 4: Using Docker/Devcontainer

```bash
# Clone repository
git clone --recurse-submodules https://github.com/o9nn/occ.git
cd occ

# Open in VS Code with Remote Containers extension
# Or manually:
docker-compose up -d
docker exec -it occ bash
```

## Build Component by Component

If you want to build specific components:

### CogUtil (No dependencies)
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_COGUTIL=ON
make -j$(nproc) cogutil
```

### AtomSpace (Requires CogUtil + Guile 3.0)
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_COGUTIL=ON \
  -DBUILD_ATOMSPACE=ON
make -j$(nproc) atomspace
```

### CogServer (Requires AtomSpace)
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_COGUTIL=ON \
  -DBUILD_ATOMSPACE=ON \
  -DBUILD_COGSERVER=ON
make -j$(nproc) cogserver
```

## Testing Your Build

### Run Basic Tests
```bash
# From repository root
./test-basic-build.sh
```

### Run Integration Tests
```bash
# From repository root
./test-integration.sh
```

### Run Synergy Check
```bash
# From repository root
./synergy.sh
```

## Common Build Issues

### Issue: Guile not found
```
CMake Error: Could NOT find Guile
```

**Solution:** Install Guile 3.0+
```bash
# Ubuntu/Debian
sudo apt-get install guile-3.0-dev

# macOS
brew install guile
```

### Issue: Boost not found
```
CMake Error: Could NOT find Boost
```

**Solution:** Install Boost libraries
```bash
# Ubuntu/Debian
sudo apt-get install libboost-all-dev

# macOS
brew install boost
```

### Issue: CxxTest not found
```
CMake Warning: Could NOT find CxxTest
```

**Solution:** Install CxxTest (optional, only needed for running tests)
```bash
# Ubuntu/Debian
sudo apt-get install cxxtest

# macOS
brew install cxxtest
```

### Issue: Octomap not found
```
CMake Error: Could NOT find octomap
```

**Solution:** Install Octomap (required for SpaceTime component)
```bash
# Ubuntu/Debian
sudo apt-get install liboctomap-dev liboctomap-tools

# macOS
brew install octomap
```

## Build Targets

The CMakeLists.txt provides several build targets:

- `all` - Build all enabled components
- `cogutil` - Build CogUtil library
- `atomspace` - Build AtomSpace hypergraph database
- `cogserver` - Build CogServer networking layer
- `coggml` - Build CogGML microkernel
- `cogself` - Build CogSelf synergy framework
- `check` - Run unit tests
- `install` - Install libraries and headers

## Next Steps

After successful build:

1. **Explore examples:** Check the `examples/` directory
2. **Read architecture docs:** See `docs/architecture.md`
3. **Run cognitive demos:** See `docs/cognitive-synergy.md`
4. **Contribute:** See `CONTRIBUTING.md`

## Build Performance Tips

### Speed up compilation:
```bash
# Use Ninja instead of Make
cmake .. -G Ninja
ninja -j$(nproc)

# Use ccache
sudo apt-get install ccache
cmake .. -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
```

### Reduce memory usage:
```bash
# Limit parallel jobs if you have limited RAM
make -j2  # instead of -j$(nproc)
```

### Clean build:
```bash
# Remove build directory and start fresh
rm -rf build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

## Platform-Specific Notes

### Windows
- Use Visual Studio 2022 or later
- See `WINDOWS_BUILD.md` for detailed instructions
- vcpkg is used for dependency management
- Pre-built binaries available via Windows CI

### macOS
- Xcode Command Line Tools required
- Use Homebrew for dependencies
- Some components may need manual configuration

### Linux
- Works best on Ubuntu 20.04+ or Debian 11+
- Other distributions may need package name adjustments
- Full support for all components

## Getting Help

If you encounter issues:

1. Check `BUILD_VALIDATION_REPORT.md` for known issues
2. Review `WORKFLOW_STATUS.md` for CI/CD status
3. See GitHub Issues for reported problems
4. Join OpenCog mailing list for community support

## See Also

- [BUILD_STATUS.md](BUILD_STATUS.md) - Current build status
- [BUILD_VALIDATION_REPORT.md](BUILD_VALIDATION_REPORT.md) - Detailed build analysis
- [WORKFLOW_STATUS.md](WORKFLOW_STATUS.md) - CI/CD workflow status
- [CLAUDE.md](CLAUDE.md) - AI assistant guide with full build details
- [README.md](README.md) - Project overview
