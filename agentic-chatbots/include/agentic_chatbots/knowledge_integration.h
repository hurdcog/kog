/**
 * Knowledge Integration
 * Integrates chatbot knowledge with AtomSpace
 */

#ifndef _AGENTIC_CHATBOTS_KNOWLEDGE_INTEGRATION_H
#define _AGENTIC_CHATBOTS_KNOWLEDGE_INTEGRATION_H

#include <memory>
#include <string>

namespace agentic_chatbots {

/**
 * KnowledgeIntegration - Bridges chatbot knowledge with AtomSpace
 */
class KnowledgeIntegration {
public:
    KnowledgeIntegration();
    ~KnowledgeIntegration();

    // Store knowledge in AtomSpace
    void storeKnowledge(const std::string& knowledge);

    // Retrieve knowledge from AtomSpace
    std::string retrieveKnowledge(const std::string& query);

    // Synchronize agent knowledge with AtomSpace
    void synchronizeKnowledge();

    // Get integration statistics
    std::string getIntegrationStats() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace agentic_chatbots

#endif // _AGENTIC_CHATBOTS_KNOWLEDGE_INTEGRATION_H
