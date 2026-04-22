/**
 * Chatbot Manager
 * Manages multiple agentic chatbots
 */

#ifndef _AGENTIC_CHATBOTS_CHATBOT_MANAGER_H
#define _AGENTIC_CHATBOTS_CHATBOT_MANAGER_H

#include <memory>
#include <string>
#include <vector>
#include "agent.h"

namespace agentic_chatbots {

/**
 * ChatbotManager - Manages agentic chatbot agents
 */
class ChatbotManager {
public:
    ChatbotManager();
    ~ChatbotManager();

    // Register a new agent
    void registerAgent(std::shared_ptr<Agent> agent);

    // Unregister an agent
    void unregisterAgent(const std::string& agentName);

    // Get agent by name
    std::shared_ptr<Agent> getAgent(const std::string& agentName) const;

    // Get all agents
    std::vector<std::shared_ptr<Agent>> getAllAgents() const;

    // Route message to appropriate agent
    std::string routeMessage(const std::string& message);

    // Get manager statistics
    std::string getStatistics() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace agentic_chatbots

#endif // _AGENTIC_CHATBOTS_CHATBOT_MANAGER_H
