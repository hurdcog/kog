# GUIX Build System Integration

## Overview

This directory contains the GUIX build system integration for the GNU Hurd Cognitive Architecture project. It implements a staged compilation system using Guile Stage0-3 phases for building and integrating all cognitive system components.

## Architecture

The build system is organized into staged compilation phases:

```
guix-build-system/
├── README.md                    # This file
├── guile-stage0/               # Stage 0: Minimal Guile bootstrap
├── guile-stage1/               # Stage 1: Core Guile functionality  
├── guile-stage2/               # Stage 2: Full Guile with extensions
├── guile-stage3/               # Stage 3: AGI-OS specific features
└── atomspace-fs/               # Atomspace filesystem partition
```

## Build Stages

### Stage 0: Minimal Guile Bootstrap
**Location**: `guile-stage0/`
**Purpose**: Minimal Guile interpreter bootstrap
**Components**:
- Basic Guile interpreter
- Minimal runtime libraries
- Core bootstrapping functionality

**Key Files**:
- `stage0-bootstrap.scm` - Bootstrap configuration
- `minimal-guile.scm` - Minimal Guile setup
- `bootstrap-deps.scm` - Bootstrap dependencies

### Stage 1: Core Guile Functionality
**Location**: `guile-stage1/`
**Purpose**: Full Guile functionality with core libraries
**Components**:
- Complete Guile interpreter
- Standard library modules
- Basic extension support

**Key Files**:
- `stage1-core.scm` - Core functionality setup
- `stdlib-integration.scm` - Standard library integration
- `module-system.scm` - Module system configuration

### Stage 2: Full Guile with Extensions
**Location**: `guile-stage2/`
**Purpose**: Extended Guile with cognitive architecture support
**Components**:
- OpenCog integration modules
- Parallel computing support (Kokkos)
- Distributed system integration
- JIT compilation support

**Key Files**:
- `stage2-extensions.scm` - Extension integration
- `opencog-integration.scm` - OpenCog Atomspace integration
- `parallel-support.scm` - Kokkos parallel computing
- `distributed-modules.scm` - Plan9/Inferno integration

### Stage 3: AGI-OS Specific Features
**Location**: `guile-stage3/`
**Purpose**: Complete AGI-OS integration with all cognitive features
**Components**:
- Cognitive workflow engine
- Real-time learning systems
- Autonomous decision making
- Distributed agent framework

**Key Files**:
- `stage3-cognitive.scm` - Cognitive system integration
- `workflow-engine.scm` - Workflow orchestration
- `learning-systems.scm` - Learning system integration
- `autonomous-agents.scm` - Agent framework

### Atomspace Filesystem
**Location**: `atomspace-fs/`
**Purpose**: Partitioned filesystem for cognitive operations
**Components**:
- Atomspace data storage
- Knowledge graph persistence
- Cognitive operation caching
- Distributed data management

## Build Process

### Prerequisites
- GNU GUIX package manager
- Guile 3.0 or later
- OpenCog development libraries
- Kokkos parallel computing framework

### Building the System

#### Complete Build
```bash
# Build all stages
make guix-build-all

# Or build individual stages
make guix-stage0
make guix-stage1 
make guix-stage2
make guix-stage3
```

#### Stage-by-Stage Build
```bash
# Stage 0: Bootstrap
cd guix-build-system/guile-stage0
guix build -f stage0-bootstrap.scm

# Stage 1: Core functionality
cd ../guile-stage1
guix build -f stage1-core.scm

# Stage 2: Extensions
cd ../guile-stage2
guix build -f stage2-extensions.scm

# Stage 3: Cognitive features
cd ../guile-stage3
guix build -f stage3-cognitive.scm
```

#### Development Build
```bash
# Development environment
guix environment --ad-hoc guile guile-opencog kokkos-cpp

# Interactive development
guile -L guix-build-system
```

## Integration with SKZ Framework

The GUIX build system integrates with all SKZ framework components:

### 1. OpenCog AtomSpace Integration
- Distributed atomspace configuration
- Knowledge representation persistence  
- Cognitive operation optimization

### 2. GNU/Hurd Microkernel
- Microkernel server compilation
- System service integration
- Resource management optimization

### 3. Kokkos Parallel Computing
- Parallel execution optimization
- GPU acceleration support
- Memory management tuning

### 4. Plan9/Inferno Distributed Systems
- Namespace integration
- Distributed resource access
- Network transparency

### 5. ECMA-262 JavaScript Support
- JavaScript engine integration
- Web-based cognitive interfaces
- Real-time interaction support

## Configuration

### Build Configuration
```scheme
;; build-config.scm
(build-configuration
 '((stages . (stage0 stage1 stage2 stage3))
   (parallel-jobs . 4)
   (optimization-level . 2)
   (debug-symbols . #t)
   (cognitive-features . (learning workflows agents decisions))))
```

### Runtime Configuration
```scheme
;; runtime-config.scm
(runtime-configuration
 '((atomspace-size . "10GB")
   (agent-pool-size . 100) 
   (workflow-timeout . 300)
   (learning-buffer . 10000)))
```

## Performance Optimization

### Build Performance
- **Parallel Compilation**: Multi-core build support
- **Incremental Builds**: Only rebuild changed components
- **Caching**: Aggressive build artifact caching
- **Dependencies**: Optimized dependency resolution

### Runtime Performance
- **Memory Management**: Efficient atomspace allocation
- **CPU Utilization**: Balanced parallel processing
- **I/O Optimization**: Optimized filesystem operations
- **Network Performance**: Distributed system optimization

## Testing

### Build System Tests
```bash
# Test build system integrity
python3 test-guix-stages-integration.py

# Validate build artifacts
guix build --check -f guile-stage3/stage3-cognitive.scm

# Integration tests
make test-integration
```

### Runtime Tests
```bash
# Test atomspace filesystem
cd atomspace-fs
python3 test-integration.py

# Test cognitive system integration
cd ../..
python3 validate-phase3-completion.py
```

## Troubleshooting

### Common Build Issues

#### Missing Dependencies
```bash
# Install missing Guile modules
guix install guile guile-lib guile-json

# Install OpenCog dependencies  
guix install opencog-dev atomspace-dev
```

#### Build Failures
```bash
# Clean build environment
make clean-all

# Rebuild with verbose output
make V=1 guix-build-all

# Check build logs
less build-logs/stage*.log
```

#### Runtime Issues
```bash
# Check atomspace filesystem
df -h /mnt/atomspace

# Monitor cognitive system performance
top -p $(pidof guile)

# Check system logs
journalctl -u cognitive-system
```

## Development

### Adding New Build Stages
1. Create new stage directory
2. Implement stage-specific Scheme files
3. Update build configuration
4. Add integration tests
5. Update documentation

### Extending Existing Stages
1. Modify stage-specific files
2. Update dependencies
3. Test build integrity
4. Validate runtime functionality

### Contributing
- Follow GUIX packaging guidelines
- Maintain backward compatibility
- Add comprehensive tests
- Update documentation

## Integration Status

- ✅ **Stage 0**: Minimal Guile bootstrap - COMPLETE
- ✅ **Stage 1**: Core Guile functionality - COMPLETE  
- ✅ **Stage 2**: Full Guile with extensions - COMPLETE
- ✅ **Stage 3**: AGI-OS specific features - COMPLETE
- ✅ **Atomspace FS**: Partitioned filesystem - COMPLETE

## Future Enhancements

- Advanced caching mechanisms
- Distributed build support
- Enhanced debugging tools
- Performance profiling integration
- Automated optimization

## Related Documentation

- [SKZ Integration Strategy](../SKZ_INTEGRATION_STRATEGY.md)
- [Phase 3 Build Orchestration Summary](../PHASE3_BUILD_ORCHESTRATION_SUMMARY.md)
- [GUIX Integration Completion](../docs/GUIX_INTEGRATION_COMPLETION.md)
- [Architecture Documentation](../docs/ARCHITECTURE.md)

---

For technical support, see the [troubleshooting guide](#troubleshooting) or consult the [community resources](../docs/open-issues/community.md).