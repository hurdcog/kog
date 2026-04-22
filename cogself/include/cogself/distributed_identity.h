/**
 * Distributed Cognition Identity Integration
 * Connects CogPrime identity with distributed cognitive processing
 * 
 * Enables identity-aware distributed cognition where:
 * - Each node maintains awareness of agent identity
 * - Identity characteristics influence shard distribution
 * - Distributed interactions enrich identity development
 * - Identity coherence is maintained across network
 */

#ifndef _COGSELF_DISTRIBUTED_IDENTITY_H
#define _COGSELF_DISTRIBUTED_IDENTITY_H

#include "cogself/cogprime_identity.h"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace cogself {

/**
 * Identity-aware cognitive node
 * Extends basic distributed node with identity awareness
 */
struct IdentityAwareNode {
    std::string nodeId;
    std::string address;
    int port;
    bool isActive;
    double processingCapacity;
    size_t activeShards;
    
    // Identity-specific fields
    std::string assignedAgentId;
    double identityCoherence;
    double synergyAlignment;
    
    IdentityAwareNode() 
        : port(0), isActive(false), processingCapacity(0.0), 
          activeShards(0), identityCoherence(0.0), synergyAlignment(0.0) {}
};

/**
 * Identity-aware shard information
 */
struct IdentityShard {
    std::string shardId;
    std::string purpose;
    std::string assignedNodeId;
    bool requiresIdentity;
    double identityDependency;  // How much this shard depends on identity (0.0-1.0)
    
    IdentityShard() 
        : requiresIdentity(false), identityDependency(0.0) {}
    
    IdentityShard(const std::string& id, const std::string& purp, bool reqId = true, double dep = 0.5)
        : shardId(id), purpose(purp), requiresIdentity(reqId), identityDependency(dep) {}
};

/**
 * DistributedIdentityManager - Manages identity across distributed cognitive network
 * 
 * Core integration class that:
 * - Distributes agent identity to network nodes
 * - Maintains identity coherence across shards
 * - Coordinates identity-aware cognitive processing
 * - Tracks identity evolution through distributed interactions
 */
class DistributedIdentityManager {
public:
    DistributedIdentityManager();
    explicit DistributedIdentityManager(std::shared_ptr<CogPrimeIdentity> identity);
    ~DistributedIdentityManager();
    
    // Identity management
    void setIdentity(std::shared_ptr<CogPrimeIdentity> identity);
    std::shared_ptr<CogPrimeIdentity> getIdentity() const;
    
    // Distributed node management
    void registerNode(const IdentityAwareNode& node);
    void unregisterNode(const std::string& nodeId);
    void updateNodeIdentityCoherence(const std::string& nodeId, double coherence);
    std::vector<IdentityAwareNode> getIdentityAwareNodes() const;
    
    // Identity-aware shard distribution
    void distributeShard(const IdentityShard& shard);
    void redistributeShards();
    std::string findOptimalNodeForShard(const IdentityShard& shard) const;
    
    // Identity propagation
    void propagateIdentityToNode(const std::string& nodeId);
    void propagateIdentityToAllNodes();
    void synchronizeIdentityAcrossNetwork();
    
    // Identity coherence monitoring
    double getNetworkIdentityCoherence() const;
    double getNodeIdentityCoherence(const std::string& nodeId) const;
    void assessIdentityFragmentation();
    
    // Distributed identity events
    void recordDistributedCognitiveEvent(const std::string& nodeId, 
                                        const std::string& eventType,
                                        const std::string& description);
    
    // Identity synergy metrics
    struct IdentitySynergyMetrics {
        double overallCoherence;
        double distributedAlignment;
        double identityStability;
        double networkIntegration;
        size_t activeNodes;
        size_t activeShards;
    };
    
    IdentitySynergyMetrics getIdentitySynergyMetrics() const;
    
    // Identity evolution through distributed processing
    void updateIdentityFromDistributedExperience();
    void strengthenCharacteristicFromNetwork(const std::string& characteristic, 
                                            double delta);
    
    // Callbacks for distributed events
    using IdentityUpdateCallback = std::function<void(const std::string&, double)>;
    void setIdentityUpdateCallback(IdentityUpdateCallback callback);
    
    // Summary and diagnostics
    std::string getDistributedIdentitySummary() const;
    bool validateIdentityIntegrity() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace cogself

#endif // _COGSELF_DISTRIBUTED_IDENTITY_H
