/**
 * Agent Implementation
 */

#include "agentic_chatbots/agent.h"
#include <iostream>

namespace agentic_chatbots {

class Agent::Impl {
public:
    std::string name;
    std::string role;
    std::string knowledgeBase;
    ResponseCallback callback;
    size_t interactionCount{0};

    Impl(const std::string& agentName, const std::string& agentRole)
        : name(agentName), role(agentRole) {}
};

Agent::Agent(const std::string& name, const std::string& role)
    : pImpl(std::make_unique<Impl>(name, role)) {
    std::cout << "[Agent] Created agent '" << name << "' with role: " << role << std::endl;
}

Agent::~Agent() = default;

std::string Agent::getName() const {
    return pImpl->name;
}

std::string Agent::getRole() const {
    return pImpl->role;
}

std::string Agent::processInput(const std::string& input) {
    pImpl->interactionCount++;
    
    std::cout << "[Agent:" << pImpl->name << "] Processing input: " << input << std::endl;
    
    // Process input using agent's knowledge and role
    std::string response = "Agent " + pImpl->name + " (" + pImpl->role + ") responds to: " + input;
    
    if (pImpl->callback) {
        pImpl->callback(response);
    }
    
    return response;
}

void Agent::setResponseCallback(ResponseCallback callback) {
    pImpl->callback = callback;
}

void Agent::updateKnowledge(const std::string& knowledge) {
    pImpl->knowledgeBase += knowledge + "\n";
    std::cout << "[Agent:" << pImpl->name << "] Knowledge updated" << std::endl;
}

std::string Agent::getStatus() const {
    std::string status = "Agent: " + pImpl->name + "\n";
    status += "Role: " + pImpl->role + "\n";
    status += "Interactions: " + std::to_string(pImpl->interactionCount) + "\n";
    status += "Knowledge Base Size: " + std::to_string(pImpl->knowledgeBase.size()) + " bytes\n";
    return status;
}

} // namespace agentic_chatbots
