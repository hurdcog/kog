/**
 * EntelechyAdapter — Implementation
 *
 * Bidirectional adapter between the Civic Angel and the VES hormone bus.
 * Follows the EndocrineConnector pattern (apply_endocrine_modulation +
 * apply_feedback) with edge-triggered feedback to prevent oscillation.
 */
#include "opencog/entelechy/entelechy_adapter.hpp"
#include <algorithm>
#include <cmath>

namespace opencog::entelechy {

// Hormone channel indices (matching VES layout)
namespace ch {
    constexpr uint8_t CRH          = 0;
    constexpr uint8_t ACTH         = 1;
    constexpr uint8_t CORTISOL     = 2;
    constexpr uint8_t DA_TONIC     = 3;
    constexpr uint8_t DA_PHASIC    = 4;
    constexpr uint8_t SEROTONIN    = 5;
    constexpr uint8_t NE           = 6;
    constexpr uint8_t OXYTOCIN     = 7;
    constexpr uint8_t T3T4         = 8;
    constexpr uint8_t MELATONIN    = 9;
    constexpr uint8_t INSULIN      = 10;
    constexpr uint8_t GLUCAGON     = 11;
    constexpr uint8_t IL6          = 12;
    constexpr uint8_t ANANDAMIDE   = 13;
    constexpr uint8_t NPU_LOAD     = 14;
    constexpr uint8_t COG_COHERENCE= 15;
    constexpr uint8_t MARDUK_LOAD  = 16;
    constexpr uint8_t ORG_COHERENCE= 17;
}

void EntelechyAdapter::apply_temperament_gains(
    HormoneBusInterface& bus) noexcept
{
    if (!angel_) return;

    GainProfile gains = angel_->compute_temperament_gains();

    // Apply multiplicative gains to each hormone channel
    bus.set_gain(ch::CRH,       gains.crh_gain);
    bus.set_gain(ch::ACTH,      gains.acth_gain);
    bus.set_gain(ch::CORTISOL,  gains.cortisol_gain);
    bus.set_gain(ch::DA_TONIC,  gains.da_tonic_gain);
    bus.set_gain(ch::DA_PHASIC, gains.da_phasic_gain);
    bus.set_gain(ch::SEROTONIN, gains.serotonin_gain);
    bus.set_gain(ch::NE,        gains.norepinephrine_gain);
    bus.set_gain(ch::OXYTOCIN,  gains.oxytocin_gain);
    bus.set_gain(ch::T3T4,      gains.t3t4_gain);
    bus.set_gain(ch::MELATONIN, gains.melatonin_gain);
    bus.set_gain(ch::INSULIN,   gains.insulin_gain);
    bus.set_gain(ch::GLUCAGON,  gains.glucagon_gain);
    bus.set_gain(ch::IL6,       gains.il6_gain);
    bus.set_gain(ch::ANANDAMIDE,gains.anandamide_gain);
}

void EntelechyAdapter::apply_endocrine_modulation(
    const HormoneBusInterface& bus) noexcept
{
    if (!angel_) return;

    // Read all 20 hormone channels into a flat array for interoceptive model
    float hormones[20];
    for (uint8_t i = 0; i < 20; ++i) {
        hormones[i] = bus.concentration(i);
    }

    // Update interoceptive state from hormones
    angel_->update_interoceptive(hormones);
}

void EntelechyAdapter::apply_feedback(HormoneBusInterface& bus) noexcept {
    if (!angel_) return;

    const auto& intero = angel_->interoceptive();
    const auto& dev = angel_->developmental();
    const auto& narr = angel_->narrative();
    const auto& social = angel_->social();

    // === Edge-triggered polyvagal state change ===
    PolyvagalState current_pv = intero.polyvagal_state();
    if (current_pv != prev_polyvagal_) {
        switch (current_pv) {
        case PolyvagalState::SYMPATHETIC:
            // Fight/flight -> cortisol + NE surge
            bus.produce(ch::CORTISOL, 0.15f);
            bus.produce(ch::NE, 0.1f);
            break;
        case PolyvagalState::DORSAL_VAGAL:
            // Freeze -> massive cortisol + melatonin (shutdown)
            bus.produce(ch::CORTISOL, 0.2f);
            bus.produce(ch::MELATONIN, 0.15f);
            break;
        case PolyvagalState::VENTRAL_VAGAL:
            // Return to safety -> oxytocin + serotonin
            bus.produce(ch::OXYTOCIN, 0.1f);
            bus.produce(ch::SEROTONIN, 0.1f);
            break;
        }
        prev_polyvagal_ = current_pv;
    }

    // === Edge-triggered developmental stage change ===
    DevelopmentalStage current_stage = dev.current_stage();
    if (current_stage != prev_stage_) {
        // Developmental milestone -> serotonin + DA_phasic reward
        bus.produce(ch::SEROTONIN, 0.15f);
        bus.produce(ch::DA_PHASIC, 0.1f);
        prev_stage_ = current_stage;
    }

    // === Continuous: allostatic load -> IL6 ===
    float allostatic = intero.state().allostatic_load;
    if (allostatic > prev_allostatic_load_ + 0.05f) {
        bus.produce(ch::IL6, (allostatic - prev_allostatic_load_) * 0.2f);
    }
    prev_allostatic_load_ = allostatic;

    // === Edge-triggered narrative coherence change ===
    float narr_coh = narr.narrative_coherence();
    float narr_delta = narr_coh - prev_narrative_coherence_;
    if (narr_delta > 0.05f) {
        // Narrative becoming more coherent -> anandamide (integration)
        bus.produce(ch::ANANDAMIDE, narr_delta * 0.2f);
    } else if (narr_delta < -0.1f) {
        // Narrative disruption -> cortisol
        bus.produce(ch::CORTISOL, std::abs(narr_delta) * 0.15f);
    }
    prev_narrative_coherence_ = narr_coh;

    // === Edge-triggered social confidence change ===
    float social_conf = social.social_confidence();
    float social_delta = social_conf - prev_social_confidence_;
    if (social_delta > 0.05f) {
        // Social confidence growing -> oxytocin
        bus.produce(ch::OXYTOCIN, social_delta * 0.15f);
    } else if (social_delta < -0.1f) {
        // Social rejection/loss -> cortisol + NE
        bus.produce(ch::CORTISOL, std::abs(social_delta) * 0.1f);
        bus.produce(ch::NE, std::abs(social_delta) * 0.05f);
    }
    prev_social_confidence_ = social_conf;

    // === Window of tolerance check ===
    if (!intero.within_window_of_tolerance()) {
        // Outside window -> stress response
        float threat = intero.neuroception_threat();
        if (threat > 0.6f) {
            bus.produce(ch::CRH, 0.05f);
            bus.produce(ch::CORTISOL, 0.05f);
        }
    }
}

} // namespace opencog::entelechy
