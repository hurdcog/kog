"""
Tests for OpenCog GNN Module
"""

import unittest
import numpy as np
import sys
import os

# Add parent directory to path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from graph_network import GraphNeuralNetwork, GraphStructure, GNNLayer
from atomspace_bridge import AtomSpaceBridge, AtomType


class TestGraphStructure(unittest.TestCase):
    """Tests for GraphStructure class"""
    
    def test_empty_graph(self):
        """Test empty graph creation"""
        graph = GraphStructure(nodes=[], edges=[], node_features={})
        self.assertEqual(len(graph.nodes), 0)
        self.assertEqual(len(graph.edges), 0)
        self.assertEqual(graph.adjacency_matrix.shape, (0, 0))
        
    def test_simple_graph(self):
        """Test simple graph with nodes and edges"""
        nodes = ['a', 'b', 'c']
        edges = [('a', 'b'), ('b', 'c')]
        features = {
            'a': np.random.randn(128),
            'b': np.random.randn(128),
            'c': np.random.randn(128)
        }
        
        graph = GraphStructure(nodes=nodes, edges=edges, node_features=features)
        
        self.assertEqual(len(graph.nodes), 3)
        self.assertEqual(len(graph.edges), 2)
        self.assertEqual(graph.adjacency_matrix.shape, (3, 3))
        
        # Check adjacency matrix
        self.assertEqual(graph.adjacency_matrix[0, 1], 1)  # a -> b
        self.assertEqual(graph.adjacency_matrix[1, 2], 1)  # b -> c
        self.assertEqual(graph.adjacency_matrix[0, 2], 0)  # no a -> c
        
    def test_node_degree(self):
        """Test node degree computation"""
        nodes = ['a', 'b', 'c']
        edges = [('a', 'b'), ('a', 'c'), ('b', 'c')]
        features = {n: np.zeros(128) for n in nodes}
        
        graph = GraphStructure(nodes=nodes, edges=edges, node_features=features)
        
        in_deg, out_deg = graph.get_node_degree('a')
        self.assertEqual(out_deg, 2)
        self.assertEqual(in_deg, 0)
        
        in_deg, out_deg = graph.get_node_degree('c')
        self.assertEqual(out_deg, 0)
        self.assertEqual(in_deg, 2)
        
    def test_get_neighbors(self):
        """Test neighbor retrieval"""
        nodes = ['a', 'b', 'c']
        edges = [('a', 'b'), ('a', 'c')]
        features = {n: np.zeros(128) for n in nodes}
        
        graph = GraphStructure(nodes=nodes, edges=edges, node_features=features)
        
        neighbors = graph.get_neighbors('a')
        self.assertEqual(set(neighbors), {'b', 'c'})
        
        neighbors = graph.get_neighbors('c')
        self.assertEqual(neighbors, [])


class TestGNNLayer(unittest.TestCase):
    """Tests for GNNLayer class"""
    
    def test_layer_initialization(self):
        """Test layer initialization"""
        layer = GNNLayer(input_dim=128, output_dim=64, layer_id='test')
        
        self.assertEqual(layer.input_dim, 128)
        self.assertEqual(layer.output_dim, 64)
        self.assertEqual(layer.weights.shape, (128, 64))
        self.assertEqual(layer.bias.shape, (64,))
        
    def test_forward_pass(self):
        """Test layer forward pass"""
        layer = GNNLayer(input_dim=128, output_dim=64, layer_id='test')
        
        nodes = ['a', 'b']
        features = {
            'a': np.random.randn(128),
            'b': np.random.randn(128)
        }
        adj_matrix = np.array([[0, 1], [1, 0]])
        
        output = layer.forward(features, adj_matrix, nodes)
        
        self.assertEqual(len(output), 2)
        self.assertEqual(output['a'].shape, (64,))
        self.assertEqual(output['b'].shape, (64,))


class TestGraphNeuralNetwork(unittest.TestCase):
    """Tests for GraphNeuralNetwork class"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.config = {
            'input_dim': 64,
            'hidden_dim': 32,
            'output_dim': 16,
            'num_layers': 2
        }
        self.gnn = GraphNeuralNetwork(self.config)
        
    def test_initialization(self):
        """Test GNN initialization"""
        self.assertEqual(self.gnn.input_dim, 64)
        self.assertEqual(self.gnn.hidden_dim, 32)
        self.assertEqual(self.gnn.output_dim, 16)
        self.assertEqual(len(self.gnn.layers), 2)
        self.assertFalse(self.gnn.is_trained)
        
    def test_forward_pass(self):
        """Test GNN forward pass"""
        nodes = ['a', 'b', 'c']
        edges = [('a', 'b'), ('b', 'c')]
        features = {n: np.random.randn(64) for n in nodes}
        
        graph = GraphStructure(nodes=nodes, edges=edges, node_features=features)
        embeddings = self.gnn.forward_pass(graph)
        
        self.assertEqual(len(embeddings), 3)
        for node_id, emb in embeddings.items():
            self.assertEqual(emb.shape, (16,))
            
    def test_training(self):
        """Test GNN training"""
        nodes = ['a', 'b', 'c', 'd']
        edges = [('a', 'b'), ('b', 'c'), ('c', 'd'), ('a', 'd')]
        features = {n: np.random.randn(64) for n in nodes}
        
        graph = GraphStructure(nodes=nodes, edges=edges, node_features=features)
        results = self.gnn.train(graph, epochs=5)
        
        self.assertTrue(self.gnn.is_trained)
        self.assertIn('final_loss', results)
        self.assertEqual(len(results['history']), 5)
        
    def test_similarity(self):
        """Test node similarity computation"""
        nodes = ['a', 'b']
        edges = [('a', 'b')]
        features = {n: np.random.randn(64) for n in nodes}
        
        graph = GraphStructure(nodes=nodes, edges=edges, node_features=features)
        self.gnn.forward_pass(graph)
        
        similarity = self.gnn.compute_similarity('a', 'b')
        self.assertIsInstance(similarity, float)
        self.assertGreaterEqual(similarity, -1.0)
        self.assertLessEqual(similarity, 1.0)
        
    def test_link_prediction(self):
        """Test link prediction"""
        nodes = ['a', 'b', 'c', 'd']
        edges = [('a', 'b'), ('c', 'd')]
        features = {n: np.random.randn(64) for n in nodes}
        
        graph = GraphStructure(nodes=nodes, edges=edges, node_features=features)
        self.gnn.train(graph, epochs=3)
        
        predictions = self.gnn.predict_links(threshold=0.0)
        self.assertIsInstance(predictions, list)
        
    def test_statistics(self):
        """Test statistics retrieval"""
        stats = self.gnn.get_statistics()
        
        self.assertEqual(stats['num_layers'], 2)
        self.assertEqual(stats['input_dim'], 64)
        self.assertFalse(stats['is_trained'])
        
    def test_empty_graph_training(self):
        """Test training on empty graph"""
        graph = GraphStructure(nodes=[], edges=[], node_features={})
        results = self.gnn.train(graph, epochs=5)
        
        self.assertEqual(results['final_loss'], 0.0)
        self.assertEqual(results['epochs'], 0)


class TestAtomSpaceBridge(unittest.TestCase):
    """Tests for AtomSpaceBridge class"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.bridge = AtomSpaceBridge(backend='mock')
        
    def test_add_atom(self):
        """Test atom addition"""
        atom_id = self.bridge.add_atom('ConceptNode', 'test', truth_value=0.9)
        
        self.assertIsNotNone(atom_id)
        atom = self.bridge.get_atom(atom_id)
        self.assertEqual(atom.name, 'test')
        self.assertEqual(atom.truth_value, 0.9)
        
    def test_create_link(self):
        """Test link creation"""
        a = self.bridge.add_atom('ConceptNode', 'a')
        b = self.bridge.add_atom('ConceptNode', 'b')
        
        link_id = self.bridge.create_link('InheritanceLink', a, b)
        
        self.assertIsNotNone(link_id)
        
        # Check that link is in source's outgoing
        source = self.bridge.get_atom(a)
        self.assertIn(link_id, source.outgoing)
        
    def test_extract_atoms(self):
        """Test atom extraction"""
        self.bridge.add_atom('ConceptNode', 'a')
        self.bridge.add_atom('ConceptNode', 'b')
        self.bridge.add_atom('PredicateNode', 'p')
        
        all_atoms = self.bridge.extract_atoms()
        self.assertEqual(len(all_atoms), 3)
        
        concepts = self.bridge.extract_atoms(filter_type='ConceptNode')
        self.assertEqual(len(concepts), 2)
        
    def test_sample_knowledge_graph(self):
        """Test sample knowledge graph creation"""
        self.bridge.create_sample_knowledge_graph()
        
        stats = self.bridge.get_statistics()
        self.assertGreater(stats['total_atoms'], 0)
        
    def test_diagnostic_atom(self):
        """Test diagnostic atom creation"""
        atom_id = self.bridge.add_diagnostic_atom('error', {
            'severity_score': 0.8,
            'message': 'Test error'
        })
        
        self.assertIsNotNone(atom_id)
        atom = self.bridge.get_atom(atom_id)
        self.assertEqual(atom.atom_type, 'ErrorNode')
        
    def test_statistics(self):
        """Test statistics retrieval"""
        self.bridge.add_atom('ConceptNode', 'a')
        self.bridge.add_atom('ConceptNode', 'b')
        
        stats = self.bridge.get_statistics()
        
        self.assertEqual(stats['backend'], 'mock')
        self.assertEqual(stats['total_atoms'], 2)
        self.assertEqual(stats['atoms_by_type']['ConceptNode'], 2)


class TestGNNAtomSpaceIntegration(unittest.TestCase):
    """Integration tests for GNN with AtomSpace"""
    
    def test_full_pipeline(self):
        """Test full GNN pipeline with AtomSpace"""
        # Create bridge and sample data
        bridge = AtomSpaceBridge(backend='mock')
        bridge.create_sample_knowledge_graph()
        
        # Create GNN
        gnn = GraphNeuralNetwork({
            'input_dim': 64,
            'hidden_dim': 32,
            'output_dim': 16,
            'num_layers': 2
        })
        gnn.set_atomspace_bridge(bridge)
        
        # Extract graph
        atomspace_data = bridge.extract_atoms()
        graph = gnn.extract_graph(atomspace_data)
        
        self.assertGreater(len(graph.nodes), 0)
        
        # Train
        results = gnn.train(graph, epochs=5)
        self.assertTrue(gnn.is_trained)
        
        # Predict links
        predictions = gnn.predict_links(threshold=0.0, max_predictions=10)
        self.assertIsInstance(predictions, list)
        
        # Write embeddings back
        bridge.write_embeddings(gnn.embeddings)


if __name__ == '__main__':
    unittest.main()
