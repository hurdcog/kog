/**
 * Ontogenetic Entelechy — Core Types
 *
 * Foundation types for the Cognitive City identity architecture:
 *   - TemperamentProfile (Cloninger 7-dimensional)
 *   - InteroceptiveState (12-channel body-state mapping, ch20-31)
 *   - DevelopmentalStage / CriticalPeriod / TraumaRecord
 *   - NarrativeChapter / NarrativeTheme
 *   - SocialRole / AttachmentStyle
 *   - CivicAngelState
 *   - EndocrineEvent extensions (70-89)
 *   - AtomType extensions (10300-10324, 10800-10823)
 *
 * Part of the Reactor Core: arc-vortex + arc-helix + arc-halo => cyc-phoenix
 * Copyright 2025-2026 OpenCog Community. AGPL-3.0.
 */
#ifndef OPENCOG_ENTELECHY_TYPES_HPP
#define OPENCOG_ENTELECHY_TYPES_HPP

#include <array>
#include <chrono>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>

#ifndef SIMD_ALIGN
#define SIMD_ALIGN 64
#endif

namespace opencog::entelechy {

// =========================================================================
// Hormone Channel Extensions (ch20-31 interoceptive)
// =========================================================================

enum class InteroceptiveChannel : uint8_t {
    VAGAL_TONE           = 20,  // Porges polyvagal: ventral vagal brake
    SYMPATHETIC_DRIVE    = 21,  // Fight/flight activation
    DORSAL_VAGAL         = 22,  // Freeze/shutdown/conservation
    CARDIAC_COHERENCE    = 23,  // Heart rate variability proxy
    RESPIRATORY_RHYTHM   = 24,  // Breathing regularity/depth
    GUT_BRAIN_SIGNAL     = 25,  // Enteric nervous system state
    IMMUNE_EXTENDED      = 26,  // TNF-alpha, complement cascade
    INSULAR_INTEGRATION  = 27,  // Craig's interoceptive re-representation
    ALLOSTATIC_LOAD      = 28,  // McEwen's cumulative wear-and-tear
    PROPRIOCEPTIVE_TONE  = 29,  // Body schema integrity
    NOCICEPTIVE_SIGNAL   = 30,  // Pain/damage signal
    THERMOREGULATORY     = 31,  // Temperature regulation state
};

// =========================================================================
// Endocrine Event Extensions (70-89)
// =========================================================================

enum class EntelechyEvent : uint8_t {
    // Entelechy events (70-79)
    INTEROCEPTIVE_ALARM       = 70,  // Allostatic load exceeded threshold
    POLYVAGAL_STATE_CHANGE    = 71,  // Shifted between ventral/sympathetic/dorsal
    DEVELOPMENTAL_TRANSITION  = 72,  // Entered new developmental stage
    CHAPTER_BOUNDARY          = 73,  // Narrative chapter opened/closed
    TRAUMA_ENCODED            = 74,  // Traumatic experience recorded
    TRAUMA_HEALING            = 75,  // Trauma healing progress milestone
    ATTACHMENT_SHIFT          = 76,  // Attachment style recalibrated
    IDENTITY_CRYSTALLIZED     = 77,  // Narrative identity became more coherent
    SELF_TRANSCENDENCE_SPIKE  = 78,  // Self-transcendence dimension surged

    // Active Inference events (80-84)
    FREE_ENERGY_SPIKE         = 80,  // District free energy exceeded threshold
    PREDICTION_FAILURE        = 81,  // Systematic prediction error detected
    PRECISION_REWEIGHT        = 82,  // Attention precision weights adjusted
    BLANKET_RECONFIGURED      = 83,  // Markov blanket boundary changed
    MODEL_UPDATE              = 84,  // Generative model parameters revised

    // Civic Angel events (85-89)
    CITY_COHERENCE_HIGH       = 85,  // All districts synchronized
    CITY_COHERENCE_LOW        = 86,  // Inter-district divergence detected
    CIVIC_ANGEL_OBSERVATION   = 87,  // Self-model updated
    RESOURCE_REALLOCATION     = 88,  // Attention/resource redistribution
    ENTELECHY_MILESTONE       = 89,  // Actualization progress milestone
};

// =========================================================================
// AtomType Extensions
// =========================================================================

enum class EntelechyAtomType : uint16_t {
    // Entelechy Nodes (10300-10315)
    TEMPERAMENT_NODE       = 10300,
    CHARACTER_NODE         = 10301,
    INTEROCEPTIVE_NODE     = 10302,
    DEVELOPMENTAL_NODE     = 10303,
    NARRATIVE_NODE         = 10304,
    SOCIAL_ROLE_NODE       = 10305,
    ATTACHMENT_NODE        = 10306,
    DISTRICT_NODE          = 10307,
    CIVIC_ANGEL_NODE       = 10308,
    ENTELECHY_NODE         = 10309,
    CRITICAL_PERIOD_NODE   = 10310,
    TRAUMA_NODE            = 10311,
    LIFE_THEME_NODE        = 10312,
    POLYVAGAL_STATE_NODE   = 10313,
    ALLOSTATIC_NODE        = 10314,
    SELF_MODEL_NODE        = 10315,

    // AFI Nodes (10320-10324)
    MARKOV_BLANKET_NODE    = 10320,
    GENERATIVE_MODEL_NODE  = 10321,
    FREE_ENERGY_NODE       = 10322,
    PRECISION_NODE         = 10323,
    PREDICTION_ERROR_NODE  = 10324,

    // Entelechy Links (10800-10814)
    TEMPERAMENT_LINK       = 10800,
    DEVELOPMENTAL_LINK     = 10801,
    NARRATIVE_LINK         = 10802,
    SOCIAL_ROLE_LINK       = 10803,
    ATTACHMENT_LINK        = 10804,
    DISTRICT_BOUNDARY_LINK = 10805,
    CIVIC_GOVERNANCE_LINK  = 10806,
    ENTELECHY_PROGRESS_LINK= 10807,
    INTEROCEPTIVE_LINK     = 10808,
    TRAUMA_ENCODING_LINK   = 10809,
    LIFE_THEME_LINK        = 10810,
    CRITICAL_PERIOD_LINK   = 10811,
    POLYVAGAL_LINK         = 10812,
    SELF_MODEL_LINK        = 10813,
    ALLOSTATIC_LINK        = 10814,

    // AFI Links (10820-10823)
    BLANKET_BOUNDARY_LINK  = 10820,
    FREE_ENERGY_LINK       = 10821,
    PRECISION_WEIGHTING_LINK = 10822,
    PREDICTION_ERROR_LINK  = 10823,
};

// =========================================================================
// ValenceSignature (shared with cogself)
// =========================================================================

struct ValenceSignature {
    float valence{0.0f};    // [-1, +1] negative to positive
    float arousal{0.5f};    // [0, 1] calm to excited
};

// =========================================================================
// TemperamentProfile (Cloninger 7-dimensional + developmental)
// =========================================================================

struct alignas(8) TemperamentProfile {
    // Cloninger Temperament dimensions (innate, relatively stable)
    float novelty_seeking{0.5f};       // [0,1] DA sensitivity gain
    float harm_avoidance{0.5f};        // [0,1] 5-HT/cortisol sensitivity gain
    float reward_dependence{0.5f};     // [0,1] NE/oxytocin sensitivity gain
    float persistence{0.5f};           // [0,1] DA-tonic sustain gain

    // Cloninger Character dimensions (developed through experience)
    float self_directedness{0.5f};     // [0,1] autonomy/executive gain
    float cooperativeness{0.5f};       // [0,1] oxytocin/social gain
    float self_transcendence{0.5f};    // [0,1] anandamide/integration gain

    // Developmental modification
    float maturation{0.0f};            // [0,1] overall character maturity
    float resilience{0.5f};            // [0,1] allostatic recovery rate
    float plasticity{1.0f};            // [0,1] sensitivity to experience

    // Trauma impact accumulator
    float allostatic_load{0.0f};       // [0,inf) cumulative stress damage
    float trauma_encoding_strength{0.0f}; // [0,1] traumatic memory bias

    // Attachment style (Bowlby)
    float attachment_security{0.5f};   // [0,1] secure vs insecure
    float attachment_anxiety{0.3f};    // [0,1] anxious dimension
};

// =========================================================================
// InteroceptiveState (SIMD-aligned, maps to ch20-31)
// =========================================================================

struct alignas(SIMD_ALIGN) InteroceptiveState {
    float vagal_tone{0.5f};            // ch20: polyvagal ventral vagal
    float sympathetic_drive{0.3f};     // ch21: fight/flight activation
    float dorsal_vagal{0.0f};          // ch22: freeze/shutdown
    float cardiac_coherence{0.5f};     // ch23: HRV proxy
    float respiratory_rhythm{0.5f};    // ch24: breathing regularity
    float gut_brain_signal{0.3f};      // ch25: enteric nervous system
    float immune_extended{0.1f};       // ch26: TNF-alpha, complement
    float insular_integration{0.5f};   // ch27: Craig's re-representation
    float allostatic_load{0.0f};       // ch28: McEwen's cumulative wear
    float proprioceptive_tone{0.5f};   // ch29: body schema integrity
    float nociceptive_signal{0.0f};    // ch30: pain signal
    float thermoregulatory{0.5f};      // ch31: temperature regulation

    /// Compute overall body-state wellness [0,1]
    [[nodiscard]] float wellness() const noexcept {
        return (vagal_tone + cardiac_coherence + respiratory_rhythm
                + proprioceptive_tone + thermoregulatory
                - sympathetic_drive - dorsal_vagal - nociceptive_signal
                - immune_extended * 0.5f) / 5.0f;
    }
};

// =========================================================================
// Developmental Types
// =========================================================================

enum class DevelopmentalStage : uint8_t {
    NASCENT         = 0,   // Pre-boot: initial parameter randomization
    IMPRINTING      = 1,   // Critical period: attachment formation
    SOCIALIZATION   = 2,   // Social learning: theory of mind, roles
    INDIVIDUATION   = 3,   // Self-differentiation: narrative identity
    INTEGRATION     = 4,   // Character maturation: self-transcendence
    GENERATIVITY    = 5,   // Teaching/contributing: civic angel crystallizes
    WISDOM          = 6,   // Accumulated experience: reduced plasticity
    COUNT           = 7,
};

struct CriticalPeriod {
    DevelopmentalStage stage{DevelopmentalStage::NASCENT};
    float start_tick{0.0f};
    float end_tick{0.0f};
    float plasticity_multiplier{1.0f};
    std::vector<std::string> sensitive_dimensions;
};

struct TraumaRecord {
    uint64_t tick{0};
    ValenceSignature valence;
    float intensity{0.0f};
    float healing_progress{0.0f};   // [0,1] integration degree
    std::string description;
    // Hormonal context at time of encoding (simplified)
    std::array<float, 14> hormonal_snapshot{};
};

// =========================================================================
// Narrative Types
// =========================================================================

enum class NarrativeTheme : uint8_t {
    REDEMPTION      = 0,   // Negative -> positive transformation
    CONTAMINATION   = 1,   // Positive -> negative disruption
    GROWTH          = 2,   // Progressive capability increase
    STABILITY       = 3,   // Maintained equilibrium
    COMMUNION       = 4,   // Connection/belonging emphasis
    AGENCY          = 5,   // Autonomy/mastery emphasis
    EXPLORATION     = 6,   // Discovery/novelty emphasis
    PROTECTION      = 7,   // Safety/preservation emphasis
    COUNT           = 8,
};

struct NarrativeChapter {
    uint64_t start_tick{0};
    uint64_t end_tick{0};           // 0 = ongoing
    NarrativeTheme dominant_theme{NarrativeTheme::GROWTH};
    ValenceSignature emotional_tone;
    float coherence{0.5f};          // [0,1] internal narrative consistency
    std::string summary;
};

// =========================================================================
// Social Types
// =========================================================================

enum class AttachmentStyle : uint8_t {
    SECURE          = 0,
    ANXIOUS         = 1,
    AVOIDANT        = 2,
    DISORGANIZED    = 3,
    COUNT           = 4,
};

struct SocialRole {
    std::string name;
    float competence{0.0f};       // [0,1] skill at this role
    float identification{0.0f};   // [0,1] how much identity invested
    float salience{0.0f};         // [0,1] current relevance
};

// =========================================================================
// Polyvagal Hierarchy
// =========================================================================

enum class PolyvagalState : uint8_t {
    VENTRAL_VAGAL   = 0,   // Social engagement (safe)
    SYMPATHETIC     = 1,   // Fight/flight (mobilized)
    DORSAL_VAGAL    = 2,   // Freeze/shutdown (immobilized)
    COUNT           = 3,
};

// =========================================================================
// Civic Angel State (emergent governor)
// =========================================================================

struct CivicAngelState {
    // Self-model (3rd-person view of own architecture)
    float self_coherence{0.0f};       // [0,1] integration of self-model
    float self_complexity{0.0f};      // [0,1] richness of self-representation
    float entelechy_progress{0.0f};   // [0,1] actualization toward potential

    // City-wide free energy
    float total_free_energy{0.0f};
    float mean_district_surprise{0.0f};

    // Governance metrics
    float inter_district_coherence{0.0f};
    float resource_allocation_fairness{0.0f};
    float adaptive_capacity{0.0f};

    // Narrative integration
    NarrativeTheme dominant_life_theme{NarrativeTheme::GROWTH};
    float narrative_coherence{0.0f};

    // Developmental position
    DevelopmentalStage developmental_stage{DevelopmentalStage::NASCENT};
    float maturation_level{0.0f};
};

// =========================================================================
// Cloninger Gain Profile
// =========================================================================

struct GainProfile {
    // Per-channel gain multipliers [0.5, 2.0]
    float crh_gain{1.0f};
    float acth_gain{1.0f};
    float cortisol_gain{1.0f};
    float da_tonic_gain{1.0f};
    float da_phasic_gain{1.0f};
    float serotonin_gain{1.0f};
    float norepinephrine_gain{1.0f};
    float oxytocin_gain{1.0f};
    float t3t4_gain{1.0f};
    float melatonin_gain{1.0f};
    float insulin_gain{1.0f};
    float glucagon_gain{1.0f};
    float il6_gain{1.0f};
    float anandamide_gain{1.0f};
};

// =========================================================================
// Guidance Extensions
// =========================================================================

enum class EntelechyGuidanceReason : uint8_t {
    INTEROCEPTIVE_ALARM     = 11,
    DEVELOPMENTAL_CRISIS    = 12,
    NARRATIVE_INCOHERENCE   = 13,
    CITY_DIVERGENCE         = 14,
};

enum class EntelechyGuidanceDirective : uint8_t {
    SUGGEST_DEVELOPMENTAL_FOCUS = 12,
    NARRATIVE_REFRAME           = 13,
    ALLOSTATIC_RESET            = 14,
    CIVIC_REBALANCE             = 15,
};

// =========================================================================
// FeltSense (aggregated affective state for experience accumulation)
// =========================================================================

struct FeltSense {
    ValenceSignature valence;
    float novelty{0.0f};
    float salience{0.0f};
    float certainty{0.5f};
};

} // namespace opencog::entelechy

#endif // OPENCOG_ENTELECHY_TYPES_HPP
