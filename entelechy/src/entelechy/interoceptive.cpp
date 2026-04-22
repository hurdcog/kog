/**
 * InteroceptiveModel — Implementation
 *
 * Maps hormone concentrations (ch0-19) to interoceptive state (ch20-31)
 * and evaluates polyvagal hierarchy.
 */
#include "opencog/entelechy/interoceptive.hpp"
#include <algorithm>
#include <cmath>

namespace opencog::entelechy {

// Hormone channel indices (matching existing VES layout)
namespace ch {
    constexpr int CRH          = 0;
    constexpr int ACTH         = 1;
    constexpr int CORTISOL     = 2;
    constexpr int DA_TONIC     = 3;
    constexpr int DA_PHASIC    = 4;
    constexpr int SEROTONIN    = 5;
    constexpr int NE           = 6;
    constexpr int OXYTOCIN     = 7;
    constexpr int T3T4         = 8;
    constexpr int MELATONIN    = 9;
    constexpr int INSULIN      = 10;
    constexpr int GLUCAGON     = 11;
    constexpr int IL6          = 12;
    constexpr int ANANDAMIDE   = 13;
    constexpr int NPU_LOAD     = 14;
    constexpr int COG_COHERENCE= 15;
    constexpr int MARDUK_LOAD  = 16;
    constexpr int ORG_COHERENCE= 17;
}

void InteroceptiveModel::initialize_defaults() noexcept {
    state_ = InteroceptiveState{};
    polyvagal_ = PolyvagalState::VENTRAL_VAGAL;
    arousal_ = 0.5f;
    window_upper_ = 0.8f;
    window_lower_ = 0.2f;
}

void InteroceptiveModel::update(const float* h, float dt) noexcept {
    if (!h) return;

    // === Vagal Tone (ch20) ===
    // High serotonin + oxytocin -> high vagal tone
    // High cortisol + NE -> suppressed vagal tone
    float vagal_drive = (h[ch::SEROTONIN] * 0.4f + h[ch::OXYTOCIN] * 0.3f
                        + h[ch::ANANDAMIDE] * 0.2f + h[ch::MELATONIN] * 0.1f);
    float vagal_suppress = (h[ch::CORTISOL] * 0.3f + h[ch::NE] * 0.3f
                           + h[ch::CRH] * 0.2f);
    state_.vagal_tone += (vagal_drive - vagal_suppress - state_.vagal_tone) * 0.1f * dt;
    state_.vagal_tone = std::clamp(state_.vagal_tone, 0.0f, 1.0f);

    // === Sympathetic Drive (ch21) ===
    // NE + cortisol + CRH -> sympathetic activation
    state_.sympathetic_drive += ((h[ch::NE] * 0.4f + h[ch::CORTISOL] * 0.3f
                                 + h[ch::CRH] * 0.2f + h[ch::ACTH] * 0.1f)
                                - state_.sympathetic_drive) * 0.15f * dt;
    state_.sympathetic_drive = std::clamp(state_.sympathetic_drive, 0.0f, 1.0f);

    // === Dorsal Vagal (ch22) ===
    // Extreme stress + depletion -> freeze response
    float extreme_stress = std::max(0.0f, h[ch::CORTISOL] - 0.7f);
    float depletion = std::max(0.0f, 0.3f - h[ch::DA_TONIC]);
    state_.dorsal_vagal += ((extreme_stress * 0.5f + depletion * 0.3f)
                           - state_.dorsal_vagal) * 0.05f * dt;
    state_.dorsal_vagal = std::clamp(state_.dorsal_vagal, 0.0f, 1.0f);

    // === Cardiac Coherence (ch23) ===
    // Vagal tone + serotonin -> coherent HRV
    // Sympathetic drive + stress -> incoherent HRV
    state_.cardiac_coherence += ((state_.vagal_tone * 0.5f + h[ch::SEROTONIN] * 0.3f)
                                - (state_.sympathetic_drive * 0.3f)
                                - state_.cardiac_coherence) * 0.1f * dt;
    state_.cardiac_coherence = std::clamp(state_.cardiac_coherence, 0.0f, 1.0f);

    // === Respiratory Rhythm (ch24) ===
    state_.respiratory_rhythm += ((state_.vagal_tone * 0.4f + h[ch::MELATONIN] * 0.2f)
                                 - (state_.sympathetic_drive * 0.2f + h[ch::NE] * 0.1f)
                                 - state_.respiratory_rhythm + 0.5f) * 0.08f * dt;
    state_.respiratory_rhythm = std::clamp(state_.respiratory_rhythm, 0.0f, 1.0f);

    // === Gut-Brain Signal (ch25) ===
    // Serotonin (90% produced in gut) + stress -> gut signals
    state_.gut_brain_signal += ((h[ch::SEROTONIN] * 0.3f - h[ch::CORTISOL] * 0.2f
                                + h[ch::INSULIN] * 0.1f)
                               - state_.gut_brain_signal + 0.3f) * 0.05f * dt;
    state_.gut_brain_signal = std::clamp(state_.gut_brain_signal, 0.0f, 1.0f);

    // === Immune Extended (ch26) ===
    // IL6 + cortisol (immunosuppressive at high levels)
    state_.immune_extended += ((h[ch::IL6] * 0.5f + h[ch::CORTISOL] * 0.2f)
                              - state_.immune_extended) * 0.03f * dt;
    state_.immune_extended = std::clamp(state_.immune_extended, 0.0f, 1.0f);

    // === Insular Integration (ch27) ===
    // Craig's re-representation: integrates all interoceptive signals
    state_.insular_integration = (state_.vagal_tone + state_.cardiac_coherence
                                 + state_.respiratory_rhythm + state_.gut_brain_signal
                                 + state_.proprioceptive_tone) / 5.0f;

    // === Allostatic Load (ch28) ===
    // McEwen: cumulative wear-and-tear from chronic stress
    float stress_contribution = (h[ch::CORTISOL] * 0.3f + h[ch::NE] * 0.2f
                                + h[ch::IL6] * 0.2f);
    float recovery = (h[ch::SEROTONIN] * 0.1f + h[ch::OXYTOCIN] * 0.1f
                     + h[ch::MELATONIN] * 0.1f);
    state_.allostatic_load += (stress_contribution - recovery) * 0.01f * dt;
    state_.allostatic_load = std::max(0.0f, state_.allostatic_load);

    // === Proprioceptive Tone (ch29) ===
    state_.proprioceptive_tone += ((1.0f - state_.dorsal_vagal * 0.5f
                                   - state_.nociceptive_signal * 0.3f)
                                  - state_.proprioceptive_tone) * 0.05f * dt;
    state_.proprioceptive_tone = std::clamp(state_.proprioceptive_tone, 0.0f, 1.0f);

    // === Nociceptive Signal (ch30) ===
    // Pain from immune activation + extreme stress
    state_.nociceptive_signal += ((h[ch::IL6] * 0.3f
                                  + std::max(0.0f, h[ch::CORTISOL] - 0.8f) * 0.5f)
                                 - state_.nociceptive_signal) * 0.1f * dt;
    state_.nociceptive_signal = std::clamp(state_.nociceptive_signal, 0.0f, 1.0f);

    // === Thermoregulatory (ch31) ===
    state_.thermoregulatory += ((h[ch::T3T4] * 0.3f + 0.5f
                                - state_.sympathetic_drive * 0.1f)
                               - state_.thermoregulatory) * 0.05f * dt;
    state_.thermoregulatory = std::clamp(state_.thermoregulatory, 0.0f, 1.0f);

    // === Compute arousal ===
    arousal_ = (state_.sympathetic_drive * 0.4f + (1.0f - state_.vagal_tone) * 0.3f
               + state_.nociceptive_signal * 0.2f + state_.immune_extended * 0.1f);
    arousal_ = std::clamp(arousal_, 0.0f, 1.0f);

    // === Evaluate polyvagal state ===
    polyvagal_ = evaluate_polyvagal();
}

PolyvagalState InteroceptiveModel::evaluate_polyvagal() const noexcept {
    // Porges hierarchy: ventral vagal > sympathetic > dorsal vagal
    // Highest activation wins
    if (state_.dorsal_vagal > 0.5f &&
        state_.dorsal_vagal > state_.sympathetic_drive) {
        return PolyvagalState::DORSAL_VAGAL;
    }
    if (state_.sympathetic_drive > 0.5f &&
        state_.sympathetic_drive > state_.vagal_tone) {
        return PolyvagalState::SYMPATHETIC;
    }
    return PolyvagalState::VENTRAL_VAGAL;
}

float InteroceptiveModel::neuroception_safety() const noexcept {
    return state_.vagal_tone * 0.4f + state_.cardiac_coherence * 0.3f
           + (1.0f - state_.sympathetic_drive) * 0.2f
           + state_.proprioceptive_tone * 0.1f;
}

float InteroceptiveModel::neuroception_threat() const noexcept {
    return state_.sympathetic_drive * 0.3f + state_.nociceptive_signal * 0.3f
           + state_.immune_extended * 0.2f + state_.dorsal_vagal * 0.2f;
}

bool InteroceptiveModel::within_window_of_tolerance() const noexcept {
    return arousal_ >= window_lower_ && arousal_ <= window_upper_;
}

void InteroceptiveModel::write_to_channels(float* channels) const noexcept {
    if (!channels) return;
    channels[20] = state_.vagal_tone;
    channels[21] = state_.sympathetic_drive;
    channels[22] = state_.dorsal_vagal;
    channels[23] = state_.cardiac_coherence;
    channels[24] = state_.respiratory_rhythm;
    channels[25] = state_.gut_brain_signal;
    channels[26] = state_.immune_extended;
    channels[27] = state_.insular_integration;
    channels[28] = state_.allostatic_load;
    channels[29] = state_.proprioceptive_tone;
    channels[30] = state_.nociceptive_signal;
    channels[31] = state_.thermoregulatory;
}

} // namespace opencog::entelechy
