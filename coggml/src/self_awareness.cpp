/**
 * Self-Awareness Module Implementation
 */

#include "coggml/self_awareness.h"
#include <sstream>
#include <iostream>

namespace coggml {

class SelfAwareness::Impl {
public:
    std::map<std::string, std::string> states;
    size_t reflectionCount{0};

    Impl() {
        states["mode"] = "learning";
        states["confidence"] = "0.0";
        states["performance"] = "0.0";
    }
};

SelfAwareness::SelfAwareness() : pImpl(std::make_unique<Impl>()) {
    std::cout << "[SelfAwareness] Module initialized" << std::endl;
}

SelfAwareness::~SelfAwareness() = default;

void SelfAwareness::updateState(const std::string& key, const std::string& value) {
    pImpl->states[key] = value;
    std::cout << "[SelfAwareness] State updated: " << key << " = " << value << std::endl;
}

std::string SelfAwareness::getState(const std::string& key) const {
    auto it = pImpl->states.find(key);
    if (it != pImpl->states.end()) {
        return it->second;
    }
    return "";
}

std::map<std::string, std::string> SelfAwareness::getAllStates() const {
    return pImpl->states;
}

std::string SelfAwareness::reflect() const {
    pImpl->reflectionCount++;
    
    std::stringstream ss;
    ss << "Self-Reflection #" << pImpl->reflectionCount << ":\n";
    ss << "Current cognitive state:\n";
    
    for (const auto& [key, value] : pImpl->states) {
        ss << "  " << key << ": " << value << "\n";
    }
    
    return ss.str();
}

double SelfAwareness::assessPerformance() const {
    // Simple performance assessment based on state
    try {
        double perf = std::stod(getState("performance"));
        return perf;
    } catch (...) {
        return 0.0;
    }
}

std::string SelfAwareness::suggestImprovements() const {
    std::stringstream ss;
    ss << "Self-Improvement Suggestions:\n";
    
    double performance = assessPerformance();
    if (performance < 0.5) {
        ss << "  - Increase learning rate\n";
        ss << "  - Enhance pattern recognition\n";
    }
    
    ss << "  - Optimize cognitive synergy pathways\n";
    ss << "  - Expand knowledge integration\n";
    
    return ss.str();
}

} // namespace coggml
