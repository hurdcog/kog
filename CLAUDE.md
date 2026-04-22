# CLAUDE.md - AI Assistant Guide for OpenCog Collection (OCC)

This document provides comprehensive guidance for AI assistants working with the OpenCog Collection monorepo.

## Project Overview

The **OpenCog Collection (OCC)** is a monorepo integrating multiple OpenCog components and external tools into a unified cognitive architecture for Artificial General Intelligence (AGI) research. The project emphasizes **cognitive synergy** - the emergent intelligence arising from the interaction of diverse AI components.

### Core Philosophy

- **Cognitive Synergy by Design**: Components are designed to interact and collaborate across paradigms (symbolic reasoning, machine learning, evolutionary algorithms)
- **Reproducible Development**: Uses GNU Guix for declarative, isolated development environments
- **AGI-OS Architecture**: Three-layer cognitive architecture (Cognumach microkernel, HurdCog, OCC)

## Repository Structure

```
occ/
├── Core OpenCog Components (C++ with CMake)
│   ├── cogutil/           # Base utilities and configuration (build first)
│   ├── atomspace/         # Hypergraph database and knowledge representation
│   ├── cogserver/         # Networking and IPC layer
│   ├── matrix/            # Sparse matrix/graph processing for ML
│   ├── learn/             # Symbolic learning algorithms
│   ├── agents/            # Interactive cognitive agents framework
│   ├── sensory/           # Dataflow system for external interaction
│   ├── attention/         # Attention allocation mechanisms
│   ├── unify/             # Unification algorithms
│   ├── ure/               # Unified Rule Engine
│   ├── miner/             # Pattern mining
│   ├── pln/               # Probabilistic Logic Networks
│   └── asmoses/           # Meta-Optimizing Semantic Evolutionary Search
│
├── Storage Backends
│   ├── atomspace-storage/ # Base storage API
│   ├── atomspace-rocks/   # RocksDB backend
│   ├── atomspace-cog/     # Network storage (CogServer-based)
│   └── atomspace-pgres/   # PostgreSQL backend
│
├── Cognitive Architecture Components
│   ├── coggml/            # CogGML self-aware microkernel
│   ├── cogself/           # CogSelf AGI synergy framework
│   ├── atomspace-accelerator/ # High-performance inference engine
│   ├── agentic-chatbots/  # Conversational AI integration
│   └── synergy/           # Cognitive synergy modules (Scheme)
│
├── Tensor Logic Components
│   ├── aten/              # ATen C++11 tensor library
│   ├── atenspace/         # ATenSpace: AtomSpace + neural tensor embeddings
│   └── tensor-logic/      # Tensor Logic integration layer
│
├── AGI-OS Layers
│   ├── cognumach/         # Layer 1: Cognitive microkernel
│   ├── hurdcog/           # Layer 2: Cognitive OS services
│   └── metamodel/         # System metamodel and ontology
│
├── Build System Files
│   ├── CMakeLists.txt     # Root CMake configuration
│   ├── Makefile           # AGI-OS unified build interface
│   ├── guix.scm           # GNU Guix package definition
│   ├── Cargo.toml         # Rust hyperon bindings
│   └── synergy.sh         # Unified synergy check script
│
├── CI/CD
│   └── .github/workflows/
│       ├── occ-win-build.yml       # Windows build with vcpkg caching
│       ├── occ-win-build-fast.yml  # Fast Windows build (vendored deps)
│       ├── vcpkg-prebuild.yml      # Tiered vcpkg dependency pre-building
│       ├── heavy-deps-build.yml    # Heavy deps (protobuf, grpc, rocksdb)
│       ├── vendor-builds.yml       # Smart incremental vendoring
│       ├── vendor-dependencies.yml # Vendor vcpkg dependencies
│       ├── electron-app-build.yml  # Electron desktop app build
│       └── wincogpre.yml           # Windows cogutil pre-build
│
└── Documentation
    ├── docs/              # Architecture and guides
    ├── README.md          # Project overview
    └── CONTRIBUTING.md    # Contribution guidelines
```

## Build System

### Primary Build Methods

1. **GNU Guix (Recommended for Reproducibility)**
   ```bash
   guix build -f guix.scm
   ```

2. **CMake Direct Build**
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   ```

3. **Makefile Interface (AGI-OS Stack)**
   ```bash
   make all          # Build complete stack
   make guix-build   # Build via Guix
   make direct-build # Build directly
   make test         # Run tests
   ```

### Build Dependency Order

Components must be built in this order due to dependencies:

**Foundation Layer:**
1. `cogutil` - Foundation utilities (build first, no dependencies)

**Core Layer:**
2. `atomspace` - Hypergraph database (requires cogutil)
3. `coggml` - CogGML self-aware microkernel (requires cogutil)
4. `atomspace-accelerator` - Inference engine (can build independently)

**Storage Layer:**
5. `atomspace-storage` - Base storage API (requires atomspace)
6. `atomspace-rocks` - RocksDB backend (requires atomspace-storage)
7. `atomspace-cog` - Network storage (requires atomspace-storage)

**Networking & Framework:**
8. `cogserver` - Networking/IPC (requires atomspace + atomspace-storage)
9. `matrix` - Sparse matrix support (requires atomspace)
10. `learn` - Symbolic learning (requires atomspace)
11. `agents` - Interactive agents (requires atomspace)
12. `sensory` - Dataflow system (requires atomspace)

**Reasoning Layer:**
13. `unify` - Unification algorithms (requires atomspace)
14. `ure` - Unified Rule Engine (requires unify)
15. `miner` - Pattern mining (requires ure)
16. `pln` - Probabilistic Logic Networks (requires ure)
17. `attention` - Attention allocation (requires atomspace)

**Cognitive Integration:**
18. `cogself` - AGI synergy framework (requires coggml)
19. `agentic-chatbots` - Conversational AI (can build independently)

### CMake Build Options

Key options in `CMakeLists.txt`:

```cmake
OPTION(BUILD_COGUTIL "Build CogUtil library" ON)
OPTION(BUILD_ATOMSPACE "Build AtomSpace hypergraph database" ON)
OPTION(BUILD_COGSERVER "Build CogServer networking" ON)
OPTION(BUILD_COGGML "Build CogGML self-aware microkernel" ON)
OPTION(BUILD_COGSELF "Build CogSelf AGI synergy framework" ON)
OPTION(BUILD_ATOMSPACE_ACCELERATOR "Build AtomSpace accelerator" ON)
OPTION(BUILD_AGENTIC_CHATBOTS "Build agentic chatbots integration" ON)
```

## Development Workflow

### System Dependencies (Ubuntu/Debian)

```bash
sudo apt-get install -y \
  build-essential cmake \
  libboost-all-dev \
  guile-3.0-dev \
  python3-dev cython3 \
  liboctomap-dev liboctomap-tools \
  cxxtest valgrind doxygen
```

### Using the Devcontainer

The recommended development environment uses VS Code devcontainers:

```bash
git clone --recurse-submodules https://github.com/Kaw-Aii/occ.git
# Open in VS Code with Remote Containers extension
```

### Running Synergy Checks

```bash
./synergy.sh  # Unified build and interoperability test
```

## Code Conventions

### C++ Standards

- C++17 standard required
- Position-independent code enabled
- Use CxxTest for unit tests
- Follow OpenCog coding style

### Python

- Python 3 with Cython bindings
- Use `requirements.txt` for dependencies
- NumPy, Pandas, scikit-learn for ML demos

### Scheme/Guile

- Guile 3.0 for AtomSpace scripting
- Scheme modules in `synergy/` directory
- `.scm` files for cognitive reasoning

### Rust (Hyperon Bindings)

- Minimal Rust used for `hyperon` bindings
- `cdylib` crate type for C interop
- Located in `src/lib.rs`

## Testing

### Running Tests

```bash
# Per-component tests
cd cogutil/build && make check
cd atomspace/build && make check

# Integration tests
make test-integration
./test-integration.sh
```

### CI/CD Pipeline

The repository uses multiple GitHub Actions workflows for comprehensive CI/CD:

#### Windows Build Workflows
- **`occ-win-build.yml`**: Full Windows build using vcpkg with GitHub Actions binary caching
  - Builds: cogutil → atomspace → moses (parallel) → cogserver
  - Uses prebuilt vcpkg packages from `vcpkg-prebuild.yml`
  - First build: ~2 hours, subsequent: ~5-10 minutes

- **`occ-win-build-fast.yml`**: Fast Windows build using vendored dependencies
  - Requires `vendor-dependencies.yml` to run first
  - Build time: ~5-10 minutes (no vcpkg rebuild)

- **`vcpkg-prebuild.yml`**: Tiered vcpkg dependency pre-building
  - Tier 1: Quick packages (Boost headers, header-only libs) ~5-10 min
  - Tier 2: Medium packages (Boost compiled, fmt, spdlog) ~15-20 min
  - Tier 3: Heavy packages (gRPC, RocksDB, Protobuf) ~100 min
  - Runs weekly or on vcpkg.json changes

#### Vendoring Workflows
- **`vendor-builds.yml`**: Smart incremental vendoring
  - Calculates source hashes to detect changes
  - Only rebuilds components with modified source
  - Commits prebuilt binaries to repository

- **`vendor-dependencies.yml`**: Vendors vcpkg dependencies

#### Electron App
- **`electron-app-build.yml`**: Builds desktop app for Windows/Linux
  - Triggered after successful Windows build
  - Creates installers (.exe, .msi, .AppImage, .deb)

## Key Files for AI Assistants

When working on this codebase, pay attention to:

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Root build configuration |
| `guix.scm` | Guix package definition |
| `Makefile` | AGI-OS unified build interface |
| `Makefile.build-sequences` | CMake build sequences |
| `vcpkg.json` | Windows vcpkg dependencies |
| `synergy.sh` | Integration verification |
| `.github/workflows/occ-win-build.yml` | Windows CI pipeline |
| `.github/workflows/vcpkg-prebuild.yml` | vcpkg dependency pre-building |
| `docs/architecture.md` | Architecture overview |
| `CONTRIBUTING.md` | Contribution guidelines |

## Common Tasks

### Adding a New Component

1. Create directory with `CMakeLists.txt`
2. Add `add_subdirectory()` in root `CMakeLists.txt`
3. Update `guix.scm` if needed
4. Add to CI workflow build order

### Modifying Build Configuration

- Edit `CMakeLists.txt` for CMake options
- Edit `guix.scm` for Guix package changes
- Update `Makefile` for AGI-OS integration

### Debugging Build Issues

1. Check dependency order in CI logs
2. Verify system dependencies are installed
3. Review `build/CMakeCache.txt` for configuration
4. Check component-specific `CMakeLists.txt`

## Architecture Notes

### Three-Layer AGI-OS Stack

1. **Cognumach (Layer 1)**: Cognitive microkernel
2. **HurdCog (Layer 2)**: OS services with cognitive extensions
3. **OCC (Layer 3)**: Full OpenCog cognitive architecture

### AtomSpace

The AtomSpace is the central knowledge representation system:
- Hypergraph database for symbolic AI
- Atoms = Nodes + Links with TruthValues
- Supports multiple storage backends
- Accessible via C++, Python, Scheme, and REST APIs

### Cognitive Synergy Modules

Located in `synergy/`:
- `core/` - Core synergy primitives
- `bridges/` - Inter-component bridges
- `identity/` - System identity management
- `membranes/` - Cognitive membrane patterns

## Important Conventions

1. **Never commit build artifacts** - Check `.gitignore`
2. **Test before committing** - Run `./synergy.sh`
3. **Follow semantic versioning** for releases
4. **Update documentation** when changing interfaces
5. **Use reproducible builds** via Guix when possible

## Windows Build System

The repository includes comprehensive Windows build support using vcpkg for dependency management.

### vcpkg Dependencies (`vcpkg.json`)

Core dependencies for Windows builds:
- **Boost**: system, filesystem, program-options, regex, thread, date-time, serialization, random
- **Networking**: asio, cppzmq, zeromq, curl (with SSL)
- **Serialization**: nlohmann-json, yaml-cpp
- **Database**: libpq (PostgreSQL), rocksdb
- **Utilities**: cxxopts, catch2, spdlog, fmt, tbb, eigen3, openssl, zlib, lz4, hwloc

**Note**: gRPC, Protobuf, and related dependencies (abseil, re2, c-ares) are installed separately.

### Windows Build Commands

```powershell
# Using vcpkg (recommended for Windows)
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
  -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel 4
```

### Build Order on Windows

The Windows CI builds components in this order:
1. **CogUtil** (foundation, no dependencies)
2. **AtomSpace** (requires CogUtil)
3. **Moses** (requires CogUtil, parallel with AtomSpace)
4. **CogServer** (requires AtomSpace, sequential after it)

### Electron Desktop App

The `electron-app/` directory contains an Electron-based desktop application:
- Native Node.js addon for OpenCog integration
- Cross-platform builds (Windows, Linux, macOS)
- Triggered automatically after successful Windows builds

## Tensor Logic - Neural-Symbolic Integration

Tensor Logic provides Multi-Entity Network-Aware Tensor-Enhanced Cognitive Synergy by integrating ATen tensors with AtomSpace.

### Components

| Component | Description |
|-----------|-------------|
| `aten/` | ATen C++11 tensor library (from PyTorch) |
| `atenspace/` | ATenSpace bridges symbolic AI with neural tensor embeddings |
| `tensor-logic/` | Integration layer for cognitive synergy |

### Features

1. **Multi-Entity AtomSpace**: Tensor representations across multiple cognitive entities
   - Entity state management with tensor embeddings
   - Entity synchronization and consensus computation
   - Cross-entity attention propagation

2. **Multi-Scale Analysis**: Time-series tensors at different granularities
   - Wavelet-like multi-scale decomposition
   - Pattern detection at multiple temporal scales
   - Trend and noise extraction

3. **Network-Aware DAS**: Flow tensors capturing Distributed AtomSpace movement patterns
   - Flow recording and history tracking
   - Optimal routing prediction
   - Node placement optimization based on tensor similarity

4. **ESN Reservoir Computing**: Echo State Networks for temporal processing
   - Reservoir transformation of inputs
   - Sequence prediction
   - Trainable output weights

5. **Tensor-Enhanced AtomSpace**: Mathematical operations on atomspace structures
   - Atom-to-tensor conversion
   - Tensor-based truth value computation
   - Similarity search using embeddings

### CMake Build Options

```cmake
OPTION(BUILD_ATEN "Build ATen tensor library" ON)
OPTION(BUILD_ATENSPACE "Build ATenSpace (AtomSpace + Tensor embeddings)" ON)
OPTION(BUILD_TENSOR_LOGIC "Build Tensor Logic integration layer" ON)
```

### Usage Example

```cpp
#include <opencog/tensor-logic/TensorLogic.h>

using namespace opencog::tensor_logic;

// Create tensor logic instance
TensorLogic::TensorConfig config;
config.embedding_dim = 128;
config.reservoir_size = 1000;

TensorLogic tl(config);
tl.initialize();

// Create cognitive entities
auto entity1 = tl.createEntity("agent1");
auto entity2 = tl.createEntity("agent2");

// Compute entity similarity
auto similarity = tl.computeEntitySimilarity(entity1, entity2);

// Multi-scale analysis
std::vector<float> signal = /* time series data */;
auto scales = tl.multiScaleDecomposition(signal);

// ESN temporal processing
auto reservoir_state = tl.reservoirTransform(signal);

// Compute cognitive synergy
auto synergy = tl.computeCognitiveSynergy({entity1, entity2}, scales);
```

## External Integrations (Optional)

These are disabled by default:
- `gnucash/` - Cognitive accounting
- `koboldcpp/` - Story/world modeling
- `aphrodite-engine/` - LLM inference

Enable with CMake flags: `-DBUILD_GNUCASH=ON`, etc.

## Troubleshooting

### Windows Build Issues

1. **vcpkg cache issues**: Clear cache with `rmdir /s /q vcpkg_cache`
2. **Missing Boost components**: Run `vcpkg install boost-serialization:x64-windows` explicitly
3. **Long build times**: Use `occ-win-build-fast.yml` with vendored dependencies
4. **CMake configuration fails**: Ensure `VCPKG_ROOT` environment variable is set

### Linux Build Issues

1. **Missing Guile**: Install with `sudo apt-get install guile-3.0-dev`
2. **Boost not found**: Install with `sudo apt-get install libboost-all-dev`
3. **CxxTest failures**: Tests use `continue-on-error` - check logs for details

### CI/CD Issues

1. **Workflow timeouts**: vcpkg-prebuild.yml has 180-minute timeout for heavy packages
2. **Artifact download fails**: Use `dawidd6/action-download-artifact@v3` for cross-workflow artifacts
3. **Cache misses**: Check cache key patterns in workflow files

## Resources

- [OpenCog Wiki](https://wiki.opencog.org/)
- [AtomSpace Documentation](https://wiki.opencog.org/w/AtomSpace)
- [GNU Guix Manual](https://guix.gnu.org/manual/)
- [vcpkg Documentation](https://vcpkg.io/en/docs/)
- [Project Repository](https://github.com/o9nn/occ)
