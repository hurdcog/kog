# Autonomous Decision Making System

## Overview

This directory implements the autonomous decision making capabilities for the SKZ cognitive framework, as specified in Phase 4 of the SKZ Integration Strategy. The system provides five levels of autonomy (0-4) following the Tensor Signature Documentation specifications.

## Architecture

### Core Components

- **`autonomous.scm`** - Main autonomous decision making system
- **Decision Context** - Structured representation of decision situations
- **Decision Outcomes** - Results of autonomous reasoning processes
- **TruthKernel Integration** - PLN-based logical reasoning for decisions

### Autonomy Levels

Following the Tensor Signature Documentation:

| Level | Name | Description | Behavior |
|-------|------|-------------|----------|
| 0 | MANUAL | Requires explicit control | Decisions require approval |
| 1 | ASSISTED | Semi-automatic with oversight | Requests human approval for low confidence |
| 2 | AUTOMATIC | Fully automatic operation | Executes decisions autonomously |
| 3 | ADAPTIVE | Learns and adapts behavior | Includes learning-based options |
| 4 | EVOLUTIONARY | Self-modifying behavior | Includes meta-programming options |

## Usage

### Basic Usage

```scheme
(use-modules (cogkernel cognitive-interface decision-making autonomous))

;; Create system with autonomy level 3 (ADAPTIVE)
(define system (make-autonomous-decision-system #:autonomy-level 3))

;; Create decision context
(define context (create-decision-context 
                  '(high-cpu-usage server-alert)
                  '(restart-service investigate scale-up optimize)
                  #:urgency 'high))

;; Make autonomous decision
(define outcome (autonomous-decide system context))
```

### Through Cognitive Interface

```scheme
(use-modules (cogkernel cognitive-interface))

;; Create interface with autonomous decision making
(define interface (make-cognitive-operations-interface #:autonomy-level 2))
(initialize-cognitive-interface interface)

;; Make decision through interface
(define outcome (execute-cognitive-operation interface 'AUTONOMOUS-DECISION
                                           '(network-issue)
                                           '(retry-connection investigate reset)))
```

## Integration

### TruthKernel Integration

The system integrates with TruthKernel for logical reasoning:

- **Situation Evaluation** - Assess criticality, complexity, and familiarity
- **Option Suitability** - Evaluate decision options using PLN reasoning
- **Confidence Assessment** - Provide confidence scores for decisions

### Learning System Integration

- Records decision experiences for learning
- Adapts decision-making based on outcomes
- Supports pattern recognition and temporal difference learning

## Testing

Run the comprehensive test suite:

```bash
cd cogkernel
GUILE_LOAD_PATH="/path/to/hurdcog:$GUILE_LOAD_PATH" guile test-autonomous-decision-simple.scm
```

## Implementation Status

✅ **Completed:**
- Five-level autonomy system (0-4)
- TruthKernel integration for logical reasoning
- Decision context and outcome management
- Cognitive interface integration
- Comprehensive testing suite
- Documentation

🎯 **Ready For:**
- Production deployment in SKZ framework
- Integration with distributed agent framework
- Real-time cognitive operations
- Phase 4 cognitive layer completion

## Technical Notes

- **Tensor DoF**: Supports 3-dimensional cognitive operations as specified
- **Performance**: Optimized for real-time decision making
- **Extensibility**: Modular design allows for easy extension
- **Integration**: Full compatibility with existing SKZ components