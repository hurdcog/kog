"""
Tests for Attention-MOSES Bridge

This module provides comprehensive tests for the attention-guided
evolutionary learning integration.
"""

import unittest
import sys
import os
from pathlib import Path

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from synergy.bridges.attention_moses_bridge import (
    AttentionMOSESBridge,
    AttentionSignal,
    AttentionSignalType,
    LearningTask,
    create_demo_attention_atoms
)


class TestAttentionSignalExtraction(unittest.TestCase):
    """Test attention signal extraction from atoms"""
    
    def setUp(self):
        self.bridge = AttentionMOSESBridge(
            sti_threshold=100.0,
            lti_threshold=50.0
        )
    
    def test_extract_high_sti_signals(self):
        """Test extraction of high STI signals"""
        atoms = [
            {'id': 'atom1', 'type': 'ConceptNode', 'sti': 150, 'lti': 30},
            {'id': 'atom2', 'type': 'ConceptNode', 'sti': 200, 'lti': 40},
            {'id': 'atom3', 'type': 'ConceptNode', 'sti': 50, 'lti': 60},
        ]
        
        signals = self.bridge.extract_attention_signals(
            atoms,
            signal_types={AttentionSignalType.STI_THRESHOLD}
        )
        
        # Should extract 2 atoms exceeding STI threshold of 100
        self.assertEqual(len(signals), 2)
        self.assertEqual(signals[0].atom_id, 'atom1')
        self.assertEqual(signals[1].atom_id, 'atom2')
    
    def test_extract_high_lti_signals(self):
        """Test extraction of high LTI signals"""
        atoms = [
            {'id': 'atom1', 'type': 'ConceptNode', 'sti': 30, 'lti': 60},
            {'id': 'atom2', 'type': 'ConceptNode', 'sti': 40, 'lti': 70},
            {'id': 'atom3', 'type': 'ConceptNode', 'sti': 50, 'lti': 30},
        ]
        
        signals = self.bridge.extract_attention_signals(
            atoms,
            signal_types={AttentionSignalType.LTI_THRESHOLD}
        )
        
        # Should extract 2 atoms exceeding LTI threshold of 50
        self.assertEqual(len(signals), 2)
        self.assertEqual(signals[0].atom_id, 'atom1')
        self.assertEqual(signals[1].atom_id, 'atom2')
    
    def test_signal_properties(self):
        """Test that signals have correct properties"""
        atoms = [
            {'id': 'test_atom', 'type': 'PredicateNode', 'sti': 150, 'lti': 60}
        ]
        
        # Extract only STI signals to get exactly one signal
        signals = self.bridge.extract_attention_signals(
            atoms,
            signal_types={AttentionSignalType.STI_THRESHOLD}
        )
        self.assertEqual(len(signals), 1)
        
        signal = signals[0]
        self.assertEqual(signal.atom_id, 'test_atom')
        self.assertEqual(signal.atom_type, 'PredicateNode')
        self.assertEqual(signal.sti, 150)
        self.assertEqual(signal.lti, 60)
        self.assertIsNotNone(signal.timestamp)


class TestFitnessBonusCalculation(unittest.TestCase):
    """Test conversion of attention to fitness bonus"""
    
    def setUp(self):
        self.bridge = AttentionMOSESBridge(fitness_scaling=2.0)
    
    def test_fitness_bonus_scaling(self):
        """Test that fitness bonus scales with attention"""
        # High attention signal
        high_signal = AttentionSignal(
            atom_id='high',
            atom_type='ConceptNode',
            sti=500,
            lti=200,
            signal_type=AttentionSignalType.STI_THRESHOLD,
            timestamp=0.0
        )
        
        # Low attention signal
        low_signal = AttentionSignal(
            atom_id='low',
            atom_type='ConceptNode',
            sti=100,
            lti=50,
            signal_type=AttentionSignalType.STI_THRESHOLD,
            timestamp=0.0
        )
        
        high_bonus = self.bridge.convert_attention_to_fitness_bonus(high_signal)
        low_bonus = self.bridge.convert_attention_to_fitness_bonus(low_signal)
        
        # High attention should give higher bonus
        self.assertGreater(high_bonus, low_bonus)
    
    def test_sti_lti_weighting(self):
        """Test that STI gets higher weight than LTI"""
        # High STI, low LTI
        sti_heavy = AttentionSignal(
            atom_id='sti',
            atom_type='ConceptNode',
            sti=1000,
            lti=0,
            signal_type=AttentionSignalType.STI_THRESHOLD,
            timestamp=0.0
        )
        
        # Low STI, high LTI
        lti_heavy = AttentionSignal(
            atom_id='lti',
            atom_type='ConceptNode',
            sti=0,
            lti=1000,
            signal_type=AttentionSignalType.LTI_THRESHOLD,
            timestamp=0.0
        )
        
        sti_bonus = self.bridge.convert_attention_to_fitness_bonus(sti_heavy)
        lti_bonus = self.bridge.convert_attention_to_fitness_bonus(lti_heavy)
        
        # STI should contribute more (0.7 weight vs 0.3 for LTI)
        self.assertGreater(sti_bonus, lti_bonus)


class TestLearningTaskManagement(unittest.TestCase):
    """Test learning task creation and prioritization"""
    
    def setUp(self):
        self.bridge = AttentionMOSESBridge(max_tasks=5)
    
    def test_create_learning_task(self):
        """Test creating a learning task"""
        signals = [
            AttentionSignal(
                atom_id='atom1',
                atom_type='ConceptNode',
                sti=150,
                lti=60,
                signal_type=AttentionSignalType.STI_THRESHOLD,
                timestamp=0.0
            )
        ]
        
        task = self.bridge.create_learning_task(
            task_id='test_task',
            description='Test learning task',
            attention_signals=signals,
            base_fitness=0.5
        )
        
        self.assertEqual(task.task_id, 'test_task')
        self.assertEqual(task.description, 'Test learning task')
        self.assertEqual(len(task.target_atoms), 1)
        self.assertEqual(task.base_fitness, 0.5)
        self.assertGreater(task.attention_bonus, 0)
        self.assertGreater(task.priority, 0.5)
    
    def test_task_prioritization(self):
        """Test that tasks are prioritized correctly"""
        # Create tasks with different attention levels
        high_attention = [AttentionSignal(
            atom_id='high', atom_type='ConceptNode',
            sti=800, lti=200,
            signal_type=AttentionSignalType.STI_THRESHOLD,
            timestamp=0.0
        )]
        
        low_attention = [AttentionSignal(
            atom_id='low', atom_type='ConceptNode',
            sti=110, lti=55,
            signal_type=AttentionSignalType.STI_THRESHOLD,
            timestamp=0.0
        )]
        
        task1 = self.bridge.create_learning_task(
            'task1', 'Low priority', low_attention, 0.5
        )
        task2 = self.bridge.create_learning_task(
            'task2', 'High priority', high_attention, 0.5
        )
        
        # Get prioritized tasks
        prioritized = self.bridge.get_prioritized_tasks(limit=2)
        
        # High attention task should be first
        self.assertEqual(prioritized[0].task_id, 'task2')
        self.assertEqual(prioritized[1].task_id, 'task1')
    
    def test_task_status_update(self):
        """Test updating task status"""
        signals = [AttentionSignal(
            atom_id='atom1', atom_type='ConceptNode',
            sti=150, lti=60,
            signal_type=AttentionSignalType.STI_THRESHOLD,
            timestamp=0.0
        )]
        
        task = self.bridge.create_learning_task(
            'status_test', 'Test status', signals, 0.5
        )
        
        self.assertEqual(task.status, 'pending')
        
        self.bridge.update_task_status('status_test', 'running')
        self.assertEqual(task.status, 'running')
        
        results = {'accuracy': 0.85}
        self.bridge.update_task_status('status_test', 'completed', results)
        self.assertEqual(task.status, 'completed')
        self.assertEqual(task.results, results)


class TestFeedbackLoop(unittest.TestCase):
    """Test feedback from learning to attention"""
    
    def setUp(self):
        self.bridge = AttentionMOSESBridge()
    
    def test_successful_learning_feedback(self):
        """Test feedback from successful learning"""
        signals = [AttentionSignal(
            atom_id='learned_atom', atom_type='ConceptNode',
            sti=150, lti=60,
            signal_type=AttentionSignalType.STI_THRESHOLD,
            timestamp=0.0
        )]
        
        task = self.bridge.create_learning_task(
            'feedback_test', 'Test feedback', signals, 0.5
        )
        
        # Simulate successful learning
        outcomes = {
            'success': True,
            'accuracy': 0.85,
            'patterns_learned': [
                {'id': 'new_pattern_1', 'confidence': 0.9},
                {'id': 'new_pattern_2', 'confidence': 0.8}
            ]
        }
        
        recommendations = self.bridge.feedback_to_attention(
            'feedback_test', outcomes
        )
        
        # Should recommend LTI increase for learned atom
        self.assertIn('updates', recommendations)
        updates = recommendations['updates']
        
        # Should have updates for learned atom and new patterns
        self.assertGreater(len(updates), 0)
        
        # Check LTI increase for learned atom
        lti_updates = [u for u in updates if 'lti_delta' in u]
        self.assertGreater(len(lti_updates), 0)
        
        # Check STI boost for new patterns
        sti_updates = [u for u in updates if 'sti_delta' in u]
        self.assertGreater(len(sti_updates), 0)
    
    def test_feedback_history(self):
        """Test that feedback is recorded in history"""
        signals = [AttentionSignal(
            atom_id='atom1', atom_type='ConceptNode',
            sti=150, lti=60,
            signal_type=AttentionSignalType.STI_THRESHOLD,
            timestamp=0.0
        )]
        
        task = self.bridge.create_learning_task(
            'history_test', 'Test history', signals, 0.5
        )
        
        outcomes = {'success': True, 'accuracy': 0.9}
        self.bridge.feedback_to_attention('history_test', outcomes)
        
        # Check feedback was recorded
        self.assertEqual(len(self.bridge.feedback_history), 1)
        self.assertEqual(
            self.bridge.feedback_history[0]['task_id'],
            'history_test'
        )


class TestIntegration(unittest.TestCase):
    """Integration tests for complete workflow"""
    
    def test_complete_workflow(self):
        """Test complete attention-guided learning workflow"""
        bridge = AttentionMOSESBridge(sti_threshold=100.0)
        
        # 1. Create atoms with attention values
        atoms = create_demo_attention_atoms(20)
        
        # 2. Extract attention signals
        signals = bridge.extract_attention_signals(atoms)
        self.assertGreater(len(signals), 0)
        
        # 3. Create learning tasks
        for i, signal in enumerate(signals[:3]):
            task = bridge.create_learning_task(
                f'workflow_task_{i}',
                f'Learn patterns for {signal.atom_id}',
                [signal],
                base_fitness=0.5
            )
            self.assertIsNotNone(task)
        
        # 4. Get prioritized tasks
        prioritized = bridge.get_prioritized_tasks(limit=2)
        self.assertEqual(len(prioritized), 2)
        
        # 5. Simulate learning and feedback
        for task in prioritized:
            bridge.update_task_status(task.task_id, 'running')
            
            outcomes = {
                'success': True,
                'accuracy': 0.85,
                'patterns_learned': [
                    {'id': f'pattern_{task.task_id}', 'confidence': 0.9}
                ]
            }
            
            bridge.update_task_status(task.task_id, 'completed', outcomes)
            recommendations = bridge.feedback_to_attention(task.task_id, outcomes)
            
            self.assertIn('updates', recommendations)
            self.assertGreater(len(recommendations['updates']), 0)
        
        # 6. Verify statistics
        stats = bridge.get_stats()
        self.assertEqual(stats['tasks_completed'], 2)
        self.assertGreater(stats['signals_processed'], 0)
        self.assertGreater(stats['feedback_cycles'], 0)


class TestBridgeStatistics(unittest.TestCase):
    """Test bridge statistics tracking"""
    
    def test_statistics_tracking(self):
        """Test that statistics are tracked correctly"""
        bridge = AttentionMOSESBridge()
        
        # Initial stats
        stats = bridge.get_stats()
        self.assertEqual(stats['signals_processed'], 0)
        self.assertEqual(stats['tasks_created'], 0)
        
        # Extract signals
        atoms = create_demo_attention_atoms(5)
        signals = bridge.extract_attention_signals(atoms)
        
        stats = bridge.get_stats()
        self.assertGreater(stats['signals_processed'], 0)
        
        # Create task
        task = bridge.create_learning_task('stat_test', 'Test', signals[:1], 0.5)
        
        stats = bridge.get_stats()
        self.assertEqual(stats['tasks_created'], 1)
        
        # Complete task
        bridge.update_task_status('stat_test', 'completed')
        
        stats = bridge.get_stats()
        self.assertEqual(stats['tasks_completed'], 1)


def run_tests():
    """Run all tests"""
    print("=" * 80)
    print("ATTENTION-MOSES BRIDGE TEST SUITE")
    print("=" * 80)
    print()
    
    # Create test suite
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    
    # Add all test classes
    suite.addTests(loader.loadTestsFromTestCase(TestAttentionSignalExtraction))
    suite.addTests(loader.loadTestsFromTestCase(TestFitnessBonusCalculation))
    suite.addTests(loader.loadTestsFromTestCase(TestLearningTaskManagement))
    suite.addTests(loader.loadTestsFromTestCase(TestFeedbackLoop))
    suite.addTests(loader.loadTestsFromTestCase(TestIntegration))
    suite.addTests(loader.loadTestsFromTestCase(TestBridgeStatistics))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Print summary
    print()
    print("=" * 80)
    if result.wasSuccessful():
        print("✓ ALL TESTS PASSED")
        print(f"  Tests run: {result.testsRun}")
        print("  Emergent capability: focused_learning validated!")
    else:
        print("✗ SOME TESTS FAILED")
        print(f"  Tests run: {result.testsRun}")
        print(f"  Failures: {len(result.failures)}")
        print(f"  Errors: {len(result.errors)}")
    print("=" * 80)
    
    return result.wasSuccessful()


if __name__ == '__main__':
    success = run_tests()
    sys.exit(0 if success else 1)
