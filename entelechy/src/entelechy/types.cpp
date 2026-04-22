/**
 * Ontogenetic Entelechy — Types compilation unit
 */
#include "opencog/entelechy/types.hpp"

// Thin compilation unit — types are header-only structs/enums.
// This file ensures the header compiles cleanly and provides
// a translation unit for any future non-inline implementations.

namespace opencog::entelechy {

// Type registration table (for AtomSpace integration)
static const char* const ENTELECHY_TYPE_NAMES[] = {
    "TemperamentNode",       // 10300
    "CharacterNode",         // 10301
    "InteroceptiveNode",     // 10302
    "DevelopmentalNode",     // 10303
    "NarrativeNode",         // 10304
    "SocialRoleNode",        // 10305
    "AttachmentNode",        // 10306
    "DistrictNode",          // 10307
    "CivicAngelNode",        // 10308
    "EntelechyNode",         // 10309
    "CriticalPeriodNode",    // 10310
    "TraumaNode",            // 10311
    "LifeThemeNode",         // 10312
    "PolyvagalStateNode",    // 10313
    "AllostaticNode",        // 10314
    "SelfModelNode",         // 10315
};

static const char* const AFI_TYPE_NAMES[] = {
    "MarkovBlanketNode",     // 10320
    "GenerativeModelNode",   // 10321
    "FreeEnergyNode",        // 10322
    "PrecisionNode",         // 10323
    "PredictionErrorNode",   // 10324
};

const char* entelechy_type_name(EntelechyAtomType type) {
    auto idx = static_cast<uint16_t>(type);
    if (idx >= 10300 && idx <= 10315) {
        return ENTELECHY_TYPE_NAMES[idx - 10300];
    }
    if (idx >= 10320 && idx <= 10324) {
        return AFI_TYPE_NAMES[idx - 10320];
    }
    return "UnknownEntelechyType";
}

} // namespace opencog::entelechy
