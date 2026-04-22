#pragma once

#include "Atom.h"
#include "AtomSpace.h"
#include "PatternMatcher.h"
#include "TruthValue.h"
#include "ForwardChainer.h"
#include <vector>
#include <set>
#include <functional>
#include <memory>

namespace at {
namespace atomspace {

/**
 * Proof - Represents a proof tree for a goal
 */
struct Proof {
    Atom::Handle goal;
    std::vector<Atom::Handle> premises;
    std::shared_ptr<InferenceRule> rule;
    std::vector<std::shared_ptr<Proof>> subproofs;
    Tensor truthValue;
    
    Proof(const Atom::Handle& g) : goal(g) {
        truthValue = TruthValue::defaultTV();
    }
    
    std::string toString(int indent = 0) const {
        std::string result(indent * 2, ' ');
        result += "Goal: " + goal->toString() + " TV: [";
        result += std::to_string(TruthValue::getStrength(truthValue)) + ", ";
        result += std::to_string(TruthValue::getConfidence(truthValue)) + "]\n";
        
        if (rule) {
            result += std::string(indent * 2, ' ') + "Rule: " + rule->getName() + "\n";
        }
        
        for (const auto& premise : premises) {
            result += std::string(indent * 2, ' ') + "Premise: " + premise->toString() + "\n";
        }
        
        for (const auto& subproof : subproofs) {
            result += subproof->toString(indent + 1);
        }
        
        return result;
    }
};

/**
 * BackwardChainer - Goal-directed backward chaining inference
 * 
 * Given a goal, searches for a proof by:
 * 1. Checking if the goal is already in the atomspace
 * 2. Finding rules that could prove the goal
 * 3. Recursively proving the premises of those rules
 * 4. Constructing a proof tree
 */
class BackwardChainer {
public:
    BackwardChainer(AtomSpace& space)
        : space_(space), maxDepth_(10), maxProofs_(5) {}
    
    /**
     * Set maximum proof search depth
     */
    void setMaxDepth(int depth) {
        maxDepth_ = depth;
    }
    
    /**
     * Set maximum number of proofs to find
     */
    void setMaxProofs(int maxProofs) {
        maxProofs_ = maxProofs;
    }
    
    /**
     * Add an inference rule for backward chaining
     */
    void addRule(std::shared_ptr<InferenceRule> rule) {
        rules_.push_back(rule);
    }
    
    /**
     * Prove a goal using backward chaining
     * 
     * @param goal Goal atom to prove
     * @return Vector of proofs found (may be empty)
     */
    std::vector<std::shared_ptr<Proof>> prove(const Atom::Handle& goal) {
        std::set<Atom::Handle> visited;
        std::vector<std::shared_ptr<Proof>> proofs;
        
        proveRecursive(goal, 0, visited, proofs);
        
        return proofs;
    }
    
    /**
     * Get the best proof for a goal (highest truth value confidence)
     * 
     * @param goal Goal to prove
     * @return Best proof, or nullptr if no proof found
     */
    std::shared_ptr<Proof> getBestProof(const Atom::Handle& goal) {
        auto proofs = prove(goal);
        
        if (proofs.empty()) {
            return nullptr;
        }
        
        // Find proof with highest confidence
        std::shared_ptr<Proof> best = proofs[0];
        float bestConf = TruthValue::getConfidence(best->truthValue);
        
        for (const auto& proof : proofs) {
            float conf = TruthValue::getConfidence(proof->truthValue);
            if (conf > bestConf) {
                best = proof;
                bestConf = conf;
            }
        }
        
        return best;
    }
    
    /**
     * Query if a goal can be proven
     * 
     * @param goal Goal atom
     * @return true if at least one proof exists
     */
    bool canProve(const Atom::Handle& goal) {
        auto proofs = prove(goal);
        return !proofs.empty();
    }
    
    /**
     * Query and return the truth value of a goal if provable
     * 
     * @param goal Goal atom
     * @return Truth value from best proof, or default TV if not provable
     */
    Tensor query(const Atom::Handle& goal) {
        auto proof = getBestProof(goal);
        if (proof) {
            return proof->truthValue;
        }
        return TruthValue::defaultTV();
    }
    
private:
    /**
     * Recursive proof search
     */
    void proveRecursive(const Atom::Handle& goal,
                       int depth,
                       std::set<Atom::Handle>& visited,
                       std::vector<std::shared_ptr<Proof>>& proofs) {
        // Check termination conditions
        if (depth > maxDepth_) return;
        if (proofs.size() >= maxProofs_) return;
        if (visited.count(goal)) return; // Avoid cycles
        
        visited.insert(goal);
        
        // Strategy 1: Check if goal is directly in atomspace
        auto directMatch = findDirectMatch(goal);
        if (directMatch) {
            auto proof = std::make_shared<Proof>(goal);
            proof->truthValue = directMatch->getTruthValue();
            proofs.push_back(proof);
            visited.erase(goal);
            return;
        }
        
        // Strategy 2: Try pattern matching with variables
        auto patternMatches = findPatternMatches(goal);
        for (const auto& [match, bindings] : patternMatches) {
            auto proof = std::make_shared<Proof>(goal);
            proof->premises.push_back(match);
            proof->truthValue = match->getTruthValue();
            proofs.push_back(proof);
            
            if (proofs.size() >= maxProofs_) {
                visited.erase(goal);
                return;
            }
        }
        
        // Strategy 3: Backward chain using rules
        backwardChainRules(goal, depth, visited, proofs);
        
        visited.erase(goal);
    }
    
    /**
     * Find direct match in atomspace
     */
    Atom::Handle findDirectMatch(const Atom::Handle& goal) {
        const auto& allAtoms = space_.getAllAtoms();
        
        for (const auto& atom : allAtoms) {
            if (atom->equals(*goal)) {
                return atom;
            }
        }
        
        return nullptr;
    }
    
    /**
     * Find pattern matches with variables
     */
    std::vector<std::pair<Atom::Handle, VariableBinding>> 
    findPatternMatches(const Atom::Handle& goal) {
        std::vector<std::pair<Atom::Handle, VariableBinding>> matches;
        
        // If goal contains variables, use pattern matching
        if (Pattern::hasVariables(goal)) {
            matches = PatternMatcher::findMatches(space_, goal);
        }
        
        return matches;
    }
    
    /**
     * Try to prove goal using backward chaining rules
     */
    void backwardChainRules(const Atom::Handle& goal,
                           int depth,
                           std::set<Atom::Handle>& visited,
                           std::vector<std::shared_ptr<Proof>>& proofs) {
        // For each rule, check if it could prove the goal
        for (const auto& rule : rules_) {
            // Try to find premises that would allow the rule to prove the goal
            auto potentialPremises = findPotentialPremises(goal, rule);
            
            for (const auto& premises : potentialPremises) {
                // Try to prove each premise recursively
                bool allPremisesProven = true;
                std::vector<std::shared_ptr<Proof>> subproofs;
                
                for (const auto& premise : premises) {
                    std::vector<std::shared_ptr<Proof>> premiseProofs;
                    proveRecursive(premise, depth + 1, visited, premiseProofs);
                    
                    if (premiseProofs.empty()) {
                        allPremisesProven = false;
                        break;
                    }
                    
                    subproofs.push_back(premiseProofs[0]); // Take best proof
                }
                
                if (allPremisesProven) {
                    // Construct proof
                    auto proof = std::make_shared<Proof>(goal);
                    proof->premises = premises;
                    proof->rule = rule;
                    proof->subproofs = subproofs;
                    
                    // Apply rule to compute truth value
                    auto conclusions = rule->apply(premises, space_);
                    if (!conclusions.empty()) {
                        proof->truthValue = conclusions[0]->getTruthValue();
                    }
                    
                    proofs.push_back(proof);
                    
                    if (proofs.size() >= maxProofs_) return;
                }
            }
        }
    }
    
    /**
     * Find potential premises that could prove goal with given rule
     */
    std::vector<std::vector<Atom::Handle>> 
    findPotentialPremises(const Atom::Handle& goal,
                         std::shared_ptr<InferenceRule> rule) {
        std::vector<std::vector<Atom::Handle>> results;
        
        // This is simplified - in a full implementation, we would:
        // 1. Analyze the rule's structure
        // 2. Work backwards from the conclusion pattern to premise patterns
        // 3. Search atomspace for atoms matching those patterns
        
        // For deduction: if goal is A→C, find A→B and B→C
        if (rule->getName() == "Deduction" && goal->isLink()) {
            const Link* goalLink = static_cast<const Link*>(goal.get());
            if (goalLink->getArity() == 2 &&
                (goal->getType() == Atom::Type::INHERITANCE_LINK ||
                 goal->getType() == Atom::Type::IMPLICATION_LINK)) {
                
                auto A = goalLink->getOutgoingAtom(0);
                auto C = goalLink->getOutgoingAtom(1);
                
                // Find all possible middle terms B
                const auto& allAtoms = space_.getAllAtoms();
                for (const auto& B : allAtoms) {
                    // Look for A→B
                    auto AB = space_.getLink(goal->getType(), {A, B});
                    if (AB) {
                        // Look for B→C
                        auto BC = space_.getLink(goal->getType(), {B, C});
                        if (BC) {
                            results.push_back({AB, BC});
                        }
                    }
                }
            }
        }
        
        return results;
    }
    
    AtomSpace& space_;
    std::vector<std::shared_ptr<InferenceRule>> rules_;
    int maxDepth_;
    size_t maxProofs_;
};

} // namespace atomspace
} // namespace at
