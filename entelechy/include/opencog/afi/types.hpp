/**
 * Active Free-energy Inference (AFI) — Core Types
 *
 * Implements Friston's Free Energy Principle as computational types:
 *   - MarkovBlanket: self/not-self boundary
 *   - FreeEnergy: variational free energy (accuracy + complexity)
 *   - PrecisionWeight: inverse variance -> ECAN STI mapping
 *   - GenerativeModel: hierarchical prediction model
 *
 * Part of the Reactor Core: arc-vortex + arc-helix + arc-halo => cyc-phoenix
 * Copyright 2025-2026 OpenCog Community. AGPL-3.0.
 */
#ifndef OPENCOG_AFI_TYPES_HPP
#define OPENCOG_AFI_TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace opencog::afi {

/// Atom identifier (matches AtomSpace handle type)
using AtomId = uint64_t;

// =========================================================================
// Markov Blanket
// =========================================================================

/**
 * MarkovBlanket defines the boundary between self and not-self.
 * Sensory states receive from external; active states act on external;
 * internal states are hidden from external.
 */
struct MarkovBlanket {
    std::vector<AtomId> sensory_states;   // Incoming from environment
    std::vector<AtomId> active_states;    // Outgoing to environment
    std::vector<AtomId> internal_states;  // Hidden internal states
    std::vector<AtomId> external_states;  // States outside the blanket

    [[nodiscard]] size_t boundary_size() const noexcept {
        return sensory_states.size() + active_states.size();
    }

    [[nodiscard]] bool is_valid() const noexcept {
        return !sensory_states.empty() && !internal_states.empty();
    }
};

// =========================================================================
// Free Energy
// =========================================================================

/**
 * Variational Free Energy = Accuracy + Complexity
 *   accuracy  = -log p(o|s) — prediction fit (lower = better)
 *   complexity = KL[q(s)||p(s)] — model complexity cost
 *   total     = accuracy + complexity = F (to be minimized)
 */
struct alignas(8) FreeEnergy {
    float accuracy{0.0f};        // Prediction error cost
    float complexity{0.0f};      // Model complexity cost
    float total{0.0f};           // accuracy + complexity = F

    [[nodiscard]] float surprise() const noexcept { return total; }

    void recompute() noexcept { total = accuracy + complexity; }
};

// =========================================================================
// Precision Weight
// =========================================================================

/**
 * Precision = inverse variance of a signal.
 * High precision = high confidence = allocate more attention (ECAN STI).
 */
struct PrecisionWeight {
    float value{1.0f};           // [0, inf): inverse variance
    AtomId target{0};            // Which atom this precision applies to

    /// Maps precision to ECAN STI adjustment
    [[nodiscard]] float to_sti_adjustment(float scale = 0.1f) const noexcept {
        return (value - 1.0f) * scale;
    }
};

// =========================================================================
// Generative Model
// =========================================================================

/**
 * Hierarchical generative model that predicts sensory input.
 * Each level predicts the level below, creating a prediction hierarchy.
 */
struct GenerativeModelLevel {
    std::string name;
    std::vector<AtomId> state_atoms;     // Internal state representation
    float prediction_error{0.0f};        // Current prediction error
    float learning_rate{0.01f};          // How fast this level adapts
    float precision{1.0f};               // Confidence in predictions
};

struct GenerativeModel {
    std::string name;
    std::vector<GenerativeModelLevel> levels;
    float total_prediction_error{0.0f};
    float model_evidence{0.0f};          // -log p(o|m) marginal likelihood

    [[nodiscard]] size_t depth() const noexcept { return levels.size(); }

    void update_total_error() noexcept {
        total_prediction_error = 0.0f;
        for (const auto& level : levels) {
            total_prediction_error += level.prediction_error * level.precision;
        }
    }
};

// =========================================================================
// District Metrics (per-district free energy summary)
// =========================================================================

struct DistrictMetrics {
    float free_energy{0.0f};      // Current variational free energy
    float surprise{0.0f};         // Bayesian surprise (KL divergence)
    float complexity{0.0f};       // Model complexity cost
    float accuracy{0.0f};         // Prediction accuracy
    float coherence{0.0f};        // Internal consistency
};

} // namespace opencog::afi

#endif // OPENCOG_AFI_TYPES_HPP
