/**
 * Distributed Cognition Identity Integration Implementation
 */

#include "cogself/distributed_identity.h"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iostream>
#include <map>
#include <mutex>
#include <cmath>

namespace cogself {

class DistributedIdentityManager::Impl {
public:
    std::shared_ptr<CogPrimeIdentity> identity;
    std::vector<IdentityAwareNode> nodes;
    std::map<std::string, IdentityShard> shards;
    std::mutex managerMutex;
    IdentityUpdateCallback identityCallback;
    
    size_t propagationCount{0};
    size_t synchronizationCount{0};
    
    IdentityAwareNode* findNode(const std::string& nodeId) {
        auto it = std::find_if(nodes.begin(), nodes.end(),
            [&nodeId](const IdentityAwareNode& n) {
                return n.nodeId == nodeId;
            });
        return (it != nodes.end()) ? &(*it) : nullptr;
    }
    
    const IdentityAwareNode* findNode(const std::string& nodeId) const {
        auto it = std::find_if(nodes.begin(), nodes.end(),
            [&nodeId](const IdentityAwareNode& n) {
                return n.nodeId == nodeId;
            });
        return (it != nodes.end()) ? &(*it) : nullptr;
    }
    
    double calculateNodeLoad(const IdentityAwareNode& node) const {
        if (node.processingCapacity == 0) return 1.0;
        return static_cast<double>(node.activeShards) / node.processingCapacity;
    }
    
    double calculateNodeSuitability(const IdentityAwareNode& node, 
                                    const IdentityShard& shard) const {
        if (!node.isActive) return 0.0;
        
        double loadScore = 1.0 - calculateNodeLoad(node);
        double identityScore = node.identityCoherence;
        double synergyScore = node.synergyAlignment;
        
        // Weight by shard's identity dependency
        double identityWeight = shard.identityDependency;
        double loadWeight = 1.0 - identityWeight;
        
        return (loadScore * loadWeight) + 
               (identityScore * identityWeight * 0.6) +
               (synergyScore * identityWeight * 0.4);
    }
    
    // Internal helper for propagating identity without locking
    // Assumes caller holds the mutex
    void propagateIdentityToNodeInternal(IdentityAwareNode& node) {
        if (!identity) return;
        
        node.assignedAgentId = identity->getAgentId();
        node.identityCoherence = identity->getSelfCoherence();
        node.synergyAlignment = identity->getCognitiveIntegration();
        
        propagationCount++;
        
        std::cout << "[DistributedIdentityManager] Propagated identity to node " << node.nodeId 
                  << " (coherence: " << node.identityCoherence << ")" << std::endl;
        
        identity->setDistributedNodeId(node.nodeId);
    }
};

DistributedIdentityManager::DistributedIdentityManager()
    : pImpl(std::make_unique<Impl>()) {
    std::cout << "[DistributedIdentityManager] Initialized without identity" << std::endl;
}

DistributedIdentityManager::DistributedIdentityManager(
    std::shared_ptr<CogPrimeIdentity> identity)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->identity = identity;
    std::cout << "[DistributedIdentityManager] Initialized with identity: " 
              << identity->getAgentName() << std::endl;
}

DistributedIdentityManager::~DistributedIdentityManager() = default;

void DistributedIdentityManager::setIdentity(std::shared_ptr<CogPrimeIdentity> identity) {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    pImpl->identity = identity;
    std::cout << "[DistributedIdentityManager] Identity set: " 
              << identity->getAgentName() << std::endl;
}

std::shared_ptr<CogPrimeIdentity> DistributedIdentityManager::getIdentity() const {
    return pImpl->identity;
}

void DistributedIdentityManager::registerNode(const IdentityAwareNode& node) {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    
    auto existing = pImpl->findNode(node.nodeId);
    if (existing) {
        *existing = node;
        std::cout << "[DistributedIdentityManager] Updated node: " << node.nodeId << std::endl;
    } else {
        pImpl->nodes.push_back(node);
        std::cout << "[DistributedIdentityManager] Registered identity-aware node: " 
                  << node.nodeId << " at " << node.address << ":" << node.port << std::endl;
    }
    
    // Propagate identity if available (use internal method to avoid deadlock)
    if (pImpl->identity) {
        auto* nodePtr = pImpl->findNode(node.nodeId);
        if (nodePtr) {
            pImpl->propagateIdentityToNodeInternal(*nodePtr);
        }
    }
}

void DistributedIdentityManager::unregisterNode(const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    
    auto it = std::remove_if(pImpl->nodes.begin(), pImpl->nodes.end(),
        [&nodeId](const IdentityAwareNode& n) {
            return n.nodeId == nodeId;
        });
    
    if (it != pImpl->nodes.end()) {
        pImpl->nodes.erase(it, pImpl->nodes.end());
        std::cout << "[DistributedIdentityManager] Unregistered node: " << nodeId << std::endl;
        
        // Record event in identity
        if (pImpl->identity) {
            pImpl->identity->recordMemory("node_unregistration",
                                         "Node " + nodeId + " unregistered",
                                         0.3);
        }
    }
}

void DistributedIdentityManager::updateNodeIdentityCoherence(const std::string& nodeId, 
                                                             double coherence) {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    auto node = pImpl->findNode(nodeId);
    if (node) {
        node->identityCoherence = std::max(0.0, std::min(1.0, coherence));
        std::cout << "[DistributedIdentityManager] Node " << nodeId 
                  << " identity coherence: " << node->identityCoherence << std::endl;
    }
}

std::vector<IdentityAwareNode> DistributedIdentityManager::getIdentityAwareNodes() const {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    return pImpl->nodes;
}

void DistributedIdentityManager::distributeShard(const IdentityShard& shard) {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    
    std::string optimalNode = findOptimalNodeForShard(shard);
    if (optimalNode.empty()) {
        std::cerr << "[DistributedIdentityManager] No suitable node for shard: " 
                  << shard.shardId << std::endl;
        return;
    }
    
    auto node = pImpl->findNode(optimalNode);
    if (node) {
        node->activeShards++;
        IdentityShard shardCopy = shard;
        shardCopy.assignedNodeId = optimalNode;
        pImpl->shards[shard.shardId] = shardCopy;
        
        std::cout << "[DistributedIdentityManager] Distributed identity-aware shard " 
                  << shard.shardId << " to node " << optimalNode 
                  << " (identity dependency: " << shard.identityDependency << ")" << std::endl;
        
        // Record in identity
        if (pImpl->identity) {
            pImpl->identity->recordDistributedInteraction(optimalNode,
                "Assigned shard: " + shard.shardId + " (" + shard.purpose + ")");
        }
    }
}

void DistributedIdentityManager::redistributeShards() {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    
    std::cout << "[DistributedIdentityManager] Redistributing " << pImpl->shards.size() 
              << " shards across network" << std::endl;
    
    // Simple redistribution: reassess each shard
    for (auto& [shardId, shard] : pImpl->shards) {
        std::string newNode = findOptimalNodeForShard(shard);
        if (!newNode.empty() && newNode != shard.assignedNodeId) {
            // Update node shard counts
            auto oldNode = pImpl->findNode(shard.assignedNodeId);
            if (oldNode) oldNode->activeShards--;
            
            auto newNodePtr = pImpl->findNode(newNode);
            if (newNodePtr) newNodePtr->activeShards++;
            
            shard.assignedNodeId = newNode;
            std::cout << "[DistributedIdentityManager] Redistributed shard " 
                      << shardId << " to node " << newNode << std::endl;
        }
    }
}

std::string DistributedIdentityManager::findOptimalNodeForShard(
    const IdentityShard& shard) const {
    
    if (pImpl->nodes.empty()) return "";
    
    std::string optimalNodeId;
    double maxSuitability = -1.0;
    
    for (const auto& node : pImpl->nodes) {
        if (!node.isActive) continue;
        
        double suitability = pImpl->calculateNodeSuitability(node, shard);
        if (suitability > maxSuitability) {
            maxSuitability = suitability;
            optimalNodeId = node.nodeId;
        }
    }
    
    return optimalNodeId;
}

void DistributedIdentityManager::propagateIdentityToNode(const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    
    if (!pImpl->identity) {
        std::cerr << "[DistributedIdentityManager] Cannot propagate: no identity set" 
                  << std::endl;
        return;
    }
    
    auto node = pImpl->findNode(nodeId);
    if (!node) {
        std::cerr << "[DistributedIdentityManager] Node not found: " << nodeId << std::endl;
        return;
    }
    
    pImpl->propagateIdentityToNodeInternal(*node);
}

void DistributedIdentityManager::propagateIdentityToAllNodes() {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    
    std::cout << "[DistributedIdentityManager] Propagating identity to all nodes" << std::endl;
    
    if (!pImpl->identity) {
        std::cerr << "[DistributedIdentityManager] Cannot propagate: no identity set" 
                  << std::endl;
        return;
    }
    
    for (auto& node : pImpl->nodes) {
        if (node.isActive) {
            pImpl->propagateIdentityToNodeInternal(node);
        }
    }
}

void DistributedIdentityManager::synchronizeIdentityAcrossNetwork() {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    pImpl->synchronizationCount++;
    
    if (!pImpl->identity) return;
    
    std::cout << "[DistributedIdentityManager] Synchronizing identity across network (sync #" 
              << pImpl->synchronizationCount << ")" << std::endl;
    
    // Calculate average coherence across nodes
    double totalCoherence = 0.0;
    size_t activeNodes = 0;
    
    for (const auto& node : pImpl->nodes) {
        if (node.isActive) {
            totalCoherence += node.identityCoherence;
            activeNodes++;
        }
    }
    
    if (activeNodes > 0) {
        double avgCoherence = totalCoherence / activeNodes;
        std::cout << "[DistributedIdentityManager] Network average identity coherence: " 
                  << avgCoherence << std::endl;
        
        // Update central identity based on distributed feedback
        if (avgCoherence > 0.7) {
            pImpl->identity->updateCharacteristic("cognitive_synergy", 
                pImpl->identity->getCharacteristic("cognitive_synergy").strength + 0.01);
        }
    }
}

double DistributedIdentityManager::getNetworkIdentityCoherence() const {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    
    if (pImpl->nodes.empty()) return 0.0;
    
    double totalCoherence = 0.0;
    size_t activeNodes = 0;
    
    for (const auto& node : pImpl->nodes) {
        if (node.isActive) {
            totalCoherence += node.identityCoherence;
            activeNodes++;
        }
    }
    
    return activeNodes > 0 ? totalCoherence / activeNodes : 0.0;
}

double DistributedIdentityManager::getNodeIdentityCoherence(const std::string& nodeId) const {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    auto node = pImpl->findNode(nodeId);
    return node ? node->identityCoherence : 0.0;
}

void DistributedIdentityManager::assessIdentityFragmentation() {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    
    std::cout << "[DistributedIdentityManager] Assessing identity fragmentation..." << std::endl;
    
    if (pImpl->nodes.empty() || !pImpl->identity) {
        std::cout << "[DistributedIdentityManager] No fragmentation (no active nodes)" << std::endl;
        return;
    }
    
    // Calculate variance in identity coherence across nodes
    double avgCoherence = getNetworkIdentityCoherence();
    double variance = 0.0;
    size_t activeNodes = 0;
    
    for (const auto& node : pImpl->nodes) {
        if (node.isActive) {
            double diff = node.identityCoherence - avgCoherence;
            variance += diff * diff;
            activeNodes++;
        }
    }
    
    variance = activeNodes > 0 ? variance / activeNodes : 0.0;
    double fragmentation = std::sqrt(variance);
    
    std::cout << "[DistributedIdentityManager] Identity fragmentation: " << fragmentation 
              << " (variance: " << variance << ")" << std::endl;
    
    if (fragmentation > 0.3) {
        std::cout << "[DistributedIdentityManager] Warning: High identity fragmentation detected"
                  << std::endl;
        // Could trigger synchronization
    }
}

void DistributedIdentityManager::recordDistributedCognitiveEvent(
    const std::string& nodeId,
    const std::string& eventType,
    const std::string& description) {
    
    if (!pImpl->identity) return;
    
    std::string eventId = "distributed_" + eventType + "_" + nodeId;
    pImpl->identity->recordMemory(eventId, description, 0.5);
    
    std::cout << "[DistributedIdentityManager] Recorded distributed event: " 
              << eventType << " on node " << nodeId << std::endl;
}

DistributedIdentityManager::IdentitySynergyMetrics 
DistributedIdentityManager::getIdentitySynergyMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    
    IdentitySynergyMetrics metrics;
    metrics.overallCoherence = getNetworkIdentityCoherence();
    metrics.activeNodes = std::count_if(pImpl->nodes.begin(), pImpl->nodes.end(),
        [](const IdentityAwareNode& n) { return n.isActive; });
    metrics.activeShards = pImpl->shards.size();
    
    // Calculate distributed alignment
    double totalAlignment = 0.0;
    for (const auto& node : pImpl->nodes) {
        if (node.isActive) {
            totalAlignment += node.synergyAlignment;
        }
    }
    metrics.distributedAlignment = metrics.activeNodes > 0 ? 
        totalAlignment / metrics.activeNodes : 0.0;
    
    // Identity stability from central identity
    metrics.identityStability = pImpl->identity ? 
        pImpl->identity->getSelfCoherence() : 0.0;
    
    // Network integration combines coherence and alignment
    metrics.networkIntegration = (metrics.overallCoherence + metrics.distributedAlignment) / 2.0;
    
    return metrics;
}

void DistributedIdentityManager::updateIdentityFromDistributedExperience() {
    if (!pImpl->identity) return;
    
    std::cout << "[DistributedIdentityManager] Updating identity from distributed experience" 
              << std::endl;
    
    auto metrics = getIdentitySynergyMetrics();
    
    // High network integration strengthens cognitive synergy
    if (metrics.networkIntegration > 0.7) {
        strengthenCharacteristicFromNetwork("cognitive_synergy", 0.02);
    }
    
    // Many active shards indicate learning capacity
    if (metrics.activeShards > 5) {
        strengthenCharacteristicFromNetwork("learning_capacity", 0.01);
    }
    
    // High coherence strengthens self-awareness
    if (metrics.overallCoherence > 0.8) {
        strengthenCharacteristicFromNetwork("self_awareness", 0.015);
    }
}

void DistributedIdentityManager::strengthenCharacteristicFromNetwork(
    const std::string& characteristic,
    double delta) {
    
    if (!pImpl->identity) return;
    
    try {
        auto current = pImpl->identity->getCharacteristic(characteristic);
        double newStrength = std::min(1.0, current.strength + delta);
        pImpl->identity->updateCharacteristic(characteristic, newStrength);
        
        std::cout << "[DistributedIdentityManager] Strengthened characteristic '" 
                  << characteristic << "' from network experience by " << delta << std::endl;
        
        // Notify via callback if set
        if (pImpl->identityCallback) {
            pImpl->identityCallback(characteristic, newStrength);
        }
    } catch (...) {
        std::cerr << "[DistributedIdentityManager] Characteristic not found: " 
                  << characteristic << std::endl;
    }
}

void DistributedIdentityManager::setIdentityUpdateCallback(IdentityUpdateCallback callback) {
    pImpl->identityCallback = callback;
}

std::string DistributedIdentityManager::getDistributedIdentitySummary() const {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    
    std::ostringstream oss;
    oss << "Distributed Identity Summary:\n";
    
    if (pImpl->identity) {
        oss << "  Agent: " << pImpl->identity->getAgentName() << "\n";
        oss << "  Central Identity Strength: " << pImpl->identity->getIdentityStrength() << "\n";
    } else {
        oss << "  No identity set\n";
    }
    
    oss << "  Active Nodes: " << std::count_if(pImpl->nodes.begin(), pImpl->nodes.end(),
        [](const IdentityAwareNode& n) { return n.isActive; }) << "\n";
    oss << "  Total Shards: " << pImpl->shards.size() << "\n";
    oss << "  Network Coherence: " << getNetworkIdentityCoherence() << "\n";
    oss << "  Propagations: " << pImpl->propagationCount << "\n";
    oss << "  Synchronizations: " << pImpl->synchronizationCount << "\n";
    
    auto metrics = getIdentitySynergyMetrics();
    oss << "  Network Integration: " << metrics.networkIntegration << "\n";
    oss << "  Distributed Alignment: " << metrics.distributedAlignment << "\n";
    
    return oss.str();
}

bool DistributedIdentityManager::validateIdentityIntegrity() const {
    std::lock_guard<std::mutex> lock(pImpl->managerMutex);
    
    if (!pImpl->identity) {
        std::cout << "[DistributedIdentityManager] No identity to validate" << std::endl;
        return false;
    }
    
    // Check identity strength is reasonable
    double strength = pImpl->identity->getIdentityStrength();
    if (strength < 0.0 || strength > 1.0) {
        std::cerr << "[DistributedIdentityManager] Invalid identity strength: " 
                  << strength << std::endl;
        return false;
    }
    
    // Check network coherence
    double networkCoherence = getNetworkIdentityCoherence();
    if (networkCoherence < 0.0 || networkCoherence > 1.0) {
        std::cerr << "[DistributedIdentityManager] Invalid network coherence: " 
                  << networkCoherence << std::endl;
        return false;
    }
    
    // Verify node consistency
    for (const auto& node : pImpl->nodes) {
        if (node.isActive && node.assignedAgentId != pImpl->identity->getAgentId()) {
            std::cerr << "[DistributedIdentityManager] Identity mismatch on node: " 
                      << node.nodeId << std::endl;
            return false;
        }
    }
    
    std::cout << "[DistributedIdentityManager] Identity integrity validated" << std::endl;
    return true;
}

} // namespace cogself
