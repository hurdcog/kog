/**
 * Agentic Chatbot Agent
 * Autonomous agent with chatbot capabilities
 */

#ifndef _AGENTIC_CHATBOTS_AGENT_H
#define _AGENTIC_CHATBOTS_AGENT_H

#include <memory>
#include <string>
#include <functional>

namespace agentic_chatbots {

/**
 * Agent - Autonomous chatbot agent
 */
class Agent {
public:
    using ResponseCallback = std::function<void(const std::string&)>;

    Agent(const std::string& name, const std::string& role);
    ~Agent();

    // Get agent name
    std::string getName() const;

    // Get agent role
    std::string getRole() const;

    // Process user input and generate response
    std::string processInput(const std::string& input);

    // Set response callback
    void setResponseCallback(ResponseCallback callback);

    // Update agent knowledge
    void updateKnowledge(const std::string& knowledge);

    // Get agent status
    std::string getStatus() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace agentic_chatbots

#endif // _AGENTIC_CHATBOTS_AGENT_H
