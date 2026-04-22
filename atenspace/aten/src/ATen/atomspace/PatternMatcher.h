#pragma once

#include "Atom.h"
#include "AtomSpace.h"
#include <unordered_map>
#include <unordered_set>
#include <functional>

namespace at {
namespace atomspace {

/**
 * VariableBinding - Maps variable nodes to their bound values
 */
using VariableBinding = std::unordered_map<Atom::Handle, Atom::Handle>;

/**
 * PatternMatcher - Pattern matching and unification engine
 * 
 * Provides pattern matching capabilities for ATenSpace, enabling:
 * - Variable binding and substitution
 * - Pattern queries with wildcards
 * - Unification of structures
 * - Support for inference rules
 * 
 * Variables are represented as VariableNode atoms in patterns.
 */
class PatternMatcher {
public:
    /**
     * Match a pattern against a target atom
     * 
     * @param pattern Pattern atom (may contain VariableNodes)
     * @param target Target atom to match against
     * @param bindings Variable bindings (input/output)
     * @return true if pattern matches target
     */
    static bool match(const Atom::Handle& pattern, 
                     const Atom::Handle& target,
                     VariableBinding& bindings) {
        // If pattern is a variable, bind it
        if (isVariable(pattern)) {
            return bindVariable(pattern, target, bindings);
        }
        
        // Check type compatibility
        if (pattern->getType() != target->getType()) {
            return false;
        }
        
        // For nodes, check name equality
        if (pattern->isNode()) {
            const Node* patternNode = static_cast<const Node*>(pattern.get());
            const Node* targetNode = static_cast<const Node*>(target.get());
            return patternNode->getName() == targetNode->getName();
        }
        
        // For links, recursively match outgoing sets
        if (pattern->isLink()) {
            const Link* patternLink = static_cast<const Link*>(pattern.get());
            const Link* targetLink = static_cast<const Link*>(target.get());
            
            const auto& patternOutgoing = patternLink->getOutgoingSet();
            const auto& targetOutgoing = targetLink->getOutgoingSet();
            
            if (patternOutgoing.size() != targetOutgoing.size()) {
                return false;
            }
            
            for (size_t i = 0; i < patternOutgoing.size(); ++i) {
                if (!match(patternOutgoing[i], targetOutgoing[i], bindings)) {
                    return false;
                }
            }
            return true;
        }
        
        return false;
    }
    
    /**
     * Find all atoms in the atomspace that match the pattern
     * 
     * @param space AtomSpace to search
     * @param pattern Pattern to match
     * @return Vector of (matched_atom, bindings) pairs
     */
    static std::vector<std::pair<Atom::Handle, VariableBinding>> 
    findMatches(AtomSpace& space, const Atom::Handle& pattern) {
        std::vector<std::pair<Atom::Handle, VariableBinding>> results;
        
        // Get all atoms of the appropriate type
        auto candidates = getCandidates(space, pattern);
        
        // Try to match each candidate
        for (const auto& candidate : candidates) {
            VariableBinding bindings;
            if (match(pattern, candidate, bindings)) {
                results.push_back({candidate, bindings});
            }
        }
        
        return results;
    }
    
    /**
     * Substitute variables in a pattern using bindings
     * 
     * @param pattern Pattern with variables
     * @param bindings Variable bindings
     * @param space AtomSpace to create atoms in
     * @return Pattern with variables substituted
     */
    static Atom::Handle substitute(const Atom::Handle& pattern,
                                   const VariableBinding& bindings,
                                   AtomSpace& space) {
        // If pattern is a variable, substitute it
        if (isVariable(pattern)) {
            auto it = bindings.find(pattern);
            if (it != bindings.end()) {
                return it->second;
            }
            return pattern; // Unbound variable
        }
        
        // For nodes, return as-is
        if (pattern->isNode()) {
            return pattern;
        }
        
        // For links, recursively substitute in outgoing set
        if (pattern->isLink()) {
            const Link* link = static_cast<const Link*>(pattern.get());
            std::vector<Atom::Handle> newOutgoing;
            
            for (const auto& atom : link->getOutgoingSet()) {
                newOutgoing.push_back(substitute(atom, bindings, space));
            }
            
            return space.addLink(link->getType(), newOutgoing);
        }
        
        return pattern;
    }
    
    /**
     * Unify two patterns, finding a common binding
     * 
     * @param pattern1 First pattern
     * @param pattern2 Second pattern
     * @param bindings Variable bindings (output)
     * @return true if patterns can be unified
     */
    static bool unify(const Atom::Handle& pattern1,
                     const Atom::Handle& pattern2,
                     VariableBinding& bindings) {
        // If either is a variable, bind it
        if (isVariable(pattern1)) {
            return bindVariable(pattern1, pattern2, bindings);
        }
        if (isVariable(pattern2)) {
            return bindVariable(pattern2, pattern1, bindings);
        }
        
        // Check type compatibility
        if (pattern1->getType() != pattern2->getType()) {
            return false;
        }
        
        // For nodes, check name equality
        if (pattern1->isNode()) {
            const Node* node1 = static_cast<const Node*>(pattern1.get());
            const Node* node2 = static_cast<const Node*>(pattern2.get());
            return node1->getName() == node2->getName();
        }
        
        // For links, recursively unify outgoing sets
        if (pattern1->isLink()) {
            const Link* link1 = static_cast<const Link*>(pattern1.get());
            const Link* link2 = static_cast<const Link*>(pattern2.get());
            
            const auto& outgoing1 = link1->getOutgoingSet();
            const auto& outgoing2 = link2->getOutgoingSet();
            
            if (outgoing1.size() != outgoing2.size()) {
                return false;
            }
            
            for (size_t i = 0; i < outgoing1.size(); ++i) {
                if (!unify(outgoing1[i], outgoing2[i], bindings)) {
                    return false;
                }
            }
            return true;
        }
        
        return false;
    }
    
    /**
     * Query the atomspace with a pattern
     * 
     * @param space AtomSpace to query
     * @param pattern Pattern to search for
     * @param callback Function to call for each match
     */
    static void query(AtomSpace& space, 
                     const Atom::Handle& pattern,
                     std::function<void(const Atom::Handle&, const VariableBinding&)> callback) {
        auto matches = findMatches(space, pattern);
        for (const auto& [atom, bindings] : matches) {
            callback(atom, bindings);
        }
    }
    
    /**
     * Check if an atom is a variable node
     */
    static bool isVariable(const Atom::Handle& atom) {
        return atom->isNode() && 
               atom->getType() == Atom::Type::VARIABLE_NODE;
    }
    
private:
    /**
     * Bind a variable to a value
     */
    static bool bindVariable(const Atom::Handle& var,
                            const Atom::Handle& value,
                            VariableBinding& bindings) {
        // Check if variable is already bound
        auto it = bindings.find(var);
        if (it != bindings.end()) {
            // Variable already bound, check consistency
            return it->second->equals(*value);
        }
        
        // Bind the variable
        bindings[var] = value;
        return true;
    }
    
    /**
     * Get candidate atoms for matching based on pattern structure
     */
    static std::vector<Atom::Handle> getCandidates(AtomSpace& space,
                                                   const Atom::Handle& pattern) {
        std::vector<Atom::Handle> candidates;
        
        // If pattern is a variable, return all atoms
        if (isVariable(pattern)) {
            const auto& allAtoms = space.getAllAtoms();
            candidates.insert(candidates.end(), allAtoms.begin(), allAtoms.end());
            return candidates;
        }
        
        // If pattern is a specific type, filter by type
        auto atomsByType = space.getAtomsByType(pattern->getType());
        
        // For nodes, we can further filter by name if it's not a variable
        if (pattern->isNode()) {
            const Node* node = static_cast<const Node*>(pattern.get());
            for (const auto& atom : atomsByType) {
                const Node* candidate = static_cast<const Node*>(atom.get());
                if (candidate->getName() == node->getName()) {
                    candidates.push_back(atom);
                }
            }
        } else {
            // For links, return all of the same type
            candidates = atomsByType;
        }
        
        return candidates;
    }
};

/**
 * Pattern - Helper class for building patterns
 */
class Pattern {
public:
    /**
     * Create a pattern from an atom
     */
    static Atom::Handle from(const Atom::Handle& atom) {
        return atom;
    }
    
    /**
     * Check if a pattern contains any variables
     */
    static bool hasVariables(const Atom::Handle& pattern) {
        if (PatternMatcher::isVariable(pattern)) {
            return true;
        }
        
        if (pattern->isLink()) {
            const Link* link = static_cast<const Link*>(pattern.get());
            for (const auto& atom : link->getOutgoingSet()) {
                if (hasVariables(atom)) {
                    return true;
                }
            }
        }
        
        return false;
    }
    
    /**
     * Extract all variables from a pattern
     */
    static std::vector<Atom::Handle> getVariables(const Atom::Handle& pattern) {
        std::unordered_set<Atom::Handle> vars;
        collectVariables(pattern, vars);
        return std::vector<Atom::Handle>(vars.begin(), vars.end());
    }
    
private:
    static void collectVariables(const Atom::Handle& pattern,
                                 std::unordered_set<Atom::Handle>& vars) {
        if (PatternMatcher::isVariable(pattern)) {
            vars.insert(pattern);
            return;
        }
        
        if (pattern->isLink()) {
            const Link* link = static_cast<const Link*>(pattern.get());
            for (const auto& atom : link->getOutgoingSet()) {
                collectVariables(atom, vars);
            }
        }
    }
};

} // namespace atomspace
} // namespace at
