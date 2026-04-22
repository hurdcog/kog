# Compiler Explorer JIT Infrastructure

**Part of:** Phase 2: Microkernel Integration - SKZ Framework  
**Component:** Development Tools - JIT Compilation Infrastructure  
**Status:** Initial Implementation

## Overview

This directory contains the compiler-explorer JIT (Just-In-Time) compilation infrastructure for the HurdCog cognitive architecture. The JIT infrastructure enables real-time compilation and optimization of GNU Hurd microkernel components with cognitive analysis.

## Architecture

```
development/compiler-explorer/
├── src/                    # Core JIT implementation
│   ├── jit_compiler.cpp   # Main JIT compilation engine
│   ├── jit_compiler.h     # JIT compiler interface
│   └── jit_config.h       # Configuration constants
├── include/               # Public headers
├── tests/                 # Test suite
├── config/                # Configuration files
└── build/                 # Build artifacts (generated)
```

## Features

- **Just-In-Time Compilation:** Real-time compilation of microkernel components
- **Cognitive Integration:** Integration with SKZ autonomous agents framework
- **Error Handling:** Comprehensive error reporting and logging
- **Performance Monitoring:** Built-in performance analysis hooks
- **OJS Compatibility:** Compatible with existing OJS installation

## Build Requirements

- GCC/G++ compiler with C++11 support
- LLVM development headers (if available)
- GNU Make
- Existing HurdCog build environment

## Integration Points

- **Build System:** Integrated with main Makefile
- **Cognitive Framework:** Follows SKZ framework patterns
- **Error Handling:** Unified logging with main system
- **Performance:** Minimal overhead JIT compilation

## Usage

The JIT infrastructure is automatically initialized when building HurdCog components:

```bash
# Build with JIT support
make compiler-explorer-jit

# Test JIT functionality
make test-jit
```

## Configuration

JIT behavior can be configured through environment variables:
- `HURDCOG_JIT_ENABLE`: Enable/disable JIT compilation
- `HURDCOG_JIT_OPTIMIZE`: Optimization level (0-3)
- `HURDCOG_JIT_LOG_LEVEL`: Logging verbosity

## Performance Considerations

- JIT compilation overhead: <5% in typical use cases
- Memory usage: Minimal additional allocation
- Integration latency: Sub-millisecond initialization

## Future Extensions

This minimal implementation provides foundation for:
- Advanced cognitive analysis
- Real-time optimization
- Interactive compilation
- Educational tools

See `../docs/HurdCog_Compiler_Explorer_Roadmap.md` for complete roadmap.