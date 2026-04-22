# AGI-OS Integration: OCC + HurdCog + Cognumach

This directory contains the integration build system for combining the OpenCog Collection (OCC), HurdCog, and Cognumach into a unified AGI-enabled operating system.

## Overview

The integration creates a three-layer architecture:

1. **Layer 1 - Cognumach**: Enhanced GNU Mach microkernel with advanced memory management, SMP enhancements, and VM optimizations
2. **Layer 2 - HurdCog**: OpenCog-powered GNU Hurd cognitive operating system with learning, reasoning, and self-adaptation capabilities
3. **Layer 3 - OCC**: OpenCog Collection AGI research platform with comprehensive tools for cognitive synergy

## Quick Start

### Prerequisites

```bash
# Install dependencies
make dev-setup
```

### Build Options

#### Option 1: Using GNU Guix (Recommended)

```bash
# Build the complete unified stack
make guix-build

# Or build individual components
make cognumach    # Build Cognumach microkernel
make hurdcog      # Build HurdCog OS
make occ          # Build OCC framework
```

#### Option 2: Direct Build (Development)

```bash
# Build all components directly
make direct-build

# Or use the quick start
make quickstart
```

### Testing

```bash
# Run integration tests
make test
```

### Installation

```bash
# Install to system (requires sudo and Guix)
make install
```

## Files

- **cognumach.scm** - Guix package definition for Cognumach microkernel
- **hurdcog.scm** - Guix package definition for HurdCog OS
- **occ-hurdcog-unified.scm** - Unified Guix package for complete stack
- **Makefile** - Build system for integration
- **integration_architecture.md** - Detailed architecture documentation
- **integration_analysis.md** - Repository analysis and integration strategy

## Architecture

### Component Relationships

```
┌─────────────────────────────────────────────────────────┐
│                    Layer 3: OCC                         │
│  (Cognitive Architecture & AGI Research Platform)       │
│  - AtomSpace, PLN, ECAN, Pattern Mining                 │
│  - Cognitive Synergy Framework                          │
│  - AGI Development Tools                                │
└─────────────────────────────────────────────────────────┘
                          ↕
┌─────────────────────────────────────────────────────────┐
│                  Layer 2: HurdCog                       │
│  (Cognitive Operating System Services)                  │
│  - Cognitive Fusion Reactor                             │
│  - Master Control Dashboard                             │
│  - GNU Hurd Servers & Translators                       │
│  - Distributed Systems (Plan9, Inferno)                 │
└─────────────────────────────────────────────────────────┘
                          ↕
┌─────────────────────────────────────────────────────────┐
│                 Layer 1: Cognumach                      │
│  (Enhanced Microkernel Foundation)                      │
│  - GNU Mach with Advanced Memory Management             │
│  - SMP Enhancements & VM Optimizations                  │
│  - IPC Mechanisms                                       │
│  - Device Drivers                                       │
└─────────────────────────────────────────────────────────┘
```

## Integration Strategy

### Phase 1: Microkernel Integration
- Replace HurdCog's GNU Mach with Cognumach
- Ensure compatibility with Hurd servers
- Test IPC mechanisms and device drivers

### Phase 2: Cognitive Layer Enhancement
- Sync OpenCog components from OCC to HurdCog
- Update AtomSpace, PLN, ECAN to latest versions
- Integrate additional OCC tools and frameworks

### Phase 3: Unified Build System
- Create Guix packages for all components
- Implement reproducible build environment
- Enable cross-compilation support

### Phase 4: Testing and Documentation
- Comprehensive integration testing
- Performance benchmarking
- Complete documentation

## Key Features

### Cognumach Enhancements
- Advanced memory management
- SMP (Symmetric Multi-Processing) support
- VM optimizations
- PCI modernization
- Valgrind integration

### HurdCog Capabilities
- Cognitive Fusion Reactor
- Master Control Dashboard
- AtomSpace-based knowledge representation
- PLN reasoning under uncertainty
- ECAN resource allocation
- Self-diagnosis and self-healing

### OCC Framework
- Comprehensive OpenCog ecosystem
- Cognitive synergy architecture
- GNU Guix reproducible builds
- Devcontainer support
- Extensive AGI research tools

## Usage

### Starting the System

```bash
# Start HurdCog dashboard
cd /home/ubuntu/hurdcog/cogkernel
./start-dashboard.sh

# Access at http://localhost:8080/dashboard
```

### Running Tests

```bash
# Integration tests
make test

# HurdCog cognitive tests
cd /home/ubuntu/hurdcog
make cognitive-test

# OCC demos
cd /home/ubuntu/occ
python3 app.py
```

## Development

### Building Individual Components

```bash
# Build Cognumach
cd /home/ubuntu/cognumach
./configure --host=i686-gnu CC='gcc -m32' LD='ld -melf_i386'
make -j$(nproc)

# Build HurdCog cogkernel
cd /home/ubuntu/hurdcog/cogkernel
make -j$(nproc)

# Build OCC
cd /home/ubuntu/occ
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Adding New Components

1. Update the appropriate Guix package file (cognumach.scm, hurdcog.scm, or occ-hurdcog-unified.scm)
2. Rebuild using `make guix-build`
3. Test the integration with `make test`

## Troubleshooting

### Common Issues

**Issue:** Guix not found
- **Solution:** Install GNU Guix or use `make direct-build`

**Issue:** 32-bit compilation errors
- **Solution:** Install gcc-multilib: `sudo apt-get install gcc-multilib`

**Issue:** MIG not found
- **Solution:** Install MIG: `sudo apt-get install mig` or build from source

**Issue:** Guile modules not found
- **Solution:** Set GUILE_LOAD_PATH: `export GUILE_LOAD_PATH=/usr/share/guile/3.0`

## Documentation

- **integration_architecture.md** - Detailed integration architecture
- **integration_analysis.md** - Repository analysis and strategy
- **OCC README** - `/home/ubuntu/occ/README.md`
- **HurdCog README** - `/home/ubuntu/hurdcog/README.md`
- **Cognumach README** - `/home/ubuntu/cognumach/README`

## Contributing

This integration is a work in progress. Contributions are welcome in the following areas:

- Build system improvements
- Additional component integration
- Testing and validation
- Documentation
- Performance optimization

## License

This integration inherits the licenses of its components:
- Cognumach: GPL-2.0+
- HurdCog: GPL-2.0+
- OCC: GPL-3.0+

## Support

For issues and questions:
- GitHub Issues: Use the respective repository issue trackers
- Mailing Lists: OpenCog Google Group, GNU Hurd mailing list
- IRC: #hurd on libera.chat

## Acknowledgments

This integration builds upon the work of:
- The GNU Hurd project
- The OpenCog project and Dr. Ben Goertzel
- The GNU Mach microkernel developers
- All contributors to the respective repositories
