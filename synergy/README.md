# Cognitive Synergy Modules

This directory contains the core infrastructure for enabling **cognitive synergy** in the OpenCog Collection. These modules implement the principles of the **Deep Tree Echo architecture** and the **Agent-Arena-Relation (AAR) framework** to create a unified, self-aware cognitive system.

## Architecture Overview

The synergy modules are organized into four main categories:

```
synergy/
├── core/              # Core orchestration and coordination
├── bridges/           # Cross-component data flow bridges
├── monitors/          # Introspection and monitoring systems
└── tests/             # Module-specific tests
```

## Core Modules

### 1. Synergy Orchestrator (`core/synergy_orchestrator.py`)

**Purpose**: Central coordination system that manages inter-component communication and synergy optimization.

**Key Features**:
- Component registration and lifecycle management
- Event bus for publish-subscribe communication
- Synergy score calculation based on interaction density and load balancing
- Real-time state tracking and reporting

**Usage**:
```python
from synergy.core.synergy_orchestrator import SynergyOrchestrator

orchestrator = SynergyOrchestrator()

# Register components
orchestrator.register_component('atomspace', {'type': 'knowledge_base'})
orchestrator.register_component('hyperon', {'type': 'reasoning_engine'})

# Update component states
orchestrator.update_component_state('atomspace', 'active', 0.6, {'atoms': 1000})

# Record interactions
orchestrator.record_interaction('atomspace', 'hyperon', {'atoms_transferred': 100})

# Calculate synergy
synergy_score = orchestrator.calculate_synergy_score()
print(f"Synergy Score: {synergy_score:.2f}")
```

**AAR Framework Mapping**:
- **Agent**: Individual cognitive components (AtomSpace, Hyperon, CogServer)
- **Arena**: Shared event bus and interaction graph
- **Relation**: Dynamic coordination and synergy optimization

### 2. Hypergraph Bridge (`bridges/hypergraph_bridge.py`)

**Purpose**: Enables seamless data flow between AtomSpace (C++) and Hyperon (Rust) through a unified hypergraph interface.

**Key Features**:
- Unified Atom representation compatible with both AtomSpace and Hyperon
- Serialization to JSON, Scheme, and MeTTa formats
- Cross-language data transfer with validation
- Hypergraph statistics and analysis

**Usage**:
```python
from synergy.bridges.hypergraph_bridge import HypergraphBridge

bridge = HypergraphBridge()

# Create atoms
cat = bridge.create_concept('cat', {'strength': 0.9, 'confidence': 0.8})
animal = bridge.create_concept('animal')

# Create relationships
inheritance = bridge.create_inheritance(cat, animal)

# Export to different formats
bridge.export_to_scheme([cat, animal, inheritance], 'atoms.scm')
bridge.export_to_metta([cat, animal, inheritance], 'atoms.metta')
```

**Deep Tree Echo Mapping**:
- **Memory Membrane**: Unified hypergraph storage
- **Communication Protocols**: Cross-language serialization

### 3. Cognitive Monitor (`monitors/cognitive_monitor.py`)

**Purpose**: System-wide monitoring and introspection for observing cognitive state and performance.

**Key Features**:
- Real-time resource utilization tracking (CPU, memory, disk, network)
- Cognitive task performance metrics
- Component health assessment
- Alert generation for critical conditions
- Comprehensive reporting

**Usage**:
```python
from synergy.monitors.cognitive_monitor import CognitiveMonitor

monitor = CognitiveMonitor()

# Capture resource snapshot
snapshot = monitor.capture_resource_snapshot()

# Record cognitive task
monitor.record_cognitive_task(
    component='atomspace',
    task_type='query',
    duration_ms=50,
    success=True,
    quality_score=0.9
)

# Get system health
health = monitor.get_system_health()
print(f"System Status: {health['status']}")

# Export report
monitor.export_report('cognitive_report.json')
```

**Deep Tree Echo Mapping**:
- **Introspection Membrane**: Self-awareness and monitoring
- **Performance Optimization**: Resource tracking and health assessment

## Integration Tests

The `tests/synergy/` directory contains comprehensive integration tests that validate:

1. **Component Registration**: Components can be registered and tracked
2. **State Management**: Component states can be updated and monitored
3. **Interaction Recording**: Inter-component communication is captured
4. **Synergy Calculation**: Synergy scores are computed correctly
5. **Hypergraph Operations**: Atoms can be created, serialized, and transferred
6. **Monitoring**: Resource and cognitive metrics are tracked
7. **End-to-End Workflows**: Complete synergy workflows function correctly

**Running Tests**:
```bash
cd /path/to/occ
python3 tests/synergy/test_synergy_integration.py
```

## Enhanced Synergy Script

The `synergy_improved.sh` script provides a comprehensive orchestration of all synergy checks:

```bash
./synergy_improved.sh
```

**What it does**:
1. Validates environment and tools
2. Runs Guix syntax tests
3. Executes synergy integration tests
4. Verifies component availability
5. Tests hypergraph bridge
6. Tests cognitive monitor
7. Tests synergy orchestrator
8. Generates comprehensive report

## Cognitive Synergy Principles

These modules implement key principles for achieving cognitive synergy:

### 1. Unified Knowledge Representation
The **Hypergraph Bridge** provides a common language for all components to share information through the AtomSpace hypergraph structure.

### 2. Dynamic Coordination
The **Synergy Orchestrator** actively manages component interactions, optimizing for load balancing and maximizing interaction density.

### 3. Self-Awareness
The **Cognitive Monitor** enables the system to observe its own performance, resource utilization, and health status.

### 4. Emergent Intelligence
By facilitating seamless communication and coordination, these modules enable emergent behaviors that arise from component interactions.

### 5. Continuous Optimization
The system continuously calculates synergy scores and adjusts coordination strategies to maximize cognitive performance.

## Synergy Score Calculation

The synergy score is a composite metric (0.0 to 1.0) based on:

1. **Interaction Density** (40% weight): Ratio of actual interactions to possible interactions
2. **Load Balance** (30% weight): Evenness of load distribution across components
3. **Activity Score** (30% weight): Proportion of recently active components

**Interpretation**:
- **0.9 - 1.0**: Excellent synergy, highly coordinated system
- **0.7 - 0.9**: Good synergy, effective collaboration
- **0.5 - 0.7**: Moderate synergy, room for improvement
- **0.0 - 0.5**: Poor synergy, components operating in isolation

## Future Enhancements

Planned enhancements to the synergy modules:

1. **Meta-Learning System**: Enable the system to learn optimal coordination strategies
2. **Cognitive Grammar Kernel**: Scheme-based symbolic reasoning over component interactions
3. **Distributed Orchestration**: Scale orchestration across multiple nodes
4. **Advanced Metrics**: Add information-theoretic measures of synergy
5. **Visualization Dashboard**: Real-time visualization of cognitive state and interactions

## Contributing

When adding new cognitive components to the OCC:

1. Register your component with the Synergy Orchestrator
2. Use the Hypergraph Bridge for knowledge representation
3. Report metrics to the Cognitive Monitor
4. Add integration tests to validate synergy
5. Update documentation with component-specific details

## References

- [Cognitive Synergy in OpenCog](../docs/cognitive-synergy.md)
- [Architecture Overview](../docs/architecture.md)
- [Deep Tree Echo Architecture](../COGNITIVE_SYNERGY_ANALYSIS.md)
- [Agent-Arena-Relation Framework](../COGNITIVE_SYNERGY_ANALYSIS.md)

## License

These modules are part of the OpenCog Collection and are licensed under GPL-3.0+.
