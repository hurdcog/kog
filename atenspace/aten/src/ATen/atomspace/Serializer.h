#pragma once

#include "Atom.h"
#include "AtomSpace.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace at {
namespace atomspace {

/**
 * Serializer - Save and load AtomSpace to/from files
 * 
 * Provides persistence support for AtomSpace, allowing knowledge graphs
 * to be saved to disk and loaded back. Uses a simple text-based format
 * for readability and portability.
 * 
 * Limitations:
 * - Embeddings are not serialized (too large for text format)
 * - Links referencing other links use hash references which are not
 *   fully restored during deserialization. For best results, ensure
 *   all referenced atoms are nodes.
 * - For production use, consider implementing binary serialization
 *   with full graph structure preservation.
 */
class Serializer {
public:
    using Handle = Atom::Handle;
    
    /**
     * Save AtomSpace to a file
     * Returns true on success
     */
    static bool save(const AtomSpace& space, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Write header
        file << "# ATenSpace Serialization Format v1.0\n";
        file << "# Atom count: " << space.size() << "\n\n";
        
        // Get all atoms
        auto atoms = space.getAtoms();
        
        // First pass: write all nodes
        file << "# Nodes\n";
        for (const auto& atom : atoms) {
            if (atom->isNode()) {
                auto node = std::dynamic_pointer_cast<Node>(atom);
                if (node) {
                    writeNode(file, node);
                }
            }
        }
        
        file << "\n# Links\n";
        // Second pass: write all links
        for (const auto& atom : atoms) {
            if (atom->isLink()) {
                auto link = std::dynamic_pointer_cast<Link>(atom);
                if (link) {
                    writeLink(file, link);
                }
            }
        }
        
        file.close();
        return true;
    }
    
    /**
     * Load AtomSpace from a file
     * Returns true on success
     */
    static bool load(AtomSpace& space, const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Parse and create atom
            if (line.find("NODE:") == 0) {
                parseAndCreateNode(space, line);
            } else if (line.find("LINK:") == 0) {
                parseAndCreateLink(space, line);
            }
        }
        
        file.close();
        return true;
    }
    
    /**
     * Export AtomSpace to string representation
     */
    static std::string toString(const AtomSpace& space) {
        std::ostringstream oss;
        
        auto atoms = space.getAtoms();
        
        // Write nodes
        for (const auto& atom : atoms) {
            if (atom->isNode()) {
                oss << atom->toString() << "\n";
            }
        }
        
        // Write links
        for (const auto& atom : atoms) {
            if (atom->isLink()) {
                oss << atom->toString() << "\n";
            }
        }
        
        return oss.str();
    }
    
private:
    /**
     * Write a node to file
     */
    static void writeNode(std::ofstream& file, const std::shared_ptr<Node>& node) {
        file << "NODE:" << typeToString(node->getType()) 
             << ":" << node->getName();
        
        // Write truth value
        auto tv = node->getTruthValue();
        if (tv.defined() && tv.numel() >= 2) {
            file << ":TV:" << tv[0].item<float>() << "," << tv[1].item<float>();
        }
        
        // Write attention value
        float attention = node->getAttention();
        if (attention != 0.0f) {
            file << ":ATT:" << attention;
        }
        
        // Note: Embeddings are not serialized in text format
        // They could be saved separately in binary format if needed
        
        file << "\n";
    }
    
    /**
     * Write a link to file
     */
    static void writeLink(std::ofstream& file, const std::shared_ptr<Link>& link) {
        file << "LINK:" << typeToString(link->getType());
        
        // Write outgoing set (referenced by node names or hashes)
        file << ":OUT:[";
        const auto& outgoing = link->getOutgoingSet();
        for (size_t i = 0; i < outgoing.size(); ++i) {
            if (i > 0) file << ",";
            file << atomReference(outgoing[i]);
        }
        file << "]";
        
        // Write truth value
        auto tv = link->getTruthValue();
        if (tv.defined() && tv.numel() >= 2) {
            file << ":TV:" << tv[0].item<float>() << "," << tv[1].item<float>();
        }
        
        // Write attention value
        float attention = link->getAttention();
        if (attention != 0.0f) {
            file << ":ATT:" << attention;
        }
        
        file << "\n";
    }
    
    /**
     * Create a reference string for an atom
     */
    static std::string atomReference(const Handle& atom) {
        if (atom->isNode()) {
            auto node = std::dynamic_pointer_cast<Node>(atom);
            return typeToString(atom->getType()) + ":" + node->getName();
        } else {
            // For links, use hash
            return "HASH:" + std::to_string(atom->getHash());
        }
    }
    
    /**
     * Parse and create a node from a line
     */
    static void parseAndCreateNode(AtomSpace& space, const std::string& line) {
        // Format: NODE:TYPE:NAME[:TV:strength,confidence][:ATT:value]
        std::vector<std::string> parts = split(line, ':');
        if (parts.size() < 3) return;
        
        Atom::Type type = stringToType(parts[1]);
        std::string name = parts[2];
        
        // Create node
        Handle node = space.addNode(type, name);
        
        // Parse optional fields
        for (size_t i = 3; i < parts.size(); i += 2) {
            if (i + 1 >= parts.size()) break;
            
            if (parts[i] == "TV") {
                auto tvParts = split(parts[i + 1], ',');
                if (tvParts.size() >= 2) {
                    float strength = std::stof(tvParts[0]);
                    float confidence = std::stof(tvParts[1]);
                    node->setTruthValue(torch::tensor({strength, confidence}));
                }
            } else if (parts[i] == "ATT") {
                float attention = std::stof(parts[i + 1]);
                node->setAttention(attention);
            }
        }
    }
    
    /**
     * Parse and create a link from a line
     */
    static void parseAndCreateLink(AtomSpace& space, const std::string& line) {
        // Format: LINK:TYPE:OUT:[ref1,ref2,...][:TV:strength,confidence][:ATT:value]
        std::vector<std::string> parts = split(line, ':');
        if (parts.size() < 3) return;
        
        Atom::Type type = stringToType(parts[1]);
        
        // Parse outgoing set
        std::vector<Handle> outgoing;
        if (parts[2] == "OUT") {
            std::string outStr = parts[3];
            // Remove brackets
            if (outStr.front() == '[') outStr = outStr.substr(1);
            if (outStr.back() == ']') outStr.pop_back();
            
            auto refs = split(outStr, ',');
            for (const auto& ref : refs) {
                // Parse reference (TYPE:NAME or HASH:value)
                auto refParts = split(ref, ':');
                if (refParts.size() >= 2) {
                    if (refParts[0] == "HASH") {
                        // Skip hash references for now
                        // Would need a hash->atom lookup table
                    } else {
                        // Node reference
                        Atom::Type nodeType = stringToType(refParts[0]);
                        std::string nodeName = refParts[1];
                        Handle atom = space.getNode(nodeType, nodeName);
                        if (atom) {
                            outgoing.push_back(atom);
                        }
                    }
                }
            }
        }
        
        if (!outgoing.empty()) {
            Handle link = space.addLink(type, outgoing);
            
            // Parse optional fields
            for (size_t i = 4; i < parts.size(); i += 2) {
                if (i + 1 >= parts.size()) break;
                
                if (parts[i] == "TV") {
                    auto tvParts = split(parts[i + 1], ',');
                    if (tvParts.size() >= 2) {
                        float strength = std::stof(tvParts[0]);
                        float confidence = std::stof(tvParts[1]);
                        link->setTruthValue(torch::tensor({strength, confidence}));
                    }
                } else if (parts[i] == "ATT") {
                    float attention = std::stof(parts[i + 1]);
                    link->setAttention(attention);
                }
            }
        }
    }
    
    /**
     * Convert type to string
     */
    static std::string typeToString(Atom::Type type) {
        switch(type) {
            case Atom::Type::NODE: return "Node";
            case Atom::Type::CONCEPT_NODE: return "ConceptNode";
            case Atom::Type::PREDICATE_NODE: return "PredicateNode";
            case Atom::Type::VARIABLE_NODE: return "VariableNode";
            case Atom::Type::LINK: return "Link";
            case Atom::Type::INHERITANCE_LINK: return "InheritanceLink";
            case Atom::Type::EVALUATION_LINK: return "EvaluationLink";
            case Atom::Type::LIST_LINK: return "ListLink";
            case Atom::Type::ORDERED_LINK: return "OrderedLink";
            case Atom::Type::UNORDERED_LINK: return "UnorderedLink";
            case Atom::Type::AND_LINK: return "AndLink";
            case Atom::Type::OR_LINK: return "OrLink";
            case Atom::Type::NOT_LINK: return "NotLink";
            case Atom::Type::MEMBER_LINK: return "MemberLink";
            case Atom::Type::SUBSET_LINK: return "SubsetLink";
            case Atom::Type::CONTEXT_LINK: return "ContextLink";
            case Atom::Type::SEQUENTIAL_LINK: return "SequentialLink";
            case Atom::Type::SIMULTANEOUS_LINK: return "SimultaneousLink";
            case Atom::Type::SIMILARITY_LINK: return "SimilarityLink";
            case Atom::Type::EXECUTION_LINK: return "ExecutionLink";
            default: return "Unknown";
        }
    }
    
    /**
     * Convert string to type
     */
    static Atom::Type stringToType(const std::string& str) {
        if (str == "Node") return Atom::Type::NODE;
        if (str == "ConceptNode") return Atom::Type::CONCEPT_NODE;
        if (str == "PredicateNode") return Atom::Type::PREDICATE_NODE;
        if (str == "VariableNode") return Atom::Type::VARIABLE_NODE;
        if (str == "Link") return Atom::Type::LINK;
        if (str == "InheritanceLink") return Atom::Type::INHERITANCE_LINK;
        if (str == "EvaluationLink") return Atom::Type::EVALUATION_LINK;
        if (str == "ListLink") return Atom::Type::LIST_LINK;
        if (str == "OrderedLink") return Atom::Type::ORDERED_LINK;
        if (str == "UnorderedLink") return Atom::Type::UNORDERED_LINK;
        if (str == "AndLink") return Atom::Type::AND_LINK;
        if (str == "OrLink") return Atom::Type::OR_LINK;
        if (str == "NotLink") return Atom::Type::NOT_LINK;
        if (str == "MemberLink") return Atom::Type::MEMBER_LINK;
        if (str == "SubsetLink") return Atom::Type::SUBSET_LINK;
        if (str == "ContextLink") return Atom::Type::CONTEXT_LINK;
        if (str == "SequentialLink") return Atom::Type::SEQUENTIAL_LINK;
        if (str == "SimultaneousLink") return Atom::Type::SIMULTANEOUS_LINK;
        if (str == "SimilarityLink") return Atom::Type::SIMILARITY_LINK;
        if (str == "ExecutionLink") return Atom::Type::EXECUTION_LINK;
        return Atom::Type::NODE;
    }
    
    /**
     * Split string by delimiter
     */
    static std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
};

} // namespace atomspace
} // namespace at
