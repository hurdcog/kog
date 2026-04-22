# Phase 2: ECAN Attention Allocation & Resource Kernel Construction

## Overview

Phase 2 implements a complete Economic Attention Networks (ECAN) system for HurdCog, providing dynamic, distributed attention allocation with cognitive economics. This implementation extends the basic attention mechanisms with:

- **Cognitive Wages**: Reward productive activity
- **Attention Rent**: Prevent resource hoarding
- **Spreading Activation**: Network-wide attention propagation
- **Priority Scheduling**: Attention-driven task execution
- **Distributed Networks**: Multi-node attention coordination

## Architecture

### Core Components

```
┌─────────────────────────────────────────────────────┐
│           ECAN Attention Allocation System          │
├─────────────────────────────────────────────────────┤
│                                                     │
│  ┌─────────────────────────────────────────────┐  │
│  │         Attention Bank                      │  │
│  │  • STI/LTI/VLTI tracking                   │  │
│  │  • Focus threshold management              │  │
│  │  • Total funds pool                        │  │
│  └─────────────────────────────────────────────┘  │
│                       ▼                            │
│  ┌─────────────────────────────────────────────┐  │
│  │      ECAN Economics Engine                  │  │
│  │  • Cognitive wages (activity rewards)      │  │
│  │  • Attention rent (resource costs)         │  │
│  │  • Spreading activation (propagation)      │  │
│  │  • Economics history tracking              │  │
│  └─────────────────────────────────────────────┘  │
│                       ▼                            │
│  ┌─────────────────────────────────────────────┐  │
│  │    Priority-Based Task Scheduler            │  │
│  │  • Effective priority = priority × STI     │  │
│  │  • Attention-driven execution order        │  │
│  │  • Dynamic re-prioritization              │  │
│  └─────────────────────────────────────────────┘  │
│                       ▼                            │
│  ┌─────────────────────────────────────────────┐  │
│  │  Distributed Attention Network              │  │
│  │  • Inter-node synchronization              │  │
│  │  • Mesh topology management                │  │
│  │  • Attention event broadcasting            │  │
│  └─────────────────────────────────────────────┘  │
│                                                     │
└─────────────────────────────────────────────────────┘
```

## ECAN Economics

### Short-Term Importance (STI)

**Purpose**: Immediate attention allocation for urgent processing

**Dynamics**:
- Increased by: Cognitive wages, stimulation, spreading activation
- Decreased by: Attention rent, natural decay
- Threshold: Items above focus threshold enter attentional focus

**Formula**:
```
new_STI = current_STI + wages - rent + spread_in - spread_out
```

### Long-Term Importance (LTI)

**Purpose**: Track persistent significance over time

**Dynamics**:
- Increased by: Repeated high STI, pattern reinforcement
- Decreased by: Long-term decay, obsolescence
- Purpose: Prevent valuable atoms from being forgotten

**Formula**:
```
new_LTI = current_LTI + (STI * conversion_rate) - long_term_decay
```

### Very-Long-Term Importance (VLTI)

**Purpose**: System-level structural importance

**Dynamics**:
- Increased by: Core system components, fundamental patterns
- Rarely changes: Reserved for foundational concepts
- Protection: High VLTI items immune to aggressive decay

### Cognitive Wages

Reward productive activity with attention allocation.

**Implementation**:
```scheme
(attention-bank-apply-wages! bank activities)

;; activities: list of (object activity-value) pairs
;; Example:
'((agent1 200)  ; agent1 performed 200 units of work
  (atom1 150))  ; atom1 was accessed 150 times
```

**Economics**:
- Wage Rate: Configurable percentage of activity (default: 10%)
- Distribution: `wage = activity_value × wage_rate`
- Application: Added to object's STI
- Tracking: Recorded in economics history

**Purpose**:
- Reward active components
- Increase attention to productive agents
- Encourage system activity

### Attention Rent

Charge rent for holding attention resources.

**Implementation**:
```scheme
(attention-bank-collect-rent! bank)
```

**Economics**:
- Rent Rate: Configurable percentage of STI (default: 5%)
- Collection: `rent = current_STI × rent_rate`
- Effect: Reduces object's STI
- Return: Rent returned to global attention funds pool
- Tracking: Recorded in economics history

**Purpose**:
- Prevent resource hoarding
- Free up attention for new items
- Maintain system dynamics
- Economic conservation

### Spreading Activation

Propagate attention through the hypergraph network.

**Implementation**:
```scheme
(attention-bank-spread-activation! bank atomspace #:max-depth 3)
```

**Algorithm**:
1. Identify focused atoms (STI > threshold)
2. For each focused atom:
   - Find connected atoms via links
   - Calculate spread amount: `source_STI × spread_rate / num_connections`
   - Distribute attention to connected atoms
3. Record spreading events

**Economics**:
- Spread Rate: Configurable percentage (default: 20%)
- Depth Limit: Maximum propagation hops
- Distribution: Equal split among connections
- Conservation: Total attention conserved

**Purpose**:
- Activate related concepts
- Context propagation
- Network-wide attention flow
- Associative reasoning

## Priority-Based Task Scheduling

Schedule tasks using attention-weighted priorities.

**Implementation**:
```scheme
(attention-bank-schedule-tasks! bank task-queue)

;; task-queue: list of (task-id priority object) tuples
;; Returns: tasks sorted by effective priority
```

**Effective Priority Formula**:
```
effective_priority = base_priority × max(1.0, object_STI)
```

**Behavior**:
- High STI objects get priority boost
- Tasks associated with focused items execute first
- Dynamic re-prioritization as STI changes
- Attention-driven execution order

**Benefits**:
- Automatic prioritization
- Context-aware scheduling
- Self-organizing task execution
- Optimal resource utilization

## Distributed Attention Networks

Coordinate attention across multiple nodes.

**Implementation**:
```scheme
;; Create distributed network
(define network (make-distributed-attention-network local-bank 
                                                    #:sync-interval 60))

;; Synchronize with remote node
(distributed-attention-sync! network "node-1")

;; Broadcast attention event
(distributed-attention-broadcast! network '(type . alert))
```

**Features**:
- **Node Synchronization**: Exchange attention state between nodes
- **Mesh Topology**: Flexible network configuration
- **Event Broadcasting**: Propagate attention events network-wide
- **Distributed Economics**: Coordinated resource management

**Use Cases**:
- Multi-node Hurd systems
- Distributed cognitive processing
- Cluster attention coordination
- Federated attention networks

## Economics History

Track all attention economics operations for transparency and analysis.

**Recorded Events**:
- Wage applications
- Rent collections
- Spreading activation
- Task scheduling
- Distributed synchronization
- Event broadcasts

**Data Structure**:
```scheme
'((type . wages)
  (amount . 150)
  (timestamp . 1729699459)
  (activities . 5))
```

**Query Economics**:
```scheme
(attention-bank-get-economics bank)

;; Returns:
'((total-funds . 10000)
  (total-sti . 1500)
  (total-lti . 750)
  (atom-count . 25)
  (agent-count . 8)
  (wage-rate . 0.1)
  (rent-rate . 0.05)
  (spread-rate . 0.2)
  (history-length . 42))
```

## API Reference

### Core Functions

#### `make-attention-bank`
Create attention bank with ECAN economics.

```scheme
(make-attention-bank #:total-funds 10000
                     #:focus-threshold 100
                     #:wage-rate 0.1
                     #:rent-rate 0.05
                     #:spread-rate 0.2)
```

#### `attention-bank-apply-wages!`
Apply cognitive wages for productive activity.

```scheme
(attention-bank-apply-wages! bank '((agent1 200) (atom1 150)))
;; Returns: total wages distributed
```

#### `attention-bank-collect-rent!`
Collect attention rent from all holders.

```scheme
(attention-bank-collect-rent! bank)
;; Returns: total rent collected
```

#### `attention-bank-spread-activation!`
Spread attention through connected atoms.

```scheme
(attention-bank-spread-activation! bank atomspace #:max-depth 3)
;; Returns: number of spreads performed
```

#### `attention-bank-schedule-tasks!`
Schedule tasks by effective priority.

```scheme
(attention-bank-schedule-tasks! bank task-queue)
;; Returns: sorted task list
```

#### `attention-bank-get-economics`
Get economics summary.

```scheme
(attention-bank-get-economics bank)
;; Returns: association list with statistics
```

### Distributed Functions

#### `make-distributed-attention-network`
Create distributed attention network.

```scheme
(make-distributed-attention-network local-bank #:sync-interval 60)
```

#### `distributed-attention-sync!`
Synchronize with remote node.

```scheme
(distributed-attention-sync! network "node-1")
```

#### `distributed-attention-broadcast!`
Broadcast attention event.

```scheme
(distributed-attention-broadcast! network '(type . alert))
```

## Configuration Parameters

### Attention Bank Parameters

| Parameter | Default | Range | Description |
|-----------|---------|-------|-------------|
| `total-funds` | 10000 | 1000-100000 | Total attention resources pool |
| `focus-threshold` | 100 | 10-1000 | STI threshold for attentional focus |
| `wage-rate` | 0.1 | 0.01-0.5 | Percentage of activity as wage |
| `rent-rate` | 0.05 | 0.01-0.2 | Percentage of STI as rent |
| `spread-rate` | 0.2 | 0.1-0.5 | Percentage of STI to spread |

### Tuning Guidelines

**High Activity Systems**: 
- Increase `wage-rate` to reward activity
- Decrease `rent-rate` to retain attention longer

**Resource Constrained**:
- Decrease `total-funds` to limit resources
- Increase `rent-rate` to free resources faster

**Associative Processing**:
- Increase `spread-rate` for more propagation
- Lower `focus-threshold` for broader context

## Performance Characteristics

### Time Complexity

- Wage application: O(n) where n = number of activities
- Rent collection: O(a + g) where a = atoms, g = agents
- Spreading activation: O(f × c × d) where f = focused atoms, c = avg connections, d = depth
- Task scheduling: O(t log t) where t = number of tasks

### Space Complexity

- Attention bank: O(a + g) for attention values
- Economics history: O(h) where h = history length
- Distributed network: O(n) where n = number of nodes

### Benchmarks

Expected performance on modern hardware:

- Wage application: ~10,000 activities/sec
- Rent collection: ~50,000 objects/sec
- Spreading activation: ~5,000 spreads/sec
- Task scheduling: ~20,000 tasks/sec

## Testing

### Test Suite

Run the complete ECAN test suite:

```bash
# Python integration test
python3 test-ecan-phase2.py

# Direct Guile test
cd cogkernel
guile -s tests/test-ecan-economics.scm
```

### Test Coverage

- ✅ Basic economics setup
- ✅ Cognitive wages application
- ✅ Attention rent collection
- ✅ STI/LTI/VLTI dynamics
- ✅ Focus threshold behavior
- ✅ Priority-based scheduling
- ✅ Stimulation types
- ✅ Distributed attention networks
- ✅ Economics history tracking
- ✅ Complete ECAN cycle

## Integration with Hurd

### Microkernel Integration

ECAN attention allocation integrates with GNU Hurd microkernel:

1. **Server Prioritization**: Hurd servers receive attention based on activity
2. **IPC Routing**: Attention-driven message routing optimization
3. **Resource Allocation**: ECAN economics guide memory/CPU allocation
4. **Task Scheduling**: Hurd tasks scheduled by effective attention priority

### AtomSpace Integration

ECAN works with the hypergraph atomspace:

1. **Atom Attention**: Every atom has STI/LTI/VLTI values
2. **Link Propagation**: Spreading activation follows link structure
3. **Pattern Mining**: High-attention patterns discovered and reinforced
4. **Memory Management**: Low-attention atoms eligible for eviction

## Future Enhancements

### Phase 3 Roadmap

- **Adaptive Parameters**: Self-tuning wage/rent/spread rates
- **Quantum Attention**: Superposition of attention states
- **Attention Markets**: Inter-agent attention trading
- **Learning Dynamics**: Reinforcement learning for optimal allocation
- **Prediction**: Anticipatory attention allocation
- **Visualization**: Real-time attention flow visualization

## References

- OpenCog ECAN: Economic Attention Networks
- Baars' Global Workspace Theory
- Kanerva's Sparse Distributed Memory
- Attention Schema Theory (Graziano)

## Status

**Phase 2: COMPLETE ✅**

All ECAN attention allocation features implemented and tested:
- ✅ Cognitive wages operational
- ✅ Attention rent functional
- ✅ STI/LTI/VLTI dynamics working
- ✅ Spreading activation implemented
- ✅ Priority scheduling active
- ✅ Distributed networks ready
- ✅ Economics history tracked
- ✅ Production ready

**Next Phase**: Phase 3 - Advanced cognitive features and self-optimization
