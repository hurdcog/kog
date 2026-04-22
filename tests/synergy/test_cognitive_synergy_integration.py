"""
Comprehensive Integration Tests for Cognitive Synergy

Tests the complete cognitive synergy system including:
- AAR Core emergence and self-awareness
- Hypergraph identity formation and refinement
- Membrane communication and isolation
- Cross-component integration
"""

import sys
import os
import unittest
import numpy as np

# Add synergy modules to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..'))

from synergy.core.aar_core import AARCore
from synergy.identity.hypergraph_identity import HypergraphIdentitySystem
from synergy.membranes.membrane_manager import MembraneManager, MessageType


class TestAARCore(unittest.TestCase):
    """Test Agent-Arena-Relation Core functionality."""
    
    def setUp(self):
        """Initialize AAR Core for testing."""
        self.aar = AARCore(dimension=32, learning_rate=0.01)
    
    def test_initialization(self):
        """Test AAR Core initializes correctly."""
        self.assertIsNotNone(self.aar.agent)
        self.assertIsNotNone(self.aar.arena)
        self.assertEqual(self.aar.dimension, 32)
    
    def test_cognitive_cycle(self):
        """Test complete cognitive cycle."""
        perception = np.random.randn(32)
        self.aar.perceive(perception)
        action = self.aar.act()
        metrics = self.aar.reflect()
        
        self.assertEqual(action.shape, (32,))
        self.assertIn('emergence_score', metrics)
        self.assertGreaterEqual(metrics['emergence_score'], 0.0)
        self.assertLessEqual(metrics['emergence_score'], 1.0)


class TestHypergraphIdentity(unittest.TestCase):
    """Test Hypergraph Identity System functionality."""
    
    def setUp(self):
        """Initialize identity system for testing."""
        self.identity = HypergraphIdentitySystem()
    
    def test_initialization(self):
        """Test identity system initializes with core_self."""
        self.assertIn('core_self', self.identity.graph.nodes)
    
    def test_refinement_tuple(self):
        """Test adding identity refinement tuples."""
        self.identity.add_refinement_tuple(
            subject='core_self',
            predicate='has_capability',
            obj='reasoning',
            context='cognitive_synergy',
            confidence=0.9
        )
        
        self.assertIn('reasoning', self.identity.graph.nodes)
        self.assertTrue(self.identity.graph.has_edge('core_self', 'reasoning'))


class TestMembraneManager(unittest.TestCase):
    """Test Membrane Architecture Manager functionality."""
    
    def setUp(self):
        """Initialize membrane manager for testing."""
        self.manager = MembraneManager()
    
    def test_message_passing(self):
        """Test message passing between membranes."""
        self.manager.send_message(
            source_id='reasoning',
            target_id='memory',
            message_type=MessageType.DATA,
            payload={'concept': 'test_concept'}
        )
        
        self.manager.process_all_messages()
        self.assertIsNotNone(self.manager.membranes.get('memory'))


class TestCognitiveSynergyIntegration(unittest.TestCase):
    """Test integrated cognitive synergy across all components."""
    
    def setUp(self):
        """Initialize all synergy components."""
        self.aar = AARCore(dimension=32)
        self.identity = HypergraphIdentitySystem()
        self.manager = MembraneManager()
    
    def test_end_to_end_cognitive_cycle(self):
        """Test complete cognitive cycle across all components."""
        # Perception
        perception = np.random.randn(32)
        self.aar.perceive(perception)
        
        # Store in memory membrane
        self.manager.send_message(
            source_id='reasoning',
            target_id='memory',
            message_type=MessageType.DATA,
            payload={'perception': perception.tolist()}
        )
        
        # Generate action
        action = self.aar.act()
        
        # Reflect and update self-awareness
        metrics = self.aar.reflect()
        
        # Refine identity
        self.identity.add_refinement_tuple(
            subject='core_self',
            predicate='has_emergence_score',
            obj=str(metrics['emergence_score']),
            context='cognitive_cycle',
            confidence=metrics['emergence_score']
        )
        
        # Process messages
        self.manager.process_all_messages()
        
        # Verify complete cycle
        self.assertIsNotNone(action)
        self.assertIn('emergence_score', metrics)
        self.assertGreater(len(self.identity.graph.nodes), 1)


def run_tests():
    """Run all cognitive synergy integration tests."""
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    
    suite.addTests(loader.loadTestsFromTestCase(TestAARCore))
    suite.addTests(loader.loadTestsFromTestCase(TestHypergraphIdentity))
    suite.addTests(loader.loadTestsFromTestCase(TestMembraneManager))
    suite.addTests(loader.loadTestsFromTestCase(TestCognitiveSynergyIntegration))
    
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    print("\n" + "="*70)
    print("COGNITIVE SYNERGY INTEGRATION TEST SUMMARY")
    print("="*70)
    print(f"Tests run: {result.testsRun}")
    print(f"Successes: {result.testsRun - len(result.failures) - len(result.errors)}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    print("="*70)
    
    return result.wasSuccessful()


if __name__ == '__main__':
    success = run_tests()
    sys.exit(0 if success else 1)
