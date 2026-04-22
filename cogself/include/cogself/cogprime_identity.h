/**
 * CogPrime Identity Module
 * Implements core self-identity for distributed cognitive agents
 * 
 * Based on CogPrime cognitive architecture principles:
 * - Self-model with cognitive characteristics
 * - Identity persistence across distributed systems
 * - Memory and goal tracking
 * - Cognitive synergy awareness
 */

#ifndef _COGSELF_COGPRIME_IDENTITY_H
#define _COGSELF_COGPRIME_IDENTITY_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>

namespace cogself {

/**
 * Cognitive characteristic representing aspect of agent identity
 */
struct CognitiveCharacteristic {
    std::string name;
    std::string description;
    double strength;        // 0.0 to 1.0
    double stability;       // How stable this characteristic is
    
    CognitiveCharacteristic(const std::string& n, const std::string& d, 
                           double str = 0.5, double stab = 0.5)
        : name(n), description(d), strength(str), stability(stab) {}
};

/**
 * Episodic memory entry
 */
struct EpisodicMemory {
    std::string eventId;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    double significance;    // 0.0 to 1.0
    std::map<std::string, std::string> context;
    
    EpisodicMemory(const std::string& id, const std::string& desc, double sig = 0.5)
        : eventId(id), description(desc), 
          timestamp(std::chrono::system_clock::now()), 
          significance(sig) {}
};

/**
 * Cognitive goal with progress tracking
 */
struct CognitiveGoal {
    std::string goalId;
    std::string description;
    double priority;        // 0.0 to 1.0
    double progress;        // 0.0 to 1.0
    bool active;
    std::chrono::system_clock::time_point createdAt;
    
    CognitiveGoal(const std::string& id, const std::string& desc, double pri = 0.5)
        : goalId(id), description(desc), priority(pri), progress(0.0), 
          active(true), createdAt(std::chrono::system_clock::now()) {}
};

/**
 * CogPrimeIdentity - Core identity for CogPrime-based cognitive agents
 * 
 * Implements self-model with:
 * - Unique identity and characteristics
 * - Episodic memory
 * - Goal hierarchy
 * - Cognitive synergy awareness
 * - Distributed network identity
 */
class CogPrimeIdentity {
public:
    CogPrimeIdentity(const std::string& agentId, const std::string& agentName);
    ~CogPrimeIdentity();
    
    // Identity accessors
    std::string getAgentId() const;
    std::string getAgentName() const;
    void setAgentName(const std::string& name);
    
    // Cognitive characteristics
    void addCharacteristic(const CognitiveCharacteristic& characteristic);
    void updateCharacteristic(const std::string& name, double strength);
    CognitiveCharacteristic getCharacteristic(const std::string& name) const;
    std::vector<CognitiveCharacteristic> getAllCharacteristics() const;
    
    // Episodic memory
    void recordMemory(const EpisodicMemory& memory);
    void recordMemory(const std::string& eventId, const std::string& description, 
                     double significance = 0.5);
    std::vector<EpisodicMemory> getRecentMemories(size_t count = 10) const;
    std::vector<EpisodicMemory> getSignificantMemories(double minSignificance = 0.7) const;
    
    // Goal management
    void addGoal(const CognitiveGoal& goal);
    void addGoal(const std::string& goalId, const std::string& description, 
                 double priority = 0.5);
    void updateGoalProgress(const std::string& goalId, double progress);
    void completeGoal(const std::string& goalId);
    std::vector<CognitiveGoal> getActiveGoals() const;
    CognitiveGoal getGoal(const std::string& goalId) const;
    
    // Self-awareness metrics
    double getSelfCoherence() const;
    double getIdentityStrength() const;
    double getCognitiveIntegration() const;
    
    // Distributed identity
    void setDistributedNodeId(const std::string& nodeId);
    std::string getDistributedNodeId() const;
    void recordDistributedInteraction(const std::string& nodeId, 
                                     const std::string& interaction);
    
    // Identity serialization for network distribution
    std::string serializeIdentity() const;
    bool deserializeIdentity(const std::string& data);
    
    // Identity snapshot for persistence
    struct IdentitySnapshot {
        std::string agentId;
        std::string agentName;
        std::string distributedNodeId;
        size_t characteristicCount;
        size_t memoryCount;
        size_t activeGoalCount;
        double selfCoherence;
        double identityStrength;
        std::chrono::system_clock::time_point snapshotTime;
    };
    
    IdentitySnapshot createSnapshot() const;
    std::string getIdentitySummary() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace cogself

#endif // _COGSELF_COGPRIME_IDENTITY_H
