# Diagnostic Atoms Module

## Overview

The Diagnostic Atoms module provides a comprehensive framework for system introspection, error tracking, and autognostic capabilities in the OpenCog Collection (OCC). This module defines specialized atom types that represent various aspects of system state, errors, performance metrics, and repair actions.

## Architecture

The diagnostic atoms system consists of three main components:

1. **Schema Definitions** (`schemas/`): Atom type definitions in both Scheme and Python
2. **Serialization** (`serialization/`): Conversion between different representations
3. **Validation** (`validation/`): Validation rules and quality checks

## Diagnostic Atom Types

### Base Types

- **DiagnosticAtom**: Base type for all diagnostic atoms
- **MetaDiagnosticAtom**: Diagnostics about the diagnostic system itself

### Error Diagnostics

- **ErrorAtom**: General system errors and exceptions
- **CriticalErrorAtom**: Critical errors requiring immediate attention
- **RecoverableErrorAtom**: Errors that can be recovered through repair actions

### Reasoning Diagnostics

- **PredictionMismatchAtom**: Mismatches between predicted and actual outcomes
- **InferenceFailureAtom**: PLN inference failures
- **TruthValueInconsistencyAtom**: Inconsistencies in truth value assignments

### Performance Diagnostics

- **StallAtom**: Performance stalls and bottlenecks
- **ResourceAtom**: Resource utilization metrics
- **MemoryPressureAtom**: High memory usage conditions
- **CPUBottleneckAtom**: CPU-bound operations

### Attention Diagnostics (ECAN)

- **AttentionDiagnosticAtom**: Base type for ECAN diagnostics
- **AttentionAllocationFailureAtom**: Attention allocation failures
- **ThresholdViolationAtom**: STI/LTI threshold violations
- **ForgettingEventAtom**: Atom forgetting events

### State Diagnostics

- **StateAtom**: System state snapshots
- **CheckpointAtom**: Full system checkpoints for rollback

### Repair Diagnostics

- **RepairAtom**: Repair actions and suggestions
- **RepairSuccessAtom**: Successful repair executions
- **RepairFailureAtom**: Failed repair attempts

## Usage

### Python API

```python
from diagnostic_atoms.schemas.diagnostic_atoms import (
    create_error_diagnostic,
    create_performance_diagnostic,
    create_attention_diagnostic,
    ComponentType,
    SeverityLevel
)

# Create an error diagnostic
error = create_error_diagnostic(
    error_type="RuntimeError",
    message="Inference timeout after 30 seconds",
    component=ComponentType.PLN,
    severity=SeverityLevel.HIGH
)

# Convert to dictionary
error_dict = error.to_dict()

# Convert to Atomese
atomese_repr = error.to_atomese()

# Create a performance diagnostic
stall = create_performance_diagnostic(
    operation="backward_chaining",
    duration_ms=1500.0,
    component=ComponentType.PLN
)

# Create an attention diagnostic
attention_failure = create_attention_diagnostic(
    atom_handle="atom_12345",
    requested_sti=100.0,
    allocated_sti=50.0,
    constraint="budget_exceeded"
)
```

### Scheme API

```scheme
;; Load diagnostic types
(load "diagnostic_types.scm")

;; Create an error diagnostic
(define error-diag
  (ErrorAtom "inference_timeout_001"))

;; Add metadata
(Evaluation
  (Predicate "severity")
  (List
    error-diag
    (Concept "critical")))

(Evaluation
  (Predicate "timestamp")
  (List
    error-diag
    (Concept "2025-12-12T10:30:45Z")))

;; Create causal relationship
(CausedByLink
  (InferenceFailureAtom "pln_failure_042")
  (MemoryPressureAtom "mem_pressure_015"))

;; Create repair link
(RepairedByLink
  (StallAtom "reasoning_stall_007")
  (RepairAtom "increase_attention_budget"))
```

## Diagnostic Relationships

The module defines several link types for expressing relationships between diagnostics:

- **CausedByLink**: Causal relationships between diagnostics
- **TriggeredByLink**: Event triggers for diagnostics
- **RepairedByLink**: Links diagnostics to repair actions
- **PrecedesLink**: Temporal ordering of diagnostic events

## Metadata Predicates

Diagnostic atoms support rich metadata through predicates:

- **SeverityPredicate**: critical, high, medium, low
- **TimestampPredicate**: ISO 8601 timestamp
- **ComponentPredicate**: Affected system component
- **ConfidencePredicate**: Confidence level (0.0 to 1.0)

## Integration with Autognostic System

Diagnostic atoms serve as the primary data structure for the autognostic LLM system:

1. **Generation**: System components generate diagnostic atoms during operation
2. **Serialization**: Diagnostic atoms are serialized to natural language for LLM input
3. **Analysis**: LLM analyzes diagnostic patterns and suggests repairs
4. **Repair**: Repair atoms are generated and executed
5. **Feedback**: Success/failure atoms provide learning feedback

## Data Flow

```
System Operation
    ↓
Diagnostic Generation (ErrorAtom, StallAtom, etc.)
    ↓
Atomspace Storage
    ↓
Serialization to Natural Language
    ↓
LLM Analysis
    ↓
RepairAtom Generation
    ↓
Repair Execution
    ↓
Success/Failure Feedback
    ↓
Meta-Learning
```

## Severity Levels

Diagnostic atoms use a five-level severity system:

1. **CRITICAL**: System instability, requires immediate action
2. **HIGH**: Significant issues affecting functionality
3. **MEDIUM**: Notable issues that should be addressed
4. **LOW**: Minor issues or warnings
5. **INFO**: Informational diagnostics for monitoring

## Component Types

Diagnostics can originate from various system components:

- **ATOMSPACE**: Core Atomspace operations
- **ECAN**: Economic Attention Network
- **PLN**: Probabilistic Logic Networks
- **COGSERVER**: CogServer and network operations
- **ATTENTION**: Attention allocation system
- **URE**: Unified Rule Engine
- **MOSES**: Meta-Optimizing Semantic Evolutionary Search

## Best Practices

### 1. Diagnostic Generation

- Generate diagnostics at appropriate granularity
- Include sufficient context for analysis
- Set appropriate severity levels
- Maintain consistent timestamp format

### 2. Metadata Management

- Always include timestamp and component information
- Set confidence levels based on diagnostic certainty
- Use structured metadata for machine processing

### 3. Causal Relationships

- Establish causal links when relationships are known
- Use temporal ordering (PrecedesLink) for event sequences
- Link repairs to the diagnostics they address

### 4. Performance Considerations

- Avoid excessive diagnostic generation in hot paths
- Use sampling for high-frequency events
- Implement diagnostic rate limiting if needed

## Testing

The module includes comprehensive tests:

```bash
# Run unit tests
python -m pytest diagnostic-atoms/tests/

# Run validation tests
python -m pytest diagnostic-atoms/validation/tests/

# Run integration tests
python -m pytest diagnostic-atoms/tests/integration/
```

## Future Enhancements

### Phase 2 (Weeks 3-4)
- LLM prompt template integration
- Natural language serialization
- Diagnostic pattern recognition

### Phase 3 (Weeks 5-6)
- Repair action execution framework
- Rollback mechanism integration
- Success metric tracking

### Phase 4 (Weeks 7-8)
- Environment feedback loop
- Meta-learning from diagnostic patterns
- Automated repair heuristic evolution

## Contributing

When adding new diagnostic atom types:

1. Define the type in `schemas/diagnostic_types.scm`
2. Create corresponding Python class in `schemas/diagnostic_atoms.py`
3. Implement `to_atomese()` method
4. Add serialization support
5. Write unit tests
6. Update this README

## References

- [OpenCog Atomspace Documentation](https://wiki.opencog.org/w/Atomspace)
- [ECAN (Economic Attention Network)](https://wiki.opencog.org/w/ECAN)
- [PLN (Probabilistic Logic Networks)](https://wiki.opencog.org/w/PLN)
- [Autognostic Systems](https://arxiv.org/abs/2103.12345) (placeholder)

## License

AGPL-3.0 - See LICENSE file for details

## Authors

OCC Evolution Team - 2025
