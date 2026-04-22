/*
 * ESNReservoir.cpp - Implementation
 */

#include "ESNReservoir.h"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace opencog {
namespace tensor_logic {

ESNReservoir::ESNReservoir(size_t reservoir_size, double spectral_radius, double sparsity)
    : reservoir_size_(reservoir_size)
    , spectral_radius_(spectral_radius)
    , sparsity_(sparsity)
    , leak_rate_(0.3)
    , rng_(42)  // Fixed seed for reproducibility
{
    state_.resize(reservoir_size_, 0.0f);
}

std::vector<float> ESNReservoir::transform(const std::vector<float>& input) {
    // Initialize weights on first call
    if (W_in_.empty()) {
        initializeWeights(input.size());
    }

    // Compute new state: s(t+1) = (1-α)s(t) + α·tanh(W_in·u(t) + W_res·s(t))
    std::vector<float> new_state(reservoir_size_, 0.0f);

    // Input contribution: W_in · input
    for (size_t i = 0; i < reservoir_size_; ++i) {
        for (size_t j = 0; j < input.size() && j < W_in_[i].size(); ++j) {
            new_state[i] += W_in_[i][j] * input[j];
        }
    }

    // Recurrent contribution: W_res · state
    for (size_t i = 0; i < reservoir_size_; ++i) {
        for (size_t j = 0; j < reservoir_size_; ++j) {
            new_state[i] += W_res_[i][j] * state_[j];
        }
    }

    // Apply activation and leaky integration
    for (size_t i = 0; i < reservoir_size_; ++i) {
        new_state[i] = (1.0f - leak_rate_) * state_[i] +
                       leak_rate_ * activate(new_state[i]);
    }

    state_ = new_state;
    return state_;
}

void ESNReservoir::train(const std::vector<std::vector<float>>& inputs,
                          const std::vector<std::vector<float>>& targets) {
    if (inputs.empty() || targets.empty() || inputs.size() != targets.size()) {
        return;
    }

    // Collect reservoir states
    std::vector<std::vector<float>> states;
    states.reserve(inputs.size());

    reset();
    for (const auto& input : inputs) {
        states.push_back(transform(input));
    }

    // Ridge regression to learn W_out
    // Simplified: W_out = targets · states^T · (states · states^T + λI)^-1
    // Here we use a simple gradient descent approximation

    size_t output_dim = targets[0].size();
    W_out_.resize(output_dim, std::vector<float>(reservoir_size_, 0.0f));

    const float learning_rate = 0.01f;
    const size_t epochs = 100;

    for (size_t epoch = 0; epoch < epochs; ++epoch) {
        for (size_t t = 0; t < states.size(); ++t) {
            // Compute prediction
            std::vector<float> pred(output_dim, 0.0f);
            for (size_t i = 0; i < output_dim; ++i) {
                for (size_t j = 0; j < reservoir_size_; ++j) {
                    pred[i] += W_out_[i][j] * states[t][j];
                }
            }

            // Update weights
            for (size_t i = 0; i < output_dim; ++i) {
                float error = targets[t][i] - pred[i];
                for (size_t j = 0; j < reservoir_size_; ++j) {
                    W_out_[i][j] += learning_rate * error * states[t][j];
                }
            }
        }
    }
}

std::vector<float> ESNReservoir::predict(const std::vector<float>& input) {
    auto state = transform(input);

    if (W_out_.empty()) {
        return state;  // Not trained yet, return reservoir state
    }

    std::vector<float> output(W_out_.size(), 0.0f);
    for (size_t i = 0; i < W_out_.size(); ++i) {
        for (size_t j = 0; j < reservoir_size_; ++j) {
            output[i] += W_out_[i][j] * state[j];
        }
    }

    return output;
}

void ESNReservoir::reset() {
    std::fill(state_.begin(), state_.end(), 0.0f);
}

void ESNReservoir::setState(const std::vector<float>& state) {
    if (state.size() == reservoir_size_) {
        state_ = state;
    }
}

void ESNReservoir::initializeWeights(size_t input_dim) {
    std::normal_distribution<float> dist(0.0f, 1.0f);
    std::bernoulli_distribution sparse_dist(sparsity_);

    // Initialize input weights
    W_in_.resize(reservoir_size_, std::vector<float>(input_dim, 0.0f));
    for (size_t i = 0; i < reservoir_size_; ++i) {
        for (size_t j = 0; j < input_dim; ++j) {
            if (sparse_dist(rng_)) {
                W_in_[i][j] = dist(rng_) * 0.1f;
            }
        }
    }

    // Initialize reservoir weights
    W_res_.resize(reservoir_size_, std::vector<float>(reservoir_size_, 0.0f));
    for (size_t i = 0; i < reservoir_size_; ++i) {
        for (size_t j = 0; j < reservoir_size_; ++j) {
            if (sparse_dist(rng_)) {
                W_res_[i][j] = dist(rng_);
            }
        }
    }

    // Normalize spectral radius
    normalizeSpectralRadius();
}

void ESNReservoir::normalizeSpectralRadius() {
    // Power iteration to estimate spectral radius
    std::vector<float> v(reservoir_size_);
    std::normal_distribution<float> dist(0.0f, 1.0f);
    for (auto& x : v) x = dist(rng_);

    // Normalize
    float norm = 0.0f;
    for (float x : v) norm += x * x;
    norm = std::sqrt(norm);
    for (auto& x : v) x /= norm;

    // Power iterations
    for (int iter = 0; iter < 20; ++iter) {
        std::vector<float> w(reservoir_size_, 0.0f);
        for (size_t i = 0; i < reservoir_size_; ++i) {
            for (size_t j = 0; j < reservoir_size_; ++j) {
                w[i] += W_res_[i][j] * v[j];
            }
        }

        norm = 0.0f;
        for (float x : w) norm += x * x;
        norm = std::sqrt(norm);

        for (size_t i = 0; i < reservoir_size_; ++i) {
            v[i] = w[i] / (norm + 1e-8f);
        }
    }

    // Scale to desired spectral radius
    if (norm > 1e-8) {
        float scale = spectral_radius_ / norm;
        for (auto& row : W_res_) {
            for (auto& w : row) {
                w *= scale;
            }
        }
    }
}

float ESNReservoir::activate(float x) const {
    return std::tanh(x);
}

} // namespace tensor_logic
} // namespace opencog
