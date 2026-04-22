/**
 * Distributed Coordinator
 * Manages distributed cognitive processing across network nodes
 */

#ifndef _COGGML_DISTRIBUTED_COORDINATOR_H
#define _COGGML_DISTRIBUTED_COORDINATOR_H

#include <memory>
#include <string>
#include <vector>
#include "cognitive_shard.h"
#include "shard_coordinator.h"

namespace coggml {

/**
 * Node representation in distributed network
 */
struct CognitiveNode {
    std::string nodeId;
    std::string address;
    int port;
    bool isActive;
    double processingCapacity;
    size_t activeShards;
};

/**
 * DistributedCoordinator - Coordinates cognitive processing across network nodes
 */
class DistributedCoordinator {
public:
    DistributedCoordinator();
    ~DistributedCoordinator();

    // Register a cognitive node in the network
    void registerNode(const std::string& nodeId, const std::string& address, int port);
    
    // Unregister a node
    void unregisterNode(const std::string& nodeId);
    
    // Distribute a shard to a specific node
    bool distributeShard(const std::string& shardId, const std::string& nodeId);
    
    // Load balance shards across nodes
    void balanceLoad();
    
    // Get optimal node for new shard
    std::string getOptimalNode() const;
    
    // Get all active nodes
    std::vector<CognitiveNode> getActiveNodes() const;
    
    // Get network health status
    struct NetworkHealth {
        size_t totalNodes;
        size_t activeNodes;
        double averageLoad;
        double networkUtilization;
    };
    
    NetworkHealth getNetworkHealth() const;
    
    // Synchronize state across distributed nodes
    void synchronizeNetwork();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace coggml

#endif // _COGGML_DISTRIBUTED_COORDINATOR_H
