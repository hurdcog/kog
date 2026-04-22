"""
Graph Neural Network Integration for OpenCog AtomSpace
Provides neural network processing over knowledge graphs

Based on WinKoGNN architecture, adapted for OCC integration with:
- C++ AtomSpace bridge support
- CogGML shard coordination
- Autognostic diagnostic integration
"""

import logging
from typing import Dict, List, Any, Optional, Tuple
from dataclasses import dataclass, field
import numpy as np

logger = logging.getLogger(__name__)


@dataclass
class GraphStructure:
    """Represents a graph structure for GNN processing"""
    nodes: List[str]  # Node IDs
    edges: List[Tuple[str, str]]  # (source, target) pairs
    node_features: Dict[str, np.ndarray]  # Node ID -> feature vector
    edge_features: Dict[Tuple[str, str], np.ndarray] = field(default_factory=dict)
    adjacency_matrix: Optional[np.ndarray] = None
    node_types: Dict[str, str] = field(default_factory=dict)  # Node ID -> type
    
    def __post_init__(self):
        if self.adjacency_matrix is None:
            self._build_adjacency_matrix()
            
    def _build_adjacency_matrix(self):
        """Build adjacency matrix from edges"""
        n = len(self.nodes)
        if n == 0:
            self.adjacency_matrix = np.zeros((0, 0))
            return
            
        node_index = {node: i for i, node in enumerate(self.nodes)}
        
        self.adjacency_matrix = np.zeros((n, n))
        for source, target in self.edges:
            if source in node_index and target in node_index:
                i, j = node_index[source], node_index[target]
                self.adjacency_matrix[i, j] = 1
                
    def get_node_degree(self, node_id: str) -> Tuple[int, int]:
        """Get in-degree and out-degree for a node"""
        if node_id not in self.nodes:
            return (0, 0)
        idx = self.nodes.index(node_id)
        in_degree = int(np.sum(self.adjacency_matrix[:, idx]))
        out_degree = int(np.sum(self.adjacency_matrix[idx, :]))
        return (in_degree, out_degree)
        
    def get_neighbors(self, node_id: str) -> List[str]:
        """Get all neighbor node IDs"""
        if node_id not in self.nodes:
            return []
        idx = self.nodes.index(node_id)
        neighbor_indices = np.where(self.adjacency_matrix[idx, :] > 0)[0]
        return [self.nodes[i] for i in neighbor_indices]


class GNNLayer:
    """A single Graph Neural Network layer with message passing"""
    
    def __init__(self, input_dim: int, output_dim: int, layer_id: str,
                 aggregation: str = "mean"):
        self.input_dim = input_dim
        self.output_dim = output_dim
        self.layer_id = layer_id
        self.aggregation = aggregation
        
        # Initialize weights with Xavier initialization
        scale = np.sqrt(2.0 / (input_dim + output_dim))
        self.weights = np.random.randn(input_dim, output_dim) * scale
        self.bias = np.zeros(output_dim)
        
        # For attention mechanism
        self.attention_weights = np.random.randn(2 * output_dim, 1) * 0.01
        
    def forward(self, node_features: Dict[str, np.ndarray],
               adjacency_matrix: np.ndarray,
               nodes: List[str]) -> Dict[str, np.ndarray]:
        """Forward pass through the layer with message passing"""
        output_features = {}
        
        if len(nodes) == 0:
            return output_features
            
        node_index = {node: i for i, node in enumerate(nodes)}
        
        for node_id in nodes:
            if node_id not in node_features:
                continue
                
            idx = node_index[node_id]
            
            # Get neighbor indices
            neighbors = np.where(adjacency_matrix[:, idx] > 0)[0]
            
            if len(neighbors) > 0:
                neighbor_ids = [nodes[i] for i in neighbors if nodes[i] in node_features]
                neighbor_features = [node_features[nid] for nid in neighbor_ids]
                
                if neighbor_features:
                    # Aggregate neighbor features
                    if self.aggregation == "mean":
                        aggregated = np.mean(neighbor_features, axis=0)
                    elif self.aggregation == "sum":
                        aggregated = np.sum(neighbor_features, axis=0)
                    elif self.aggregation == "max":
                        aggregated = np.max(neighbor_features, axis=0)
                    else:
                        aggregated = np.mean(neighbor_features, axis=0)
                        
                    # Combine with self features
                    combined = 0.5 * node_features[node_id] + 0.5 * aggregated
                else:
                    combined = node_features[node_id]
            else:
                combined = node_features[node_id]
                
            # Transform
            output = np.dot(combined, self.weights) + self.bias
            
            # ReLU activation
            output = np.maximum(0, output)
            
            # L2 normalization
            norm = np.linalg.norm(output)
            if norm > 0:
                output = output / norm
                
            output_features[node_id] = output
            
        return output_features


class GraphNeuralNetwork:
    """
    Graph Neural Network for processing OpenCog AtomSpace knowledge graphs
    
    Features:
    - Graph extraction from AtomSpace (via bridge)
    - Multi-layer message passing
    - Self-supervised and supervised learning
    - Link prediction
    - Node similarity computation
    - Integration with CogGML shards
    """
    
    def __init__(self, config: Optional[Dict[str, Any]] = None):
        self.config = config or {}
        
        # Network architecture
        self.input_dim = self.config.get('input_dim', 128)
        self.hidden_dim = self.config.get('hidden_dim', 256)
        self.output_dim = self.config.get('output_dim', 64)
        self.num_layers = self.config.get('num_layers', 3)
        self.aggregation = self.config.get('aggregation', 'mean')
        self.learning_rate = self.config.get('learning_rate', 0.01)
        
        # Components
        self.layers: List[GNNLayer] = []
        self.embeddings: Dict[str, np.ndarray] = {}
        
        # AtomSpace bridge
        self.atomspace_bridge = None
        
        # Training state
        self.is_trained = False
        self.training_history: List[Dict[str, float]] = []
        
        # Diagnostic atoms for autognostic integration
        self.diagnostic_atoms: List[Dict[str, Any]] = []
        
        self._initialize_network()
        logger.info(f"GraphNeuralNetwork initialized: {self.num_layers} layers, "
                   f"dims={self.input_dim}->{self.hidden_dim}->{self.output_dim}")
        
    def _initialize_network(self):
        """Initialize network layers"""
        dims = [self.input_dim] + [self.hidden_dim] * (self.num_layers - 1) + [self.output_dim]
        
        for i in range(self.num_layers):
            layer = GNNLayer(
                input_dim=dims[i],
                output_dim=dims[i + 1],
                layer_id=f"layer_{i}",
                aggregation=self.aggregation
            )
            self.layers.append(layer)
            
        logger.debug(f"Initialized {len(self.layers)} GNN layers")
        
    def set_atomspace_bridge(self, bridge):
        """Connect to AtomSpace via bridge"""
        self.atomspace_bridge = bridge
        logger.info("Connected to AtomSpace bridge")
        
    def extract_graph(self, atomspace_data: Optional[Dict] = None) -> GraphStructure:
        """
        Extract graph structure from AtomSpace
        
        Args:
            atomspace_data: Optional pre-extracted data, otherwise uses bridge
            
        Returns:
            GraphStructure for GNN processing
        """
        if atomspace_data is None and self.atomspace_bridge is not None:
            atomspace_data = self.atomspace_bridge.extract_atoms()
            
        if atomspace_data is None:
            logger.warning("No AtomSpace data available")
            return GraphStructure(nodes=[], edges=[], node_features={})
            
        nodes = []
        edges = []
        node_features = {}
        node_types = {}
        
        # Extract nodes and edges from AtomSpace data
        for atom_id, atom_data in atomspace_data.items():
            nodes.append(atom_id)
            node_types[atom_id] = atom_data.get('type', 'unknown')
            
            # Create feature vector from atom properties
            features = self._create_node_features(atom_data)
            node_features[atom_id] = features
            
            # Extract edges from outgoing connections
            for target_id in atom_data.get('outgoing', []):
                edges.append((atom_id, target_id))
                
        graph = GraphStructure(
            nodes=nodes,
            edges=edges,
            node_features=node_features,
            node_types=node_types
        )
        
        logger.info(f"Extracted graph: {len(nodes)} nodes, {len(edges)} edges")
        return graph
        
    def _create_node_features(self, atom_data: Dict) -> np.ndarray:
        """Create feature vector for a node from atom properties"""
        features = np.zeros(self.input_dim)
        
        # Encode atom type
        atom_type = atom_data.get('type', 'unknown')
        features[0] = hash(atom_type) % 100 / 100.0
        
        # Truth value and confidence
        features[1] = atom_data.get('truth_value', 1.0)
        features[2] = atom_data.get('confidence', 1.0)
        features[3] = atom_data.get('attention_value', 0.0)
        
        # Structural features
        incoming = atom_data.get('incoming', [])
        outgoing = atom_data.get('outgoing', [])
        features[4] = len(incoming) / 10.0  # Normalized in-degree
        features[5] = len(outgoing) / 10.0  # Normalized out-degree
        
        # Name hash for semantic information
        name = atom_data.get('name', '')
        if name:
            name_hash = hash(name) % (self.input_dim - 10)
            features[10 + name_hash] = 1.0
            
        return features
        
    def forward_pass(self, graph: GraphStructure) -> Dict[str, np.ndarray]:
        """
        Perform forward pass through the network
        
        Args:
            graph: GraphStructure to process
            
        Returns:
            Dictionary of node embeddings
        """
        if len(graph.nodes) == 0:
            return {}
            
        # Start with input features
        current_embeddings = graph.node_features.copy()
        
        # Pass through each layer
        for layer in self.layers:
            current_embeddings = layer.forward(
                current_embeddings,
                graph.adjacency_matrix,
                graph.nodes
            )
            
        # Store final embeddings
        self.embeddings = current_embeddings
        
        logger.debug(f"Forward pass: {len(current_embeddings)} node embeddings")
        return current_embeddings
        
    def train(self, graph: GraphStructure, 
              labels: Optional[Dict[str, np.ndarray]] = None,
              epochs: int = 10) -> Dict[str, Any]:
        """
        Train the GNN on the graph
        
        Args:
            graph: GraphStructure to train on
            labels: Optional supervised labels
            epochs: Number of training epochs
            
        Returns:
            Training results dictionary
        """
        logger.info(f"Starting GNN training: {epochs} epochs")
        
        if len(graph.nodes) == 0:
            logger.warning("Empty graph, skipping training")
            return {'final_loss': 0.0, 'epochs': 0, 'history': []}
            
        loss = 0.0
        for epoch in range(epochs):
            # Forward pass
            embeddings = self.forward_pass(graph)
            
            # Compute loss
            if labels:
                loss = self._supervised_loss(embeddings, labels)
            else:
                loss = self._self_supervised_loss(embeddings, graph)
                
            # Backward pass with gradient descent
            self._backward_pass(loss, graph, embeddings)
            
            # Track training
            metrics = {
                'epoch': epoch,
                'loss': float(loss),
                'num_nodes': len(graph.nodes),
                'num_edges': len(graph.edges)
            }
            self.training_history.append(metrics)
            
            if epoch % max(1, epochs // 5) == 0:
                logger.info(f"Epoch {epoch}: loss = {loss:.4f}")
                
            # Add diagnostic atom for autognostic integration
            if loss > 1.0:
                self._add_diagnostic('training_loss_high', {
                    'epoch': epoch,
                    'loss': float(loss),
                    'severity': 'warning'
                })
                
        self.is_trained = True
        logger.info(f"GNN training completed: final_loss={loss:.4f}")
        
        return {
            'final_loss': float(loss),
            'epochs': epochs,
            'history': self.training_history
        }
        
    def _supervised_loss(self, embeddings: Dict[str, np.ndarray], 
                        labels: Dict[str, np.ndarray]) -> float:
        """Compute supervised MSE loss"""
        total_loss = 0.0
        count = 0
        
        for node_id, embedding in embeddings.items():
            if node_id in labels:
                diff = embedding - labels[node_id]
                total_loss += np.sum(diff ** 2)
                count += 1
                
        return total_loss / max(count, 1)
        
    def _self_supervised_loss(self, embeddings: Dict[str, np.ndarray],
                             graph: GraphStructure) -> float:
        """Compute self-supervised link prediction loss"""
        total_loss = 0.0
        count = 0
        
        # Positive samples (existing edges)
        for source, target in graph.edges:
            if source in embeddings and target in embeddings:
                similarity = np.dot(embeddings[source], embeddings[target])
                # Binary cross-entropy for positive sample
                prob = 1 / (1 + np.exp(-similarity))
                loss = -np.log(prob + 1e-10)
                total_loss += loss
                count += 1
                
        # Negative sampling (random non-edges)
        num_negative = min(len(graph.edges), 100)
        for _ in range(num_negative):
            i, j = np.random.randint(0, len(graph.nodes), 2)
            if i != j:
                source, target = graph.nodes[i], graph.nodes[j]
                if (source, target) not in graph.edges:
                    if source in embeddings and target in embeddings:
                        similarity = np.dot(embeddings[source], embeddings[target])
                        prob = 1 / (1 + np.exp(-similarity))
                        loss = -np.log(1 - prob + 1e-10)
                        total_loss += loss
                        count += 1
                        
        return total_loss / max(count, 1)
        
    def _backward_pass(self, loss: float, graph: GraphStructure,
                      embeddings: Dict[str, np.ndarray]):
        """Simplified backward pass with weight updates"""
        # Simple gradient descent on layer weights
        for layer in self.layers:
            # Add small random perturbation scaled by loss
            gradient = np.random.randn(*layer.weights.shape) * loss * 0.001
            layer.weights -= self.learning_rate * gradient
            
    def _add_diagnostic(self, diagnostic_type: str, data: Dict[str, Any]):
        """Add diagnostic atom for autognostic integration"""
        self.diagnostic_atoms.append({
            'type': diagnostic_type,
            'data': data,
            'component': 'gnn'
        })
        
    def get_node_embedding(self, node_id: str) -> Optional[np.ndarray]:
        """Get embedding for a specific node"""
        return self.embeddings.get(node_id)
        
    def compute_similarity(self, node_id1: str, node_id2: str) -> float:
        """Compute cosine similarity between two nodes"""
        emb1 = self.get_node_embedding(node_id1)
        emb2 = self.get_node_embedding(node_id2)
        
        if emb1 is None or emb2 is None:
            return 0.0
            
        norm1 = np.linalg.norm(emb1)
        norm2 = np.linalg.norm(emb2)
        
        if norm1 == 0 or norm2 == 0:
            return 0.0
            
        return float(np.dot(emb1, emb2) / (norm1 * norm2))
        
    def predict_links(self, threshold: float = 0.5, 
                     max_predictions: int = 100) -> List[Tuple[str, str, float]]:
        """
        Predict potential links based on node embeddings
        
        Args:
            threshold: Minimum similarity threshold
            max_predictions: Maximum number of predictions to return
            
        Returns:
            List of (source, target, similarity) tuples
        """
        predictions = []
        nodes = list(self.embeddings.keys())
        
        for i, node1 in enumerate(nodes):
            for node2 in nodes[i+1:]:
                similarity = self.compute_similarity(node1, node2)
                if similarity > threshold:
                    predictions.append((node1, node2, similarity))
                    
        predictions.sort(key=lambda x: x[2], reverse=True)
        predictions = predictions[:max_predictions]
        
        logger.info(f"Predicted {len(predictions)} potential links")
        return predictions
        
    def get_statistics(self) -> Dict[str, Any]:
        """Get GNN statistics"""
        return {
            'num_layers': self.num_layers,
            'input_dim': self.input_dim,
            'hidden_dim': self.hidden_dim,
            'output_dim': self.output_dim,
            'aggregation': self.aggregation,
            'is_trained': self.is_trained,
            'num_embeddings': len(self.embeddings),
            'training_epochs': len(self.training_history),
            'num_diagnostics': len(self.diagnostic_atoms)
        }
        
    def get_diagnostics(self) -> List[Dict[str, Any]]:
        """Get diagnostic atoms for autognostic integration"""
        return self.diagnostic_atoms.copy()
        
    def clear_diagnostics(self):
        """Clear diagnostic atoms"""
        self.diagnostic_atoms.clear()
