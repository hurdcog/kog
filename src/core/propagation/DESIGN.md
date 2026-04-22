# Echo Propagation Engine - Design Document

**Component**: Core Engine - Echo Propagation  
**Milestone**: Phase 1, Milestone 1.3  
**Status**: Design Phase  
**Date**: November 2, 2025

## Overview

The Echo Propagation Engine implements spreading activation, pattern recognition, and feedback loops for the Deep Tree Echo Architecture. It operates on the Hypergraph Memory Space, propagating activation through the network to enable associative memory, pattern matching, and emergent cognitive behavior.

## Theoretical Foundation

### Spreading Activation

Spreading activation is a cognitive model where activation spreads from source nodes through connected nodes in a network. The model is inspired by:

**Neural Activation**: Biological neurons fire and propagate signals to connected neurons  
**Associative Memory**: Related concepts become activated together  
**Priming Effects**: Prior activation influences subsequent processing  
**Semantic Networks**: Meaning emerges from network structure and activation patterns

### Key Principles

**1. Activation as Energy**: Activation represents cognitive "energy" or "attention" flowing through the network

**2. Decay Over Distance**: Activation decreases as it spreads through the network

**3. Summation**: Multiple activation sources combine at nodes

**4. Threshold**: Nodes activate when total activation exceeds a threshold

**5. Feedback**: Activation can flow bidirectionally, creating feedback loops

## Architecture

### Components

```
Echo Propagation Engine
├── Spreading Activation
│   ├── Activation State (per atom)
│   ├── Propagation Algorithm
│   ├── Decay Function
│   └── Normalization
├── Pattern Recognition
│   ├── Pattern Matcher
│   ├── Template Library
│   ├── Similarity Metrics
│   └── Pattern Extraction
└── Feedback Loops
    ├── Reinforcement Learning
    ├── Hebbian Learning
    ├── Attention Modulation
    └── Truth Value Updates
```

### Data Structures

#### Activation State

Each atom in the AtomSpace has an associated activation state:

```scheme
(define-record-type <activation-state>
  (activation        ; Current activation level [0.0, 1.0]
   prev-activation   ; Previous activation (for delta calculation)
   sources           ; List of (source-uuid . contribution) pairs
   timestamp))       ; Last update timestamp
```

#### Propagation Parameters

Global parameters controlling propagation behavior:

```scheme
(define-record-type <propagation-params>
  (decay-rate        ; Activation decay per hop [0.0, 1.0]
   threshold         ; Minimum activation to propagate [0.0, 1.0]
   max-hops          ; Maximum propagation distance
   normalization     ; Normalization strategy ('sum, 'max, 'softmax)
   bidirectional?    ; Allow backward propagation through links
   use-truth-values? ; Weight propagation by truth values
   use-attention?))  ; Weight propagation by attention values
```

## Algorithms

### 1. Spreading Activation Algorithm

**Input**: Source atoms with initial activation levels  
**Output**: Activation levels for all reachable atoms

**Algorithm** (Breadth-First Spreading):

```
1. Initialize:
   - Set activation for source atoms
   - Create priority queue ordered by activation
   - Mark all atoms as unvisited

2. While queue is not empty:
   a. Dequeue atom with highest activation
   b. If atom visited or activation < threshold: skip
   c. Mark atom as visited
   
   d. For each outgoing link from atom:
      - Calculate activation to propagate:
        propagated = current_activation * decay_rate
      - If use_truth_values:
        propagated *= link_truth_strength
      - If use_attention:
        propagated *= normalize(link_sti)
      
      - For each target atom in link:
        - Add propagated activation to target
        - Enqueue target if activation increased
   
   e. If bidirectional, repeat for incoming links

3. Normalize all activations (optional)

4. Return activation map
```

**Time Complexity**: O(V + E) where V = atoms, E = links  
**Space Complexity**: O(V) for activation map

### 2. Pattern Recognition Algorithm

**Input**: Query pattern (subgraph), target graph (AtomSpace)  
**Output**: List of matching subgraphs with similarity scores

**Algorithm** (Subgraph Isomorphism with Fuzzy Matching):

```
1. Extract pattern structure:
   - Node types and names
   - Link types and connectivity
   - Truth value constraints
   - Variable bindings

2. Generate candidate matches:
   - Find atoms matching pattern node types
   - Filter by name constraints (exact or fuzzy)
   - Filter by truth value ranges

3. For each candidate root:
   a. Attempt to match pattern recursively:
      - Match link types and connectivity
      - Bind variables consistently
      - Check truth value constraints
   
   b. Calculate similarity score:
      - Structural similarity (topology match)
      - Semantic similarity (truth values, names)
      - Attention-weighted similarity
   
   c. If score > threshold, add to results

4. Rank results by similarity score

5. Return top-k matches
```

**Time Complexity**: O(V^k) where k = pattern size (NP-complete)  
**Optimization**: Use indexing and pruning heuristics

### 3. Feedback Loop Mechanisms

#### Hebbian Learning

"Neurons that fire together, wire together"

```scheme
;; Update link truth value based on co-activation
(define (hebbian-update link source-activation target-activation learning-rate)
  (let* ((current-tv (link-tv link))
         (current-strength (tv-strength current-tv))
         (delta (* learning-rate source-activation target-activation))
         (new-strength (+ current-strength delta)))
    (make-truth-value 
      #:strength (clamp new-strength 0.0 1.0)
      #:confidence (tv-confidence current-tv)
      #:count (+ (tv-count current-tv) 1))))
```

#### Attention Modulation

Activated atoms gain attention (STI), inactive atoms lose attention

```scheme
;; Update attention value based on activation
(define (attention-update atom activation attention-gain attention-decay)
  (let* ((current-av (atom-av atom))
         (current-sti (av-sti current-av))
         (delta (if (> activation 0.5)
                   (* attention-gain activation)
                   (* attention-decay (- activation 0.5))))
         (new-sti (+ current-sti delta)))
    (make-attention-value
      #:sti (clamp new-sti -1000 1000)
      #:lti (av-lti current-av)
      #:vlti (av-vlti current-av))))
```

#### Reinforcement Learning

Reward/punish patterns based on outcomes

```scheme
;; Update truth values based on reward signal
(define (reinforcement-update pattern reward learning-rate)
  (for-each
    (lambda (atom-or-link)
      (let* ((current-tv (get-tv atom-or-link))
             (current-strength (tv-strength current-tv))
             (delta (* learning-rate reward))
             (new-strength (+ current-strength delta)))
        (set-tv! atom-or-link
          (make-truth-value
            #:strength (clamp new-strength 0.0 1.0)
            #:confidence (tv-confidence current-tv)
            #:count (+ (tv-count current-tv) 1)))))
    pattern))
```

## Integration with AtomSpace

### Activation Storage

Activation states are stored separately from the AtomSpace to avoid polluting the knowledge graph:

```scheme
;; Activation map: UUID -> activation-state
(define activation-map (make-hash-table))

;; Get activation for atom
(define (get-activation atom)
  (hash-table-ref/default activation-map (atom-uuid atom) 0.0))

;; Set activation for atom
(define (set-activation! atom value)
  (hash-table-set! activation-map (atom-uuid atom) value))
```

### Propagation Triggers

Propagation can be triggered by:

1. **Explicit Activation**: User activates specific atoms
2. **Query Processing**: Queries activate relevant atoms
3. **Perception**: New sensory input activates atoms
4. **Goal Activation**: Active goals activate related atoms
5. **Periodic Refresh**: Background activation maintenance

### Performance Considerations

**Incremental Propagation**: Only propagate changed activations  
**Lazy Evaluation**: Compute activation on-demand  
**Caching**: Cache frequently accessed activation patterns  
**Pruning**: Stop propagation below threshold  
**Parallelization**: Propagate multiple sources in parallel

## API Design

### Core Functions

```scheme
;; Spread activation from source atoms
(propagate-activation atomspace sources params)
  ;; sources: list of (atom . initial-activation) pairs
  ;; params: propagation-params record
  ;; returns: activation-map (uuid -> activation)

;; Find patterns matching template
(find-patterns atomspace pattern params)
  ;; pattern: pattern template (subgraph)
  ;; params: pattern-matching-params
  ;; returns: list of (match . similarity-score) pairs

;; Apply Hebbian learning to co-activated atoms
(hebbian-learn atomspace activation-map learning-rate)
  ;; Updates truth values of links between activated atoms

;; Update attention based on activation
(update-attention atomspace activation-map params)
  ;; Updates STI based on activation levels

;; Reinforce/punish pattern based on reward
(reinforce-pattern atomspace pattern reward learning-rate)
  ;; Updates truth values of atoms/links in pattern
```

### Usage Examples

```scheme
;; Example 1: Spread activation from "Cat"
(define cat (get-atoms-by-name as "Cat"))
(define activation-map 
  (propagate-activation as 
    (list (cons cat 1.0))
    (make-propagation-params 
      #:decay-rate 0.8
      #:threshold 0.1
      #:max-hops 3)))

;; Example 2: Find patterns similar to "X is-a Animal"
(define pattern
  (make-pattern
    (list (make-variable-node "$X")
          (make-concept-node "Animal"))
    (make-inheritance-link (var "$X") (concept "Animal"))))

(define matches (find-patterns as pattern default-params))

;; Example 3: Hebbian learning
(hebbian-learn as activation-map 0.01)

;; Example 4: Attention update
(update-attention as activation-map default-attention-params)
```

## Testing Strategy

### Unit Tests

1. **Activation Propagation**
   - Single-hop propagation
   - Multi-hop propagation with decay
   - Bidirectional propagation
   - Threshold cutoff
   - Truth value weighting
   - Attention value weighting

2. **Pattern Matching**
   - Exact pattern matching
   - Fuzzy pattern matching
   - Variable binding
   - Similarity scoring
   - Performance on large graphs

3. **Feedback Loops**
   - Hebbian learning updates
   - Attention modulation
   - Reinforcement learning
   - Convergence behavior

### Integration Tests

1. **Memory + Propagation**
   - Activation spreads through AtomSpace
   - Patterns emerge from activation
   - Learning modifies graph structure

2. **Performance Tests**
   - Propagation on 1000+ atom graphs
   - Pattern matching on complex patterns
   - Learning convergence speed

### Benchmarks

| Operation | Target | Measurement |
|-----------|--------|-------------|
| Propagate 1000 atoms | <100ms | Avg time for full propagation |
| Find pattern (size 5) | <50ms | Avg time to find all matches |
| Hebbian update | <10ms | Time to update 100 links |
| Attention update | <10ms | Time to update 1000 atoms |

## Success Criteria

Milestone 1.3 is complete when:

- ✅ Spreading activation algorithm implemented and tested
- ✅ Pattern recognition framework operational
- ✅ Feedback loops (Hebbian, attention, reinforcement) working
- ✅ Integration with AtomSpace seamless
- ✅ All unit tests passing (>80% coverage)
- ✅ Performance benchmarks met
- ✅ Documentation complete
- ✅ Examples demonstrating key features

## Future Enhancements

**Phase 2+**:
- Temporal dynamics (activation over time)
- Oscillatory patterns (neural rhythms)
- Attention competition (winner-take-all)
- Meta-learning (learning to learn)
- Distributed propagation (multi-node)

## References

- **Spreading Activation Theory**: Collins & Loftus (1975)
- **Hebbian Learning**: Hebb (1949)
- **Pattern Recognition**: Mitchell (1997)
- **OpenCog AtomSpace**: Goertzel et al. (2014)
- **Neural-Symbolic Integration**: Garcez et al. (2019)

---

**Next Steps**: Implement spreading activation algorithm in `spreading.scm`
