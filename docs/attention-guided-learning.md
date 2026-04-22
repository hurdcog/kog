# Attention-Guided Evolutionary Learning

## Overview

The **Attention-Guided Evolutionary Learning** integration bridges the Attention Allocation (ECAN) system with MOSES (Meta-Optimizing Semantic Evolutionary Search) to enable **focused learning** - the ability for the system to prioritize learning based on what it deems important.

This is Feature #2 from the [Autogenesis Roadmap](../../autogenesis_roadmap.json), implementing one of the key "adjacent possible" cognitive capabilities identified by the autogenesis engine.

## Emergent Capability: `focused_learning`

The system gains the ability to:
- **Focus learning on important concepts**: High-STI atoms guide what MOSES learns
- **Allocate learning resources efficiently**: Tasks are prioritized by attention + fitness
- **Create feedback loops**: Successful learning reinforces attention to learned concepts
- **Discover important patterns**: New patterns get attention boosts automatically

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                   Attention System (ECAN)                    │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ AtomSpace with Attention Values (STI, LTI, VLTI)     │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────┬───────────────────────────────────────┘
                      │
                      │ 1. Extract high-STI atoms
                      ▼
┌─────────────────────────────────────────────────────────────┐
│              Attention-MOSES Bridge                          │
│                                                              │
│  ┌────────────────────────────────────────────────┐         │
│  │ 1. Attention Signal Extraction                 │         │
│  │    - STI threshold filtering                   │         │
│  │    - LTI threshold filtering                   │         │
│  │    - Attentional focus tracking                │         │
│  └────────────────────────────────────────────────┘         │
│                      │                                       │
│                      │ 2. Convert to fitness bonus           │
│                      ▼                                       │
│  ┌────────────────────────────────────────────────┐         │
│  │ 2. Fitness Bonus Calculation                   │         │
│  │    bonus = (0.7*STI + 0.3*LTI) * scaling       │         │
│  └────────────────────────────────────────────────┘         │
│                      │                                       │
│                      │ 3. Create prioritized tasks           │
│                      ▼                                       │
│  ┌────────────────────────────────────────────────┐         │
│  │ 3. Learning Task Management                    │         │
│  │    priority = base_fitness + attention_bonus   │         │
│  │    Tasks sorted by priority                    │         │
│  └────────────────────────────────────────────────┘         │
│                      │                                       │
└──────────────────────┼───────────────────────────────────────┘
                       │ 4. Prioritized learning tasks
                       ▼
┌─────────────────────────────────────────────────────────────┐
│                 MOSES (Evolutionary Learning)                │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ Learn patterns guided by attention                   │   │
│  │ - Fitness includes attention bonus                   │   │
│  │ - Focus on important atoms                           │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────┬───────────────────────────────────────┘
                      │
                      │ 5. Learning outcomes
                      ▼
┌─────────────────────────────────────────────────────────────┐
│              Feedback Loop to Attention                      │
│  ┌────────────────────────────────────────────────┐         │
│  │ - Increase LTI of successfully learned atoms   │         │
│  │ - Boost STI of newly discovered patterns       │         │
│  │ - Reinforce attention on valuable knowledge    │         │
│  └────────────────────────────────────────────────┘         │
└─────────────────────────────────────────────────────────────┘
```

## Components

### 1. Python Bridge (`synergy/bridges/attention_moses_bridge.py`)

The core implementation providing:

- **AttentionMOSESBridge**: Main bridge class
- **AttentionSignal**: Attention signal data structure
- **LearningTask**: Learning task with priority
- Signal extraction, fitness calculation, task management, feedback loop

### 2. Scheme Interface (`synergy/bridges/attention-moses-bridge.scm`)

Scheme bindings for AtomSpace integration:

- `(get-high-sti-atoms threshold)`: Extract important atoms
- `(create-attention-guided-task description atoms)`: Create learning task
- `(get-prioritized-tasks limit)`: Get tasks by priority
- `(feedback-successful-learning task-id patterns accuracy)`: Apply feedback

### 3. Tests (`tests/synergy/test_attention_moses_bridge.py`)

Comprehensive test suite covering:
- Attention signal extraction
- Fitness bonus calculation
- Task prioritization
- Feedback loops
- Complete workflows

## Usage Examples

### Python API

```python
from synergy.bridges.attention_moses_bridge import AttentionMOSESBridge

# Initialize bridge
bridge = AttentionMOSESBridge(
    sti_threshold=100.0,
    lti_threshold=50.0,
    fitness_scaling=2.0
)

# 1. Extract attention signals from atoms
atoms = [
    {'id': 'concept1', 'type': 'ConceptNode', 'sti': 150, 'lti': 60},
    {'id': 'concept2', 'type': 'ConceptNode', 'sti': 200, 'lti': 80},
]
signals = bridge.extract_attention_signals(atoms)

# 2. Create learning task
task = bridge.create_learning_task(
    task_id='learn_important_concepts',
    description='Learn patterns in important concepts',
    attention_signals=signals,
    base_fitness=0.5
)

# 3. Get prioritized tasks
prioritized = bridge.get_prioritized_tasks(limit=5)

# 4. Simulate learning and feedback
for task in prioritized[:2]:
    bridge.update_task_status(task.task_id, 'running')
    
    # ... run MOSES learning ...
    
    outcomes = {
        'success': True,
        'accuracy': 0.85,
        'patterns_learned': [{'id': 'new_pattern_1', 'confidence': 0.9}]
    }
    
    bridge.update_task_status(task.task_id, 'completed', outcomes)
    recommendations = bridge.feedback_to_attention(task.task_id, outcomes)
    
    # Apply recommendations to attention system
    for update in recommendations['updates']:
        # Update atom attention values
        pass
```

### Scheme API

```scheme
(use-modules (opencog) (opencog attention))
(load "synergy/bridges/attention-moses-bridge.scm")

; 1. Create atoms with attention values
(define concept1 (ConceptNode "important-concept"))
(cog-set-av! concept1 (cog-new-av 150 60 0))

; 2. Extract high-STI atoms
(define important-atoms (get-high-sti-atoms 100))

; 3. Create learning task
(define task (create-attention-guided-task
               "Learn important patterns"
               important-atoms))

; 4. Get prioritized tasks
(define tasks (get-prioritized-tasks 3))

; 5. Provide feedback after learning
(define new-pattern (PredicateNode "learned-pattern"))
(feedback-successful-learning
  (assoc-ref task 'task-id)
  (list new-pattern)
  0.85)
```

## Configuration Parameters

### AttentionMOSESBridge Parameters

- **sti_threshold** (default: 100.0): Minimum STI for atom to be considered important
- **lti_threshold** (default: 50.0): Minimum LTI for atom to be considered important
- **fitness_scaling** (default: 1.0): Scaling factor for attention → fitness conversion
- **max_tasks** (default: 10): Maximum number of concurrent learning tasks
- **max_importance** (default: 1000.0): Maximum expected importance value for normalization
- **feedback_lti_delta** (default: 10.0): LTI increase for successfully learned atoms
- **feedback_sti_boost** (default: 50.0): STI boost for newly discovered patterns

### Fitness Calculation Weights

- **STI weight**: 0.7 (short-term importance gets higher weight)
- **LTI weight**: 0.3 (long-term importance contributes less)
- **Max importance**: Configurable normalization ceiling (default: 1000.0)

Formula: `bonus = (0.7 * (STI/max_importance) + 0.3 * (LTI/max_importance)) * fitness_scaling`

## Testing

Run the test suite:

```bash
cd /home/runner/work/occ/occ
python3 tests/synergy/test_attention_moses_bridge.py
```

Run the demo:

```bash
python3 synergy/bridges/attention_moses_bridge.py
```

Run the Scheme demo:

```scheme
(load "synergy/bridges/attention-moses-bridge.scm")
(demo-attention-guided-learning)
```

## Integration with Existing Systems

### AtomSpace Integration

The bridge works seamlessly with AtomSpace attention values:

```python
# Extract attention from AtomSpace atoms
from opencog.atomspace import AtomSpace, types

atomspace = AtomSpace()
atom = atomspace.add_node(types.ConceptNode, "test")
atom.av = {"sti": 150, "lti": 60, "vlti": 0}

# Convert to bridge format
atom_dict = {
    'id': atom.name,
    'type': atom.type_name,
    'sti': atom.av['sti'],
    'lti': atom.av['lti']
}
```

### MOSES Integration

The fitness bonus can be integrated into MOSES scoring:

```python
def moses_fitness_function(candidate, base_fitness, attention_bonus):
    """
    MOSES fitness function with attention guidance
    """
    # Base fitness from accuracy on training data
    base = evaluate_candidate(candidate)
    
    # Add attention bonus to encourage learning important patterns
    total_fitness = base + attention_bonus
    
    return total_fitness
```

## Performance Considerations

- **Signal Extraction**: O(n) where n = number of atoms
- **Fitness Calculation**: O(1) per signal
- **Task Prioritization**: O(m log m) where m = number of tasks
- **Feedback Application**: O(k) where k = number of updates

For large AtomSpaces (>100k atoms), consider:
- Maintaining an index of high-attention atoms
- Batch processing of attention signals
- Lazy evaluation of fitness bonuses

## Future Enhancements

Planned improvements (see autogenesis roadmap):

1. **Dynamic Threshold Adaptation**: Automatically adjust STI/LTI thresholds based on system load
2. **Multi-Modal Attention**: Integrate attention from multiple sources (visual, linguistic, etc.)
3. **Meta-Learning Feedback**: Learn optimal fitness scaling parameters
4. **Distributed Learning**: Coordinate attention-guided learning across multiple MOSES instances
5. **Pattern Transfer**: Transfer learned patterns back to AtomSpace with appropriate attention values

## References

- [ECAN Attention Allocation](../../attention/README.md)
- [MOSES Evolutionary Learning](../../asmoses/README.md)
- [Autogenesis Roadmap](../../autogenesis_roadmap.json)
- [Cognitive Synergy Principles](../README_COGNITIVE_SYNERGY.md)

## Implementation Status

- **Status**: Phase 1 Complete ✓
- **Estimated Effort**: 1-2 weeks (per autogenesis analysis)
- **Actual Effort**: 1 day (initial implementation)
- **Prerequisites**: ✅ attention, ✅ moses (both exist)
- **Emergent Capability**: ✅ `focused_learning` validated through tests

## Contributing

When extending this bridge:

1. Maintain backward compatibility with existing APIs
2. Add tests for new functionality
3. Update documentation
4. Consider cognitive synergy implications
5. Follow existing code style and patterns

## License

Part of the OpenCog Collection, licensed under GPL-3.0+
