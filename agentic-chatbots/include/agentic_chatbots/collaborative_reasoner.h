/**
 * Collaborative Reasoner
 * Multi-agent collaborative reasoning and consensus building
 */

#ifndef _AGENTIC_CHATBOTS_COLLABORATIVE_REASONER_H
#define _AGENTIC_CHATBOTS_COLLABORATIVE_REASONER_H

#include <memory>
#include <string>
#include <vector>

namespace agentic_chatbots {

/**
 * Reasoning proposition from an agent
 */
struct ReasoningProposition {
    std::string agentId;
    std::string proposition;
    double confidence;
    std::vector<std::string> supportingEvidence;
};

/**
 * Collaborative reasoning session
 */
struct ReasoningSession {
    std::string sessionId;
    std::string topic;
    std::vector<std::string> participatingAgents;
    std::vector<ReasoningProposition> propositions;
    std::string consensusResult;
    double consensusConfidence;
};

/**
 * CollaborativeReasoner - Enables multi-agent reasoning
 */
class CollaborativeReasoner {
public:
    CollaborativeReasoner();
    ~CollaborativeReasoner();

    // Create a new reasoning session
    std::string createSession(const std::string& topic);
    
    // Add agent to reasoning session
    void addAgent(const std::string& sessionId, const std::string& agentId);
    
    // Submit a reasoning proposition
    void submitProposition(const std::string& sessionId, const ReasoningProposition& proposition);
    
    // Build consensus from propositions
    std::string buildConsensus(const std::string& sessionId);
    
    // Get session details
    ReasoningSession getSession(const std::string& sessionId) const;
    
    // Evaluate proposition quality
    double evaluateProposition(const ReasoningProposition& proposition) const;
    
    // Get collaborative reasoning statistics
    std::string getReasoningStats() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace agentic_chatbots

#endif // _AGENTIC_CHATBOTS_COLLABORATIVE_REASONER_H
