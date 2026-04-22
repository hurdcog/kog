---
description: "Build and install OpenCog Collection (OCC) with CMake or Guix. Choose by platform, speed, or reproducibility requirements."
name: "Build & Install OCC"
argument-hint: "Strategy: quick (CMake), full (Guix), or development"
agent: "agent"
tools: ["web", "search"]
---

# Build & Install OpenCog Collection

Help with building, installing, and testing the OCC monorepo on Linux/macOS. This prompt provides:
- **Quick CMake builds** for rapid prototyping (5-10 minutes)
- **Reproducible Guix builds** for consistency (15-30 minutes)
- **Development setups** with testing and incremental builds
- **Dependency validation** and troubleshooting

## Build Strategies

### 1. ⚡ Quick CMake Build (5 min - Recommended for Development)

**Best for**: Fast iteration, rapid prototyping, local testing

```bash
# Install dependencies first
sudo apt-get update && sudo apt-get install -y \
  build-essential cmake libboost-all-dev guile-3.0-dev \
  python3-dev cython3 liboctomap-dev liboctomap-tools cxxtest

# Build with minimal components
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_ATOMSPACE=ON \
  -DBUILD_ATOMSPACE_STORAGE=ON \
  -DBUILD_COGSERVER=ON \
  -DBUILD_LEARN=OFF \
  -DBUILD_URP=OFF \
  -DBUILD_TESTS=OFF
make -j$(nproc)

# Install (optional)
sudo make install
```

**Key Points:**
- Only builds core components (atomspace → storage → cogserver)
- Disables heavy components (learning, pattern mining)
- Disables tests to save compile time
- Use `-DBUILD_TESTS=ON` to run unit tests with `make test`

---

### 2. 🔒 Reproducible Guix Build (15-30 min)

**Best for**: CI/CD, reproducible releases, isolated environments

```bash
# Ensure Guix and channels configured
mkdir -p ~/.config/guix
# Copy guix.scm to workspace root if not present

# Full reproducible build
guix build -f guix.scm

# Or with substitutes (faster if available)
guix build -f guix.scm --with-commit=HEAD --check
```

**Key Points:**
- Fully hermetic: all dependencies declared in `guix.scm`
- Reproducible across machines and time
- Slower initial build but enables caching
- Substitute server pre-builds heavy dependencies

---

### 3. 🛠️ Development Build with Testing

**Best for**: Contributing code, validating changes

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_ATOMSPACE=ON \
  -DBUILD_ATOMSPACE_STORAGE=ON \
  -DBUILD_COGSERVER=ON \
  -DBUILD_TESTS=ON
make -j$(nproc)

# Run all unit tests
make test

# Or run specific test suite
ctest -R atomspace -V  # verbose output
```

**Key Points:**
- Debug symbols for debugging (`-DCMAKE_BUILD_TYPE=Debug`)
- Enables unit tests with CxxTest
- Slower compilation but easier debugging
- Use `ctest` for granular test control

---

## Dependency Checklist

Before building, verify system dependencies:

```bash
# Check all required packages
cmake --version          # ✓ Should be 3.12+
g++ --version           # ✓ Should be 7.0+
boost-config --version  # ✓ Boost libraries
guile --version         # ✓ GNU Guile 3.0+
python3 --version       # ✓ Python 3.6+
cython3 --version       # ✓ For Cython bindings
```

**Install missing dependencies:**

```bash
# Ubuntu/Debian
sudo apt-get install -y \
  build-essential cmake libboost-all-dev guile-3.0-dev \
  python3-dev cython3 liboctomap-dev liboctomap-tools cxxtest

# macOS (using Homebrew)
brew install cmake boost guile@3 python3 octomap
```

---

## Critical Build Order (11-Layer Dependency Chain)

The OCC uses strict layer ordering. **Building in the wrong order causes link errors.**

```
Layer 0: MIG (microkernel foundation)
  ↓
Layer 1: Cognumach (cognitive microkernel)
  ↓
Layer 2: HurdCog (cognitive OS services)
  ↓
Layer 3-4: 
  • cogutil       ← BUILD FIRST (foundation utilities)
  • Atomspace     ← depend on cogutil
  • Storage backends (RocksDB, PostgreSQL, CogServer)
  ↓
Layer 5-11: 
  • cogserver     ← MUST build storage first
  • learn, agents, PLN, etc.
```

**CMake handles ordering automatically** — don't worry about this unless you're building specific components in isolation.

---

## Common Build Issues & Fixes

| Issue | Solution |
|-------|----------|
| `"atomspace/config.h: No such file"` | Run `cmake` first; missing `cogutil` component |
| `"undefined reference to cogServerNet"` | Must build `atomspace-storage` before `cogserver` |
| `"cxxtest not found"` | Install `cxxtest`: `sudo apt-get install cxxtest` |
| `"guile-3.0 not found"` | Install Guile dev headers: `suite `sudo apt-get install guile-3.0-dev` |
| Build hangs on Windows | Use fast vendored build instead; see [QUICKSTART.md](../../QUICKSTART.md) |
| CMake cache errors | Clean build: `rm -rf build && mkdir build && cd build` |

---

## What to Build (Component Guidelines)

| Component | Build? | Reason |
|-----------|--------|--------|
| **cogutil** | ✅ Always | Foundation utilities; required by everything |
| **atomspace** | ✅ Always | Hypergraph database; core knowledge representation |
| **atomspace-storage** | ✅ Recommended | Storage backends (RocksDB, PostgreSQL); enables persistence |
| **cogserver** | ✅ Recommended | Network/IPC layer; required for distributed cognition |
| **learn** | ❌ Optional | Heavy; pattern mining and learning algorithms |
| **PLN** | ❌ Optional | Probabilistic Logic Networks; advanced reasoning |
| **URP** | ❌ Optional | Unified Rule Engine; advanced rule inferencing |
| **agents** | ✅ If needed | Interactive cognitive agents framework |
| **Tests** | ❓ Dev only | Enable only if `make test` needed; adds 30% build time |

---

## Installation & System Integration

After building:

```bash
# Install to system
cd build && sudo make install

# Verify installation
opencog-config --version          # Check installed version
pkg-config --cflags --libs libopencog  # Check pkg-config

# Python bindings
python3 -c "from opencog.atomspace import AtomSpace; print('OK')"
```

---

## Incremental Development Workflow

```bash
cd build

# Make changes to source files
# Then rebuild only affected components
make cogutil              # Rebuild just cogutil
ctest -R cogutil -V      # Test just cogutil

# Or rebuild everything
make -j$(nproc) && make test
```

---

## Next Steps

- **[BUILD_DEPENDENCY_ORDER.md](../../BUILD_DEPENDENCY_ORDER.md)**: Complete 11-layer dependency graph
- **[QUICKSTART.md](../../QUICKSTART.md)**: Platform-specific quick-start guides
- **[CONTRIBUTING.md](../../CONTRIBUTING.md)**: Contributing guidelines
- **[docs/architecture.md](../../docs/architecture.md)**: Architecture overview

---

💡 **Tip**: For CI/CD automation, see [.github/workflows/](../../.github/workflows/) for ready-made GitHub Actions workflows.
