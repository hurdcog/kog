/**
 * CognitiveDistrict — Implementation
 */
#include "opencog/afi/district.hpp"

namespace opencog::afi {

void CognitiveDistrict::update(float dt) noexcept {
    engine_.update(dt);
}

void CognitiveDistrict::observe(const std::vector<float>& observations) noexcept {
    engine_.observe(observations);
}

const FreeEnergy& CognitiveDistrict::free_energy() const noexcept {
    return engine_.current_free_energy();
}

DistrictMetrics CognitiveDistrict::metrics() const noexcept {
    const auto& fe = engine_.current_free_energy();
    DistrictMetrics m;
    m.free_energy = fe.total;
    m.surprise = fe.surprise();
    m.complexity = fe.complexity;
    m.accuracy = fe.accuracy;
    // Coherence: inverse of free energy (lower FE = higher coherence)
    m.coherence = 1.0f / (1.0f + fe.total);
    return m;
}

} // namespace opencog::afi
