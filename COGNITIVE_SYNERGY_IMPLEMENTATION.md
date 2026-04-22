# Cognitive Synergy Implementation Guide

## Overview

This document describes the implementation of cognitive synergy enhancements in the OpenCog Collection (OCC). These enhancements transform the repository from a collection of independent components into a truly synergistic cognitive architecture.

## Architecture

The cognitive synergy framework implements the **Agent-Arena-Relation (AAR)** architecture and the **Deep Tree Echo** membrane hierarchy:

```
┌─────────────────────────────────────────────────────────┐
│                 Cognitive Synergy Layer                  │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌──────────────────┐      ┌────────────────────────┐  │
│  │    Orchestrator  │◄────►│  Cognitive Monitor     │  │
│  │  (Coordination)  │      │  (Introspection)       │  │
│  └────────┬─────────┘      └────────────────────────┘  │
│           │                                              │
│           │ ┌──────────────────────────────────────┐   │
│           └►│     Hypergraph Bridge                │   │
│             │  (Data Flow & Integration)           │   │
│             └──────────────────────────────────────┘   │
│                                                          │
├─────────────────────────────────────────────────────────┤
│              Component Layer (Arena)                     │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐             │
│  │AtomSpace │  │ Hyperon  │  │Python ML │             │
│  │  (C++)   │  │  (Rust)  │  │ (Python) │             │
│  └──────────┘  └──────────┘  └──────────┘             │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

## Components

### 1. Cognitive Synergy Orchestrator

**Location**: `synergy/core/orchestrator.py`

**Purpose**: Central coordination system that manages inter-component communication and synergy optimization.

**Key Features**:
- Component registration and lifecycle management
- Event-based communication system
- Synergy metrics calculation
- System health monitoring

**Usage Example**:
```python
from synergy.core.orchestrator import get_orchestrator, ComponentState

# Get orchestrator instance
orchestrator = get_orchestrator()
orchestrator.start()

# Register a component
orchestrator.register_component(
    name="atomspace",
    component_type="hypergraph_db",
    capabilities=["storage", "query", "reasoning"],
    dependencies=[]
)

# Record data transfer between components
orchestrator.record_data_transfer(
    source="atomspace",
    target="hyperon",
    data_type="atom_data",
    data_size=1024
)

# Get synergy report
report = orchestrator.get_synergy_report()
print(report['metrics']['current_synergy_score'])
```

**Synergy Metrics**:
- **Synergy Score**: Composite metric measuring inter-component collaboration
- **Interaction Diversity**: Number of unique component interactions
- **Interaction Density**: Frequency of interactions per component pair
- **System Health**: Overall health based on component states

### 2. Hypergraph Bridge

**Location**: `synergy/core/hypergraph_bridge.py`

**Purpose**: Provides seamless data flow between AtomSpace (C++) and Hyperon (Rust) through unified hypergraph operations.

**Key Features**:
- Unified atom representation
- Serialization/deserialization (JSON)
- Format conversion (Scheme ↔ MeTTa)
- Data consistency validation

**Usage Example**:
```python
from synergy.core.hypergraph_bridge import get_bridge, AtomType

# Get bridge instance
bridge = get_bridge()

# Create atoms
cat = bridge.create_concept("cat", truth_value={'strength': 0.9, 'confidence': 0.8})
animal = bridge.create_concept("animal")

# Create inheritance link
inheritance = bridge.create_inheritance(cat, animal)

# Convert to AtomSpace format (Scheme)
scheme_code = bridge.convert_to_atomspace(inheritance)
print(scheme_code)
# Output: (InheritanceLink (ConceptNode "cat" (stv 0.9 0.8)) (ConceptNode "animal"))

# Convert to Hyperon format (MeTTa)
metta_code = bridge.convert_to_hyperon(inheritance)
print(metta_code)

# Serialize to JSON for storage/transfer
json_str = bridge.serialize_to_json(inheritance)

# Deserialize from JSON
restored = bridge.deserialize_from_json(json_str)
```

**Supported Atom Types**:
- ConceptNode
- PredicateNode
- Link (generic)
- EvaluationLink
- InheritanceLink
- SimilarityLink
- ExecutionLink
- VariableNode
- NumberNode

### 3. Cognitive Monitor

**Location**: `synergy/monitoring/cognitive_monitor.py`

**Purpose**: System-wide monitoring and self-awareness capabilities. Implements the Introspection Membrane from Deep Tree Echo architecture.

**Key Features**:
- Resource utilization tracking (CPU, memory, I/O)
- Performance metrics for cognitive operations
- Cognitive load assessment
- Historical data collection

**Usage Example**:
```python
from synergy.monitoring.cognitive_monitor import get_monitor, PerformanceTimer

# Get monitor instance
monitor = get_monitor()
monitor.start_monitoring(interval=5.0)

# Time a cognitive operation
with PerformanceTimer(monitor, "reasoning_task", "hyperon"):
    # Perform reasoning
    result = perform_reasoning()

# Get cognitive state
state = monitor.get_cognitive_state()
print(f"Cognitive Load: {state['cognitive_load']}")
print(f"CPU Average: {state['resources']['cpu']['average']:.2f}%")

# Get performance statistics
perf_stats = monitor.get_performance_statistics()
for component, stats in perf_stats['by_component'].items():
    print(f"{component}: {stats['avg_duration_ms']:.2f}ms avg")

# Generate report
monitor.save_report("cognitive_state_report.json")
```

**Cognitive Load Levels**:
- **Light**: CPU < 30%, Memory < 50%
- **Moderate**: CPU < 60%, Memory < 75%
- **Heavy**: CPU < 85%, Memory < 90%
- **Critical**: CPU ≥ 85% or Memory ≥ 90%

## Integration Testing

**Location**: `tests/synergy/test_integration.py`

Comprehensive integration tests that validate cognitive synergy across all components.

**Test Coverage**:
1. Component registration
2. Hypergraph bridge creation and serialization
3. Format conversion (Scheme ↔ MeTTa)
4. Data transfer recording
5. Synergy score calculation
6. Performance monitoring
7. Cognitive state assessment
8. Synergy report generation
9. End-to-end cognitive tasks

**Running Tests**:
```bash
# Run integration tests
python3 tests/synergy/test_integration.py

# Or use the enhanced synergy script
./synergy_enhanced.sh
```

## Workflow Improvements

### Enhanced Guix Build Workflow

**Location**: `.github/workflows/guix-build.yml`

**Improvements**:
1. **Multiple Mirror Sources**: Added fallback URLs for Guix installation
   - Primary: git.savannah.gnu.org
   - Mirror: github.com/guix-mirror
   - Fallback: ftp.gnu.org

2. **Improved Retry Logic**:
   - Increased connection timeout (60s)
   - More retry attempts (5)
   - Exponential backoff delays
   - Better error diagnostics

3. **Caching**: Added GitHub Actions cache for Guix installation
   - Caches `/gnu`, `/var/guix`, and `~/.config/guix`
   - Significantly speeds up subsequent runs

4. **Build Artifacts**: Uploads build logs for debugging

5. **Extended Timeout**: Increased from 60 to 90 minutes

## Synergy Testing Scripts

### Enhanced Synergy Script

**Location**: `synergy_enhanced.sh`

Comprehensive test script that validates cognitive synergy across all components.

**Features**:
- Environment validation
- Component testing (Python ML, Orchestrator, Monitor, Bridge)
- Integration testing
- Performance timing
- Detailed markdown reports
- Success rate calculation
- Synergy level assessment

**Usage**:
```bash
./synergy_enhanced.sh
```

**Output**:
- Console output with colored status indicators
- Detailed markdown report in `synergy_reports/`
- Test timing and performance metrics
- Synergy level assessment (Excellent/Good/Moderate/Poor)

## Metrics and Monitoring

### Synergy Metrics

The framework tracks several key metrics to assess cognitive synergy:

1. **Synergy Score** (0-100):
   - Based on interaction diversity and density
   - Higher scores indicate better synergy
   - Formula: `(diversity × 0.6) + (min(density, 10) × 0.4)`

2. **Component Health**:
   - Percentage of components in ready/active state
   - Tracks error states
   - Overall health score

3. **Interaction Statistics**:
   - Total events
   - Unique interactions
   - Most active component pairs

4. **Performance Metrics**:
   - Operation duration (average, min, max)
   - Success rate per component
   - Resource utilization trends

### Cognitive State Assessment

The monitor assesses overall cognitive state based on:
- Resource utilization (CPU, memory, I/O)
- Performance metrics
- Component health
- Cognitive load level

## Best Practices

### 1. Component Registration

Always register components with the orchestrator:
```python
orchestrator.register_component(
    name="my_component",
    component_type="cognitive_agent",
    capabilities=["reasoning", "learning"],
    dependencies=["atomspace"]
)
```

### 2. Data Transfer Recording

Record all inter-component data transfers:
```python
orchestrator.record_data_transfer(
    source="component_a",
    target="component_b",
    data_type="atom_data",
    data_size=len(data)
)
```

### 3. Performance Monitoring

Use PerformanceTimer for all cognitive operations:
```python
with PerformanceTimer(monitor, "operation_name", "component_name"):
    perform_operation()
```

### 4. Format Conversion

Use the hypergraph bridge for all cross-component data:
```python
# Create in unified format
atom = bridge.create_concept("concept_name")

# Convert to target format
atomspace_code = bridge.convert_to_atomspace(atom)
hyperon_code = bridge.convert_to_hyperon(atom)
```

## Future Enhancements

### Phase 2: Advanced Features

1. **Meta-Learning System**:
   - Analyze synergy patterns
   - Automatic optimization of component interactions
   - Self-tuning capabilities

2. **Cognitive Grammar Kernel**:
   - Scheme-based symbolic reasoning
   - Meta-cognitive reflection
   - Self-modification of cognitive strategies

3. **Visualization Tools**:
   - Real-time synergy dashboard
   - Component interaction graphs
   - Performance trend visualization

4. **Database Integration**:
   - Store synergy metrics in Supabase/Neon
   - Historical trend analysis
   - Cross-session comparisons

## Troubleshooting

### Common Issues

**Issue**: Import errors for synergy modules
```
Solution: Ensure you're running from the repository root and __init__.py files exist
```

**Issue**: Orchestrator not receiving events
```
Solution: Make sure orchestrator.start() is called before emitting events
```

**Issue**: Monitor shows "unknown" cognitive load
```
Solution: Wait for monitoring data to accumulate (at least 2-3 intervals)
```

**Issue**: Synergy tests fail
```
Solution: Check that all required Python packages are installed (numpy, pandas, scikit-learn, psutil)
```

## Contributing

When adding new components or features:

1. Register components with the orchestrator
2. Use the hypergraph bridge for data exchange
3. Add performance monitoring with PerformanceTimer
4. Create integration tests in `tests/synergy/`
5. Update this documentation
6. Run `./synergy_enhanced.sh` to validate

## References

- [Agent-Arena-Relation (AAR) Framework](docs/aar_framework.md)
- [Deep Tree Echo Architecture](docs/deep_tree_echo.md)
- [Hypergraph Memory Space](docs/hypergraph_memory.md)
- [Cognitive Synergy Principles](docs/cognitive_synergy.md)

---

*Last Updated: 2025-11-06*
*Version: 0.1.0*
