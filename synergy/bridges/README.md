# Cognitive Synergy Bridges

This directory contains bridge implementations that connect different OpenCog components to create emergent cognitive capabilities through their interaction.

## Overview

Bridges are integration modules that enable **cognitive synergy** - the emergence of intelligence from the interaction of diverse AI components. Each bridge implements specific patterns of interaction between components, creating capabilities that neither component possesses alone.

## Implemented Bridges

### Attention-MOSES Bridge ✅

**Files:**
- `attention_moses_bridge.py` - Python implementation
- `attention-moses-bridge.scm` - Scheme interface

**Purpose:** Connects the Attention Allocation (ECAN) system with MOSES evolutionary learning to enable **focused learning** - the system can prioritize learning based on what it deems important.

**Key Features:**
- Extract high-STI (Short-Term Importance) atoms as learning targets
- Convert attention signals to MOSES fitness bonuses
- Prioritize learning tasks by importance
- Feed learning outcomes back to attention system

**Emergent Capability:** `focused_learning`

**Documentation:** [docs/attention-guided-learning.md](../../docs/attention-guided-learning.md)

**Status:** Phase 1 Complete (Autogenesis Feature #2)

### Hypergraph Bridge

**Files:**
- `hypergraph_bridge.py`
- `hypergraph_bridge_enhanced.py`

**Purpose:** Provides enhanced hypergraph operations and transformations for AtomSpace.

**Status:** Existing infrastructure

### AGI-OS Bridge

**Files:**
- `agi_os_bridge.py`

**Purpose:** Integration with AGI-OS kernel layers (Cognumach, HurdCog).

**Status:** Existing infrastructure

### Scheme-Python FFI

**Files:**
- `scheme_python_ffi.py`

**Purpose:** Foreign Function Interface between Scheme and Python components.

**Status:** Existing infrastructure

## Bridge Architecture Pattern

All bridges follow a common architectural pattern:

```
┌──────────────────┐
│   Component A    │
│  (e.g., ECAN)    │
└────────┬─────────┘
         │
         │ Extract signals/data
         ▼
┌─────────────────────────────────┐
│          Bridge Layer           │
│                                 │
│  1. Signal Extraction           │
│  2. Transformation/Mapping      │
│  3. Task Management             │
│  4. Feedback Loop               │
└─────────┬───────────────────────┘
          │
          │ Transformed data/tasks
          ▼
┌──────────────────┐
│   Component B    │
│  (e.g., MOSES)   │
└────────┬─────────┘
         │
         │ Results/outcomes
         ▼
┌─────────────────────────────────┐
│      Feedback to Component A    │
└─────────────────────────────────┘
```

## Creating a New Bridge

To implement a new cognitive synergy bridge:

### 1. Design Phase

- Identify two or more components to integrate
- Define the emergent capability that will arise
- Design signal extraction mechanisms
- Plan transformation/mapping logic
- Design feedback loops

### 2. Implementation

Create these files:

```
synergy/bridges/
├── your_bridge.py           # Python implementation
├── your-bridge.scm          # Scheme interface (optional)
└── README.md                # Update this file
```

Required components:

```python
# your_bridge.py

class YourBridge:
    """Bridge between Component A and Component B"""
    
    def __init__(self, config):
        """Initialize bridge with configuration"""
        pass
    
    def extract_signals(self, source_data):
        """Extract signals from Component A"""
        pass
    
    def transform_signals(self, signals):
        """Transform signals for Component B"""
        pass
    
    def create_tasks(self, transformed_data):
        """Create tasks for Component B"""
        pass
    
    def feedback_to_source(self, results):
        """Feed results back to Component A"""
        pass
    
    def get_stats(self):
        """Return bridge statistics"""
        pass
```

### 3. Testing

Create comprehensive tests:

```
tests/synergy/
└── test_your_bridge.py
```

Test coverage should include:
- Signal extraction
- Transformation logic
- Task management
- Feedback loops
- Integration workflow
- Statistics tracking

### 4. Documentation

Create documentation:

```
docs/
└── your-bridge-integration.md
```

Include:
- Overview and purpose
- Architecture diagram
- Usage examples (Python and Scheme)
- Configuration parameters
- Integration patterns
- Performance considerations

### 5. Update Autogenesis

If this bridge implements an autogenesis feature, update:

```
autogenesis_report.md
autogenesis_roadmap.json
```

## Bridge Design Principles

### 1. Cognitive Synergy

Bridges should create emergent capabilities that neither component has alone:

✅ **Good:** Attention guides learning → `focused_learning` capability  
❌ **Bad:** Simple data pass-through with no emergent behavior

### 2. Bidirectional Flow

Information should flow in both directions:

✅ **Good:** Attention → Learning → Feedback to Attention  
❌ **Bad:** One-way data flow with no feedback

### 3. Minimal Coupling

Bridges should be loosely coupled to components:

✅ **Good:** Generic interfaces that work with any attention/learning system  
❌ **Bad:** Tight coupling to specific implementations

### 4. Observable Behavior

Bridge behavior should be measurable:

✅ **Good:** Statistics, metrics, testable outcomes  
❌ **Bad:** Black box with no observability

### 5. Configurable

Bridges should be configurable for different scenarios:

✅ **Good:** Thresholds, weights, scaling factors as parameters  
❌ **Bad:** Hard-coded magic numbers

## Planned Bridges

From the [Autogenesis Roadmap](../../autogenesis_roadmap.json):

1. **PLN-MOSES Bridge** (Feature #1: Cross-Modal Cognitive Fusion)
   - Integrate reasoning and learning into unified cognitive loops
   - Emergent capability: `unified_cognitive_processing`

2. **Pattern-PLN Bridge** (Feature #6: Inference-Driven Pattern Discovery)
   - Use PLN to guide pattern mining with semantic constraints
   - Emergent capability: `semantic_pattern_mining`

3. **CogGML-AtomSpace Bridge** (Feature #5: Distributed Cognitive Shard Network)
   - Network of specialized cognitive shards with shared memory
   - Emergent capability: `distributed_cognition`

## Performance Guidelines

### Signal Extraction
- Should be O(n) or better where n = source data size
- Use indices for large datasets
- Consider lazy evaluation

### Transformation
- Should be O(1) per signal
- Batch processing for efficiency
- Cache frequently used transformations

### Task Management
- Use priority queues for efficient sorting
- Limit active tasks to prevent resource exhaustion
- Clean up completed tasks periodically

### Feedback
- Asynchronous when possible
- Batch updates for efficiency
- Rate limiting to prevent feedback storms

## Testing Requirements

All bridges must have:

- ✅ Unit tests for each component method
- ✅ Integration tests for complete workflows
- ✅ Performance benchmarks
- ✅ Edge case handling tests
- ✅ Statistics validation tests

Target: >90% code coverage

## Contributing

When contributing a new bridge:

1. Follow the architecture pattern above
2. Include comprehensive tests
3. Write clear documentation
4. Consider cognitive synergy implications
5. Update this README
6. Submit PR with examples

## References

- [Cognitive Synergy Principles](../README_COGNITIVE_SYNERGY.md)
- [Autogenesis Roadmap](../../autogenesis_roadmap.json)
- [Attention Allocation](../../attention/README.md)
- [MOSES](../../asmoses/README.md)

## License

Part of the OpenCog Collection, licensed under GPL-3.0+
