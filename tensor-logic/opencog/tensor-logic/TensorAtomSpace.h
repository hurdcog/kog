/*
 * TensorAtomSpace.h - Tensor-Enhanced AtomSpace Operations
 *
 * Provides mathematical tensor operations on AtomSpace structures,
 * enabling neural-symbolic integration.
 */

#ifndef _OPENCOG_TENSOR_ATOMSPACE_H
#define _OPENCOG_TENSOR_ATOMSPACE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <functional>

namespace opencog {
namespace tensor_logic {

class TensorAtomSpace {
public:
    struct TensorAtom {
        std::string id;
        std::string type;
        std::vector<float> embedding;
        std::vector<float> truth_value;  // [strength, confidence]
        std::vector<std::string> outgoing;
    };

    explicit TensorAtomSpace(size_t embedding_dim = 128);
    ~TensorAtomSpace() = default;

    // Atom-Tensor conversion
    std::vector<float> atomToTensor(const std::string& atom_id);
    std::string tensorToAtom(const std::vector<float>& tensor);
    std::vector<float> linkToTensor(const std::string& link_id);

    // Tensor operations on atoms
    std::vector<float> computeInheritance(const std::string& child, const std::string& parent);
    std::vector<float> computeSimilarity(const std::string& atom1, const std::string& atom2);
    std::vector<float> computeContextual(const std::string& atom, const std::vector<std::string>& context);

    // Truth value operations
    std::vector<float> tensorToTruthValue(const std::vector<float>& tensor);
    std::vector<float> truthValueToTensor(float strength, float confidence);
    std::vector<float> mergeTruthValues(const std::vector<float>& tv1, const std::vector<float>& tv2);

    // Atom management
    void registerAtom(const TensorAtom& atom);
    void updateAtomEmbedding(const std::string& atom_id, const std::vector<float>& embedding);
    TensorAtom getAtom(const std::string& atom_id) const;
    bool hasAtom(const std::string& atom_id) const;

    // Query operations
    std::vector<std::string> findSimilar(const std::vector<float>& query, size_t k = 10);
    std::vector<std::string> findByType(const std::string& type);

    // Embedding operations
    std::vector<float> averageEmbedding(const std::vector<std::string>& atom_ids);
    std::vector<float> weightedEmbedding(const std::vector<std::string>& atom_ids,
                                         const std::vector<float>& weights);

private:
    size_t embedding_dim_;
    mutable std::mutex mutex_;
    std::unordered_map<std::string, TensorAtom> atoms_;

    std::vector<float> hashToEmbedding(const std::string& str);
    float computeCosineSimilarity(const std::vector<float>& a, const std::vector<float>& b);
};

} // namespace tensor_logic
} // namespace opencog

#endif // _OPENCOG_TENSOR_ATOMSPACE_H
