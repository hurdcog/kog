#!/usr/bin/env python3
"""
Synergy Integration Tests

Tests the interaction and synergy between cognitive components.
Validates that components work together to produce emergent intelligence.
"""

import sys
import time
import unittest
from pathlib import Path

# Add synergy modules to path
sys.path.insert(0, str(Path(__file__).parent.parent.parent / 'synergy'))

from core.synergy_orchestrator import SynergyOrchestrator, SynergyEvent
from bridges.hypergraph_bridge import HypergraphBridge, Atom, AtomType
from monitors.cognitive_monitor import CognitiveMonitor


class TestSynergyIntegration(unittest.TestCase):
    """Test suite for cognitive synergy integration."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.orchestrator = SynergyOrchestrator()
        self.bridge = HypergraphBridge()
        self.monitor = CognitiveMonitor(history_size=100)
    
    def tearDown(self):
        """Clean up after tests."""
        self.orchestrator.shutdown()
    
    def test_component_registration(self):
        """Test that components can be registered with the orchestrator."""
        self.orchestrator.register_component('test_component', {'type': 'test'})
        self.assertIn('test_component', self.orchestrator.components)
        
        component = self.orchestrator.components['test_component']
        self.assertEqual(component.name, 'test_component')
        self.assertEqual(component.status, 'idle')
    
    def test_component_state_updates(self):
        """Test that component states can be updated."""
        self.orchestrator.register_component('atomspace', {'type': 'knowledge_base'})
        
        self.orchestrator.update_component_state('atomspace', 'active', 0.5, {'atoms': 100})
        
        component = self.orchestrator.components['atomspace']
        self.assertEqual(component.status, 'active')
        self.assertEqual(component.load, 0.5)
        self.assertEqual(component.metrics['atoms'], 100)
    
    def test_component_interactions(self):
        """Test that component interactions are recorded."""
        self.orchestrator.register_component('atomspace', {'type': 'knowledge_base'})
        self.orchestrator.register_component('hyperon', {'type': 'reasoning_engine'})
        
        self.orchestrator.record_interaction('atomspace', 'hyperon', {'atoms_transferred': 50})
        
        self.assertIn('hyperon', self.orchestrator.interaction_graph['atomspace'])
    
    def test_synergy_score_calculation(self):
        """Test that synergy score is calculated correctly."""
        # Register components
        self.orchestrator.register_component('atomspace', {'type': 'knowledge_base'})
        self.orchestrator.register_component('hyperon', {'type': 'reasoning_engine'})
        self.orchestrator.register_component('cogserver', {'type': 'communication'})
        
        # Update states
        self.orchestrator.update_component_state('atomspace', 'active', 0.6)
        self.orchestrator.update_component_state('hyperon', 'processing', 0.7)
        self.orchestrator.update_component_state('cogserver', 'active', 0.5)
        
        # Record interactions
        self.orchestrator.record_interaction('atomspace', 'hyperon', {})
        self.orchestrator.record_interaction('hyperon', 'cogserver', {})
        
        # Calculate synergy
        synergy_score = self.orchestrator.calculate_synergy_score()
        
        self.assertGreater(synergy_score, 0.0)
        self.assertLessEqual(synergy_score, 1.0)
    
    def test_hypergraph_atom_creation(self):
        """Test that atoms can be created through the bridge."""
        cat = self.bridge.create_concept('cat', {'strength': 0.9, 'confidence': 0.8})
        
        self.assertEqual(cat.atom_type, AtomType.CONCEPT.value)
        self.assertEqual(cat.name, 'cat')
        self.assertIsNotNone(cat.truth_value)
        self.assertEqual(cat.truth_value['strength'], 0.9)
    
    def test_hypergraph_relationships(self):
        """Test that relationships can be created between atoms."""
        cat = self.bridge.create_concept('cat')
        animal = self.bridge.create_concept('animal')
        
        inheritance = self.bridge.create_inheritance(cat, animal, {'strength': 0.95, 'confidence': 0.9})
        
        self.assertEqual(inheritance.atom_type, AtomType.INHERITANCE.value)
        self.assertEqual(len(inheritance.outgoing), 2)
        self.assertEqual(inheritance.outgoing[0].name, 'cat')
        self.assertEqual(inheritance.outgoing[1].name, 'animal')
    
    def test_hypergraph_serialization(self):
        """Test that atoms can be serialized and deserialized."""
        # Create atoms
        cat = self.bridge.create_concept('cat')
        animal = self.bridge.create_concept('animal')
        inheritance = self.bridge.create_inheritance(cat, animal)
        
        atoms = [cat, animal, inheritance]
        
        # Serialize
        temp_path = Path('/tmp/test_atoms.json')
        self.bridge.serialize_to_json(atoms, temp_path)
        
        # Deserialize
        loaded_atoms = self.bridge.deserialize_from_json(temp_path)
        
        self.assertEqual(len(loaded_atoms), len(atoms))
        self.assertEqual(loaded_atoms[0].name, 'cat')
        self.assertEqual(loaded_atoms[1].name, 'animal')
        
        # Clean up
        temp_path.unlink()
    
    def test_hypergraph_scheme_export(self):
        """Test that atoms can be exported to Scheme format."""
        cat = self.bridge.create_concept('cat', {'strength': 0.9, 'confidence': 0.8})
        scheme = cat.to_scheme()
        
        self.assertIn('ConceptNode', scheme)
        self.assertIn('cat', scheme)
        self.assertIn('stv', scheme)
    
    def test_hypergraph_metta_export(self):
        """Test that atoms can be exported to MeTTa format."""
        cat = self.bridge.create_concept('cat')
        metta = cat.to_metta()
        
        self.assertIn('ConceptNode', metta)
        self.assertIn('cat', metta)
    
    def test_cognitive_monitoring(self):
        """Test that cognitive tasks can be monitored."""
        self.monitor.record_cognitive_task(
            component='atomspace',
            task_type='query',
            duration_ms=50,
            success=True,
            quality_score=0.9
        )
        
        summary = self.monitor.get_cognitive_summary(window_seconds=60)
        
        self.assertEqual(summary['total_tasks'], 1)
        self.assertEqual(summary['successful_tasks'], 1)
        self.assertEqual(summary['success_rate'], 1.0)
    
    def test_resource_monitoring(self):
        """Test that system resources can be monitored."""
        snapshot = self.monitor.capture_resource_snapshot()
        
        self.assertIsNotNone(snapshot.cpu_percent)
        self.assertIsNotNone(snapshot.memory_percent)
        self.assertGreaterEqual(snapshot.cpu_percent, 0)
        self.assertGreaterEqual(snapshot.memory_percent, 0)
    
    def test_component_health_tracking(self):
        """Test that component health can be tracked."""
        # Record some tasks
        for i in range(10):
            self.monitor.record_cognitive_task(
                component='atomspace',
                task_type='query',
                duration_ms=50 + i * 5,
                success=True
            )
        
        health = self.monitor.get_component_health('atomspace')
        
        self.assertIn('status', health)
        self.assertIn('success_rate', health)
        self.assertEqual(health['success_rate'], 1.0)
    
    def test_end_to_end_synergy_workflow(self):
        """Test complete synergy workflow from atom creation to monitoring."""
        # 1. Register components
        self.orchestrator.register_component('atomspace', {'type': 'knowledge_base'})
        self.orchestrator.register_component('hyperon', {'type': 'reasoning_engine'})
        
        # 2. Create knowledge in hypergraph
        start_time = time.time()
        cat = self.bridge.create_concept('cat')
        animal = self.bridge.create_concept('animal')
        inheritance = self.bridge.create_inheritance(cat, animal)
        duration_ms = (time.time() - start_time) * 1000
        
        # 3. Monitor the operation
        self.monitor.record_cognitive_task(
            component='atomspace',
            task_type='atom_creation',
            duration_ms=duration_ms,
            success=True,
            quality_score=1.0
        )
        
        # 4. Simulate data transfer
        atoms = [cat, animal, inheritance]
        self.orchestrator.record_interaction('atomspace', 'hyperon', {
            'atoms_transferred': len(atoms),
            'transfer_time_ms': duration_ms
        })
        
        # 5. Update component states
        self.orchestrator.update_component_state('atomspace', 'active', 0.5, {'atoms': len(atoms)})
        self.orchestrator.update_component_state('hyperon', 'processing', 0.7, {'inferences': 1})
        
        # 6. Calculate synergy
        synergy_score = self.orchestrator.calculate_synergy_score()
        
        # 7. Check system health
        health = self.monitor.get_system_health()
        
        # Assertions
        self.assertGreater(synergy_score, 0.0)
        self.assertIn('status', health)
        self.assertEqual(len(self.orchestrator.components), 2)
        self.assertIn('hyperon', self.orchestrator.interaction_graph['atomspace'])


class TestSynergyMetrics(unittest.TestCase):
    """Test suite for synergy metrics and scoring."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.orchestrator = SynergyOrchestrator()
    
    def tearDown(self):
        """Clean up after tests."""
        self.orchestrator.shutdown()
    
    def test_synergy_score_with_no_components(self):
        """Test synergy score when no components are registered."""
        score = self.orchestrator.calculate_synergy_score()
        self.assertEqual(score, 0.0)
    
    def test_synergy_score_with_isolated_components(self):
        """Test synergy score when components don't interact."""
        self.orchestrator.register_component('comp1', {})
        self.orchestrator.register_component('comp2', {})
        self.orchestrator.register_component('comp3', {})
        
        # Update states but no interactions
        self.orchestrator.update_component_state('comp1', 'active', 0.5)
        self.orchestrator.update_component_state('comp2', 'active', 0.5)
        self.orchestrator.update_component_state('comp3', 'active', 0.5)
        
        score = self.orchestrator.calculate_synergy_score()
        
        # Score should be moderate due to activity but no interactions
        self.assertLess(score, 0.7)
    
    def test_synergy_score_with_full_interaction(self):
        """Test synergy score when all components interact."""
        self.orchestrator.register_component('comp1', {})
        self.orchestrator.register_component('comp2', {})
        self.orchestrator.register_component('comp3', {})
        
        # Update states
        self.orchestrator.update_component_state('comp1', 'active', 0.5)
        self.orchestrator.update_component_state('comp2', 'active', 0.5)
        self.orchestrator.update_component_state('comp3', 'active', 0.5)
        
        # Create full interaction mesh
        self.orchestrator.record_interaction('comp1', 'comp2', {})
        self.orchestrator.record_interaction('comp1', 'comp3', {})
        self.orchestrator.record_interaction('comp2', 'comp1', {})
        self.orchestrator.record_interaction('comp2', 'comp3', {})
        self.orchestrator.record_interaction('comp3', 'comp1', {})
        self.orchestrator.record_interaction('comp3', 'comp2', {})
        
        score = self.orchestrator.calculate_synergy_score()
        
        # Score should be high due to full interaction
        self.assertGreater(score, 0.7)


def run_tests():
    """Run all tests."""
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    
    suite.addTests(loader.loadTestsFromTestCase(TestSynergyIntegration))
    suite.addTests(loader.loadTestsFromTestCase(TestSynergyMetrics))
    
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    return result.wasSuccessful()


if __name__ == '__main__':
    success = run_tests()
    sys.exit(0 if success else 1)
