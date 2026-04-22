/*
 * TensorLogic.h - Multi-Entity Network-Aware Tensor-Enhanced Cognitive Synergy
 *
 * This header defines the main TensorLogic class that integrates:
 * - ATen tensor operations
 * - ATenSpace symbolic-neural bridging
 * - Multi-Entity AtomSpace management
 * - Multi-Scale temporal analysis
 * - Network-Aware Distributed AtomSpace (DAS)
 * - ESN Reservoir Computing
 *
 * Part of the OpenCog Collection (OCC)
 */

#ifndef _OPENCOG_TENSOR_LOGIC_H
#define _OPENCOG_TENSOR_LOGIC_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <atomic>

namespace opencog {
namespace tensor_logic {

// Forward declarations
class MultiEntityAtomSpace;
class MultiScaleAnalysis;
class NetworkAwareDAS;
class ESNReservoir;
class TensorAtomSpace;

/**
 * TensorLogic - Main integration class for tensor-enhanced cognitive synergy
 *
 * Provides unified interface for:
 * 1. Multi-Entity Management: Handle multiple cognitive entities with tensor state
 * 2. Multi-Scale Processing: Analyze patterns at different temporal granularities
 * 3. Network-Aware Distribution: Track and optimize DAS flow patterns
 * 4. Reservoir Computing: ESN-based temporal processing
 * 5. Tensor Operations: Mathematical operations on atomspace structures
 */
class TensorLogic {
public:
    // Tensor dimension configuration
    struct TensorConfig {
        size_t embedding_dim = 128;      // Default embedding dimension
        size_t hidden_dim = 256;         // Hidden layer dimension
        size_t reservoir_size = 1000;    // ESN reservoir size
        double spectral_radius = 0.9;    // ESN spectral radius
        double sparsity = 0.1;           // ESN connection sparsity
        size_t num_scales = 5;           // Number of temporal scales
        double scale_factor = 2.0;       // Scale multiplier
    };

    // Entity state representation
    struct EntityState {
        std::string entity_id;
        std::vector<float> tensor_state;   // Current tensor embedding
        std::vector<float> attention_weights;
        double activation_level;
        uint64_t last_update_timestamp;
    };

    // Flow tensor for DAS awareness
    struct FlowTensor {
        std::string source_node;
        std::string target_node;
        std::vector<float> flow_vector;
        double bandwidth;
        double latency;
        uint64_t timestamp;
    };

    // Constructor/Destructor
    TensorLogic();
    explicit TensorLogic(const TensorConfig& config);
    ~TensorLogic();

    // Initialization
    bool initialize();
    bool shutdown();
    bool isInitialized() const { return initialized_.load(); }

    // Configuration
    void setConfig(const TensorConfig& config);
    TensorConfig getConfig() const;

    // Multi-Entity AtomSpace Operations
    std::string createEntity(const std::string& name);
    bool removeEntity(const std::string& entity_id);
    EntityState getEntityState(const std::string& entity_id) const;
    bool updateEntityState(const std::string& entity_id, const std::vector<float>& new_state);
    std::vector<std::string> getAllEntities() const;

    // Entity Tensor Operations
    std::vector<float> computeEntitySimilarity(const std::string& entity1, const std::string& entity2);
    std::vector<float> aggregateEntityStates(const std::vector<std::string>& entity_ids);
    bool synchronizeEntities(const std::vector<std::string>& entity_ids);

    // Multi-Scale Analysis Operations
    std::vector<float> analyzeAtScale(const std::vector<float>& input, size_t scale_index);
    std::vector<std::vector<float>> multiScaleDecomposition(const std::vector<float>& input);
    std::vector<float> multiScaleReconstruction(const std::vector<std::vector<float>>& scales);
    std::vector<float> detectPatternAtScale(const std::vector<float>& pattern, size_t scale);

    // Network-Aware DAS Operations
    void recordFlow(const FlowTensor& flow);
    std::vector<FlowTensor> getFlowHistory(const std::string& node_id, size_t limit = 100);
    std::vector<float> predictOptimalRoute(const std::string& source, const std::string& target);
    double estimateTransferCost(const std::string& source, const std::string& target);
    std::vector<std::string> getOptimalNodePlacement(const std::vector<float>& atom_tensor);

    // ESN Reservoir Computing Operations
    std::vector<float> reservoirTransform(const std::vector<float>& input);
    void trainReservoir(const std::vector<std::vector<float>>& inputs,
                       const std::vector<std::vector<float>>& targets);
    std::vector<float> predictSequence(const std::vector<float>& seed, size_t steps);
    void resetReservoirState();

    // Tensor-Enhanced AtomSpace Operations
    std::vector<float> atomToTensor(const std::string& atom_id);
    std::string tensorToAtom(const std::vector<float>& tensor);
    std::vector<float> linkToTensor(const std::string& link_id);
    std::vector<float> computeTensorTruthValue(const std::vector<float>& tensor);

    // Cognitive Synergy Operations
    std::vector<float> computeCognitiveSynergy(
        const std::vector<std::string>& entity_ids,
        const std::vector<std::vector<float>>& multi_scale_inputs);

    double measureSynergyStrength(const std::vector<std::string>& entity_ids);

    std::vector<float> propagateAttention(
        const std::vector<float>& source_attention,
        const std::vector<std::string>& target_entities);

    // Utility methods
    static std::vector<float> normalizeVector(const std::vector<float>& vec);
    static float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b);
    static std::vector<float> elementwiseMultiply(const std::vector<float>& a, const std::vector<float>& b);
    static std::vector<float> matrixVectorMultiply(const std::vector<std::vector<float>>& matrix,
                                                   const std::vector<float>& vec);

private:
    TensorConfig config_;
    std::atomic<bool> initialized_{false};
    mutable std::mutex mutex_;

    // Component instances
    std::unique_ptr<MultiEntityAtomSpace> multi_entity_;
    std::unique_ptr<MultiScaleAnalysis> multi_scale_;
    std::unique_ptr<NetworkAwareDAS> network_das_;
    std::unique_ptr<ESNReservoir> esn_reservoir_;
    std::unique_ptr<TensorAtomSpace> tensor_atomspace_;

    // Internal state
    std::unordered_map<std::string, EntityState> entities_;
    std::vector<FlowTensor> flow_history_;
    std::vector<float> reservoir_state_;

    // Helper methods
    void initializeComponents();
    void cleanupComponents();
    std::string generateEntityId();
};

} // namespace tensor_logic
} // namespace opencog

#endif // _OPENCOG_TENSOR_LOGIC_H
