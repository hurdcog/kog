/**
 * ActiveInferenceEngine — Friston's Free Energy Principle
 *
 * Implements:
 *   - Variational free energy minimization
 *   - Markov blanket boundary management
 *   - Precision-weighted prediction error
 *   - Hierarchical generative model
 *   - Expected free energy for action selection
 *   - ECAN STI mapping (precision -> attention)
 *
 * Part of the Reactor Core: arc-halo => cyc-phoenix
 * Copyright 2025-2026 OpenCog Community. AGPL-3.0.
 */
#ifndef OPENCOG_AFI_ACTIVE_INFERENCE_HPP
#define OPENCOG_AFI_ACTIVE_INFERENCE_HPP

#include "opencog/afi/types.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

namespace opencog::afi {

class ActiveInferenceEngine {
public:
    ActiveInferenceEngine() = default;

    /// Initialize with a generative model
    explicit ActiveInferenceEngine(GenerativeModel model)
        : model_(std::move(model)) {}

    /// Core update: compute free energy from prediction errors
    void update(float dt) noexcept;

    /// Compute variational free energy for current state
    [[nodiscard]] FreeEnergy compute_free_energy() const noexcept;

    /// Compute expected free energy for an action policy
    [[nodiscard]] float expected_free_energy(
        const std::vector<float>& predicted_observations,
        const std::vector<float>& preferred_observations) const noexcept;

    /// Update precision weights based on prediction history
    void update_precision_weights(float dt) noexcept;

    /// Feed sensory observations into the model
    void observe(const std::vector<float>& observations) noexcept;

    /// Generate predictions from the model
    [[nodiscard]] std::vector<float> predict() const noexcept;

    /// Get precision-weighted STI adjustments for ECAN
    [[nodiscard]] std::vector<PrecisionWeight> get_sti_adjustments() const noexcept;

    /// Accessors
    [[nodiscard]] const GenerativeModel& model() const noexcept { return model_; }
    [[nodiscard]] GenerativeModel& model_mut() noexcept { return model_; }
    [[nodiscard]] const FreeEnergy& current_free_energy() const noexcept { return current_fe_; }
    [[nodiscard]] const MarkovBlanket& blanket() const noexcept { return blanket_; }
    [[nodiscard]] MarkovBlanket& blanket_mut() noexcept { return blanket_; }

    /// Reconfigure the Markov blanket boundary
    void reconfigure_blanket(MarkovBlanket blanket) noexcept;

private:
    GenerativeModel model_;
    MarkovBlanket blanket_;
    FreeEnergy current_fe_;
    std::vector<float> last_observations_;
    std::vector<float> last_predictions_;
    std::vector<PrecisionWeight> precision_weights_;
    float learning_rate_{0.01f};
    float precision_decay_{0.99f};
};

} // namespace opencog::afi

#endif // OPENCOG_AFI_ACTIVE_INFERENCE_HPP
