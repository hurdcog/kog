/**
 * Pattern Miner
 * Advanced pattern mining and discovery in cognitive data
 */

#ifndef _ATOMSPACE_ACCELERATOR_PATTERN_MINER_H
#define _ATOMSPACE_ACCELERATOR_PATTERN_MINER_H

#include <memory>
#include <string>
#include <vector>

namespace atomspace_accelerator {

/**
 * Discovered pattern representation
 */
struct CognitivePattern {
    std::string patternId;
    std::string description;
    double support;      // How often the pattern appears
    double confidence;   // Reliability of the pattern
    size_t frequency;    // Number of occurrences
    std::vector<std::string> components;
};

/**
 * PatternMiner - Discovers patterns in cognitive data
 */
class PatternMiner {
public:
    PatternMiner();
    ~PatternMiner();

    // Mine patterns from data
    std::vector<CognitivePattern> minePatterns(const std::string& data);
    
    // Mine frequent patterns (appearing > minSupport)
    std::vector<CognitivePattern> mineFrequentPatterns(double minSupport);
    
    // Mine association rules between patterns
    std::vector<std::pair<std::string, std::string>> mineAssociations(double minConfidence);
    
    // Add data for pattern mining
    void addData(const std::string& data);
    
    // Get pattern mining statistics
    std::string getMiningStats() const;
    
    // Set minimum support threshold
    void setMinimumSupport(double support);
    
    // Set minimum confidence threshold
    void setMinimumConfidence(double confidence);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace atomspace_accelerator

#endif // _ATOMSPACE_ACCELERATOR_PATTERN_MINER_H
