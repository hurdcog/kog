# OpenCog Collection (OCC) Repository Analysis

**Generated**: April 6, 2026  
**Repository**: github.com/o9nn/occ  
**Status**: Active monorepo with integrated CI/CD

---

## Executive Summary

The OpenCog Collection is a **sophisticated monorepo** that integrates:
- **3-layer AGI-OS architecture** (Cognumach → HurdCog → OCC)
- **11 dependency layers** for cognitive components
- **Multi-platform support** (Linux, Windows, containers, Guix)
- **Complex dependency graphs** with critical path sequencing

The repository prioritizes **cognitive synergy** through strategic component integration, with extensive build automation and reproducible environment support via GNU Guix.

---

## 1. Build/Test Commands & Automation

### 1.1 Primary Build Commands

#### Standard CMake Build
```bash
# Minimal build (5 min, no heavy dependencies)
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_ATOMSPACE=OFF \
  -DBUILD_COGSERVER=OFF \
  -DBUILD_MATRIX=OFF
make -j$(nproc)

# Full build (30+ min, all dependencies)
mkdir build && cd build
cmake ..
make -j$(nproc)
```

#### Makefile High-Level Targets
```bash
make help              # Show all available targets
make all              # Build complete AGI-OS stack
make test             # Run integration tests
make dev-setup        # Setup development environment
make guix-build       # Build using GNU Guix (reproducible)
make direct-build     # Direct CMake build
make cmake-agi-os-stack  # CMake with proper sequences
```

#### Staged Builds (from Makefile.build-sequences)
```bash
# Layer-by-layer builds with dependencies
make cmake-foundation      # cogutil + atomspace
make cmake-storage         # Storage backends
make cmake-network         # cogserver
make cmake-reasoning       # unify, ure, pln, miner
make cmake-learning        # learn components
make cmake-attention       # attention subsystems
make cmake-agents          # agents, sensory
```

### 1.2 Testing Strategy

#### Test Framework: CxxTest
- **Default setting**: Tests excluded from main build (`EXCLUDE_FROM_ALL`)
- **Activation**: Run `make tests` after build or include in CMake targets
- **Coverage mode**: `cmake .. -DCMAKE_BUILD_TYPE=Coverage`

#### Test Execution
```bash
# Enable and run tests
make tests
cd build
ctest --force-new-ctest-process

# With coverage analysis
make tests-coverage      # Generates HTML coverage reports in tests/lcov
```

#### Test Component Organization
- Each major component (cogutil, atomspace, cogserver) has `tests/` subdirectory
- CxxTest test files typically named `*-test.cxxtest.h`
- Python integration tests in `tests/synergy/`

### 1.3 Windows-Specific Build Commands

#### Option A: Vendored Dependencies (Fast - 5-10 min)
```powershell
# Requires: prebuilt/ directory populated by vendor-dependencies.yml workflow
cmake -B build -S . `
  -G "Visual Studio 17 2022" `
  -A x64 `
  -DCMAKE_PREFIX_PATH="$PWD/prebuilt/x64-windows"

cmake --build build --config Release
```

#### Option B: vcpkg Manifest Mode
```powershell
# vcpkg.json automatically specifies dependencies
cmake -B build -S . `
  -G "Visual Studio 17 2022" `
  -A x64 `
  -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"

cmake --build build --config Release
```

### 1.4 Guix Build (Reproducible)
```bash
# Guix provides fully declarative environment
guix build -f guix.scm

# Or with channel substitution
guix pull && guix build -f guix.scm

# Options configured in guix.scm with CMake flags for selective building
```

### 1.5 Component-Specific Build Commands

#### CogUtil (Foundation)
```bash
cd cogutil
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

#### AtomSpace
```bash
cd atomspace
cmake -B build -S . \
  -DCMAKE_PREFIX_PATH=/path/to/cogutil/install \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

#### CogServer (requires atomspace-storage)
```bash
cd cogserver
cmake -B build -S . \
  -DCMAKE_PREFIX_PATH=/path/to/atomspace-storage/install \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### 1.6 Automation Best Practices Observed

1. **Parallel builds**: All Makefiles use `-j$(nproc)` for multi-core builds
2. **Non-print-directory**: `MAKEFLAGS += --no-print-directory` reduces noise
3. **Color-coded output**: Informational messages use ANSI colors for clarity
4. **Dependency tracking**: CMake properly tracks inter-component dependencies
5. **Out-of-source builds**: All builds use separate `build/` directories
6. **Staged validation**: Layer-by-layer builds with individual targets

---

## 2. Key Architectural Patterns & Component Boundaries

### 2.1 Monorepo Organization (11 Dependency Layers)

```
Layer 0: Build Tools
├── build-tools/mig          (Mach Interface Generator, ~5 min)

Layer 1: Microkernel
├── cognumach/               (GNU Mach enhanced, ~30 min, autotools)

Layer 2: Operating System
├── hurdcog/                 (GNU Hurd + cognitive extensions, depends on Layer 1)

Layer 3: Foundation
├── cogutil/                 (ALL other components depend on this)

Layer 4: Core Knowledge Representation
├── atomspace/               (Hypergraph database for symbolic AI)

Layer 5: Storage Foundation
├── atomspace-storage/       (CRITICAL: API base for all storage backends - must build before cogserver)

Layer 6-7: Network & Storage Backends (Parallel)
├── cogserver/               (Networking, IPC, REST API - depends on atomspace-storage)
├── atomspace-rocks/         (RocksDB backend)
├── atomspace-pgres/         (PostgreSQL backend)
├── atomspace-cog/           (CogServer network storage)

Layer 8: Math & Reasoning
├── matrix/                  (Sparse vector support)
├── unify/                   (Pattern matching unification)
├── ure/                      (Unified Rule Engine)
├── pln/                      (Probabilistic Logic Networks)
├── miner/                    (Pattern/rule discovery)
├── attention/                (Attention allocation mechanisms)

Layer 9: Learning & Evolution
├── learn/                    (Symbolic learning)
├── asmoses/                  (Evolutionary algo - MOSES)

Layer 10: Agent & Integration Framework
├── agents/                   (Interactive agents framework)
├── sensory/                  (Sensory dataflow system)
├── coggml/                   (Self-aware microkernel)
├── cogself/                  (AGI synergy framework)
├── atomspace-accelerator/    (Tensor inference engine)
├── agentic-chatbots/         (LLM + Agent integration)

Layer 11: Tensor-Neural-Symbolic (Optional Advanced)
├── aten/                     (C++11 tensor library)
├── atenspace/                (AtomSpace + Tensor embeddings)
└── integration-layer/        (Neural-symbolic bridge)
```

### 2.2 Critical Dependency Chain

**MUST be built in this exact order:**
```
cogutil → atomspace → atomspace-storage → cogserver
                                       ↓
                                atomspace-rocks
                                atomspace-pgres
                                atomspace-cog
```

⚠️ **Critical Issue**: If `atomspace-storage` is skipped or built after `cogserver`, the build will fail with:
```
Could NOT find StorageNode
Could NOT find StorageNode_DIR
```

### 2.3 Component Boundaries & Responsibilities

| Component | Purpose | Language | Dependencies | Key Pattern |
|-----------|---------|----------|--------------|-------------|
| **cogutil** | C++ utilities, logging, threading | C++ | Boost, system | Foundation lib (libcogutil.so) |
| **atomspace** | Hypergraph knowledge base | C++/Scheme | cogutil, Guile | In-memory graph + indexes |
| **cogserver** | Network I/O, REST API, Scheme REPL | C++/Scheme | atomspace-storage | Server socket mgmt |
| **atomspace-storage** | Abstract storage API | C++ | atomspace | Plugin architecture for backends |
| **atomspace-rocks** | Persistent KV store | C++ | RocksDB, atomspace-storage | Filesystem-backed persistence |
| **matrix** | Sparse matrix algebra | C++ | Boost | Math operations on sparse data |
| **ure** | Logic inference engine | Scheme/C++ | atomspace, unify | Forward/backward chaining |
| **pln** | Probabilistic reasoning | Scheme | atomspace, ure | Truth value propagation |
| **learn** | Symbolic learning | Scheme/Python | atomspace | Pattern extraction, generalization |
| **asmoses** | Evolutionary computation | C++/Python | cogutil | Genetic programming |
| **agents** | Agent behavior framework | C++/Scheme | atomspace, cogserver | Agent loop implementation |
| **sensory** | Data input pipelines | C++ | atomspace | Stream processing nodes |

### 2.4 Architectural Patterns in Code

#### Pattern 1: CMake Component Submodules
```cmake
# Each component is add_subdirectory(component_name)
# with EXCLUDE_FROM_ALL for tests
IF(BUILD_COGUTIL)
    MESSAGE(STATUS "Building CogUtil...")
    add_subdirectory(cogutil)
ENDIF()

# Tests must be explicitly enabled
ADD_CUSTOM_TARGET(tests)
ADD_SUBDIRECTORY(tests EXCLUDE_FROM_ALL)
```

#### Pattern 2: Namespace Isolation
- Each component in `opencog/component_name/` namespace
- Clear header hierarchy: `#include <opencog/componentname/Class.h>`
- Prevents symbol collisions in complex dependency graph

#### Pattern 3: Storage Backend Plugin Pattern
```cpp
// All storage implementations inherit from StorageNode
class RocksStorageNode : public StorageNode { /* ... */ }
class PostgresStorageNode : public StorageNode { /* ... */ }
```
- Enables swappable persistence backends
- Clear interface contract for storage providers

#### Pattern 4: Guile Scripting Integration
- Scheme bindings provided for all core C++ components
- Allows dynamic reconfiguration without recompile
- REPL available via cogserver

#### Pattern 5: Staged Component EnableDisable
- All components optional via CMake flags `BUILD_*`
- Supports minimal builds for resource-constrained environments
- Enables CI matrix testing of different configurations

---

## 3. Project-Specific Conventions Differing from Typical C++

### 3.1 Build System Conventions

| Convention | Pattern | Rationale |
|-----------|---------|-----------|
| **Three build systems** | CMake (primary) + autotools (microkernel) + cargo (Rust) | Different subsystems have been developed with different build tools |
| **Staged builds** | Layer-by-layer with `make cmake-foundation`, etc. | Manages complex 11-layer dependency graph explicitly |
| **Guix integration** | `guix.scm` alongside CMakeLists.txt | Reproducible builds, FSF endorsement, declarative dependencies |
| **Debian packaging** | `opencog-debian/` subdirectory tracks .deb packages | Official package distribution independent of source build |
| **Windows first-class** | vcpkg.json, prebuilt/ directory with CI | Not an afterthought; Windows builds as fast as Linux |
| **Component selectivity** | BUILD_COGUTIL=ON/OFF flags for each component | Supports minimal deployment footprint |

### 3.2 Naming Conventions

#### Executable/Library Naming
- **Libraries**: `libcomponent.so` (Linux) / `component.lib` (Windows)
- **Executables**: Scheme shell access through `cogserver` rather than separate binaries
- **Test binaries**: Implicit in CxxTest framework

#### CMake Variable Conventions
```cmake
BUILD_COMPONENTNAME     # Enable/disable component build
ComponentName_DIR       # CMake find_package location
CMAKE_PREFIX_PATH       # Search path for transitive dependencies
CXXTEST_FOUND          # Conditional test enablement
```

#### Directory Conventions
```
component/
├── opencog/
│   └── component/          # C++ headers in opencog::component namespace
├── lib/
│   ├── implementation.cc    # Private implementation
│   └── opencog/component/   # Public headers
├── tests/
│   └── *-test.cxxtest.h   # CxxTest test files
└── CMakeLists.txt
```

### 3.3 Dependency Resolution Specifics

#### Local Dependency Model (Monorepo)
- No version pinning between components
- All components always use "tip of tree" versions
- Requires strict ordering: cannot build atomspace before cogutil complete
- No semantic versioning within monorepo

#### Inter-monorepo Dependencies
- External: Boost, Guile, Python, system libraries
- Managed differently on each platform:
  - **Linux**: System package manager or manual
  - **Windows**: vcpkg manifest or prebuilt binaries
  - **Reproducibility**: Guix with pinned package versions

### 3.4 Testing Conventions

#### CxxTest Specifics
- Test discovery: Manual framework macros, not template instantiation
- Test headers: MUST be `.h`, not `.cpp`
- Coverage: Optional via `CMAKE_BUILD_TYPE=Coverage`
- Execution: Default excluded from build; run separately
- Python tests: In `tests/synergy/` for integration testing

#### No Standard Test Targets
```bash
# NOT available by default:
make test           # (in top-level Makefile only)
ctest              # (requires separate invocation)

# INSTEAD: Component-specific or via make tests
```

### 3.5 Git/Workflow Conventions

#### Repository Flags
- **Submodules**: Removed (was complex dependency management)
- **Monorepo pattern**: All components in single repository
- **Branch strategy**: main/master with feature branches

#### CI/CD Workflow Patterns
- **Disabled workflows**: 50+ `.yml.disabled` files indicate extensive CI experimentation
- **Active workflows**: Only 3-4 workflows actively used:
  - `auto-sync-runner.yml` - Sync with external repos
  - `occ-win-build-fast.yml` - Windows builds
  - `heavy-deps-build.yml` - Full dependency Linux builds
  - `electron-app-build.yml` - Desktop app building

### 3.6 Deployment/Distribution Conventions

#### Linux Distribution
- **Debian packages**: Via `opencog-debian/` subdirectory
- **Guix packages**: `guix.scm` at repository root
- **Installation path**: `/usr/local/agi-os` (configurable)

#### Windows Distribution
- **vcpkg port**: Infrastructure exists for vcpkg integration
- **Chocolatey packages**: Experimental support
- **Prebuilt binaries**: `prebuilt/x64-windows/` for CI speedup

#### Container Distribution
- **Docker**: `Dockerfile` based on `opencog/opencog-dev:latest`
- **DevContainer**: `.devcontainer/devcontainer.json` for VS Code
- **Guix environment**: Can create isolated shells via `guix environment -f guix.scm`

---

## 4. Common Development Environment Issues & Pitfalls

### 4.1 Dependency Order Failures

**Symptom**: `Could NOT find StorageNode`
```
CMake Error at cogserver/CMakeLists.txt:45 (FIND_PACKAGE):
  Could NOT find StorageNode (missing: StorageNode_DIR)
```

**Root Cause**: `atomspace-storage` must be built before `cogserver`

**Fix**:
```bash
# Correct order
make cmake-foundation        # cogutil + atomspace
make cmake-storage          # atomspace-storage
make cmake-network          # cogserver
```

### 4.2 Missing System Dependencies

**Symptom**: `Could NOT find Asio`
```
CMake Error: Could NOT find Asio (missing: Asio_INCLUDE_DIR)
```

**Root Cause**: Linux only - missing `libasio-dev`

**Fix**:
```bash
sudo apt-get install libasio-dev
```

### 4.3 CxxTest Not Found

**Symptom**: Tests don't compile, `CXXTEST_FOUND` is false

**Root Cause**: Optional dependency, not installed

**Fix**:
```bash
sudo apt-get install cxxtest
# Or on macOS:
brew install cxxtest
```

### 4.4 Guile Version Conflicts

**Symptom**: `Guile was not found; the scheme bindings will not be built`
```
CMake Warning (dev) at atomspace/CMakeLists.txt:56 (MESSAGE):
  Guile was not found; the scheme bindings will not be built
```

**Root Cause**: Guile 3.0 required, but older version installed or missing

**Fix**:
```bash
sudo apt-get install guile-3.0 guile-3.0-dev
# Or update: sudo apt-get install --upgrade guile-3.0
```

### 4.5 Windows MSVC Path Issues

**Symptom**: Link errors with vendored dependencies not found
```
error: Cannot open input file "boost.lib"
```

**Root Cause**: `CMAKE_PREFIX_PATH` not set correctly to `prebuilt/` directory

**Fix**:
```powershell
# Use absolute path
-DCMAKE_PREFIX_PATH="C:\Full\Path\To\occ\prebuilt\x64-windows"
```

### 4.6 Out-of-Sync CMakeLists.txt

**Symptom**: Components build individually but not together
```
Could NOT find ComponentX
```

**Root Cause**: CMakeLists.txt in root not synchronized with component CMakeLists.txt

**Fix**:
- Verify `add_subdirectory(component)` exists in root CMakeLists.txt
- Check that BUILD_COMPONENT flag logic is correct

### 4.7 Python Binding Issues

**Symptom**: Import errors: `ImportError: No module named opencog`

**Root Cause**: Python bindings not built (`cython3` missing) or install path not in PYTHONPATH

**Fix**:
```bash
# Install build requirements
sudo apt-get install cython3 python3-dev

# Add to PYTHONPATH after install
export PYTHONPATH="/usr/local/agi-os/lib/python3:$PYTHONPATH"
```

### 4.8 Scheme Bindings in CogServer

**Symptom**: Scheme REPL fails in cogserver
```
Scheme not available: eval failed
```

**Root Cause**: Built without Guile support

**Fix**:
```bash
# Rebuild with guile:
cmake .. -DCMAKE_BUILD_TYPE=Release
# (should auto-detect guile if installed)
```

### 4.9 Cross-architecture Building

**Pitfall**: 32-bit builds mixed with 64-bit

**Fix**: Specify explicitly in cmake:
```bash
# Force 64-bit
cmake .. -DCMAKE_CXX_FLAGS="-m64"

# Force 32-bit (if gcc-multilib installed)
cmake .. -DCMAKE_CXX_FLAGS="-m32"
```

### 4.10 Incremental Build Stale Objects

**Symptom**: Changes not reflected in rebuild
```
# Code changed but binary unchanged
```

**Fix**: Clean build directory completely:
```bash
rm -rf build/
mkdir build && cd build
cmake ..
make -j$(nproc)
```

---

## 5. Key Files & Directories Exemplifying Patterns

### 5.1 Essential Build Configuration Files

| File | Purpose | Key Insights |
|------|---------|--------------|
| **CMakeLists.txt** (root) | Master build orchestration | 11-layer dependency declaration, all BUILD_* flags |
| **Makefile.build-sequences** | Layer-by-layer build targets | Pattern: `cmake-foundation` → `cmake-storage` → `cmake-network` |
| **Makefile** | High-level unified interface | Includes Makefile.build-sequences, defines NPROC, colors |
| **guix.scm** | Reproducible environment declaration | Complete dependency graph in Scheme, FSF-compliant |
| **vcpkg.json** | Windows dependency manifest | Lists: boost, rocksdb, protobuf, grpc, catch2, spdlog, nlohmann-json, yaml-cpp |
| **docker-compose.yml** | Container orchestration | Multi-service setup for development |

### 5.2 Component CMakeLists.txt Exemplar (atomspace)

**Path**: `atomspace/CMakeLists.txt`

**Key Patterns**:
- Flexible build type selection (Release/Debug/Coverage/Profile/RelWithDebInfo)
- Project definition with version
- Cxxtest framework integration
- Guile binding conditional build
- Python binding with Cython/SWIG
- CTest integration for automated testing
- Coverage report generation (lcov)

### 5.3 Architecture Documentation

| File | Coverage |
|------|----------|
| **README.md** | Overview, key features, getting started, system dependencies |
| **QUICKSTART.md** | Fast build options, minimal vs full, verification scripts |
| **BUILD_INSTRUCTIONS.md** | Detailed prerequisites, CMake options, troubleshooting |
| **WINDOWS_BUILD.md** | vcpkg setup, MSVC build, component order, manual deps |
| **BUILD_DEPENDENCY_ORDER.md** | Complete 11-layer dependency graph with timing estimates |
| **CONTRIBUTING.md** | PR process, code of conduct, contribution guidelines |

### 5.4 Key Testing Files

| File | Pattern |
|------|---------|
| **cogutil/tests/** | CxxTest header files for utility functions |
| **atomspace/tests/** | Complex test suite for hypergraph operations |
| **tests/synergy/** | Python integration tests for cognitive modules |
| **build-test/test-basic-build.sh** | Minimal build verification script |
| **test-guix-syntax.sh** | Validates guix.scm syntax without full build |

### 5.5 CI/CD Workflow Files

**Pattern**: Active workflows are simple and focused; numerous `.disabled` versions represent experimentation

#### Active Workflows
```
.github/workflows/
├── auto-sync-runner.yml          (Sync with external OpenCog repos)
├── occ-win-build-fast.yml         (5-10 min Windows CI using prebuilt)
├── heavy-deps-build.yml           (Full Linux build with all dependencies)
├── electron-app-build.yml         (Desktop application build)
└── wincogpre.yml                  (Windows pre-release)
```

#### Disabled Experiment Workflows (50+)
- Various attempts at: CI optimization, Guix CI, Docker CI, Android CI
- Indicates: Extensive experimentation, gradual settling on stable patterns
- Learning: Multi-platform CI is genuinely hard for complex monorepos

### 5.6 Development Environment Configuration

| File | Configuration |
|------|---------------|
| **.devcontainer/devcontainer.json** | VS Code container with Guix/Shepherd |
| **Dockerfile** | Based on opencog/opencog-dev:latest |
| **docker-compose.yml** | Multi-service orchestration (can extend for new services) |
| **.guix/guix-channel** | Official GNU Guix channel definition |
| **.vscode/** | Workspace settings, extensions, launch configs |

### 5.7 Exemplary Component Structure

**Path**: `cogutil/` - Model component showing best practices

```
cogutil/
├── CMakeLists.txt                 # 220 lines, well-commented
├── opencog/util/                  # Public API headers
│   ├── tree.h
│   ├── Logger.h
│   ├── Config.h
│   └── ... (28 more headers)
├── lib/
│   ├── opencog/util/              # Implementation headers
│   ├── [implementation .cc files]
│   └── tests/                      # Unit tests
├── tests/
│   ├── utilUTest-test.cxxtest.h
│   ├── configUTest-test.cxxtest.h
│   └── CMakeLists.txt
├── cmake/
│   ├── AddCxxtest.cmake            # CxxTest helper macros
│   ├── FindCxxtest.cmake           # CxxTest detection
│   └── ... (other helper modules)
└── README.md                       # Comprehensive component docs
```

**Key Pattern**: Public headers in `opencog/util/`, implementation in `lib/`, tests isolated and conditional

---

## 6. Build System Deep Dive

### 6.1 CMake Architecture

#### Root CMakeLists.txt Strategy
```cmake
# 1. Global settings
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(opencog-collection)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 2. Declare all BUILD_* options upfront
OPTION(BUILD_COGNUMACH ...)
OPTION(BUILD_HURDCOG ...)
OPTION(BUILD_COGUTIL ...)
... (20+ more)

# 3. Layer-by-layer conditional building
IF(BUILD_COGNUMACH)
    add_subdirectory(cognumach)
ENDIF()
IF(BUILD_HURDCOG)
    IF(BUILD_COGNUMACH)
        add_subdirectory(hurdcog)
    ELSE()
        MESSAGE(WARNING "HurdCog requires Cognumach")
    ENDIF()
ENDIF()
... (similar pattern for all layers)
```

#### Parallelization Strategy
- Uses `-j$(nproc)` to utilize all CPU cores
- Each major layer can be built in parallel (e.g., storage backends)
- Within-layer serial ordering enforced by CMake target dependencies

### 6.2 Makefile Integration

#### Makefile Structure
```makefile
include Makefile.build-sequences  # Import staged build targets

PROJECT_NAME := AGI-OS
NPROC := $(shell nproc)           # CPU count for parallel builds
MAKEFLAGS += --no-print-directory # Reduce noise

# Color definitions for status output
COLOR_RESET := \033[0m
COLOR_BLUE := \033[34m
```

#### Key Makefile Targets
```makefile
all              # Build everything (via CMake)
clean            # (Not implemented—use: rm -rf build/)
help             # Show all targets
guix-build       # Use Guix for reproducible build
cmake-*          # Layer-specific builds
```

**Observation**: Makefile delegates to CMake instead of using raw Make—indicates maturity, consensus that CMake is cleaner

### 6.3 Windows Build Optimization

#### Prebuilt Vendored Approach (Fast)
```powershell
# Workflow: vendor-dependencies.yml populates prebuilt/ directory
# Then occ-win-build-fast.yml uses:
-DCMAKE_PREFIX_PATH="${{ github.workspace }}/prebuilt/x64-windows"

# Result: ~5-10 minutes instead of 2+ hours with vcpkg
```

**Strategy**: Download & cache prebuilt Windows libraries in CI, commit to repo (or artifact cache)

#### Manifest Mode (Reproducible)
```cmake
# vcpkg.json declares all dependencies
# CMake auto-installs via MSVC toolchain
```

### 6.4 Test Framework Integration

#### CxxTest Discovery
```cmake
# In each component's CMakeLists.txt:
FIND_PACKAGE(Cxxtest)
IF (CXXTEST_FOUND)
    ADD_CUSTOM_TARGET(tests)
    ADD_SUBDIRECTORY(tests EXCLUDE_FROM_ALL)
ENDIF()
```

#### CxxTest Macro Usage
```c++
// In tests/*-test.cxxtest.h:
class MyTest : public CxxTest::TestSuite {
public:
    void test_example() {
        TS_ASSERT_EQUALS(1 + 1, 2);
    }
};
```

#### Coverage Reporting
```bash
# Enables lcov coverage tracking
cmake .. -DCMAKE_BUILD_TYPE=Coverage
make tests
# Generates: tests/lcov/index.html
```

### 6.5 Guix Integration

#### guix.scm Structure
```scheme
(define-public opencog-collection
  (package
    (name "opencog-collection")
    (version "0.1.0")
    (build-system cmake-build-system)
    (arguments
      `(#:tests? #f              ; Skip tests (network issues in CI)
        #:configure-flags        ; Pass to CMake
        '("-DCMAKE_BUILD_TYPE=Release"
          "-DBUILD_COGUTIL=ON"
          ... (selective component builds)
```

#### Key Guix Features
- **Reproducibility**: Pinned versions of all dependencies
- **FSF Endorsement**: Free Software Foundation compatible
- **Isolation**: Can create fully isolated build environments
- **Local source**: Uses `(local-file "." ...)` for monorepo

---

## 7. Summary: Key Takeaways

### Critical Success Factors

1. **Explicit Dependency Ordering**: 11-layer model prevents circular dependencies
2. **Multiple Build Paths**: Guix (reproducible), CMake (standard), Makefile (convenience)
3. **Selective Building**: Components can be included/excluded via `BUILD_*` flags
4. **Windows Parity**: Prebuilt dependencies ensure Windows builds aren't slow second-class citizens
5. **Test Isolation**: Tests excluded from main build by default, run separately for faster CI

### Common Mistakes to Avoid

1. **Building out of order**: Don't skip atomspace-storage
2. **Missing system deps**: cxxtest, guile-3.0-dev, libasio-dev are common misses
3. **Wrong build directory**: Always use separate `build/` directory, not in-source builds
4. **Guile version mismatch**: Must be 3.0, not 2.2 or 3.2
5. **Stale CMake cache**: Delete `build/` directory completely if CMakeLists.txt changed

### Recommendations for Developers

- **For new developers**: Follow QUICKSTART.md minimal build first (5 min)
- **For system integration**: Use Guix or Debian packages, not direct CMake install
- **For Windows**: Use prebuilt vendored dependencies from CI artifacts
- **For quick iteration**: Build only needed components with selective `BUILD_*` flags
- **For CI**: Use `heavy-deps-build.yml` as reference, not individual component jobs

---

## Appendix: Reference Commands

### Minimal Setup
```bash
git clone --recurse-submodules https://github.com/o9nn/occ.git
cd occ
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_ATOMSPACE=OFF -DBUILD_COGSERVER=OFF
make -j$(nproc)
```

### Full Reproducible Setup (Guix)
```bash
git clone https://github.com/o9nn/occ.git
cd occ
guix pull
guix build -f guix.scm
```

### Windows Fast Build
```powershell
git clone https://github.com/o9nn/occ.git
cd occ
# Requires: prebuilt/x64-windows populated from CI artifacts
cmake -B build -S . -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="$PWD/prebuilt/x64-windows"
cmake --build build --config Release
```

### Component-Specific Testing
```bash
cd atomspace/build
ctest --verbose
```

### Python Integration Test
```bash
python3 tests/synergy/test_integration.py
```
