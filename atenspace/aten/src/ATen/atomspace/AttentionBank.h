#pragma once

#include "Atom.h"
#include <algorithm>
#include <map>
#include <mutex>
#include <queue>
#include <vector>

namespace at {
namespace atomspace {

/**
 * AttentionBank - Manages attention values and attentional focus
 * 
 * The AttentionBank manages attentional values assigned to Atoms, simulating
 * a cognitive focus mechanism. This enables prioritization of certain pieces
 * of knowledge for reasoning, learning, or decision-making, supporting efficient
 * resource allocation similar to "attention" in the human mind.
 */
class AttentionBank {
public:
    using Handle = Atom::Handle;
    
    /**
     * Attention Value structure
     * - STI (Short-Term Importance): Current importance/salience
     * - LTI (Long-Term Importance): Historical importance
     * - VLTI (Very Long-Term Importance): Archival importance
     */
    struct AttentionValue {
        float sti;   // Short-Term Importance
        float lti;   // Long-Term Importance  
        float vlti;  // Very Long-Term Importance
        
        AttentionValue() : sti(0.0f), lti(0.0f), vlti(0.0f) {}
        AttentionValue(float s, float l, float v) : sti(s), lti(l), vlti(v) {}
        
        // Total importance score
        float totalImportance() const {
            return sti + lti + vlti;
        }
    };
    
    AttentionBank() : max_af_size_(100), min_sti_threshold_(0.0f) {}
    ~AttentionBank() = default;
    
    // Prevent copying
    AttentionBank(const AttentionBank&) = delete;
    AttentionBank& operator=(const AttentionBank&) = delete;
    
    /**
     * Set attention value for an atom
     */
    void setAttentionValue(Handle atom, const AttentionValue& av) {
        if (!atom) return;
        std::lock_guard<std::mutex> lock(mutex_);
        attention_values_[atom] = av;
        updateAttentionalFocus();
    }
    
    /**
     * Get attention value for an atom
     */
    AttentionValue getAttentionValue(Handle atom) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = attention_values_.find(atom);
        if (it != attention_values_.end()) {
            return it->second;
        }
        return AttentionValue();
    }
    
    /**
     * Stimulate an atom (increase STI)
     */
    void stimulate(Handle atom, float amount) {
        if (!atom) return;
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = attention_values_.find(atom);
        if (it != attention_values_.end()) {
            it->second.sti += amount;
        } else {
            attention_values_[atom] = AttentionValue(amount, 0.0f, 0.0f);
        }
        updateAttentionalFocus();
    }
    
    /**
     * Update STI for an atom
     */
    void updateSTI(Handle atom, float sti) {
        if (!atom) return;
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = attention_values_.find(atom);
        if (it != attention_values_.end()) {
            it->second.sti = sti;
        } else {
            attention_values_[atom] = AttentionValue(sti, 0.0f, 0.0f);
        }
        updateAttentionalFocus();
    }
    
    /**
     * Update LTI for an atom
     */
    void updateLTI(Handle atom, float lti) {
        if (!atom) return;
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = attention_values_.find(atom);
        if (it != attention_values_.end()) {
            it->second.lti = lti;
        } else {
            attention_values_[atom] = AttentionValue(0.0f, lti, 0.0f);
        }
    }
    
    /**
     * Update VLTI for an atom
     */
    void updateVLTI(Handle atom, float vlti) {
        if (!atom) return;
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = attention_values_.find(atom);
        if (it != attention_values_.end()) {
            it->second.vlti = vlti;
        } else {
            attention_values_[atom] = AttentionValue(0.0f, 0.0f, vlti);
        }
    }
    
    /**
     * Get the attentional focus (atoms with highest STI)
     */
    std::vector<Handle> getAttentionalFocus() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return attentional_focus_;
    }
    
    /**
     * Get atoms above STI threshold
     */
    std::vector<Handle> getAtomsAboveSTI(float threshold) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Handle> result;
        for (const auto& [atom, av] : attention_values_) {
            if (av.sti >= threshold) {
                result.push_back(atom);
            }
        }
        return result;
    }
    
    /**
     * Get atoms above LTI threshold
     */
    std::vector<Handle> getAtomsAboveLTI(float threshold) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Handle> result;
        for (const auto& [atom, av] : attention_values_) {
            if (av.lti >= threshold) {
                result.push_back(atom);
            }
        }
        return result;
    }
    
    /**
     * Get top N atoms by STI
     */
    std::vector<std::pair<Handle, float>> getTopSTI(size_t n) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::pair<Handle, float>> atoms;
        for (const auto& [atom, av] : attention_values_) {
            atoms.push_back({atom, av.sti});
        }
        
        // Sort by STI descending
        std::partial_sort(atoms.begin(), 
                         atoms.begin() + std::min(n, atoms.size()),
                         atoms.end(),
                         [](const auto& a, const auto& b) {
                             return a.second > b.second;
                         });
        
        if (atoms.size() > n) {
            atoms.resize(n);
        }
        return atoms;
    }
    
    /**
     * Get top N atoms by LTI
     */
    std::vector<std::pair<Handle, float>> getTopLTI(size_t n) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::pair<Handle, float>> atoms;
        for (const auto& [atom, av] : attention_values_) {
            atoms.push_back({atom, av.lti});
        }
        
        // Sort by LTI descending
        std::partial_sort(atoms.begin(), 
                         atoms.begin() + std::min(n, atoms.size()),
                         atoms.end(),
                         [](const auto& a, const auto& b) {
                             return a.second > b.second;
                         });
        
        if (atoms.size() > n) {
            atoms.resize(n);
        }
        return atoms;
    }
    
    /**
     * Decay all STI values by a factor
     */
    void decaySTI(float factor = 0.9f) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& [atom, av] : attention_values_) {
            av.sti *= factor;
        }
        updateAttentionalFocus();
    }
    
    /**
     * Transfer STI from one atom to another
     */
    void transferSTI(Handle from, Handle to, float amount) {
        if (!from || !to) return;
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it_from = attention_values_.find(from);
        auto it_to = attention_values_.find(to);
        
        if (it_from != attention_values_.end()) {
            it_from->second.sti -= amount;
            if (it_to != attention_values_.end()) {
                it_to->second.sti += amount;
            } else {
                attention_values_[to] = AttentionValue(amount, 0.0f, 0.0f);
            }
        }
        updateAttentionalFocus();
    }
    
    /**
     * Set maximum size of attentional focus
     */
    void setMaxAFSize(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        max_af_size_ = size;
        updateAttentionalFocus();
    }
    
    /**
     * Set minimum STI threshold for attentional focus
     */
    void setMinSTIThreshold(float threshold) {
        std::lock_guard<std::mutex> lock(mutex_);
        min_sti_threshold_ = threshold;
        updateAttentionalFocus();
    }
    
    /**
     * Remove attention value for an atom
     */
    void remove(Handle atom) {
        std::lock_guard<std::mutex> lock(mutex_);
        attention_values_.erase(atom);
        updateAttentionalFocus();
    }
    
    /**
     * Clear all attention values
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        attention_values_.clear();
        attentional_focus_.clear();
    }
    
    /**
     * Get number of atoms being tracked
     */
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return attention_values_.size();
    }
    
private:
    /**
     * Update the attentional focus based on current STI values
     * Must be called with mutex locked
     */
    void updateAttentionalFocus() {
        attentional_focus_.clear();
        
        // Collect atoms above threshold
        std::vector<std::pair<Handle, float>> candidates;
        for (const auto& [atom, av] : attention_values_) {
            if (av.sti >= min_sti_threshold_) {
                candidates.push_back({atom, av.sti});
            }
        }
        
        // Sort by STI descending
        std::sort(candidates.begin(), candidates.end(),
                  [](const auto& a, const auto& b) {
                      return a.second > b.second;
                  });
        
        // Take top max_af_size_ atoms
        size_t count = std::min(max_af_size_, candidates.size());
        for (size_t i = 0; i < count; ++i) {
            attentional_focus_.push_back(candidates[i].first);
        }
    }
    
    mutable std::mutex mutex_;
    std::map<Handle, AttentionValue> attention_values_;
    std::vector<Handle> attentional_focus_;
    size_t max_af_size_;
    float min_sti_threshold_;
};

} // namespace atomspace
} // namespace at
