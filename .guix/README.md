# Guix Packaging for OpenCog Collection

This directory contains GNU Guix packaging files for the OpenCog Collection (OCC) project - a monorepo integration of OpenCog components for cognitive synergy.

## Files

- **`.guix/modules/opencog-package.scm`** - Main package definition module
- **`.guix/manifest.scm`** - Development environment manifest with all dependencies
- **`.guix-channel`** - Channel definition for adding this repo as a Guix channel
- **`guix.scm`** - Top-level package file for building the complete system
- **`verify.sh`** - Validation script to test the configuration

## Usage

### Using the development environment

To enter a development shell with all dependencies:

```bash
guix shell -m .guix/manifest.scm
```

This provides you with:
- **C++ Development**: GCC, CMake, Boost, GSL, BLAS/LAPACK, CxxTest
- **Python Stack**: Python 3 with NumPy, Pandas, scikit-learn, Matplotlib, Jupyter
- **Rust Toolchain**: Rust and Cargo for Hyperon components
- **Scheme/Guile**: Guile with modules for OpenCog development
- **Build Tools**: pkg-config, autotools, Git, and development utilities

### Building the complete package

To build the integrated OpenCog Collection with all C++ components:

```bash
guix build -f guix.scm
```

This builds:
- **CogUtil** - Base utilities and configuration
- **AtomSpace** - Hypergraph database and query engine
- **CogServer** - Networking and communication layer
- **Matrix** - Sparse vector and graph processing
- **Learn** - Symbolic learning algorithms
- **Sensory** - Dataflow system for external world interaction
- **Python Demo** - Machine learning demonstration
- **Rust Hyperon** - Cognitive computing framework (if present)

### Manual CMake build

For development and testing:

```bash
guix shell -m .guix/manifest.scm
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

Build options:
- `-DBUILD_COGUTIL=ON/OFF` - Build CogUtil library
- `-DBUILD_ATOMSPACE=ON/OFF` - Build AtomSpace hypergraph database
- `-DBUILD_COGSERVER=ON/OFF` - Build CogServer networking
- `-DBUILD_MATRIX=ON/OFF` - Build Matrix sparse vector support
- `-DBUILD_LEARN=ON/OFF` - Build symbolic learning
- `-DBUILD_SENSORY=ON/OFF` - Build sensory dataflow system

### Installing the package

To install to your profile:

```bash
guix install -f guix.scm
```

### Using as a channel

Add to your `~/.config/guix/channels.scm`:

```scheme
(cons* (channel
        (name 'opencog-collection)
        (url "https://github.com/cogpy/occ.git")
        (branch "main"))
       %default-channels)
```

Then:

```bash
guix pull
guix install opencog-collection
```

## Package Structure

The integrated package provides:

### Core OpenCog Components
- **CogUtil** - Foundation utilities and macros
- **AtomSpace** - Hypergraph knowledge representation
- **CogServer** - Network protocol and shell
- **Matrix** - Sparse matrix operations for graphs
- **Learn** - Grammar learning and symbolic AI
- **Sensory** - I/O and perception framework

### Additional Components
- **Python Demo** - Machine learning with scikit-learn
- **Rust Hyperon** - Next-generation cognitive architecture
- **Development Environment** - Complete toolchain for research

## Validation

Test the configuration:

```bash
./.guix/verify.sh
```

This checks:
- Guix file structure and syntax
- CMake configuration
- Component availability
- Python and Rust components

## Dependencies

Automatically managed:
- **C++**: Boost, GSL, BLAS/LAPACK, protobuf, zlib, OpenSSL
- **Python**: NumPy, Pandas, scikit-learn, Matplotlib, Jupyter
- **Rust**: Cargo and toolchain
- **Scheme**: Guile 3.0 with modules
- **Build**: CMake, pkg-config, autotools, GCC

## Running Applications

After installation:

```bash
# Run Python demo
opencog-demo

# Run Hyperon (if built)
hyperon

# Or manually
python3 /gnu/store/.../share/opencog-collection/app.py
```

## Development Workflow

```bash
# Enter development environment
guix shell -m .guix/manifest.scm

# Build everything
mkdir build && cd build
cmake .. && make -j$(nproc)

# Build specific components
cmake .. -DBUILD_ATOMSPACE=ON -DBUILD_COGUTIL=ON -DBUILD_COGSERVER=OFF
make

# Test Python components
python3 ../app.py

# Build Rust components
cd .. && cargo build --release
```

## Integration Philosophy

This configuration supports the **cognitive synergy** goal by:

1. **Unified Build System** - Root CMakeLists.txt coordinates all components
2. **Dependency Management** - Guix ensures reproducible environments
3. **Modular Architecture** - Components can be built independently or together
4. **Multi-Language Support** - C++, Python, Rust, and Scheme integration
5. **Research Friendly** - Complete source available for exploration and modification