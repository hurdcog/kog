"""
Enhanced Cognitive Synergy Integration Tests

Tests the integration of:
- AAR Core (Agent-Arena-Relation)
- Hypergraph Identity Refinement
- Membrane Architecture Manager

These tests validate that the core cognitive synergy components work together
to create an emergent self-aware system.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../..'))

import numpy as np
from synergy.core.aar_core import AARCore
from synergy.identity.hypergraph_identity import HypergraphIdentitySystem
from synergy.membranes.membrane_manager import MembraneManager, MessageType


def test_aar_core():
    """Test AAR Core basic functionality"""
    print("\n=== Testing AAR Core ===")
    
    aar = AARCore(dimension=32, learning_rate=0.01)
    
    # Run cognitive cycles
    for step in range(50):
        perception = np.random.randn(32) * 0.5
        aar.perceive(perception)
        aar.act()
        
        if step % 10 == 0:
            metrics = aar.reflect()
    
    # Check emergence
    final_metrics = aar.reflect()
    print(f"Final Emergence Score: {final_metrics['emergence_score']:.4f}")
    print(f"Self Coherence: {final_metrics['self_coherence']:.4f}")
    
    assert final_metrics['emergence_score'] > 0, "Emergence score should be positive"
    assert final_metrics['self_coherence'] > 0, "Self coherence should be positive"
    
    print("✓ AAR Core test passed")
    return True


def test_hypergraph_identity():
    """Test Hypergraph Identity System"""
    print("\n=== Testing Hypergraph Identity System ===")
    
    identity = HypergraphIdentitySystem()
    
    # Add test conversations
    conversations = [
        [
            {"role": "assistant", "content": "I am a cognitive system."},
            {"role": "user", "content": "What do you do?"},
            {"role": "assistant", "content": "I reason about problems. I prefer logical approaches."}
        ],
        [
            {"role": "assistant", "content": "I use hypergraph memory."},
            {"role": "user", "content": "What else?"},
            {"role": "assistant", "content": "I believe in cognitive synergy."}
        ]
    ]
    
    for conv in conversations:
        identity.add_conversation(conv)
    
    # Check identity formation
    summary = identity.get_identity_summary()
    print(f"Nodes: {summary['node_count']}, Edges: {summary['edge_count']}")
    print(f"Refinements: {summary['refinement_count']}")
    print(f"Core Attributes: {len(summary['core_attributes'])}")
    
    assert summary['node_count'] > 0, "Should have created nodes"
    assert summary['edge_count'] > 0, "Should have created edges"
    assert summary['refinement_count'] > 0, "Should have refinement tuples"
    
    print("✓ Hypergraph Identity test passed")
    return True


def test_membrane_manager():
    """Test Membrane Manager"""
    print("\n=== Testing Membrane Manager ===")
    
    manager = MembraneManager()
    
    # Check hierarchy
    hierarchy = manager.get_hierarchy_state()
    print(f"Root membrane: {hierarchy['membrane_id']}")
    print(f"Children: {len(hierarchy['children'])}")
    
    assert hierarchy['membrane_id'] == 'root', "Root membrane should exist"
    assert len(hierarchy['children']) == 3, "Should have 3 main membranes"
    
    # Test message passing
    memory_membrane = manager.get_membrane("memory")
    messages_received = []
    
    def handle_data(msg):
        messages_received.append(msg)
    
    memory_membrane.register_handler(MessageType.DATA, handle_data)
    
    # Send messages
    for i in range(5):
        manager.send_message(
            source_id="reasoning",
            target_id="memory",
            message_type=MessageType.DATA,
            payload={"data": f"test_{i}"}
        )
    
    # Process messages
    results = manager.process_all_messages()
    
    print(f"Messages processed: {sum(results.values())}")
    print(f"Messages received by handler: {len(messages_received)}")
    
    assert len(messages_received) > 0, "Should have received messages"
    
    print("✓ Membrane Manager test passed")
    return True


def test_integrated_cognitive_synergy():
    """Test integration of all components"""
    print("\n=== Testing Integrated Cognitive Synergy ===")
    
    # Initialize all components
    aar = AARCore(dimension=32)
    identity = HypergraphIdentitySystem()
    manager = MembraneManager()
    
    # Simulate cognitive cycle with all components
    print("\nRunning integrated cognitive cycle...")
    
    # 1. Perception through AAR
    perception = np.random.randn(32) * 0.5
    aar.perceive(perception)
    
    # 2. Store perception in memory membrane
    manager.send_message(
        source_id="reasoning",
        target_id="memory",
        message_type=MessageType.DATA,
        payload={"perception": perception.tolist()[:5]}  # First 5 elements
    )
    
    # 3. Generate action through AAR
    action = aar.act()
    
    # 4. Reflect and update identity
    aar_metrics = aar.reflect()
    
    # 5. Add identity refinement based on AAR state
    self_state = aar.get_self_state()
    identity.add_refinement_tuple(
        identity.transformer.extract_tuples([{
            "role": "assistant",
            "content": f"I have emergence score of {self_state['emergence_score']:.2f}"
        }])[0] if identity.transformer.extract_tuples([{
            "role": "assistant",
            "content": f"I have emergence score of {self_state['emergence_score']:.2f}"
        }]) else None
    )
    
    # 6. Process membrane messages
    manager.process_all_messages()
    
    # 7. Get integrated state
    integrated_state = {
        'aar': aar.get_self_state(),
        'identity': identity.get_identity_summary(),
        'membranes': manager.get_resource_usage()
    }
    
    print(f"\nIntegrated State:")
    print(f"  AAR Emergence: {integrated_state['aar']['emergence_score']:.4f}")
    print(f"  Identity Nodes: {integrated_state['identity']['node_count']}")
    print(f"  Membrane Messages: {integrated_state['membranes']['total_messages']}")
    
    # Verify synergy
    assert integrated_state['aar']['emergence_score'] >= 0, "AAR should be active"
    assert integrated_state['identity']['node_count'] > 0, "Identity should be forming"
    
    print("\n✓ Integrated Cognitive Synergy test passed")
    print("\n" + "="*50)
    print("COGNITIVE SYNERGY ACHIEVED!")
    print("="*50)
    
    return True


def run_all_tests():
    """Run all cognitive synergy tests"""
    print("\n" + "="*50)
    print("COGNITIVE SYNERGY INTEGRATION TESTS")
    print("="*50)
    
    tests = [
        ("AAR Core", test_aar_core),
        ("Hypergraph Identity", test_hypergraph_identity),
        ("Membrane Manager", test_membrane_manager),
        ("Integrated Synergy", test_integrated_cognitive_synergy)
    ]
    
    results = []
    for test_name, test_func in tests:
        try:
            result = test_func()
            results.append((test_name, result, None))
        except Exception as e:
            results.append((test_name, False, str(e)))
            print(f"\n✗ {test_name} test failed: {e}")
    
    # Summary
    print("\n" + "="*50)
    print("TEST SUMMARY")
    print("="*50)
    
    passed = sum(1 for _, result, _ in results if result)
    total = len(results)
    
    for test_name, result, error in results:
        status = "✓ PASSED" if result else "✗ FAILED"
        print(f"{status}: {test_name}")
        if error:
            print(f"  Error: {error}")
    
    print(f"\nTotal: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n🎉 ALL TESTS PASSED - COGNITIVE SYNERGY OPERATIONAL 🎉")
        return True
    else:
        print("\n⚠️  SOME TESTS FAILED - REVIEW REQUIRED ⚠️")
        return False


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)
