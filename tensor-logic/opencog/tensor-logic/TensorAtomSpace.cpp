/*
 * TensorAtomSpace.cpp - Implementation
 */

#include "TensorAtomSpace.h"
#include <cmath>
#include <algorithm>
#include <random>
#include <sstream>

namespace opencog {
namespace tensor_logic {

TensorAtomSpace::TensorAtomSpace(size_t embedding_dim)
    : embedding_dim_(embedding_dim) {}

std::vector<float> TensorAtomSpace::atomToTensor(const std::string& atom_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = atoms_.find(atom_id);
    if (it != atoms_.end() && !it->second.embedding.empty()) {
        return it->second.embedding;
    }

    // Generate embedding from hash
    return hashToEmbedding(atom_id);
}

std::string TensorAtomSpace::tensorToAtom(const std::vector<float>& tensor) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Find most similar existing atom
    float best_similarity = -1.0f;
    std::string best_id;

    for (const auto& pair : atoms_) {
        if (!pair.second.embedding.empty()) {
            float sim = computeCosineSimilarity(tensor, pair.second.embedding);
            if (sim > best_similarity) {
                best_similarity = sim;
                best_id = pair.first;
            }
        }
    }

    // If good match found, return it
    if (best_similarity > 0.9f) {
        return best_id;
    }

    // Otherwise create new atom
    std::ostringstream oss;
    oss << "tensor_atom_" << atoms_.size();
    std::string new_id = oss.str();

    TensorAtom atom;
    atom.id = new_id;
    atom.type = "ConceptNode";
    atom.embedding = tensor;
    atom.truth_value = tensorToTruthValue(tensor);
    atoms_[new_id] = atom;

    return new_id;
}

std::vector<float> TensorAtomSpace::linkToTensor(const std::string& link_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = atoms_.find(link_id);
    if (it != atoms_.end()) {
        // Combine outgoing embeddings
        if (!it->second.outgoing.empty()) {
            std::vector<float> combined(embedding_dim_, 0.0f);
            for (const auto& out_id : it->second.outgoing) {
                auto out_it = atoms_.find(out_id);
                if (out_it != atoms_.end() && !out_it->second.embedding.empty()) {
                    for (size_t i = 0; i < embedding_dim_; ++i) {
                        combined[i] += out_it->second.embedding[i];
                    }
                }
            }
            // Normalize
            float norm = 0.0f;
            for (float v : combined) norm += v * v;
            norm = std::sqrt(norm);
            if (norm > 1e-8f) {
                for (auto& v : combined) v /= norm;
            }
            return combined;
        }
        return it->second.embedding;
    }

    return hashToEmbedding(link_id);
}

std::vector<float> TensorAtomSpace::computeInheritance(const std::string& child,
                                                        const std::string& parent) {
    auto child_emb = atomToTensor(child);
    auto parent_emb = atomToTensor(parent);

    // Inheritance: child should be "contained" in parent concept
    // Compute projection of child onto parent
    float dot = 0.0f;
    float parent_norm = 0.0f;
    for (size_t i = 0; i < embedding_dim_; ++i) {
        dot += child_emb[i] * parent_emb[i];
        parent_norm += parent_emb[i] * parent_emb[i];
    }

    if (parent_norm < 1e-8f) {
        return std::vector<float>(2, 0.5f);
    }

    float projection = dot / parent_norm;
    float strength = std::max(0.0f, std::min(1.0f, (projection + 1.0f) / 2.0f));
    float confidence = std::min(1.0f, std::sqrt(parent_norm));

    return {strength, confidence};
}

std::vector<float> TensorAtomSpace::computeSimilarity(const std::string& atom1,
                                                       const std::string& atom2) {
    auto emb1 = atomToTensor(atom1);
    auto emb2 = atomToTensor(atom2);

    float similarity = computeCosineSimilarity(emb1, emb2);

    // Convert to truth value
    float strength = (similarity + 1.0f) / 2.0f;  // Map [-1,1] to [0,1]
    float confidence = 0.9f;  // High confidence for direct comparison

    return {strength, confidence};
}

std::vector<float> TensorAtomSpace::computeContextual(const std::string& atom,
                                                       const std::vector<std::string>& context) {
    if (context.empty()) {
        return atomToTensor(atom);
    }

    auto atom_emb = atomToTensor(atom);
    auto context_emb = averageEmbedding(context);

    // Contextual embedding: element-wise gating
    std::vector<float> result(embedding_dim_);
    for (size_t i = 0; i < embedding_dim_; ++i) {
        float gate = (std::tanh(context_emb[i]) + 1.0f) / 2.0f;
        result[i] = atom_emb[i] * gate;
    }

    return result;
}

std::vector<float> TensorAtomSpace::tensorToTruthValue(const std::vector<float>& tensor) {
    float strength = 0.0f;
    float magnitude = 0.0f;

    for (float v : tensor) {
        strength += v;
        magnitude += v * v;
    }

    if (!tensor.empty()) {
        strength /= tensor.size();
    }
    magnitude = std::sqrt(magnitude);

    // Map to [0, 1]
    strength = (std::tanh(strength) + 1.0f) / 2.0f;
    float confidence = std::min(1.0f, magnitude / std::sqrt(static_cast<float>(tensor.size())));

    return {strength, confidence};
}

std::vector<float> TensorAtomSpace::truthValueToTensor(float strength, float confidence) {
    std::vector<float> tensor(embedding_dim_);

    // Use truth value to modulate random embedding
    std::hash<float> hasher;
    std::mt19937 gen(hasher(strength * 1000.0f + confidence));
    std::normal_distribution<float> dist(0.0f, 1.0f);

    float scale = strength * confidence;
    for (auto& v : tensor) {
        v = dist(gen) * scale;
    }

    return tensor;
}

std::vector<float> TensorAtomSpace::mergeTruthValues(const std::vector<float>& tv1,
                                                      const std::vector<float>& tv2) {
    if (tv1.size() < 2 || tv2.size() < 2) {
        return {0.5f, 0.0f};
    }

    // Revision formula (simplified PLN)
    float s1 = tv1[0], c1 = tv1[1];
    float s2 = tv2[0], c2 = tv2[1];

    float c_new = c1 + c2 - c1 * c2;
    float s_new = (c1 > 0 || c2 > 0) ?
                  (s1 * c1 + s2 * c2 - s1 * c1 * c2 - s2 * c1 * c2 + s1 * s2 * c1 * c2) /
                  (c1 + c2 - c1 * c2) : 0.5f;

    return {s_new, c_new};
}

void TensorAtomSpace::registerAtom(const TensorAtom& atom) {
    std::lock_guard<std::mutex> lock(mutex_);
    atoms_[atom.id] = atom;
}

void TensorAtomSpace::updateAtomEmbedding(const std::string& atom_id,
                                           const std::vector<float>& embedding) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = atoms_.find(atom_id);
    if (it != atoms_.end()) {
        it->second.embedding = embedding;
    }
}

TensorAtomSpace::TensorAtom TensorAtomSpace::getAtom(const std::string& atom_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = atoms_.find(atom_id);
    if (it != atoms_.end()) {
        return it->second;
    }
    return TensorAtom{};
}

bool TensorAtomSpace::hasAtom(const std::string& atom_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return atoms_.find(atom_id) != atoms_.end();
}

std::vector<std::string> TensorAtomSpace::findSimilar(const std::vector<float>& query, size_t k) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::pair<std::string, float>> scores;
    for (const auto& pair : atoms_) {
        if (!pair.second.embedding.empty()) {
            float sim = computeCosineSimilarity(query, pair.second.embedding);
            scores.emplace_back(pair.first, sim);
        }
    }

    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    std::vector<std::string> result;
    for (size_t i = 0; i < k && i < scores.size(); ++i) {
        result.push_back(scores[i].first);
    }

    return result;
}

std::vector<std::string> TensorAtomSpace::findByType(const std::string& type) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::string> result;
    for (const auto& pair : atoms_) {
        if (pair.second.type == type) {
            result.push_back(pair.first);
        }
    }

    return result;
}

std::vector<float> TensorAtomSpace::averageEmbedding(const std::vector<std::string>& atom_ids) {
    std::vector<float> weights(atom_ids.size(), 1.0f);
    return weightedEmbedding(atom_ids, weights);
}

std::vector<float> TensorAtomSpace::weightedEmbedding(const std::vector<std::string>& atom_ids,
                                                       const std::vector<float>& weights) {
    std::vector<float> result(embedding_dim_, 0.0f);
    float total_weight = 0.0f;

    for (size_t i = 0; i < atom_ids.size(); ++i) {
        auto emb = atomToTensor(atom_ids[i]);
        float w = (i < weights.size()) ? weights[i] : 1.0f;
        for (size_t j = 0; j < embedding_dim_; ++j) {
            result[j] += emb[j] * w;
        }
        total_weight += w;
    }

    if (total_weight > 1e-8f) {
        for (auto& v : result) {
            v /= total_weight;
        }
    }

    return result;
}

std::vector<float> TensorAtomSpace::hashToEmbedding(const std::string& str) {
    std::hash<std::string> hasher;
    size_t hash = hasher(str);

    std::vector<float> embedding(embedding_dim_);
    std::mt19937 gen(hash);
    std::normal_distribution<float> dist(0.0f, 1.0f / std::sqrt(embedding_dim_));

    for (auto& v : embedding) {
        v = dist(gen);
    }

    return embedding;
}

float TensorAtomSpace::computeCosineSimilarity(const std::vector<float>& a,
                                                const std::vector<float>& b) {
    if (a.size() != b.size() || a.empty()) return 0.0f;

    float dot = 0.0f, norm_a = 0.0f, norm_b = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }

    float denom = std::sqrt(norm_a) * std::sqrt(norm_b);
    return denom > 1e-8f ? dot / denom : 0.0f;
}

} // namespace tensor_logic
} // namespace opencog
