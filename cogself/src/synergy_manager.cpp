/**
 * Synergy Manager Implementation
 */

#include "cogself/synergy_manager.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <cmath>

namespace cogself {

// Component interaction strength
struct ComponentInteraction {
    std::string component1;
    std::string component2;
    double interactionStrength{0.0};
    size_t interactionCount{0};
    
    void recordInteraction(double strength) {
        interactionStrength = (interactionStrength * interactionCount + strength) / (interactionCount + 1);
        interactionCount++;
    }
};

class SynergyManager::Impl {
public:
    std::vector<std::string> components;
    double baselineSynergy{0.1};
    size_t optimizationCount{0};
    
    // Enhanced synergy tracking
    std::unordered_map<std::string, double> componentActivation;
    std::vector<ComponentInteraction> interactions;
    
    // Synergy optimization history
    std::vector<double> synergyHistory;
    static constexpr size_t MAX_HISTORY = 100;
    
    // Synergy enhancement factors
    double diversityBonus{0.0};
    double integrationBonus{0.0};
    double adaptivityScore{0.0};
    
    void recordSynergyLevel(double level) {
        synergyHistory.push_back(level);
        if (synergyHistory.size() > MAX_HISTORY) {
            synergyHistory.erase(synergyHistory.begin());
        }
        
        // Calculate adaptivity (rate of improvement)
        if (synergyHistory.size() >= 2) {
            double recent = synergyHistory.back();
            double previous = synergyHistory[synergyHistory.size() - 2];
            adaptivityScore = (recent - previous) / std::max(previous, 0.01);
        }
    }
    
    void updateInteractions() {
        // Update pairwise component interactions
        for (size_t i = 0; i < components.size(); ++i) {
            for (size_t j = i + 1; j < components.size(); ++j) {
                bool found = false;
                for (auto& interaction : interactions) {
                    if ((interaction.component1 == components[i] && interaction.component2 == components[j]) ||
                        (interaction.component1 == components[j] && interaction.component2 == components[i])) {
                        // Strengthen existing interaction
                        double strength = 0.1 + (componentActivation[components[i]] + componentActivation[components[j]]) / 2.0;
                        interaction.recordInteraction(strength);
                        found = true;
                        break;
                    }
                }
                
                if (!found) {
                    // Create new interaction
                    ComponentInteraction newInteraction;
                    newInteraction.component1 = components[i];
                    newInteraction.component2 = components[j];
                    newInteraction.recordInteraction(0.1);
                    interactions.push_back(newInteraction);
                }
            }
        }
    }
    
    double calculateDiversityBonus() const {
        // More diverse components create richer cognitive synergy
        if (components.size() <= 1) return 0.0;
        
        // Normalize activation values to probabilities
        double totalActivation = 0.0;
        for (const auto& [comp, activation] : componentActivation) {
            totalActivation += activation;
        }
        
        if (totalActivation == 0.0) return 0.0;
        
        // Shannon diversity index adapted for cognitive components
        double diversity = 0.0;
        for (const auto& [comp, activation] : componentActivation) {
            if (activation > 0 && totalActivation > 0) {
                double probability = activation / totalActivation;
                diversity -= probability * std::log2(probability);
            }
        }
        
        return std::min(diversity * 0.1, 0.3); // Cap at 0.3
    }
    
    double calculateIntegrationBonus() const {
        // Integration bonus based on interaction strength
        if (interactions.empty()) return 0.0;
        
        double totalStrength = 0.0;
        for (const auto& interaction : interactions) {
            totalStrength += interaction.interactionStrength;
        }
        
        double avgStrength = totalStrength / interactions.size();
        return std::min(avgStrength * 0.5, 0.4); // Cap at 0.4
    }
};

SynergyManager::SynergyManager() : pImpl(std::make_unique<Impl>()) {
    std::cout << "[SynergyManager] Initialized" << std::endl;
}

SynergyManager::~SynergyManager() = default;

double SynergyManager::calculateSynergyLevel() const {
    if (pImpl->components.empty()) {
        return pImpl->baselineSynergy;
    }

    // Base synergy from component count
    double componentBonus = pImpl->components.size() * 0.05;
    double optimizationBonus = pImpl->optimizationCount * 0.02;
    
    // Enhanced synergy factors
    double diversity = pImpl->calculateDiversityBonus();
    double integration = pImpl->calculateIntegrationBonus();
    
    // Emergent synergy through non-linear combination
    double rawSynergy = pImpl->baselineSynergy + componentBonus + optimizationBonus;
    double enhancedSynergy = rawSynergy * (1.0 + diversity + integration);
    
    // Adaptivity bonus - reward improving systems
    if (pImpl->adaptivityScore > 0) {
        enhancedSynergy *= (1.0 + std::min(pImpl->adaptivityScore, 0.2));
    }
    
    return std::min(enhancedSynergy, 1.0);
}

void SynergyManager::optimizeSynergyPathways() {
    pImpl->optimizationCount++;
    std::cout << "[SynergyManager] Optimizing synergy pathways (iteration " 
              << pImpl->optimizationCount << ")" << std::endl;
    
    // Update component interactions
    pImpl->updateInteractions();
    
    // Calculate current synergy and record
    double currentSynergy = calculateSynergyLevel();
    pImpl->recordSynergyLevel(currentSynergy);
    
    // Update activation levels for all components
    for (const auto& comp : pImpl->components) {
        double currentActivation = pImpl->componentActivation[comp];
        // Gradually increase activation through optimization
        pImpl->componentActivation[comp] = std::min(currentActivation + 0.05, 1.0);
    }
    
    // Update enhancement factors
    pImpl->diversityBonus = pImpl->calculateDiversityBonus();
    pImpl->integrationBonus = pImpl->calculateIntegrationBonus();
    
    std::cout << "[SynergyManager] Analyzing " << pImpl->components.size() 
              << " component interactions" << std::endl;
    std::cout << "[SynergyManager] Diversity bonus: " << pImpl->diversityBonus 
              << ", Integration bonus: " << pImpl->integrationBonus << std::endl;
    std::cout << "[SynergyManager] Current synergy level: " << currentSynergy << std::endl;
}

void SynergyManager::addComponent(const std::string& componentName) {
    pImpl->components.push_back(componentName);
    pImpl->componentActivation[componentName] = 0.1; // Initial activation
    std::cout << "[SynergyManager] Added component: " << componentName << std::endl;
}

void SynergyManager::removeComponent(const std::string& componentName) {
    auto it = std::remove(pImpl->components.begin(), pImpl->components.end(), componentName);
    if (it != pImpl->components.end()) {
        pImpl->components.erase(it, pImpl->components.end());
        pImpl->componentActivation.erase(componentName);
        
        // Remove interactions involving this component
        pImpl->interactions.erase(
            std::remove_if(pImpl->interactions.begin(), pImpl->interactions.end(),
                [&componentName](const ComponentInteraction& interaction) {
                    return interaction.component1 == componentName || 
                           interaction.component2 == componentName;
                }),
            pImpl->interactions.end()
        );
        
        std::cout << "[SynergyManager] Removed component: " << componentName << std::endl;
    }
}

std::vector<std::string> SynergyManager::getComponents() const {
    return pImpl->components;
}

std::string SynergyManager::getSynergyMetrics() const {
    std::stringstream ss;
    ss << "Synergy Metrics:\n";
    ss << "  Total Components: " << pImpl->components.size() << "\n";
    ss << "  Current Synergy Level: " << calculateSynergyLevel() << "\n";
    ss << "  Optimization Iterations: " << pImpl->optimizationCount << "\n";
    ss << "  Diversity Bonus: " << pImpl->diversityBonus << "\n";
    ss << "  Integration Bonus: " << pImpl->integrationBonus << "\n";
    ss << "  Adaptivity Score: " << pImpl->adaptivityScore << "\n";
    ss << "  Component Interactions: " << pImpl->interactions.size() << "\n";
    
    ss << "  Active Components:\n";
    for (const auto& comp : pImpl->components) {
        double activation = pImpl->componentActivation.count(comp) > 0 
                          ? pImpl->componentActivation.at(comp) : 0.0;
        ss << "    - " << comp << " (activation: " << activation << ")\n";
    }
    
    if (!pImpl->synergyHistory.empty()) {
        double avgSynergy = 0.0;
        for (double s : pImpl->synergyHistory) {
            avgSynergy += s;
        }
        avgSynergy /= pImpl->synergyHistory.size();
        ss << "  Average Synergy (history): " << avgSynergy << "\n";
        ss << "  Synergy Trend: " << (pImpl->adaptivityScore > 0 ? "Improving" : 
                                      pImpl->adaptivityScore < 0 ? "Declining" : "Stable") << "\n";
    }
    
    return ss.str();
}

} // namespace cogself
