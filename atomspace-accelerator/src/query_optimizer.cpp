/**
 * Query Optimizer Implementation
 */

#include "atomspace_accelerator/query_optimizer.h"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <regex>

namespace atomspace_accelerator {

class QueryOptimizer::Impl {
public:
    int optimizationLevel{2};
    size_t optimizedCount{0};
    size_t cacheHits{0};
    size_t cacheMisses{0};
    size_t rewriteCount{0};
    
    // Query result cache with timestamp
    struct CachedQuery {
        std::string result;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    std::unordered_map<std::string, CachedQuery> queryCache;
    static constexpr size_t MAX_CACHE_SIZE = 100;
    static constexpr int CACHE_TTL_SECONDS = 300; // 5 minutes
    
    // Query pattern rules for rewriting
    struct RewriteRule {
        std::regex pattern;
        std::string replacement;
        std::string description;
    };
    
    std::vector<RewriteRule> rewriteRules;
    
    Impl() {
        initializeRewriteRules();
    }
    
    void initializeRewriteRules() {
        // Common query optimization patterns
        rewriteRules.push_back({
            std::regex(R"(SELECT \* FROM (\w+) WHERE \w+ = \w+ AND \w+ = \w+)"),
            "INDEX_SCAN[$1]",
            "Convert multi-condition WHERE to index scan"
        });
        
        rewriteRules.push_back({
            std::regex(R"(SELECT (\w+) FROM (\w+) ORDER BY \1)"),
            "SORTED_ACCESS[$2,$1]",
            "Use sorted index for ordered retrieval"
        });
        
        rewriteRules.push_back({
            std::regex(R"(COUNT\(\*\) FROM (\w+))"),
            "FAST_COUNT[$1]",
            "Use metadata for count queries"
        });
    }
    
    std::string applyRewriteRules(const std::string& query) {
        std::string optimized = query;
        
        for (const auto& rule : rewriteRules) {
            if (std::regex_search(query, rule.pattern)) {
                optimized = std::regex_replace(query, rule.pattern, rule.replacement);
                rewriteCount++;
                std::cout << "[QueryOptimizer] Applied rule: " << rule.description << std::endl;
                break; // Apply first matching rule
            }
        }
        
        return optimized;
    }
    
    bool isCacheValid(const CachedQuery& cached) const {
        auto now = std::chrono::steady_clock::now();
        auto age = std::chrono::duration_cast<std::chrono::seconds>(now - cached.timestamp);
        return age.count() < CACHE_TTL_SECONDS;
    }
    
    void evictOldCacheEntries() {
        if (queryCache.size() < MAX_CACHE_SIZE) {
            return;
        }
        
        // Remove all invalid entries in one pass
        auto now = std::chrono::steady_clock::now();
        for (auto it = queryCache.begin(); it != queryCache.end();) {
            if (!isCacheValid(it->second)) {
                it = queryCache.erase(it);
            } else {
                ++it;
            }
        }
        
        // If still too large, remove entries in batch
        if (queryCache.size() >= MAX_CACHE_SIZE) {
            size_t toRemove = queryCache.size() - (MAX_CACHE_SIZE * 3 / 4); // Keep 75%
            auto it = queryCache.begin();
            for (size_t i = 0; i < toRemove && it != queryCache.end(); ++i) {
                it = queryCache.erase(it);
            }
        }
    }
};

QueryOptimizer::QueryOptimizer() : pImpl(std::make_unique<Impl>()) {
    std::cout << "[QueryOptimizer] Created" << std::endl;
}

QueryOptimizer::~QueryOptimizer() = default;

std::string QueryOptimizer::optimizeQuery(const std::string& query) {
    pImpl->optimizedCount++;
    
    // Check cache first
    auto cacheIt = pImpl->queryCache.find(query);
    if (cacheIt != pImpl->queryCache.end() && pImpl->isCacheValid(cacheIt->second)) {
        pImpl->cacheHits++;
        std::cout << "[QueryOptimizer] Cache hit for query" << std::endl;
        return cacheIt->second.result;
    }
    
    pImpl->cacheMisses++;
    
    std::cout << "[QueryOptimizer] Optimizing query (level " 
              << pImpl->optimizationLevel << ")" << std::endl;
    
    // Apply optimization based on level
    std::string optimized = query;
    
    if (pImpl->optimizationLevel >= 1) {
        // Level 1: Basic normalization
        optimized = "NORMALIZED[" + optimized + "]";
    }
    
    if (pImpl->optimizationLevel >= 2) {
        // Level 2: Query rewriting
        optimized = pImpl->applyRewriteRules(optimized);
    }
    
    if (pImpl->optimizationLevel >= 3) {
        // Level 3: Advanced pattern recognition
        optimized = "PATTERN_OPTIMIZED[" + optimized + "]";
    }
    
    // Cache the result
    pImpl->evictOldCacheEntries();
    pImpl->queryCache[query] = {optimized, std::chrono::steady_clock::now()};
    
    return optimized;
}

std::string QueryOptimizer::getOptimizationStats() const {
    std::stringstream ss;
    ss << "Query Optimizer Statistics:\n";
    ss << "  Optimization Level: " << pImpl->optimizationLevel << "\n";
    ss << "  Queries Optimized: " << pImpl->optimizedCount << "\n";
    ss << "  Cache Hits: " << pImpl->cacheHits << "\n";
    ss << "  Cache Misses: " << pImpl->cacheMisses << "\n";
    ss << "  Cache Hit Rate: ";
    if (pImpl->cacheHits + pImpl->cacheMisses > 0) {
        double hitRate = (double)pImpl->cacheHits / (pImpl->cacheHits + pImpl->cacheMisses);
        ss << (hitRate * 100.0) << "%\n";
    } else {
        ss << "N/A\n";
    }
    ss << "  Query Rewrites: " << pImpl->rewriteCount << "\n";
    ss << "  Cache Size: " << pImpl->queryCache.size() << "/" << pImpl->MAX_CACHE_SIZE << "\n";
    return ss.str();
}

void QueryOptimizer::setOptimizationLevel(int level) {
    pImpl->optimizationLevel = level;
    std::cout << "[QueryOptimizer] Optimization level set to " << level << std::endl;
}

} // namespace atomspace_accelerator
