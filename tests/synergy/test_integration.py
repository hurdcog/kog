#!/usr/bin/env python3
"""
Cognitive Synergy Integration Tests

Tests the integration and interaction between different cognitive components
to validate true cognitive synergy in the OpenCog Collection.
"""

import sys
import os
import unittest
import json
import time

# Add parent directories to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../..'))

try:
    from synergy.core.orchestrator import (
        get_orchestrator,
        ComponentState,
        EventType
    )
    from synergy.core.hypergraph_bridge import (
        get_bridge,
        AtomType
    )
    from synergy.monitoring.cognitive_monitor import (
        get_monitor,
        PerformanceTimer
    )
    SYNERGY_MODULES_AVAILABLE = True
except ImportError as e:
    print(f"Warning: Could not import synergy modules: {e}")
    SYNERGY_MODULES_AVAILABLE = False


class TestCognitiveSynergyIntegration(unittest.TestCase):
    """Integration tests for cognitive synergy"""
    
    @classmethod
    def setUpClass(cls):
        """Set up test environment"""
        if not SYNERGY_MODULES_AVAILABLE:
            raise unittest.SkipTest("Synergy modules not available")
        
        cls.orchestrator = get_orchestrator()
        cls.bridge = get_bridge()
        cls.monitor = get_monitor()
        
        # Start orchestrator
        cls.orchestrator.start()
        cls.monitor.start_monitoring(interval=1.0)
    
    @classmethod
    def tearDownClass(cls):
        """Clean up after tests"""
        if SYNERGY_MODULES_AVAILABLE:
            cls.orchestrator.stop()
            cls.monitor.stop_monitoring()
    
    def test_01_component_registration(self):
        """Test component registration with orchestrator"""
        component = self.orchestrator.register_component(
            name="test_atomspace",
            component_type="hypergraph_db",
            capabilities=["storage", "query"],
            dependencies=[]
        )
        
        self.assertIsNotNone(component)
        self.assertEqual(component.name, "test_atomspace")
        self.assertEqual(component.state, ComponentState.READY)
        self.assertIn("test_atomspace", self.orchestrator.components)
    
    def test_02_hypergraph_bridge_creation(self):
        """Test hypergraph bridge atom creation"""
        # Create atoms
        cat = self.bridge.create_concept("cat")
        animal = self.bridge.create_concept("animal")
        
        self.assertIsNotNone(cat)
        self.assertIsNotNone(animal)
        self.assertEqual(cat.atom_type, AtomType.CONCEPT)
        self.assertEqual(cat.name, "cat")
    
    def test_03_hypergraph_bridge_serialization(self):
        """Test atom serialization and deserialization"""
        # Create an atom
        original = self.bridge.create_concept(
            "test_concept",
            truth_value={'strength': 0.8, 'confidence': 0.9}
        )
        
        # Serialize to JSON
        json_str = self.bridge.serialize_to_json(original)
        self.assertIsNotNone(json_str)
        
        # Deserialize
        restored = self.bridge.deserialize_from_json(json_str)
        self.assertEqual(restored.name, original.name)
        self.assertEqual(restored.atom_type, original.atom_type)
        self.assertEqual(restored.truth_value, original.truth_value)
    
    def test_04_hypergraph_bridge_conversion(self):
        """Test conversion between AtomSpace and Hyperon formats"""
        # Create inheritance link
        cat = self.bridge.create_concept("cat")
        animal = self.bridge.create_concept("animal")
        inheritance = self.bridge.create_inheritance(cat, animal)
        
        # Convert to AtomSpace format
        scheme = self.bridge.convert_to_atomspace(inheritance)
        self.assertIn("InheritanceLink", scheme)
        self.assertIn("cat", scheme)
        self.assertIn("animal", scheme)
        
        # Convert to Hyperon format
        metta = self.bridge.convert_to_hyperon(inheritance)
        self.assertIsNotNone(metta)
    
    def test_05_data_transfer_recording(self):
        """Test recording of data transfers between components"""
        # Register components
        self.orchestrator.register_component(
            "source_comp",
            "test_component",
            capabilities=["output"]
        )
        self.orchestrator.register_component(
            "target_comp",
            "test_component",
            capabilities=["input"]
        )
        
        # Record data transfer
        self.orchestrator.record_data_transfer(
            source="source_comp",
            target="target_comp",
            data_type="atom_data",
            data_size=1024
        )
        
        # Check metrics
        stats = self.orchestrator.metrics.get_statistics()
        self.assertGreater(stats['total_events'], 0)
        self.assertGreater(stats['total_interactions'], 0)
    
    def test_06_synergy_score_calculation(self):
        """Test synergy score calculation"""
        # Create some interactions
        for i in range(5):
            self.orchestrator.record_data_transfer(
                source=f"comp_{i}",
                target=f"comp_{(i+1)%5}",
                data_type="test_data",
                data_size=100
            )
        
        # Calculate synergy score
        score = self.orchestrator.metrics.calculate_synergy_score()
        self.assertIsInstance(score, float)
        self.assertGreaterEqual(score, 0.0)
    
    def test_07_performance_monitoring(self):
        """Test performance monitoring with timer"""
        # Simulate a cognitive operation
        with PerformanceTimer(
            self.monitor,
            "test_operation",
            "test_component"
        ):
            time.sleep(0.1)
        
        # Check that metric was recorded
        stats = self.monitor.get_performance_statistics()
        self.assertGreater(stats.get('total_operations', 0), 0)
    
    def test_08_cognitive_state_assessment(self):
        """Test cognitive state assessment"""
        # Wait for some monitoring data
        time.sleep(2)
        
        # Get cognitive state
        state = self.monitor.get_cognitive_state()
        
        self.assertIn('cognitive_load', state)
        self.assertIn('resources', state)
        self.assertIn('performance', state)
        self.assertIn(state['cognitive_load'], 
                     ['unknown', 'light', 'moderate', 'heavy', 'critical'])
    
    def test_09_synergy_report_generation(self):
        """Test synergy report generation"""
        report = self.orchestrator.get_synergy_report()
        
        self.assertIn('timestamp', report)
        self.assertIn('components', report)
        self.assertIn('metrics', report)
        self.assertIn('system_health', report)
        
        # Check system health
        health = report['system_health']
        self.assertIn('health_score', health)
        self.assertIn('status', health)
    
    def test_10_end_to_end_cognitive_task(self):
        """Test end-to-end cognitive task with all components"""
        # Register cognitive components
        atomspace = self.orchestrator.register_component(
            "atomspace",
            "hypergraph_db",
            capabilities=["storage", "query", "reasoning"]
        )
        
        hyperon = self.orchestrator.register_component(
            "hyperon",
            "cognitive_framework",
            capabilities=["reasoning", "learning"],
            dependencies=["atomspace"]
        )
        
        # Create knowledge in hypergraph
        with PerformanceTimer(self.monitor, "create_knowledge", "atomspace"):
            cat = self.bridge.create_concept("cat")
            animal = self.bridge.create_concept("animal")
            inheritance = self.bridge.create_inheritance(cat, animal)
        
        # Simulate data transfer
        self.orchestrator.record_data_transfer(
            source="atomspace",
            target="hyperon",
            data_type="inheritance_link",
            data_size=len(self.bridge.serialize_to_json(inheritance))
        )
        
        # Simulate reasoning
        with PerformanceTimer(self.monitor, "reasoning", "hyperon"):
            time.sleep(0.05)  # Simulate reasoning time
        
        # Transfer result back
        self.orchestrator.record_data_transfer(
            source="hyperon",
            target="atomspace",
            data_type="inference_result",
            data_size=256
        )
        
        # Verify synergy
        synergy_score = self.orchestrator.metrics.calculate_synergy_score()
        self.assertGreater(synergy_score, 0.0)
        
        # Verify performance tracking
        perf_stats = self.monitor.get_performance_statistics()
        self.assertGreater(perf_stats['total_operations'], 0)


def run_tests():
    """Run all tests and generate report"""
    # Create test suite
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromTestCase(TestCognitiveSynergyIntegration)
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Generate summary
    print("\n" + "="*70)
    print("  Cognitive Synergy Integration Test Summary")
    print("="*70)
    print(f"Tests Run: {result.testsRun}")
    print(f"Successes: {result.testsRun - len(result.failures) - len(result.errors)}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    print(f"Skipped: {len(result.skipped)}")
    
    success_rate = ((result.testsRun - len(result.failures) - len(result.errors)) / 
                   result.testsRun * 100) if result.testsRun > 0 else 0
    print(f"Success Rate: {success_rate:.1f}%")
    print("="*70)
    
    # Return exit code
    return 0 if result.wasSuccessful() else 1


if __name__ == '__main__':
    sys.exit(run_tests())
