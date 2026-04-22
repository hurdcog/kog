/**
 * Chatbot Manager Implementation
 */

#include "agentic_chatbots/chatbot_manager.h"
#include <algorithm>
#include <iostream>

namespace agentic_chatbots {

class ChatbotManager::Impl {
public:
    std::vector<std::shared_ptr<Agent>> agents;
    size_t messageCount{0};
};

ChatbotManager::ChatbotManager() : pImpl(std::make_unique<Impl>()) {
    std::cout << "[ChatbotManager] Initialized" << std::endl;
}

ChatbotManager::~ChatbotManager() = default;

void ChatbotManager::registerAgent(std::shared_ptr<Agent> agent) {
    if (!agent) {
        return;
    }

    pImpl->agents.push_back(agent);
    std::cout << "[ChatbotManager] Registered agent: " << agent->getName() << std::endl;
}

void ChatbotManager::unregisterAgent(const std::string& agentName) {
    auto it = std::remove_if(pImpl->agents.begin(), pImpl->agents.end(),
        [&agentName](const std::shared_ptr<Agent>& agent) {
            return agent->getName() == agentName;
        });
    
    if (it != pImpl->agents.end()) {
        pImpl->agents.erase(it, pImpl->agents.end());
        std::cout << "[ChatbotManager] Unregistered agent: " << agentName << std::endl;
    }
}

std::shared_ptr<Agent> ChatbotManager::getAgent(const std::string& agentName) const {
    auto it = std::find_if(pImpl->agents.begin(), pImpl->agents.end(),
        [&agentName](const std::shared_ptr<Agent>& agent) {
            return agent->getName() == agentName;
        });
    
    if (it != pImpl->agents.end()) {
        return *it;
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<Agent>> ChatbotManager::getAllAgents() const {
    return pImpl->agents;
}

std::string ChatbotManager::routeMessage(const std::string& message) {
    pImpl->messageCount++;
    
    std::cout << "[ChatbotManager] Routing message #" << pImpl->messageCount << std::endl;
    
    // Simple routing: use first available agent
    if (!pImpl->agents.empty()) {
        return pImpl->agents[0]->processInput(message);
    }
    
    return "No agents available to process message";
}

std::string ChatbotManager::getStatistics() const {
    std::string stats = "Chatbot Manager Statistics:\n";
    stats += "  Total Agents: " + std::to_string(pImpl->agents.size()) + "\n";
    stats += "  Messages Routed: " + std::to_string(pImpl->messageCount) + "\n";
    stats += "  Registered Agents:\n";
    for (const auto& agent : pImpl->agents) {
        stats += "    - " + agent->getName() + " (" + agent->getRole() + ")\n";
    }
    return stats;
}

} // namespace agentic_chatbots
