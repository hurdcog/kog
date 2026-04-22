/**
 * CogPrime Identity Implementation
 */

#include "cogself/cogprime_identity.h"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <iostream>

namespace cogself {

class CogPrimeIdentity::Impl {
public:
    std::string agentId;
    std::string agentName;
    std::string distributedNodeId;
    
    std::vector<CognitiveCharacteristic> characteristics;
    std::vector<EpisodicMemory> memories;
    std::vector<CognitiveGoal> goals;
    
    std::map<std::string, std::vector<std::string>> distributedInteractions;
    
    Impl(const std::string& id, const std::string& name)
        : agentId(id), agentName(name) {}
    
    CognitiveCharacteristic* findCharacteristic(const std::string& name) {
        auto it = std::find_if(characteristics.begin(), characteristics.end(),
            [&name](const CognitiveCharacteristic& c) { return c.name == name; });
        return (it != characteristics.end()) ? &(*it) : nullptr;
    }
    
    const CognitiveCharacteristic* findCharacteristic(const std::string& name) const {
        auto it = std::find_if(characteristics.begin(), characteristics.end(),
            [&name](const CognitiveCharacteristic& c) { return c.name == name; });
        return (it != characteristics.end()) ? &(*it) : nullptr;
    }
    
    CognitiveGoal* findGoal(const std::string& goalId) {
        auto it = std::find_if(goals.begin(), goals.end(),
            [&goalId](const CognitiveGoal& g) { return g.goalId == goalId; });
        return (it != goals.end()) ? &(*it) : nullptr;
    }
    
    const CognitiveGoal* findGoal(const std::string& goalId) const {
        auto it = std::find_if(goals.begin(), goals.end(),
            [&goalId](const CognitiveGoal& g) { return g.goalId == goalId; });
        return (it != goals.end()) ? &(*it) : nullptr;
    }
};

CogPrimeIdentity::CogPrimeIdentity(const std::string& agentId, const std::string& agentName)
    : pImpl(std::make_unique<Impl>(agentId, agentName)) {
    
    std::cout << "[CogPrimeIdentity] Created identity for agent: " << agentName 
              << " (ID: " << agentId << ")" << std::endl;
    
    // Initialize with foundational cognitive characteristics
    addCharacteristic(CognitiveCharacteristic(
        "general_intelligence", 
        "Capacity for general problem-solving and reasoning",
        0.5, 0.7));
    
    addCharacteristic(CognitiveCharacteristic(
        "cognitive_synergy",
        "Ability to integrate diverse cognitive processes",
        0.4, 0.6));
    
    addCharacteristic(CognitiveCharacteristic(
        "self_awareness",
        "Meta-cognitive self-monitoring capabilities",
        0.3, 0.5));
    
    addCharacteristic(CognitiveCharacteristic(
        "learning_capacity",
        "Ability to acquire and refine knowledge and skills",
        0.5, 0.7));
}

CogPrimeIdentity::~CogPrimeIdentity() = default;

std::string CogPrimeIdentity::getAgentId() const {
    return pImpl->agentId;
}

std::string CogPrimeIdentity::getAgentName() const {
    return pImpl->agentName;
}

void CogPrimeIdentity::setAgentName(const std::string& name) {
    pImpl->agentName = name;
    std::cout << "[CogPrimeIdentity] Agent name updated to: " << name << std::endl;
}

void CogPrimeIdentity::addCharacteristic(const CognitiveCharacteristic& characteristic) {
    auto existing = pImpl->findCharacteristic(characteristic.name);
    if (existing) {
        std::cout << "[CogPrimeIdentity] Updating existing characteristic: " 
                  << characteristic.name << std::endl;
        *existing = characteristic;
    } else {
        pImpl->characteristics.push_back(characteristic);
        std::cout << "[CogPrimeIdentity] Added characteristic: " 
                  << characteristic.name << " (strength: " << characteristic.strength << ")" 
                  << std::endl;
    }
}

void CogPrimeIdentity::updateCharacteristic(const std::string& name, double strength) {
    auto characteristic = pImpl->findCharacteristic(name);
    if (characteristic) {
        double oldStrength = characteristic->strength;
        characteristic->strength = std::max(0.0, std::min(1.0, strength));
        std::cout << "[CogPrimeIdentity] Updated characteristic " << name 
                  << ": " << oldStrength << " -> " << characteristic->strength << std::endl;
    } else {
        std::cout << "[CogPrimeIdentity] Warning: Characteristic not found: " << name << std::endl;
    }
}

CognitiveCharacteristic CogPrimeIdentity::getCharacteristic(const std::string& name) const {
    auto characteristic = pImpl->findCharacteristic(name);
    if (characteristic) {
        return *characteristic;
    }
    throw std::runtime_error("Characteristic not found: " + name);
}

std::vector<CognitiveCharacteristic> CogPrimeIdentity::getAllCharacteristics() const {
    return pImpl->characteristics;
}

void CogPrimeIdentity::recordMemory(const EpisodicMemory& memory) {
    pImpl->memories.push_back(memory);
    std::cout << "[CogPrimeIdentity] Recorded memory: " << memory.eventId 
              << " (significance: " << memory.significance << ")" << std::endl;
}

void CogPrimeIdentity::recordMemory(const std::string& eventId, 
                                     const std::string& description,
                                     double significance) {
    recordMemory(EpisodicMemory(eventId, description, significance));
}

std::vector<EpisodicMemory> CogPrimeIdentity::getRecentMemories(size_t count) const {
    std::vector<EpisodicMemory> recent;
    size_t start = pImpl->memories.size() > count ? pImpl->memories.size() - count : 0;
    recent.insert(recent.end(), 
                  pImpl->memories.begin() + start, 
                  pImpl->memories.end());
    return recent;
}

std::vector<EpisodicMemory> CogPrimeIdentity::getSignificantMemories(double minSignificance) const {
    std::vector<EpisodicMemory> significant;
    std::copy_if(pImpl->memories.begin(), pImpl->memories.end(),
                 std::back_inserter(significant),
                 [minSignificance](const EpisodicMemory& m) {
                     return m.significance >= minSignificance;
                 });
    return significant;
}

void CogPrimeIdentity::addGoal(const CognitiveGoal& goal) {
    auto existing = pImpl->findGoal(goal.goalId);
    if (existing) {
        std::cout << "[CogPrimeIdentity] Updating existing goal: " << goal.goalId << std::endl;
        *existing = goal;
    } else {
        pImpl->goals.push_back(goal);
        std::cout << "[CogPrimeIdentity] Added goal: " << goal.goalId 
                  << " (priority: " << goal.priority << ")" << std::endl;
    }
}

void CogPrimeIdentity::addGoal(const std::string& goalId, 
                                const std::string& description,
                                double priority) {
    addGoal(CognitiveGoal(goalId, description, priority));
}

void CogPrimeIdentity::updateGoalProgress(const std::string& goalId, double progress) {
    auto goal = pImpl->findGoal(goalId);
    if (goal && goal->active) {
        double oldProgress = goal->progress;
        goal->progress = std::max(0.0, std::min(1.0, progress));
        std::cout << "[CogPrimeIdentity] Goal progress updated " << goalId 
                  << ": " << oldProgress << " -> " << goal->progress << std::endl;
        
        if (goal->progress >= 1.0) {
            completeGoal(goalId);
        }
    }
}

void CogPrimeIdentity::completeGoal(const std::string& goalId) {
    auto goal = pImpl->findGoal(goalId);
    if (goal) {
        goal->active = false;
        goal->progress = 1.0;
        std::cout << "[CogPrimeIdentity] Goal completed: " << goalId << std::endl;
        
        // Record as significant memory
        recordMemory("goal_completion_" + goalId, 
                    "Completed goal: " + goal->description, 
                    0.8);
    }
}

std::vector<CognitiveGoal> CogPrimeIdentity::getActiveGoals() const {
    std::vector<CognitiveGoal> activeGoals;
    std::copy_if(pImpl->goals.begin(), pImpl->goals.end(),
                 std::back_inserter(activeGoals),
                 [](const CognitiveGoal& g) { return g.active; });
    return activeGoals;
}

CognitiveGoal CogPrimeIdentity::getGoal(const std::string& goalId) const {
    auto goal = pImpl->findGoal(goalId);
    if (goal) {
        return *goal;
    }
    throw std::runtime_error("Goal not found: " + goalId);
}

double CogPrimeIdentity::getSelfCoherence() const {
    if (pImpl->characteristics.empty()) return 0.0;
    
    // Calculate coherence based on characteristic stability
    double totalStability = std::accumulate(
        pImpl->characteristics.begin(), 
        pImpl->characteristics.end(), 
        0.0,
        [](double sum, const CognitiveCharacteristic& c) {
            return sum + c.stability;
        });
    
    return totalStability / pImpl->characteristics.size();
}

double CogPrimeIdentity::getIdentityStrength() const {
    if (pImpl->characteristics.empty()) return 0.0;
    
    // Identity strength is based on characteristic strengths and memory significance
    double charStrength = std::accumulate(
        pImpl->characteristics.begin(),
        pImpl->characteristics.end(),
        0.0,
        [](double sum, const CognitiveCharacteristic& c) {
            return sum + c.strength;
        }) / pImpl->characteristics.size();
    
    // Memory contribution (recent significant memories strengthen identity)
    auto recent = getRecentMemories(10);
    double memoryStrength = recent.empty() ? 0.0 : 
        std::accumulate(recent.begin(), recent.end(), 0.0,
                       [](double sum, const EpisodicMemory& m) {
                           return sum + m.significance;
                       }) / recent.size();
    
    // Combined strength
    return (charStrength * 0.7 + memoryStrength * 0.3);
}

double CogPrimeIdentity::getCognitiveIntegration() const {
    // Integration based on active goal coordination
    auto activeGoals = getActiveGoals();
    if (activeGoals.empty()) return 0.5;
    
    // Higher integration when goals are aligned and making progress
    double avgProgress = std::accumulate(
        activeGoals.begin(), activeGoals.end(), 0.0,
        [](double sum, const CognitiveGoal& g) {
            return sum + g.progress;
        }) / activeGoals.size();
    
    // Look for cognitive_synergy characteristic
    try {
        auto synergy = getCharacteristic("cognitive_synergy");
        return (avgProgress * 0.5 + synergy.strength * 0.5);
    } catch (...) {
        return avgProgress;
    }
}

void CogPrimeIdentity::setDistributedNodeId(const std::string& nodeId) {
    pImpl->distributedNodeId = nodeId;
    std::cout << "[CogPrimeIdentity] Distributed node ID set: " << nodeId << std::endl;
    
    recordMemory("distributed_node_assignment",
                "Assigned to distributed node: " + nodeId,
                0.6);
}

std::string CogPrimeIdentity::getDistributedNodeId() const {
    return pImpl->distributedNodeId;
}

void CogPrimeIdentity::recordDistributedInteraction(const std::string& nodeId,
                                                     const std::string& interaction) {
    pImpl->distributedInteractions[nodeId].push_back(interaction);
    
    recordMemory("distributed_interaction_" + nodeId,
                "Interaction with node " + nodeId + ": " + interaction,
                0.4);
}

std::string CogPrimeIdentity::serializeIdentity() const {
    std::ostringstream oss;
    oss << "COGPRIME_IDENTITY_V1\n";
    oss << "AgentID:" << pImpl->agentId << "\n";
    oss << "AgentName:" << pImpl->agentName << "\n";
    oss << "NodeID:" << pImpl->distributedNodeId << "\n";
    oss << "Characteristics:" << pImpl->characteristics.size() << "\n";
    oss << "Memories:" << pImpl->memories.size() << "\n";
    oss << "Goals:" << getActiveGoals().size() << "\n";
    oss << "SelfCoherence:" << getSelfCoherence() << "\n";
    oss << "IdentityStrength:" << getIdentityStrength() << "\n";
    return oss.str();
}

bool CogPrimeIdentity::deserializeIdentity(const std::string& data) {
    // Simplified deserialization - in production would parse full format
    std::cout << "[CogPrimeIdentity] Deserializing identity data" << std::endl;
    return data.find("COGPRIME_IDENTITY_V1") == 0;
}

CogPrimeIdentity::IdentitySnapshot CogPrimeIdentity::createSnapshot() const {
    IdentitySnapshot snapshot;
    snapshot.agentId = pImpl->agentId;
    snapshot.agentName = pImpl->agentName;
    snapshot.distributedNodeId = pImpl->distributedNodeId;
    snapshot.characteristicCount = pImpl->characteristics.size();
    snapshot.memoryCount = pImpl->memories.size();
    snapshot.activeGoalCount = getActiveGoals().size();
    snapshot.selfCoherence = getSelfCoherence();
    snapshot.identityStrength = getIdentityStrength();
    snapshot.snapshotTime = std::chrono::system_clock::now();
    return snapshot;
}

std::string CogPrimeIdentity::getIdentitySummary() const {
    std::ostringstream oss;
    oss << "CogPrime Identity Summary:\n";
    oss << "  Agent: " << pImpl->agentName << " (" << pImpl->agentId << ")\n";
    oss << "  Distributed Node: " << pImpl->distributedNodeId << "\n";
    oss << "  Characteristics: " << pImpl->characteristics.size() << "\n";
    oss << "  Memories: " << pImpl->memories.size() << "\n";
    oss << "  Active Goals: " << getActiveGoals().size() << "\n";
    oss << "  Self Coherence: " << getSelfCoherence() << "\n";
    oss << "  Identity Strength: " << getIdentityStrength() << "\n";
    oss << "  Cognitive Integration: " << getCognitiveIntegration() << "\n";
    return oss.str();
}

} // namespace cogself
