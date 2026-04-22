"""
OpenCog GNN Module
Graph Neural Network integration for AtomSpace knowledge graphs

This module provides neural network processing capabilities for OpenCog's
hypergraph-based knowledge representation system, enabling:
- Graph extraction from AtomSpace
- Node embedding computation
- Link prediction
- Pattern recognition
- Self-supervised learning

Based on WinKoGNN architecture, adapted for OCC integration.
"""

from .graph_network import GraphNeuralNetwork, GraphStructure, GNNLayer
from .atomspace_bridge import AtomSpaceBridge

__version__ = "0.1.0"
__all__ = [
    "GraphNeuralNetwork",
    "GraphStructure", 
    "GNNLayer",
    "AtomSpaceBridge"
]
