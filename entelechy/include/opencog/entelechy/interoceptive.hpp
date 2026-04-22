/**
 * InteroceptiveModel — Full body-state mapping with Polyvagal hierarchy
 *
 * Implements Craig's interoceptive re-representation model and
 * Porges' Polyvagal Theory as a computational system:
 *   - 12-channel interoceptive state (ch20-31)
 *   - Three-tier polyvagal hierarchy (ventral/sympathetic/dorsal)
 *   - Allostatic load accumulation (McEwen)
 *   - Window of tolerance (Siegel)
 *   - Neuroception (pre-conscious threat/safety evaluation)
 *
 * Part of the Reactor Core: arc-halo => cyc-phoenix
 * Copyright 2025-2026 OpenCog Community. AGPL-3.0.
 */
#ifndef OPENCOG_ENTELECHY_INTEROCEPTIVE_HPP
#define OPENCOG_ENTELECHY_INTEROCEPTIVE_HPP

#include "opencog/entelechy/types.hpp"
#include <algorithm>
#include <cmath>

namespace opencog::entelechy {

class InteroceptiveModel {
public:
    InteroceptiveModel() { initialize_defaults(); }

    /// Initialize with biologically plausible defaults
    void initialize_defaults() noexcept;

    /// Update interoceptive state from hormone concentrations (ch0-19)
    /// and produce interoceptive signals on ch20-31
    void update(const float* hormone_concentrations, float dt) noexcept;

    /// Evaluate polyvagal hierarchy state
    [[nodiscard]] PolyvagalState evaluate_polyvagal() const noexcept;

    /// Neuroception: pre-conscious threat/safety evaluation
    [[nodiscard]] float neuroception_safety() const noexcept;
    [[nodiscard]] float neuroception_threat() const noexcept;

    /// Window of tolerance check (Siegel)
    [[nodiscard]] bool within_window_of_tolerance() const noexcept;

    /// Accessors
    [[nodiscard]] const InteroceptiveState& state() const noexcept { return state_; }
    [[nodiscard]] InteroceptiveState& state_mut() noexcept { return state_; }
    [[nodiscard]] PolyvagalState polyvagal_state() const noexcept { return polyvagal_; }
    [[nodiscard]] float current_arousal() const noexcept { return arousal_; }

    /// Write interoceptive state to hormone channels 20-31
    void write_to_channels(float* channels) const noexcept;

private:
    InteroceptiveState state_;
    PolyvagalState polyvagal_{PolyvagalState::VENTRAL_VAGAL};
    float arousal_{0.5f};
    float window_upper_{0.8f};
    float window_lower_{0.2f};
};

} // namespace opencog::entelechy

#endif // OPENCOG_ENTELECHY_INTEROCEPTIVE_HPP
