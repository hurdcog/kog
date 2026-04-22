/**
 * Pattern Miner Implementation
 */

#include "atomspace_accelerator/pattern_miner.h"
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <algorithm>

namespace atomspace_accelerator {

class PatternMiner::Impl {
public:
    double minSupport{0.1};
    double minConfidence{0.5};
    
    std::vector<std::string> dataCollection;
    std::unordered_map<std::string, CognitivePattern> discoveredPatterns;
    
    size_t miningOperations{0};
    size_t patternsDiscovered{0};
    size_t associationsFound{0};
    
    // Extract tokens/components from data
    std::vector<std::string> tokenize(const std::string& data) const {
        std::vector<std::string> tokens;
        std::stringstream ss(data);
        std::string token;
        
        while (ss >> token) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    // Calculate support for a pattern
    double calculateSupport(const std::vector<std::string>& pattern) const {
        if (dataCollection.empty()) return 0.0;
        
        size_t matches = 0;
        for (const auto& data : dataCollection) {
            auto tokens = tokenize(data);
            
            // Check if all pattern components appear in tokens
            bool allFound = true;
            for (const auto& component : pattern) {
                if (std::find(tokens.begin(), tokens.end(), component) == tokens.end()) {
                    allFound = false;
                    break;
                }
            }
            
            if (allFound) {
                matches++;
            }
        }
        
        return static_cast<double>(matches) / dataCollection.size();
    }
    
    // Generate single-item patterns
    std::unordered_map<std::string, size_t> generateFrequentItems() {
        std::unordered_map<std::string, size_t> itemCounts;
        
        for (const auto& data : dataCollection) {
            auto tokens = tokenize(data);
            for (const auto& token : tokens) {
                itemCounts[token]++;
            }
        }
        
        return itemCounts;
    }
    
    // Create pattern from components
    CognitivePattern createPattern(const std::vector<std::string>& components) {
        CognitivePattern pattern;
        
        // Generate pattern ID
        std::stringstream idStream;
        for (size_t i = 0; i < components.size(); ++i) {
            idStream << components[i];
            if (i < components.size() - 1) idStream << "_";
        }
        pattern.patternId = idStream.str();
        
        pattern.components = components;
        pattern.support = calculateSupport(components);
        pattern.frequency = static_cast<size_t>(pattern.support * dataCollection.size());
        pattern.confidence = pattern.support; // Simplified
        
        std::stringstream desc;
        desc << "Pattern of " << components.size() << " components";
        pattern.description = desc.str();
        
        return pattern;
    }
};

PatternMiner::PatternMiner() : pImpl(std::make_unique<Impl>()) {
    std::cout << "[PatternMiner] Initialized for cognitive pattern discovery" << std::endl;
}

PatternMiner::~PatternMiner() = default;

std::vector<CognitivePattern> PatternMiner::minePatterns(const std::string& data) {
    pImpl->miningOperations++;
    
    std::vector<CognitivePattern> patterns;
    auto tokens = pImpl->tokenize(data);
    
    std::cout << "[PatternMiner] Mining patterns from data with " << tokens.size() << " tokens" << std::endl;
    
    // Generate single-item patterns
    for (const auto& token : tokens) {
        CognitivePattern pattern = pImpl->createPattern({token});
        if (pattern.support >= pImpl->minSupport) {
            patterns.push_back(pattern);
            pImpl->discoveredPatterns[pattern.patternId] = pattern;
            pImpl->patternsDiscovered++;
        }
    }
    
    // Generate two-item patterns
    for (size_t i = 0; i < tokens.size(); ++i) {
        for (size_t j = i + 1; j < tokens.size() && j < i + 5; ++j) {
            CognitivePattern pattern = pImpl->createPattern({tokens[i], tokens[j]});
            if (pattern.support >= pImpl->minSupport) {
                patterns.push_back(pattern);
                pImpl->discoveredPatterns[pattern.patternId] = pattern;
                pImpl->patternsDiscovered++;
            }
        }
    }
    
    std::cout << "[PatternMiner] Discovered " << patterns.size() << " patterns" << std::endl;
    return patterns;
}

std::vector<CognitivePattern> PatternMiner::mineFrequentPatterns(double minSupport) {
    std::vector<CognitivePattern> frequentPatterns;
    
    std::cout << "[PatternMiner] Mining frequent patterns (min support: " << minSupport << ")" << std::endl;
    
    // Get frequent items
    auto itemCounts = pImpl->generateFrequentItems();
    size_t threshold = static_cast<size_t>(minSupport * pImpl->dataCollection.size());
    
    for (const auto& [item, count] : itemCounts) {
        if (count >= threshold) {
            CognitivePattern pattern = pImpl->createPattern({item});
            frequentPatterns.push_back(pattern);
        }
    }
    
    std::cout << "[PatternMiner] Found " << frequentPatterns.size() << " frequent patterns" << std::endl;
    return frequentPatterns;
}

std::vector<std::pair<std::string, std::string>> PatternMiner::mineAssociations(double minConfidence) {
    std::vector<std::pair<std::string, std::string>> associations;
    
    std::cout << "[PatternMiner] Mining associations (min confidence: " << minConfidence << ")" << std::endl;
    
    // Find associations between discovered patterns
    std::vector<CognitivePattern> patterns;
    for (const auto& [id, pattern] : pImpl->discoveredPatterns) {
        patterns.push_back(pattern);
    }
    
    for (size_t i = 0; i < patterns.size(); ++i) {
        for (size_t j = i + 1; j < patterns.size(); ++j) {
            // Calculate confidence of association
            double confidence = std::min(patterns[i].confidence, patterns[j].confidence);
            
            if (confidence >= minConfidence) {
                associations.push_back({patterns[i].patternId, patterns[j].patternId});
                pImpl->associationsFound++;
            }
        }
    }
    
    std::cout << "[PatternMiner] Found " << associations.size() << " associations" << std::endl;
    return associations;
}

void PatternMiner::addData(const std::string& data) {
    pImpl->dataCollection.push_back(data);
    std::cout << "[PatternMiner] Added data to collection (total: " 
              << pImpl->dataCollection.size() << ")" << std::endl;
}

std::string PatternMiner::getMiningStats() const {
    std::stringstream ss;
    ss << "Pattern Mining Statistics:\n";
    ss << "  Mining Operations: " << pImpl->miningOperations << "\n";
    ss << "  Patterns Discovered: " << pImpl->patternsDiscovered << "\n";
    ss << "  Associations Found: " << pImpl->associationsFound << "\n";
    ss << "  Data Collection Size: " << pImpl->dataCollection.size() << "\n";
    ss << "  Unique Patterns: " << pImpl->discoveredPatterns.size() << "\n";
    ss << "  Min Support: " << pImpl->minSupport << "\n";
    ss << "  Min Confidence: " << pImpl->minConfidence << "\n";
    
    // Top patterns by support
    std::vector<CognitivePattern> topPatterns;
    for (const auto& [id, pattern] : pImpl->discoveredPatterns) {
        topPatterns.push_back(pattern);
    }
    
    std::sort(topPatterns.begin(), topPatterns.end(),
        [](const CognitivePattern& a, const CognitivePattern& b) {
            return a.support > b.support;
        });
    
    if (!topPatterns.empty()) {
        ss << "  Top Patterns:\n";
        for (size_t i = 0; i < std::min(size_t(3), topPatterns.size()); ++i) {
            ss << "    " << (i+1) << ". " << topPatterns[i].patternId
               << " (support: " << topPatterns[i].support << ")\n";
        }
    }
    
    return ss.str();
}

void PatternMiner::setMinimumSupport(double support) {
    pImpl->minSupport = support;
    std::cout << "[PatternMiner] Minimum support set to " << support << std::endl;
}

void PatternMiner::setMinimumConfidence(double confidence) {
    pImpl->minConfidence = confidence;
    std::cout << "[PatternMiner] Minimum confidence set to " << confidence << std::endl;
}

} // namespace atomspace_accelerator
