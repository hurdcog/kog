/**
 * CogPrime Identity + Distributed Cognition Integration Example
 * 
 * Demonstrates the integration of:
 * - CogPrime identity system with cognitive characteristics, memories, and goals
 * - Distributed cognition across network nodes
 * - Identity-aware shard distribution
 * - Cognitive synergy through identity coherence
 */

#include <cogself/cogself.h>
#include <cogself/cogprime_identity.h>
#include <cogself/distributed_identity.h>
#include <iostream>
#include <thread>
#include <chrono>

void printSeparator(const std::string& title = "") {
    std::cout << "\n" << std::string(70, '=') << "\n";
    if (!title.empty()) {
        std::cout << "  " << title << "\n";
        std::cout << std::string(70, '=') << "\n";
    }
}

void demo_basic_identity() {
    printSeparator("Demo 1: Basic CogPrime Identity");
    
    // Create a CogPrime identity
    auto identity = std::make_shared<cogself::CogPrimeIdentity>(
        "agent_001", 
        "Cognitive Agent Alpha"
    );
    
    std::cout << "\nInitial Identity State:\n";
    std::cout << identity->getIdentitySummary() << std::endl;
    
    // Add custom characteristics
    identity->addCharacteristic(cogself::CognitiveCharacteristic(
        "creativity",
        "Ability to generate novel solutions",
        0.6, 0.5
    ));
    
    identity->addCharacteristic(cogself::CognitiveCharacteristic(
        "analytical_thinking",
        "Capacity for logical reasoning and analysis",
        0.7, 0.8
    ));
    
    // Record some experiences
    identity->recordMemory("first_task", "Completed initial reasoning task", 0.7);
    identity->recordMemory("pattern_discovery", "Discovered novel pattern in data", 0.85);
    identity->recordMemory("collaboration", "Collaborated with other cognitive agents", 0.6);
    
    // Add goals
    identity->addGoal("master_reasoning", "Master advanced reasoning capabilities", 0.9);
    identity->addGoal("develop_creativity", "Enhance creative problem-solving", 0.75);
    
    std::cout << "\nEnhanced Identity State:\n";
    std::cout << identity->getIdentitySummary() << std::endl;
    
    // Show cognitive metrics
    std::cout << "\nCognitive Metrics:\n";
    std::cout << "  Self Coherence: " << identity->getSelfCoherence() << "\n";
    std::cout << "  Identity Strength: " << identity->getIdentityStrength() << "\n";
    std::cout << "  Cognitive Integration: " << identity->getCognitiveIntegration() << "\n";
}

void demo_integrated_cogself() {
    printSeparator("Demo 2: CogSelf with Integrated Identity");
    
    // Create CogSelf framework with identity
    cogself::CogSelf framework;
    framework.initialize("agent_002", "CogSelf Integrated Agent");
    
    std::cout << "\nInitial Framework State:\n";
    std::cout << framework.getIdentitySummary() << std::endl;
    
    // Record cognitive events
    framework.recordCognitiveEvent("startup_complete", 
        "CogSelf framework initialized successfully", 0.8);
    
    // Update synergy state multiple times
    for (int i = 0; i < 5; ++i) {
        framework.updateSynergyState();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Generate improvement plan
    std::cout << "\n" << framework.generateImprovementPlan() << std::endl;
}

void demo_distributed_cognition() {
    printSeparator("Demo 3: Distributed Cognition with Identity");
    
    // Create framework with identity
    cogself::CogSelf framework;
    framework.initialize("agent_003", "Distributed Cognitive Agent");
    
    // Enable distributed cognition
    framework.enableDistributedCognition();
    
    // Register multiple distributed nodes
    framework.registerDistributedNode("node_alpha", "10.0.0.1", 8001);
    framework.registerDistributedNode("node_beta", "10.0.0.2", 8002);
    framework.registerDistributedNode("node_gamma", "10.0.0.3", 8003);
    
    std::cout << "\nDistributed nodes registered\n";
    
    // Get distributed manager for more detailed operations
    auto distMgr = framework.getDistributedManager();
    
    // Distribute identity-aware shards
    distMgr->distributeShard(cogself::IdentityShard(
        "reasoning_shard", 
        "Logical reasoning and inference",
        true,   // requires identity
        0.8     // high identity dependency
    ));
    
    distMgr->distributeShard(cogself::IdentityShard(
        "learning_shard",
        "Pattern recognition and learning",
        true,
        0.7
    ));
    
    distMgr->distributeShard(cogself::IdentityShard(
        "memory_shard",
        "Episodic memory management",
        true,
        0.9     // very high identity dependency
    ));
    
    // Propagate identity to all nodes
    distMgr->propagateIdentityToAllNodes();
    
    // Synchronize identity across network
    framework.synchronizeDistributedIdentity();
    
    // Get identity synergy metrics
    auto metrics = distMgr->getIdentitySynergyMetrics();
    
    std::cout << "\nDistributed Identity Synergy Metrics:\n";
    std::cout << "  Overall Coherence: " << metrics.overallCoherence << "\n";
    std::cout << "  Distributed Alignment: " << metrics.distributedAlignment << "\n";
    std::cout << "  Identity Stability: " << metrics.identityStability << "\n";
    std::cout << "  Network Integration: " << metrics.networkIntegration << "\n";
    std::cout << "  Active Nodes: " << metrics.activeNodes << "\n";
    std::cout << "  Active Shards: " << metrics.activeShards << "\n";
    
    // Record distributed cognitive events
    distMgr->recordDistributedCognitiveEvent("node_alpha", "reasoning_complete",
        "Completed complex reasoning task");
    distMgr->recordDistributedCognitiveEvent("node_beta", "pattern_learned",
        "Learned new pattern from data");
    
    // Update identity from distributed experience
    distMgr->updateIdentityFromDistributedExperience();
    
    // Validate identity integrity across network
    bool valid = distMgr->validateIdentityIntegrity();
    std::cout << "\nIdentity Integrity: " << (valid ? "VALID" : "INVALID") << "\n";
    
    // Get comprehensive summary
    std::cout << "\n" << distMgr->getDistributedIdentitySummary() << std::endl;
}

void demo_identity_evolution() {
    printSeparator("Demo 4: Identity Evolution Through Distributed Processing");
    
    cogself::CogSelf framework;
    framework.initialize("agent_004", "Evolving Cognitive Agent");
    framework.enableDistributedCognition();
    
    auto identity = framework.getIdentity();
    auto distMgr = framework.getDistributedManager();
    
    // Register nodes
    for (int i = 1; i <= 5; ++i) {
        std::string nodeId = "node_" + std::to_string(i);
        framework.registerDistributedNode(nodeId, "10.0.0." + std::to_string(i), 8000 + i);
    }
    
    std::cout << "\nInitial Characteristics:\n";
    auto initial_chars = identity->getAllCharacteristics();
    for (const auto& c : initial_chars) {
        std::cout << "  " << c.name << ": " << c.strength << "\n";
    }
    
    // Simulate cognitive cycles with distributed processing
    std::cout << "\nSimulating distributed cognitive processing...\n";
    
    for (int cycle = 0; cycle < 10; ++cycle) {
        // Distribute shards
        std::string shardId = "shard_cycle_" + std::to_string(cycle);
        distMgr->distributeShard(cogself::IdentityShard(
            shardId,
            "Cognitive processing for cycle " + std::to_string(cycle),
            true,
            0.6 + (cycle * 0.03)  // Increasing identity dependency
        ));
        
        // Update synergy state
        framework.updateSynergyState();
        
        // Synchronize
        if (cycle % 3 == 0) {
            distMgr->synchronizeIdentityAcrossNetwork();
        }
        
        // Update from distributed experience
        distMgr->updateIdentityFromDistributedExperience();
        
        // Update some goals
        identity->updateGoalProgress("achieve_cognitive_synergy", 
                                    (cycle + 1) * 0.1);
        
        std::cout << "  Cycle " << cycle + 1 << " complete - "
                  << "Network coherence: " << framework.getDistributedCoherence() << "\n";
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    std::cout << "\nFinal Characteristics:\n";
    auto final_chars = identity->getAllCharacteristics();
    for (const auto& c : final_chars) {
        std::cout << "  " << c.name << ": " << c.strength << "\n";
    }
    
    // Show significant memories
    auto memories = identity->getSignificantMemories(0.6);
    std::cout << "\nSignificant Memories (" << memories.size() << " total):\n";
    for (size_t i = 0; i < std::min<size_t>(5, memories.size()); ++i) {
        std::cout << "  - " << memories[i].description 
                  << " (significance: " << memories[i].significance << ")\n";
    }
    
    // Final identity snapshot
    auto snapshot = identity->createSnapshot();
    std::cout << "\nFinal Identity Snapshot:\n";
    std::cout << "  Agent: " << snapshot.agentName << "\n";
    std::cout << "  Characteristics: " << snapshot.characteristicCount << "\n";
    std::cout << "  Memories: " << snapshot.memoryCount << "\n";
    std::cout << "  Active Goals: " << snapshot.activeGoalCount << "\n";
    std::cout << "  Self Coherence: " << snapshot.selfCoherence << "\n";
    std::cout << "  Identity Strength: " << snapshot.identityStrength << "\n";
}

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  CogPrime Identity + Distributed Cognition Integration Demo       ║\n";
    std::cout << "║  OpenCog Collection - Cognitive Architecture                       ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════╝\n";
    
    try {
        demo_basic_identity();
        demo_integrated_cogself();
        demo_distributed_cognition();
        demo_identity_evolution();
        
        printSeparator("All Demos Completed Successfully");
        std::cout << "\nKey Integration Features Demonstrated:\n";
        std::cout << "  ✓ CogPrime identity with characteristics, memories, and goals\n";
        std::cout << "  ✓ Integration with CogSelf cognitive synergy framework\n";
        std::cout << "  ✓ Distributed cognition across network nodes\n";
        std::cout << "  ✓ Identity-aware shard distribution\n";
        std::cout << "  ✓ Identity coherence monitoring across network\n";
        std::cout << "  ✓ Identity evolution through distributed experience\n";
        std::cout << "  ✓ Network synchronization and integrity validation\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n";
    return 0;
}
