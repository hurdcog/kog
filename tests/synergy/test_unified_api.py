#!/usr/bin/env python3
"""
Comprehensive Integration Tests for Unified Synergy API

Tests the integration of AAR Core, Hypergraph Identity, and Membrane Manager
through the unified CognitiveSynergy interface.
"""

import sys
from pathlib import Path
import numpy as np

# Add synergy module to path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

try:
    from synergy.api import CognitiveSynergy, SynergyConfig
    SYNERGY_AVAILABLE = True
except ImportError as e:
    print(f"Warning: Could not import synergy API: {e}")
    SYNERGY_AVAILABLE = False


def test_initialization():
    """Test that CognitiveSynergy initializes correctly."""
    if not SYNERGY_AVAILABLE:
        print("SKIP: test_initialization (synergy not available)")
        return False
    
    try:
        cs = CognitiveSynergy()
        assert cs.cycle_count == 0
        assert cs.current_state == "initialized"
        print("✓ PASS: test_initialization")
        return True
    except Exception as e:
        print(f"✗ FAIL: test_initialization - {e}")
        return False


def test_custom_config():
    """Test initialization with custom configuration."""
    if not SYNERGY_AVAILABLE:
        print("SKIP: test_custom_config (synergy not available)")
        return False
    
    try:
        config = SynergyConfig(
            aar_dimension=32,
            aar_learning_rate=0.05,
            enable_hypergraph=True,
            enable_membranes=True
        )
        cs = CognitiveSynergy(config=config)
        assert cs.config.aar_dimension == 32
        assert cs.config.aar_learning_rate == 0.05
        print("✓ PASS: test_custom_config")
        return True
    except Exception as e:
        print(f"✗ FAIL: test_custom_config - {e}")
        return False


def test_perception():
    """Test perception processing."""
    if not SYNERGY_AVAILABLE:
        print("SKIP: test_perception (synergy not available)")
        return False
    
    try:
        cs = CognitiveSynergy()
        perception = np.random.randn(64)
        results = cs.perceive(perception)
        
        assert "components" in results
        assert "aar" in results["components"]
        assert results["components"]["aar"]["processed"] == True
        print("✓ PASS: test_perception")
        return True
    except Exception as e:
        print(f"✗ FAIL: test_perception - {e}")
        return False


def test_action_generation():
    """Test action generation."""
    if not SYNERGY_AVAILABLE:
        print("SKIP: test_action_generation (synergy not available)")
        return False
    
    try:
        cs = CognitiveSynergy()
        perception = np.random.randn(64)
        cs.perceive(perception)
        
        action = cs.act()
        assert action is not None
        assert len(action) == 64  # Should match AAR dimension
        print("✓ PASS: test_action_generation")
        return True
    except Exception as e:
        print(f"✗ FAIL: test_action_generation - {e}")
        return False


def test_reflection():
    """Test reflection and metrics collection."""
    if not SYNERGY_AVAILABLE:
        print("SKIP: test_reflection (synergy not available)")
        return False
    
    try:
        cs = CognitiveSynergy()
        perception = np.random.randn(64)
        cs.perceive(perception)
        cs.act()
        
        metrics = cs.reflect()
        assert "aar" in metrics
        assert "emergence_score" in metrics["aar"]
        assert "coherence" in metrics["aar"]
        print("✓ PASS: test_reflection")
        return True
    except Exception as e:
        print(f"✗ FAIL: test_reflection - {e}")
        return False


def test_complete_cognitive_cycle():
    """Test a complete cognitive cycle."""
    if not SYNERGY_AVAILABLE:
        print("SKIP: test_complete_cognitive_cycle (synergy not available)")
        return False
    
    try:
        cs = CognitiveSynergy()
        perception = np.random.randn(64)
        
        results = cs.cognitive_cycle(perception)
        
        assert "cycle" in results
        assert "perception" in results
        assert "processing" in results
        assert "action" in results
        assert "metrics" in results
        assert cs.cycle_count == 1
        print("✓ PASS: test_complete_cognitive_cycle")
        return True
    except Exception as e:
        print(f"✗ FAIL: test_complete_cognitive_cycle - {e}")
        return False


def test_multiple_cycles():
    """Test multiple cognitive cycles."""
    if not SYNERGY_AVAILABLE:
        print("SKIP: test_multiple_cycles (synergy not available)")
        return False
    
    try:
        cs = CognitiveSynergy()
        
        for i in range(5):
            perception = np.random.randn(64)
            results = cs.cognitive_cycle(perception)
            assert results["cycle"] == i
        
        assert cs.cycle_count == 5
        print("✓ PASS: test_multiple_cycles")
        return True
    except Exception as e:
        print(f"✗ FAIL: test_multiple_cycles - {e}")
        return False


def test_synergy_score():
    """Test synergy score calculation."""
    if not SYNERGY_AVAILABLE:
        print("SKIP: test_synergy_score (synergy not available)")
        return False
    
    try:
        cs = CognitiveSynergy()
        perception = np.random.randn(64)
        cs.cognitive_cycle(perception)
        
        score = cs.get_synergy_score()
        assert 0.0 <= score <= 1.0
        print(f"✓ PASS: test_synergy_score (score: {score:.4f})")
        return True
    except Exception as e:
        print(f"✗ FAIL: test_synergy_score - {e}")
        return False


def test_metrics_history():
    """Test metrics history collection."""
    if not SYNERGY_AVAILABLE:
        print("SKIP: test_metrics_history (synergy not available)")
        return False
    
    try:
        config = SynergyConfig(collect_metrics=True, metrics_history_size=10)
        cs = CognitiveSynergy(config=config)
        
        for _ in range(15):
            perception = np.random.randn(64)
            cs.cognitive_cycle(perception)
        
        assert len(cs.metrics_history) == 10  # Should cap at history size
        print("✓ PASS: test_metrics_history")
        return True
    except Exception as e:
        print(f"✗ FAIL: test_metrics_history - {e}")
        return False


def test_get_all_metrics():
    """Test comprehensive metrics retrieval."""
    if not SYNERGY_AVAILABLE:
        print("SKIP: test_get_all_metrics (synergy not available)")
        return False
    
    try:
        cs = CognitiveSynergy()
        perception = np.random.randn(64)
        cs.cognitive_cycle(perception)
        
        all_metrics = cs.get_all_metrics()
        
        assert "cycle_count" in all_metrics
        assert "current_state" in all_metrics
        assert "aar" in all_metrics
        assert "config" in all_metrics
        print("✓ PASS: test_get_all_metrics")
        return True
    except Exception as e:
        print(f"✗ FAIL: test_get_all_metrics - {e}")
        return False


def test_reset():
    """Test system reset functionality."""
    if not SYNERGY_AVAILABLE:
        print("SKIP: test_reset (synergy not available)")
        return False
    
    try:
        cs = CognitiveSynergy()
        
        # Run some cycles
        for _ in range(3):
            perception = np.random.randn(64)
            cs.cognitive_cycle(perception)
        
        assert cs.cycle_count == 3
        
        # Reset
        cs.reset()
        
        assert cs.cycle_count == 0
        assert cs.current_state == "initialized"
        assert len(cs.metrics_history) == 0
        print("✓ PASS: test_reset")
        return True
    except Exception as e:
        print(f"✗ FAIL: test_reset - {e}")
        return False


def test_hypergraph_integration():
    """Test hypergraph identity integration."""
    if not SYNERGY_AVAILABLE:
        print("SKIP: test_hypergraph_integration (synergy not available)")
        return False
    
    try:
        config = SynergyConfig(enable_hypergraph=True, hypergraph_auto_refine=True)
        cs = CognitiveSynergy(config=config)
        
        # Run cycles to build identity
        for _ in range(5):
            perception = np.random.randn(64)
            cs.cognitive_cycle(perception)
        
        metrics = cs.get_all_metrics()
        assert "hypergraph" in metrics
        assert metrics["hypergraph"]["total_nodes"] > 0
        print("✓ PASS: test_hypergraph_integration")
        return True
    except Exception as e:
        print(f"✗ FAIL: test_hypergraph_integration - {e}")
        return False


def test_membrane_integration():
    """Test membrane manager integration."""
    if not SYNERGY_AVAILABLE:
        print("SKIP: test_membrane_integration (synergy not available)")
        return False
    
    try:
        config = SynergyConfig(enable_membranes=True)
        cs = CognitiveSynergy(config=config)
        
        perception = np.random.randn(64)
        results = cs.perceive(perception)
        
        assert "membrane" in results["components"]
        assert results["components"]["membrane"]["stored"] == True
        print("✓ PASS: test_membrane_integration")
        return True
    except Exception as e:
        print(f"✗ FAIL: test_membrane_integration - {e}")
        return False


def run_all_tests():
    """Run all integration tests."""
    print("=" * 60)
    print("Unified Synergy API Integration Tests")
    print("=" * 60)
    print()
    
    tests = [
        test_initialization,
        test_custom_config,
        test_perception,
        test_action_generation,
        test_reflection,
        test_complete_cognitive_cycle,
        test_multiple_cycles,
        test_synergy_score,
        test_metrics_history,
        test_get_all_metrics,
        test_reset,
        test_hypergraph_integration,
        test_membrane_integration
    ]
    
    passed = 0
    failed = 0
    skipped = 0
    
    for test in tests:
        result = test()
        if result is True:
            passed += 1
        elif result is False:
            failed += 1
        else:
            skipped += 1
    
    print()
    print("=" * 60)
    print(f"Results: {passed} passed, {failed} failed, {skipped} skipped")
    print("=" * 60)
    
    return failed == 0


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
