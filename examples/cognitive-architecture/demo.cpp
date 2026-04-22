/**
 * Cognitive Architecture Integration Example
 * Demonstrates the autonomous cognitive architecture with all components
 */

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

// Include all cognitive architecture components
#include "coggml/microkernel.h"
#include "coggml/cognitive_shard.h"
#include "coggml/shard_coordinator.h"
#include "coggml/self_awareness.h"
#include "cogself/cogself.h"
#include "atomspace_accelerator/inference_engine.h"
#include "atomspace_accelerator/accelerator.h"
#include "agentic_chatbots/agent.h"
#include "agentic_chatbots/chatbot_manager.h"
#include "agentic_chatbots/knowledge_integration.h"

void printSeparator() {
    std::cout << "\n" << std::string(70, '=') << "\n\n";
}

int main() {
    std::cout << "OpenCog Autonomous Cognitive Architecture Demo\n";
    std::cout << "==============================================\n\n";
    
    // 1. Initialize CogGML Microkernel
    printSeparator();
    std::cout << "1. Initializing CogGML Self-Aware Microkernel\n";
    printSeparator();
    
    coggml::Microkernel kernel;
    kernel.initialize();
    
    // Create cognitive shards
    auto reasoningShard = std::make_shared<coggml::CognitiveShard>("reasoning", "Logical inference");
    auto learningShard = std::make_shared<coggml::CognitiveShard>("learning", "Pattern recognition");
    auto perceptionShard = std::make_shared<coggml::CognitiveShard>("perception", "Sensory processing");
    
    // Coordinate shards
    coggml::ShardCoordinator coordinator;
    coordinator.registerShard(reasoningShard);
    coordinator.registerShard(learningShard);
    coordinator.registerShard(perceptionShard);
    
    std::cout << "\nActive shards: " << coordinator.getActiveShardCount() << "\n";
    coordinator.coordinate();
    coordinator.optimizeSynergy();
    
    // 2. Initialize AtomSpace Accelerator
    printSeparator();
    std::cout << "2. Initializing AtomSpace Accelerator Inference Engine\n";
    printSeparator();
    
    atomspace_accelerator::InferenceEngine inferenceEngine;
    inferenceEngine.initialize();
    
    atomspace_accelerator::Accelerator accelerator;
    accelerator.enable();
    
    std::cout << "\n" << accelerator.getPerformanceMetrics() << "\n";
    
    // Execute some queries
    std::string result1 = inferenceEngine.executeQuery("Find(ConceptNode 'AGI')");
    std::cout << result1 << "\n";
    
    // 3. Initialize Agentic Chatbots
    printSeparator();
    std::cout << "3. Initializing Agentic Chatbots\n";
    printSeparator();
    
    auto researchAgent = std::make_shared<agentic_chatbots::Agent>("ResearchBot", "AGI Research Assistant");
    auto tutorAgent = std::make_shared<agentic_chatbots::Agent>("TutorBot", "Cognitive Science Tutor");
    
    agentic_chatbots::ChatbotManager chatbotMgr;
    chatbotMgr.registerAgent(researchAgent);
    chatbotMgr.registerAgent(tutorAgent);
    
    // Process some messages
    std::string response1 = chatbotMgr.routeMessage("Explain cognitive synergy");
    std::cout << "\nResponse: " << response1 << "\n";
    
    // Knowledge integration
    agentic_chatbots::KnowledgeIntegration knowledge;
    knowledge.storeKnowledge("Cognitive synergy enables emergent intelligence");
    knowledge.synchronizeKnowledge();
    
    std::cout << "\n" << chatbotMgr.getStatistics() << "\n";
    
    // 4. Initialize CogSelf AGI Framework
    printSeparator();
    std::cout << "4. Initializing CogSelf AGI Cognitive Synergy Framework\n";
    printSeparator();
    
    cogself::CogSelf framework;
    framework.initialize();
    
    // Update synergy state
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    framework.updateSynergyState();
    
    // Assess AGI progress
    double agiProgress = framework.assessAGIProgress();
    std::cout << "\nAGI Progress: " << (agiProgress * 100.0) << "%\n";
    std::cout << "Synergy Level: " << framework.getSynergyLevel() << "\n";
    
    // 5. Generate Self-Improvement Plan
    printSeparator();
    std::cout << "5. Generating Autonomous Self-Improvement Plan\n";
    printSeparator();
    
    std::string improvementPlan = framework.generateImprovementPlan();
    std::cout << "\n" << improvementPlan << "\n";
    
    // 6. Demonstrate Cognitive Synergy
    printSeparator();
    std::cout << "6. Demonstrating Cognitive Synergy\n";
    printSeparator();
    
    std::cout << "\nCognitive Architecture Components:\n";
    std::cout << "  ✓ CogGML Microkernel: " << coordinator.getActiveShardCount() << " active shards\n";
    std::cout << "  ✓ AtomSpace Accelerator: " << accelerator.getAccelerationFactor() << "x speedup\n";
    std::cout << "  ✓ Agentic Chatbots: " << chatbotMgr.getAllAgents().size() << " agents\n";
    std::cout << "  ✓ CogSelf Framework: Synergy level " << framework.getSynergyLevel() << "\n";
    
    std::cout << "\nCognitive Synergy: Components working together for emergent intelligence\n";
    
    // 7. Self-Awareness Demonstration
    printSeparator();
    std::cout << "7. Self-Awareness Module Demonstration\n";
    printSeparator();
    
    coggml::SelfAwareness awareness;
    awareness.updateState("mode", "autonomous");
    awareness.updateState("confidence", "0.75");
    awareness.updateState("performance", "0.82");
    
    std::cout << "\n" << awareness.reflect() << "\n";
    std::cout << "Performance Assessment: " << awareness.assessPerformance() << "\n";
    std::cout << "\n" << awareness.suggestImprovements() << "\n";
    
    // 8. Cleanup
    printSeparator();
    std::cout << "8. Shutting Down Cognitive Architecture\n";
    printSeparator();
    
    framework.shutdown();
    inferenceEngine.shutdown();
    kernel.shutdown();
    
    std::cout << "\nCognitive Architecture demonstration complete!\n";
    std::cout << "\nThis demonstration shows:\n";
    std::cout << "  • Self-aware cognitive shards (CogGML)\n";
    std::cout << "  • Accelerated inference (AtomSpace Accelerator)\n";
    std::cout << "  • Agentic chatbots with knowledge integration\n";
    std::cout << "  • AGI-oriented cognitive synergy (CogSelf)\n";
    std::cout << "  • Autonomous self-improvement capabilities\n";
    
    printSeparator();
    
    return 0;
}
