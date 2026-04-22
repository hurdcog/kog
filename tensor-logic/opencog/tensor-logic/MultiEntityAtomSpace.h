/*
 * MultiEntityAtomSpace.h - Multi-Entity AtomSpace Management
 *
 * Manages tensor representations across multiple cognitive entities,
 * enabling distributed cognition and entity synchronization.
 */

#ifndef _OPENCOG_MULTI_ENTITY_ATOMSPACE_H
#define _OPENCOG_MULTI_ENTITY_ATOMSPACE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace opencog {
namespace tensor_logic {

class MultiEntityAtomSpace {
public:
    struct Entity {
        std::string id;
        std::string name;
        std::vector<float> embedding;
        std::unordered_map<std::string, std::vector<float>> atom_cache;
    };

    MultiEntityAtomSpace() = default;
    ~MultiEntityAtomSpace() = default;

    void registerEntity(const std::string& id, const std::string& name,
                       const std::vector<float>& initial_embedding);
    void unregisterEntity(const std::string& id);

    bool hasEntity(const std::string& id) const;
    Entity getEntity(const std::string& id) const;
    std::vector<std::string> getAllEntityIds() const;

    void updateEntityEmbedding(const std::string& id, const std::vector<float>& embedding);
    void cacheAtomTensor(const std::string& entity_id, const std::string& atom_id,
                        const std::vector<float>& tensor);

    std::vector<float> getCachedAtomTensor(const std::string& entity_id,
                                           const std::string& atom_id) const;

    std::vector<float> computeEntityConsensus(const std::vector<std::string>& entity_ids) const;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, Entity> entities_;
};

} // namespace tensor_logic
} // namespace opencog

#endif // _OPENCOG_MULTI_ENTITY_ATOMSPACE_H
