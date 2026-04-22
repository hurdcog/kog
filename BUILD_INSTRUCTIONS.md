# OCC Monorepo Build Instructions

**OpenCog Cognitive Computing - Unified Monorepo**  
**Date**: 2025-11-30  
**Status**: Cohesive monorepo build without submodules

---

## Overview

The OCC repository is now a fully integrated monorepo that builds all OpenCog components without requiring git submodules. All dependencies are included directly in the repository and build in the correct order.

---

## Prerequisites

### Ubuntu 22.04 / 24.04

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    guile-3.0 \
    guile-3.0-dev \
    python3 \
    python3-dev \
    libboost-all-dev \
    cxxtest \
    binutils-dev \
    libiberty-dev \
    libasio-dev
```

**Note**: `libasio-dev` is required for building CogServer and its network communication components.

### Optional Dependencies

**For Python bindings**:
```bash
sudo apt-get install -y cython3 python3-nose
```

**For documentation**:
```bash
sudo apt-get install -y doxygen graphviz
```

**For performance**:
```bash
sudo apt-get install -y libfolly-dev libsparsehash-dev
```

---

## Quick Start

### Build Everything

```bash
cd /path/to/occ
mkdir build
cd build
cmake ..
make -j$(nproc)
sudo make install
```

### Build Specific Components

```bash
cmake .. \
    -DBUILD_COGUTIL=ON \
    -DBUILD_ATOMSPACE=ON \
    -DBUILD_OPENCOG=ON \
    -DBUILD_GHOST=ON \
    -DBUILD_PLN=ON \
    -DBUILD_ECAN=ON
make -j$(nproc)
```

---

## Build Order

The monorepo builds components in dependency order:

1. **CogUtil** - Foundation library (utility functions, logging, configuration)
2. **AtomSpace** - Hypergraph knowledge representation
3. **OpenCog** - Cognitive architecture components
4. **GHOST** - Conversational system (requires Guile)
5. **PLN** - Probabilistic Logic Networks reasoning
6. **ECAN** - Economic Attention Networks

---

## Component-Specific Builds

### CogUtil Only

```bash
cd cogutil
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

**Output**: `libcogutil.so` in `/usr/local/lib/opencog/`

### AtomSpace Only

```bash
# Requires CogUtil to be installed first
cd atomspace
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

**Output**: Multiple libraries in `/usr/local/lib/opencog/`:
- `libatombase.so` - Core atom types
- `libatomcore.so` - Core atom operations
- `libvalue.so` - Value system
- `libexecution.so` - Execution framework
- `libgrounded.so` - Grounded procedures
- And many more...

### GHOST Conversational System

```bash
# Requires AtomSpace and Guile
cd opencog/opencog/ghost
# Build instructions TBD - integrated into main build
```

---

## Build Configuration

### CMake Options

**Core Components**:
- `BUILD_COGUTIL` - Build CogUtil library (default: ON)
- `BUILD_ATOMSPACE` - Build AtomSpace (default: ON)
- `BUILD_OPENCOG` - Build OpenCog components (default: ON)
- `BUILD_GHOST` - Build GHOST system (default: ON)
- `BUILD_PLN` - Build PLN reasoning (default: ON)
- `BUILD_ECAN` - Build ECAN attention (default: ON)

**Cognitive Architecture**:
- `BUILD_COGGML` - Build CogGML microkernel (default: ON)
- `BUILD_COGSELF` - Build CogSelf AGI framework (default: ON)
- `BUILD_ATOMSPACE_ACCELERATOR` - Build inference accelerator (default: ON)
- `BUILD_AGENTIC_CHATBOTS` - Build agentic chatbots (default: ON)

**Extensions**:
- `BUILD_ATOMSPACE_STORAGE` - Build storage backends (default: OFF)
- `BUILD_ATOMSPACE_EXTENSIONS` - Build language extensions (default: OFF)

**External Integrations**:
- `BUILD_GNUCASH` - Cognitive accounting (default: OFF)
- `BUILD_KOBOLDCPP` - Story/world modeling (default: OFF)
- `BUILD_APHRODITE` - LLM inference (default: OFF)

### Build Types

```bash
# Debug build with symbols
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release build with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release

# Release with debug info
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

---

## Installation

### System-wide Installation

```bash
sudo make install
```

**Default installation paths**:
- Libraries: `/usr/local/lib/opencog/`
- Headers: `/usr/local/include/opencog/`
- Scheme modules: `/usr/local/share/opencog/scm/`
- Documentation: `/usr/local/share/doc/opencog/`

### Custom Installation Prefix

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/opencog
make install
```

### Library Path Configuration

Add to `~/.bashrc` or `/etc/ld.so.conf.d/opencog.conf`:

```bash
export LD_LIBRARY_PATH=/usr/local/lib/opencog:$LD_LIBRARY_PATH
export GUILE_LOAD_PATH=/usr/local/share/opencog/scm:$GUILE_LOAD_PATH
```

Then run:
```bash
sudo ldconfig
```

---

## Testing

### Run Unit Tests

```bash
cd build
make test
# Or
ctest --verbose
```

### Test Individual Components

```bash
cd cogutil/build
make tests
ctest

cd atomspace/build
make tests
ctest
```

---

## Troubleshooting

### CogUtil Not Found

**Error**: `Could NOT find CogUtil (missing: CogUtil_DIR)`

**Solution**:
```bash
cd cogutil/build
sudo make install
sudo ldconfig
```

### Guile Not Found

**Error**: `Guile was not found; the scheme bindings will not be built`

**Solution**:
```bash
sudo apt-get install guile-3.0 guile-3.0-dev
```

### Asio Library Not Found

**Error**: `Could NOT find Asio (missing: Asio_INCLUDE_DIR)` or `Asio_VERSION: 0.0.0`

**Solution**:
```bash
sudo apt-get update
sudo apt-get install libasio-dev
```

Alternatively, if Asio is installed in a custom location:
```bash
cmake .. -DAsio_INCLUDE_DIR=/path/to/asio/include
```

**Note**: The Asio library is required for building CogServer and components that depend on it (AtomSpace-Cog, Attention, SpaceTime, PLN, Learn). It provides asynchronous I/O capabilities for network communication.

### Missing CMakeLists.txt in lib/

**Error**: `The source directory /path/to/cogutil/lib does not contain a CMakeLists.txt file`

**Solution**: This has been fixed. The `cogutil/lib/CMakeLists.txt` file is now included in the repository.

### Build Failures

**Clean rebuild**:
```bash
rm -rf build
mkdir build
cd build
cmake ..
make -j$(nproc)
```

---

## Development Workflow

### Incremental Builds

After modifying source code:

```bash
cd build
make -j$(nproc)
```

CMake automatically detects changes and rebuilds only affected files.

### Adding New Components

1. Create component directory with `CMakeLists.txt`
2. Add to root `CMakeLists.txt`:
   ```cmake
   OPTION(BUILD_MYCOMPONENT "Build MyComponent" ON)
   IF(BUILD_MYCOMPONENT AND BUILD_ATOMSPACE)
       add_subdirectory(mycomponent)
   ENDIF()
   ```
3. Rebuild from root

---

## CI/CD Integration

### GitHub Actions

The repository includes `.github/workflows/` for automated builds:

- `build.yml` - Main build workflow
- `test.yml` - Unit test execution
- `docs.yml` - Documentation generation

### Build Matrix

Tests against:
- Ubuntu 22.04, 24.04
- GCC 11, 13
- Guile 2.2, 3.0

---

## Performance Optimization

### Parallel Builds

```bash
make -j$(nproc)  # Use all CPU cores
make -j8         # Use 8 cores
```

### Compiler Optimizations

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 -march=native"
```

### Link-Time Optimization

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
```

---

## Integration with HurdCog

The OCC monorepo is designed for integration with HurdCog (modified GNU Hurd) and Cognumach (GNU Mach microkernel):

### AtomSpace-HurdCog Bridge

Located in `src/atomspace-hurdcog-bridge/`:
- C++ bridge interface
- Python bindings
- IPC mechanism for cognitive message passing

### Build for HurdCog

```bash
cmake .. \
    -DBUILD_HURDCOG_BRIDGE=ON \
    -DCMAKE_SYSTEM_NAME=GNU
```

See `docs/integration/HURDCOG_INTEGRATION_GUIDE.md` for details.

---

## Packaging

### Debian Package

```bash
cd build
cpack -G DEB
```

### Tarball

```bash
cd build
cpack -G TGZ
```

---

## Documentation Generation

### API Documentation

```bash
cd build
make doxygen
```

Output in `build/doc/html/index.html`

### User Documentation

See `docs/` directory:
- `docs/architecture/` - Architecture diagrams
- `docs/integration/` - Integration guides
- `AGI_OS_INTEGRATION_PLAN.md` - Overall integration plan
- `AGI_OS_INTERFACE_INTEGRATION.md` - Interface integration

---

## Support

**Issues**: https://github.com/cogpy/occ/issues  
**Documentation**: https://github.com/cogpy/occ/tree/main/docs  
**Community**: OpenCog mailing list and Discord

---

## License

See individual component directories for license information. Most components are under AGPL-3.0 or Apache-2.0.

---

## Conclusion

The OCC monorepo provides a cohesive build system for all OpenCog components without submodule dependencies. All components build in the correct order with proper dependency management, making it easy to develop, test, and deploy the entire cognitive architecture.

**Next Steps**:
1. Build and install CogUtil and AtomSpace
2. Explore GHOST conversational system
3. Integrate with AGI-OS interface (see `AGI_OS_INTERFACE_INTEGRATION.md`)
4. Deploy to HurdCog for full cognitive OS capabilities
