/**
 * Collaborative Reasoner Implementation
 */

#include "agentic_chatbots/collaborative_reasoner.h"
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <chrono>

namespace agentic_chatbots {

class CollaborativeReasoner::Impl {
public:
    std::unordered_map<std::string, ReasoningSession> sessions;
    
    size_t sessionsCreated{0};
    size_t propositionsSubmitted{0};
    size_t consensusBuilt{0};
    
    // Constants
    static constexpr double DEFAULT_CONSENSUS_CONFIDENCE = 0.0;
    
    std::string generateSessionId() {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        return "session_" + std::to_string(timestamp);
    }
    
    // Calculate agreement between propositions
    double calculateAgreement(const std::vector<ReasoningProposition>& propositions) const {
        if (propositions.size() < 2) return 1.0;
        
        // Simplified agreement: average confidence
        double totalConfidence = 0.0;
        for (const auto& prop : propositions) {
            totalConfidence += prop.confidence;
        }
        
        return totalConfidence / propositions.size();
    }
    
    // Merge propositions into consensus
    std::string mergePropositions(const std::vector<ReasoningProposition>& propositions) const {
        if (propositions.empty()) return "No consensus reached";
        
        // Sort by confidence
        std::vector<ReasoningProposition> sorted = propositions;
        std::sort(sorted.begin(), sorted.end(),
            [](const ReasoningProposition& a, const ReasoningProposition& b) {
                return a.confidence > b.confidence;
            });
        
        // Take high-confidence propositions
        std::stringstream consensus;
        consensus << "Consensus (from " << propositions.size() << " agents):\n";
        
        for (size_t i = 0; i < std::min(size_t(3), sorted.size()); ++i) {
            consensus << "  - " << sorted[i].proposition 
                     << " [Agent: " << sorted[i].agentId 
                     << ", Confidence: " << sorted[i].confidence << "]\n";
        }
        
        return consensus.str();
    }
};

CollaborativeReasoner::CollaborativeReasoner() : pImpl(std::make_unique<Impl>()) {
    std::cout << "[CollaborativeReasoner] Initialized for multi-agent reasoning" << std::endl;
}

CollaborativeReasoner::~CollaborativeReasoner() = default;

std::string CollaborativeReasoner::createSession(const std::string& topic) {
    std::string sessionId = pImpl->generateSessionId();
    
    ReasoningSession session;
    session.sessionId = sessionId;
    session.topic = topic;
    session.consensusConfidence = Impl::DEFAULT_CONSENSUS_CONFIDENCE;
    
    pImpl->sessions[sessionId] = session;
    pImpl->sessionsCreated++;
    
    std::cout << "[CollaborativeReasoner] Created reasoning session: " << sessionId 
              << " on topic: " << topic << std::endl;
    
    return sessionId;
}

void CollaborativeReasoner::addAgent(const std::string& sessionId, const std::string& agentId) {
    auto it = pImpl->sessions.find(sessionId);
    if (it == pImpl->sessions.end()) {
        std::cerr << "[CollaborativeReasoner] Session not found: " << sessionId << std::endl;
        return;
    }
    
    it->second.participatingAgents.push_back(agentId);
    
    std::cout << "[CollaborativeReasoner] Added agent " << agentId 
              << " to session " << sessionId << std::endl;
}

void CollaborativeReasoner::submitProposition(const std::string& sessionId, 
                                               const ReasoningProposition& proposition) {
    auto it = pImpl->sessions.find(sessionId);
    if (it == pImpl->sessions.end()) {
        std::cerr << "[CollaborativeReasoner] Session not found: " << sessionId << std::endl;
        return;
    }
    
    it->second.propositions.push_back(proposition);
    pImpl->propositionsSubmitted++;
    
    std::cout << "[CollaborativeReasoner] Agent " << proposition.agentId 
              << " submitted proposition (confidence: " << proposition.confidence << ")" << std::endl;
}

std::string CollaborativeReasoner::buildConsensus(const std::string& sessionId) {
    auto it = pImpl->sessions.find(sessionId);
    if (it == pImpl->sessions.end()) {
        return "Session not found";
    }
    
    std::cout << "[CollaborativeReasoner] Building consensus for session: " << sessionId << std::endl;
    
    auto& session = it->second;
    
    if (session.propositions.empty()) {
        session.consensusResult = "No propositions to build consensus from";
        session.consensusConfidence = Impl::DEFAULT_CONSENSUS_CONFIDENCE;
    } else {
        session.consensusResult = pImpl->mergePropositions(session.propositions);
        session.consensusConfidence = pImpl->calculateAgreement(session.propositions);
        pImpl->consensusBuilt++;
    }
    
    std::cout << "[CollaborativeReasoner] Consensus built with confidence: " 
              << session.consensusConfidence << std::endl;
    
    return session.consensusResult;
}

ReasoningSession CollaborativeReasoner::getSession(const std::string& sessionId) const {
    auto it = pImpl->sessions.find(sessionId);
    if (it != pImpl->sessions.end()) {
        return it->second;
    }
    
    return ReasoningSession{};
}

double CollaborativeReasoner::evaluateProposition(const ReasoningProposition& proposition) const {
    double quality = proposition.confidence;
    
    // Boost quality based on supporting evidence
    double evidenceBonus = std::min(proposition.supportingEvidence.size() * 0.1, 0.3);
    quality = std::min(quality + evidenceBonus, 1.0);
    
    return quality;
}

std::string CollaborativeReasoner::getReasoningStats() const {
    std::stringstream ss;
    ss << "Collaborative Reasoning Statistics:\n";
    ss << "  Sessions Created: " << pImpl->sessionsCreated << "\n";
    ss << "  Propositions Submitted: " << pImpl->propositionsSubmitted << "\n";
    ss << "  Consensus Built: " << pImpl->consensusBuilt << "\n";
    ss << "  Active Sessions: " << pImpl->sessions.size() << "\n";
    
    // Session details
    if (!pImpl->sessions.empty()) {
        ss << "  Recent Sessions:\n";
        size_t count = 0;
        for (const auto& [id, session] : pImpl->sessions) {
            if (count++ >= 3) break;
            ss << "    - " << session.topic 
               << " (" << session.participatingAgents.size() << " agents, "
               << session.propositions.size() << " propositions)\n";
        }
    }
    
    // Calculate average consensus confidence
    if (pImpl->consensusBuilt > 0) {
        double totalConfidence = 0.0;
        size_t validSessions = 0;
        for (const auto& [id, session] : pImpl->sessions) {
            if (!session.propositions.empty()) {
                totalConfidence += session.consensusConfidence;
                validSessions++;
            }
        }
        
        if (validSessions > 0) {
            ss << "  Average Consensus Confidence: " << (totalConfidence / validSessions) << "\n";
        }
    }
    
    return ss.str();
}

} // namespace agentic_chatbots
