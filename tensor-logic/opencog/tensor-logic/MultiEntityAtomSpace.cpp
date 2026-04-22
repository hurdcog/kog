/*
 * MultiEntityAtomSpace.cpp - Implementation
 */

#include "MultiEntityAtomSpace.h"
#include <algorithm>
#include <cmath>

namespace opencog {
namespace tensor_logic {

void MultiEntityAtomSpace::registerEntity(const std::string& id, const std::string& name,
                                          const std::vector<float>& initial_embedding) {
    std::lock_guard<std::mutex> lock(mutex_);
    Entity entity;
    entity.id = id;
    entity.name = name;
    entity.embedding = initial_embedding;
    entities_[id] = entity;
}

void MultiEntityAtomSpace::unregisterEntity(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    entities_.erase(id);
}

bool MultiEntityAtomSpace::hasEntity(const std::string& id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return entities_.find(id) != entities_.end();
}

MultiEntityAtomSpace::Entity MultiEntityAtomSpace::getEntity(const std::string& id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = entities_.find(id);
    if (it != entities_.end()) {
        return it->second;
    }
    return Entity{};
}

std::vector<std::string> MultiEntityAtomSpace::getAllEntityIds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> ids;
    ids.reserve(entities_.size());
    for (const auto& pair : entities_) {
        ids.push_back(pair.first);
    }
    return ids;
}

void MultiEntityAtomSpace::updateEntityEmbedding(const std::string& id,
                                                  const std::vector<float>& embedding) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = entities_.find(id);
    if (it != entities_.end()) {
        it->second.embedding = embedding;
    }
}

void MultiEntityAtomSpace::cacheAtomTensor(const std::string& entity_id,
                                           const std::string& atom_id,
                                           const std::vector<float>& tensor) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = entities_.find(entity_id);
    if (it != entities_.end()) {
        it->second.atom_cache[atom_id] = tensor;
    }
}

std::vector<float> MultiEntityAtomSpace::getCachedAtomTensor(const std::string& entity_id,
                                                              const std::string& atom_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto entity_it = entities_.find(entity_id);
    if (entity_it != entities_.end()) {
        auto atom_it = entity_it->second.atom_cache.find(atom_id);
        if (atom_it != entity_it->second.atom_cache.end()) {
            return atom_it->second;
        }
    }
    return {};
}

std::vector<float> MultiEntityAtomSpace::computeEntityConsensus(
    const std::vector<std::string>& entity_ids) const {

    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<float> consensus;
    size_t count = 0;

    for (const auto& id : entity_ids) {
        auto it = entities_.find(id);
        if (it != entities_.end() && !it->second.embedding.empty()) {
            if (consensus.empty()) {
                consensus.resize(it->second.embedding.size(), 0.0f);
            }
            for (size_t i = 0; i < consensus.size() && i < it->second.embedding.size(); ++i) {
                consensus[i] += it->second.embedding[i];
            }
            ++count;
        }
    }

    if (count > 0) {
        for (auto& v : consensus) {
            v /= count;
        }
    }

    return consensus;
}

} // namespace tensor_logic
} // namespace opencog
