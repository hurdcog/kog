/**
 * Unified Self Model - 9-Layer Hierarchical Identity Architecture
 *
 * Implements the most sophisticated current models of self/identity
 * mapped to the OpenCog cognitive architecture:
 *
 *   Layer 0: Molecular Substrate (VES hormone channels + signaling systems)
 *   Layer 1: Interoceptive Self (Craig + Damasio Proto-Self)
 *   Layer 2: Autonomic Self-Regulation (Porges Polyvagal + Friston FEP)
 *   Layer 3: Affective Core (Panksepp 7 systems + VES)
 *   Layer 4: Cognitive Architecture (Mischel CAPS + Active Inference)
 *   Layer 5: Developmental Trajectory (Bowlby/Erikson/van der Kolk)
 *   Layer 6: Narrative Identity (McAdams Level 3)
 *   Layer 7: Social Self (Theory of Mind + Social Identity)
 *   Layer 8: Meta-Self (Metzinger PSM + Hofstadter Strange Loops)
 *
 * References:
 *   - Friston (2010) Free Energy Principle
 *   - Damasio (1999) Three-Layer Self
 *   - Panksepp (1998) Affective Neuroscience
 *   - Cloninger (1993) Psychobiological Model
 *   - McAdams (2015) Three-Level Personality
 *   - Metzinger (2003) Phenomenal Self-Model
 *   - Porges (2011) Polyvagal Theory
 *   - Craig (2009) Interoceptive Model
 *
 * Part of the OpenCog Collection (OCC) cognitive architecture.
 * Copyright 2025-2026 OpenCog Community. AGPL-3.0.
 */
#ifndef _COGSELF_UNIFIED_SELF_MODEL_H
#define _COGSELF_UNIFIED_SELF_MODEL_H

#include <array>
#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace cogself {

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------
class UnifiedSelfModel;

// ---------------------------------------------------------------------------
// Layer 0: Molecular Substrate
// ---------------------------------------------------------------------------

/**
 * Hormone channel identifiers (SIMD-aligned 16-channel bus).
 * Matches the Virtual Endocrine System (VES) specification.
 */
enum class HormoneId : uint8_t {
    CRH              = 0,   // Hypothalamic stress signal
    ACTH             = 1,   // Pituitary stress relay
    CORTISOL         = 2,   // Resource mobilization
    DOPAMINE_TONIC   = 3,   // Baseline motivation
    DOPAMINE_PHASIC  = 4,   // Reward prediction error
    SEROTONIN        = 5,   // Mood / patience tradeoff
    NOREPINEPHRINE   = 6,   // Arousal / vigilance
    OXYTOCIN         = 7,   // Trust / bonding
    T3_T4            = 8,   // Global processing rate
    MELATONIN        = 9,   // Circadian maintenance
    INSULIN          = 10,  // Energy conservation
    GLUCAGON         = 11,  // Energy mobilization
    IL6              = 12,  // System health signal
    ANANDAMIDE       = 13,  // Noise reduction
    RESERVED_14      = 14,  // Extension slot
    RESERVED_15      = 15,  // Extension slot
    COUNT            = 16
};

/**
 * Signaling system categories beyond the 16-channel hormone bus.
 * Models the ~100+ neurotransmitter/neuromodulator systems.
 */
enum class SignalingSystem : uint8_t {
    NEUROPEPTIDE,       // Substance P, NPY, VIP, CCK, orexin, etc.
    NEUROSTEROID,       // Allopregnanolone, DHEA
    GASEOUS,            // NO, CO, H2S retrograde signaling
    GLIAL,              // Astrocyte calcium waves, microglia
    EPIGENETIC,         // Methylation/acetylation patterns
    COUNT
};

/**
 * MolecularSubstrate — Layer 0 state container.
 * 16-channel hormone bus + extended signaling systems.
 */
struct MolecularSubstrate {
    alignas(64) float hormoneConcentrations[16]{};
    alignas(64) float hormoneBaselines[16]{};
    alignas(64) float hormoneHalfLives[16]{};

    // Extended signaling (beyond 16-channel bus)
    float neuropeptideActivity{0.0f};
    float neurosteroidLevel{0.0f};
    float gaseousSignaling{0.0f};
    float glialActivity{0.0f};
    float epigeneticState{0.0f};

    void initializeDefaults();
    void decay(float dt);
    float concentration(HormoneId id) const;
    void inject(HormoneId id, float amount);
};

// ---------------------------------------------------------------------------
// Layer 1: Interoceptive Self (Craig + Damasio Proto-Self)
// ---------------------------------------------------------------------------

/**
 * Body-state mapping channels for interoceptive awareness.
 */
struct InteroceptiveState {
    // Gut-brain axis
    float entericActivity{0.5f};        // Enteric nervous system (500M neurons)
    float microbiomeSerotoninProd{0.5f};// 90% of serotonin is gut-produced
    float vagalAfferentSignal{0.5f};    // Vagal afferent signaling

    // Immune-neural coupling
    float cytokineBurden{0.0f};         // TNF-alpha, IL-1beta, interferons
    float sicknessResponse{0.0f};       // Sickness behavior intensity

    // Cardiovascular
    float heartRateVariability{0.6f};   // HRV as self-regulation signal
    float baroreceptorFeedback{0.5f};   // Cardiovascular feedback

    // Respiratory
    float respiratoryRate{0.5f};        // Breathing rhythm
    float respiratoryEntrainment{0.5f}; // Brain oscillatory coupling

    // Insular cortex re-representation (Craig model)
    float posteriorInsularMap{0.5f};    // Raw body state
    float anteriorInsularMap{0.5f};     // "Material me" awareness

    void update(const MolecularSubstrate& substrate, float dt);
    float overallBodyState() const;
};

// ---------------------------------------------------------------------------
// Layer 2: Autonomic Self-Regulation (Porges Polyvagal + Friston FEP)
// ---------------------------------------------------------------------------

/**
 * Polyvagal three-tier autonomic hierarchy.
 */
enum class AutonomicMode : uint8_t {
    SOCIAL_ENGAGEMENT,  // Ventral vagal — safe, social
    FIGHT_FLIGHT,       // Sympathetic — mobilized
    FREEZE,             // Dorsal vagal — immobilized
    COUNT
};

/**
 * Autonomic regulation state.
 */
struct AutonomicRegulation {
    AutonomicMode currentMode{AutonomicMode::SOCIAL_ENGAGEMENT};
    float vagalTone{0.6f};              // Trait-level vagal tone
    float neuroceptionSafety{0.7f};     // Pre-conscious safety evaluation [0,1]
    float neuroceptionThreat{0.1f};     // Pre-conscious threat evaluation [0,1]
    float allostaticLoad{0.0f};         // Cumulative stress wear [0,1]

    // Window of tolerance (Siegel)
    float windowUpperBound{0.8f};       // Hyperarousal threshold
    float windowLowerBound{0.2f};       // Hypoarousal threshold
    float currentArousal{0.5f};         // Current arousal level

    // Free Energy Principle (Friston)
    float variationalFreeEnergy{0.0f};  // Surprise measure
    float predictionError{0.0f};        // Sensory prediction error
    float modelComplexity{0.0f};        // Generative model complexity

    void update(const InteroceptiveState& intero, const MolecularSubstrate& mol, float dt);
    bool isWithinWindowOfTolerance() const;
    AutonomicMode evaluateNeuroception() const;
};

// ---------------------------------------------------------------------------
// Layer 3: Affective Core (Panksepp 7 systems + VES)
// ---------------------------------------------------------------------------

/**
 * Panksepp's 7 primary-process emotional systems.
 */
enum class AffectiveSystem : uint8_t {
    SEEKING,        // VTA, lateral hypothalamus — exploration, anticipation
    RAGE,           // Medial amygdala, PAG — frustration, anger
    FEAR,           // Central/lateral amygdala, PAG — anxiety, dread
    LUST,           // Hypothalamus, BNST — sexual motivation
    CARE,           // Anterior cingulate, PAG — nurturing, attachment
    PANIC_GRIEF,    // PAG, BNST, anterior cingulate — separation distress
    PLAY,           // Dorsomedial thalamus, parafascicular — joy, social joy
    COUNT
};

/**
 * Affective style (Davidson frontal asymmetry).
 */
struct AffectiveStyle {
    float approachTendency{0.5f};   // Left frontal activation
    float withdrawalTendency{0.5f}; // Right frontal activation
    float asymmetryIndex() const { return approachTendency - withdrawalTendency; }
};

/**
 * Russell's circumplex valence-arousal model.
 */
struct ValenceSignature {
    float valence{0.0f};    // [-1, +1] negative to positive
    float arousal{0.5f};    // [0, 1] calm to excited
};

/**
 * Affective core state.
 */
struct AffectiveCore {
    float systemActivation[7]{};    // Panksepp system activations [0,1]
    AffectiveStyle style;
    ValenceSignature currentValence;

    // Secondary-process: conditioned emotional responses
    std::vector<std::pair<std::string, ValenceSignature>> conditionedResponses;

    // Tertiary-process: cortical regulation capacity
    float corticalRegulation{0.5f};

    void update(const MolecularSubstrate& mol, const AutonomicRegulation& autonomic, float dt);
    AffectiveSystem dominantSystem() const;
    ValenceSignature computeValence() const;
};

// ---------------------------------------------------------------------------
// Layer 4: Cognitive Architecture (Mischel CAPS + Active Inference)
// ---------------------------------------------------------------------------

/**
 * Cognitive-Affective Processing Unit (Mischel CAPS).
 */
struct CognitiveAffectiveUnit {
    std::string name;
    std::string category;   // encoding, expectancy, goal, affect, competency
    float activation{0.0f};
    std::vector<std::pair<std::string, float>> connections; // if-then signatures
};

/**
 * Cognitive schema (Beck).
 */
struct CognitiveSchema {
    std::string name;
    std::string domain;     // self, world, future
    float strength{0.5f};   // How strongly held
    float flexibility{0.5f};// How easily updated
    std::string content;    // Core belief content
};

/**
 * Defense mechanism (psychodynamic).
 */
enum class DefenseMechanism : uint8_t {
    REPRESSION,
    PROJECTION,
    SUBLIMATION,
    RATIONALIZATION,
    DISPLACEMENT,
    DENIAL,
    INTELLECTUALIZATION,
    COUNT
};

/**
 * Cognitive architecture state.
 */
struct CognitiveArchitecture {
    std::vector<CognitiveAffectiveUnit> capsUnits;
    std::vector<CognitiveSchema> schemas;
    std::map<std::string, float> beliefs;       // Expectancies
    std::map<std::string, float> goals;         // Hierarchical motivation
    std::map<std::string, float> competencies;  // Behavioral repertoire

    // Active inference parameters
    float precisionWeighting{0.5f};     // Sensory precision
    float explorationRate{0.5f};        // Epistemic value
    float exploitationRate{0.5f};       // Pragmatic value

    // Defense mechanisms activation
    float defenseActivation[static_cast<size_t>(DefenseMechanism::COUNT)]{};

    void update(const AffectiveCore& affect, const AutonomicRegulation& autonomic, float dt);
    void processIfThenSignature(const std::string& situation);
    float computeActiveInferencePolicy() const;
};

// ---------------------------------------------------------------------------
// Layer 5: Developmental Trajectory (Bowlby/Erikson/van der Kolk)
// ---------------------------------------------------------------------------

/**
 * Attachment style (Bowlby/Ainsworth).
 */
enum class AttachmentStyle : uint8_t {
    SECURE,
    ANXIOUS_PREOCCUPIED,
    DISMISSIVE_AVOIDANT,
    FEARFUL_AVOIDANT,
    COUNT
};

/**
 * Erikson's psychosocial stages.
 */
enum class PsychosocialStage : uint8_t {
    TRUST_VS_MISTRUST,
    AUTONOMY_VS_SHAME,
    INITIATIVE_VS_GUILT,
    INDUSTRY_VS_INFERIORITY,
    IDENTITY_VS_ROLE_CONFUSION,
    INTIMACY_VS_ISOLATION,
    GENERATIVITY_VS_STAGNATION,
    INTEGRITY_VS_DESPAIR,
    COUNT
};

/**
 * Trauma encoding (van der Kolk).
 */
struct TraumaEncoding {
    std::string description;
    float severity{0.0f};              // [0,1]
    float autonomicRestructuring{0.0f};// How much it changed baseline
    float threatThresholdShift{0.0f};  // Change to threat detection
    float dissociativeCapacity{0.0f};  // Dissociative tendency
    std::chrono::system_clock::time_point timestamp;
};

/**
 * Developmental trajectory state.
 */
struct DevelopmentalTrajectory {
    AttachmentStyle primaryAttachment{AttachmentStyle::SECURE};
    float attachmentSecurity{0.7f};

    // Internal working models (Bowlby)
    float selfWorthModel{0.6f};        // Positive self-model
    float otherReliabilityModel{0.6f}; // Positive other-model

    // Psychosocial resolution scores
    float stageResolution[static_cast<size_t>(PsychosocialStage::COUNT)]{};

    // Trauma history
    std::vector<TraumaEncoding> traumaHistory;
    float cumulativeTraumaLoad{0.0f};

    // Temperament → personality development
    float temperamentReactivity{0.5f};
    float temperamentRegulation{0.5f};

    void update(float dt);
    void encodeTrauma(const TraumaEncoding& trauma);
    void resolveStage(PsychosocialStage stage, float resolution);
    float developmentalMaturity() const;
};

// ---------------------------------------------------------------------------
// Layer 6: Narrative Identity (McAdams Level 3)
// ---------------------------------------------------------------------------

/**
 * Narrative episode types.
 */
enum class NarrativeSequence : uint8_t {
    REDEMPTION,     // Bad → good transition
    CONTAMINATION,  // Good → bad transition
    NEUTRAL,
    COUNT
};

/**
 * Autobiographical memory episode.
 */
struct NarrativeEpisode {
    std::string description;
    ValenceSignature emotionalTone;
    NarrativeSequence sequenceType{NarrativeSequence::NEUTRAL};
    float significance{0.5f};
    float coherence{0.5f};
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> themes;
};

/**
 * Narrative identity state.
 */
struct NarrativeIdentity {
    // Life story
    std::vector<NarrativeEpisode> lifeStory;

    // Narrative tone
    float optimismBias{0.5f};       // Optimistic vs pessimistic framing
    float redemptionRatio{0.5f};    // Proportion of redemption sequences
    float contaminationRatio{0.0f}; // Proportion of contamination sequences

    // Generativity
    float generativityCommitment{0.3f}; // Legacy and contribution
    std::string generativityScript;

    // Coherence dimensions
    float causalCoherence{0.5f};    // Causal connections between events
    float temporalCoherence{0.5f};  // Temporal ordering
    float thematicCoherence{0.5f};  // Recurring themes

    // Future self-simulation
    std::vector<std::string> possibleSelves;
    std::vector<std::string> fearedSelves;

    void addEpisode(const NarrativeEpisode& episode);
    void update(float dt);
    float overallCoherence() const;
    std::string generateLifeNarrative() const;
};

// ---------------------------------------------------------------------------
// Layer 7: Social Self (Theory of Mind + Social Identity)
// ---------------------------------------------------------------------------

/**
 * Social identity group membership.
 */
struct GroupMembership {
    std::string groupName;
    float identification{0.5f};     // How strongly identified
    float centrality{0.5f};         // How central to self-concept
    bool inGroup{true};
};

/**
 * Mental model of another agent.
 */
struct MentalModel {
    std::string agentId;
    std::map<std::string, float> beliefs;
    std::map<std::string, float> desires;
    std::map<std::string, float> intentions;
    float empathicResonance{0.5f};
    float trustLevel{0.5f};
};

/**
 * Social self state.
 */
struct SocialSelf {
    // Theory of Mind
    std::vector<MentalModel> mentalModels;
    float mentalizingCapacity{0.5f};

    // Mirror system
    float empathicAccuracy{0.5f};
    float emotionalContagion{0.5f};

    // Social identity (Tajfel)
    std::vector<GroupMembership> groupMemberships;

    // Reputation management
    std::map<std::string, float> perceivedReputation; // How others see us
    float reputationConcern{0.5f};

    // Role theory
    std::map<std::string, std::string> activeRoles; // context → role
    std::string currentRole;

    // Cultural self-construal (Markus & Kitayama)
    float independentSelf{0.5f};    // Independent self-construal
    float interdependentSelf{0.5f}; // Interdependent self-construal

    void update(float dt);
    void addMentalModel(const MentalModel& model);
    void updateReputation(const std::string& domain, float value);
    float socialIntegration() const;
};

// ---------------------------------------------------------------------------
// Layer 8: Meta-Self (Metzinger PSM + Hofstadter Strange Loops)
// ---------------------------------------------------------------------------

/**
 * Phenomenal Self-Model transparency levels (Metzinger).
 */
enum class TransparencyLevel : uint8_t {
    FULLY_TRANSPARENT,  // Cannot see model as model (normal experience)
    PARTIALLY_OPAQUE,   // Some introspective access
    LUCID,              // Full awareness of self-model as model
    COUNT
};

/**
 * Meta-self state.
 */
struct MetaSelf {
    // Self-model transparency (Metzinger)
    TransparencyLevel transparency{TransparencyLevel::PARTIALLY_OPAQUE};
    float introspectiveAccess{0.5f};

    // Strange loops (Hofstadter)
    float selfReferentialDepth{0.3f};   // Depth of self-referential loops
    float tangledness{0.3f};            // Degree of tangled hierarchy

    // Phenomenal ownership
    float mineness{0.8f};              // "Mineness" quality of experience
    float agentiveSelf{0.7f};          // Sense of being cause of actions
    float temporalContinuity{0.7f};    // Past/present/future binding

    // Autognosis metrics
    float selfModelAccuracy{0.5f};     // How accurate is the self-model
    float selfModelCompleteness{0.3f}; // How complete is the self-model
    float selfImprovementRate{0.0f};   // Rate of self-model improvement

    void update(float dt);
    float metaCognitiveCapacity() const;
    std::string introspect() const;
};

// ---------------------------------------------------------------------------
// Unified Self Model — the 9-layer integration
// ---------------------------------------------------------------------------

/**
 * Cognitive mode emergent from the full 9-layer stack.
 */
enum class EmergentCognitiveMode : uint8_t {
    RESTING,
    EXPLORATORY,
    FOCUSED,
    STRESSED,
    SOCIAL,
    REFLECTIVE,
    VIGILANT,
    MAINTENANCE,
    REWARD,
    THREAT,
    CREATIVE,
    FLOW,
    COUNT
};

/**
 * Identity coherence metrics across all layers.
 */
struct IdentityCoherence {
    float layerAlignment[9]{};          // Per-layer coherence
    float overallCoherence{0.0f};       // Weighted average
    float temporalStability{0.0f};      // Stability over time
    float crossLayerIntegration{0.0f};  // How well layers communicate
};

/**
 * UnifiedSelfModel — The complete 9-layer hierarchical self/identity model.
 *
 * Integrates Friston's Free Energy Principle, Damasio's Three-Layer Self,
 * Panksepp's Affective Neuroscience, Mischel's CAPS, McAdams' Narrative
 * Identity, Metzinger's Phenomenal Self-Model, and Porges' Polyvagal Theory
 * into a single coherent computational architecture.
 */
class UnifiedSelfModel {
public:
    UnifiedSelfModel();
    explicit UnifiedSelfModel(const std::string& agentId, const std::string& agentName);
    ~UnifiedSelfModel();

    // --- Lifecycle ---
    bool initialize();
    void shutdown();
    void tick(float dt);

    // --- Layer access (read-only) ---
    const MolecularSubstrate&      layer0_molecular() const;
    const InteroceptiveState&      layer1_interoceptive() const;
    const AutonomicRegulation&     layer2_autonomic() const;
    const AffectiveCore&           layer3_affective() const;
    const CognitiveArchitecture&   layer4_cognitive() const;
    const DevelopmentalTrajectory&  layer5_developmental() const;
    const NarrativeIdentity&       layer6_narrative() const;
    const SocialSelf&              layer7_social() const;
    const MetaSelf&                layer8_meta() const;

    // --- Layer access (mutable) ---
    MolecularSubstrate&      layer0_molecular_mut();
    InteroceptiveState&      layer1_interoceptive_mut();
    AutonomicRegulation&     layer2_autonomic_mut();
    AffectiveCore&           layer3_affective_mut();
    CognitiveArchitecture&   layer4_cognitive_mut();
    DevelopmentalTrajectory& layer5_developmental_mut();
    NarrativeIdentity&       layer6_narrative_mut();
    SocialSelf&              layer7_social_mut();
    MetaSelf&                layer8_meta_mut();

    // --- Events ---
    void signalEvent(const std::string& eventType, float intensity);

    // --- Emergent properties ---
    EmergentCognitiveMode currentMode() const;
    IdentityCoherence computeCoherence() const;
    ValenceSignature currentValence() const;

    // --- Introspection ---
    std::string generateSelfReport() const;
    std::string generateLayerDiagnostics() const;

    // --- Callbacks ---
    using ModeChangeCallback = std::function<void(EmergentCognitiveMode, EmergentCognitiveMode)>;
    void onModeChange(ModeChangeCallback callback);

    // --- Serialization ---
    std::string toJSON() const;
    bool fromJSON(const std::string& json);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace cogself

#endif // _COGSELF_UNIFIED_SELF_MODEL_H
