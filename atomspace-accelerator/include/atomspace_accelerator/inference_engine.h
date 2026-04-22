/**
 * AtomSpace Accelerator Inference Engine
 * High-performance inference over AtomSpace hypergraphs
 */

#ifndef _ATOMSPACE_ACCELERATOR_INFERENCE_ENGINE_H
#define _ATOMSPACE_ACCELERATOR_INFERENCE_ENGINE_H

#include <memory>
#include <string>
#include <vector>

namespace atomspace_accelerator {

/**
 * InferenceEngine - Accelerated inference over knowledge graphs
 */
class InferenceEngine {
public:
    InferenceEngine();
    ~InferenceEngine();

    // Initialize the inference engine
    bool initialize();

    // Shutdown the inference engine
    void shutdown();

    // Execute inference query
    std::string executeQuery(const std::string& query);

    // Get inference statistics
    std::string getStatistics() const;

    // Optimize inference paths
    void optimizeInferencePaths();

    // Check if engine is ready
    bool isReady() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace atomspace_accelerator

#endif // _ATOMSPACE_ACCELERATOR_INFERENCE_ENGINE_H
