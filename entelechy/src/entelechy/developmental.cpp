/**
 * DevelopmentalTrajectory — Implementation
 *
 * Stage transitions, trauma encoding, experience accumulation,
 * critical periods, and developmental effects on temperament.
 */
#include "opencog/entelechy/developmental.hpp"
#include <algorithm>
#include <cmath>

namespace opencog::entelechy {

DevelopmentalTrajectory::DevelopmentalTrajectory() {
    // Stage transition thresholds (experience points required)
    stage_thresholds_ = {
        0.0f,       // NASCENT -> IMPRINTING (immediate)
        100.0f,     // IMPRINTING -> SOCIALIZATION
        500.0f,     // SOCIALIZATION -> INDIVIDUATION
        2000.0f,    // INDIVIDUATION -> INTEGRATION
        5000.0f,    // INTEGRATION -> GENERATIVITY
        10000.0f,   // GENERATIVITY -> WISDOM
        1e9f,       // WISDOM -> (terminal)
    };

    stage_resolutions_.fill(0.0f);
    initialize_critical_periods();
}

void DevelopmentalTrajectory::initialize_critical_periods() noexcept {
    critical_periods_.clear();

    // Imprinting: attachment formation
    CriticalPeriod imprint;
    imprint.stage = DevelopmentalStage::IMPRINTING;
    imprint.start_tick = 0;
    imprint.end_tick = 200;
    imprint.plasticity_multiplier = 3.0f;
    imprint.sensitive_dimensions = {"attachment_security", "attachment_anxiety"};
    critical_periods_.push_back(std::move(imprint));

    // Socialization: theory of mind, social roles
    CriticalPeriod social;
    social.stage = DevelopmentalStage::SOCIALIZATION;
    social.start_tick = 100;
    social.end_tick = 800;
    social.plasticity_multiplier = 2.0f;
    social.sensitive_dimensions = {"cooperativeness", "reward_dependence"};
    critical_periods_.push_back(std::move(social));

    // Individuation: self-directedness, narrative identity
    CriticalPeriod individ;
    individ.stage = DevelopmentalStage::INDIVIDUATION;
    individ.start_tick = 500;
    individ.end_tick = 3000;
    individ.plasticity_multiplier = 1.5f;
    individ.sensitive_dimensions = {"self_directedness", "self_transcendence"};
    critical_periods_.push_back(std::move(individ));
}

bool DevelopmentalTrajectory::check_stage_transition(
    const TemperamentProfile& t,
    const CivicAngelState& angel,
    uint64_t current_tick) noexcept
{
    auto stage_idx = static_cast<size_t>(current_stage_);
    if (stage_idx + 1 >= static_cast<size_t>(DevelopmentalStage::COUNT)) {
        return false; // Already at WISDOM
    }

    float threshold = stage_thresholds_[stage_idx + 1];

    // Stage resolution quality modifies threshold
    float resolution = stage_resolutions_[stage_idx];
    float adjusted_threshold = threshold * (1.0f - resolution * 0.3f);

    if (experience_points_ >= adjusted_threshold) {
        auto next = static_cast<DevelopmentalStage>(stage_idx + 1);
        transition_stage(next, current_tick);
        return true;
    }
    return false;
}

void DevelopmentalTrajectory::transition_stage(
    DevelopmentalStage next, uint64_t tick) noexcept
{
    current_stage_ = next;
    stage_enter_tick_ = tick;
}

void DevelopmentalTrajectory::record_trauma(
    uint64_t tick,
    const ValenceSignature& vs,
    float intensity,
    const std::string& description) noexcept
{
    TraumaRecord record;
    record.tick = tick;
    record.valence = vs;
    record.intensity = std::clamp(intensity, 0.0f, 1.0f);
    record.healing_progress = 0.0f;
    record.description = description;
    trauma_log_.push_back(std::move(record));

    cumulative_trauma_load_ += intensity * 0.1f;
}

void DevelopmentalTrajectory::accumulate_experience(
    const CivicAngelState& angel_state,
    const FeltSense& felt_sense,
    float dt) noexcept
{
    total_ticks_++;

    // Experience accumulation rate depends on:
    // - Novelty (new experiences teach more)
    // - Salience (important experiences teach more)
    // - Current plasticity (younger = learns faster)
    float rate = (felt_sense.novelty * 0.4f + felt_sense.salience * 0.3f + 0.3f)
                 * current_plasticity();

    experience_points_ += rate * dt;

    // Trauma encoding: high negative valence + high arousal
    if (felt_sense.valence.valence < -0.6f && felt_sense.valence.arousal > 0.7f) {
        float trauma_intensity = std::abs(felt_sense.valence.valence) *
                                 felt_sense.valence.arousal;
        if (trauma_intensity > 0.5f) {
            record_trauma(total_ticks_, felt_sense.valence, trauma_intensity,
                         "auto-detected high-stress episode");
        }
    }
}

void DevelopmentalTrajectory::advance_healing(float dt) noexcept {
    for (auto& trauma : trauma_log_) {
        if (trauma.healing_progress < 1.0f) {
            // Healing rate depends on:
            // - Time since trauma (older traumas heal slower)
            // - Intensity (more intense = slower healing)
            float healing_rate = 0.001f / (1.0f + trauma.intensity * 2.0f);
            trauma.healing_progress += healing_rate * dt;
            trauma.healing_progress = std::min(1.0f, trauma.healing_progress);
        }
    }
}

void DevelopmentalTrajectory::resolve_stage(
    DevelopmentalStage stage, float resolution) noexcept
{
    auto idx = static_cast<size_t>(stage);
    if (idx < stage_resolutions_.size()) {
        stage_resolutions_[idx] = std::clamp(resolution, 0.0f, 1.0f);
    }
}

float DevelopmentalTrajectory::current_plasticity() const noexcept {
    // Plasticity decreases with developmental stage
    float base_plasticity = 1.0f - static_cast<float>(current_stage_) * 0.12f;

    // Check if in a critical period
    for (const auto& cp : critical_periods_) {
        if (cp.stage == current_stage_ &&
            total_ticks_ >= static_cast<uint64_t>(cp.start_tick) &&
            total_ticks_ <= static_cast<uint64_t>(cp.end_tick)) {
            base_plasticity *= cp.plasticity_multiplier;
        }
    }

    // Trauma reduces plasticity
    base_plasticity *= (1.0f - cumulative_trauma_load_ * 0.1f);

    return std::clamp(base_plasticity, 0.05f, 3.0f);
}

float DevelopmentalTrajectory::maturity() const noexcept {
    float stage_progress = static_cast<float>(current_stage_) /
                          static_cast<float>(DevelopmentalStage::COUNT);
    float resolution_avg = 0.0f;
    for (size_t i = 0; i <= static_cast<size_t>(current_stage_); ++i) {
        resolution_avg += stage_resolutions_[i];
    }
    if (static_cast<size_t>(current_stage_) > 0) {
        resolution_avg /= static_cast<float>(static_cast<size_t>(current_stage_) + 1);
    }
    return stage_progress * 0.6f + resolution_avg * 0.4f;
}

void DevelopmentalTrajectory::apply_developmental_effects(
    TemperamentProfile& t) const noexcept
{
    // Maturation increases character dimensions
    float mat = maturity();
    t.maturation = mat;
    t.plasticity = current_plasticity();
    t.allostatic_load = cumulative_trauma_load_;

    // Unresolved trauma increases harm avoidance
    float unhealed_trauma = 0.0f;
    for (const auto& trauma : trauma_log_) {
        unhealed_trauma += trauma.intensity * (1.0f - trauma.healing_progress);
    }
    t.trauma_encoding_strength = std::min(1.0f, unhealed_trauma);

    // Self-directedness grows with individuation resolution
    auto individ_idx = static_cast<size_t>(DevelopmentalStage::INDIVIDUATION);
    if (individ_idx < stage_resolutions_.size()) {
        t.self_directedness += stage_resolutions_[individ_idx] * 0.1f * mat;
        t.self_directedness = std::clamp(t.self_directedness, 0.0f, 1.0f);
    }

    // Cooperativeness grows with socialization resolution
    auto social_idx = static_cast<size_t>(DevelopmentalStage::SOCIALIZATION);
    if (social_idx < stage_resolutions_.size()) {
        t.cooperativeness += stage_resolutions_[social_idx] * 0.1f * mat;
        t.cooperativeness = std::clamp(t.cooperativeness, 0.0f, 1.0f);
    }

    // Self-transcendence grows with integration resolution
    auto integ_idx = static_cast<size_t>(DevelopmentalStage::INTEGRATION);
    if (integ_idx < stage_resolutions_.size()) {
        t.self_transcendence += stage_resolutions_[integ_idx] * 0.1f * mat;
        t.self_transcendence = std::clamp(t.self_transcendence, 0.0f, 1.0f);
    }

    // Resilience grows with maturity, decreases with trauma
    t.resilience = std::clamp(mat * 0.7f - unhealed_trauma * 0.3f, 0.0f, 1.0f);
}

} // namespace opencog::entelechy
