/**
 * Distributed Coordinator Implementation
 */

#include "coggml/distributed_coordinator.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <numeric>

namespace coggml {

class DistributedCoordinator::Impl {
public:
    std::vector<CognitiveNode> nodes;
    std::unordered_map<std::string, std::string> shardToNode; // shard ID -> node ID mapping
    std::mutex coordMutex;
    
    size_t syncCount{0};
    size_t loadBalanceCount{0};
    
    CognitiveNode* findNode(const std::string& nodeId) {
        auto it = std::find_if(nodes.begin(), nodes.end(),
            [&nodeId](const CognitiveNode& node) {
                return node.nodeId == nodeId;
            });
        return (it != nodes.end()) ? &(*it) : nullptr;
    }
    
    const CognitiveNode* findNode(const std::string& nodeId) const {
        auto it = std::find_if(nodes.begin(), nodes.end(),
            [&nodeId](const CognitiveNode& node) {
                return node.nodeId == nodeId;
            });
        return (it != nodes.end()) ? &(*it) : nullptr;
    }
    
    double calculateNodeLoad(const CognitiveNode& node) const {
        if (node.processingCapacity == 0) return 1.0;
        return static_cast<double>(node.activeShards) / node.processingCapacity;
    }
};

DistributedCoordinator::DistributedCoordinator() : pImpl(std::make_unique<Impl>()) {
    std::cout << "[DistributedCoordinator] Initialized for distributed cognitive processing" << std::endl;
}

DistributedCoordinator::~DistributedCoordinator() = default;

void DistributedCoordinator::registerNode(const std::string& nodeId, const std::string& address, int port) {
    std::lock_guard<std::mutex> lock(pImpl->coordMutex);
    
    CognitiveNode node;
    node.nodeId = nodeId;
    node.address = address;
    node.port = port;
    node.isActive = true;
    node.processingCapacity = 10.0; // Default capacity for 10 shards
    node.activeShards = 0;
    
    pImpl->nodes.push_back(node);
    
    std::cout << "[DistributedCoordinator] Registered node: " << nodeId 
              << " at " << address << ":" << port << std::endl;
}

void DistributedCoordinator::unregisterNode(const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(pImpl->coordMutex);
    
    auto it = std::remove_if(pImpl->nodes.begin(), pImpl->nodes.end(),
        [&nodeId](const CognitiveNode& node) {
            return node.nodeId == nodeId;
        });
    
    if (it != pImpl->nodes.end()) {
        // Redistribute shards from this node
        std::vector<std::string> shardsToRedistribute;
        for (const auto& [shardId, assignedNodeId] : pImpl->shardToNode) {
            if (assignedNodeId == nodeId) {
                shardsToRedistribute.push_back(shardId);
            }
        }
        
        // Remove node
        pImpl->nodes.erase(it, pImpl->nodes.end());
        
        std::cout << "[DistributedCoordinator] Unregistered node: " << nodeId 
                  << " (" << shardsToRedistribute.size() << " shards to redistribute)" << std::endl;
    }
}

bool DistributedCoordinator::distributeShard(const std::string& shardId, const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(pImpl->coordMutex);
    
    auto node = pImpl->findNode(nodeId);
    if (!node || !node->isActive) {
        std::cerr << "[DistributedCoordinator] Cannot distribute to inactive node: " << nodeId << std::endl;
        return false;
    }
    
    // Check if node has capacity
    double currentLoad = pImpl->calculateNodeLoad(*node);
    if (currentLoad >= 1.0) {
        std::cerr << "[DistributedCoordinator] Node at capacity: " << nodeId << std::endl;
        return false;
    }
    
    pImpl->shardToNode[shardId] = nodeId;
    node->activeShards++;
    
    std::cout << "[DistributedCoordinator] Distributed shard " << shardId 
              << " to node " << nodeId << std::endl;
    return true;
}

void DistributedCoordinator::balanceLoad() {
    std::lock_guard<std::mutex> lock(pImpl->coordMutex);
    pImpl->loadBalanceCount++;
    
    std::cout << "[DistributedCoordinator] Balancing load across " 
              << pImpl->nodes.size() << " nodes" << std::endl;
    
    // Calculate average load
    double totalLoad = 0.0;
    size_t activeNodes = 0;
    
    for (const auto& node : pImpl->nodes) {
        if (node.isActive) {
            totalLoad += pImpl->calculateNodeLoad(node);
            activeNodes++;
        }
    }
    
    if (activeNodes == 0) return;
    
    double avgLoad = totalLoad / activeNodes;
    
    // Find overloaded and underloaded nodes
    std::vector<CognitiveNode*> overloaded;
    std::vector<CognitiveNode*> underloaded;
    
    for (auto& node : pImpl->nodes) {
        if (!node.isActive) continue;
        
        double nodeLoad = pImpl->calculateNodeLoad(node);
        if (nodeLoad > avgLoad * 1.2) {
            overloaded.push_back(&node);
        } else if (nodeLoad < avgLoad * 0.8) {
            underloaded.push_back(&node);
        }
    }
    
    std::cout << "[DistributedCoordinator] Found " << overloaded.size() 
              << " overloaded and " << underloaded.size() << " underloaded nodes" << std::endl;
    
    // Simulate load balancing (in real implementation, would migrate shards)
    for (auto* node : overloaded) {
        std::cout << "[DistributedCoordinator] Rebalancing from node: " << node->nodeId << std::endl;
    }
}

std::string DistributedCoordinator::getOptimalNode() const {
    std::lock_guard<std::mutex> lock(pImpl->coordMutex);
    
    CognitiveNode const* optimalNode = nullptr;
    double minLoad = 2.0;
    
    for (const auto& node : pImpl->nodes) {
        if (!node.isActive) continue;
        
        double load = pImpl->calculateNodeLoad(node);
        if (load < minLoad) {
            minLoad = load;
            optimalNode = &node;
        }
    }
    
    return optimalNode ? optimalNode->nodeId : "";
}

std::vector<CognitiveNode> DistributedCoordinator::getActiveNodes() const {
    std::lock_guard<std::mutex> lock(pImpl->coordMutex);
    
    std::vector<CognitiveNode> activeNodes;
    std::copy_if(pImpl->nodes.begin(), pImpl->nodes.end(), 
                 std::back_inserter(activeNodes),
                 [](const CognitiveNode& node) { return node.isActive; });
    
    return activeNodes;
}

DistributedCoordinator::NetworkHealth DistributedCoordinator::getNetworkHealth() const {
    std::lock_guard<std::mutex> lock(pImpl->coordMutex);
    
    NetworkHealth health;
    health.totalNodes = pImpl->nodes.size();
    health.activeNodes = std::count_if(pImpl->nodes.begin(), pImpl->nodes.end(),
        [](const CognitiveNode& node) { return node.isActive; });
    
    if (health.activeNodes > 0) {
        double totalLoad = 0.0;
        size_t totalShards = 0;
        double totalCapacity = 0.0;
        
        for (const auto& node : pImpl->nodes) {
            if (node.isActive) {
                totalLoad += pImpl->calculateNodeLoad(node);
                totalShards += node.activeShards;
                totalCapacity += node.processingCapacity;
            }
        }
        
        health.averageLoad = totalLoad / health.activeNodes;
        health.networkUtilization = totalCapacity > 0 ? (totalShards / totalCapacity) : 0.0;
    } else {
        health.averageLoad = 0.0;
        health.networkUtilization = 0.0;
    }
    
    return health;
}

void DistributedCoordinator::synchronizeNetwork() {
    std::lock_guard<std::mutex> lock(pImpl->coordMutex);
    pImpl->syncCount++;
    
    std::cout << "[DistributedCoordinator] Synchronizing network state across " 
              << pImpl->nodes.size() << " nodes (sync #" << pImpl->syncCount << ")" << std::endl;
    
    // In a real implementation, this would:
    // - Broadcast state updates to all nodes
    // - Reconcile any conflicts
    // - Ensure consensus on shard distribution
    
    auto health = getNetworkHealth();
    std::cout << "[DistributedCoordinator] Network health: "
              << health.activeNodes << "/" << health.totalNodes << " nodes active, "
              << "avg load: " << health.averageLoad 
              << ", utilization: " << (health.networkUtilization * 100.0) << "%" << std::endl;
}

} // namespace coggml
