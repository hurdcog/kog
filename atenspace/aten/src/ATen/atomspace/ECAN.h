#pragma once

#include "Atom.h"
#include "AtomSpace.h"
#include "AttentionBank.h"
#include <algorithm>
#include <map>
#include <mutex>
#include <set>
#include <vector>

namespace at {
namespace atomspace {

/**
 * ECAN (Economic Attention Networks) - Implementation of OpenCog's attention allocation system
 * 
 * ECAN treats attention as a scarce resource managed through economic principles:
 * - STI (Short-Term Importance): Currency for processor allocation
 * - LTI (Long-Term Importance): Currency for memory allocation
 * - HebbianLinks: Track correlation between atoms in attentional focus
 * - Importance Spreading: STI flows along HebbianLinks
 * - Forgetting: Atoms with low LTI are removed
 * - Rent: Atoms pay STI rent to stay in attentional focus
 * - Wages: Atoms receive STI wages for being used in cognitive processes
 */

/**
 * HebbianLinkManager - Manages Hebbian link creation and updates
 * 
 * HebbianLinks track temporal correlation between atoms. When atoms appear
 * together in the attentional focus, Hebbian links between them are created
 * or strengthened. These links then guide attention spreading.
 * 
 * Three types of Hebbian links:
 * - Symmetric: Mutual reinforcement (both atoms equally likely to activate each other)
 * - Asymmetric: Directional activation (source activates target more than reverse)
 * - Inverse: Inhibition (activation of one decreases importance of other)
 */
class HebbianLinkManager {
public:
    using Handle = Atom::Handle;
    
    HebbianLinkManager(AtomSpace& space, AttentionBank& bank)
        : space_(space), attention_bank_(bank) {}
    
    /**
     * Update Hebbian links based on current attentional focus
     * Creates new links or strengthens existing ones between co-occurring atoms
     */
    void updateHebbianLinks() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto focus = attention_bank_.getAttentionalFocus();
        if (focus.size() < 2) return;
        
        // Update links between all pairs in focus
        for (size_t i = 0; i < focus.size(); ++i) {
            for (size_t j = i + 1; j < focus.size(); ++j) {
                updateLinkBetween(focus[i], focus[j]);
            }
        }
    }
    
    /**
     * Get Hebbian links from an atom
     */
    std::vector<Handle> getHebbianLinks(Handle atom) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<Handle> links;
        auto incoming = atom->getIncomingSet();
        
        for (const auto& weak_link : incoming) {
            if (auto link = weak_link.lock()) {
                auto type = link->getType();
                if (isHebbianLinkType(type)) {
                    links.push_back(link);
                }
            }
        }
        
        return links;
    }
    
    /**
     * Get atoms connected by Hebbian links to the given atom
     */
    std::vector<std::pair<Handle, float>> getHebbianNeighbors(Handle atom) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<std::pair<Handle, float>> neighbors;
        auto links = getHebbianLinks(atom);
        
        for (const auto& link : links) {
            if (auto link_ptr = std::dynamic_pointer_cast<Link>(link)) {
                auto outgoing = link_ptr->getOutgoingSet();
                if (outgoing.size() == 2) {
                    // Get the other atom in the link
                    Handle other = (outgoing[0] == atom) ? outgoing[1] : outgoing[0];
                    
                    // Get link strength from truth value
                    auto tv = link->getTruthValue();
                    float strength = (tv.defined() && tv.numel() > 0) ? tv[0].item().toFloat() : 0.5f;
                    
                    neighbors.push_back({other, strength});
                }
            }
        }
        
        return neighbors;
    }
    
    /**
     * Create or strengthen Hebbian link between two atoms
     */
    void createOrStrengthenLink(Handle atom1, Handle atom2, Atom::Type link_type = Atom::Type::SYMMETRIC_HEBBIAN_LINK) {
        std::lock_guard<std::mutex> lock(mutex_);
        updateLinkBetween(atom1, atom2, link_type);
    }
    
    /**
     * Get statistics about Hebbian links
     */
    size_t getHebbianLinkCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        size_t count = 0;
        auto all_atoms = space_.getAtomsByType(Atom::Type::LINK);
        
        for (const auto& atom : all_atoms) {
            if (isHebbianLinkType(atom->getType())) {
                count++;
            }
        }
        
        return count;
    }

private:
    /**
     * Check if a type is a Hebbian link type
     */
    static bool isHebbianLinkType(Atom::Type type) {
        return type == Atom::Type::HEBBIAN_LINK ||
               type == Atom::Type::SYMMETRIC_HEBBIAN_LINK ||
               type == Atom::Type::ASYMMETRIC_HEBBIAN_LINK ||
               type == Atom::Type::INVERSE_HEBBIAN_LINK;
    }
    
    /**
     * Update link between two atoms
     * Creates new link or strengthens existing one
     */
    void updateLinkBetween(Handle atom1, Handle atom2, Atom::Type link_type = Atom::Type::SYMMETRIC_HEBBIAN_LINK) {
        if (!atom1 || !atom2 || atom1 == atom2) return;
        
        // Try to find existing link by checking incoming sets
        Handle existing = nullptr;
        auto incoming = atom1->getIncomingSet();
        for (const auto& weak_link : incoming) {
            if (auto link = weak_link.lock()) {
                if (link->getType() == link_type) {
                    auto link_ptr = std::dynamic_pointer_cast<Link>(link);
                    if (link_ptr) {
                        auto outgoing = link_ptr->getOutgoingSet();
                        if (outgoing.size() == 2 && 
                            ((outgoing[0] == atom1 && outgoing[1] == atom2) ||
                             (outgoing[0] == atom2 && outgoing[1] == atom1))) {
                            existing = link;
                            break;
                        }
                    }
                }
            }
        }
        
        if (existing) {
            // Strengthen existing link
            auto tv = existing->getTruthValue();
            float strength = (tv.defined() && tv.numel() > 0) ? tv[0].item().toFloat() : 0.5f;
            float confidence = (tv.defined() && tv.numel() > 1) ? tv[1].item().toFloat() : 0.1f;
            
            // Hebbian learning: increase strength and confidence
            constexpr float LEARNING_RATE = 0.1f;
            constexpr float MAX_CONFIDENCE = 0.99f;
            
            strength = std::min(1.0f, strength + LEARNING_RATE * (1.0f - strength));
            confidence = std::min(MAX_CONFIDENCE, confidence + LEARNING_RATE * (1.0f - confidence));
            
            existing->setTruthValue(torch::tensor({strength, confidence}));
        } else {
            // Create new link with initial strength
            auto link = space_.addLink(link_type, {atom1, atom2});
            if (link) {
                // Initial truth value: moderate strength, low confidence
                link->setTruthValue(torch::tensor({0.5f, 0.1f}));
            }
        }
    }
    
    mutable std::mutex mutex_;
    AtomSpace& space_;
    AttentionBank& attention_bank_;
};

/**
 * ImportanceSpreadingAgent - Spreads STI along Hebbian links
 * 
 * This is the primary attention spreading mechanism in ECAN.
 * Atoms in the attentional focus spread their "excess" STI to connected atoms
 * via Hebbian links, potentially bringing new atoms into focus.
 */
class ImportanceSpreadingAgent {
public:
    using Handle = Atom::Handle;
    
    ImportanceSpreadingAgent(AtomSpace& space, AttentionBank& bank, HebbianLinkManager& hebbian)
        : space_(space), attention_bank_(bank), hebbian_manager_(hebbian),
          spread_percentage_(0.1f), focus_threshold_(0.0f), max_spread_amount_(100.0f) {}
    
    /**
     * Run one cycle of importance spreading
     * Spreads STI from focused atoms to their Hebbian neighbors
     */
    void spread() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto focus = attention_bank_.getAttentionalFocus();
        
        for (const auto& atom : focus) {
            auto av = attention_bank_.getAttentionValue(atom);
            
            // Only spread if atom has excess STI above threshold
            if (av.sti > focus_threshold_) {
                spreadFromAtom(atom, av.sti);
            }
        }
    }
    
    /**
     * Set percentage of STI to spread (default 0.1 = 10%)
     */
    void setSpreadPercentage(float percentage) {
        std::lock_guard<std::mutex> lock(mutex_);
        spread_percentage_ = std::max(0.0f, std::min(1.0f, percentage));
    }
    
    /**
     * Set threshold - only atoms above this STI will spread
     */
    void setFocusThreshold(float threshold) {
        std::lock_guard<std::mutex> lock(mutex_);
        focus_threshold_ = threshold;
    }
    
    /**
     * Set maximum amount of STI that can be spread from one atom
     */
    void setMaxSpreadAmount(float amount) {
        std::lock_guard<std::mutex> lock(mutex_);
        max_spread_amount_ = amount;
    }

private:
    /**
     * Spread STI from a single atom to its Hebbian neighbors
     */
    void spreadFromAtom(Handle source, float source_sti) {
        auto neighbors = hebbian_manager_.getHebbianNeighbors(source);
        if (neighbors.empty()) return;
        
        // Calculate total STI to spread
        float to_spread = std::min(source_sti * spread_percentage_, max_spread_amount_);
        
        // Calculate total weight for distribution
        float total_weight = 0.0f;
        for (const auto& [neighbor, strength] : neighbors) {
            total_weight += strength;
        }
        
        if (total_weight < 1e-6f) return;
        
        // Distribute STI proportional to link strength
        for (const auto& [neighbor, strength] : neighbors) {
            float amount = to_spread * (strength / total_weight);
            attention_bank_.transferSTI(source, neighbor, amount);
        }
    }
    
    mutable std::mutex mutex_;
    AtomSpace& space_;
    AttentionBank& attention_bank_;
    HebbianLinkManager& hebbian_manager_;
    float spread_percentage_;      // Percentage of STI to spread
    float focus_threshold_;        // Minimum STI to spread from
    float max_spread_amount_;      // Maximum STI to spread per atom
};

/**
 * ForgettingAgent - Removes atoms with persistently low LTI
 * 
 * Simulates forgetting by removing atoms that haven't been important
 * for a long time. This is essential for resource management in large
 * knowledge bases.
 */
class ForgettingAgent {
public:
    using Handle = Atom::Handle;
    
    ForgettingAgent(AtomSpace& space, AttentionBank& bank)
        : space_(space), attention_bank_(bank), lti_threshold_(0.0f), atoms_forgotten_(0) {}
    
    /**
     * Run one cycle of forgetting
     * Removes atoms below LTI threshold
     */
    void forget() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Get all atoms being tracked
        auto all_atoms = space_.getAtomsByType(Atom::Type::NODE);
        auto all_links = space_.getAtomsByType(Atom::Type::LINK);
        all_atoms.insert(all_atoms.end(), all_links.begin(), all_links.end());
        
        std::vector<Handle> to_remove;
        
        for (const auto& atom : all_atoms) {
            auto av = attention_bank_.getAttentionValue(atom);
            
            // Mark for removal if below LTI threshold
            if (av.lti < lti_threshold_) {
                to_remove.push_back(atom);
            }
        }
        
        // Remove atoms
        for (const auto& atom : to_remove) {
            if (canForget(atom)) {
                attention_bank_.remove(atom);
                // Note: We don't remove from AtomSpace as that could break
                // other references. Just remove from attention tracking.
                atoms_forgotten_++;
            }
        }
    }
    
    /**
     * Set LTI threshold for forgetting (default 0.0)
     */
    void setLTIThreshold(float threshold) {
        std::lock_guard<std::mutex> lock(mutex_);
        lti_threshold_ = threshold;
    }
    
    /**
     * Get number of atoms forgotten
     */
    size_t getAtomsForgotten() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return atoms_forgotten_;
    }
    
    /**
     * Reset statistics
     */
    void resetStatistics() {
        std::lock_guard<std::mutex> lock(mutex_);
        atoms_forgotten_ = 0;
    }

private:
    /**
     * Check if atom can be safely forgotten
     * Don't forget atoms that are currently in focus or have high STI
     */
    bool canForget(Handle atom) const {
        auto av = attention_bank_.getAttentionValue(atom);
        
        // Don't forget if STI is high (currently important)
        constexpr float MIN_STI_FOR_PROTECTION = 10.0f;
        if (av.sti > MIN_STI_FOR_PROTECTION) {
            return false;
        }
        
        // Check if in attentional focus
        auto focus = attention_bank_.getAttentionalFocus();
        if (std::find(focus.begin(), focus.end(), atom) != focus.end()) {
            return false;
        }
        
        return true;
    }
    
    mutable std::mutex mutex_;
    AtomSpace& space_;
    AttentionBank& attention_bank_;
    float lti_threshold_;
    size_t atoms_forgotten_;
};

/**
 * RentAgent - Collects STI rent from atoms in attentional focus
 * 
 * Implements economic scarcity - atoms must "pay" to stay in focus.
 * This creates competition for the limited resource of attention.
 */
class RentAgent {
public:
    using Handle = Atom::Handle;
    
    RentAgent(AttentionBank& bank)
        : attention_bank_(bank), rent_rate_(1.0f) {}
    
    /**
     * Collect rent from all atoms in attentional focus
     */
    void collectRent() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto focus = attention_bank_.getAttentionalFocus();
        
        for (const auto& atom : focus) {
            auto av = attention_bank_.getAttentionValue(atom);
            
            // Charge rent (reduce STI)
            float new_sti = std::max(0.0f, av.sti - rent_rate_);
            attention_bank_.updateSTI(atom, new_sti);
        }
    }
    
    /**
     * Set rent rate (STI deducted per cycle)
     */
    void setRentRate(float rate) {
        std::lock_guard<std::mutex> lock(mutex_);
        rent_rate_ = std::max(0.0f, rate);
    }
    
    /**
     * Get current rent rate
     */
    float getRentRate() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return rent_rate_;
    }

private:
    mutable std::mutex mutex_;
    AttentionBank& attention_bank_;
    float rent_rate_;
};

/**
 * WageAgent - Pays STI wages to atoms used in cognitive processes
 * 
 * Rewards atoms that participate in successful reasoning, learning,
 * or other cognitive processes. This reinforces useful knowledge.
 */
class WageAgent {
public:
    using Handle = Atom::Handle;
    
    WageAgent(AttentionBank& bank)
        : attention_bank_(bank), wage_amount_(5.0f) {}
    
    /**
     * Pay wage to an atom for being used
     */
    void payWage(Handle atom) {
        if (!atom) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        attention_bank_.stimulate(atom, wage_amount_);
    }
    
    /**
     * Pay wages to multiple atoms
     */
    void payWages(const std::vector<Handle>& atoms) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (const auto& atom : atoms) {
            if (atom) {
                attention_bank_.stimulate(atom, wage_amount_);
            }
        }
    }
    
    /**
     * Set wage amount (STI added per payment)
     */
    void setWageAmount(float amount) {
        std::lock_guard<std::mutex> lock(mutex_);
        wage_amount_ = std::max(0.0f, amount);
    }
    
    /**
     * Get current wage amount
     */
    float getWageAmount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return wage_amount_;
    }

private:
    mutable std::mutex mutex_;
    AttentionBank& attention_bank_;
    float wage_amount_;
};

/**
 * ECANManager - Coordinates all ECAN agents
 * 
 * Provides a unified interface for running the complete ECAN cycle:
 * 1. Update Hebbian links based on attentional focus
 * 2. Spread importance along Hebbian links
 * 3. Collect rent from focused atoms
 * 4. Forget low-LTI atoms
 * 5. Pay wages to used atoms (external trigger)
 */
class ECANManager {
public:
    using Handle = Atom::Handle;
    
    ECANManager(AtomSpace& space, AttentionBank& bank)
        : space_(space),
          attention_bank_(bank),
          hebbian_manager_(space, bank),
          spreading_agent_(space, bank, hebbian_manager_),
          forgetting_agent_(space, bank),
          rent_agent_(bank),
          wage_agent_(bank),
          cycle_count_(0) {}
    
    /**
     * Run one complete ECAN cycle
     */
    void runCycle() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // 1. Update Hebbian links based on co-occurrence in focus
        hebbian_manager_.updateHebbianLinks();
        
        // 2. Spread importance along Hebbian links
        spreading_agent_.spread();
        
        // 3. Collect rent from atoms in focus
        rent_agent_.collectRent();
        
        // 4. Forget atoms with low LTI (run less frequently)
        if (cycle_count_ % 10 == 0) {  // Every 10 cycles
            forgetting_agent_.forget();
        }
        
        cycle_count_++;
    }
    
    /**
     * Pay wage to atoms used in cognitive processes
     */
    void payWage(Handle atom) {
        wage_agent_.payWage(atom);
    }
    
    /**
     * Pay wages to multiple atoms
     */
    void payWages(const std::vector<Handle>& atoms) {
        wage_agent_.payWages(atoms);
    }
    
    // Access to individual agents for configuration
    HebbianLinkManager& getHebbianManager() { return hebbian_manager_; }
    ImportanceSpreadingAgent& getSpreadingAgent() { return spreading_agent_; }
    ForgettingAgent& getForgettingAgent() { return forgetting_agent_; }
    RentAgent& getRentAgent() { return rent_agent_; }
    WageAgent& getWageAgent() { return wage_agent_; }
    
    /**
     * Get statistics
     */
    size_t getCycleCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return cycle_count_;
    }
    
    size_t getHebbianLinkCount() const {
        return hebbian_manager_.getHebbianLinkCount();
    }
    
    size_t getAtomsForgotten() const {
        return forgetting_agent_.getAtomsForgotten();
    }

private:
    mutable std::mutex mutex_;
    AtomSpace& space_;
    AttentionBank& attention_bank_;
    HebbianLinkManager hebbian_manager_;
    ImportanceSpreadingAgent spreading_agent_;
    ForgettingAgent forgetting_agent_;
    RentAgent rent_agent_;
    WageAgent wage_agent_;
    size_t cycle_count_;
};

} // namespace atomspace
} // namespace at
