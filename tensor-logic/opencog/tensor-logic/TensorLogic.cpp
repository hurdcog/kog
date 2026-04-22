/*
 * TensorLogic.cpp - Implementation of Multi-Entity Network-Aware Tensor-Enhanced Cognitive Synergy
 *
 * Part of the OpenCog Collection (OCC)
 */

#include "TensorLogic.h"
#include "MultiEntityAtomSpace.h"
#include "MultiScaleAnalysis.h"
#include "NetworkAwareDAS.h"
#include "ESNReservoir.h"
#include "TensorAtomSpace.h"

#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace opencog {
namespace tensor_logic {

// Constructor
TensorLogic::TensorLogic() : TensorLogic(TensorConfig{}) {}

TensorLogic::TensorLogic(const TensorConfig& config) : config_(config) {
    reservoir_state_.resize(config_.reservoir_size, 0.0f);
}

// Destructor
TensorLogic::~TensorLogic() {
    if (initialized_.load()) {
        shutdown();
    }
}

// Initialization
bool TensorLogic::initialize() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (initialized_.load()) {
        return true;
    }

    try {
        initializeComponents();
        initialized_.store(true);
        return true;
    } catch (const std::exception& e) {
        cleanupComponents();
        return false;
    }
}

bool TensorLogic::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_.load()) {
        return true;
    }

    cleanupComponents();
    initialized_.store(false);
    return true;
}

void TensorLogic::setConfig(const TensorConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
    reservoir_state_.resize(config_.reservoir_size, 0.0f);
}

TensorLogic::TensorConfig TensorLogic::getConfig() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return config_;
}

// Multi-Entity AtomSpace Operations
std::string TensorLogic::createEntity(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string entity_id = generateEntityId();

    EntityState state;
    state.entity_id = entity_id;
    state.tensor_state.resize(config_.embedding_dim, 0.0f);
    state.attention_weights.resize(config_.embedding_dim, 1.0f / config_.embedding_dim);
    state.activation_level = 1.0;
    state.last_update_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    // Initialize with random embedding
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> dist(0.0f, 1.0f / std::sqrt(config_.embedding_dim));
    for (auto& v : state.tensor_state) {
        v = dist(gen);
    }

    entities_[entity_id] = state;

    if (multi_entity_) {
        multi_entity_->registerEntity(entity_id, name, state.tensor_state);
    }

    return entity_id;
}

bool TensorLogic::removeEntity(const std::string& entity_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = entities_.find(entity_id);
    if (it == entities_.end()) {
        return false;
    }

    entities_.erase(it);

    if (multi_entity_) {
        multi_entity_->unregisterEntity(entity_id);
    }

    return true;
}

TensorLogic::EntityState TensorLogic::getEntityState(const std::string& entity_id) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = entities_.find(entity_id);
    if (it == entities_.end()) {
        return EntityState{};
    }

    return it->second;
}

bool TensorLogic::updateEntityState(const std::string& entity_id, const std::vector<float>& new_state) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = entities_.find(entity_id);
    if (it == entities_.end()) {
        return false;
    }

    it->second.tensor_state = new_state;
    it->second.last_update_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    return true;
}

std::vector<std::string> TensorLogic::getAllEntities() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::string> result;
    result.reserve(entities_.size());
    for (const auto& pair : entities_) {
        result.push_back(pair.first);
    }
    return result;
}

// Entity Tensor Operations
std::vector<float> TensorLogic::computeEntitySimilarity(const std::string& entity1, const std::string& entity2) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it1 = entities_.find(entity1);
    auto it2 = entities_.find(entity2);

    if (it1 == entities_.end() || it2 == entities_.end()) {
        return {};
    }

    // Compute element-wise similarity and overall cosine similarity
    const auto& s1 = it1->second.tensor_state;
    const auto& s2 = it2->second.tensor_state;

    std::vector<float> result(s1.size() + 1);
    for (size_t i = 0; i < s1.size(); ++i) {
        result[i] = s1[i] * s2[i];
    }
    result[s1.size()] = cosineSimilarity(s1, s2);

    return result;
}

std::vector<float> TensorLogic::aggregateEntityStates(const std::vector<std::string>& entity_ids) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (entity_ids.empty()) {
        return {};
    }

    std::vector<float> result(config_.embedding_dim, 0.0f);
    size_t count = 0;

    for (const auto& id : entity_ids) {
        auto it = entities_.find(id);
        if (it != entities_.end()) {
            const auto& state = it->second.tensor_state;
            for (size_t i = 0; i < state.size() && i < result.size(); ++i) {
                result[i] += state[i] * it->second.activation_level;
            }
            ++count;
        }
    }

    if (count > 0) {
        for (auto& v : result) {
            v /= count;
        }
    }

    return normalizeVector(result);
}

bool TensorLogic::synchronizeEntities(const std::vector<std::string>& entity_ids) {
    if (entity_ids.size() < 2) {
        return false;
    }

    // Compute aggregate state
    auto aggregate = aggregateEntityStates(entity_ids);
    if (aggregate.empty()) {
        return false;
    }

    // Update each entity to move toward aggregate (with damping)
    const float damping = 0.1f;

    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& id : entity_ids) {
        auto it = entities_.find(id);
        if (it != entities_.end()) {
            auto& state = it->second.tensor_state;
            for (size_t i = 0; i < state.size(); ++i) {
                state[i] = state[i] * (1.0f - damping) + aggregate[i] * damping;
            }
        }
    }

    return true;
}

// Multi-Scale Analysis Operations
std::vector<float> TensorLogic::analyzeAtScale(const std::vector<float>& input, size_t scale_index) {
    if (multi_scale_) {
        return multi_scale_->analyzeAtScale(input, scale_index);
    }

    // Fallback: simple averaging based on scale
    size_t window_size = static_cast<size_t>(std::pow(config_.scale_factor, scale_index));
    if (window_size >= input.size()) {
        return {std::accumulate(input.begin(), input.end(), 0.0f) / input.size()};
    }

    std::vector<float> result;
    result.reserve(input.size() / window_size + 1);

    for (size_t i = 0; i + window_size <= input.size(); i += window_size) {
        float sum = 0.0f;
        for (size_t j = 0; j < window_size; ++j) {
            sum += input[i + j];
        }
        result.push_back(sum / window_size);
    }

    return result;
}

std::vector<std::vector<float>> TensorLogic::multiScaleDecomposition(const std::vector<float>& input) {
    std::vector<std::vector<float>> scales;
    scales.reserve(config_.num_scales);

    for (size_t s = 0; s < config_.num_scales; ++s) {
        scales.push_back(analyzeAtScale(input, s));
    }

    return scales;
}

std::vector<float> TensorLogic::multiScaleReconstruction(const std::vector<std::vector<float>>& scales) {
    if (scales.empty()) {
        return {};
    }

    // Use finest scale as base
    std::vector<float> result = scales[0];

    // Add contributions from coarser scales
    for (size_t s = 1; s < scales.size(); ++s) {
        const auto& scale_data = scales[s];
        size_t window_size = static_cast<size_t>(std::pow(config_.scale_factor, s));

        for (size_t i = 0; i < result.size(); ++i) {
            size_t scale_idx = std::min(i / window_size, scale_data.size() - 1);
            result[i] += scale_data[scale_idx] * (1.0f / (s + 1));
        }
    }

    return normalizeVector(result);
}

std::vector<float> TensorLogic::detectPatternAtScale(const std::vector<float>& pattern, size_t scale) {
    auto scaled_input = analyzeAtScale(pattern, scale);

    // Simple pattern detection via autocorrelation
    std::vector<float> correlations;
    correlations.reserve(scaled_input.size());

    for (size_t lag = 0; lag < scaled_input.size(); ++lag) {
        float corr = 0.0f;
        size_t count = 0;
        for (size_t i = 0; i + lag < scaled_input.size(); ++i) {
            corr += scaled_input[i] * scaled_input[i + lag];
            ++count;
        }
        if (count > 0) {
            correlations.push_back(corr / count);
        }
    }

    return correlations;
}

// Network-Aware DAS Operations
void TensorLogic::recordFlow(const FlowTensor& flow) {
    std::lock_guard<std::mutex> lock(mutex_);
    flow_history_.push_back(flow);

    // Keep history bounded
    const size_t max_history = 10000;
    if (flow_history_.size() > max_history) {
        flow_history_.erase(flow_history_.begin(), flow_history_.begin() + flow_history_.size() - max_history);
    }

    if (network_das_) {
        NetworkAwareDAS::FlowRecord flow_record{
            flow.source_node,
            flow.target_node,
            flow.flow_vector,
            flow.bandwidth,
            flow.latency,
            flow.timestamp
        };
        network_das_->recordFlow(flow_record);
    }
}

std::vector<TensorLogic::FlowTensor> TensorLogic::getFlowHistory(const std::string& node_id, size_t limit) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<FlowTensor> result;
    for (auto it = flow_history_.rbegin(); it != flow_history_.rend() && result.size() < limit; ++it) {
        if (it->source_node == node_id || it->target_node == node_id) {
            result.push_back(*it);
        }
    }

    return result;
}

std::vector<float> TensorLogic::predictOptimalRoute(const std::string& source, const std::string& target) {
    if (network_das_) {
        return network_das_->predictOptimalRoute(source, target);
    }

    // Fallback: return empty prediction
    return {};
}

double TensorLogic::estimateTransferCost(const std::string& source, const std::string& target) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Estimate based on historical flows
    double total_latency = 0.0;
    double total_bandwidth = 0.0;
    size_t count = 0;

    for (const auto& flow : flow_history_) {
        if (flow.source_node == source && flow.target_node == target) {
            total_latency += flow.latency;
            total_bandwidth += flow.bandwidth;
            ++count;
        }
    }

    if (count == 0) {
        return std::numeric_limits<double>::max(); // Unknown route
    }

    return (total_latency / count) / (total_bandwidth / count);
}

std::vector<std::string> TensorLogic::getOptimalNodePlacement(const std::vector<float>& atom_tensor) {
    if (network_das_) {
        return network_das_->getOptimalNodePlacement(atom_tensor);
    }

    // Fallback: return all available nodes
    return getAllEntities();
}

// ESN Reservoir Computing Operations
std::vector<float> TensorLogic::reservoirTransform(const std::vector<float>& input) {
    if (esn_reservoir_) {
        return esn_reservoir_->transform(input);
    }

    // Fallback: simple reservoir update
    std::vector<float> new_state(reservoir_state_.size());

    // Sparse random projection
    std::random_device rd;
    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::normal_distribution<float> dist(0.0f, config_.spectral_radius / std::sqrt(reservoir_state_.size() * config_.sparsity));
    std::bernoulli_distribution sparse_dist(config_.sparsity);

    for (size_t i = 0; i < new_state.size(); ++i) {
        float activation = 0.0f;

        // Input contribution
        for (size_t j = 0; j < input.size(); ++j) {
            if (sparse_dist(gen)) {
                activation += input[j] * dist(gen);
            }
        }

        // Recurrent contribution
        for (size_t j = 0; j < reservoir_state_.size(); ++j) {
            if (sparse_dist(gen)) {
                activation += reservoir_state_[j] * dist(gen);
            }
        }

        // Nonlinearity (tanh)
        new_state[i] = std::tanh(activation);
    }

    reservoir_state_ = new_state;
    return new_state;
}

void TensorLogic::trainReservoir(const std::vector<std::vector<float>>& inputs,
                                  const std::vector<std::vector<float>>& targets) {
    if (esn_reservoir_) {
        esn_reservoir_->train(inputs, targets);
    }
    // Training requires matrix operations - simplified here
}

std::vector<float> TensorLogic::predictSequence(const std::vector<float>& seed, size_t steps) {
    std::vector<float> result;
    result.reserve(steps * seed.size());

    auto current = seed;
    for (size_t i = 0; i < steps; ++i) {
        auto transformed = reservoirTransform(current);

        // Take first embedding_dim elements as prediction
        current.clear();
        for (size_t j = 0; j < config_.embedding_dim && j < transformed.size(); ++j) {
            current.push_back(transformed[j]);
            result.push_back(transformed[j]);
        }
    }

    return result;
}

void TensorLogic::resetReservoirState() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::fill(reservoir_state_.begin(), reservoir_state_.end(), 0.0f);
}

// Tensor-Enhanced AtomSpace Operations
std::vector<float> TensorLogic::atomToTensor(const std::string& atom_id) {
    if (tensor_atomspace_) {
        return tensor_atomspace_->atomToTensor(atom_id);
    }

    // Fallback: hash-based embedding
    std::hash<std::string> hasher;
    size_t hash = hasher(atom_id);

    std::vector<float> result(config_.embedding_dim);
    std::mt19937 gen(hash);
    std::normal_distribution<float> dist(0.0f, 1.0f);

    for (auto& v : result) {
        v = dist(gen);
    }

    return normalizeVector(result);
}

std::string TensorLogic::tensorToAtom(const std::vector<float>& tensor) {
    if (tensor_atomspace_) {
        return tensor_atomspace_->tensorToAtom(tensor);
    }

    // Fallback: create simple identifier based on tensor signature
    std::ostringstream oss;
    oss << "TensorAtom_";

    float sum = 0.0f, max_val = tensor.empty() ? 0.0f : tensor[0];
    for (float v : tensor) {
        sum += v;
        max_val = std::max(max_val, v);
    }

    oss << std::fixed << std::setprecision(4) << sum << "_" << max_val;
    return oss.str();
}

std::vector<float> TensorLogic::linkToTensor(const std::string& link_id) {
    if (tensor_atomspace_) {
        return tensor_atomspace_->linkToTensor(link_id);
    }

    // Fallback: similar to atomToTensor
    return atomToTensor(link_id);
}

std::vector<float> TensorLogic::computeTensorTruthValue(const std::vector<float>& tensor) {
    // Compute [strength, confidence] from tensor
    float strength = 0.0f;
    float magnitude = 0.0f;

    for (float v : tensor) {
        strength += v;
        magnitude += v * v;
    }

    if (!tensor.empty()) {
        strength /= tensor.size();
    }
    magnitude = std::sqrt(magnitude);

    // Map to [0, 1] range
    strength = (std::tanh(strength) + 1.0f) / 2.0f;
    float tensor_scale = tensor.empty() ? 1.0f : static_cast<float>(std::sqrt(static_cast<double>(tensor.size())));
    float confidence = std::min(1.0f, magnitude / tensor_scale);

    return {strength, confidence};
}

// Cognitive Synergy Operations
std::vector<float> TensorLogic::computeCognitiveSynergy(
    const std::vector<std::string>& entity_ids,
    const std::vector<std::vector<float>>& multi_scale_inputs) {

    // Aggregate entity states
    auto entity_aggregate = aggregateEntityStates(entity_ids);

    // Combine multi-scale information
    std::vector<float> scale_aggregate;
    if (!multi_scale_inputs.empty()) {
        scale_aggregate = multiScaleReconstruction(multi_scale_inputs);
    }

    // Combine through reservoir
    std::vector<float> combined;
    combined.reserve(entity_aggregate.size() + scale_aggregate.size());
    combined.insert(combined.end(), entity_aggregate.begin(), entity_aggregate.end());
    combined.insert(combined.end(), scale_aggregate.begin(), scale_aggregate.end());

    return reservoirTransform(combined);
}

double TensorLogic::measureSynergyStrength(const std::vector<std::string>& entity_ids) {
    if (entity_ids.size() < 2) {
        return 0.0;
    }

    double total_similarity = 0.0;
    size_t pairs = 0;

    for (size_t i = 0; i < entity_ids.size(); ++i) {
        for (size_t j = i + 1; j < entity_ids.size(); ++j) {
            auto sim = computeEntitySimilarity(entity_ids[i], entity_ids[j]);
            if (!sim.empty()) {
                total_similarity += sim.back(); // Last element is cosine similarity
                ++pairs;
            }
        }
    }

    return pairs > 0 ? total_similarity / pairs : 0.0;
}

std::vector<float> TensorLogic::propagateAttention(
    const std::vector<float>& source_attention,
    const std::vector<std::string>& target_entities) {

    std::vector<float> result(target_entities.size(), 0.0f);

    std::lock_guard<std::mutex> lock(mutex_);

    for (size_t i = 0; i < target_entities.size(); ++i) {
        auto it = entities_.find(target_entities[i]);
        if (it != entities_.end()) {
            const auto& target_state = it->second.tensor_state;

            // Compute attention-weighted similarity
            float attention_score = 0.0f;
            for (size_t j = 0; j < source_attention.size() && j < target_state.size(); ++j) {
                attention_score += source_attention[j] * target_state[j];
            }

            result[i] = std::tanh(attention_score);
        }
    }

    return normalizeVector(result);
}

// Utility methods
std::vector<float> TensorLogic::normalizeVector(const std::vector<float>& vec) {
    if (vec.empty()) return vec;

    float norm = 0.0f;
    for (float v : vec) {
        norm += v * v;
    }
    norm = std::sqrt(norm);

    if (norm < 1e-8f) return vec;

    std::vector<float> result(vec.size());
    for (size_t i = 0; i < vec.size(); ++i) {
        result[i] = vec[i] / norm;
    }
    return result;
}

float TensorLogic::cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size() || a.empty()) return 0.0f;

    float dot = 0.0f, norm_a = 0.0f, norm_b = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }

    float denom = std::sqrt(norm_a) * std::sqrt(norm_b);
    return denom > 1e-8f ? dot / denom : 0.0f;
}

std::vector<float> TensorLogic::elementwiseMultiply(const std::vector<float>& a, const std::vector<float>& b) {
    size_t size = std::min(a.size(), b.size());
    std::vector<float> result(size);
    for (size_t i = 0; i < size; ++i) {
        result[i] = a[i] * b[i];
    }
    return result;
}

std::vector<float> TensorLogic::matrixVectorMultiply(const std::vector<std::vector<float>>& matrix,
                                                      const std::vector<float>& vec) {
    if (matrix.empty() || vec.empty()) return {};

    std::vector<float> result(matrix.size(), 0.0f);
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < vec.size() && j < matrix[i].size(); ++j) {
            result[i] += matrix[i][j] * vec[j];
        }
    }
    return result;
}

// Private helper methods
void TensorLogic::initializeComponents() {
    multi_entity_ = std::make_unique<MultiEntityAtomSpace>();
    multi_scale_ = std::make_unique<MultiScaleAnalysis>(config_.num_scales, config_.scale_factor);
    network_das_ = std::make_unique<NetworkAwareDAS>();
    esn_reservoir_ = std::make_unique<ESNReservoir>(config_.reservoir_size, config_.spectral_radius, config_.sparsity);
    tensor_atomspace_ = std::make_unique<TensorAtomSpace>(config_.embedding_dim);
}

void TensorLogic::cleanupComponents() {
    tensor_atomspace_.reset();
    esn_reservoir_.reset();
    network_das_.reset();
    multi_scale_.reset();
    multi_entity_.reset();
}

std::string TensorLogic::generateEntityId() {
    static std::atomic<uint64_t> counter{0};

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()).count();

    std::ostringstream oss;
    oss << "entity_" << std::hex << timestamp << "_" << counter++;
    return oss.str();
}

} // namespace tensor_logic
} // namespace opencog
