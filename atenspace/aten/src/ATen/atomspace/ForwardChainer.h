#pragma once

#include "Atom.h"
#include "AtomSpace.h"
#include "PatternMatcher.h"
#include "TruthValue.h"
#include "AttentionBank.h"
#include <vector>
#include <functional>
#include <memory>

namespace at {
namespace atomspace {

/**
 * InferenceRule - Base class for PLN inference rules
 * 
 * An inference rule takes premises (input atoms) and produces
 * conclusions (new atoms) with computed truth values.
 */
class InferenceRule {
public:
    virtual ~InferenceRule() = default;
    
    /**
     * Get the rule name
     */
    virtual std::string getName() const = 0;
    
    /**
     * Check if the rule can be applied to the given premises
     */
    virtual bool canApply(const std::vector<Atom::Handle>& premises) const = 0;
    
    /**
     * Apply the rule to generate conclusions
     * 
     * @param premises Input atoms
     * @param space AtomSpace to create new atoms in
     * @return Vector of newly created conclusion atoms
     */
    virtual std::vector<Atom::Handle> apply(const std::vector<Atom::Handle>& premises,
                                            AtomSpace& space) = 0;
};

/**
 * DeductionRule - Implements A→B, B→C ⊢ A→C
 */
class DeductionRule : public InferenceRule {
public:
    std::string getName() const override {
        return "Deduction";
    }
    
    bool canApply(const std::vector<Atom::Handle>& premises) const override {
        if (premises.size() != 2) return false;
        
        // Both premises must be inheritance links (or other implication links)
        return premises[0]->isLink() && premises[1]->isLink() &&
               (premises[0]->getType() == Atom::Type::INHERITANCE_LINK ||
                premises[0]->getType() == Atom::Type::IMPLICATION_LINK) &&
               (premises[1]->getType() == Atom::Type::INHERITANCE_LINK ||
                premises[1]->getType() == Atom::Type::IMPLICATION_LINK);
    }
    
    std::vector<Atom::Handle> apply(const std::vector<Atom::Handle>& premises,
                                    AtomSpace& space) override {
        std::vector<Atom::Handle> conclusions;
        
        const Link* link1 = static_cast<const Link*>(premises[0].get());
        const Link* link2 = static_cast<const Link*>(premises[1].get());
        
        // Check if they chain: A→B and B→C
        if (link1->getArity() != 2 || link2->getArity() != 2) {
            return conclusions;
        }
        
        auto A = link1->getOutgoingAtom(0);
        auto B1 = link1->getOutgoingAtom(1);
        auto B2 = link2->getOutgoingAtom(0);
        auto C = link2->getOutgoingAtom(1);
        
        // Check if B matches
        if (!B1->equals(*B2)) {
            return conclusions;
        }
        
        // Create A→C
        auto conclusion = space.addLink(premises[0]->getType(), {A, C});
        
        // Compute truth value using deduction formula
        Tensor tv = TruthValue::deduction(premises[0]->getTruthValue(),
                                         premises[1]->getTruthValue());
        conclusion->setTruthValue(tv);
        
        conclusions.push_back(conclusion);
        return conclusions;
    }
};

/**
 * InductionRule - Generalize from specific instances
 */
class InductionRule : public InferenceRule {
public:
    std::string getName() const override {
        return "Induction";
    }
    
    bool canApply(const std::vector<Atom::Handle>& premises) const override {
        // Need at least one evaluation link
        if (premises.empty()) return false;
        
        for (const auto& premise : premises) {
            if (premise->getType() != Atom::Type::EVALUATION_LINK) {
                return false;
            }
        }
        return true;
    }
    
    std::vector<Atom::Handle> apply(const std::vector<Atom::Handle>& premises,
                                    AtomSpace& space) override {
        std::vector<Atom::Handle> conclusions;
        
        // Group premises by predicate
        std::map<Atom::Handle, std::vector<Atom::Handle>> byPredicate;
        
        for (const auto& premise : premises) {
            const Link* eval = static_cast<const Link*>(premise.get());
            if (eval->getArity() >= 1) {
                auto predicate = eval->getOutgoingAtom(0);
                byPredicate[predicate].push_back(premise);
            }
        }
        
        // For each predicate, create an induced rule if we have enough evidence
        for (const auto& [predicate, instances] : byPredicate) {
            if (instances.size() >= 2) {
                // Count positive instances (high truth value)
                int positiveCount = 0;
                for (const auto& instance : instances) {
                    float strength = TruthValue::getStrength(instance->getTruthValue());
                    if (strength > 0.5f) {
                        positiveCount++;
                    }
                }
                
                // Create a general rule about this predicate
                // This is simplified - real induction would extract the pattern
                Tensor tv = TruthValue::induction(positiveCount, instances.size());
                
                // We don't create a specific conclusion here without more context
                // This is a placeholder for more sophisticated induction
            }
        }
        
        return conclusions;
    }
};

/**
 * AbductionRule - Reason to best explanation: B, A→B ⊢ A
 */
class AbductionRule : public InferenceRule {
public:
    std::string getName() const override {
        return "Abduction";
    }
    
    bool canApply(const std::vector<Atom::Handle>& premises) const override {
        // Need an observation and a rule
        return premises.size() == 2 &&
               premises[1]->isLink() &&
               (premises[1]->getType() == Atom::Type::INHERITANCE_LINK ||
                premises[1]->getType() == Atom::Type::IMPLICATION_LINK);
    }
    
    std::vector<Atom::Handle> apply(const std::vector<Atom::Handle>& premises,
                                    AtomSpace& space) override {
        std::vector<Atom::Handle> conclusions;
        
        auto observation = premises[0];
        const Link* rule = static_cast<const Link*>(premises[1].get());
        
        if (rule->getArity() != 2) {
            return conclusions;
        }
        
        auto A = rule->getOutgoingAtom(0);
        auto B = rule->getOutgoingAtom(1);
        
        // Check if observation matches B
        if (!observation->equals(*B)) {
            return conclusions;
        }
        
        // Abduce A
        auto conclusion = A;
        
        // Compute truth value using abduction formula
        Tensor tv = TruthValue::abduction(observation->getTruthValue(),
                                         rule->getTruthValue());
        conclusion->setTruthValue(tv);
        
        conclusions.push_back(conclusion);
        return conclusions;
    }
};

/**
 * ForwardChainer - Forward chaining inference engine
 * 
 * Applies inference rules to derive new knowledge from existing atoms.
 * Uses attention values to prioritize which inferences to perform.
 */
class ForwardChainer {
public:
    ForwardChainer(AtomSpace& space) 
        : space_(space), maxIterations_(100), confidenceThreshold_(0.1f) {
        // Register default rules
        addRule(std::make_shared<DeductionRule>());
        addRule(std::make_shared<InductionRule>());
        addRule(std::make_shared<AbductionRule>());
    }
    
    /**
     * Add an inference rule
     */
    void addRule(std::shared_ptr<InferenceRule> rule) {
        rules_.push_back(rule);
    }
    
    /**
     * Set maximum iterations
     */
    void setMaxIterations(int maxIter) {
        maxIterations_ = maxIter;
    }
    
    /**
     * Set minimum confidence threshold for new conclusions
     */
    void setConfidenceThreshold(float threshold) {
        confidenceThreshold_ = threshold;
    }
    
    /**
     * Run forward chaining to exhaustion or max iterations
     * 
     * @param attentionBank Optional attention bank for priority guidance
     * @return Number of new atoms created
     */
    int run(AttentionBank* attentionBank = nullptr) {
        int totalNewAtoms = 0;
        
        for (int iteration = 0; iteration < maxIterations_; ++iteration) {
            int newAtomsThisIteration = performIteration(attentionBank);
            totalNewAtoms += newAtomsThisIteration;
            
            // Stop if no new atoms were created
            if (newAtomsThisIteration == 0) {
                break;
            }
        }
        
        return totalNewAtoms;
    }
    
    /**
     * Perform a single forward chaining step
     * 
     * @param target Optional target atom to focus inference on
     * @param attentionBank Optional attention bank for priority
     * @return Number of new atoms created
     */
    int step(Atom::Handle target = nullptr, AttentionBank* attentionBank = nullptr) {
        return performIteration(attentionBank, target);
    }
    
    /**
     * Apply all applicable rules to a set of premises
     * 
     * @param premises Input atoms
     * @return New conclusions
     */
    std::vector<Atom::Handle> applyRules(const std::vector<Atom::Handle>& premises) {
        std::vector<Atom::Handle> allConclusions;
        
        for (const auto& rule : rules_) {
            if (rule->canApply(premises)) {
                auto conclusions = rule->apply(premises, space_);
                
                // Filter by confidence threshold
                for (const auto& conclusion : conclusions) {
                    float conf = TruthValue::getConfidence(conclusion->getTruthValue());
                    if (conf >= confidenceThreshold_) {
                        allConclusions.push_back(conclusion);
                    }
                }
            }
        }
        
        return allConclusions;
    }
    
private:
    /**
     * Perform one iteration of forward chaining
     */
    int performIteration(AttentionBank* attentionBank, Atom::Handle target = nullptr) {
        int newAtoms = 0;
        size_t initialSize = space_.getSize();
        
        // Get all atoms, optionally sorted by attention
        std::vector<Atom::Handle> atoms;
        if (attentionBank) {
            atoms = attentionBank->getAttentionalFocus();
            // Also include some random atoms
            const auto& allAtoms = space_.getAllAtoms();
            size_t sampleSize = std::min(size_t(50), allAtoms.size());
            for (size_t i = 0; i < sampleSize && atoms.size() < 100; ++i) {
                atoms.push_back(allAtoms[i]);
            }
        } else {
            atoms = space_.getAllAtoms();
        }
        
        // Try to apply rules to pairs of atoms
        for (size_t i = 0; i < atoms.size() && newAtoms < 100; ++i) {
            for (size_t j = i + 1; j < atoms.size() && newAtoms < 100; ++j) {
                std::vector<Atom::Handle> premises = {atoms[i], atoms[j]};
                
                auto conclusions = applyRules(premises);
                newAtoms += conclusions.size();
            }
        }
        
        return space_.getSize() - initialSize;
    }
    
    AtomSpace& space_;
    std::vector<std::shared_ptr<InferenceRule>> rules_;
    int maxIterations_;
    float confidenceThreshold_;
};

} // namespace atomspace
} // namespace at
