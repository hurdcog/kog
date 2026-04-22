# Hypergraph Flowchart: NETWORK_SEND

## Cognitive Fragment Visualization

This diagram shows the hypergraph representation of the NETWORK_SEND primitive in the OpenCog-GNUHurd cognitive kernel integration.

```mermaid
flowchart TD
    PRIM["NETWORK_SEND Primitive"]:::primitive
    MOD["NETWORK Modality"]:::modality
    CTX["USER Context"]:::context
    ATOM1["CONCEPT: NETWORK_SEND"]:::atom
    ATOM2["CONCEPT: NETWORK"]:::atom
    ATOM3["CONCEPT: USER"]:::atom
    LINK1["INHERITANCE Link"]:::link
    LINK2["EVALUATION Link"]:::link
    TENSOR["Tensor Shape: [8,4,8,10,5]<br/>Elements: 12,800"]:::tensor
    PATTERN["Pattern Signature<br/>Salience: 5, Autonomy: 2"]:::pattern

    PRIM --> MOD
    PRIM --> CTX
    PRIM --> TENSOR
    MOD --> ATOM2
    CTX --> ATOM3
    ATOM1 --> LINK1
    ATOM2 --> LINK1
    LINK1 --> PATTERN
    LINK2 --> PATTERN
    TENSOR --> PATTERN

    classDef primitive fill:#ff9999,stroke:#333,stroke-width:2px,color:#000
    classDef modality fill:#99ccff,stroke:#333,stroke-width:2px,color:#000
    classDef context fill:#99ff99,stroke:#333,stroke-width:2px,color:#000
    classDef atom fill:#ffcc99,stroke:#333,stroke-width:1px,color:#000
    classDef link fill:#cc99ff,stroke:#333,stroke-width:1px,color:#000
    classDef tensor fill:#ffff99,stroke:#333,stroke-width:2px,color:#000
    classDef pattern fill:#ff99cc,stroke:#333,stroke-width:2px,color:#000
```

### Components:
- **Primitive Node**: The core GNUMach primitive
- **Modality Node**: Operational domain classification
- **Context Node**: Execution environment
- **Atom Nodes**: Individual AtomSpace concepts
- **Link Nodes**: Hypergraph relationships
- **Tensor Node**: Mathematical encoding information
- **Pattern Node**: Unique signature and statistics

Generated: 1754191179
