# AGI-OS Integration Guide

## Overview

The **AGI Operating System (AGI-OS)** represents a revolutionary three-layer architecture that integrates cognitive computing capabilities from the microkernel level up through the operating system to the application framework. This document provides comprehensive guidance for building, deploying, and developing with the AGI-OS stack.

## Three-Layer Architecture

### Layer 0: Cognumach Microkernel

**Cognumach** is an enhanced version of the GNU Mach microkernel, providing the foundational primitives for the AGI-OS stack.

#### Key Features
- Advanced memory management with copy-on-write optimization
- Symmetric multiprocessing (SMP) support for parallel cognitive processing
- Enhanced virtual memory system for large knowledge bases
- Modernized device driver infrastructure
- Kernel debugger support for development
- Message-passing IPC optimized for cognitive agents

#### Architecture
- **Target**: i386 (32-bit x86)
- **Build System**: Autotools
- **Dependencies**: gcc-multilib, mig, binutils

#### Building Cognumach

```bash
cd opencog-debian/cognumach
./update-cognumach.sh
cd cognumach-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../cognumach_*.deb
```

#### Installation

```bash
sudo apt-get install cognumach cognumach-dev
```

### Layer 1: HurdCog Cognitive Operating System

**HurdCog** is an OpenCog-powered GNU Hurd system that integrates cognitive computing primitives at the operating system level.

#### Key Features
- **Cognitive Fusion Reactor**: Multi-paradigm AI integration
- **Master Control Dashboard**: Real-time system monitoring
- **MachSpace**: AtomSpace integration at the OS level
- **Cognitive-grip**: Abstraction layer for hypergraph operations
- **Learning Translators**: System services that adapt and learn
- **Distributed Cognition**: Cognitive processing across Hurd translators

#### Architecture
- **Target**: i386 (32-bit x86)
- **Build System**: Autotools + CMake (for cognitive components)
- **Dependencies**: cognumach, libcogutil-dev, opencog-atomspace

#### Components

1. **Core System Servers**
   - `auth`: Authentication with cognitive trust models
   - `proc`: Process management with attention allocation
   - `pfinet`: Network stack with adaptive routing
   - `ext2fs`: Filesystem with semantic indexing

2. **Cognitive Translators**
   - `cogfs`: Filesystem translator with AtomSpace backend
   - `cognet`: Network translator with learning capabilities
   - `cogproc`: Process translator with resource optimization

3. **Master Control Dashboard**
   - Web-based monitoring interface
   - Real-time AtomSpace visualization
   - Cognitive process analytics
   - System performance metrics

#### Building HurdCog

```bash
cd opencog-debian/hurdcog
./update-hurdcog.sh
cd hurdcog-<version>
sudo apt-get build-dep .
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../hurdcog_*.deb ../hurdcog-dev_*.deb ../hurdcog-dashboard_*.deb
```

#### Installation

```bash
sudo apt-get install hurdcog hurdcog-dev hurdcog-dashboard
```

### Layer 2: OpenCog Collection

The **OpenCog Collection** provides the complete AGI research platform with hypergraph knowledge representation, probabilistic reasoning, and cognitive synergy.

#### Core Components

1. **Foundation**
   - `libcogutil-dev`: Low-level C++ utilities

2. **Knowledge Representation**
   - `opencog-atomspace`: Hypergraph database
   - `opencog-atomspace-cog`: CogServer storage backend
   - `opencog-atomspace-rocks`: RocksDB storage backend
   - `opencog-atomspace-pgres`: PostgreSQL storage backend

3. **Reasoning and Learning**
   - `opencog-ure`: Unified Rule Engine
   - `opencog-pln`: Probabilistic Logic Networks
   - `opencog-attention`: Economic Attention Networks
   - `opencog-miner`: Pattern mining
   - `opencog-learn`: Language learning
   - `opencog-generate`: Natural language generation

4. **Specialized Systems**
   - `opencog-moses`: Evolutionary optimization
   - `opencog-vision`: Visual perception
   - `opencog-agi-bio`: Bioinformatics applications

#### Building OpenCog

```bash
cd opencog-debian
./build-all-packages.sh
```

Or build individual packages:

```bash
cd opencog-debian/cogutil
./update-cogutil.sh
cd cogutil-<version>
dpkg-buildpackage -rfakeroot -us -uc
sudo dpkg -i ../libcogutil-dev_*.deb
```

## Unified AGI-OS Package

The `agi-os-unified` meta-package installs the complete three-layer stack.

### Installation

```bash
sudo apt-get install agi-os-unified
```

This will install:
- Cognumach microkernel
- HurdCog cognitive OS
- Complete OpenCog Collection
- Integration tools and utilities

### Initialization

After installation, initialize the AGI-OS environment:

```bash
sudo agi-os-init
```

This creates:
- `/etc/agi-os/` - Configuration directory
- `/var/lib/agi-os/` - Data directory
- `/var/log/agi-os/` - Log directory
- Default configuration files

### Status Check

Check the status of all AGI-OS components:

```bash
agi-os-status
```

## Build System

### Automated Build

The repository includes a comprehensive build system that handles dependency ordering and parallel builds:

```bash
cd opencog-debian
./build-all-packages.sh
```

#### Build Options

```bash
# Build with custom parallel jobs
PARALLEL_JOBS=8 ./build-all-packages.sh

# Build without installing packages
INSTALL_PACKAGES=no ./build-all-packages.sh

# Build including AGI-OS components
BUILD_AGI_OS=yes ./build-all-packages.sh
```

### CI/CD Integration

For continuous integration:

```bash
cd opencog-debian
./ci-build.sh
```

This script:
- Detects CI environment (GitHub Actions, GitLab CI)
- Installs build dependencies
- Builds all packages
- Collects artifacts

### Manual Build Order

If building manually, follow this dependency order:

1. **Stage 1**: `cogutil`
2. **Stage 2**: `atomspace`
3. **Stage 3**: `atomspace-cog`, `atomspace-rocks`, `atomspace-pgres` (parallel)
4. **Stage 4**: `cogserver`, `ure` (parallel)
5. **Stage 5**: `attention`, `pln`, `miner`, `unify`, `spacetime` (parallel)
6. **Stage 6**: `learn`, `generate` (parallel)
7. **Stage 7**: `lg-atomese`, `relex` (parallel)
8. **Stage 8**: `moses`, `asmoses`, `agi-bio`, `vision` (parallel)
9. **Stage 9**: `opencog`

For AGI-OS:
- **Stage 0**: `cognumach`
- **Stage 1.5**: `hurdcog`
- **Stage 10**: `agi-os-unified`

## Development

### Setting Up Development Environment

```bash
# Install development packages
sudo apt-get install agi-os-dev

# This includes:
# - cognumach-dev
# - hurdcog-dev
# - libcogutil-dev
# - build-essential
# - cmake
# - guile-3.0-dev
# - python3-dev
```

### Creating Cognitive Translators

HurdCog allows you to create system servers that integrate with the AtomSpace:

```c
#include <hurd.h>
#include <opencog/atomspace/AtomSpace.h>
#include <hurdcog/cognitive-grip.h>

// Example: Cognitive filesystem translator
class CognitiveFS : public Translator {
public:
    CognitiveFS(AtomSpace* as) : atomspace(as) {}
    
    // Implement translator interface
    error_t read(off_t offset, size_t* len, void* buf) {
        // Access AtomSpace for semantic file content
        Handle file_handle = atomspace->get_handle(current_file);
        // ... implementation
    }
    
private:
    AtomSpace* atomspace;
};
```

### AtomSpace Integration

Access the system-wide AtomSpace from your applications:

```python
from opencog.atomspace import AtomSpace
from opencog.type_constructors import *

# Connect to system AtomSpace
atomspace = AtomSpace()

# Add knowledge
concept = ConceptNode("AGI-OS")
property = PredicateNode("is_cognitive")
evaluation = EvaluationLink(property, concept)

# Query knowledge
results = atomspace.get_atoms_by_type(ConceptNode)
```

### Cognitive Synergy Development

Leverage multiple AI components together:

```scheme
; Load cognitive synergy modules
(use-modules (opencog))
(use-modules (opencog pln))
(use-modules (opencog attention))
(use-modules (opencog ure))

; Configure attention allocation
(cog-set-af-boundary! 100)

; Run PLN inference
(pln-bc target-atom)

; Pattern mining with attention
(cog-mine (cog-get-atoms 'ConceptNode #t))
```

## Configuration

### AtomSpace Configuration

Edit `/etc/agi-os/config.scm`:

```scheme
;; Storage backend
(define atomspace-config
  '((storage-type . "rocksdb")
    (storage-path . "/var/lib/agi-os/atomspace")
    (cache-size . 1000000)))

;; Enable persistence
(use-modules (opencog persist-rocks))
(define storage (RocksStorageNode "rocks:///var/lib/agi-os/atomspace"))
(cog-open storage)
```

### CogServer Configuration

```scheme
;; Network configuration
(define cogserver-config
  '((port . 17001)
    (network-interface . "127.0.0.1")
    (max-connections . 100)))
```

### HurdCog Configuration

Edit `/etc/hurdcog/hurdcog.conf`:

```ini
[cognitive-fusion]
enable_learning = true
attention_allocation = ecan
reasoning_engine = pln

[machspace]
atomspace_path = /var/lib/agi-os/atomspace
sync_interval = 60

[dashboard]
port = 8080
enable_visualization = true
```

## Performance Tuning

### Memory Management

For large knowledge bases:

```bash
# Increase AtomSpace cache
echo "atomspace.cache_size = 10000000" >> /etc/agi-os/config.scm

# Configure swap for virtual memory
sudo swapon -s
```

### Parallel Processing

Enable SMP in Cognumach:

```bash
# Boot with SMP support
grub-mkconfig -o /boot/grub/grub.cfg
# Add: cognumach.smp=yes
```

### Storage Optimization

Choose appropriate backend:

- **RocksDB**: Best for write-heavy workloads
- **PostgreSQL**: Best for complex queries and ACID compliance
- **CogServer**: Best for distributed access

## Monitoring and Debugging

### Dashboard Access

Access the Master Control Dashboard:

```bash
# Start dashboard service
sudo systemctl start hurdcog-dashboard

# Access at http://localhost:8080
```

### Logging

View AGI-OS logs:

```bash
# System logs
tail -f /var/log/agi-os/system.log

# AtomSpace operations
tail -f /var/log/agi-os/atomspace.log

# Cognitive processes
tail -f /var/log/agi-os/cognition.log
```

### Debugging

Enable debug mode:

```bash
# Set debug level
export OPENCOG_LOG_LEVEL=DEBUG

# Enable Cognumach kernel debugging
sudo sysctl kernel.cognumach.debug=1
```

## Use Cases

### Natural Language Understanding

```python
from opencog.nlp import *
from opencog.pln import *

# Parse text
text = "The AGI-OS enables cognitive computing"
parsed = parse_text(text)

# Perform reasoning
inference_results = pln_inference(parsed)
```

### Bioinformatics

```scheme
; Load biological knowledge
(load "bioinformatics-kb.scm")

; Mine patterns in protein interactions
(cog-mine (cog-get-atoms 'ProteinNode #t))

; Reason about gene relationships
(pln-bc (EvaluationLink (PredicateNode "regulates") 
                        (ListLink gene1 gene2)))
```

### Autonomous Systems

```python
from opencog.robotics import *
from opencog.attention import *

# Allocate attention to sensory input
allocate_attention(visual_atoms)

# Plan actions with PLN
action_plan = plan_actions(goal_atom)

# Execute with learning
execute_and_learn(action_plan)
```

## Troubleshooting

### Build Issues

**Problem**: Missing build dependencies

```bash
# Install all build dependencies
sudo apt-get build-dep opencog-atomspace
```

**Problem**: Parallel build failures

```bash
# Build with single thread
PARALLEL_JOBS=1 ./build-all-packages.sh
```

### Runtime Issues

**Problem**: AtomSpace connection failed

```bash
# Check CogServer status
sudo systemctl status cogserver

# Restart CogServer
sudo systemctl restart cogserver
```

**Problem**: Memory exhaustion

```bash
# Increase swap space
sudo fallocate -l 8G /swapfile
sudo chmod 600 /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
```

## Contributing

### Package Maintenance

To add a new component to the packaging system:

1. Create package directory: `opencog-debian/new-component/`
2. Add `debian/` subdirectory with packaging files
3. Create `update-new-component.sh` script
4. Update `BUILD_ORDER.md` with dependency information
5. Add to `build-all-packages.sh` in appropriate stage

### Testing

Run validation:

```bash
cd opencog-debian
./validate-packaging.sh
```

### Documentation

Update documentation when adding features:
- `PACKAGING_ARCHITECTURE.md` - Package structure
- `BUILD_ORDER.md` - Build dependencies
- `AGI_OS_INTEGRATION.md` - Integration guide

## References

### OpenCog Documentation
- [OpenCog Wiki](https://wiki.opencog.org/)
- [AtomSpace Documentation](https://wiki.opencog.org/w/AtomSpace)
- [PLN Documentation](https://wiki.opencog.org/w/PLN)

### Debian Packaging
- [Debian Policy Manual](https://www.debian.org/doc/debian-policy/)
- [Debian New Maintainers' Guide](https://www.debian.org/doc/manuals/maint-guide/)

### GNU Hurd
- [GNU Hurd Documentation](https://www.gnu.org/software/hurd/)
- [GNU Mach Documentation](https://www.gnu.org/software/hurd/microkernel/mach/gnumach.html)

### AGI-OS Project
- [GitHub Repository](https://github.com/cogpy/occ)
- [Issue Tracker](https://github.com/cogpy/occ/issues)
- [Discussions](https://github.com/cogpy/occ/discussions)

## License

The AGI-OS stack is released under multiple licenses:

- **Cognumach**: GPL-2.0 (GNU Mach license)
- **HurdCog**: AGPL-3.0 (with GPL-2.0 components from Hurd)
- **OpenCog**: AGPL-3.0
- **Documentation**: CC-BY-SA-4.0

See individual package copyright files for detailed license information.

## Support

For support and questions:
- GitHub Issues: https://github.com/cogpy/occ/issues
- Mailing List: opencog@googlegroups.com
- IRC: #opencog on Libera.Chat

---

**AGI-OS** - Building the Future of Cognitive Computing
