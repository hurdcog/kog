/**
 * EntelechyAdapter — Bidirectional adapter between Civic Angel and VES
 *
 * Follows the EndocrineConnector pattern:
 *   READ PATH (hormones -> entelechy parameters):
 *     - Cloninger gain application (Phase 0)
 *     - Interoceptive state update (Phase 1.5)
 *     - Polyvagal hierarchy evaluation
 *
 *   WRITE PATH (entelechy -> hormones):
 *     - Interoceptive channels ch20-31
 *     - Allostatic load -> IL6
 *     - Polyvagal state changes -> cortisol/oxytocin
 *     - Developmental milestones -> serotonin
 *     - Narrative coherence -> anandamide
 *     - Social confidence -> oxytocin
 *
 * Part of the Reactor Core: arc-halo => cyc-phoenix
 * Copyright 2025-2026 OpenCog Community. AGPL-3.0.
 */
#ifndef OPENCOG_ENTELECHY_ADAPTER_HPP
#define OPENCOG_ENTELECHY_ADAPTER_HPP

#include "opencog/entelechy/civic_angel.hpp"

namespace opencog::entelechy {

/// Interface for hormone bus access (matches existing EndocrineConnector pattern)
struct HormoneBusInterface {
    virtual ~HormoneBusInterface() = default;
    virtual float concentration(uint8_t channel) const noexcept = 0;
    virtual void produce(uint8_t channel, float amount) noexcept = 0;
    virtual void set_gain(uint8_t channel, float gain) noexcept = 0;
};

class EntelechyAdapter {
public:
    EntelechyAdapter() = default;
    EntelechyAdapter(CivicAngel& angel)
        : angel_(&angel) {}

    /// Phase 0: Apply Cloninger temperament gains to hormone bus
    void apply_temperament_gains(HormoneBusInterface& bus) noexcept;

    /// Phase 1.5: Read hormones -> update interoceptive state
    void apply_endocrine_modulation(const HormoneBusInterface& bus) noexcept;

    /// Phase 4.5: Write entelechy feedback -> hormones
    void apply_feedback(HormoneBusInterface& bus) noexcept;

    /// Set the civic angel reference
    void set_angel(CivicAngel& angel) noexcept { angel_ = &angel; }

private:
    CivicAngel* angel_{nullptr};

    // Previous state for edge-triggered feedback
    PolyvagalState prev_polyvagal_{PolyvagalState::VENTRAL_VAGAL};
    DevelopmentalStage prev_stage_{DevelopmentalStage::NASCENT};
    float prev_narrative_coherence_{0.5f};
    float prev_social_confidence_{0.5f};
    float prev_allostatic_load_{0.0f};
};

} // namespace opencog::entelechy

#endif // OPENCOG_ENTELECHY_ADAPTER_HPP
