/**
 * CognitiveDistrict — A region of the Cognitive City
 *
 * Each district wraps one cognitive subsystem (PLN, ECAN, VES, etc.)
 * with an Active Inference boundary (Markov blanket), computing local
 * free energy and contributing to city-wide coherence.
 *
 * Districts:
 *   1. Temperament District (Cloninger gains)
 *   2. Interoceptive District (body-state mapping)
 *   3. Developmental District (trajectory/trauma)
 *   4. Narrative District (life story)
 *   5. Social District (roles/attachment)
 *   6. Cognitive District (PLN/ECAN/PatternMatcher)
 *   7. Temporal District (TCS/CrystalBus)
 *   8. Endocrine District (VES/glands)
 *   9. Neural District (VNS/NPU)
 *
 * Part of the Reactor Core: arc-halo => cyc-phoenix
 * Copyright 2025-2026 OpenCog Community. AGPL-3.0.
 */
#ifndef OPENCOG_AFI_DISTRICT_HPP
#define OPENCOG_AFI_DISTRICT_HPP

#include "opencog/afi/types.hpp"
#include "opencog/afi/active_inference.hpp"
#include <string>
#include <memory>

namespace opencog::afi {

enum class DistrictId : uint8_t {
    TEMPERAMENT   = 0,
    INTEROCEPTIVE = 1,
    DEVELOPMENTAL = 2,
    NARRATIVE     = 3,
    SOCIAL        = 4,
    COGNITIVE     = 5,
    TEMPORAL      = 6,
    ENDOCRINE     = 7,
    NEURAL        = 8,
    COUNT         = 9,
};

class CognitiveDistrict {
public:
    CognitiveDistrict() = default;
    CognitiveDistrict(DistrictId id, std::string name)
        : id_(id), name_(std::move(name)) {}

    /// Update the district's free energy from its subsystem state
    void update(float dt) noexcept;

    /// Feed observations from the subsystem
    void observe(const std::vector<float>& observations) noexcept;

    /// Get the district's current free energy
    [[nodiscard]] const FreeEnergy& free_energy() const noexcept;

    /// Get district metrics
    [[nodiscard]] DistrictMetrics metrics() const noexcept;

    /// Accessors
    [[nodiscard]] DistrictId id() const noexcept { return id_; }
    [[nodiscard]] const std::string& name() const noexcept { return name_; }
    [[nodiscard]] ActiveInferenceEngine& engine() noexcept { return engine_; }
    [[nodiscard]] const ActiveInferenceEngine& engine() const noexcept { return engine_; }

private:
    DistrictId id_{DistrictId::TEMPERAMENT};
    std::string name_;
    ActiveInferenceEngine engine_;
};

} // namespace opencog::afi

#endif // OPENCOG_AFI_DISTRICT_HPP
