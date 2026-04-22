#pragma once

#include <torch/torch.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace at {
namespace atomspace {

// Forward declarations
class AtomSpace;

/**
 * Atom - Base class for all atoms in ATenSpace
 * 
 * An Atom is the fundamental unit of knowledge representation.
 * It can be either a Node (entity) or a Link (relationship).
 * Atoms are immutable and globally unique within an AtomSpace.
 */
class Atom : public std::enable_shared_from_this<Atom> {
public:
    using Handle = std::shared_ptr<Atom>;
    using WeakHandle = std::weak_ptr<Atom>;
    
    enum class Type {
        NODE,
        LINK,
        CONCEPT_NODE,
        PREDICATE_NODE,
        VARIABLE_NODE,
        INHERITANCE_LINK,
        IMPLICATION_LINK,
        EVALUATION_LINK,
        LIST_LINK,
        ORDERED_LINK,
        UNORDERED_LINK,
        // Logical links
        AND_LINK,
        OR_LINK,
        NOT_LINK,
        // Set membership links
        MEMBER_LINK,
        SUBSET_LINK,
        // Contextual links
        CONTEXT_LINK,
        // Temporal links
        SEQUENTIAL_LINK,
        SIMULTANEOUS_LINK,
        // Similarity links
        SIMILARITY_LINK,
        // Execution links
        EXECUTION_LINK,
        // Hebbian links (for ECAN attention spreading)
        HEBBIAN_LINK,
        SYMMETRIC_HEBBIAN_LINK,
        ASYMMETRIC_HEBBIAN_LINK,
        INVERSE_HEBBIAN_LINK
    };
    
    virtual ~Atom() = default;
    
    // Type information
    virtual Type getType() const = 0;
    virtual std::string getTypeName() const = 0;
    virtual bool isNode() const = 0;
    virtual bool isLink() const = 0;
    
    // Identity
    size_t getHash() const { return hash_; }
    
    // Truth value (stored as tensor for flexible representation)
    void setTruthValue(const Tensor& tv) { truth_value_ = tv; }
    Tensor getTruthValue() const { return truth_value_; }
    
    // Attention value (importance)
    void setAttention(float attention) { attention_ = attention; }
    float getAttention() const { return attention_; }
    
    // Incoming set (atoms that reference this atom)
    const std::vector<WeakHandle>& getIncomingSet() const { return incoming_set_; }
    void addIncoming(WeakHandle atom) { incoming_set_.push_back(atom); }
    
    // String representation
    virtual std::string toString() const = 0;
    
    // Equality
    virtual bool equals(const Atom& other) const = 0;
    
protected:
    Atom() : hash_(0), attention_(0.0f) {
        // Default truth value: [strength=1.0, confidence=1.0]
        truth_value_ = torch::tensor({1.0f, 1.0f});
    }
    
    void computeHash(const std::string& content) {
        hash_ = std::hash<std::string>{}(content);
    }
    
    size_t hash_;
    Tensor truth_value_;
    float attention_;
    std::vector<WeakHandle> incoming_set_;
};

/**
 * Node - Represents an entity or concept
 * 
 * Nodes are named atoms that represent concepts, entities, or values.
 * They can optionally contain a tensor representation for embeddings.
 */
class Node : public Atom {
public:
    Node(Type type, const std::string& name) 
        : type_(type), name_(name) {
        computeHash(getTypeName() + ":" + name_);
    }
    
    Node(Type type, const std::string& name, const Tensor& embedding)
        : type_(type), name_(name), embedding_(embedding) {
        computeHash(getTypeName() + ":" + name_);
    }
    
    Type getType() const override { return type_; }
    
    std::string getTypeName() const override {
        switch(type_) {
            case Type::NODE: return "Node";
            case Type::CONCEPT_NODE: return "ConceptNode";
            case Type::PREDICATE_NODE: return "PredicateNode";
            case Type::VARIABLE_NODE: return "VariableNode";
            default: return "Node";
        }
    }
    
    bool isNode() const override { return true; }
    bool isLink() const override { return false; }
    
    std::string getName() const { return name_; }
    
    // Tensor embedding for the node
    void setEmbedding(const Tensor& embedding) { embedding_ = embedding; }
    Tensor getEmbedding() const { return embedding_; }
    bool hasEmbedding() const { return embedding_.defined(); }
    
    std::string toString() const override {
        return "(" + getTypeName() + " \"" + name_ + "\")";
    }
    
    bool equals(const Atom& other) const override {
        if (!other.isNode()) return false;
        const Node& otherNode = static_cast<const Node&>(other);
        return type_ == otherNode.type_ && name_ == otherNode.name_;
    }
    
private:
    Type type_;
    std::string name_;
    Tensor embedding_;
};

/**
 * Link - Represents a relationship between atoms
 * 
 * Links connect atoms (nodes or other links) together, forming a hypergraph.
 * They maintain an ordered list of outgoing atoms.
 */
class Link : public Atom {
public:
    using OutgoingSet = std::vector<Handle>;
    
    Link(Type type, const OutgoingSet& outgoing)
        : type_(type), outgoing_(outgoing) {
        computeHashFromOutgoing();
    }
    
    Type getType() const override { return type_; }
    
    std::string getTypeName() const override {
        switch(type_) {
            case Type::LINK: return "Link";
            case Type::INHERITANCE_LINK: return "InheritanceLink";
            case Type::IMPLICATION_LINK: return "ImplicationLink";
            case Type::EVALUATION_LINK: return "EvaluationLink";
            case Type::LIST_LINK: return "ListLink";
            case Type::ORDERED_LINK: return "OrderedLink";
            case Type::UNORDERED_LINK: return "UnorderedLink";
            case Type::AND_LINK: return "AndLink";
            case Type::OR_LINK: return "OrLink";
            case Type::NOT_LINK: return "NotLink";
            case Type::MEMBER_LINK: return "MemberLink";
            case Type::SUBSET_LINK: return "SubsetLink";
            case Type::CONTEXT_LINK: return "ContextLink";
            case Type::SEQUENTIAL_LINK: return "SequentialLink";
            case Type::SIMULTANEOUS_LINK: return "SimultaneousLink";
            case Type::SIMILARITY_LINK: return "SimilarityLink";
            case Type::EXECUTION_LINK: return "ExecutionLink";
            default: return "Link";
        }
    }
    
    bool isNode() const override { return false; }
    bool isLink() const override { return true; }
    
    const OutgoingSet& getOutgoingSet() const { return outgoing_; }
    size_t getArity() const { return outgoing_.size(); }
    
    Handle getOutgoingAtom(size_t index) const {
        if (index < outgoing_.size()) {
            return outgoing_[index];
        }
        return nullptr;
    }
    
    std::string toString() const override {
        std::string result = "(" + getTypeName();
        for (const auto& atom : outgoing_) {
            result += "\n  " + atom->toString();
        }
        result += ")";
        return result;
    }
    
    bool equals(const Atom& other) const override {
        if (!other.isLink()) return false;
        const Link& otherLink = static_cast<const Link&>(other);
        if (type_ != otherLink.type_) return false;
        if (outgoing_.size() != otherLink.outgoing_.size()) return false;
        for (size_t i = 0; i < outgoing_.size(); ++i) {
            if (!outgoing_[i]->equals(*otherLink.outgoing_[i])) {
                return false;
            }
        }
        return true;
    }
    
private:
    void computeHashFromOutgoing() {
        std::string content = getTypeName() + ":";
        for (const auto& atom : outgoing_) {
            content += std::to_string(atom->getHash()) + ",";
        }
        computeHash(content);
    }
    
    Type type_;
    OutgoingSet outgoing_;
};

} // namespace atomspace
} // namespace at
