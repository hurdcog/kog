/**
 * CivicAngel — Implementation
 *
 * The emergent governor of the Cognitive City. Orchestrates all
 * subsystems (Cloninger, interoceptive, developmental, narrative,
 * social) and districts (AFI), computing city-wide coherence and
 * tracking entelechy progress.
 */
#include "opencog/entelechy/civic_angel.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace opencog::entelechy {

CivicAngel::CivicAngel() {
    initialize_districts();
}

void CivicAngel::initialize_districts() noexcept {
    using D = afi::DistrictId;
    const char* names[] = {
        "Temperament", "Interoceptive", "Developmental",
        "Narrative", "Social", "Cognitive",
        "Temporal", "Endocrine", "Neural"
    };
    for (size_t i = 0; i < static_cast<size_t>(D::COUNT); ++i) {
        districts_[i] = afi::CognitiveDistrict(
            static_cast<D>(i), names[i]);
    }
}

void CivicAngel::tick(float dt) noexcept {
    // Update all districts
    for (auto& d : districts_) {
        d.update(dt);
    }

    // Update subsystems
    narrative_.update(dt);
    social_.update(dt);
    developmental_.advance_healing(dt);

    // Assess city coherence
    assess_city_coherence();

    // Update entelechy progress
    update_entelechy_progress();

    // Compute self-model
    compute_self_model();
}

GainProfile CivicAngel::compute_temperament_gains() const noexcept {
    return cloninger_.compute_gains();
}

void CivicAngel::update_interoceptive(const float* hormone_channels) noexcept {
    interoceptive_.update(hormone_channels, 1.0f);

    // Feed interoceptive state as observations to the interoceptive district
    const auto& s = interoceptive_.state();
    std::vector<float> obs = {
        s.vagal_tone, s.sympathetic_drive, s.dorsal_vagal,
        s.cardiac_coherence, s.respiratory_rhythm, s.gut_brain_signal,
        s.immune_extended, s.insular_integration, s.allostatic_load,
        s.proprioceptive_tone, s.nociceptive_signal, s.thermoregulatory
    };
    districts_[static_cast<size_t>(afi::DistrictId::INTEROCEPTIVE)].observe(obs);
}

void CivicAngel::update_identity(
    const FeltSense& felt_sense, uint64_t current_tick) noexcept
{
    // 1. Accumulate developmental experience
    developmental_.accumulate_experience(state_, felt_sense, 1.0f);

    // 2. Check for developmental stage transition
    developmental_.check_stage_transition(
        cloninger_.profile(), state_, current_tick);

    // 3. Apply developmental effects to temperament
    developmental_.apply_developmental_effects(cloninger_.profile_mut());

    // 4. Update narrative
    // Check if we should open a new chapter (first tick or after close)
    if (narrative_.chapters().empty()) {
        narrative_.open_new_chapter(current_tick, felt_sense.valence);
    }

    // 5. Update social self
    social_.develop_theory_of_mind(felt_sense.salience, 1.0f);

    // 6. Feed observations to identity districts
    std::vector<float> dev_obs = {
        developmental_.maturity(),
        developmental_.current_plasticity(),
        developmental_.cumulative_trauma_load(),
        static_cast<float>(developmental_.current_stage()),
        developmental_.experience_points() / 10000.0f
    };
    districts_[static_cast<size_t>(afi::DistrictId::DEVELOPMENTAL)].observe(dev_obs);

    std::vector<float> narr_obs = {
        narrative_.narrative_coherence(),
        narrative_.identity_strength(),
        narrative_.redemption_ratio(),
        narrative_.contamination_ratio(),
        static_cast<float>(narrative_.life_theme())
    };
    districts_[static_cast<size_t>(afi::DistrictId::NARRATIVE)].observe(narr_obs);

    std::vector<float> social_obs = {
        social_.social_integration(),
        social_.theory_of_mind_depth(),
        social_.social_confidence(),
        static_cast<float>(social_.attachment_style()),
        social_.independent_self(),
        social_.interdependent_self()
    };
    districts_[static_cast<size_t>(afi::DistrictId::SOCIAL)].observe(social_obs);
}

void CivicAngel::assess_city_coherence() noexcept {
    // Compute inter-district coherence
    float total_fe = 0.0f;
    float total_coherence = 0.0f;

    for (const auto& d : districts_) {
        auto m = d.metrics();
        total_fe += m.free_energy;
        total_coherence += m.coherence;
    }

    float n = static_cast<float>(districts_.size());
    state_.total_free_energy = total_fe;
    state_.mean_district_surprise = total_fe / n;
    state_.inter_district_coherence = total_coherence / n;

    // Resource allocation fairness: variance of district free energies
    float mean_fe = total_fe / n;
    float variance = 0.0f;
    for (const auto& d : districts_) {
        float diff = d.free_energy().total - mean_fe;
        variance += diff * diff;
    }
    variance /= n;
    state_.resource_allocation_fairness = 1.0f / (1.0f + variance);

    // Adaptive capacity: inverse of allostatic load * maturity
    state_.adaptive_capacity = developmental_.maturity() *
        (1.0f - std::min(1.0f, interoceptive_.state().allostatic_load));
}

void CivicAngel::update_entelechy_progress() noexcept {
    // Entelechy = actualization toward potential
    // Combines: maturity, narrative coherence, social integration,
    // inter-district coherence, self-transcendence

    float maturity = developmental_.maturity();
    float narr_coherence = narrative_.narrative_coherence();
    float social_integ = social_.social_integration();
    float city_coherence = state_.inter_district_coherence;
    float transcendence = cloninger_.profile().self_transcendence;

    state_.entelechy_progress =
        maturity * 0.25f +
        narr_coherence * 0.20f +
        social_integ * 0.15f +
        city_coherence * 0.20f +
        transcendence * 0.20f;

    state_.developmental_stage = developmental_.current_stage();
    state_.maturation_level = maturity;
    state_.narrative_coherence = narr_coherence;
    state_.dominant_life_theme = narrative_.life_theme();

    // Check for milestone
    if (state_.entelechy_progress - last_entelechy_milestone_ >
        entelechy_milestone_threshold_) {
        last_entelechy_milestone_ = state_.entelechy_progress;
        // EntelechyEvent::ENTELECHY_MILESTONE would be signaled here
    }
}

void CivicAngel::compute_self_model() noexcept {
    // Self-coherence: how well the self-model matches actual state
    state_.self_coherence = (state_.inter_district_coherence * 0.4f +
                            narrative_.narrative_coherence() * 0.3f +
                            social_.social_integration() * 0.3f);

    // Self-complexity: richness of self-representation
    float district_diversity = 0.0f;
    for (const auto& d : districts_) {
        if (d.free_energy().total > 0.01f) {
            district_diversity += 1.0f;
        }
    }
    district_diversity /= static_cast<float>(districts_.size());

    state_.self_complexity = (district_diversity * 0.3f +
                             developmental_.maturity() * 0.3f +
                             narrative_.identity_strength() * 0.2f +
                             social_.theory_of_mind_depth() * 0.2f);
}

afi::CognitiveDistrict& CivicAngel::district(afi::DistrictId id) noexcept {
    return districts_[static_cast<size_t>(id)];
}

const afi::CognitiveDistrict& CivicAngel::district(afi::DistrictId id) const noexcept {
    return districts_[static_cast<size_t>(id)];
}

bool CivicAngel::should_request_guidance() const noexcept {
    // Check various alarm conditions
    if (interoceptive_.state().allostatic_load > 0.8f) return true;
    if (state_.inter_district_coherence < coherence_alarm_threshold_) return true;
    if (state_.total_free_energy > free_energy_alarm_threshold_) return true;
    if (narrative_.narrative_coherence() < 0.2f) return true;
    return false;
}

EntelechyGuidanceReason CivicAngel::guidance_reason() const noexcept {
    if (interoceptive_.state().allostatic_load > 0.8f) {
        return EntelechyGuidanceReason::INTEROCEPTIVE_ALARM;
    }
    if (state_.inter_district_coherence < coherence_alarm_threshold_) {
        return EntelechyGuidanceReason::CITY_DIVERGENCE;
    }
    if (narrative_.narrative_coherence() < 0.2f) {
        return EntelechyGuidanceReason::NARRATIVE_INCOHERENCE;
    }
    return EntelechyGuidanceReason::DEVELOPMENTAL_CRISIS;
}

void CivicAngel::write_interoceptive_channels(float* channels) const noexcept {
    interoceptive_.write_to_channels(channels);
}

} // namespace opencog::entelechy
