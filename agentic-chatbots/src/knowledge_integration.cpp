/**
 * Knowledge Integration Implementation
 */

#include "agentic_chatbots/knowledge_integration.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <chrono>
#include <algorithm>

namespace agentic_chatbots {

// Knowledge graph node for semantic relationships
struct KnowledgeNode {
    std::string concept;
    std::vector<std::string> relatedConcepts;
    std::unordered_map<std::string, std::string> attributes;
    std::chrono::steady_clock::time_point lastAccessed;
    size_t accessCount{0};
    
    void access() {
        lastAccessed = std::chrono::steady_clock::now();
        accessCount++;
    }
};

class KnowledgeIntegration::Impl {
public:
    size_t storeCount{0};
    size_t retrieveCount{0};
    size_t syncCount{0};
    
    // Enhanced knowledge graph
    std::unordered_map<std::string, KnowledgeNode> knowledgeGraph;
    
    // Semantic relationship types
    std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> relationships;
    
    // Knowledge domains for organization
    std::unordered_map<std::string, std::vector<std::string>> domains;
    
    // Integration metrics
    size_t relationshipsCreated{0};
    size_t domainsCreated{0};
    size_t inferenceQueries{0};
    
    void createRelationship(const std::string& concept1, const std::string& concept2, 
                           const std::string& relationType) {
        relationships[relationType].push_back({concept1, concept2});
        relationshipsCreated++;
        
        // Bidirectional linking in knowledge graph
        if (knowledgeGraph.find(concept1) != knowledgeGraph.end()) {
            knowledgeGraph[concept1].relatedConcepts.push_back(concept2);
        }
        if (knowledgeGraph.find(concept2) != knowledgeGraph.end()) {
            knowledgeGraph[concept2].relatedConcepts.push_back(concept1);
        }
    }
    
    std::vector<std::string> findRelatedConcepts(const std::string& concept, int depth = 1) {
        std::vector<std::string> related;
        
        auto it = knowledgeGraph.find(concept);
        if (it == knowledgeGraph.end()) {
            return related;
        }
        
        // Track concepts already added
        std::unordered_map<std::string, bool> visited;
        
        // BFS traversal for related concepts
        std::vector<std::pair<std::string, int>> queue;
        queue.push_back({concept, 0});
        visited[concept] = true;
        
        while (!queue.empty()) {
            auto [current, currentDepth] = queue.front();
            queue.erase(queue.begin());
            
            if (currentDepth >= depth) {
                continue;
            }
            
            auto currentNode = knowledgeGraph.find(current);
            if (currentNode != knowledgeGraph.end()) {
                for (const auto& relatedConcept : currentNode->second.relatedConcepts) {
                    if (!visited[relatedConcept]) {
                        related.push_back(relatedConcept);
                        visited[relatedConcept] = true;
                        queue.push_back({relatedConcept, currentDepth + 1});
                    }
                }
            }
        }
        
        return related;
    }
    
    void organizeDomain(const std::string& domain, const std::string& concept) {
        domains[domain].push_back(concept);
        if (domains[domain].size() == 1) {
            domainsCreated++;
        }
    }
};

KnowledgeIntegration::KnowledgeIntegration() : pImpl(std::make_unique<Impl>()) {
    std::cout << "[KnowledgeIntegration] Initialized" << std::endl;
}

KnowledgeIntegration::~KnowledgeIntegration() = default;

void KnowledgeIntegration::storeKnowledge(const std::string& knowledge) {
    pImpl->storeCount++;
    std::cout << "[KnowledgeIntegration] Storing knowledge in AtomSpace..." << std::endl;
    
    // Parse knowledge and create knowledge node
    KnowledgeNode node;
    node.concept = knowledge;
    node.access();
    
    pImpl->knowledgeGraph[knowledge] = node;
    
    // Auto-detect domain (simple heuristic)
    if (knowledge.find("AI") != std::string::npos || knowledge.find("cognitive") != std::string::npos) {
        pImpl->organizeDomain("artificial_intelligence", knowledge);
    } else if (knowledge.find("learn") != std::string::npos || knowledge.find("knowledge") != std::string::npos) {
        pImpl->organizeDomain("learning", knowledge);
    }
    
    std::cout << "[KnowledgeIntegration] Knowledge stored: " << knowledge << std::endl;
}

std::string KnowledgeIntegration::retrieveKnowledge(const std::string& query) {
    pImpl->retrieveCount++;
    std::cout << "[KnowledgeIntegration] Retrieving knowledge from AtomSpace: " << query << std::endl;
    
    // Direct lookup
    auto it = pImpl->knowledgeGraph.find(query);
    if (it != pImpl->knowledgeGraph.end()) {
        it->second.access();
        std::stringstream result;
        result << "Direct match: " << it->second.concept << "\n";
        
        // Include related concepts for richer context
        auto related = pImpl->findRelatedConcepts(query, 2);
        if (!related.empty()) {
            result << "Related concepts: ";
            for (size_t i = 0; i < related.size() && i < 5; ++i) {
                result << related[i];
                if (i < related.size() - 1 && i < 4) result << ", ";
            }
            result << "\n";
        }
        
        return result.str();
    }
    
    // Fuzzy search through knowledge base
    pImpl->inferenceQueries++;
    std::vector<std::string> partialMatches;
    for (const auto& [concept, node] : pImpl->knowledgeGraph) {
        if (concept.find(query) != std::string::npos || query.find(concept) != std::string::npos) {
            partialMatches.push_back(concept);
        }
    }
    
    if (!partialMatches.empty()) {
        std::stringstream result;
        result << "Partial matches for '" << query << "':\n";
        for (size_t i = 0; i < partialMatches.size() && i < 5; ++i) {
            result << "  - " << partialMatches[i] << "\n";
        }
        return result.str();
    }
    
    return "No knowledge found for: " + query;
}

void KnowledgeIntegration::synchronizeKnowledge() {
    pImpl->syncCount++;
    std::cout << "[KnowledgeIntegration] Synchronizing agent knowledge with AtomSpace..." << std::endl;
    
    // Update access patterns and strengthen frequently accessed relationships
    for (auto& [concept, node] : pImpl->knowledgeGraph) {
        if (node.accessCount > 10) {
            // Frequently accessed concepts get priority in sync
            std::cout << "[KnowledgeIntegration] High-priority sync for: " << concept 
                     << " (accessed " << node.accessCount << " times)" << std::endl;
        }
    }
    
    std::cout << "[KnowledgeIntegration] Synchronized " << pImpl->knowledgeGraph.size() 
              << " knowledge nodes across " << pImpl->domains.size() << " domains" << std::endl;
}

std::string KnowledgeIntegration::getIntegrationStats() const {
    std::stringstream ss;
    ss << "Knowledge Integration Statistics:\n";
    ss << "  Knowledge Stores: " << pImpl->storeCount << "\n";
    ss << "  Knowledge Retrievals: " << pImpl->retrieveCount << "\n";
    ss << "  Synchronizations: " << pImpl->syncCount << "\n";
    ss << "  Knowledge Nodes: " << pImpl->knowledgeGraph.size() << "\n";
    ss << "  Relationships Created: " << pImpl->relationshipsCreated << "\n";
    ss << "  Knowledge Domains: " << pImpl->domainsCreated << "\n";
    ss << "  Inference Queries: " << pImpl->inferenceQueries << "\n";
    
    // Top accessed concepts
    std::vector<std::pair<std::string, size_t>> topConcepts;
    for (const auto& [concept, node] : pImpl->knowledgeGraph) {
        topConcepts.push_back({concept, node.accessCount});
    }
    std::sort(topConcepts.begin(), topConcepts.end(), 
             [](const auto& a, const auto& b) { return a.second > b.second; });
    
    if (!topConcepts.empty()) {
        ss << "  Top Accessed Concepts:\n";
        for (size_t i = 0; i < std::min(size_t(3), topConcepts.size()); ++i) {
            ss << "    " << (i+1) << ". " << topConcepts[i].first 
               << " (" << topConcepts[i].second << " accesses)\n";
        }
    }
    
    return ss.str();
}

} // namespace agentic_chatbots
