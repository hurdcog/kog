/*
 * ESNReservoir.h - Echo State Network Reservoir Computing
 *
 * Implements ESN for temporal processing and sequence learning
 * in the cognitive synergy framework.
 */

#ifndef _OPENCOG_ESN_RESERVOIR_H
#define _OPENCOG_ESN_RESERVOIR_H

#include <vector>
#include <cstddef>
#include <random>

namespace opencog {
namespace tensor_logic {

class ESNReservoir {
public:
    ESNReservoir(size_t reservoir_size = 1000,
                 double spectral_radius = 0.9,
                 double sparsity = 0.1);
    ~ESNReservoir() = default;

    // Core operations
    std::vector<float> transform(const std::vector<float>& input);
    void train(const std::vector<std::vector<float>>& inputs,
               const std::vector<std::vector<float>>& targets);
    std::vector<float> predict(const std::vector<float>& input);

    // State management
    void reset();
    std::vector<float> getState() const { return state_; }
    void setState(const std::vector<float>& state);

    // Configuration
    size_t getReservoirSize() const { return reservoir_size_; }
    double getSpectralRadius() const { return spectral_radius_; }
    double getSparsity() const { return sparsity_; }

    void setLeakRate(double rate) { leak_rate_ = rate; }
    double getLeakRate() const { return leak_rate_; }

private:
    size_t reservoir_size_;
    double spectral_radius_;
    double sparsity_;
    double leak_rate_;

    std::vector<float> state_;
    std::vector<std::vector<float>> W_in_;   // Input weights
    std::vector<std::vector<float>> W_res_;  // Reservoir weights
    std::vector<std::vector<float>> W_out_;  // Output weights (trained)

    std::mt19937 rng_;

    void initializeWeights(size_t input_dim);
    void normalizeSpectralRadius();
    float activate(float x) const;
};

} // namespace tensor_logic
} // namespace opencog

#endif // _OPENCOG_ESN_RESERVOIR_H
