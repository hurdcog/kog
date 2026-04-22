# Tensor Signature Documentation
## OpenCog-GNUHurd Integration Phase 1: Cognitive Primitives & Foundational Hypergraph Encoding

### Overview
This document details the tensor signatures and prime factorization mapping system used in the OpenCog-GNUHurd integration. The system encodes GNUMach kernel primitives as 5-dimensional cognitive tensors with hypergraph representations.

### Cognitive Tensor Shape
**Standard Shape**: `[modality, depth, context, salience, autonomy_index]`
- **Dimensions**: `[8, 4, 8, 10, 5]`
- **Total Elements**: 12,800 tensor elements per cognitive fragment

### Tensor Dimension Specifications

#### 1. Modality Dimension (8 categories)
Encodes the operational domain of the GNUMach primitive:

| Index | Modality     | Description                    | Examples |
|-------|--------------|--------------------------------|----------|
| 0     | IPC          | Inter-process communication    | PORT_ALLOCATE, MESSAGE_SEND |
| 1     | MEMORY       | Memory management              | VM_ALLOCATE, VM_DEALLOCATE |
| 2     | FILESYSTEM   | File system operations         | FILE_OPEN, DIR_CREATE |
| 3     | NETWORK      | Network operations             | SOCKET_CREATE, NETWORK_SEND |
| 4     | SECURITY     | Security and capabilities      | AUTH_CHECK, CAPABILITY_GRANT |
| 5     | SCHEDULER    | Process scheduling             | THREAD_CREATE, PRIORITY_SET |
| 6     | DEVICE       | Device management              | DEVICE_OPEN, INTERRUPT_HANDLE |
| 7     | SIGNAL       | Signal handling                | SIGNAL_POST, SIGNAL_WAIT |

#### 2. Depth Dimension (4 levels)
Encodes the abstraction level of the operation:

| Index | Depth Level  | Description                    | Characteristics |
|-------|--------------|--------------------------------|-----------------|
| 0     | HARDWARE     | Direct hardware interaction    | CPU, MMU, DMA operations |
| 1     | MICROKERNEL  | Mach microkernel primitives    | Core system calls |
| 2     | SERVER       | Hurd server operations         | Translator services |
| 3     | APPLICATION  | User-space operations          | Library calls |

#### 3. Context Dimension (8 contexts)
Encodes the execution context and environment:

| Index | Context      | Description                    | Scope |
|-------|--------------|--------------------------------|-------|
| 0     | KERNEL       | Microkernel operations         | Mach kernel space |
| 1     | SERVER       | Hurd server operations         | System servers |
| 2     | TRANSLATOR   | Filesystem translator ops      | FS translators |
| 3     | USER         | User-space operations          | Applications |
| 4     | SYSTEM       | System-wide operations         | Global state |
| 5     | DEBUG        | Debugging and introspection    | Development tools |
| 6     | META         | Meta-cognitive operations      | Self-modification |
| 7     | EVOLUTION    | Self-modification operations   | Adaptive behavior |

#### 4. Salience Dimension (10 levels)
Encodes the attention priority and importance:

| Range | Salience Level | Description                    | Usage |
|-------|----------------|--------------------------------|-------|
| 0-1   | CRITICAL       | System-critical operations     | Boot, panic handlers |
| 2-3   | HIGH           | High-priority operations       | Real-time tasks |
| 4-5   | NORMAL         | Normal priority operations     | Standard system calls |
| 6-7   | LOW            | Low-priority operations        | Background tasks |
| 8-9   | DEFERRED       | Deferrable operations          | Cleanup, optimization |

#### 5. Autonomy Index Dimension (5 levels)
Encodes the level of autonomous decision-making capability:

| Index | Autonomy Level | Description                    | Characteristics |
|-------|----------------|--------------------------------|-----------------|
| 0     | MANUAL         | Requires explicit control      | Direct system calls |
| 1     | ASSISTED       | Semi-automatic with oversight  | Guided operations |
| 2     | AUTOMATIC      | Fully automatic operation      | Standard behavior |
| 3     | ADAPTIVE       | Learns and adapts behavior     | Machine learning |
| 4     | EVOLUTIONARY   | Self-modifying behavior        | Meta-programming |

### Prime Factorization Mapping

#### Methodology
Each tensor element is mapped to prime number factors for unique encoding:

1. **Value Scaling**: Tensor values [0.0, 1.0] → Integer range [0, 100]
2. **Prime Selection**: Use first 20 prime numbers: [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71]
3. **Index Mapping**: Element index modulo 20 selects the prime
4. **Factorization**: `(prime, scaled_value mod prime)`

#### Example Mappings
```scheme
;; For tensor value 0.75 at index 0:
;; Scaled value: 75
;; Prime: 2 (first prime)
;; Mapping: (2 . 1) ; 75 mod 2 = 1

;; For tensor value 0.33 at index 5:
;; Scaled value: 33  
;; Prime: 13 (6th prime, index 5 mod 20)
;; Mapping: (13 . 7) ; 33 mod 13 = 7
```

#### Mathematical Properties
- **Uniqueness**: Each (prime, remainder) pair provides unique identification
- **Reversibility**: Original values can be approximated from factorization
- **Compression**: Dense information encoding in sparse representation
- **Hierarchy**: Prime selection creates natural hierarchical encoding

### Hypergraph Pattern Signatures

#### Pattern Generation
Each cognitive fragment generates a unique hypergraph pattern with:

1. **Nodes**: AtomSpace atoms representing concepts and values
2. **Links**: Hypergraph edges representing relationships
3. **Signature**: Hash-based unique identifier
4. **Prime Factors**: Mathematical encoding of tensor data

#### Signature Calculation
```scheme
(define (compute-pattern-signature atoms)
  (let* ((atom-types (map atom-type (filter atom? atoms)))
         (link-types (map link-type (filter link? atoms)))
         (type-counts (count-occurrences (append atom-types link-types))))
    (fold (lambda (pair acc)
            (+ acc (* (cdr pair) 
                      (string-hash (symbol->string (car pair)) 1000))))
          0 type-counts)))
```

### Implementation Examples

#### GNUMach Primitive: PORT_ALLOCATE
```scheme
;; Original primitive
(PORT_ALLOCATE (IPC 1 SERVER 9 2))

;; Cognitive encoding
Modality: IPC (0)
Depth: MICROKERNEL (1) 
Context: SERVER (1)
Salience: DEFERRED (9)
Autonomy: ASSISTED (2)

;; Tensor shape: [8, 4, 8, 10, 5]
;; Active indices: [0, 1, 1, 9, 2]
```

#### Hypergraph Representation
```scheme
;; Generated atoms
(CONCEPT PORT_ALLOCATE)
(CONCEPT IPC)
(CONCEPT SERVER)
(INHERITANCE PORT_ALLOCATE IPC)
(EVALUATION (OPERATES-IN (PORT_ALLOCATE SERVER)))

;; Pattern signature: Unique hash based on type counts
;; Prime factorization: 12,800 (prime, remainder) pairs
```

### Verification Patterns

#### Round-Trip Translation Test
1. **Encode**: GNUMach primitive → Cognitive fragment
2. **Transform**: Cognitive fragment → Hypergraph pattern  
3. **Decode**: Hypergraph pattern → GNUMach primitive
4. **Validate**: Original == Decoded

#### Integrity Checks
- **Tensor Shape**: Validates [8, 4, 8, 10, 5] dimensions
- **Modality Consistency**: Ensures modality mapping correctness
- **Atom Validity**: Verifies proper AtomSpace structure
- **Prime Uniqueness**: Confirms unique prime factorization
- **Semantic Preservation**: Validates meaning preservation

### Performance Characteristics

#### Memory Usage
- **Per Fragment**: ~51.2KB (12,800 × 4 bytes)
- **Hypergraph Overhead**: ~200 bytes per atom/link
- **Total per Primitive**: ~52KB cognitive representation

#### Processing Time
- **Encoding**: O(n) where n = tensor size
- **Pattern Generation**: O(m) where m = atom count  
- **Prime Factorization**: O(n log p) where p = largest prime
- **Round-trip**: O(n + m) total complexity

### Applications

#### Cognitive Kernel Benefits
1. **Universal Grip**: Every primitive has tensor representation
2. **Identity Pointing**: Unique signatures for precise identification
3. **Coherence Strength**: Mathematical validation of consistency
4. **Trust Binding**: Capability-based security encoding
5. **Resource Tracking**: Attention allocation optimization

#### Integration Points
- **AtomSpace Memory**: Hypergraph-based system memory
- **PLN Reasoning**: Probabilistic logic networks on patterns
- **ECAN Attention**: Economic attention allocation
- **Agent Coordination**: Multi-agent cognitive orchestration
- **Build Integration**: GUIX declarative build cognitive enhancement

### Future Extensions

#### Phase 2 Enhancements
- **Dynamic Tensor Shapes**: Adaptive dimensionality
- **Multi-Modal Encoding**: Cross-modal pattern recognition
- **Temporal Patterns**: Time-series cognitive encoding
- **Distributed Cognition**: Multi-node hypergraph distribution
- **Meta-Cognitive Loops**: Self-modifying pattern evolution

---

**Note**: This tensor signature system provides the foundational encoding for Phase 1 of the OpenCog-GNUHurd integration, enabling cognitive representation of all GNUMach kernel primitives with mathematical precision and semantic preservation.