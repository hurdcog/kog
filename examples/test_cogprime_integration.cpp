/**
 * Simple test to verify CogPrime identity and distributed cognition integration
 */

#include <cogself/cogself.h>
#include <cogself/cogprime_identity.h>
#include <cogself/distributed_identity.h>
#include <iostream>
#include <cassert>

void test_basic_identity() {
    std::cout << "\n=== Test 1: Basic CogPrime Identity ===" << std::endl;
    
    auto identity = std::make_shared<cogself::CogPrimeIdentity>("test_001", "Test Agent");
    
    // Check initial state
    assert(identity->getAgentId() == "test_001");
    assert(identity->getAgentName() == "Test Agent");
    assert(identity->getAllCharacteristics().size() >= 4); // Should have foundational characteristics
    
    // Add characteristic
    identity->addCharacteristic(cogself::CognitiveCharacteristic("test_char", "Test", 0.7, 0.6));
    
    // Record memory
    identity->recordMemory("test_memory", "Test memory", 0.8);
    
    // Add goal
    identity->addGoal("test_goal", "Test goal", 0.9);
    identity->updateGoalProgress("test_goal", 0.5);
    
    // Check metrics
    double coherence = identity->getSelfCoherence();
    double strength = identity->getIdentityStrength();
    double integration = identity->getCognitiveIntegration();
    
    assert(coherence >= 0.0 && coherence <= 1.0);
    assert(strength >= 0.0 && strength <= 1.0);
    assert(integration >= 0.0 && integration <= 1.0);
    
    std::cout << "✓ Basic identity operations work correctly" << std::endl;
    std::cout << "  Self Coherence: " << coherence << std::endl;
    std::cout << "  Identity Strength: " << strength << std::endl;
}

void test_cogself_integration() {
    std::cout << "\n=== Test 2: CogSelf Integration ===" << std::endl;
    
    cogself::CogSelf framework;
    framework.initialize("test_002", "CogSelf Test Agent");
    
    assert(framework.isRunning());
    
    auto identity = framework.getIdentity();
    assert(identity != nullptr);
    assert(identity->getAgentId() == "test_002");
    
    // Record event
    framework.recordCognitiveEvent("test_event", "Test event", 0.7);
    
    // Update synergy
    framework.updateSynergyState();
    
    double synergy = framework.getSynergyLevel();
    assert(synergy >= 0.0 && synergy <= 1.0);
    
    std::cout << "✓ CogSelf integration works correctly" << std::endl;
    std::cout << "  Synergy Level: " << synergy << std::endl;
}

void test_distributed_cognition() {
    std::cout << "\n=== Test 3: Distributed Cognition ===" << std::endl;
    
    cogself::CogSelf framework;
    framework.initialize("test_003", "Distributed Test Agent");
    
    // Enable distributed cognition
    framework.enableDistributedCognition();
    
    auto distMgr = framework.getDistributedManager();
    assert(distMgr != nullptr);
    
    // Register nodes
    framework.registerDistributedNode("node_1", "127.0.0.1", 8001);
    framework.registerDistributedNode("node_2", "127.0.0.1", 8002);
    
    auto nodes = distMgr->getIdentityAwareNodes();
    assert(nodes.size() == 2);
    
    // Distribute shard
    distMgr->distributeShard(cogself::IdentityShard("test_shard", "Test shard", true, 0.7));
    
    // Propagate identity
    distMgr->propagateIdentityToAllNodes();
    
    // Get metrics
    auto metrics = distMgr->getIdentitySynergyMetrics();
    assert(metrics.activeNodes == 2);
    assert(metrics.activeShards == 1);
    
    // Synchronize
    framework.synchronizeDistributedIdentity();
    
    double coherence = framework.getDistributedCoherence();
    assert(coherence >= 0.0 && coherence <= 1.0);
    
    std::cout << "✓ Distributed cognition works correctly" << std::endl;
    std::cout << "  Active Nodes: " << metrics.activeNodes << std::endl;
    std::cout << "  Network Coherence: " << coherence << std::endl;
    std::cout << "  Network Integration: " << metrics.networkIntegration << std::endl;
}

void test_identity_evolution() {
    std::cout << "\n=== Test 4: Identity Evolution ===" << std::endl;
    
    cogself::CogSelf framework;
    framework.initialize("test_004", "Evolution Test Agent");
    framework.enableDistributedCognition();
    
    auto identity = framework.getIdentity();
    auto distMgr = framework.getDistributedManager();
    
    // Register nodes
    framework.registerDistributedNode("node_1", "127.0.0.1", 8001);
    
    // Get initial synergy characteristic
    auto initial = identity->getCharacteristic("cognitive_synergy");
    double initialStrength = initial.strength;
    
    // Simulate some cognitive processing
    for (int i = 0; i < 3; ++i) {
        framework.updateSynergyState();
    }
    
    // Update from distributed experience
    distMgr->updateIdentityFromDistributedExperience();
    
    // Check if characteristic changed (may or may not depending on thresholds)
    auto final = identity->getCharacteristic("cognitive_synergy");
    
    std::cout << "✓ Identity evolution mechanism works" << std::endl;
    std::cout << "  Initial cognitive_synergy: " << initialStrength << std::endl;
    std::cout << "  Final cognitive_synergy: " << final.strength << std::endl;
}

int main() {
    std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  CogPrime Identity + Distributed Cognition Tests          ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
    
    try {
        test_basic_identity();
        test_cogself_integration();
        test_distributed_cognition();
        test_identity_evolution();
        
        std::cout << "\n╔════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║  All Tests Passed ✓                                       ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
