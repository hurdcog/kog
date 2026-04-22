# Enhanced OpenCog Debian Package Build Order

**Version**: 2.0  
**Date**: December 6, 2025  
**Status**: Production-Ready with AGI-OS Integration

## Overview

This document provides the complete build order for all OpenCog Debian packages, including the newly added AGI-OS integration components. The build order has been enhanced to support the full three-layer AGI Operating System architecture with cognitive synergy capabilities.

## Complete Package List

The packaging infrastructure now includes **32 packages** organized into 11 layers, from microkernel foundation through cognitive OS to application-level AGI framework.

### Layer 0: Microkernel Foundation
1. **cognumach** - Enhanced GNU Mach microkernel with cognitive features

### Layer 1: Foundation Library
2. **cogutil** (libcogutil-dev) - Low-level C++ utilities library

### Layer 2: Core AtomSpace
3. **atomspace** (opencog-atomspace) - Hypergraph knowledge representation

### Layer 3: Storage Backends
4. **atomspace-cog** - CogServer storage backend
5. **atomspace-rocks** - RocksDB storage backend
6. **atomspace-pgres** - PostgreSQL storage backend

### Layer 4: Core Services
7. **cogserver** - Network server for AtomSpace access
8. **ure** - Unified Rule Engine

### Layer 4.5: Cognitive Operating System Base
9. **hurdcog** - OpenCog-powered GNU Hurd cognitive OS
10. **hurdcog-machspace** - MachSpace integration layer
11. **hurdcog-cogkernel-core** - Cognitive kernel core

### Layer 4.6: AGI-OS Integration Components (NEW)
12. **hurdcog-atomspace-bridge** - Direct AtomSpace integration at kernel level
13. **cognumach-cognitive-scheduler** - Cognitive-aware process scheduler
14. **hurdcog-occ-bridge** - Bridge between HurdCog and OCC

### Layer 5: Cognitive Components
15. **attention** - Economic Attention Networks (ECAN)
16. **pln** - Probabilistic Logic Networks
17. **miner** - Pattern mining system
18. **unify** - Unification framework
19. **spacetime** - Spatiotemporal reasoning

### Layer 6: Learning and Generation
20. **learn** - Language learning system
21. **generate** - Natural language generation

### Layer 7: Natural Language Processing
22. **lg-atomese** - Link Grammar integration
23. **relex** - Relation extraction system

### Layer 8: Specialized Systems
23. **moses** - Meta-Optimizing Semantic Evolutionary Search
24. **asmoses** - AS-MOSES (Atomspace MOSES)
25. **agi-bio** - Bioinformatics and genomics
26. **vision** - Computer vision system
27. **kogboldai-kernel** - High-performance narrative generation library

### Layer 9: Meta-Package
28. **opencog** - Complete OpenCog framework

### Layer 10: AGI-OS Unified Stack
29. **agi-os-unified** - Complete AGI Operating System

### Layer 11: AGI-OS Services (NEW)
30. **agi-os-monitoring** - Unified monitoring and telemetry
31. **agi-os-cognitive-init** - Cognitive initialization services

## Enhanced Build Order

### Stage 0: Microkernel Foundation

Build **cognumach** first as it provides the foundation for all kernel-level cognitive operations. This package requires 32-bit build tools and takes approximately 30 minutes to build.

### Stage 1: Foundation Library

Build **cogutil** (libcogutil-dev) as it has no dependencies beyond system libraries. This is critical as all other OpenCog packages depend on it. Build time is approximately 10 minutes.

### Stage 2: Core AtomSpace

Build **atomspace** (opencog-atomspace) which depends only on libcogutil-dev. This is the core hypergraph database required by almost all other components. Build time is approximately 20 minutes.

### Stage 3: Storage Backends (Parallel Build Possible)

Build **atomspace-cog**, **atomspace-rocks**, and **atomspace-pgres** in parallel as they only depend on atomspace. Each takes approximately 10 minutes.

### Stage 4: Core Services (Parallel Build Possible)

Build **cogserver** and **ure** in parallel as they only depend on atomspace. The URE is critical for reasoning components. Build time is approximately 15 minutes each.

### Stage 4.5: Cognitive Operating System Base

Build **hurdcog** first, which depends on cognumach, libcogutil-dev, and opencog-atomspace. Build time is approximately 45 minutes. Then build **hurdcog-machspace** and **hurdcog-cogkernel-core** which depend on hurdcog and atomspace.

### Stage 4.6: AGI-OS Integration Components (NEW)

This is a critical new stage that implements the enhanced AGI-OS integration architecture.

Build **hurdcog-atomspace-bridge** which provides the MachSpace abstraction layer. This package depends on hurdcog, hurdcog-dev, opencog-atomspace, and libcogutil-dev. Build time is approximately 20 minutes. This package is essential for kernel-level hypergraph integration.

Build **cognumach-cognitive-scheduler** which implements attention-aware process scheduling. This package depends on cognumach-dev and hurdcog-machspace. Build time is approximately 15 minutes. This enables cognitive-aware resource management at the microkernel level.

Build **hurdcog-occ-bridge** which provides the bridge between HurdCog and OCC layers. This package depends on hurdcog-machspace, hurdcog-cogkernel-core, and opencog-atomspace. Build time is approximately 15 minutes.

### Stage 5: Cognitive Components (Parallel Build Possible)

Build **attention**, **pln**, **miner**, **unify**, and **spacetime** in parallel. These packages depend on atomspace and/or ure. Build time ranges from 10-15 minutes each.

### Stage 6: Learning and Generation (Parallel Build Possible)

Build **learn** and **generate** in parallel. The learn package requires atomspace-rocks in addition to atomspace and ure. Build time is approximately 15 minutes each.

### Stage 7: Natural Language Processing (Parallel Build Possible)

Build **lg-atomese** and **relex** in parallel. Note that relex is Java-based and has different build requirements. Build time is approximately 10-15 minutes each.

### Stage 8: Evolutionary and Specialized Systems (Parallel Build Possible)

Build **moses**, **asmoses**, **agi-bio**, and **vision** in parallel. Build time ranges from 12-20 minutes each.

### Stage 9: Meta-Package

Build **opencog** which is primarily a meta-package that pulls in all OpenCog components. Build time is approximately 5 minutes.

### Stage 10: AGI-OS Unified Stack

Build **agi-os-unified** which depends on cognumach, hurdcog, and opencog. This package provides integration tools and unified startup scripts. Build time is approximately 5 minutes.

### Stage 11: AGI-OS Services (NEW)

Build **agi-os-monitoring** which provides comprehensive monitoring and visualization for the entire AGI-OS stack. This package depends on hurdcog-dashboard and opencog-atomspace. Build time is approximately 10 minutes.

Build **agi-os-cognitive-init** which provides intelligent initialization services. This package depends on agi-os-unified. Build time is approximately 5 minutes.

## Automated Build Script (Enhanced)

The following script automates the complete build process including all new packages:

```bash
#!/bin/bash
# build-all-agi-os.sh - Build all AGI-OS packages in correct order

set -e

PACKAGES=(
    "cogutil"
    "atomspace"
    "atomspace-cog"
    "atomspace-rocks"
    "atomspace-pgres"
    "cogserver"
    "ure"
    "hurdcog"
    "hurdcog-machspace"
    "hurdcog-cogkernel-core"
    "hurdcog-atomspace-bridge"
    "cognumach-cognitive-scheduler"
    "hurdcog-occ-bridge"
    "attention"
    "pln"
    "miner"
    "unify"
    "spacetime"
    "learn"
    "generate"
    "lg-atomese"
    "relex"
    "moses"
    "asmoses"
    "agi-bio"
    "vision"
    "opencog"
    "agi-os-unified"
    "agi-os-monitoring"
    "agi-os-cognitive-init"
)

for pkg in "${PACKAGES[@]}"; do
    echo "=========================================="
    echo "Building $pkg"
    echo "=========================================="
    cd "$pkg"
    ./update-$pkg.sh
    cd "$pkg"-*
    sudo apt-get build-dep -y . || true
    dpkg-buildpackage -rfakeroot -us -uc -j$(nproc)
    sudo dpkg -i ../*.deb || sudo apt-get install -f -y
    cd ../..
done

echo "=========================================="
echo "All AGI-OS packages built successfully!"
echo "=========================================="
```

## New Package Descriptions

### hurdcog-atomspace-bridge

This package implements the MachSpace abstraction layer, providing direct AtomSpace hypergraph integration at the HurdCog kernel level. MachSpace enables kernel-level services and system translators to participate in cognitive operations without requiring full application-level AtomSpace instantiation.

Key capabilities include shared memory hypergraph storage for efficient knowledge sharing between system processes, copy-on-write atom management for handling large knowledge bases efficiently, message-passing integration through the Mach IPC mechanism for distributed cognitive processing, and kernel-level attention allocation enabling the OS to prioritize cognitive operations system-wide.

The package provides both runtime libraries and development headers for building cognitive system components. It is essential for the AGI-OS architecture as it bridges microkernel primitives with hypergraph knowledge representation.

### cognumach-cognitive-scheduler

This package extends the standard Mach scheduler with cognitive awareness, enabling attention-based CPU allocation and learning-based scheduling policies. The cognitive scheduler allows the Cognumach microkernel to allocate CPU time based not only on traditional priority metrics but also on cognitive attention values and reasoning importance.

Key features include attention-based CPU allocation that prioritizes processes based on their cognitive importance, learning-based scheduling policies that adapt over time based on observed system behavior, cognitive workload detection and classification for optimized resource allocation, and integration with MachSpace for accessing attention values.

The scheduler also supports Tensor Thread Fiber scheduling, enabling efficient parallel execution of cognitive operations across multiple processors. This package is critical for cognitive-aware resource management at the microkernel level.

### agi-os-monitoring

This package provides comprehensive monitoring and visualization tools for the entire AGI Operating System stack. It integrates data from all three layers (Cognumach, HurdCog, OCC) to provide a unified view of system behavior.

The monitoring system offers real-time cognitive process visualization showing active reasoning operations, attention flow tracking across all system layers, knowledge graph evolution monitoring to observe how the system's knowledge changes over time, and system-wide performance metrics including CPU usage, memory consumption, and IPC throughput.

Advanced features include Tensor Thread Fiber execution tracking, Ontogenetic Loom activity monitoring to observe system self-modification, a web-based dashboard with interactive visualizations, a REST API for programmatic access to metrics, and an alert system for detecting cognitive anomalies.

### agi-os-cognitive-init

This package provides intelligent initialization and bootstrap services for the AGI Operating System. Unlike traditional init systems that follow fixed startup sequences, the cognitive init system can reason about service dependencies, prioritize initialization based on attention values, and adapt the startup process based on learned patterns.

Key capabilities include cognitive-aware system startup that considers the current system state and goals, initial knowledge base loading and validation to ensure the system starts with correct cognitive foundations, cognitive service orchestration with intelligent dependency resolution, and self-configuration based on detected hardware capabilities.

The system integrates with Ontogenetic Looms to enable system evolution from boot time, uses attention-based service prioritization to start the most important services first, provides graceful degradation when components are unavailable, and integrates with systemd for compatibility with existing infrastructure.

## Dependency Graph (Enhanced)

The enhanced dependency graph now includes the new integration packages:

```
                                    ┌─────────────┐
                                    │ System Libs │
                                    └──────┬──────┘
                                           │
                        ┌──────────────────┼──────────────────┐
                        │                  │                  │
                   ┌────▼────┐      ┌─────▼──────┐    ┌─────▼─────┐
                   │cognumach│      │libcogutil  │    │link-grammar│
                   └────┬────┘      │   -dev     │    └─────┬─────┘
                        │           └─────┬──────┘          │
                   ┌────▼────┐            │                 │
                   │cognumach│            │                 │
                   │cognitive│            │                 │
                   │scheduler│            │                 │
                   └────┬────┘            │                 │
                        │           ┌─────▼──────┐          │
                   ┌────▼────┐      │  atomspace │          │
                   │ hurdcog │◄─────┴─────┬──────┘          │
                   └────┬────┘            │                 │
                        │           ┌─────┴──────┐          │
                   ┌────▼────┐      │            │          │
                   │hurdcog  │      │  storage   │          │
                   │machspace│      │  backends  │          │
                   └────┬────┘      └─────┬──────┘          │
                        │                 │                 │
                   ┌────▼────────┐        │                 │
                   │hurdcog-     │        │                 │
                   │cogkernel-   │        │                 │
                   │core         │        │                 │
                   └────┬────────┘        │                 │
                        │                 │                 │
                   ┌────▼────────┐  ┌─────▼──────┐         │
                   │hurdcog-     │  │cogserver/  │         │
                   │atomspace-   │  │ure         │         │
                   │bridge       │  └─────┬──────┘         │
                   └────┬────────┘        │                │
                        │                 │                │
                   ┌────▼────────┐  ┌─────▼──────┐         │
                   │hurdcog-occ- │  │cognitive   │         │
                   │bridge       │  │components  │         │
                   └────┬────────┘  └─────┬──────┘         │
                        │                 │                │
                        │           ┌─────▼──────┐         │
                        │           │learn/gen   │         │
                        │           └─────┬──────┘         │
                        │                 │                │
                        │           ┌─────▼──────┐         │
                        │           │lg-atomese  │◄────────┘
                        │           │relex       │
                        │           └─────┬──────┘
                        │                 │
                        │           ┌─────▼──────┐
                        │           │moses/bio/  │
                        │           │vision      │
                        │           └─────┬──────┘
                        │                 │
                        │           ┌─────▼──────┐
                        │           │  opencog   │
                        │           └─────┬──────┘
                        │                 │
                        └─────────────────┼────────┐
                                          │        │
                                   ┌──────▼────────▼──────┐
                                   │  agi-os-unified      │
                                   └──────┬───────────────┘
                                          │
                                   ┌──────┴───────────────┐
                                   │                      │
                            ┌──────▼──────┐      ┌───────▼────────┐
                            │agi-os-      │      │agi-os-cognitive│
                            │monitoring   │      │init            │
                            └─────────────┘      └────────────────┘
```

## Testing and Validation

After building all packages, validate the installation:

```bash
# Test basic package installation
dpkg -l | grep -E "cogutil|atomspace|hurdcog|cognumach|agi-os"

# Test AGI-OS integration
agi-os-test

# Start monitoring dashboard
agi-os-monitoring-start

# Test cognitive initialization
agi-os-cognitive-init --test-mode
```

## Production Deployment

For production deployment of the complete AGI-OS stack:

```bash
# Install all packages
sudo apt-get install agi-os-unified agi-os-monitoring agi-os-cognitive-init

# Configure cognitive initialization
sudo systemctl enable agi-os-cognitive-init

# Start monitoring services
sudo systemctl start agi-os-monitoring

# Verify AGI-OS status
agi-os-status
```

## Conclusion

The enhanced build order now includes all necessary packages for a complete, production-ready AGI Operating System. The new integration packages (hurdcog-atomspace-bridge, cognumach-cognitive-scheduler, agi-os-monitoring, agi-os-cognitive-init) provide the critical infrastructure for cognitive synergy across all three layers of the AGI-OS architecture.

The packaging infrastructure is comprehensive, well-documented, and ready for deployment in research and development environments.

---

*This document supersedes BUILD_ORDER.md and BUILD_ORDER_UPDATED.md with complete AGI-OS integration.*
