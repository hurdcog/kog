/**
 * ActiveInferenceEngine — Implementation
 *
 * Variational free energy minimization, precision-weighted prediction
 * error, hierarchical generative model updates.
 */
#include "opencog/afi/active_inference.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace opencog::afi {

void ActiveInferenceEngine::update(float dt) noexcept {
    // 1. Compute prediction errors at each level
    if (!last_observations_.empty() && !last_predictions_.empty()) {
        size_t n = std::min(last_observations_.size(), last_predictions_.size());
        for (auto& level : model_.levels) {
            float error = 0.0f;
            size_t level_size = std::min(n, level.state_atoms.size());
            if (level_size == 0) level_size = n;

            for (size_t i = 0; i < std::min(n, level_size); ++i) {
                float diff = last_observations_[i] - last_predictions_[i];
                error += diff * diff;
            }
            if (n > 0) {
                error /= static_cast<float>(n);
            }
            level.prediction_error = std::sqrt(error);
        }
    }

    // 2. Update total prediction error
    model_.update_total_error();

    // 3. Compute free energy
    current_fe_ = compute_free_energy();

    // 4. Update precision weights
    update_precision_weights(dt);

    // 5. Update learning rates based on precision
    for (auto& level : model_.levels) {
        level.learning_rate = learning_rate_ * level.precision;
    }
}

FreeEnergy ActiveInferenceEngine::compute_free_energy() const noexcept {
    FreeEnergy fe;

    // Accuracy: precision-weighted prediction error
    fe.accuracy = model_.total_prediction_error;

    // Complexity: KL divergence approximation
    // (simplified: model depth * average prediction error)
    float complexity = 0.0f;
    for (const auto& level : model_.levels) {
        complexity += level.prediction_error * (1.0f / (level.precision + 0.01f));
    }
    if (!model_.levels.empty()) {
        complexity /= static_cast<float>(model_.levels.size());
    }
    fe.complexity = complexity;

    fe.recompute();
    return fe;
}

float ActiveInferenceEngine::expected_free_energy(
    const std::vector<float>& predicted_observations,
    const std::vector<float>& preferred_observations) const noexcept
{
    if (predicted_observations.empty() || preferred_observations.empty()) {
        return 0.0f;
    }

    size_t n = std::min(predicted_observations.size(),
                        preferred_observations.size());

    // Epistemic value: information gain (simplified as prediction uncertainty)
    float epistemic = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        float uncertainty = std::abs(predicted_observations[i] - 0.5f);
        epistemic += (0.5f - uncertainty);
    }
    epistemic /= static_cast<float>(n);

    // Pragmatic value: preference satisfaction
    float pragmatic = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        float diff = predicted_observations[i] - preferred_observations[i];
        pragmatic += diff * diff;
    }
    pragmatic /= static_cast<float>(n);

    // Expected free energy = epistemic + pragmatic
    return epistemic + pragmatic;
}

void ActiveInferenceEngine::update_precision_weights(float dt) noexcept {
    for (auto& pw : precision_weights_) {
        // Precision decays toward 1.0 (neutral)
        pw.value = pw.value * precision_decay_ + (1.0f - precision_decay_);
    }

    // Update level precisions based on prediction error history
    for (auto& level : model_.levels) {
        // Low prediction error -> high precision (confident predictions)
        float error_inverse = 1.0f / (level.prediction_error + 0.01f);
        level.precision += (error_inverse - level.precision) * 0.1f * dt;
        level.precision = std::clamp(level.precision, 0.1f, 10.0f);
    }
}

void ActiveInferenceEngine::observe(
    const std::vector<float>& observations) noexcept
{
    last_observations_ = observations;
}

std::vector<float> ActiveInferenceEngine::predict() const noexcept {
    // Generate predictions from the model (simplified: return last observations
    // adjusted by learning)
    if (last_predictions_.empty()) {
        return last_observations_;
    }
    return last_predictions_;
}

std::vector<PrecisionWeight> ActiveInferenceEngine::get_sti_adjustments() const noexcept {
    return precision_weights_;
}

void ActiveInferenceEngine::reconfigure_blanket(MarkovBlanket blanket) noexcept {
    blanket_ = std::move(blanket);
}

} // namespace opencog::afi
