# OpenCog Collection - Formal Specification Documentation

## Overview

This directory contains comprehensive technical architecture documentation and formal Z++ specifications for the OpenCog Collection (OCC) cognitive architecture system.

## Documentation Structure

### 1. Architecture Overview (`architecture_overview.md`)

Comprehensive technical architecture documentation with Mermaid diagrams covering:

- **System Architecture**: High-level component overview and integration patterns
- **Component Interactions**: Sequence diagrams showing request processing and cognitive synergy
- **Data Flow**: AtomSpace data flow and storage backend architecture
- **Integration Boundaries**: External system interfaces and dependency flows
- **Network Protocols**: CogServer protocol stack and communication layers
- **State Management**: State diagrams for AtomSpace, CogServer, and CogGML
- **Concurrency Model**: Thread safety and shard parallelism
- **Performance Characteristics**: Latency profiles and scalability limits
- **Security Considerations**: Authentication, authorization, and attack surface
- **Deployment Architecture**: Single-node and distributed deployment patterns

### 2. Data Model Specification (`data_model.zpp`)

Z++ formal specification of the data layer including:

- **Basic Types**: UUID, timestamps, atomic types, truth values, attention values
- **AtomSpace Core Model**: 
  - Atom, Node, and Link schemas
  - Truth values and attention values
  - Type system and key-value stores
  - AtomSpace and AtomTable state
- **CogServer Network State**: Connections, sessions, and server state
- **CogGML Microkernel**: Cognitive shards, messages, and coordination
- **CogSelf Framework**: AGI goals, synergy management, improvement plans
- **Storage Backends**: StorageNode abstraction and statistics
- **Global Invariants**: UUID uniqueness, referential integrity, type hierarchy

### 3. System State Specification (`system_state.zpp`)

Z++ formal specification of system-wide state including:

- **Component States**:
  - AtomSpace subsystem with query cache and statistics
  - CogServer subsystem with request/response queues
  - CogGML subsystem with microkernel and shard coordination
  - CogSelf subsystem with AGI assessment and autonomy
  - Reasoning subsystem (PLN + URE)
  - Learning subsystem (MOSES + pattern mining)
  - Attention subsystem (ECAN)
- **Integrated System State**: Top-level OpenCogSystemState schema
- **State Transitions**: Init, Operational, Degraded, and Shutdown states
- **Subsystem Interactions**: Invariants governing component coordination
- **Derived Properties**: System health, cognitive load, and improvement triggers

### 4. Operations Specification (`operations.zpp`)

Z++ formal specification of system operations including:

- **AtomSpace CRUD**: CreateAtom, GetAtom, UpdateAtom, DeleteAtom
- **Query Operations**: ExecuteQuery, AcceleratedQuery with pattern matching
- **Inference Operations**: PLNInferenceStep, URForwardChain, URBackwardChain
- **Shard Coordination**: ActivateShard, DeactivateShard, SendShardMessage, CoordinateShards
- **Network Operations**: AcceptConnection, AuthenticateConnection, ExecuteNetworkCommand
- **AGI Coordination**: AssessAGIProgress, GenerateImprovementPlan, UpdateSynergyState
- **Composite Operations**: InferenceCycle, CognitiveSynergyCycle

### 5. External Integrations Specification (`integrations.zpp`)

Z++ formal specification of external system contracts including:

- **Storage Backend Contracts**:
  - StorageBackendContract interface
  - RocksDB backend specification
  - PostgreSQL backend specification
  - Distributed Cog storage specification
- **Network Protocols**:
  - TCP Binary Protocol
  - WebSocket Protocol
  - JSON API Protocol
  - Model Context Protocol (MCP)
- **External Service APIs**:
  - LLM Inference (Aphrodite Engine)
  - Knowledge Graph Integration (GnuCash)
  - Story/World Modeling (KoboldCpp)
- **Inter-Component Communication**: Communication protocols between subsystems
- **Error Handling**: Recovery protocols for storage, network, and service errors
- **Performance SLAs**: Quality of service contracts for all integrations

## Z++ Notation Guide

### Basic Symbols

- `ℕ` - Natural numbers (0, 1, 2, ...)
- `ℤ` - Integers (..., -1, 0, 1, ...)
- `ℝ` - Real numbers
- `𝔹` - Booleans {true, false}
- `seq T` - Sequence of type T
- `ℙ T` - Power set (set of all subsets) of T
- `T ⇸ U` - Partial function from T to U
- `T → U` - Total function from T to U

### Schema Notation

```z
schema SchemaName
  variable1: Type1
  variable2: Type2
where
  -- Invariants/constraints
  constraint1
  constraint2
end
```

### Operation Notation

- `ΔSchema` - Schema with before/after state (modification)
- `ΞSchema` - Schema with unchanged state (query)
- `variable?` - Input parameter
- `variable!` - Output parameter
- `variable'` - After state of variable

### Set Operations

- `∈` - Element of
- `∉` - Not element of
- `⊆` - Subset of
- `∪` - Set union
- `∩` - Set intersection
- `∖` - Set difference
- `#S` - Cardinality (size) of set S
- `dom f` - Domain of function f
- `ran f` - Range of function f

### Logic Operators

- `∧` - Logical AND
- `∨` - Logical OR
- `⇒` - Implies
- `¬` - Logical NOT
- `∀` - For all (universal quantifier)
- `∃` - There exists (existential quantifier)

## Using These Specifications

### For Developers

1. **Understanding System Behavior**: Read the operations specifications to understand pre-conditions, post-conditions, and invariants for each operation
2. **Implementing New Features**: Use the data model as a reference for maintaining consistency with existing structures
3. **Debugging**: Check invariants in system_state.zpp to identify violated constraints
4. **Integration**: Refer to integrations.zpp for contracts that must be satisfied by external components

### For Researchers

1. **Formal Verification**: Use Z++ specifications as input to formal verification tools
2. **Property Checking**: Validate system invariants against execution traces
3. **Architecture Analysis**: Use Mermaid diagrams to understand component interactions
4. **Performance Modeling**: Leverage the specified latency and scalability characteristics

### For System Architects

1. **Design Documentation**: Comprehensive reference for system architecture decisions
2. **API Design**: Use operation specifications as basis for API contracts
3. **Capacity Planning**: Reference performance characteristics and scalability limits
4. **Security Analysis**: Review trust boundaries and attack surface documentation

## Verification and Validation

These specifications can be used with formal methods tools:

- **Z3 Theorem Prover**: Verify invariants and check satisfiability
- **Alloy Analyzer**: Model checking for finite scopes
- **TLA+ Model Checker**: Temporal logic verification
- **Coq/Isabelle**: Interactive theorem proving

## Relationship to Implementation

The specifications in this directory provide a **formal model** of the OCC system. The actual C++/Python/Scheme implementation should:

1. **Satisfy all invariants** defined in the specifications
2. **Implement operations** that respect pre/post-conditions
3. **Maintain data structures** consistent with the data model
4. **Honor integration contracts** when interfacing with external systems

## Maintenance

These specifications should be updated when:

- New components are added to the system
- Existing component interfaces change
- New external integrations are introduced
- System invariants are modified
- Performance characteristics significantly change

## Contributing

When updating specifications:

1. Maintain consistency across all files (data_model, system_state, operations, integrations)
2. Update related Mermaid diagrams in architecture_overview.md
3. Ensure all invariants remain satisfiable
4. Document any breaking changes
5. Validate Z++ syntax using a Z checker tool if available

## References

- **Z Notation**: Formal specification language (ISO/IEC 13568:2002)
- **Z++**: Object-oriented extension of Z notation
- **OpenCog AtomSpace**: https://github.com/opencog/atomspace
- **OpenCog CogServer**: https://github.com/opencog/cogserver
- **Mermaid Diagrams**: https://mermaid-js.github.io/

## Version History

- **v1.0** (2025-11-06): Initial comprehensive specification
  - Complete architecture documentation with Mermaid diagrams
  - Full Z++ data model specification
  - System state formalization
  - Operations specification covering CRUD, inference, and coordination
  - External integrations with storage, network, and service contracts

---

**Note**: This is a living document. As the OCC system evolves, these specifications will be updated to reflect the current architecture and implementation.
