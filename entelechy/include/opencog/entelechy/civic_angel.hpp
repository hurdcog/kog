/**
 * CivicAngel — The emergent governor of the Cognitive City
 *
 * The Civic Angel is the self-model that observes all districts,
 * maintains inter-district coherence, allocates resources, and
 * tracks entelechy progress (actualization toward potential).
 *
 * Implements:
 *   - Self-model (3rd-person view of own architecture)
 *   - City-wide free energy aggregation
 *   - Inter-district coherence monitoring
 *   - Resource (STI) reallocation across districts
 *   - Entelechy progress tracking
 *   - Guidance request generation
 *
 * Part of the Reactor Core: arc-halo => cyc-phoenix
 * Copyright 2025-2026 OpenCog Community. AGPL-3.0.
 */
#ifndef OPENCOG_ENTELECHY_CIVIC_ANGEL_HPP
#define OPENCOG_ENTELECHY_CIVIC_ANGEL_HPP

#include "opencog/entelechy/types.hpp"
#include "opencog/entelechy/cloninger.hpp"
#include "opencog/entelechy/interoceptive.hpp"
#include "opencog/entelechy/developmental.hpp"
#include "opencog/entelechy/narrative.hpp"
#include "opencog/entelechy/social.hpp"
#include "opencog/afi/district.hpp"
#include <array>
#include <memory>

namespace opencog::entelechy {

class CivicAngel {
public:
    CivicAngel();

    /// Full tick: update all subsystems and districts
    void tick(float dt = 1.0f) noexcept;

    /// Phase 0: Apply Cloninger gains (before gland production)
    [[nodiscard]] GainProfile compute_temperament_gains() const noexcept;

    /// Phase 1.5: Update interoceptive state from hormones
    void update_interoceptive(const float* hormone_channels) noexcept;

    /// Phase 5.5: Post-valence developmental + narrative + social update
    void update_identity(const FeltSense& felt_sense, uint64_t current_tick) noexcept;

    /// Phase 6: City-wide coherence assessment
    void assess_city_coherence() noexcept;

    /// Phase 7: Entelechy progress
    void update_entelechy_progress() noexcept;

    /// State accessors
    [[nodiscard]] const CivicAngelState& state() const noexcept { return state_; }
    [[nodiscard]] CivicAngelState& state_mut() noexcept { return state_; }

    /// Subsystem accessors
    [[nodiscard]] CloningerSystem& cloninger() noexcept { return cloninger_; }
    [[nodiscard]] const CloningerSystem& cloninger() const noexcept { return cloninger_; }
    [[nodiscard]] InteroceptiveModel& interoceptive() noexcept { return interoceptive_; }
    [[nodiscard]] const InteroceptiveModel& interoceptive() const noexcept { return interoceptive_; }
    [[nodiscard]] DevelopmentalTrajectory& developmental() noexcept { return developmental_; }
    [[nodiscard]] const DevelopmentalTrajectory& developmental() const noexcept { return developmental_; }
    [[nodiscard]] NarrativeIdentity& narrative() noexcept { return narrative_; }
    [[nodiscard]] const NarrativeIdentity& narrative() const noexcept { return narrative_; }
    [[nodiscard]] SocialSelf& social() noexcept { return social_; }
    [[nodiscard]] const SocialSelf& social() const noexcept { return social_; }

    /// District access
    [[nodiscard]] afi::CognitiveDistrict& district(afi::DistrictId id) noexcept;
    [[nodiscard]] const afi::CognitiveDistrict& district(afi::DistrictId id) const noexcept;

    /// Check if guidance should be requested
    [[nodiscard]] bool should_request_guidance() const noexcept;
    [[nodiscard]] EntelechyGuidanceReason guidance_reason() const noexcept;

    /// Write interoceptive channels to hormone bus
    void write_interoceptive_channels(float* channels) const noexcept;

private:
    CivicAngelState state_;

    // Core subsystems
    CloningerSystem cloninger_;
    InteroceptiveModel interoceptive_;
    DevelopmentalTrajectory developmental_;
    NarrativeIdentity narrative_;
    SocialSelf social_;

    // Districts (one per cognitive subsystem)
    std::array<afi::CognitiveDistrict,
               static_cast<size_t>(afi::DistrictId::COUNT)> districts_;

    // Thresholds
    float coherence_alarm_threshold_{0.3f};
    float free_energy_alarm_threshold_{2.0f};
    float entelechy_milestone_threshold_{0.1f};
    float last_entelechy_milestone_{0.0f};

    void initialize_districts() noexcept;
    void compute_self_model() noexcept;
};

} // namespace opencog::entelechy

#endif // OPENCOG_ENTELECHY_CIVIC_ANGEL_HPP
