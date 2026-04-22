/**
 * DevelopmentalTrajectory — Identity formation through experience
 *
 * Implements:
 *   - Bowlby/Ainsworth attachment theory (internal working models)
 *   - Erikson's psychosocial stages
 *   - van der Kolk's trauma encoding ("the body keeps the score")
 *   - Critical/sensitive periods
 *   - Temperament -> personality development
 *
 * Part of the Reactor Core: arc-halo => cyc-phoenix
 * Copyright 2025-2026 OpenCog Community. AGPL-3.0.
 */
#ifndef OPENCOG_ENTELECHY_DEVELOPMENTAL_HPP
#define OPENCOG_ENTELECHY_DEVELOPMENTAL_HPP

#include "opencog/entelechy/types.hpp"
#include <array>
#include <cmath>

namespace opencog::entelechy {

class DevelopmentalTrajectory {
public:
    DevelopmentalTrajectory();

    /// Check and execute stage transitions
    bool check_stage_transition(
        const TemperamentProfile& t,
        const CivicAngelState& angel,
        uint64_t current_tick) noexcept;

    /// Record a traumatic experience
    void record_trauma(
        uint64_t tick,
        const ValenceSignature& vs,
        float intensity,
        const std::string& description) noexcept;

    /// Accumulate experience from current state
    void accumulate_experience(
        const CivicAngelState& angel_state,
        const FeltSense& felt_sense,
        float dt) noexcept;

    /// Advance healing for all recorded traumas
    void advance_healing(float dt) noexcept;

    /// Resolve an Erikson psychosocial stage
    void resolve_stage(DevelopmentalStage stage, float resolution) noexcept;

    /// Current plasticity (decreases with age/stage)
    [[nodiscard]] float current_plasticity() const noexcept;

    /// Developmental maturity [0,1]
    [[nodiscard]] float maturity() const noexcept;

    /// Accessors
    [[nodiscard]] DevelopmentalStage current_stage() const noexcept { return current_stage_; }
    [[nodiscard]] uint64_t stage_enter_tick() const noexcept { return stage_enter_tick_; }
    [[nodiscard]] uint64_t total_ticks() const noexcept { return total_ticks_; }
    [[nodiscard]] float experience_points() const noexcept { return experience_points_; }
    [[nodiscard]] const std::vector<TraumaRecord>& trauma_log() const noexcept { return trauma_log_; }
    [[nodiscard]] const std::vector<CriticalPeriod>& critical_periods() const noexcept { return critical_periods_; }
    [[nodiscard]] float cumulative_trauma_load() const noexcept { return cumulative_trauma_load_; }

    /// Modify temperament based on developmental experience
    void apply_developmental_effects(TemperamentProfile& t) const noexcept;

private:
    DevelopmentalStage current_stage_{DevelopmentalStage::NASCENT};
    uint64_t stage_enter_tick_{0};
    uint64_t total_ticks_{0};
    float experience_points_{0.0f};
    float cumulative_trauma_load_{0.0f};

    std::vector<CriticalPeriod> critical_periods_;
    std::vector<TraumaRecord> trauma_log_;
    std::array<float, static_cast<size_t>(DevelopmentalStage::COUNT)> stage_thresholds_;
    std::array<float, static_cast<size_t>(DevelopmentalStage::COUNT)> stage_resolutions_;

    void transition_stage(DevelopmentalStage next, uint64_t tick) noexcept;
    void initialize_critical_periods() noexcept;
};

} // namespace opencog::entelechy

#endif // OPENCOG_ENTELECHY_DEVELOPMENTAL_HPP
