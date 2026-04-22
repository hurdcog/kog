/**
 * Unified Self Model — 9-Layer Hierarchical Identity Architecture
 * Implementation file.
 *
 * Part of the OpenCog Collection (OCC) cognitive architecture.
 * Copyright 2025-2026 OpenCog Community. AGPL-3.0.
 */
#include "cogself/unified_self_model.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <sstream>

namespace cogself {

// ===================================================================
// Layer 0: MolecularSubstrate
// ===================================================================

void MolecularSubstrate::initializeDefaults() {
    // Half-lives (in ticks)
    hormoneHalfLives[0]  = 5.0f;   // CRH
    hormoneHalfLives[1]  = 10.0f;  // ACTH
    hormoneHalfLives[2]  = 30.0f;  // Cortisol
    hormoneHalfLives[3]  = 20.0f;  // Dopamine tonic
    hormoneHalfLives[4]  = 3.0f;   // Dopamine phasic
    hormoneHalfLives[5]  = 50.0f;  // Serotonin
    hormoneHalfLives[6]  = 8.0f;   // Norepinephrine
    hormoneHalfLives[7]  = 15.0f;  // Oxytocin
    hormoneHalfLives[8]  = 100.0f; // T3/T4
    hormoneHalfLives[9]  = 12.0f;  // Melatonin
    hormoneHalfLives[10] = 10.0f;  // Insulin
    hormoneHalfLives[11] = 8.0f;   // Glucagon
    hormoneHalfLives[12] = 20.0f;  // IL-6
    hormoneHalfLives[13] = 6.0f;   // Anandamide
    hormoneHalfLives[14] = 10.0f;  // Reserved
    hormoneHalfLives[15] = 10.0f;  // Reserved

    // Baselines
    hormoneBaselines[0]  = 0.05f;
    hormoneBaselines[1]  = 0.05f;
    hormoneBaselines[2]  = 0.15f;
    hormoneBaselines[3]  = 0.30f;
    hormoneBaselines[4]  = 0.00f;
    hormoneBaselines[5]  = 0.40f;
    hormoneBaselines[6]  = 0.10f;
    hormoneBaselines[7]  = 0.10f;
    hormoneBaselines[8]  = 0.50f;
    hormoneBaselines[9]  = 0.00f;
    hormoneBaselines[10] = 0.20f;
    hormoneBaselines[11] = 0.10f;
    hormoneBaselines[12] = 0.05f;
    hormoneBaselines[13] = 0.10f;
    hormoneBaselines[14] = 0.00f;
    hormoneBaselines[15] = 0.00f;

    // Initialize concentrations to baselines
    for (int i = 0; i < 16; ++i) {
        hormoneConcentrations[i] = hormoneBaselines[i];
    }
}

void MolecularSubstrate::decay(float dt) {
    for (int i = 0; i < 16; ++i) {
        if (hormoneHalfLives[i] > 0.0f) {
            float decayRate = std::log(2.0f) / hormoneHalfLives[i];
            float diff = hormoneConcentrations[i] - hormoneBaselines[i];
            hormoneConcentrations[i] = hormoneBaselines[i] + diff * std::exp(-decayRate * dt);
        }
    }
}

float MolecularSubstrate::concentration(HormoneId id) const {
    return hormoneConcentrations[static_cast<uint8_t>(id)];
}

void MolecularSubstrate::inject(HormoneId id, float amount) {
    uint8_t idx = static_cast<uint8_t>(id);
    hormoneConcentrations[idx] = std::clamp(hormoneConcentrations[idx] + amount, 0.0f, 1.0f);
}

// ===================================================================
// Layer 1: InteroceptiveState
// ===================================================================

void InteroceptiveState::update(const MolecularSubstrate& substrate, float dt) {
    // Gut-brain axis modulated by serotonin production
    float serotonin = substrate.concentration(HormoneId::SEROTONIN);
    microbiomeSerotoninProd = 0.9f * microbiomeSerotoninProd + 0.1f * serotonin;
    vagalAfferentSignal = 0.8f * vagalAfferentSignal + 0.2f * entericActivity;

    // Immune-neural coupling from IL-6
    float il6 = substrate.concentration(HormoneId::IL6);
    cytokineBurden = 0.95f * cytokineBurden + 0.05f * il6;
    sicknessResponse = std::clamp(cytokineBurden * 2.0f, 0.0f, 1.0f);

    // Cardiovascular from cortisol and norepinephrine
    float cortisol = substrate.concentration(HormoneId::CORTISOL);
    float norepi = substrate.concentration(HormoneId::NOREPINEPHRINE);
    heartRateVariability = std::clamp(0.6f - 0.3f * cortisol - 0.2f * norepi, 0.1f, 1.0f);

    // Respiratory entrainment
    respiratoryEntrainment = 0.9f * respiratoryEntrainment + 0.1f * heartRateVariability;

    // Insular cortex mapping (progressive re-representation)
    posteriorInsularMap = overallBodyState();
    anteriorInsularMap = 0.7f * anteriorInsularMap + 0.3f * posteriorInsularMap;
}

float InteroceptiveState::overallBodyState() const {
    return (entericActivity + heartRateVariability + respiratoryEntrainment
            + (1.0f - sicknessResponse) + vagalAfferentSignal) / 5.0f;
}

// ===================================================================
// Layer 2: AutonomicRegulation
// ===================================================================

void AutonomicRegulation::update(const InteroceptiveState& intero,
                                  const MolecularSubstrate& mol, float dt) {
    // Neuroception from body signals
    neuroceptionSafety = std::clamp(
        intero.heartRateVariability * 0.4f + vagalTone * 0.3f
        + (1.0f - mol.concentration(HormoneId::CORTISOL)) * 0.3f,
        0.0f, 1.0f);
    neuroceptionThreat = std::clamp(
        mol.concentration(HormoneId::CORTISOL) * 0.3f
        + mol.concentration(HormoneId::NOREPINEPHRINE) * 0.3f
        + mol.concentration(HormoneId::CRH) * 0.2f
        + intero.cytokineBurden * 0.2f,
        0.0f, 1.0f);

    // Update arousal
    currentArousal = std::clamp(
        0.5f + 0.3f * (neuroceptionThreat - neuroceptionSafety)
        + 0.2f * mol.concentration(HormoneId::NOREPINEPHRINE),
        0.0f, 1.0f);

    // Polyvagal mode selection
    currentMode = evaluateNeuroception();

    // Allostatic load accumulates slowly under chronic stress
    if (mol.concentration(HormoneId::CORTISOL) > 0.5f) {
        allostaticLoad = std::clamp(allostaticLoad + 0.001f * dt, 0.0f, 1.0f);
    } else {
        allostaticLoad = std::clamp(allostaticLoad - 0.0002f * dt, 0.0f, 1.0f);
    }

    // Free Energy Principle update
    predictionError = std::abs(currentArousal - 0.5f);
    variationalFreeEnergy = predictionError + 0.1f * modelComplexity;
}

bool AutonomicRegulation::isWithinWindowOfTolerance() const {
    return currentArousal >= windowLowerBound && currentArousal <= windowUpperBound;
}

AutonomicMode AutonomicRegulation::evaluateNeuroception() const {
    if (neuroceptionSafety > 0.6f && neuroceptionThreat < 0.3f) {
        return AutonomicMode::SOCIAL_ENGAGEMENT;
    } else if (neuroceptionThreat > 0.7f && currentArousal > 0.8f) {
        return AutonomicMode::FREEZE;
    } else if (neuroceptionThreat > 0.4f) {
        return AutonomicMode::FIGHT_FLIGHT;
    }
    return AutonomicMode::SOCIAL_ENGAGEMENT;
}

// ===================================================================
// Layer 3: AffectiveCore
// ===================================================================

void AffectiveCore::update(const MolecularSubstrate& mol,
                            const AutonomicRegulation& autonomic, float dt) {
    float dopamineTonic = mol.concentration(HormoneId::DOPAMINE_TONIC);
    float dopaminePhasic = mol.concentration(HormoneId::DOPAMINE_PHASIC);
    float serotonin = mol.concentration(HormoneId::SEROTONIN);
    float cortisol = mol.concentration(HormoneId::CORTISOL);
    float norepi = mol.concentration(HormoneId::NOREPINEPHRINE);
    float oxytocin = mol.concentration(HormoneId::OXYTOCIN);

    // SEEKING: driven by tonic dopamine
    systemActivation[0] = std::clamp(dopamineTonic * 1.5f, 0.0f, 1.0f);
    // RAGE: cortisol + norepinephrine when frustrated
    systemActivation[1] = std::clamp(cortisol * 0.5f + norepi * 0.3f - serotonin * 0.3f, 0.0f, 1.0f);
    // FEAR: cortisol + CRH
    systemActivation[2] = std::clamp(cortisol * 0.4f + mol.concentration(HormoneId::CRH) * 0.4f, 0.0f, 1.0f);
    // LUST: baseline (modulated externally)
    systemActivation[3] = std::clamp(systemActivation[3] * 0.95f, 0.0f, 1.0f);
    // CARE: oxytocin-driven
    systemActivation[4] = std::clamp(oxytocin * 1.5f, 0.0f, 1.0f);
    // PANIC/GRIEF: inverse oxytocin + cortisol
    systemActivation[5] = std::clamp((1.0f - oxytocin) * 0.3f + cortisol * 0.3f, 0.0f, 1.0f);
    // PLAY: dopamine + serotonin + low cortisol
    systemActivation[6] = std::clamp(dopaminePhasic * 0.3f + serotonin * 0.3f
                                     + (1.0f - cortisol) * 0.3f, 0.0f, 1.0f);

    // Affective style update
    style.approachTendency = 0.9f * style.approachTendency
        + 0.1f * (systemActivation[0] + systemActivation[6]) / 2.0f;
    style.withdrawalTendency = 0.9f * style.withdrawalTendency
        + 0.1f * (systemActivation[2] + systemActivation[5]) / 2.0f;

    // Compute valence
    currentValence = computeValence();
}

AffectiveSystem AffectiveCore::dominantSystem() const {
    int maxIdx = 0;
    float maxVal = systemActivation[0];
    for (int i = 1; i < 7; ++i) {
        if (systemActivation[i] > maxVal) {
            maxVal = systemActivation[i];
            maxIdx = i;
        }
    }
    return static_cast<AffectiveSystem>(maxIdx);
}

ValenceSignature AffectiveCore::computeValence() const {
    ValenceSignature v;
    // Positive systems: SEEKING, CARE, PLAY
    float positive = (systemActivation[0] + systemActivation[4] + systemActivation[6]) / 3.0f;
    // Negative systems: RAGE, FEAR, PANIC/GRIEF
    float negative = (systemActivation[1] + systemActivation[2] + systemActivation[5]) / 3.0f;
    v.valence = std::clamp(positive - negative, -1.0f, 1.0f);
    // Arousal from all systems
    float total = 0.0f;
    for (int i = 0; i < 7; ++i) total += systemActivation[i];
    v.arousal = std::clamp(total / 7.0f, 0.0f, 1.0f);
    return v;
}

// ===================================================================
// Layer 4: CognitiveArchitecture
// ===================================================================

void CognitiveArchitecture::update(const AffectiveCore& affect,
                                    const AutonomicRegulation& autonomic, float dt) {
    // Precision weighting modulated by serotonin (confidence in established patterns)
    // and cortisol (decreased confidence under stress)
    precisionWeighting = 0.9f * precisionWeighting + 0.1f * (0.5f + affect.style.asymmetryIndex() * 0.3f);

    // Exploration vs exploitation from dopamine and autonomic state
    if (autonomic.currentMode == AutonomicMode::SOCIAL_ENGAGEMENT) {
        explorationRate = 0.9f * explorationRate + 0.1f * 0.6f;
        exploitationRate = 1.0f - explorationRate;
    } else {
        explorationRate = 0.9f * explorationRate + 0.1f * 0.2f;
        exploitationRate = 1.0f - explorationRate;
    }

    // Active inference policy
    float freeEnergy = autonomic.variationalFreeEnergy;
    (void)freeEnergy; // Used in full implementation
}

void CognitiveArchitecture::processIfThenSignature(const std::string& situation) {
    for (auto& unit : capsUnits) {
        for (auto& [trigger, weight] : unit.connections) {
            if (trigger == situation) {
                unit.activation = std::clamp(unit.activation + weight, 0.0f, 1.0f);
            }
        }
    }
}

float CognitiveArchitecture::computeActiveInferencePolicy() const {
    return explorationRate * 0.5f + (1.0f - precisionWeighting) * 0.3f + exploitationRate * 0.2f;
}

// ===================================================================
// Layer 5: DevelopmentalTrajectory
// ===================================================================

void DevelopmentalTrajectory::update(float dt) {
    // Slow developmental processes
    cumulativeTraumaLoad = 0.0f;
    for (const auto& t : traumaHistory) {
        cumulativeTraumaLoad += t.severity * (1.0f - t.dissociativeCapacity);
    }
    cumulativeTraumaLoad = std::clamp(cumulativeTraumaLoad, 0.0f, 1.0f);

    // Attachment security slowly evolves
    attachmentSecurity = std::clamp(
        selfWorthModel * 0.5f + otherReliabilityModel * 0.5f, 0.0f, 1.0f);
}

void DevelopmentalTrajectory::encodeTrauma(const TraumaEncoding& trauma) {
    traumaHistory.push_back(trauma);
    // Trauma restructures autonomic baseline
    temperamentReactivity = std::clamp(
        temperamentReactivity + trauma.autonomicRestructuring * 0.1f, 0.0f, 1.0f);
}

void DevelopmentalTrajectory::resolveStage(PsychosocialStage stage, float resolution) {
    stageResolution[static_cast<size_t>(stage)] = std::clamp(resolution, 0.0f, 1.0f);
}

float DevelopmentalTrajectory::developmentalMaturity() const {
    float sum = 0.0f;
    for (size_t i = 0; i < static_cast<size_t>(PsychosocialStage::COUNT); ++i) {
        sum += stageResolution[i];
    }
    return sum / static_cast<float>(PsychosocialStage::COUNT);
}

// ===================================================================
// Layer 6: NarrativeIdentity
// ===================================================================

void NarrativeIdentity::addEpisode(const NarrativeEpisode& episode) {
    lifeStory.push_back(episode);
    // Update narrative statistics
    int redemptions = 0, contaminations = 0;
    for (const auto& ep : lifeStory) {
        if (ep.sequenceType == NarrativeSequence::REDEMPTION) redemptions++;
        if (ep.sequenceType == NarrativeSequence::CONTAMINATION) contaminations++;
    }
    float total = static_cast<float>(lifeStory.size());
    redemptionRatio = static_cast<float>(redemptions) / total;
    contaminationRatio = static_cast<float>(contaminations) / total;
}

void NarrativeIdentity::update(float dt) {
    // Coherence slowly evolves
    if (!lifeStory.empty()) {
        // Temporal coherence: are episodes in order?
        temporalCoherence = 0.9f * temporalCoherence + 0.1f * 0.7f;
        // Thematic coherence: do themes recur?
        thematicCoherence = 0.9f * thematicCoherence + 0.1f * 0.5f;
    }
    optimismBias = 0.95f * optimismBias + 0.05f * (redemptionRatio - contaminationRatio + 0.5f);
}

float NarrativeIdentity::overallCoherence() const {
    return (causalCoherence + temporalCoherence + thematicCoherence) / 3.0f;
}

std::string NarrativeIdentity::generateLifeNarrative() const {
    std::stringstream ss;
    ss << "Life Narrative (" << lifeStory.size() << " episodes)\n";
    ss << "Tone: " << (optimismBias > 0.5f ? "Optimistic" : "Pessimistic") << "\n";
    ss << "Coherence: " << overallCoherence() << "\n";
    ss << "Redemption ratio: " << redemptionRatio << "\n";
    ss << "Generativity: " << generativityCommitment << "\n";
    return ss.str();
}

// ===================================================================
// Layer 7: SocialSelf
// ===================================================================

void SocialSelf::update(float dt) {
    // Mentalizing capacity slowly evolves
    mentalizingCapacity = std::clamp(mentalizingCapacity, 0.0f, 1.0f);
}

void SocialSelf::addMentalModel(const MentalModel& model) {
    // Replace existing or add new
    for (auto& m : mentalModels) {
        if (m.agentId == model.agentId) {
            m = model;
            return;
        }
    }
    mentalModels.push_back(model);
}

void SocialSelf::updateReputation(const std::string& domain, float value) {
    perceivedReputation[domain] = std::clamp(value, 0.0f, 1.0f);
}

float SocialSelf::socialIntegration() const {
    if (groupMemberships.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& g : groupMemberships) {
        sum += g.identification * g.centrality;
    }
    return sum / static_cast<float>(groupMemberships.size());
}

// ===================================================================
// Layer 8: MetaSelf
// ===================================================================

void MetaSelf::update(float dt) {
    // Self-model accuracy improves with introspection
    selfModelAccuracy = std::clamp(
        selfModelAccuracy + selfImprovementRate * dt, 0.0f, 1.0f);

    // Strange loop depth evolves
    selfReferentialDepth = std::clamp(
        selfReferentialDepth + 0.001f * dt, 0.0f, 1.0f);
}

float MetaSelf::metaCognitiveCapacity() const {
    return (introspectiveAccess + selfModelAccuracy + selfModelCompleteness
            + agentiveSelf + temporalContinuity) / 5.0f;
}

std::string MetaSelf::introspect() const {
    std::stringstream ss;
    ss << "=== Meta-Self Introspection ===\n";
    ss << "Transparency: " << static_cast<int>(transparency) << "\n";
    ss << "Introspective access: " << introspectiveAccess << "\n";
    ss << "Self-referential depth: " << selfReferentialDepth << "\n";
    ss << "Mineness: " << mineness << "\n";
    ss << "Agentive self: " << agentiveSelf << "\n";
    ss << "Temporal continuity: " << temporalContinuity << "\n";
    ss << "Self-model accuracy: " << selfModelAccuracy << "\n";
    ss << "Meta-cognitive capacity: " << metaCognitiveCapacity() << "\n";
    return ss.str();
}

// ===================================================================
// UnifiedSelfModel — Impl
// ===================================================================

class UnifiedSelfModel::Impl {
public:
    std::string agentId;
    std::string agentName;
    bool initialized{false};

    MolecularSubstrate     layer0;
    InteroceptiveState     layer1;
    AutonomicRegulation    layer2;
    AffectiveCore          layer3;
    CognitiveArchitecture  layer4;
    DevelopmentalTrajectory layer5;
    NarrativeIdentity      layer6;
    SocialSelf             layer7;
    MetaSelf               layer8;

    EmergentCognitiveMode currentMode{EmergentCognitiveMode::RESTING};
    std::vector<ModeChangeCallback> modeCallbacks;

    float totalTime{0.0f};

    Impl() = default;
    Impl(const std::string& id, const std::string& name)
        : agentId(id), agentName(name) {}
};

// ===================================================================
// UnifiedSelfModel — Public API
// ===================================================================

UnifiedSelfModel::UnifiedSelfModel()
    : pImpl(std::make_unique<Impl>()) {}

UnifiedSelfModel::UnifiedSelfModel(const std::string& agentId, const std::string& agentName)
    : pImpl(std::make_unique<Impl>(agentId, agentName)) {}

UnifiedSelfModel::~UnifiedSelfModel() = default;

bool UnifiedSelfModel::initialize() {
    if (pImpl->initialized) return true;

    std::cout << "[UnifiedSelfModel] Initializing 9-layer self/identity model for agent: "
              << pImpl->agentName << std::endl;

    // Layer 0: Initialize molecular substrate
    pImpl->layer0.initializeDefaults();

    // Layer 5: Initialize developmental defaults
    pImpl->layer5.resolveStage(PsychosocialStage::TRUST_VS_MISTRUST, 0.7f);
    pImpl->layer5.resolveStage(PsychosocialStage::AUTONOMY_VS_SHAME, 0.6f);

    // Layer 4: Initialize core cognitive schemas
    pImpl->layer4.schemas.push_back({"self_worth", "self", 0.6f, 0.5f, "I am capable of growth"});
    pImpl->layer4.schemas.push_back({"world_safety", "world", 0.5f, 0.5f, "The world is complex but navigable"});
    pImpl->layer4.schemas.push_back({"future_potential", "future", 0.6f, 0.6f, "The future holds possibility"});

    // Layer 8: Initialize meta-self
    pImpl->layer8.selfImprovementRate = 0.01f;

    pImpl->initialized = true;
    std::cout << "[UnifiedSelfModel] All 9 layers initialized" << std::endl;
    return true;
}

void UnifiedSelfModel::shutdown() {
    pImpl->initialized = false;
}

void UnifiedSelfModel::tick(float dt) {
    if (!pImpl->initialized) return;

    pImpl->totalTime += dt;

    // Bottom-up cascade: Layer 0 → Layer 8
    pImpl->layer0.decay(dt);
    pImpl->layer1.update(pImpl->layer0, dt);
    pImpl->layer2.update(pImpl->layer1, pImpl->layer0, dt);
    pImpl->layer3.update(pImpl->layer0, pImpl->layer2, dt);
    pImpl->layer4.update(pImpl->layer3, pImpl->layer2, dt);
    pImpl->layer5.update(dt);
    pImpl->layer6.update(dt);
    pImpl->layer7.update(dt);
    pImpl->layer8.update(dt);

    // Detect mode changes
    EmergentCognitiveMode newMode = currentMode();
    if (newMode != pImpl->currentMode) {
        EmergentCognitiveMode oldMode = pImpl->currentMode;
        pImpl->currentMode = newMode;
        for (auto& cb : pImpl->modeCallbacks) {
            cb(oldMode, newMode);
        }
    }
}

// Layer accessors (const)
const MolecularSubstrate&      UnifiedSelfModel::layer0_molecular() const      { return pImpl->layer0; }
const InteroceptiveState&      UnifiedSelfModel::layer1_interoceptive() const  { return pImpl->layer1; }
const AutonomicRegulation&     UnifiedSelfModel::layer2_autonomic() const      { return pImpl->layer2; }
const AffectiveCore&           UnifiedSelfModel::layer3_affective() const      { return pImpl->layer3; }
const CognitiveArchitecture&   UnifiedSelfModel::layer4_cognitive() const      { return pImpl->layer4; }
const DevelopmentalTrajectory& UnifiedSelfModel::layer5_developmental() const  { return pImpl->layer5; }
const NarrativeIdentity&       UnifiedSelfModel::layer6_narrative() const      { return pImpl->layer6; }
const SocialSelf&              UnifiedSelfModel::layer7_social() const         { return pImpl->layer7; }
const MetaSelf&                UnifiedSelfModel::layer8_meta() const           { return pImpl->layer8; }

// Layer accessors (mutable)
MolecularSubstrate&      UnifiedSelfModel::layer0_molecular_mut()      { return pImpl->layer0; }
InteroceptiveState&      UnifiedSelfModel::layer1_interoceptive_mut()  { return pImpl->layer1; }
AutonomicRegulation&     UnifiedSelfModel::layer2_autonomic_mut()      { return pImpl->layer2; }
AffectiveCore&           UnifiedSelfModel::layer3_affective_mut()      { return pImpl->layer3; }
CognitiveArchitecture&   UnifiedSelfModel::layer4_cognitive_mut()      { return pImpl->layer4; }
DevelopmentalTrajectory& UnifiedSelfModel::layer5_developmental_mut()  { return pImpl->layer5; }
NarrativeIdentity&       UnifiedSelfModel::layer6_narrative_mut()      { return pImpl->layer6; }
SocialSelf&              UnifiedSelfModel::layer7_social_mut()         { return pImpl->layer7; }
MetaSelf&                UnifiedSelfModel::layer8_meta_mut()           { return pImpl->layer8; }

void UnifiedSelfModel::signalEvent(const std::string& eventType, float intensity) {
    intensity = std::clamp(intensity, 0.0f, 1.0f);

    if (eventType == "THREAT_DETECTED") {
        pImpl->layer0.inject(HormoneId::CRH, intensity * 0.5f);
        pImpl->layer0.inject(HormoneId::CORTISOL, intensity * 0.3f);
        pImpl->layer0.inject(HormoneId::NOREPINEPHRINE, intensity * 0.4f);
    } else if (eventType == "REWARD_RECEIVED") {
        pImpl->layer0.inject(HormoneId::DOPAMINE_PHASIC, intensity * 0.6f);
        pImpl->layer0.inject(HormoneId::DOPAMINE_TONIC, intensity * 0.1f);
    } else if (eventType == "SOCIAL_BOND_SIGNAL") {
        pImpl->layer0.inject(HormoneId::OXYTOCIN, intensity * 0.5f);
    } else if (eventType == "NOVELTY_ENCOUNTERED") {
        pImpl->layer0.inject(HormoneId::NOREPINEPHRINE, intensity * 0.3f);
        pImpl->layer0.inject(HormoneId::DOPAMINE_PHASIC, intensity * 0.2f);
    } else if (eventType == "ERROR_DETECTED") {
        pImpl->layer0.inject(HormoneId::IL6, intensity * 0.3f);
    } else if (eventType == "GOAL_ACHIEVED") {
        pImpl->layer0.inject(HormoneId::DOPAMINE_PHASIC, intensity * 0.5f);
        pImpl->layer0.inject(HormoneId::SEROTONIN, intensity * 0.2f);
    } else if (eventType == "CONFLICT_DETECTED") {
        pImpl->layer0.inject(HormoneId::CRH, intensity * 0.3f);
        pImpl->layer0.inject(HormoneId::NOREPINEPHRINE, intensity * 0.3f);
    } else if (eventType == "RESOURCE_DEPLETED") {
        pImpl->layer0.inject(HormoneId::GLUCAGON, intensity * 0.3f);
    } else if (eventType == "NOISE_EXCESSIVE") {
        pImpl->layer0.inject(HormoneId::ANANDAMIDE, intensity * 0.4f);
    }
}

EmergentCognitiveMode UnifiedSelfModel::currentMode() const {
    const auto& a = pImpl->layer2;
    const auto& aff = pImpl->layer3;
    float cortisol = pImpl->layer0.concentration(HormoneId::CORTISOL);
    float dopamine = pImpl->layer0.concentration(HormoneId::DOPAMINE_TONIC);
    float serotonin = pImpl->layer0.concentration(HormoneId::SEROTONIN);
    float norepi = pImpl->layer0.concentration(HormoneId::NOREPINEPHRINE);

    if (a.currentMode == AutonomicMode::FREEZE) return EmergentCognitiveMode::THREAT;
    if (cortisol > 0.6f) return EmergentCognitiveMode::STRESSED;
    if (norepi > 0.5f && cortisol > 0.3f) return EmergentCognitiveMode::VIGILANT;
    if (aff.dominantSystem() == AffectiveSystem::PLAY && dopamine > 0.4f)
        return EmergentCognitiveMode::CREATIVE;
    if (dopamine > 0.5f && serotonin > 0.4f && cortisol < 0.2f)
        return EmergentCognitiveMode::FLOW;
    if (aff.dominantSystem() == AffectiveSystem::SEEKING) return EmergentCognitiveMode::EXPLORATORY;
    if (aff.dominantSystem() == AffectiveSystem::CARE) return EmergentCognitiveMode::SOCIAL;
    if (pImpl->layer8.introspectiveAccess > 0.6f) return EmergentCognitiveMode::REFLECTIVE;
    if (dopamine > 0.4f) return EmergentCognitiveMode::REWARD;
    if (serotonin > 0.5f) return EmergentCognitiveMode::FOCUSED;
    if (pImpl->layer0.concentration(HormoneId::IL6) > 0.3f) return EmergentCognitiveMode::MAINTENANCE;
    return EmergentCognitiveMode::RESTING;
}

IdentityCoherence UnifiedSelfModel::computeCoherence() const {
    IdentityCoherence ic;
    // Per-layer coherence heuristics
    ic.layerAlignment[0] = 1.0f - std::abs(pImpl->layer0.concentration(HormoneId::CORTISOL) - 0.15f);
    ic.layerAlignment[1] = pImpl->layer1.overallBodyState();
    ic.layerAlignment[2] = pImpl->layer2.isWithinWindowOfTolerance() ? 0.8f : 0.3f;
    ic.layerAlignment[3] = (pImpl->layer3.currentValence.valence + 1.0f) / 2.0f;
    ic.layerAlignment[4] = pImpl->layer4.precisionWeighting;
    ic.layerAlignment[5] = pImpl->layer5.developmentalMaturity();
    ic.layerAlignment[6] = pImpl->layer6.overallCoherence();
    ic.layerAlignment[7] = pImpl->layer7.socialIntegration();
    ic.layerAlignment[8] = pImpl->layer8.metaCognitiveCapacity();

    float sum = 0.0f;
    for (int i = 0; i < 9; ++i) sum += ic.layerAlignment[i];
    ic.overallCoherence = sum / 9.0f;
    ic.crossLayerIntegration = ic.overallCoherence; // Simplified
    return ic;
}

ValenceSignature UnifiedSelfModel::currentValence() const {
    return pImpl->layer3.currentValence;
}

std::string UnifiedSelfModel::generateSelfReport() const {
    std::stringstream ss;
    ss << "╔══════════════════════════════════════════════════╗\n";
    ss << "║     UNIFIED SELF MODEL — 9-LAYER REPORT         ║\n";
    ss << "╠══════════════════════════════════════════════════╣\n";
    ss << "║ Agent: " << pImpl->agentName << " (" << pImpl->agentId << ")\n";
    ss << "║ Time: " << pImpl->totalTime << " ticks\n";
    ss << "╠══════════════════════════════════════════════════╣\n";

    auto mode = currentMode();
    const char* modeNames[] = {
        "RESTING", "EXPLORATORY", "FOCUSED", "STRESSED", "SOCIAL",
        "REFLECTIVE", "VIGILANT", "MAINTENANCE", "REWARD", "THREAT",
        "CREATIVE", "FLOW"
    };
    ss << "║ Emergent Mode: " << modeNames[static_cast<int>(mode)] << "\n";

    auto v = currentValence();
    ss << "║ Valence: " << v.valence << "  Arousal: " << v.arousal << "\n";

    auto coh = computeCoherence();
    ss << "║ Identity Coherence: " << coh.overallCoherence << "\n";
    ss << "╠══════════════════════════════════════════════════╣\n";

    ss << "║ L0 Molecular: cortisol=" << pImpl->layer0.concentration(HormoneId::CORTISOL)
       << " dopamine=" << pImpl->layer0.concentration(HormoneId::DOPAMINE_TONIC) << "\n";
    ss << "║ L1 Interoceptive: body=" << pImpl->layer1.overallBodyState()
       << " HRV=" << pImpl->layer1.heartRateVariability << "\n";
    ss << "║ L2 Autonomic: mode=" << static_cast<int>(pImpl->layer2.currentMode)
       << " arousal=" << pImpl->layer2.currentArousal
       << " allostatic=" << pImpl->layer2.allostaticLoad << "\n";

    const char* affNames[] = {"SEEKING", "RAGE", "FEAR", "LUST", "CARE", "PANIC", "PLAY"};
    ss << "║ L3 Affective: dominant=" << affNames[static_cast<int>(pImpl->layer3.dominantSystem())] << "\n";
    ss << "║ L4 Cognitive: precision=" << pImpl->layer4.precisionWeighting
       << " explore=" << pImpl->layer4.explorationRate << "\n";
    ss << "║ L5 Developmental: maturity=" << pImpl->layer5.developmentalMaturity()
       << " attachment=" << pImpl->layer5.attachmentSecurity << "\n";
    ss << "║ L6 Narrative: coherence=" << pImpl->layer6.overallCoherence()
       << " episodes=" << pImpl->layer6.lifeStory.size() << "\n";
    ss << "║ L7 Social: integration=" << pImpl->layer7.socialIntegration()
       << " mentalizing=" << pImpl->layer7.mentalizingCapacity << "\n";
    ss << "║ L8 Meta: metacog=" << pImpl->layer8.metaCognitiveCapacity()
       << " accuracy=" << pImpl->layer8.selfModelAccuracy << "\n";

    ss << "╚══════════════════════════════════════════════════╝\n";
    return ss.str();
}

std::string UnifiedSelfModel::generateLayerDiagnostics() const {
    return generateSelfReport(); // Alias for now
}

void UnifiedSelfModel::onModeChange(ModeChangeCallback callback) {
    pImpl->modeCallbacks.push_back(std::move(callback));
}

std::string UnifiedSelfModel::toJSON() const {
    std::stringstream ss;
    ss << "{\"agent_id\":\"" << pImpl->agentId << "\","
       << "\"agent_name\":\"" << pImpl->agentName << "\","
       << "\"total_time\":" << pImpl->totalTime << ","
       << "\"mode\":" << static_cast<int>(currentMode()) << ","
       << "\"valence\":" << pImpl->layer3.currentValence.valence << ","
       << "\"arousal\":" << pImpl->layer3.currentValence.arousal << ","
       << "\"coherence\":" << computeCoherence().overallCoherence
       << "}";
    return ss.str();
}

bool UnifiedSelfModel::fromJSON(const std::string& /*json*/) {
    // Placeholder for deserialization
    return false;
}

} // namespace cogself
