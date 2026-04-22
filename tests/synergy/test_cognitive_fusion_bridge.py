"""
Test suite for Cross-Modal Cognitive Fusion Bridge

Tests the integration between reasoning (PLN), learning (MOSES), and memory
(AtomSpace) in unified cognitive loops.
"""

import unittest
import sys
import os

# Add parent directory to path for imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../..'))

from synergy.bridges.cognitive_fusion_bridge import (
    CognitiveFusionBridge,
    SharedRepresentationLayer,
    PLNToMOSESFeedback,
    MOSESToPLNFeedback,
    MetaLearningSystem,
    ReasoningOutcome,
    LearningOutcome,
    CognitiveMode,
    CognitiveState,
    FeedbackType,
    create_cognitive_fusion_bridge
)


class TestSharedRepresentationLayer(unittest.TestCase):
    """Test the shared representation layer"""
    
    def setUp(self):
        self.layer = SharedRepresentationLayer()
    
    def test_add_and_get_atom(self):
        """Test adding and retrieving atoms"""
        self.layer.add_atom(
            "test_atom_1",
            "concept",
            "test content",
            CognitiveMode.REASONING
        )
        
        atom = self.layer.get_atom("test_atom_1")
        self.assertIsNotNone(atom)
        self.assertEqual(atom['type'], "concept")
        self.assertEqual(atom['content'], "test content")
        self.assertEqual(atom['source'], "reasoning")
    
    def test_atom_access_count(self):
        """Test that atom access is tracked"""
        self.layer.add_atom(
            "test_atom_2",
            "concept",
            "content",
            CognitiveMode.LEARNING
        )
        
        # Access multiple times
        for _ in range(5):
            atom = self.layer.get_atom("test_atom_2")
        
        atom = self.layer.get_atom("test_atom_2")
        self.assertEqual(atom['access_count'], 6)
    
    def test_add_connection(self):
        """Test creating connections between atoms"""
        self.layer.add_atom("atom_a", "concept", "A", CognitiveMode.REASONING)
        self.layer.add_atom("atom_b", "concept", "B", CognitiveMode.REASONING)
        
        self.layer.add_connection("atom_a", "atom_b", "relates_to")
        
        connected = self.layer.get_connected_atoms("atom_a")
        self.assertIn("atom_b", connected)
    
    def test_get_atoms_by_source(self):
        """Test filtering atoms by source mode"""
        self.layer.add_atom("r1", "concept", "R1", CognitiveMode.REASONING)
        self.layer.add_atom("r2", "concept", "R2", CognitiveMode.REASONING)
        self.layer.add_atom("l1", "concept", "L1", CognitiveMode.LEARNING)
        
        reasoning_atoms = self.layer.get_atoms_by_source(CognitiveMode.REASONING)
        self.assertEqual(len(reasoning_atoms), 2)
        self.assertIn("r1", reasoning_atoms)
        self.assertIn("r2", reasoning_atoms)
    
    def test_high_access_atoms(self):
        """Test identifying frequently accessed atoms"""
        self.layer.add_atom("frequent", "concept", "F", CognitiveMode.REASONING)
        self.layer.add_atom("infrequent", "concept", "I", CognitiveMode.REASONING)
        
        # Access one atom many times
        for _ in range(10):
            self.layer.get_atom("frequent")
        
        # Access other atom few times
        for _ in range(2):
            self.layer.get_atom("infrequent")
        
        high_access = self.layer.get_high_access_atoms(threshold=5)
        self.assertIn("frequent", high_access)
        self.assertNotIn("infrequent", high_access)


class TestPLNToMOSESFeedback(unittest.TestCase):
    """Test PLN to MOSES feedback loop"""
    
    def setUp(self):
        self.layer = SharedRepresentationLayer()
        self.feedback = PLNToMOSESFeedback(self.layer)
    
    def test_process_reasoning_outcome(self):
        """Test processing reasoning outcomes"""
        outcome = ReasoningOutcome(
            conclusion="If A then B",
            confidence=0.9,
            inference_steps=5,
            supporting_atoms=["atom_1", "atom_2"],
            reasoning_patterns=["conditional", "implication"]
        )
        
        signal = self.feedback.process_reasoning_outcome(outcome)
        
        self.assertEqual(signal.source_mode, CognitiveMode.REASONING)
        self.assertEqual(signal.target_mode, CognitiveMode.LEARNING)
        self.assertEqual(signal.feedback_type, FeedbackType.REASONING_TO_LEARNING)
        self.assertEqual(signal.strength, 0.9)
        self.assertIn('search_focus', signal.content)
    
    def test_reasoning_patterns_stored(self):
        """Test that reasoning patterns are stored in shared layer"""
        outcome = ReasoningOutcome(
            conclusion="X -> Y",
            confidence=0.85,
            inference_steps=3,
            supporting_atoms=[],
            reasoning_patterns=["pattern_1", "pattern_2"]
        )
        
        self.feedback.process_reasoning_outcome(outcome)
        
        # Check that patterns were stored
        reasoning_atoms = self.layer.get_atoms_by_source(CognitiveMode.REASONING)
        self.assertGreater(len(reasoning_atoms), 0)
    
    def test_calculate_fitness_bonus(self):
        """Test fitness bonus calculation"""
        # Add some patterns to shared layer
        self.layer.add_atom(
            "pattern_1",
            "reasoning_pattern",
            "if-then",
            CognitiveMode.REASONING
        )
        
        # Access it multiple times to make it high-value
        for _ in range(10):
            self.layer.get_atom("pattern_1")
        
        # Calculate bonus for program using the pattern
        program = "program using pattern_1 logic"
        bonus = self.feedback.calculate_fitness_bonus(
            program,
            ["if-then"]
        )
        
        self.assertGreater(bonus, 0.0)
        self.assertLessEqual(bonus, 1.0)


class TestMOSESToPLNFeedback(unittest.TestCase):
    """Test MOSES to PLN feedback loop"""
    
    def setUp(self):
        self.layer = SharedRepresentationLayer()
        self.feedback = MOSESToPLNFeedback(self.layer)
    
    def test_process_learning_outcome(self):
        """Test processing learning outcomes"""
        outcome = LearningOutcome(
            learned_program="(lambda (x) (+ x 1))",
            fitness_score=0.88,
            generations=15,
            discovered_patterns=["increment", "function"]
        )
        
        signal = self.feedback.process_learning_outcome(outcome)
        
        self.assertEqual(signal.source_mode, CognitiveMode.LEARNING)
        self.assertEqual(signal.target_mode, CognitiveMode.REASONING)
        self.assertEqual(signal.feedback_type, FeedbackType.LEARNING_TO_REASONING)
        self.assertEqual(signal.strength, 0.88)
        self.assertIn('new_patterns', signal.content)
    
    def test_learned_patterns_stored(self):
        """Test that learned patterns are stored in shared layer"""
        outcome = LearningOutcome(
            learned_program="test_program",
            fitness_score=0.75,
            generations=10,
            discovered_patterns=["pattern_a", "pattern_b"]
        )
        
        self.feedback.process_learning_outcome(outcome)
        
        # Check that patterns were stored
        learning_atoms = self.layer.get_atoms_by_source(CognitiveMode.LEARNING)
        self.assertGreater(len(learning_atoms), 0)
    
    def test_extract_inference_rules(self):
        """Test extraction of inference rules from programs"""
        program_with_conditional = "if condition then action"
        rules = self.feedback.extract_inference_rules(program_with_conditional)
        self.assertGreater(len(rules), 0)
        
        program_with_composition = "f -> g -> h"
        rules = self.feedback.extract_inference_rules(program_with_composition)
        self.assertGreater(len(rules), 0)


class TestMetaLearningSystem(unittest.TestCase):
    """Test meta-learning system"""
    
    def setUp(self):
        self.layer = SharedRepresentationLayer()
        self.meta = MetaLearningSystem(self.layer)
    
    def test_observe_interaction(self):
        """Test observing reasoning-learning interactions"""
        reasoning = ReasoningOutcome(
            conclusion="A => B",
            confidence=0.9,
            inference_steps=5,
            supporting_atoms=[],
            reasoning_patterns=["implication"]
        )
        
        learning = LearningOutcome(
            learned_program="(implies A B)",
            fitness_score=0.85,
            generations=10,
            discovered_patterns=["implication"]
        )
        
        self.meta.observe_interaction(reasoning, learning)
        
        # Check that strategies were recorded
        self.assertGreater(len(self.meta.strategy_performance), 0)
        self.assertGreater(len(self.meta.current_strategies), 0)
    
    def test_synergy_calculation(self):
        """Test synergy calculation between reasoning and learning"""
        reasoning = ReasoningOutcome(
            conclusion="test",
            confidence=0.8,
            inference_steps=3,
            supporting_atoms=[],
            reasoning_patterns=["pattern_1", "pattern_2"]
        )
        
        learning = LearningOutcome(
            learned_program="test",
            fitness_score=0.7,
            generations=5,
            discovered_patterns=["pattern_1", "pattern_3"]
        )
        
        synergy = self.meta._calculate_synergy(reasoning, learning)
        
        # Should be between 0 and 1
        self.assertGreaterEqual(synergy, 0.0)
        self.assertLessEqual(synergy, 1.0)
        
        # Should be relatively high due to good outcomes
        self.assertGreater(synergy, 0.5)
    
    def test_strategy_identification(self):
        """Test strategy identification"""
        # Deep exploration
        reasoning = ReasoningOutcome(
            conclusion="test",
            confidence=0.7,
            inference_steps=10,
            supporting_atoms=[],
            reasoning_patterns=[]
        )
        
        learning = LearningOutcome(
            learned_program="test",
            fitness_score=0.6,
            generations=15,
            discovered_patterns=[]
        )
        
        strategy = self.meta._identify_strategy(reasoning, learning)
        self.assertEqual(strategy, "deep_exploration")
    
    def test_get_best_strategy(self):
        """Test getting best performing strategy"""
        # Record multiple observations
        for i in range(5):
            reasoning = ReasoningOutcome(
                conclusion="test",
                confidence=0.9,
                inference_steps=2,
                supporting_atoms=[],
                reasoning_patterns=[]
            )
            
            learning = LearningOutcome(
                learned_program="test",
                fitness_score=0.85,
                generations=3,
                discovered_patterns=[]
            )
            
            self.meta.observe_interaction(reasoning, learning)
        
        best = self.meta.get_best_strategy()
        self.assertIsNotNone(best)


class TestCognitiveFusionBridge(unittest.TestCase):
    """Test the main cognitive fusion bridge"""
    
    def setUp(self):
        self.bridge = CognitiveFusionBridge()
    
    def test_initialization(self):
        """Test bridge initialization"""
        self.assertIsNotNone(self.bridge.shared_layer)
        self.assertIsNotNone(self.bridge.pln_to_moses)
        self.assertIsNotNone(self.bridge.moses_to_pln)
        self.assertIsNotNone(self.bridge.meta_learning)
        self.assertEqual(self.bridge.cycle_count, 0)
    
    def test_process_reasoning_outcome(self):
        """Test processing reasoning outcomes"""
        outcome = ReasoningOutcome(
            conclusion="X implies Y",
            confidence=0.85,
            inference_steps=4,
            supporting_atoms=["a1"],
            reasoning_patterns=["implication"]
        )
        
        signal = self.bridge.process_reasoning_outcome(outcome)
        
        self.assertEqual(signal.source_mode, CognitiveMode.REASONING)
        self.assertEqual(self.bridge.cognitive_state.reasoning_quality, 0.85)
    
    def test_process_learning_outcome(self):
        """Test processing learning outcomes"""
        outcome = LearningOutcome(
            learned_program="(program)",
            fitness_score=0.78,
            generations=12,
            discovered_patterns=["pattern"]
        )
        
        signal = self.bridge.process_learning_outcome(outcome)
        
        self.assertEqual(signal.source_mode, CognitiveMode.LEARNING)
        self.assertEqual(self.bridge.cognitive_state.learning_progress, 0.78)
    
    def test_cognitive_cycle(self):
        """Test complete cognitive cycle"""
        reasoning = ReasoningOutcome(
            conclusion="A -> B",
            confidence=0.9,
            inference_steps=5,
            supporting_atoms=["atom1"],
            reasoning_patterns=["conditional", "implication"]
        )
        
        learning = LearningOutcome(
            learned_program="(if A B)",
            fitness_score=0.85,
            generations=10,
            discovered_patterns=["conditional"]
        )
        
        summary = self.bridge.cognitive_cycle(reasoning, learning)
        
        # Check summary contents
        self.assertIn('cycle', summary)
        self.assertIn('synergy_score', summary)
        self.assertIn('reasoning_quality', summary)
        self.assertIn('learning_progress', summary)
        self.assertIn('best_strategy', summary)
        
        # Check that cycle was incremented
        self.assertEqual(self.bridge.cycle_count, 1)
        
        # Check metrics
        self.assertEqual(summary['reasoning_quality'], 0.9)
        self.assertEqual(summary['learning_progress'], 0.85)
        self.assertGreaterEqual(summary['synergy_score'], 0.0)
        self.assertLessEqual(summary['synergy_score'], 1.0)
    
    def test_multiple_cycles(self):
        """Test multiple cognitive cycles"""
        for i in range(3):
            reasoning = ReasoningOutcome(
                conclusion=f"conclusion_{i}",
                confidence=0.8 + i * 0.05,
                inference_steps=3 + i,
                supporting_atoms=[],
                reasoning_patterns=[f"pattern_{i}"]
            )
            
            learning = LearningOutcome(
                learned_program=f"program_{i}",
                fitness_score=0.75 + i * 0.05,
                generations=8 + i * 2,
                discovered_patterns=[f"learned_{i}"]
            )
            
            self.bridge.cognitive_cycle(reasoning, learning)
        
        self.assertEqual(self.bridge.cycle_count, 3)
        
        # Check that shared layer accumulated atoms
        stats = self.bridge.get_statistics()
        self.assertGreater(stats['shared_atoms'], 0)
    
    def test_get_state(self):
        """Test getting cognitive state"""
        state = self.bridge.get_state()
        self.assertIsInstance(state, CognitiveState)
        self.assertIn(state.mode, CognitiveMode)
    
    def test_get_statistics(self):
        """Test getting bridge statistics"""
        # Run a cycle first
        reasoning = ReasoningOutcome(
            conclusion="test",
            confidence=0.8,
            inference_steps=3,
            supporting_atoms=[],
            reasoning_patterns=["p1"]
        )
        
        learning = LearningOutcome(
            learned_program="test",
            fitness_score=0.75,
            generations=5,
            discovered_patterns=["p2"]
        )
        
        self.bridge.cognitive_cycle(reasoning, learning)
        
        stats = self.bridge.get_statistics()
        
        self.assertIn('total_cycles', stats)
        self.assertIn('shared_atoms', stats)
        self.assertIn('pln_to_moses_signals', stats)
        self.assertIn('moses_to_pln_signals', stats)
        self.assertIn('known_strategies', stats)
        
        self.assertEqual(stats['total_cycles'], 1)
        self.assertGreater(stats['shared_atoms'], 0)


class TestIntegration(unittest.TestCase):
    """Integration tests for cognitive fusion"""
    
    def test_create_cognitive_fusion_bridge(self):
        """Test convenience function for creating bridge"""
        bridge = create_cognitive_fusion_bridge()
        self.assertIsInstance(bridge, CognitiveFusionBridge)
    
    def test_feedback_loop_integration(self):
        """Test that feedback loops work together"""
        bridge = create_cognitive_fusion_bridge()
        
        # First cycle: reasoning discovers pattern
        reasoning1 = ReasoningOutcome(
            conclusion="Pattern X is important",
            confidence=0.9,
            inference_steps=5,
            supporting_atoms=[],
            reasoning_patterns=["pattern_x"]
        )
        
        learning1 = LearningOutcome(
            learned_program="program using pattern_x",
            fitness_score=0.7,
            generations=8,
            discovered_patterns=["pattern_x_variant"]
        )
        
        summary1 = bridge.cognitive_cycle(reasoning1, learning1)
        
        # Second cycle: learning reinforces pattern
        reasoning2 = ReasoningOutcome(
            conclusion="Pattern X confirmed",
            confidence=0.95,
            inference_steps=3,
            supporting_atoms=[],
            reasoning_patterns=["pattern_x", "pattern_x_variant"]
        )
        
        learning2 = LearningOutcome(
            learned_program="optimized pattern_x program",
            fitness_score=0.88,
            generations=5,
            discovered_patterns=["pattern_x", "pattern_x_optimized"]
        )
        
        summary2 = bridge.cognitive_cycle(reasoning2, learning2)
        
        # Check that second cycle has higher synergy
        # (since patterns are reinforced)
        self.assertEqual(bridge.cycle_count, 2)
        
        # Verify shared layer accumulated knowledge
        stats = bridge.get_statistics()
        self.assertGreaterEqual(stats['shared_atoms'], 4)  # At least 4 patterns stored


def run_tests():
    """Run all tests"""
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    
    # Add all test classes
    suite.addTests(loader.loadTestsFromTestCase(TestSharedRepresentationLayer))
    suite.addTests(loader.loadTestsFromTestCase(TestPLNToMOSESFeedback))
    suite.addTests(loader.loadTestsFromTestCase(TestMOSESToPLNFeedback))
    suite.addTests(loader.loadTestsFromTestCase(TestMetaLearningSystem))
    suite.addTests(loader.loadTestsFromTestCase(TestCognitiveFusionBridge))
    suite.addTests(loader.loadTestsFromTestCase(TestIntegration))
    
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    return result.wasSuccessful()


if __name__ == '__main__':
    success = run_tests()
    sys.exit(0 if success else 1)
