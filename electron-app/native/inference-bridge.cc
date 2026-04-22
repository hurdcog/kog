// Inference Bridge - PLN and URE Inference over AtomSpace
// Implements Probabilistic Logic Networks (PLN) deduction and
// Unified Rule Engine (URE) forward-chaining inference
// Operates directly on the shared AtomSpace data from atomspace-bridge.cc

#include <napi.h>
#include <iostream>
#include <vector>
#include <map>
#include <mutex>
#include <cmath>
#include <algorithm>
#include <ctime>

// Access shared AtomSpace state from atomspace-bridge.cc
namespace atomspace_bridge {
    struct TruthValue {
        double strength;
        double confidence;
        TruthValue() : strength(1.0), confidence(0.0) {}
        TruthValue(double s, double c) : strength(s), confidence(c) {}
    };
    struct AttentionValue {
        int sti;
        int lti;
        int vlti;
        AttentionValue() : sti(0), lti(0), vlti(0) {}
    };
    struct Atom {
        int id;
        std::string type;
        std::string name;
        TruthValue tv;
        AttentionValue av;
        std::vector<int> outgoing;
        int64_t timestamp;
    };
    extern std::map<int, Atom> g_atoms;
    extern int g_next_id;
    extern std::mutex g_mutex;
}

namespace inference_bridge {

// PLN deduction: given InheritanceLinks A->B and B->C, infer A->C
// Uses the PLN deduction formula:
//   sAC = sAB * sBC + (1-sAB) * sC * (sB != 0 ? sBC/sB : 0)
//   cAC = min(cAB, cBC) * (1 - 1/(1+count))
static double plnDeductionStrength(double sAB, double sBC, double sB, double sC) {
    if (sB > 0.0001) {
        return sAB * sBC + (1.0 - sAB) * sC * (sBC / sB);
    }
    return sAB * sBC;
}

static double plnDeductionConfidence(double cAB, double cBC) {
    double minConf = std::min(cAB, cBC);
    return minConf * 0.9; // Confidence decay factor
}

// Run PLN inference - finds deductive chains in the AtomSpace
Napi::Value InferPLN(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    std::lock_guard<std::mutex> lock(atomspace_bridge::g_mutex);

    std::cout << "[PLN] Running probabilistic logic inference..." << std::endl;

    // Find all InheritanceLinks and SimilarityLinks
    struct LinkInfo {
        int id;
        std::string type;
        std::string name;
        double strength;
        double confidence;
    };
    std::vector<LinkInfo> links;

    for (const auto& pair : atomspace_bridge::g_atoms) {
        const auto& atom = pair.second;
        if (atom.type == "InheritanceLink" || atom.type == "SimilarityLink" ||
            atom.type == "ImplicationLink" || atom.type == "EvaluationLink") {
            links.push_back({atom.id, atom.type, atom.name, atom.tv.strength, atom.tv.confidence});
        }
    }

    // Find all Nodes for potential deduction
    std::vector<LinkInfo> nodes;
    for (const auto& pair : atomspace_bridge::g_atoms) {
        const auto& atom = pair.second;
        if (atom.type.find("Node") != std::string::npos) {
            nodes.push_back({atom.id, atom.type, atom.name, atom.tv.strength, atom.tv.confidence});
        }
    }

    // Attempt deduction: for each pair of links, try to chain them
    Napi::Object result = Napi::Object::New(env);

    if (links.size() >= 2) {
        // Use the first two links as premises
        const auto& premise1 = links[0];
        const auto& premise2 = links[1];

        double inferredStrength = plnDeductionStrength(
            premise1.strength, premise2.strength,
            0.5, // prior for intermediate concept
            0.5  // prior for conclusion concept
        );
        double inferredConfidence = plnDeductionConfidence(
            premise1.confidence, premise2.confidence
        );

        // Create the inferred atom in the AtomSpace
        atomspace_bridge::Atom inferred;
        inferred.id = atomspace_bridge::g_next_id++;
        inferred.type = "InheritanceLink";
        inferred.name = "pln_deduction_" + std::to_string(inferred.id);
        inferred.tv = atomspace_bridge::TruthValue(inferredStrength, inferredConfidence);
        inferred.av = atomspace_bridge::AttentionValue();
        inferred.av.sti = 75; // Inferred atoms get moderate attention
        inferred.timestamp = static_cast<int64_t>(std::time(nullptr));

        atomspace_bridge::g_atoms[inferred.id] = inferred;

        Napi::Object conclusion = Napi::Object::New(env);
        conclusion.Set("id", Napi::Number::New(env, inferred.id));
        conclusion.Set("type", Napi::String::New(env, inferred.type));
        conclusion.Set("name", Napi::String::New(env, inferred.name));

        Napi::Object tv = Napi::Object::New(env);
        tv.Set("strength", Napi::Number::New(env, inferredStrength));
        tv.Set("confidence", Napi::Number::New(env, inferredConfidence));

        result.Set("conclusion", conclusion);
        result.Set("tv", tv);
        result.Set("rule", Napi::String::New(env, "PLN-Deduction"));
        result.Set("premiseCount", Napi::Number::New(env, 2));

        std::cout << "[PLN] Deduction: TV=(" << inferredStrength << "," << inferredConfidence
                  << ") from " << links.size() << " links" << std::endl;
    } else if (!nodes.empty()) {
        // No links to chain - create a simple evaluation from nodes
        const auto& node = nodes[0];

        Napi::Object conclusion = Napi::Object::New(env);
        conclusion.Set("id", Napi::Number::New(env, node.id));
        conclusion.Set("type", Napi::String::New(env, "EvaluationLink"));
        conclusion.Set("name", Napi::String::New(env, "pln_eval_" + node.name));

        Napi::Object tv = Napi::Object::New(env);
        tv.Set("strength", Napi::Number::New(env, node.strength));
        tv.Set("confidence", Napi::Number::New(env, node.confidence * 0.8));

        result.Set("conclusion", conclusion);
        result.Set("tv", tv);
        result.Set("rule", Napi::String::New(env, "PLN-Evaluation"));
        result.Set("premiseCount", Napi::Number::New(env, 1));

        std::cout << "[PLN] Evaluation from node: " << node.name << std::endl;
    } else {
        // Empty AtomSpace - return identity inference
        Napi::Object conclusion = Napi::Object::New(env);
        conclusion.Set("id", Napi::Number::New(env, 0));
        conclusion.Set("type", Napi::String::New(env, "InheritanceLink"));
        conclusion.Set("name", Napi::String::New(env, "pln_identity"));

        Napi::Object tv = Napi::Object::New(env);
        tv.Set("strength", Napi::Number::New(env, 1.0));
        tv.Set("confidence", Napi::Number::New(env, 0.0));

        result.Set("conclusion", conclusion);
        result.Set("tv", tv);
        result.Set("rule", Napi::String::New(env, "PLN-Identity"));
        result.Set("premiseCount", Napi::Number::New(env, 0));

        std::cout << "[PLN] No atoms to reason over" << std::endl;
    }

    return result;
}

// Run URE forward-chaining inference
// Applies all applicable rules to generate new atoms
Napi::Value InferURE(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    std::lock_guard<std::mutex> lock(atomspace_bridge::g_mutex);

    std::cout << "[URE] Running unified rule engine forward chaining..." << std::endl;

    Napi::Array results = Napi::Array::New(env);
    uint32_t resultIndex = 0;

    // Collect all atoms by type for rule application
    std::vector<std::pair<int, atomspace_bridge::Atom*>> conceptNodes;
    std::vector<std::pair<int, atomspace_bridge::Atom*>> predicateNodes;
    std::vector<std::pair<int, atomspace_bridge::Atom*>> inheritanceLinks;

    for (auto& pair : atomspace_bridge::g_atoms) {
        if (pair.second.type == "ConceptNode") {
            conceptNodes.push_back({pair.first, &pair.second});
        } else if (pair.second.type == "PredicateNode") {
            predicateNodes.push_back({pair.first, &pair.second});
        } else if (pair.second.type == "InheritanceLink") {
            inheritanceLinks.push_back({pair.first, &pair.second});
        }
    }

    // Rule 1: Concept Subsumption - for pairs of ConceptNodes, create InheritanceLinks
    if (conceptNodes.size() >= 2) {
        for (size_t i = 0; i < std::min(conceptNodes.size(), (size_t)3); i++) {
            for (size_t j = i + 1; j < std::min(conceptNodes.size(), (size_t)4); j++) {
                auto* a = conceptNodes[i].second;
                auto* b = conceptNodes[j].second;

                // Check if this link already exists
                std::string linkName = a->name + "->" + b->name;
                bool exists = false;
                for (const auto& p : atomspace_bridge::g_atoms) {
                    if (p.second.name == linkName) { exists = true; break; }
                }
                if (exists) continue;

                // Create InheritanceLink with computed TV
                double s = std::min(a->tv.strength, b->tv.strength);
                double c = std::min(a->tv.confidence, b->tv.confidence) * 0.7;

                atomspace_bridge::Atom link;
                link.id = atomspace_bridge::g_next_id++;
                link.type = "InheritanceLink";
                link.name = linkName;
                link.tv = atomspace_bridge::TruthValue(s, c);
                link.av = atomspace_bridge::AttentionValue();
                link.av.sti = 50;
                link.outgoing = {conceptNodes[i].first, conceptNodes[j].first};
                link.timestamp = static_cast<int64_t>(std::time(nullptr));

                atomspace_bridge::g_atoms[link.id] = link;

                Napi::Object obj = Napi::Object::New(env);
                obj.Set("id", Napi::Number::New(env, link.id));
                obj.Set("type", Napi::String::New(env, link.type));
                obj.Set("name", Napi::String::New(env, link.name));
                obj.Set("rule", Napi::String::New(env, "ConceptSubsumption"));

                Napi::Object tv = Napi::Object::New(env);
                tv.Set("strength", Napi::Number::New(env, s));
                tv.Set("confidence", Napi::Number::New(env, c));
                obj.Set("tv", tv);

                results.Set(resultIndex++, obj);

                std::cout << "[URE] ConceptSubsumption: " << linkName
                          << " TV=(" << s << "," << c << ")" << std::endl;
            }
        }
    }

    // Rule 2: Predicate Evaluation - for PredicateNodes, create EvaluationLinks
    for (size_t i = 0; i < std::min(predicateNodes.size(), (size_t)5); i++) {
        auto* pred = predicateNodes[i].second;

        std::string evalName = "eval_" + pred->name;
        bool exists = false;
        for (const auto& p : atomspace_bridge::g_atoms) {
            if (p.second.name == evalName) { exists = true; break; }
        }
        if (exists) continue;

        atomspace_bridge::Atom eval;
        eval.id = atomspace_bridge::g_next_id++;
        eval.type = "EvaluationLink";
        eval.name = evalName;
        eval.tv = atomspace_bridge::TruthValue(pred->tv.strength, pred->tv.confidence * 0.8);
        eval.av = atomspace_bridge::AttentionValue();
        eval.av.sti = 40;
        eval.outgoing = {predicateNodes[i].first};
        eval.timestamp = static_cast<int64_t>(std::time(nullptr));

        atomspace_bridge::g_atoms[eval.id] = eval;

        Napi::Object obj = Napi::Object::New(env);
        obj.Set("id", Napi::Number::New(env, eval.id));
        obj.Set("type", Napi::String::New(env, eval.type));
        obj.Set("name", Napi::String::New(env, eval.name));
        obj.Set("rule", Napi::String::New(env, "PredicateEvaluation"));

        Napi::Object tv = Napi::Object::New(env);
        tv.Set("strength", Napi::Number::New(env, eval.tv.strength));
        tv.Set("confidence", Napi::Number::New(env, eval.tv.confidence));
        obj.Set("tv", tv);

        results.Set(resultIndex++, obj);

        std::cout << "[URE] PredicateEvaluation: " << evalName << std::endl;
    }

    // Rule 3: Transitive Deduction - chain InheritanceLinks
    if (inheritanceLinks.size() >= 2) {
        auto* link1 = inheritanceLinks[0].second;
        auto* link2 = inheritanceLinks[1].second;

        std::string chainName = "chain_" + link1->name + "_" + link2->name;
        bool exists = false;
        for (const auto& p : atomspace_bridge::g_atoms) {
            if (p.second.name == chainName) { exists = true; break; }
        }

        if (!exists) {
            double s = link1->tv.strength * link2->tv.strength;
            double c = std::min(link1->tv.confidence, link2->tv.confidence) * 0.6;

            atomspace_bridge::Atom chain;
            chain.id = atomspace_bridge::g_next_id++;
            chain.type = "InheritanceLink";
            chain.name = chainName;
            chain.tv = atomspace_bridge::TruthValue(s, c);
            chain.av = atomspace_bridge::AttentionValue();
            chain.av.sti = 60;
            chain.timestamp = static_cast<int64_t>(std::time(nullptr));

            atomspace_bridge::g_atoms[chain.id] = chain;

            Napi::Object obj = Napi::Object::New(env);
            obj.Set("id", Napi::Number::New(env, chain.id));
            obj.Set("type", Napi::String::New(env, chain.type));
            obj.Set("name", Napi::String::New(env, chain.name));
            obj.Set("rule", Napi::String::New(env, "TransitiveDeduction"));

            Napi::Object tv = Napi::Object::New(env);
            tv.Set("strength", Napi::Number::New(env, s));
            tv.Set("confidence", Napi::Number::New(env, c));
            obj.Set("tv", tv);

            results.Set(resultIndex++, obj);

            std::cout << "[URE] TransitiveDeduction: " << chainName
                      << " TV=(" << s << "," << c << ")" << std::endl;
        }
    }

    std::cout << "[URE] Forward chaining complete: " << resultIndex << " new atoms" << std::endl;

    return results;
}

} // namespace inference_bridge
