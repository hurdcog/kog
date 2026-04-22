/**
 * CogSelf - AGI Cognitive Synergy Framework
 * Coordinates cognitive processes for AGI goal achievement
 * Now with CogPrime identity and distributed cognition integration
 */

#ifndef _COGSELF_COGSELF_H
#define _COGSELF_COGSELF_H

#include <memory>
#include <string>

namespace cogself {

// Forward declarations
class CogPrimeIdentity;
class DistributedIdentityManager;

/**
 * CogSelf - Main coordination class for AGI cognitive synergy
 * Enhanced with CogPrime identity and distributed cognition
 */
class CogSelf {
public:
    CogSelf();
    explicit CogSelf(const std::string& agentName);
    ~CogSelf();

    // Initialize CogSelf framework
    bool initialize();
    bool initialize(const std::string& agentId, const std::string& agentName);

    // Shutdown CogSelf framework
    void shutdown();

    // Update cognitive synergy state
    void updateSynergyState();

    // Assess progress toward AGI goals
    double assessAGIProgress() const;

    // Generate self-improvement plan
    std::string generateImprovementPlan() const;

    // Get current synergy level
    double getSynergyLevel() const;

    // Check if framework is running
    bool isRunning() const;
    
    // CogPrime Identity integration
    std::shared_ptr<CogPrimeIdentity> getIdentity() const;
    void recordCognitiveEvent(const std::string& eventId, 
                             const std::string& description,
                             double significance = 0.5);
    std::string getIdentitySummary() const;
    
    // Distributed cognition integration
    std::shared_ptr<DistributedIdentityManager> getDistributedManager() const;
    void enableDistributedCognition();
    void registerDistributedNode(const std::string& nodeId, 
                                const std::string& address,
                                int port);
    void synchronizeDistributedIdentity();
    double getDistributedCoherence() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace cogself

#endif // _COGSELF_COGSELF_H
