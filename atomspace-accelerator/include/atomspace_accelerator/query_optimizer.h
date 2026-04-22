/**
 * Query Optimizer
 * Optimizes queries for efficient execution
 */

#ifndef _ATOMSPACE_ACCELERATOR_QUERY_OPTIMIZER_H
#define _ATOMSPACE_ACCELERATOR_QUERY_OPTIMIZER_H

#include <memory>
#include <string>

namespace atomspace_accelerator {

/**
 * QueryOptimizer - Optimizes query execution plans
 */
class QueryOptimizer {
public:
    QueryOptimizer();
    ~QueryOptimizer();

    // Optimize a query
    std::string optimizeQuery(const std::string& query);

    // Get optimization statistics
    std::string getOptimizationStats() const;

    // Set optimization level
    void setOptimizationLevel(int level);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace atomspace_accelerator

#endif // _ATOMSPACE_ACCELERATOR_QUERY_OPTIMIZER_H
