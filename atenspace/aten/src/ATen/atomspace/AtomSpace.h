#pragma once

#include "Atom.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>

namespace at {
namespace atomspace {

/**
 * AtomSpace - Container and manager for atoms
 * 
 * AtomSpace maintains a hypergraph database of atoms (nodes and links).
 * It ensures atom uniqueness, manages atom lifecycle, and provides
 * query capabilities.
 */
class AtomSpace {
public:
    using Handle = Atom::Handle;
    using AtomSet = std::unordered_set<Handle>;
    
    AtomSpace() = default;
    ~AtomSpace() = default;
    
    // Prevent copying
    AtomSpace(const AtomSpace&) = delete;
    AtomSpace& operator=(const AtomSpace&) = delete;
    
    /**
     * Add a node to the atomspace
     * Returns existing node if one with same type and name already exists
     */
    Handle addNode(Atom::Type type, const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Check if node already exists
        std::string key = nodeKey(type, name);
        auto it = node_index_.find(key);
        if (it != node_index_.end()) {
            return it->second;
        }
        
        // Create new node
        Handle node = std::make_shared<Node>(type, name);
        node_index_[key] = node;
        atoms_.insert(node);
        
        return node;
    }
    
    /**
     * Add a node with tensor embedding
     */
    Handle addNode(Atom::Type type, const std::string& name, const Tensor& embedding) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string key = nodeKey(type, name);
        auto it = node_index_.find(key);
        if (it != node_index_.end()) {
            // Update embedding if node exists
            auto nodePtr = std::dynamic_pointer_cast<Node>(it->second);
            if (nodePtr) {
                nodePtr->setEmbedding(embedding);
            }
            return it->second;
        }
        
        Handle node = std::make_shared<Node>(type, name, embedding);
        node_index_[key] = node;
        atoms_.insert(node);
        
        return node;
    }
    
    /**
     * Add a link to the atomspace
     * Returns existing link if one with same type and outgoing set already exists
     */
    Handle addLink(Atom::Type type, const std::vector<Handle>& outgoing) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Create temporary link to compute hash
        Handle link = std::make_shared<Link>(type, outgoing);
        size_t hash = link->getHash();
        
        // Check if link already exists
        auto range = link_index_.equal_range(hash);
        for (auto it = range.first; it != range.second; ++it) {
            if (it->second->equals(*link)) {
                return it->second;
            }
        }
        
        // Add new link
        link_index_.insert({hash, link});
        atoms_.insert(link);
        
        // Update incoming sets
        for (const auto& atom : outgoing) {
            atom->addIncoming(link);
        }
        
        return link;
    }
    
    /**
     * Get a node by type and name
     */
    Handle getNode(Atom::Type type, const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string key = nodeKey(type, name);
        auto it = node_index_.find(key);
        return (it != node_index_.end()) ? it->second : nullptr;
    }
    
    /**
     * Remove an atom from the atomspace
     * Returns true if atom was removed
     */
    bool removeAtom(Handle atom) {
        if (!atom) return false;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Check if atom has incoming links
        if (!atom->getIncomingSet().empty()) {
            return false; // Cannot remove atom that is referenced
        }
        
        // If this is a link, remove references from outgoing atoms' incoming sets
        if (atom->isLink()) {
            auto linkPtr = std::dynamic_pointer_cast<Link>(atom);
            if (linkPtr) {
                for (const auto& outgoingAtom : linkPtr->getOutgoingSet()) {
                    // Note: We can't directly modify incoming sets from here
                    // since they use weak_ptr. The weak_ptrs will naturally
                    // become invalid when the atom is destroyed.
                }
            }
        }
        
        if (atom->isNode()) {
            auto nodePtr = std::dynamic_pointer_cast<Node>(atom);
            if (nodePtr) {
                std::string key = nodeKey(atom->getType(), nodePtr->getName());
                node_index_.erase(key);
            }
        } else {
            size_t hash = atom->getHash();
            auto range = link_index_.equal_range(hash);
            for (auto it = range.first; it != range.second; ++it) {
                if (it->second == atom) {
                    link_index_.erase(it);
                    break;
                }
            }
        }
        
        atoms_.erase(atom);
        return true;
    }
    
    /**
     * Get all atoms in the atomspace
     */
    AtomSet getAtoms() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return atoms_;
    }
    
    /**
     * Get atoms by type
     */
    std::vector<Handle> getAtomsByType(Atom::Type type) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Handle> result;
        for (const auto& atom : atoms_) {
            if (atom->getType() == type) {
                result.push_back(atom);
            }
        }
        return result;
    }
    
    /**
     * Get the number of atoms
     */
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return atoms_.size();
    }
    
    /**
     * Clear all atoms
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        atoms_.clear();
        node_index_.clear();
        link_index_.clear();
    }
    
    /**
     * Query atoms matching a pattern
     * Returns atoms whose embeddings are similar to the query tensor
     */
    std::vector<std::pair<Handle, float>> querySimilar(
        const Tensor& query, 
        size_t k = 10, 
        float threshold = 0.0) const {
        
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::pair<Handle, float>> results;
        
        // Collect all nodes with embeddings
        std::vector<Handle> embeddingNodes;
        std::vector<Tensor> embeddings;
        
        for (const auto& atom : atoms_) {
            if (atom->isNode()) {
                auto nodePtr = std::dynamic_pointer_cast<Node>(atom);
                if (nodePtr && nodePtr->hasEmbedding()) {
                    embeddingNodes.push_back(atom);
                    embeddings.push_back(nodePtr->getEmbedding());
                }
            }
        }
        
        if (embeddings.empty()) {
            return results;
        }
        
        // Batch compute similarities for efficiency
        Tensor embeddingStack = torch::stack(embeddings);
        Tensor queryExpanded = query.unsqueeze(0);
        
        // Compute cosine similarity for all embeddings at once
        Tensor similarities = torch::cosine_similarity(
            queryExpanded.expand({embeddingStack.size(0), -1}),
            embeddingStack,
            /*dim=*/1
        );
        
        // Collect results above threshold
        for (size_t i = 0; i < embeddingNodes.size(); ++i) {
            float similarity = similarities[i].item<float>();
            if (similarity >= threshold) {
                results.push_back({embeddingNodes[i], similarity});
            }
        }
        
        // Sort by similarity (descending)
        std::sort(results.begin(), results.end(),
                  [](const auto& a, const auto& b) {
                      return a.second > b.second;
                  });
        
        // Return top k
        if (results.size() > k) {
            results.resize(k);
        }
        
        return results;
    }
    
private:
    std::string nodeKey(Atom::Type type, const std::string& name) const {
        return std::to_string(static_cast<int>(type)) + ":" + name;
    }
    
    mutable std::mutex mutex_;
    AtomSet atoms_;
    std::unordered_map<std::string, Handle> node_index_;
    std::unordered_multimap<size_t, Handle> link_index_;
};

} // namespace atomspace
} // namespace at
