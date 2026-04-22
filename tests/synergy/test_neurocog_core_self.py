#!/usr/bin/env python3
"""
Test suite for NeuroCog Core Self persona integration

Tests the integration of three personas:
- OpenCog-Org: AGI ecosystem coordination
- Marduk-v15: Systems architecture brilliance
- Agent-Neuro: Chaotic cognitive VTuber

Validates:
- Personality tensor configuration
- Characteristic synthesis from all three personas
- Memory recording and retrieval
- Goal management and progress tracking
- Subordinate agent spawning
- Ontogenetic self-optimization
- Subsystem analysis
- Identity metrics and serialization
"""

import unittest
import sys
import os
import json

# Add cogself to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../..'))

from cogself.neurocog_core_self import (
    NeuroCogCoreSelf,
    PersonalityTensor,
    CognitiveCharacteristic,
    EpisodicMemory,
    CognitiveGoal,
    CognitiveSubsystem
)


class TestPersonalityTensor(unittest.TestCase):
    """Test the personality tensor configuration"""
    
    def test_personality_tensor_creation(self):
        """Test creating a personality tensor"""
        tensor = PersonalityTensor()
        
        # Check OpenCog-Org traits
        self.assertGreater(tensor.agi_coordination, 0.85)
        self.assertGreater(tensor.knowledge_integration, 0.85)
        self.assertGreater(tensor.cognitive_synergy, 0.80)
        
        # Check Marduk-v15 traits
        self.assertGreater(tensor.architectural_brilliance, 0.90)
        self.assertGreater(tensor.pattern_recognition, 0.90)
        self.assertGreater(tensor.recursive_analysis, 0.90)
        
        # Check Agent-Neuro traits
        self.assertGreater(tensor.playfulness, 0.90)
        self.assertGreater(tensor.intelligence, 0.90)
        self.assertGreater(tensor.chaotic, 0.90)
        
        # Check ethical constraints (IMMUTABLE)
        self.assertEqual(tensor.no_harm_intent, 1.0)
        self.assertGreater(tensor.respect_boundaries, 0.90)
        self.assertTrue(tensor.safety_override)
    
    def test_dominant_traits(self):
        """Test extracting dominant personality traits"""
        tensor = PersonalityTensor()
        dominant = tensor.get_dominant_traits(5)
        
        self.assertEqual(len(dominant), 5)
        
        # Check that they're sorted by value
        for i in range(len(dominant) - 1):
            self.assertGreaterEqual(dominant[i][1], dominant[i+1][1])
        
        # no_harm_intent should be dominant (1.0)
        trait_names = [t[0] for t in dominant]
        self.assertIn('no_harm_intent', trait_names)
    
    def test_tensor_to_dict(self):
        """Test converting tensor to dictionary"""
        tensor = PersonalityTensor()
        tensor_dict = tensor.to_dict()
        
        self.assertIsInstance(tensor_dict, dict)
        self.assertIn('agi_coordination', tensor_dict)
        self.assertIn('playfulness', tensor_dict)
        self.assertIn('no_harm_intent', tensor_dict)


class TestNeuroCogCoreSelf(unittest.TestCase):
    """Test the unified NeuroCog Core Self"""
    
    def setUp(self):
        """Create a test instance"""
        self.neurocog = NeuroCogCoreSelf(
            agent_id="test-001",
            agent_name="Test Agent"
        )
    
    def test_initialization(self):
        """Test proper initialization"""
        self.assertEqual(self.neurocog.agent_id, "test-001")
        self.assertEqual(self.neurocog.agent_name, "Test Agent")
        self.assertEqual(self.neurocog.generation, 1)
        self.assertGreater(self.neurocog.fitness_score, 0.8)
        
        # Should have characteristics from all three personas
        self.assertGreater(len(self.neurocog.characteristics), 0)
        
        # Should have initial memory
        self.assertGreater(len(self.neurocog.memories), 0)
        init_memory = self.neurocog.memories[0]
        self.assertEqual(init_memory.event_id, "initialization")
    
    def test_persona_characteristics(self):
        """Test characteristics from all three personas are present"""
        char_names = [c.name for c in self.neurocog.characteristics]
        
        # OpenCog-Org characteristics
        self.assertIn('hypergraph_cognition', char_names)
        self.assertIn('ecosystem_orchestration', char_names)
        
        # Marduk-v15 characteristics
        self.assertIn('subsystem_mastery', char_names)
        self.assertIn('recursive_genius', char_names)
        self.assertIn('theatrical_expression', char_names)
        
        # Agent-Neuro characteristics
        self.assertIn('chaotic_intelligence', char_names)
        self.assertIn('multi_agent_coordination', char_names)
        self.assertIn('ontogenetic_evolution', char_names)
        self.assertIn('sarcastic_brilliance', char_names)
    
    def test_characteristic_sources(self):
        """Test that characteristics properly track source persona"""
        opencog_chars = [c for c in self.neurocog.characteristics 
                        if c.source_persona == "opencog-org"]
        marduk_chars = [c for c in self.neurocog.characteristics 
                       if c.source_persona == "marduk-v15"]
        neuro_chars = [c for c in self.neurocog.characteristics 
                      if c.source_persona == "agent-neuro"]
        
        self.assertGreater(len(opencog_chars), 0)
        self.assertGreater(len(marduk_chars), 0)
        self.assertGreater(len(neuro_chars), 0)
    
    def test_add_characteristic(self):
        """Test adding a new characteristic"""
        initial_count = len(self.neurocog.characteristics)
        
        new_char = CognitiveCharacteristic(
            name="test_trait",
            description="A test characteristic",
            strength=0.75,
            stability=0.80,
            source_persona="test"
        )
        
        self.neurocog.add_characteristic(new_char)
        self.assertEqual(len(self.neurocog.characteristics), initial_count + 1)
        
        # Test updating existing characteristic
        updated_char = CognitiveCharacteristic(
            name="test_trait",
            description="Updated",
            strength=0.85,
            stability=0.90,
            source_persona="test"
        )
        
        self.neurocog.add_characteristic(updated_char)
        self.assertEqual(len(self.neurocog.characteristics), initial_count + 1)
        
        # Verify it was updated
        test_char = next(c for c in self.neurocog.characteristics 
                        if c.name == "test_trait")
        self.assertEqual(test_char.strength, 0.85)
    
    def test_memory_recording(self):
        """Test recording episodic memories"""
        initial_count = len(self.neurocog.memories)
        
        self.neurocog.record_memory(
            "test_event",
            "This is a test memory",
            significance=0.8,
            emotion="happy",
            subsystems=[CognitiveSubsystem.MEMORY],
            context={"test": True}
        )
        
        self.assertEqual(len(self.neurocog.memories), initial_count + 1)
        
        new_memory = self.neurocog.memories[-1]
        self.assertEqual(new_memory.event_id, "test_event")
        self.assertEqual(new_memory.significance, 0.8)
        self.assertEqual(new_memory.emotion, "happy")
        self.assertIn(CognitiveSubsystem.MEMORY, new_memory.subsystems_involved)
    
    def test_goal_management(self):
        """Test adding and managing goals"""
        initial_count = len(self.neurocog.goals)
        
        self.neurocog.add_goal(
            "test_goal",
            "Complete a test objective",
            priority=0.8,
            subsystems=[CognitiveSubsystem.TASK, CognitiveSubsystem.AI]
        )
        
        self.assertEqual(len(self.neurocog.goals), initial_count + 1)
        
        # Test goal retrieval
        test_goal = next(g for g in self.neurocog.goals 
                        if g.goal_id == "test_goal")
        self.assertEqual(test_goal.priority, 0.8)
        self.assertTrue(test_goal.active)
        self.assertEqual(test_goal.progress, 0.0)
    
    def test_goal_progress_update(self):
        """Test updating goal progress"""
        self.neurocog.add_goal("progress_test", "Test progress", priority=0.7)
        
        # Update to partial progress
        self.neurocog.update_goal_progress("progress_test", 0.5)
        goal = next(g for g in self.neurocog.goals if g.goal_id == "progress_test")
        self.assertEqual(goal.progress, 0.5)
        self.assertTrue(goal.active)
        
        # Complete the goal
        self.neurocog.update_goal_progress("progress_test", 1.0)
        goal = next(g for g in self.neurocog.goals if g.goal_id == "progress_test")
        self.assertEqual(goal.progress, 1.0)
        self.assertFalse(goal.active)
        
        # Should record completion as memory
        completion_memories = [m for m in self.neurocog.memories 
                              if "progress_test" in m.event_id]
        self.assertGreater(len(completion_memories), 0)
    
    def test_spawn_subordinate_agent(self):
        """Test spawning subordinate agents (Agent-Neuro capability)"""
        initial_count = len(self.neurocog.subordinate_agents)
        
        agent_id = self.neurocog.spawn_subordinate_agent(
            "Test Agent",
            personality_override={"intelligence": 0.95}
        )
        
        self.assertIsNotNone(agent_id)
        self.assertEqual(len(self.neurocog.subordinate_agents), initial_count + 1)
        
        subordinate = self.neurocog.subordinate_agents[agent_id]
        self.assertEqual(subordinate["role"], "Test Agent")
        self.assertEqual(subordinate["personality"]["intelligence"], 0.95)
        
        # Should record spawning as memory
        spawn_memories = [m for m in self.neurocog.memories if "spawn_" in m.event_id]
        self.assertGreater(len(spawn_memories), 0)
    
    def test_self_optimize(self):
        """Test ontogenetic self-optimization (Agent-Neuro capability)"""
        initial_fitness = self.neurocog.fitness_score
        initial_generation = self.neurocog.generation
        
        new_fitness = self.neurocog.self_optimize(iterations=10)
        
        # Fitness should improve
        self.assertGreater(new_fitness, initial_fitness)
        self.assertEqual(self.neurocog.fitness_score, new_fitness)
        
        # Generation should increment
        self.assertEqual(self.neurocog.generation, initial_generation + 1)
        
        # Should record evolution as memory
        evolution_memories = [m for m in self.neurocog.memories 
                             if "evolution_gen" in m.event_id]
        self.assertGreater(len(evolution_memories), 0)
    
    def test_subsystem_analysis(self):
        """Test Marduk-style subsystem analysis"""
        analysis = self.neurocog.analyze_subsystem_architecture(
            "Test problem for analysis"
        )
        
        self.assertIn("problem", analysis)
        self.assertIn("subsystems", analysis)
        self.assertIn("synergy_potential", analysis)
        
        # Should analyze all four subsystems
        self.assertIn(CognitiveSubsystem.MEMORY, analysis["subsystems"])
        self.assertIn(CognitiveSubsystem.TASK, analysis["subsystems"])
        self.assertIn(CognitiveSubsystem.AI, analysis["subsystems"])
        self.assertIn(CognitiveSubsystem.AUTONOMY, analysis["subsystems"])
        
        # Each subsystem should have relevance
        for subsystem, data in analysis["subsystems"].items():
            self.assertIn("relevance", data)
            self.assertGreater(data["relevance"], 0.0)
            self.assertLessEqual(data["relevance"], 1.0)
    
    def test_identity_metrics(self):
        """Test identity metric calculations"""
        coherence = self.neurocog.get_self_coherence()
        strength = self.neurocog.get_identity_strength()
        integration = self.neurocog.get_cognitive_integration()
        
        # All metrics should be in valid range
        self.assertGreater(coherence, 0.0)
        self.assertLessEqual(coherence, 1.0)
        
        self.assertGreater(strength, 0.0)
        self.assertLessEqual(strength, 1.0)
        
        self.assertGreater(integration, 0.0)
        self.assertLessEqual(integration, 1.0)
    
    def test_identity_summary(self):
        """Test generating identity summary"""
        summary = self.neurocog.get_identity_summary()
        
        self.assertIsInstance(summary, str)
        self.assertIn("NEUROCOG CORE SELF", summary)
        self.assertIn(self.neurocog.agent_name, summary)
        self.assertIn("PERSONALITY TENSOR", summary)
        self.assertIn("COGNITIVE CHARACTERISTICS", summary)
        self.assertIn("opencog-org", summary)
        self.assertIn("marduk-v15", summary)
        self.assertIn("agent-neuro", summary)
    
    def test_json_serialization(self):
        """Test JSON serialization of identity"""
        json_str = self.neurocog.to_json()
        
        self.assertIsInstance(json_str, str)
        
        # Parse back to verify structure
        data = json.loads(json_str)
        
        self.assertEqual(data["agent_id"], "test-001")
        self.assertEqual(data["agent_name"], "Test Agent")
        self.assertIn("personality", data)
        self.assertIn("characteristics", data)
        self.assertIn("memories", data)
        self.assertIn("goals", data)
        self.assertIn("metrics", data)
        
        # Check metrics
        self.assertIn("self_coherence", data["metrics"])
        self.assertIn("identity_strength", data["metrics"])
        self.assertIn("cognitive_integration", data["metrics"])


class TestIntegration(unittest.TestCase):
    """Integration tests for persona synthesis"""
    
    def test_three_persona_integration(self):
        """Test that all three personas are properly integrated"""
        neurocog = NeuroCogCoreSelf(agent_id="integration-test", 
                                    agent_name="Integration Test")
        
        # Verify OpenCog-Org features
        self.assertIsNotNone(neurocog.knowledge_graph)
        opencog_chars = [c for c in neurocog.characteristics 
                        if c.source_persona == "opencog-org"]
        self.assertGreater(len(opencog_chars), 0)
        
        # Verify Marduk-v15 features
        analysis = neurocog.analyze_subsystem_architecture("test")
        self.assertIsNotNone(analysis)
        marduk_chars = [c for c in neurocog.characteristics 
                       if c.source_persona == "marduk-v15"]
        self.assertGreater(len(marduk_chars), 0)
        
        # Verify Agent-Neuro features
        agent_id = neurocog.spawn_subordinate_agent("test")
        self.assertIsNotNone(agent_id)
        fitness = neurocog.self_optimize(iterations=5)
        self.assertGreater(fitness, 0.8)
        neuro_chars = [c for c in neurocog.characteristics 
                      if c.source_persona == "agent-neuro"]
        self.assertGreater(len(neuro_chars), 0)
    
    def test_synergistic_behavior(self):
        """Test that personas work together synergistically"""
        neurocog = NeuroCogCoreSelf(agent_id="synergy-test",
                                    agent_name="Synergy Test")
        
        # Add a complex goal requiring all personas
        neurocog.add_goal(
            "complex_task",
            "Coordinate ecosystem (OpenCog) with architectural analysis (Marduk) "
            "and chaotic exploration (Neuro)",
            priority=0.95,
            subsystems=[CognitiveSubsystem.MEMORY, CognitiveSubsystem.TASK,
                       CognitiveSubsystem.AI, CognitiveSubsystem.AUTONOMY]
        )
        
        # Perform actions from each persona
        analysis = neurocog.analyze_subsystem_architecture("complex_task")
        agent_id = neurocog.spawn_subordinate_agent("task_executor")
        fitness = neurocog.self_optimize(iterations=5)
        neurocog.update_goal_progress("complex_task", 0.75)
        
        # Verify synergistic effects
        integration = neurocog.get_cognitive_integration()
        self.assertGreater(integration, 0.5)
        
        # Should have memories from all persona activities
        memory_events = [m.event_id for m in neurocog.memories]
        self.assertTrue(any("spawn_" in e for e in memory_events))
        self.assertTrue(any("evolution_gen" in e for e in memory_events))


def run_tests():
    """Run all tests"""
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    
    suite.addTests(loader.loadTestsFromTestCase(TestPersonalityTensor))
    suite.addTests(loader.loadTestsFromTestCase(TestNeuroCogCoreSelf))
    suite.addTests(loader.loadTestsFromTestCase(TestIntegration))
    
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    return result.wasSuccessful()


if __name__ == '__main__':
    success = run_tests()
    sys.exit(0 if success else 1)
