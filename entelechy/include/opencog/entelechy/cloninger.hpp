/**
 * CloningerSystem — 7-dimensional temperament/character gain parameters
 *
 * Applies multiplicative gain to hormone channel sensitivities based on
 * Cloninger's Psychobiological Model:
 *   Harm Avoidance   <-> serotonin/cortisol
 *   Novelty Seeking   <-> dopamine
 *   Reward Dependence <-> norepinephrine/oxytocin
 *   Persistence       <-> DA-tonic sustain
 *   Self-Directedness <-> autonomy/executive
 *   Cooperativeness   <-> oxytocin/social
 *   Self-Transcendence<-> anandamide/integration
 *
 * MUST run BEFORE glands produce (Phase 0 of tick pipeline).
 *
 * Part of the Reactor Core: arc-halo => cyc-phoenix
 * Copyright 2025-2026 OpenCog Community. AGPL-3.0.
 */
#ifndef OPENCOG_ENTELECHY_CLONINGER_HPP
#define OPENCOG_ENTELECHY_CLONINGER_HPP

#include "opencog/entelechy/types.hpp"
#include <algorithm>
#include <cmath>

namespace opencog::entelechy {

class CloningerSystem {
public:
    CloningerSystem() = default;
    explicit CloningerSystem(const TemperamentProfile& profile)
        : profile_(profile) {}

    /// Compute per-channel gain multipliers from temperament profile
    [[nodiscard]] GainProfile compute_gains() const noexcept {
        return compute_gains(profile_);
    }

    [[nodiscard]] static GainProfile compute_gains(
        const TemperamentProfile& t) noexcept
    {
        GainProfile g;

        // Harm Avoidance -> cortisol/serotonin sensitivity
        // High HA = amplified stress response, amplified serotonin calming
        g.cortisol_gain  = clamp_gain(1.0f + (t.harm_avoidance - 0.5f) * 0.8f);
        g.crh_gain       = clamp_gain(1.0f + (t.harm_avoidance - 0.5f) * 0.6f);
        g.acth_gain      = clamp_gain(1.0f + (t.harm_avoidance - 0.5f) * 0.4f);
        g.serotonin_gain = clamp_gain(1.0f + (t.harm_avoidance - 0.5f) * 0.6f);

        // Novelty Seeking -> dopamine sensitivity
        // High NS = amplified DA response to novelty
        g.da_tonic_gain  = clamp_gain(1.0f + (t.novelty_seeking - 0.5f) * 0.8f);
        g.da_phasic_gain = clamp_gain(1.0f + (t.novelty_seeking - 0.5f) * 1.0f);

        // Reward Dependence -> NE/oxytocin sensitivity
        // High RD = amplified social/reward signals
        g.norepinephrine_gain = clamp_gain(1.0f + (t.reward_dependence - 0.5f) * 0.6f);
        g.oxytocin_gain = clamp_gain(1.0f + (t.reward_dependence - 0.5f) * 0.8f);

        // Persistence -> DA-tonic sustain (longer tonic DA)
        g.da_tonic_gain *= clamp_gain(1.0f + (t.persistence - 0.5f) * 0.4f);

        // Self-Directedness -> T3/T4 (processing rate / executive function)
        g.t3t4_gain = clamp_gain(1.0f + (t.self_directedness - 0.5f) * 0.6f);

        // Cooperativeness -> oxytocin (social bonding)
        g.oxytocin_gain *= clamp_gain(1.0f + (t.cooperativeness - 0.5f) * 0.4f);

        // Self-Transcendence -> anandamide (integration/noise reduction)
        g.anandamide_gain = clamp_gain(1.0f + (t.self_transcendence - 0.5f) * 0.8f);

        // Metabolic channels less affected by temperament
        g.melatonin_gain = 1.0f;
        g.insulin_gain   = 1.0f;
        g.glucagon_gain  = 1.0f;
        g.il6_gain       = clamp_gain(1.0f + t.allostatic_load * 0.1f);

        return g;
    }

    /// Update the temperament profile
    void set_profile(const TemperamentProfile& p) noexcept { profile_ = p; }
    [[nodiscard]] const TemperamentProfile& profile() const noexcept { return profile_; }
    [[nodiscard]] TemperamentProfile& profile_mut() noexcept { return profile_; }

private:
    TemperamentProfile profile_;

    static float clamp_gain(float v) noexcept {
        return std::clamp(v, 0.5f, 2.0f);
    }
};

} // namespace opencog::entelechy

#endif // OPENCOG_ENTELECHY_CLONINGER_HPP
