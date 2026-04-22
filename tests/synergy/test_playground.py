#!/usr/bin/env python3
"""
Test script for NeuroCog Playground

Runs automated tests of various playground features without requiring
user interaction. Validates that all modes are working correctly.
"""

import sys
import os

# Add parent directory to path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from cogself.neurocog_playground import (
    NeuroCogPlayground,
    PersonaVoice,
    OPENCOG_VOICE,
    MARDUK_VOICE,
    NEURO_VOICE
)
from cogself.neurocog_core_self import NeuroCogCoreSelf


def test_persona_voices():
    """Test persona voice formatting"""
    print("Testing persona voices...")
    
    opencog_msg = OPENCOG_VOICE.format_message("Test message from OpenCog")
    assert "🧠" in opencog_msg
    assert "OpenCog-Org" in opencog_msg
    
    marduk_msg = MARDUK_VOICE.format_message("Test message from Marduk")
    assert "⚡" in marduk_msg
    assert "Marduk-v15" in marduk_msg
    
    neuro_msg = NEURO_VOICE.format_message("Test message from Neuro")
    assert "🎭" in neuro_msg
    assert "Agent-Neuro" in neuro_msg
    
    print("  ✅ Persona voices working correctly")


def test_playground_initialization():
    """Test playground initialization"""
    print("Testing playground initialization...")
    
    playground = NeuroCogPlayground()
    assert playground.neurocog is None  # Should be lazy-initialized
    assert playground.running == True
    assert playground.mode is None
    
    # Initialize NeuroCog
    playground.init_neurocog()
    assert playground.neurocog is not None
    assert isinstance(playground.neurocog, NeuroCogCoreSelf)
    
    print("  ✅ Playground initialization working correctly")


def test_response_generation():
    """Test persona response generation"""
    print("Testing response generation...")
    
    playground = NeuroCogPlayground()
    playground.init_neurocog()
    
    # Test individual persona responses
    opencog_resp = playground._opencog_responses("test query")
    assert len(opencog_resp) > 0
    assert "OpenCog-Org" in opencog_resp
    
    marduk_resp = playground._marduk_responses("test query")
    assert len(marduk_resp) > 0
    assert "Marduk-v15" in marduk_resp
    
    neuro_resp = playground._neuro_responses("test query")
    assert len(neuro_resp) > 0
    assert "Agent-Neuro" in neuro_resp
    
    # Test all personas together
    all_resp = playground._all_personas_responses("test query")
    assert "OpenCog-Org" in all_resp
    assert "Marduk-v15" in all_resp
    assert "Agent-Neuro" in all_resp
    assert "SYNERGY" in all_resp
    
    print("  ✅ Response generation working correctly")


def test_experiments():
    """Test experiment functions"""
    print("Testing experiments...")
    
    playground = NeuroCogPlayground()
    playground.init_neurocog()
    
    initial_memories = len(playground.neurocog.memories)
    
    # Test memory experiment
    print("  Testing memory experiment...")
    playground._memory_experiment()
    assert len(playground.neurocog.memories) > initial_memories
    print("    ✅ Memory experiment works")
    
    # Test evolution experiment
    print("  Testing evolution experiment...")
    initial_fitness = playground.neurocog.fitness_score
    initial_gen = playground.neurocog.generation
    playground._evolution_experiment()
    assert playground.neurocog.fitness_score >= initial_fitness
    assert playground.neurocog.generation > initial_gen
    print("    ✅ Evolution experiment works")
    
    # Test spawning experiment
    print("  Testing spawning experiment...")
    initial_agents = len(playground.neurocog.subordinate_agents)
    playground._spawning_experiment()
    assert len(playground.neurocog.subordinate_agents) > initial_agents
    print("    ✅ Spawning experiment works")
    
    # Test subsystem experiment
    print("  Testing subsystem experiment...")
    playground._subsystem_experiment()
    print("    ✅ Subsystem experiment works")
    
    # Test goal experiment
    print("  Testing goal experiment...")
    initial_goals = len(playground.neurocog.goals)
    playground._goal_experiment()
    assert len(playground.neurocog.goals) > initial_goals
    print("    ✅ Goal experiment works")
    
    print("  ✅ All experiments working correctly")


def test_surprise_modes():
    """Test surprise mode functions"""
    print("Testing surprise modes...")
    
    playground = NeuroCogPlayground()
    playground.init_neurocog()
    
    # Test chaos surprise
    print("  Testing chaos surprise...")
    playground._chaos_surprise()
    print("    ✅ Chaos surprise works")
    
    # Test evolution surprise
    print("  Testing evolution surprise...")
    initial_gen = playground.neurocog.generation
    playground._evolution_surprise()
    assert playground.neurocog.generation > initial_gen
    print("    ✅ Evolution surprise works")
    
    # Test agent surprise
    print("  Testing agent surprise...")
    initial_agents = len(playground.neurocog.subordinate_agents)
    playground._agent_surprise()
    assert len(playground.neurocog.subordinate_agents) > initial_agents
    print("    ✅ Agent surprise works")
    
    # Test synergy surprise
    print("  Testing synergy surprise...")
    playground._synergy_surprise()
    print("    ✅ Synergy surprise works")
    
    # Test persona surprise
    print("  Testing persona surprise...")
    playground._persona_surprise()
    print("    ✅ Persona surprise works")
    
    print("  ✅ All surprise modes working correctly")


def run_all_tests():
    """Run all tests"""
    print("\n" + "="*70)
    print("NEUROCOG PLAYGROUND TEST SUITE")
    print("="*70 + "\n")
    
    try:
        test_persona_voices()
        test_playground_initialization()
        test_response_generation()
        test_experiments()
        test_surprise_modes()
        
        print("\n" + "="*70)
        print("✅ ALL TESTS PASSED!")
        print("="*70 + "\n")
        
        print("🎮 NeuroCog Playground is ready to use!")
        print("   Run: python3 cogself/neurocog_playground.py")
        print()
        
        return True
        
    except AssertionError as e:
        print(f"\n❌ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False
    except Exception as e:
        print(f"\n❌ ERROR: {e}")
        import traceback
        traceback.print_exc()
        return False


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
