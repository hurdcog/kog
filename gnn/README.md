# OpenCog GNN Module

Graph Neural Network integration for AtomSpace knowledge graphs.

## Overview

This module provides neural network processing capabilities for OpenCog's hypergraph-based knowledge representation system, enabling:

- **Graph Extraction**: Extract graph structures from AtomSpace
- **Node Embedding**: Compute vector representations of atoms
- **Link Prediction**: Discover potential relationships in knowledge graphs
- **Pattern Recognition**: Identify structural patterns using neural networks
- **Self-Supervised Learning**: Train without explicit labels using link prediction

## Architecture

Based on WinKoGNN architecture, adapted for OCC integration with:

- CogGML shard coordination support
- Autognostic diagnostic integration
- Multiple AtomSpace backend support (mock, Cython, gRPC, CogServer)

```
┌─────────────────────────────────────────────────────────────┐
│                    GNN Module                                │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │  AtomSpace   │  │    Graph     │  │     GNN      │      │
│  │   Bridge     │◄─┤  Structure   │◄─┤   Layers     │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│         │                  │                                 │
│         ▼                  ▼                                 │
│  ┌──────────────┐  ┌──────────────┐                        │
│  │  C++ Atom    │  │  Embeddings  │                        │
│  │   Space      │  │   & Links    │                        │
│  └──────────────┘  └──────────────┘                        │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

## Installation

```bash
pip install numpy
```

## Quick Start

```python
from gnn import GraphNeuralNetwork, AtomSpaceBridge

# Create AtomSpace bridge
bridge = AtomSpaceBridge(backend='mock')
bridge.create_sample_knowledge_graph()

# Create and configure GNN
gnn = GraphNeuralNetwork({
    'input_dim': 128,
    'hidden_dim': 256,
    'output_dim': 64,
    'num_layers': 3
})
gnn.set_atomspace_bridge(bridge)

# Extract graph and train
atomspace_data = bridge.extract_atoms()
graph = gnn.extract_graph(atomspace_data)
results = gnn.train(graph, epochs=10)

# Predict links
predictions = gnn.predict_links(threshold=0.5)
for source, target, score in predictions[:5]:
    print(f"{source} -> {target}: {score:.3f}")
```

## Components

### GraphNeuralNetwork

Main GNN class with multi-layer message passing.

```python
gnn = GraphNeuralNetwork(config)
gnn.forward_pass(graph)          # Compute embeddings
gnn.train(graph, epochs=10)      # Train the network
gnn.predict_links(threshold=0.5) # Predict new links
gnn.compute_similarity(a, b)     # Node similarity
```

### AtomSpaceBridge

Bridge between Python GNN and C++ AtomSpace.

```python
bridge = AtomSpaceBridge(backend='mock')  # or 'cython', 'grpc', 'cogserver'
bridge.add_atom('ConceptNode', 'name', truth_value=0.9)
bridge.create_link('InheritanceLink', source_id, target_id)
bridge.extract_atoms()
```

### GraphStructure

Represents graph data for GNN processing.

```python
graph = GraphStructure(
    nodes=['a', 'b', 'c'],
    edges=[('a', 'b'), ('b', 'c')],
    node_features={'a': np.array([...]), ...}
)
```

## Configuration Options

| Parameter | Default | Description |
|-----------|---------|-------------|
| input_dim | 128 | Input feature dimension |
| hidden_dim | 256 | Hidden layer dimension |
| output_dim | 64 | Output embedding dimension |
| num_layers | 3 | Number of GNN layers |
| aggregation | 'mean' | Neighbor aggregation method |
| learning_rate | 0.01 | Training learning rate |

## Autognostic Integration

The GNN module supports diagnostic atom creation for autognostic integration:

```python
# GNN automatically creates diagnostic atoms during training
diagnostics = gnn.get_diagnostics()
for diag in diagnostics:
    print(f"Type: {diag['type']}, Data: {diag['data']}")

# Manual diagnostic creation via bridge
bridge.add_diagnostic_atom('error', {
    'severity_score': 0.8,
    'message': 'Training loss too high'
})
```

## Testing

```bash
cd gnn
python -m unittest tests.test_gnn -v
```

## Integration with CogGML

The GNN module can be integrated with CogGML shards:

```python
# Set callbacks for shard coordination
bridge.set_callbacks(
    on_added=lambda id, data: shard.notify_atom_added(id),
    on_removed=lambda id: shard.notify_atom_removed(id)
)
```

## License

See [LICENSE](../LICENSE) in the root directory.

## References

- [WinKoGNN](https://github.com/cogpy/WinKoGNN) - Original architecture
- [OpenCog AtomSpace](https://github.com/opencog/atomspace) - Knowledge representation
- [Graph Neural Networks](https://arxiv.org/abs/1901.00596) - GNN survey
