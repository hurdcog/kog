# OpenCog Collection - Formal Specification Suite

## Overview

This document provides an index to the comprehensive formal specification suite for the OpenCog Collection (OCC) cognitive architecture. The specifications provide a rigorous, mathematically precise model of the system suitable for formal verification, implementation validation, and architectural analysis.

## Quick Links

- **[Complete Specification Suite](formal-spec/)** - All formal specifications and architecture documentation
- **[Architecture Overview](formal-spec/architecture_overview.md)** - Comprehensive architecture with Mermaid diagrams
- **[Data Model](formal-spec/data_model.zpp)** - Z++ data layer specification
- **[System State](formal-spec/system_state.zpp)** - Z++ system state specification
- **[Operations](formal-spec/operations.zpp)** - Z++ behavioral operations specification
- **[Integrations](formal-spec/integrations.zpp)** - Z++ external integration contracts

## What's Included

### 1. Technical Architecture Documentation

**File**: [`formal-spec/architecture_overview.md`](formal-spec/architecture_overview.md) (855 lines, 23 KB)

A comprehensive technical architecture document featuring:

- **15 Mermaid Diagrams** visualizing:
  - System component architecture
  - Request processing flows
  - Cognitive synergy processing
  - Data flow patterns
  - Integration boundaries
  - Network protocol stack
  - State machine diagrams
  - Concurrency models
  - Trust boundaries
  - Deployment architectures

- **Technical Details** including:
  - Component responsibility matrix
  - Performance characteristics and latency profiles
  - Scalability limits and resource bounds
  - Security considerations
  - Error handling strategies
  - Future architecture enhancements

### 2. Formal Z++ Specifications

Four interconnected Z++ specification files providing mathematically rigorous models:

#### Data Model Specification
**File**: [`formal-spec/data_model.zpp`](formal-spec/data_model.zpp) (577 lines, 18 KB)

- **40+ Schemas** defining:
  - AtomSpace core: Atom, Node, Link, TruthValue, AttentionValue
  - CogServer network state: Connection, Session, Server
  - CogGML microkernel: CognitiveShard, ShardMessage, Coordinator
  - CogSelf framework: AGIGoal, SynergyManager, ImprovementPlan
  - Storage backends: StorageNode, statistics

- **8 Global Invariants** ensuring:
  - UUID uniqueness across instances
  - Referential integrity
  - Type hierarchy acyclicity
  - Message deliverability
  - Connection-session consistency

#### System State Specification
**File**: [`formal-spec/system_state.zpp`](formal-spec/system_state.zpp) (684 lines, 20 KB)

- **20+ Schemas** modeling:
  - 7 integrated subsystems (AtomSpace, CogServer, CogGML, CogSelf, Reasoning, Learning, Attention)
  - Component statistics and metrics
  - State transitions (Init, Operational, Degraded, Shutdown)
  - Query caches and performance optimization

- **6 Major Invariants** governing:
  - AtomSpace consistency across subsystems
  - Shard-AtomSpace coordination
  - Network session consistency
  - Synergy integration
  - Resource bounds
  - Temporal consistency

- **3 Derived Functions**:
  - System health calculation
  - Cognitive load measurement
  - Improvement trigger conditions

#### Operations Specification
**File**: [`formal-spec/operations.zpp`](formal-spec/operations.zpp) (902 lines, 28 KB)

- **30+ Operations** with pre/post conditions:
  
  **AtomSpace Operations**:
  - CreateAtom, GetAtom, UpdateAtom, DeleteAtom
  
  **Query Operations**:
  - ExecuteQuery, AcceleratedQuery
  
  **Reasoning Operations**:
  - PLNInferenceStep, URForwardChain, URBackwardChain
  
  **Shard Coordination**:
  - ActivateShard, DeactivateShard, SendShardMessage, CoordinateShards, OptimizeSynergy
  
  **Network Operations**:
  - AcceptConnection, AuthenticateConnection, ExecuteNetworkCommand, CloseConnection
  
  **AGI Coordination**:
  - AssessAGIProgress, GenerateImprovementPlan, UpdateSynergyState
  
  **Composite Operations**:
  - InferenceCycle, CognitiveSynergyCycle

#### External Integrations Specification
**File**: [`formal-spec/integrations.zpp`](formal-spec/integrations.zpp) (904 lines, 24 KB)

- **15+ Integration Contracts** for:
  
  **Storage Backends**:
  - RocksDB backend (local disk)
  - PostgreSQL backend (relational DB)
  - Cog distributed backend (network)
  
  **Network Protocols**:
  - TCP Binary Protocol (S-expressions)
  - WebSocket Protocol
  - JSON API Protocol
  - Model Context Protocol (MCP)
  
  **External Services**:
  - LLM Inference (Aphrodite Engine)
  - Knowledge Graph Integration (GnuCash)
  - Story/World Modeling (KoboldCpp)
  
  **Quality of Service**:
  - Storage performance SLAs
  - Network performance SLAs
  - External service SLAs
  
  **Error Handling**:
  - Storage error recovery
  - Network error recovery
  - Service fallback strategies

### 3. Documentation Guide

**File**: [`formal-spec/README.md`](formal-spec/README.md) (226 lines, 9 KB)

Comprehensive guide including:
- Z++ notation reference
- How to use specifications (for developers, researchers, architects)
- Verification and validation approaches
- Relationship to implementation
- Maintenance guidelines
- Version history

## Specification Statistics

| Category | Count | Details |
|----------|-------|---------|
| **Total Files** | 6 | 1 architecture doc + 4 Z++ specs + 1 README |
| **Total Lines** | 4,148 | Comprehensive coverage |
| **Total Size** | ~121 KB | Detailed specifications |
| **Mermaid Diagrams** | 15 | Complete visual architecture |
| **Z++ Schemas** | 100+ | Formal data models |
| **Operations** | 30+ | Behavioral specifications |
| **Global Invariants** | 14 | System-wide constraints |
| **Integration Contracts** | 15+ | External interface specs |

## Key Features

### Comprehensive Coverage

The specification suite covers:
- ✅ Core hypergraph database (AtomSpace)
- ✅ Network server and API layer (CogServer)
- ✅ Distributed cognitive processing (CogGML microkernel)
- ✅ AGI coordination framework (CogSelf)
- ✅ Reasoning engines (PLN, URE)
- ✅ Learning systems (MOSES, pattern mining)
- ✅ Attention allocation (ECAN)
- ✅ Storage backends (RocksDB, PostgreSQL, distributed)
- ✅ Network protocols (TCP, WebSocket, JSON, MCP)
- ✅ External services (LLMs, knowledge graphs, world modeling)

### Formal Rigor

All specifications include:
- **Pre-conditions**: What must be true before an operation
- **Post-conditions**: What is guaranteed after an operation
- **Invariants**: What must always be true
- **Type constraints**: Precise typing for all data
- **Error handling**: Formal recovery protocols

### Practical Utility

The specifications support:
- **Implementation**: Reference for developers building components
- **Verification**: Input for formal verification tools (Z3, Alloy, TLA+)
- **Testing**: Invariants can be checked at runtime
- **Documentation**: Precise API contracts
- **Analysis**: Understanding system behavior and performance

## Use Cases

### For Software Engineers

```bash
# When implementing a new AtomSpace operation:
1. Read operations.zpp to understand existing operation patterns
2. Check data_model.zpp for data structure invariants
3. Ensure pre/post conditions are satisfied in implementation
4. Verify global invariants are maintained
```

### For Researchers

```bash
# When analyzing cognitive architecture properties:
1. Review architecture_overview.md for system structure
2. Examine system_state.zpp for subsystem interactions
3. Analyze operations.zpp for behavioral properties
4. Use Z++ specs as input to formal verification tools
```

### For System Architects

```bash
# When designing new integrations:
1. Review integrations.zpp for existing patterns
2. Understand integration contracts and SLAs
3. Model new integration using same formal approach
4. Ensure consistency with existing architecture
```

## Verification Opportunities

These specifications enable:

1. **Static Analysis**: Type checking and invariant verification
2. **Model Checking**: Finite-state exploration with Alloy/TLA+
3. **Theorem Proving**: Properties verification with Coq/Isabelle
4. **Runtime Monitoring**: Invariant checking during execution
5. **Performance Analysis**: Latency and throughput modeling

## Getting Started

1. **Start with Architecture Overview**:
   ```bash
   cat docs/formal-spec/architecture_overview.md | less
   ```
   Understand the system structure with Mermaid diagrams

2. **Explore Data Models**:
   ```bash
   cat docs/formal-spec/data_model.zpp | less
   ```
   Learn the core data structures and invariants

3. **Study Operations**:
   ```bash
   cat docs/formal-spec/operations.zpp | less
   ```
   Understand system behavior and operation contracts

4. **Review Integrations**:
   ```bash
   cat docs/formal-spec/integrations.zpp | less
   ```
   See how external systems interface with OCC

## Maintenance and Updates

The specifications are **living documents** that should be updated when:

- ✓ New components are added
- ✓ Component interfaces change
- ✓ New external integrations are introduced
- ✓ System invariants are modified
- ✓ Performance characteristics change significantly

## Tools and Resources

### Recommended Tools

- **Z3 Theorem Prover**: https://github.com/Z3Prover/z3
- **Alloy Analyzer**: https://alloytools.org/
- **TLA+ Toolbox**: https://lamport.azurewebsites.net/tla/toolbox.html
- **Mermaid Live Editor**: https://mermaid.live/

### Z Notation References

- ISO/IEC 13568:2002 - Z Formal Specification Notation
- "The Way of Z" by Jonathan Jacky
- "Using Z" by Jim Woodcock and Jim Davies
- Z++ Language Reference (object-oriented extension)

## Contributing

To contribute to the formal specifications:

1. Maintain consistency across all files
2. Update related diagrams when modifying architecture
3. Ensure invariants remain satisfiable
4. Document breaking changes
5. Validate Z++ syntax
6. Update README with significant changes

## License

These specifications are part of the OpenCog Collection and follow the same license as the main repository.

## Citation

If you use these specifications in research or academic work:

```bibtex
@misc{occ-formal-spec-2025,
  title={OpenCog Collection Formal Specification Suite},
  author={OpenCog Community},
  year={2025},
  howpublished={\url{https://github.com/opencog/occ/tree/main/docs/formal-spec}},
  note={Comprehensive Z++ formal specifications and architecture documentation}
}
```

## Contact and Support

- **GitHub Discussions**: https://github.com/opencog/occ/discussions
- **OpenCog Mailing List**: https://groups.google.com/g/opencog
- **Documentation Issues**: https://github.com/opencog/occ/issues

---

**Last Updated**: 2025-11-06  
**Version**: 1.0  
**Status**: Complete initial specification suite
