# Real-time Learning Systems

This module implements comprehensive real-time learning and adaptation capabilities for the SKZ autonomous agents framework. It provides pattern recognition, temporal difference learning, reinforcement learning, and meta-learning capabilities.

## Overview

The real-time learning system is designed to continuously learn from interactions and experiences, adapting behavior based on observed patterns and outcomes. It integrates with the OpenCog atomspace for knowledge representation and uses attention mechanisms for prioritizing learning.

## Key Features

### 1. Multi-Modal Learning
- **Pattern Recognition**: Identifies recurring patterns in experiences
- **Temporal Difference Learning**: Learns state values and predictions
- **Reinforcement Learning**: Q-learning for action-value functions
- **Meta-Learning**: Higher-order pattern detection across experiences

### 2. Real-time Adaptation
- Continuous learning from ongoing experiences
- Adaptive parameter adjustment based on performance
- Experience replay for improved learning efficiency
- Behavior adaptation based on learned patterns

### 3. Integration with Cognitive Architecture
- AtomSpace integration for knowledge storage
- Attention mechanism for learning prioritization
- Agent system callbacks for learning events
- Tensor-based representation for cognitive processes

## Core Components

### Learning Experience Record
```scheme
(define-record-type <learning-experience>
  (make-learning-experience-record id context action outcome feedback timestamp)
  learning-experience?
  (id learning-experience-id)
  (context learning-experience-context)
  (action learning-experience-action)
  (outcome learning-experience-outcome)
  (feedback learning-experience-feedback)
  (timestamp learning-experience-timestamp))
```

### Learning System Record
```scheme
(define-record-type <learning-system>
  (make-learning-system-record pattern-learning temporal-difference reinforcement experience-buffer callbacks)
  learning-system?
  (pattern-learning learning-system-pattern-learning)
  (temporal-difference learning-system-temporal-difference)
  (reinforcement learning-system-reinforcement)
  (experience-buffer learning-system-experience-buffer)
  (callbacks learning-system-callbacks))
```

## Usage Examples

### Basic Learning System Creation
```scheme
(use-modules (cogkernel cognitive-interface learning-systems realtime))

;; Create a learning system with all learning types enabled
(define ls (make-learning-system #:pattern-learning #t
                                #:temporal-difference #t
                                #:reinforcement #t))
```

### Learning from Experiences
```scheme
;; Create and process a learning experience
(define exp (create-learning-experience 'WEB-REQUEST 'CACHE-HIT 'FAST-RESPONSE 'SUCCESS))
(learn-from-experience ls exp)

;; The system will automatically:
;; - Store the experience in the buffer
;; - Add atoms to the atomspace
;; - Update pattern recognition
;; - Adjust temporal difference values
;; - Update reinforcement learning policies
```

### Pattern Recognition
```scheme
;; Recognize patterns in data
(define patterns (pattern-recognition ls 'WEB-REQUEST))

;; Advanced pattern learning with confidence threshold
(define confident-patterns (advanced-pattern-learning ls '() 0.75))
```

### Behavior Adaptation
```scheme
;; Adapt behavior based on learned patterns
(define adapted-action (adapt-behavior ls 'ERROR-CONTEXT))
;; Returns a symbol representing the recommended action
```

### Learning Effectiveness Monitoring
```scheme
;; Evaluate learning effectiveness
(define effectiveness (evaluate-learning-effectiveness ls))
;; Returns success rate as a number between 0.0 and 1.0

;; Comprehensive monitoring
(define monitor-results (monitor-learning-effectiveness ls))
;; Returns association list with detailed metrics
```

### Callbacks and Events
```scheme
;; Register callback for learning events
(register-learning-callback ls 'my-callback
  (lambda (experience)
    (format #t "Learning event: ~a~%" (learning-experience-id experience))))
```

## Advanced Features

### Q-Learning for Reinforcement Learning
```scheme
;; Update Q-values for state-action pairs
(q-learning-update ls 'STATE-A 'ACTION-X 1.0 'STATE-B)

;; Retrieve Q-value
(define q-val (get-q-value ls "Q-STATE-A-ACTION-X"))
```

### Temporal Difference Learning
```scheme
;; Apply TD learning to an experience
(temporal-difference-learning ls experience)

;; Get state value estimates
(define state-val (get-state-value ls 'MY-STATE))
```

### Experience Replay
```scheme
;; Replay random experiences for improved learning
(experience-replay ls 5)  ; Replay 5 random experiences
```

### Continuous Learning Loop
```scheme
;; Run enhanced continuous learning loop
(enhanced-continuous-learning-loop ls)
;; Includes experience replay, parameter adaptation, and monitoring
```

## Integration with SKZ Framework

The real-time learning system integrates seamlessly with the SKZ autonomous agents framework:

1. **AtomSpace Integration**: All learning experiences and patterns are stored in the OpenCog atomspace
2. **Attention Mechanism**: Important patterns receive attention allocation for prioritized processing
3. **Agent Communication**: Learning events can trigger callbacks to registered agents
4. **Tensor Representation**: Learning data uses tensor-based representation for cognitive processing

### Global Learning System
```scheme
;; Use the global learning system instance
*global-learning-system*

;; Or create specialized instances for specific domains
(define domain-specific-ls (make-learning-system #:pattern-learning #t))
```

## Learning Types and Algorithms

### 1. Pattern Recognition Learning
- Identifies recurring context-action-outcome patterns
- Uses statistical confidence measures
- Creates pattern atoms in the atomspace
- Applies attention stimulation to important patterns

### 2. Temporal Difference Learning
- Estimates state values using TD algorithms
- Adjustable learning rate (alpha = 0.15)
- Updates value functions based on prediction errors
- Suitable for value-based decision making

### 3. Reinforcement Learning (Q-Learning)
- Action-value function learning
- Configurable learning rate and discount factor
- Exploration-exploitation balance
- Policy improvement through experience

### 4. Meta-Learning
- Higher-order pattern detection
- Cross-experience analysis
- Adaptive parameter adjustment
- Learning-to-learn capabilities

## Performance and Monitoring

The learning system provides comprehensive monitoring and adaptation:

- **Learning Effectiveness**: Success rate tracking
- **Pattern Discovery**: Pattern count and confidence
- **Experience Management**: Buffer size and replay efficiency
- **Adaptive Parameters**: Dynamic adjustment based on performance

## Error Handling and Robustness

- Experience validation and sanitization
- Graceful handling of malformed data
- Automatic parameter bounds checking
- Fallback mechanisms for failed learning operations

## Testing

Run the comprehensive test suite:
```bash
cd cogkernel
guile -s test-realtime-learning.scm
```

The test suite validates:
- Learning system creation and configuration
- Experience processing and storage
- Pattern recognition and adaptation
- Integration with atomspace and attention systems
- Learning effectiveness evaluation
- Advanced learning algorithms

## Performance Considerations

- Experience buffer management prevents memory overflow
- Attention-based prioritization optimizes processing
- Experience replay improves sample efficiency
- Adaptive parameters maintain learning quality
- Periodic cleanup prevents atomspace bloat

## Future Enhancements

- Deep learning integration for complex pattern recognition
- Distributed learning across multiple agents
- Online learning with streaming data
- Causal inference and reasoning
- Transfer learning between domains
- Neuromorphic computing adaptation