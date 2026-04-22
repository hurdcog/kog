/**
 * CogSelf Implementation
 * Enhanced with CogPrime identity and distributed cognition
 */

#include "cogself/cogself.h"
#include "cogself/synergy_manager.h"
#include "cogself/agi_goal_tracker.h"
#include "cogself/cogprime_identity.h"
#include "cogself/distributed_identity.h"
#include <atomic>
#include <iostream>
#include <sstream>

namespace cogself {

class CogSelf::Impl {
public:
    std::atomic<bool> running{false};
    std::unique_ptr<SynergyManager> synergyMgr;
    std::unique_ptr<AGIGoalTracker> goalTracker;
    std::shared_ptr<CogPrimeIdentity> identity;
    std::shared_ptr<DistributedIdentityManager> distributedMgr;
    double currentSynergyLevel{0.0};
    bool distributedEnabled{false};

    Impl() {
        synergyMgr = std::make_unique<SynergyManager>();
        goalTracker = std::make_unique<AGIGoalTracker>();
    }
};

CogSelf::CogSelf() : pImpl(std::make_unique<Impl>()) {
    std::cout << "[CogSelf] Framework created" << std::endl;
}

CogSelf::CogSelf(const std::string& agentName) : pImpl(std::make_unique<Impl>()) {
    std::cout << "[CogSelf] Framework created for agent: " << agentName << std::endl;
    // Identity will be initialized in initialize()
}

CogSelf::~CogSelf() {
    shutdown();
}

bool CogSelf::initialize() {
    return initialize("cogself_agent_" + std::to_string(std::time(nullptr)), 
                     "CogSelf Agent");
}

bool CogSelf::initialize(const std::string& agentId, const std::string& agentName) {
    if (pImpl->running) {
        return true;
    }

    std::cout << "[CogSelf] Initializing AGI cognitive synergy framework..." << std::endl;
    
    // Initialize CogPrime identity
    pImpl->identity = std::make_shared<CogPrimeIdentity>(agentId, agentName);
    std::cout << "[CogSelf] CogPrime identity initialized" << std::endl;
    
    // Add initial cognitive goals to identity
    pImpl->identity->addGoal("achieve_cognitive_synergy", 
                            "Develop strong cognitive synergy across components", 
                            0.9);
    pImpl->identity->addGoal("develop_self_awareness",
                            "Enhance meta-cognitive capabilities",
                            0.8);
    pImpl->identity->addGoal("autonomous_learning",
                            "Master self-directed learning and adaptation",
                            0.85);
    
    // Initialize AGI goals (legacy system)
    pImpl->goalTracker->addGoal("cognitive_synergy", 1.0);
    pImpl->goalTracker->addGoal("self_awareness", 1.0);
    pImpl->goalTracker->addGoal("autonomous_learning", 1.0);
    pImpl->goalTracker->addGoal("general_intelligence", 1.0);
    
    // Initialize synergy components
    pImpl->synergyMgr->addComponent("coggml_microkernel");
    pImpl->synergyMgr->addComponent("atomspace_accelerator");
    pImpl->synergyMgr->addComponent("agentic_chatbots");
    pImpl->synergyMgr->addComponent("cogprime_identity");
    
    pImpl->running = true;
    pImpl->currentSynergyLevel = 0.1;
    
    // Record initialization in identity
    pImpl->identity->recordMemory("cogself_initialization",
                                 "CogSelf framework initialized with CogPrime identity",
                                 0.9);
    
    std::cout << "[CogSelf] Framework initialized successfully with identity: " 
              << agentName << std::endl;
    return true;
}

void CogSelf::shutdown() {
    if (!pImpl->running) {
        return;
    }

    std::cout << "[CogSelf] Shutting down framework..." << std::endl;
    pImpl->running = false;
    std::cout << "[CogSelf] Framework shutdown complete" << std::endl;
}

void CogSelf::updateSynergyState() {
    if (!pImpl->running) {
        return;
    }

    pImpl->currentSynergyLevel = pImpl->synergyMgr->calculateSynergyLevel();
    pImpl->synergyMgr->optimizeSynergyPathways();
    
    // Update AGI goal progress based on synergy
    pImpl->goalTracker->updateProgress("cognitive_synergy", pImpl->currentSynergyLevel);
    
    // Update identity characteristics based on synergy
    if (pImpl->identity) {
        pImpl->identity->updateCharacteristic("cognitive_synergy", pImpl->currentSynergyLevel);
        
        // Update identity goals
        pImpl->identity->updateGoalProgress("achieve_cognitive_synergy", 
                                           pImpl->currentSynergyLevel);
        
        // Record synergy state in episodic memory
        std::ostringstream oss;
        oss << "Synergy state update: level=" << pImpl->currentSynergyLevel;
        pImpl->identity->recordMemory("synergy_update", oss.str(), 0.3);
    }
    
    // If distributed cognition is enabled, update from network
    if (pImpl->distributedEnabled && pImpl->distributedMgr) {
        pImpl->distributedMgr->updateIdentityFromDistributedExperience();
    }
    
    std::cout << "[CogSelf] Synergy state updated: " << pImpl->currentSynergyLevel << std::endl;
}

double CogSelf::assessAGIProgress() const {
    if (!pImpl->running) {
        return 0.0;
    }

    return pImpl->goalTracker->calculateOverallProgress();
}

std::string CogSelf::generateImprovementPlan() const {
    if (!pImpl->running) {
        return "Framework not running";
    }

    std::string plan = "AGI Self-Improvement Plan:\n\n";
    plan += "=== Goal Progress ===\n";
    plan += pImpl->goalTracker->generateProgressReport();
    plan += "\n";
    plan += pImpl->goalTracker->identifyPriorityGoals();
    plan += "\n\n=== Synergy Metrics ===\n";
    plan += pImpl->synergyMgr->getSynergyMetrics();
    
    // Add CogPrime identity insights
    if (pImpl->identity) {
        plan += "\n\n=== CogPrime Identity Status ===\n";
        plan += pImpl->identity->getIdentitySummary();
        
        // Add recent significant memories
        auto memories = pImpl->identity->getSignificantMemories(0.6);
        if (!memories.empty()) {
            plan += "\n=== Recent Significant Experiences ===\n";
            size_t count = std::min<size_t>(5, memories.size());
            for (size_t i = memories.size() - count; i < memories.size(); ++i) {
                plan += "  - " + memories[i].description + 
                       " (significance: " + std::to_string(memories[i].significance) + ")\n";
            }
        }
    }
    
    // Add distributed cognition status if enabled
    if (pImpl->distributedEnabled && pImpl->distributedMgr) {
        plan += "\n\n=== Distributed Cognition Status ===\n";
        plan += pImpl->distributedMgr->getDistributedIdentitySummary();
    }
    
    return plan;
}

double CogSelf::getSynergyLevel() const {
    return pImpl->currentSynergyLevel;
}

bool CogSelf::isRunning() const {
    return pImpl->running;
}

std::shared_ptr<CogPrimeIdentity> CogSelf::getIdentity() const {
    return pImpl->identity;
}

void CogSelf::recordCognitiveEvent(const std::string& eventId,
                                   const std::string& description,
                                   double significance) {
    if (pImpl->identity) {
        pImpl->identity->recordMemory(eventId, description, significance);
    }
}

std::string CogSelf::getIdentitySummary() const {
    if (pImpl->identity) {
        return pImpl->identity->getIdentitySummary();
    }
    return "No identity initialized";
}

std::shared_ptr<DistributedIdentityManager> CogSelf::getDistributedManager() const {
    return pImpl->distributedMgr;
}

void CogSelf::enableDistributedCognition() {
    if (!pImpl->running || !pImpl->identity) {
        std::cerr << "[CogSelf] Cannot enable distributed cognition: "
                  << "framework not running or identity not initialized" << std::endl;
        return;
    }
    
    if (!pImpl->distributedMgr) {
        pImpl->distributedMgr = std::make_shared<DistributedIdentityManager>(pImpl->identity);
        pImpl->distributedEnabled = true;
        
        std::cout << "[CogSelf] Distributed cognition enabled with identity integration" 
                  << std::endl;
        
        // Record this milestone
        pImpl->identity->recordMemory("distributed_cognition_enabled",
                                     "Enabled distributed cognitive processing with identity awareness",
                                     0.8);
    } else {
        std::cout << "[CogSelf] Distributed cognition already enabled" << std::endl;
    }
}

void CogSelf::registerDistributedNode(const std::string& nodeId,
                                     const std::string& address,
                                     int port) {
    if (!pImpl->distributedEnabled || !pImpl->distributedMgr) {
        std::cerr << "[CogSelf] Distributed cognition not enabled" << std::endl;
        return;
    }
    
    IdentityAwareNode node;
    node.nodeId = nodeId;
    node.address = address;
    node.port = port;
    node.isActive = true;
    node.processingCapacity = 10.0;
    node.activeShards = 0;
    
    pImpl->distributedMgr->registerNode(node);
    
    std::cout << "[CogSelf] Registered distributed node: " << nodeId << std::endl;
}

void CogSelf::synchronizeDistributedIdentity() {
    if (pImpl->distributedEnabled && pImpl->distributedMgr) {
        pImpl->distributedMgr->synchronizeIdentityAcrossNetwork();
    }
}

double CogSelf::getDistributedCoherence() const {
    if (pImpl->distributedEnabled && pImpl->distributedMgr) {
        return pImpl->distributedMgr->getNetworkIdentityCoherence();
    }
    return 0.0;
}

} // namespace cogself
