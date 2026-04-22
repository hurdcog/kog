# Neural-Symbolic Integration Design

**Component**: Cognitive Grammar Kernel - Neural-Symbolic Bridge  
**Purpose**: Bridge between Echo Propagation (neural/sub-symbolic) and Symbolic Reasoning  
**Status**: Design Phase

## Overview

The Neural-Symbolic Integration component enables bidirectional translation between:
- **Neural representations**: Activation patterns in the hypergraph
- **Symbolic representations**: Atoms, links, and inference rules

This bridges the gap between **sub-symbolic processing** (spreading activation, pattern recognition) and **symbolic reasoning** (logical inference, rule application).

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│         Neural-Symbolic Integration Layer               │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌──────────────────┐         ┌──────────────────┐     │
│  │  Concept         │         │  Rule            │     │
│  │  Formation       │         │  Extraction      │     │
│  │                  │         │                  │     │
│  │  Activation  →   │         │  Co-activation → │     │
│  │  Patterns    →   │         │  Patterns     →  │     │
│  │              →   │         │               →  │     │
│  │  New Concepts    │         │  New Rules       │     │
│  └──────────────────┘         └──────────────────┘     │
│           ↓                            ↓                │
│  ┌──────────────────────────────────────────────────┐  │
│  │         Symbol Grounding                         │  │
│  │  (Connect symbols to sensorimotor experience)    │  │
│  └──────────────────────────────────────────────────┘  │
│           ↓                            ↓                │
│  ┌──────────────────┐         ┌──────────────────┐     │
│  │  Hybrid          │         │  Guided          │     │
│  │  Reasoning       │         │  Propagation     │     │
│  │                  │         │                  │     │
│  │  Symbolic +      │         │  Symbolic →      │     │
│  │  Neural          │         │  Neural          │     │
│  └──────────────────┘         └──────────────────┘     │
│                                                          │
└─────────────────────────────────────────────────────────┘
         ↑                                    ↓
┌────────────────────┐            ┌────────────────────┐
│  Echo Propagation  │            │  Symbolic          │
│  Engine            │            │  Reasoning Engine  │
│  (Neural)          │            │  (Symbolic)        │
└────────────────────┘            └────────────────────┘
```

## Components

### 1. Concept Formation from Activation

**Purpose**: Discover new concepts from recurring activation patterns

**Input**: Activation history from Echo Propagation Engine  
**Output**: New concept nodes representing discovered patterns

**Algorithm**:

1. **Pattern Detection**
   - Track activation patterns over time
   - Identify frequently co-activated atom clusters
   - Compute cluster coherence (how often atoms activate together)

2. **Concept Extraction**
   - For each significant cluster:
     - Create new concept node
     - Link to constituent atoms
     - Assign truth value based on pattern strength
     - Assign attention value based on frequency

3. **Concept Refinement**
   - Merge similar concepts
   - Split overly broad concepts
   - Prune weak concepts

**Example**:
```
Activation Pattern:
  Cat (0.9), Whiskers (0.8), Meow (0.7), Fur (0.8)
  → frequently co-activate

Concept Formation:
  Create: FelineCharacteristics
  Link: FelineCharacteristics → {Cat, Whiskers, Meow, Fur}
  TV: (0.85, 0.90) [high strength, high confidence]
```

### 2. Rule Extraction from Co-Activation

**Purpose**: Discover inference rules from activation patterns

**Input**: Co-activation patterns from Echo Propagation Engine  
**Output**: New inference rules

**Algorithm**:

1. **Pattern Mining**
   - Track temporal co-activation sequences
   - Identify causal patterns (A activates → B activates)
   - Compute pattern support and confidence

2. **Rule Generation**
   - For each significant pattern:
     - Create rule: IF (pattern A) THEN (pattern B)
     - Assign strength based on confidence
     - Validate rule through backward chaining

3. **Rule Validation**
   - Test rule on held-out data
   - Compute precision and recall
   - Prune low-quality rules

**Example**:
```
Co-Activation Pattern:
  Cat (t=0) → Mammal (t=1) [90% of the time]
  Mammal (t=0) → Animal (t=1) [95% of the time]

Rule Extraction:
  Rule 1: IF Cat THEN Mammal (strength: 0.90)
  Rule 2: IF Mammal THEN Animal (strength: 0.95)
  
  Validate: Cat → Animal (derived via transitivity)
```

### 3. Symbol Grounding

**Purpose**: Connect abstract symbols to sensorimotor experience

**Input**: Symbols (concepts, predicates) + sensorimotor data  
**Output**: Grounded symbols with experiential meaning

**Mechanisms**:

1. **Perceptual Grounding**
   - Link concepts to sensory patterns
   - Example: "Red" → visual activation pattern

2. **Motor Grounding**
   - Link actions to motor patterns
   - Example: "Grasp" → motor command sequence

3. **Embodied Simulation**
   - Activate sensorimotor patterns when reasoning about concepts
   - Example: Reasoning about "Ball" activates visual + motor patterns

**Example**:
```
Symbol: "Cat"
Grounding:
  - Visual: {fur texture, whiskers, ears, eyes}
  - Auditory: {meow sound pattern}
  - Tactile: {soft fur sensation}
  - Motor: {petting motion}
```

### 4. Hybrid Reasoning

**Purpose**: Combine symbolic and neural reasoning

**Mechanisms**:

1. **Symbolic-Guided Neural**
   - Use symbolic rules to guide activation spreading
   - Example: If reasoning about "Cat", boost activation of "Mammal"

2. **Neural-Weighted Symbolic**
   - Use activation values to weight inference rules
   - Example: Apply "Cat→Mammal" with strength proportional to activation

3. **Iterative Refinement**
   - Alternate between symbolic and neural steps
   - Example: Symbolic inference → neural propagation → symbolic inference

**Example**:
```
Query: "Is Cat an Animal?"

Hybrid Reasoning:
  1. Symbolic: Apply transitivity (Cat→Mammal→Animal)
  2. Neural: Spread activation from Cat
  3. Symbolic: Verify Animal is highly activated
  4. Result: Yes (confidence: 0.95)
```

### 5. Guided Propagation

**Purpose**: Use symbolic knowledge to guide neural activation

**Mechanisms**:

1. **Rule-Based Boosting**
   - When atom X activates, boost atoms Y where (X→Y) rule exists
   - Boost amount proportional to rule strength

2. **Constraint Satisfaction**
   - Use symbolic constraints to prune invalid activation paths
   - Example: Don't activate "Mammal" if "Reptile" is already active

3. **Goal-Directed Spreading**
   - Use symbolic goals to bias activation spreading
   - Example: If goal is "Find food", boost food-related concepts

**Example**:
```
Activation: Cat (1.0)
Symbolic Knowledge: Cat→Mammal (0.9), Cat→Pet (0.8)

Guided Propagation:
  - Boost Mammal by 0.9
  - Boost Pet by 0.8
  - Result: Mammal (0.9), Pet (0.8) activated
```

## Data Structures

### Activation History

```scheme
(define-record-type activation-history
  (fields
    atom           ; The atom being tracked
    timestamps     ; List of activation timestamps
    values         ; List of activation values
    co-activations ; Hash table: atom → co-activation count
    ))
```

### Concept Cluster

```scheme
(define-record-type concept-cluster
  (fields
    atoms          ; List of atoms in cluster
    coherence      ; How often atoms co-activate (0-1)
    frequency      ; How often cluster activates
    concept-node   ; The concept node representing this cluster
    ))
```

### Activation Rule

```scheme
(define-record-type activation-rule
  (fields
    antecedent     ; Activation pattern (list of atoms)
    consequent     ; Resulting activation pattern
    support        ; How often pattern occurs
    confidence     ; P(consequent | antecedent)
    strength       ; Overall rule strength
    ))
```

### Grounding

```scheme
(define-record-type symbol-grounding
  (fields
    symbol         ; The symbolic atom
    modality       ; Sensory modality (visual, auditory, etc.)
    pattern        ; The sensorimotor pattern
    strength       ; Grounding strength (0-1)
    ))
```

## Algorithms

### Algorithm 1: Concept Formation

```scheme
(define (form-concepts-from-activation atomspace history threshold)
  "Discover new concepts from activation patterns.
   
   Parameters:
   - atomspace: The atomspace to add concepts to
   - history: Activation history data
   - threshold: Minimum coherence for concept formation
   
   Returns: List of newly formed concept nodes"
  
  ;; 1. Detect clusters
  (let ((clusters (detect-activation-clusters history threshold)))
    
    ;; 2. Create concept nodes
    (map (lambda (cluster)
           (let* ((concept-name (generate-concept-name cluster))
                  (concept-node (create-concept-node atomspace concept-name)))
             
             ;; Link concept to constituent atoms
             (for-each (lambda (atom)
                        (create-inheritance-link atomspace atom concept-node))
                      (concept-cluster-atoms cluster))
             
             ;; Set truth value based on coherence
             (atom-set-tv! concept-node
                          (make-truth-value (concept-cluster-coherence cluster)
                                          0.9))
             
             ;; Set attention value based on frequency
             (atom-set-av! concept-node
                          (make-attention-value (concept-cluster-frequency cluster)
                                              0.5 0.5))
             
             concept-node))
         clusters)))
```

### Algorithm 2: Rule Extraction

```scheme
(define (extract-rules-from-coactivation atomspace history min-support min-confidence)
  "Extract inference rules from co-activation patterns.
   
   Parameters:
   - atomspace: The atomspace
   - history: Activation history data
   - min-support: Minimum pattern frequency
   - min-confidence: Minimum rule confidence
   
   Returns: List of extracted inference rules"
  
  ;; 1. Mine co-activation patterns
  (let ((patterns (mine-coactivation-patterns history min-support)))
    
    ;; 2. Generate rules
    (filter-map
      (lambda (pattern)
        (let* ((antecedent (pattern-antecedent pattern))
               (consequent (pattern-consequent pattern))
               (support (pattern-support pattern))
               (confidence (pattern-confidence pattern)))
          
          ;; Only create rule if confidence is high enough
          (if (>= confidence min-confidence)
              (make-inference-rule
                (generate-rule-name pattern)
                antecedent
                consequent
                confidence)
              #f)))
      patterns)))
```

### Algorithm 3: Symbol Grounding

```scheme
(define (ground-symbol atomspace symbol modality pattern strength)
  "Ground a symbol in sensorimotor experience.
   
   Parameters:
   - atomspace: The atomspace
   - symbol: The symbolic atom to ground
   - modality: Sensory modality (visual, auditory, etc.)
   - pattern: The sensorimotor pattern
   - strength: Grounding strength (0-1)
   
   Returns: Grounding record"
  
  ;; Create grounding link
  (let* ((modality-node (create-concept-node atomspace modality))
         (pattern-node (create-concept-node atomspace 
                                           (format #f "~a-pattern" modality)))
         (grounding-link (create-evaluation-link atomspace
                                                pattern-node
                                                (list symbol modality-node))))
    
    ;; Set strength
    (link-set-tv! grounding-link (make-truth-value strength 0.9))
    
    ;; Create grounding record
    (make-symbol-grounding symbol modality pattern strength)))
```

### Algorithm 4: Hybrid Reasoning

```scheme
(define (hybrid-reason atomspace query symbolic-rules propagation-params)
  "Perform hybrid symbolic-neural reasoning.
   
   Parameters:
   - atomspace: The atomspace
   - query: The query atom
   - symbolic-rules: List of inference rules
   - propagation-params: Parameters for activation spreading
   
   Returns: Reasoning result with confidence"
  
  ;; 1. Symbolic phase: Apply inference rules
  (let ((symbolic-result (forward-chain atomspace symbolic-rules 3)))
    
    ;; 2. Neural phase: Spread activation from query
    (let ((activation-map (propagate-activation atomspace
                                               (list (cons query 1.0))
                                               propagation-params)))
      
      ;; 3. Combine results
      (let ((combined-result
              (combine-symbolic-neural symbolic-result activation-map)))
        
        ;; 4. Compute confidence
        (let ((confidence (compute-hybrid-confidence combined-result)))
          
          (cons combined-result confidence))))))
```

## Integration Points

### With Echo Propagation Engine

1. **Activation History Collection**
   - Hook into propagation to record activation patterns
   - Store timestamps, values, co-activations

2. **Guided Propagation**
   - Inject symbolic knowledge into propagation
   - Boost/inhibit based on rules

### With Symbolic Reasoning Engine

1. **Rule Injection**
   - Add discovered rules to rule base
   - Validate rules through inference

2. **Concept Integration**
   - Add formed concepts to atomspace
   - Use in symbolic reasoning

### With Hypergraph Memory

1. **Pattern Storage**
   - Store activation patterns as episodic memory
   - Query patterns for rule extraction

2. **Concept Storage**
   - Store formed concepts as declarative memory
   - Link to constituent atoms

## Performance Targets

| Operation | Target | Rationale |
|-----------|--------|-----------|
| **Concept formation** | <100ms for 1000 activations | Real-time learning |
| **Rule extraction** | <500ms for 10K patterns | Batch processing |
| **Symbol grounding** | <10ms per symbol | Interactive use |
| **Hybrid reasoning** | <200ms per query | Real-time inference |
| **Guided propagation** | <150ms per step | Real-time guidance |

## Success Criteria

1. ✅ **Concept formation works**
   - Discovers meaningful clusters
   - Creates appropriate concept nodes
   - Links to constituent atoms

2. ✅ **Rule extraction works**
   - Mines co-activation patterns
   - Generates valid inference rules
   - Validates rules through testing

3. ✅ **Symbol grounding works**
   - Connects symbols to sensorimotor patterns
   - Enables embodied simulation
   - Improves reasoning quality

4. ✅ **Hybrid reasoning works**
   - Combines symbolic and neural
   - Outperforms either alone
   - Produces calibrated confidence

5. ✅ **Guided propagation works**
   - Uses symbolic knowledge effectively
   - Improves activation spreading
   - Satisfies constraints

6. ✅ **Integration complete**
   - Works with Echo Propagation
   - Works with Symbolic Reasoning
   - Works with Hypergraph Memory

7. ✅ **Performance targets met**
   - All operations within time bounds
   - Scales to large atomspaces
   - Memory efficient

8. ✅ **Tests passing**
   - Unit tests for each component
   - Integration tests
   - End-to-end tests

## Implementation Plan

### Phase 1: Concept Formation (Current)
- Activation history tracking
- Cluster detection
- Concept node creation
- Tests

### Phase 2: Rule Extraction
- Co-activation pattern mining
- Rule generation
- Rule validation
- Tests

### Phase 3: Symbol Grounding
- Grounding mechanisms
- Sensorimotor integration
- Embodied simulation
- Tests

### Phase 4: Hybrid Reasoning
- Symbolic-neural combination
- Confidence computation
- Iterative refinement
- Tests

### Phase 5: Integration
- Echo Propagation hooks
- Symbolic Reasoning hooks
- End-to-end tests
- Performance optimization

## References

- **NARS**: Non-Axiomatic Reasoning System (concept formation)
- **CLARION**: Hybrid symbolic-connectionist architecture
- **ACT-R**: Activation-based cognitive architecture
- **Soar**: Chunking and rule learning
- **Deep Learning + Logic**: Neural-symbolic integration approaches

## Notes

- Focus on **bidirectional** translation (neural ↔ symbolic)
- Emphasize **online learning** (real-time concept/rule formation)
- Ensure **interpretability** (discovered concepts/rules are understandable)
- Maintain **efficiency** (don't slow down core reasoning)
- Enable **emergence** (let concepts/rules arise naturally from data)

---

**Status**: Design complete, ready for implementation  
**Next**: Implement concept formation from activation patterns
