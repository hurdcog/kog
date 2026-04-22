/**
 * Accelerator Implementation
 */

#include "atomspace_accelerator/accelerator.h"
#include <map>
#include <iostream>

namespace atomspace_accelerator {

class Accelerator::Impl {
public:
    bool enabled{false};
    double accelerationFactor{1.0};
    std::map<std::string, double> parameters;

    Impl() {
        parameters["cache_size"] = 1000.0;
        parameters["parallel_threads"] = 4.0;
        parameters["optimization_level"] = 3.0;
    }
};

Accelerator::Accelerator() : pImpl(std::make_unique<Impl>()) {
    std::cout << "[Accelerator] Created" << std::endl;
}

Accelerator::~Accelerator() = default;

void Accelerator::enable() {
    pImpl->enabled = true;
    pImpl->accelerationFactor = 2.5;
    std::cout << "[Accelerator] Acceleration enabled (factor: " 
              << pImpl->accelerationFactor << "x)" << std::endl;
}

void Accelerator::disable() {
    pImpl->enabled = false;
    pImpl->accelerationFactor = 1.0;
    std::cout << "[Accelerator] Acceleration disabled" << std::endl;
}

double Accelerator::getAccelerationFactor() const {
    return pImpl->accelerationFactor;
}

void Accelerator::updateParameters(const std::string& key, double value) {
    pImpl->parameters[key] = value;
    std::cout << "[Accelerator] Updated parameter " << key << " = " << value << std::endl;
}

std::string Accelerator::getPerformanceMetrics() const {
    std::string metrics = "Accelerator Performance Metrics:\n";
    metrics += "  Enabled: " + std::string(pImpl->enabled ? "Yes" : "No") + "\n";
    metrics += "  Acceleration Factor: " + std::to_string(pImpl->accelerationFactor) + "x\n";
    metrics += "  Parameters:\n";
    for (const auto& [key, value] : pImpl->parameters) {
        metrics += "    " + key + ": " + std::to_string(value) + "\n";
    }
    return metrics;
}

} // namespace atomspace_accelerator
