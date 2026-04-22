# HurdCog Integration Guide for OCC

**Version**: 1.0.0  
**Date**: 2025-11-30  
**Status**: Active Development

---

## Overview

This guide provides comprehensive instructions for integrating the OpenCog Collection (OCC) with HurdCog (cognitive operating system) and Cognumach (enhanced microkernel) to create a unified AGI Operating System.

---

## Architecture

The AGI-OS stack consists of three integrated layers that work together to provide cognitive capabilities at every level of the system.

### Layer 1: Cognumach (Enhanced Microkernel)

Cognumach extends the GNU Mach microkernel with advanced features specifically designed for cognitive computing. Key enhancements include symmetric multiprocessing support, improved virtual memory management, and cognitive primitives at the kernel level.

The microkernel provides the foundation for all higher-level cognitive operations. It manages hardware resources, process scheduling, and inter-process communication through Mach ports. The enhanced VM subsystem includes predictive page fault handling and cognitive cache management that learns from access patterns.

### Layer 2: HurdCog (Cognitive Operating System)

HurdCog builds on top of Cognumach to provide OS-level cognitive capabilities. It implements a cognitive kernel (CogKernel) that integrates reasoning, learning, and self-monitoring into core OS operations.

The CogKernel maintains a persistent knowledge base using AtomSpace technology, enabling the OS to learn from user behavior, system events, and application patterns. The learning scheduler uses probabilistic logic to predict optimal process execution order, while the adaptive memory manager anticipates future memory needs.

### Layer 3: OCC (Cognitive Applications & Research Platform)

OCC provides the high-level cognitive architecture and research tools. It includes the AtomSpace hypergraph database, PLN reasoning engine, URE rule system, ECAN attention allocation, and various learning modules.

Applications at this layer can leverage the full cognitive stack, from kernel-level primitives through OS services to high-level reasoning capabilities. The integration enables seamless data flow between layers, allowing cognitive insights to propagate throughout the system.

---

## Integration Components

### AtomSpace-HurdCog Bridge

The bridge provides bidirectional communication between OCC's AtomSpace and HurdCog's cognitive kernel. It implements efficient serialization of atoms for kernel storage, IPC mechanisms using Mach ports, and callback registration for asynchronous events.

**Location**: `src/atomspace-hurdcog-bridge/`

**Key Files**:
- `bridge.h`: C++ interface definition
- `bridge.cpp`: Core implementation
- `python/atomspace_hurdcog.py`: Python bindings

**Usage Example** (Python):

```python
from atomspace_hurdcog import AtomSpaceHurdCogBridge, send_pln_inference

# Initialize bridge
bridge = AtomSpaceHurdCogBridge()
bridge.initialize("/dev/hurdcog/cogkernel")

# Send inference to kernel
result = send_pln_inference(
    "(InheritanceLink (ConceptNode \"Socrates\") (ConceptNode \"mortal\"))"
)
print(f"Inference result: {result}")

# Check statistics
stats = bridge.get_statistics()
print(f"Messages sent: {stats['messages_sent']}")
```

### Cognitive Dashboard

The dashboard provides real-time monitoring and visualization of the AGI-OS cognitive state. It displays AtomSpace statistics, inference activity, attention allocation, learning progress, and HurdCog kernel status.

**Location**: `src/cognitive-dashboard/`

**Starting the Dashboard**:

```bash
cd src/cognitive-dashboard
python3 server.py
```

Access at: `http://localhost:5000`

### Enhanced Autogenesis

The autogenesis framework enables autonomous system evolution through self-modification. It discovers improvement opportunities, generates implementations, validates changes through safety checks, and deploys enhancements with automatic rollback on failure.

**Location**: `src/autogenesis-enhanced/`

**Usage Example**:

```python
from evolution_manager import EvolutionManager

# Initialize manager
manager = EvolutionManager()

# Discover opportunities
candidates = manager.discover_evolution_opportunities()

# Process candidates
for candidate in candidates:
    if manager.validate_candidate(candidate):
        manager.generate_implementation(candidate)
        manager.deploy_evolution(candidate)

# Generate report
report = manager.get_evolution_report()
```

---

## Building and Installation

### Prerequisites

The system requires GNU Guix for reproducible builds, Guile 3.0 or later for Scheme support, Python 3.8 or later, CMake 3.10 or later, C++ compiler with C++17 support, and Boost libraries version 1.65 or later.

### Building with Guix

```bash
# Clone repository
git clone --recurse-submodules https://github.com/cogpy/occ.git
cd occ

# Build unified package
guix build -f occ-hurdcog-unified.scm

# Install
guix package -f occ-hurdcog-unified.scm
```

### Building Individual Components

```bash
# Build AtomSpace-HurdCog bridge
cd src/atomspace-hurdcog-bridge
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install

# Install Python bindings
cd ../python
pip3 install -e .
```

---

## Configuration

### Environment Variables

Set the following environment variables for proper integration:

```bash
export AGI_OS_ROOT=/path/to/occ
export HURDCOG_ROOT=/path/to/hurdcog
export COGNUMACH_ROOT=/path/to/cognumach
export GUILE_LOAD_PATH=$HURDCOG_ROOT/share/hurdcog/cogkernel:$GUILE_LOAD_PATH
```

### HurdCog Kernel Configuration

Edit `/etc/hurdcog/cogkernel.conf`:

```ini
[CogKernel]
atomspace_backend = persistent
atomspace_path = /var/lib/hurdcog/atomspace
enable_learning = true
enable_reasoning = true

[Integration]
occ_bridge_port = /dev/hurdcog/cogkernel
max_connections = 100
timeout_ms = 5000

[Performance]
cache_size_mb = 512
worker_threads = 4
```

---

## Testing

### Integration Tests

```bash
# Run full integration test suite
cd occ
./test-integration.sh

# Run specific tests
python3 -m pytest src/atomspace-hurdcog-bridge/tests/
```

### Cognitive Synergy Tests

```bash
# Test cognitive synergy across all components
./synergy_improved.sh
```

---

## Troubleshooting

### Bridge Connection Issues

If the bridge fails to connect to HurdCog kernel, verify that the HurdCog daemon is running by checking `systemctl status hurdcog`. Confirm the kernel port exists at `/dev/hurdcog/cogkernel`, check permissions with `ls -l /dev/hurdcog/`, and review logs at `/var/log/hurdcog/cogkernel.log`.

### Performance Issues

For slow inference or high latency, increase the cache size in `cogkernel.conf`, enable parallel processing, check CPU and memory usage, and review the dashboard for bottlenecks.

### Memory Leaks

If you observe growing memory usage, enable memory profiling in the dashboard, check for circular references in AtomSpace, review the evolution log for problematic changes, and restart the HurdCog daemon if necessary.

---

## API Reference

### AtomSpace-HurdCog Bridge API

**C++ Interface**:

```cpp
#include "bridge.h"

using namespace opencog::hurdcog;

// Initialize bridge
AtomSpaceHurdCogBridge& bridge = AtomSpaceHurdCogBridge::instance();
bridge.initialize("/dev/hurdcog/cogkernel");

// Send operation
CognitiveMessage msg;
msg.op_type = CognitiveOpType::INFERENCE;
msg.payload = "(InheritanceLink ...)";
msg.priority = 5;

CognitiveResponse resp = bridge.send_operation(msg);
if (resp.status == BridgeStatus::SUCCESS) {
    std::cout << "Result: " << resp.result << std::endl;
}
```

**Python Interface**:

See Python bindings documentation in `src/atomspace-hurdcog-bridge/python/README.md`.

---

## Best Practices

### Cognitive Operations

Always set appropriate priority levels for operations based on importance. Use asynchronous callbacks for long-running inferences. Implement proper error handling and retries. Monitor bridge statistics to detect issues early.

### Self-Modification

Never modify critical system components without extensive testing. Always implement rollback mechanisms. Use feature flags for gradual rollout. Validate changes in isolated environments first.

### Performance Optimization

Cache frequently accessed atoms to reduce kernel calls. Batch operations when possible to minimize IPC overhead. Use distributed AtomSpace for scalability. Profile regularly to identify bottlenecks.

---

## Contributing

Contributions to the integration layer are welcome. Please follow the coding standards in `CONTRIBUTING.md`, write comprehensive tests for all changes, update documentation accordingly, and submit pull requests with clear descriptions.

---

## Support

For integration issues, consult the troubleshooting section above, check GitHub issues for similar problems, join the OpenCog mailing list for community support, or review the architecture documentation for deeper understanding.

---

## License

The integration components are licensed under GPL 3.0 or later, consistent with the OpenCog project and GNU Hurd licensing.

---

## Acknowledgments

This integration builds on the pioneering work of Dr. Ben Goertzel and the OpenCog community, the GNU Hurd project, and all contributors to the cognitive computing ecosystem.
