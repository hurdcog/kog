// AtomSpace Bridge - Self-contained AtomSpace Implementation
// Provides a fully functional in-process AtomSpace for the Electron desktop app
// No external OpenCog library dependencies required - this IS the AtomSpace

#include <napi.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <ctime>
#include <algorithm>

namespace atomspace_bridge {

// Core AtomSpace data structures
struct TruthValue {
    double strength;
    double confidence;
    TruthValue() : strength(1.0), confidence(0.0) {}
    TruthValue(double s, double c) : strength(s), confidence(c) {}
};

struct AttentionValue {
    int sti;  // Short-Term Importance
    int lti;  // Long-Term Importance
    int vlti; // Very Long-Term Importance
    AttentionValue() : sti(0), lti(0), vlti(0) {}
};

struct Atom {
    int id;
    std::string type;
    std::string name;
    TruthValue tv;
    AttentionValue av;
    std::vector<int> outgoing; // For links: IDs of outgoing atoms
    int64_t timestamp;         // Creation timestamp
};

// AtomSpace state (thread-safe, shared across bridge modules)
std::map<int, Atom> g_atoms;
int g_next_id = 1;
std::mutex g_mutex;
int g_attention_threshold = 50;

// Create a new atom in the AtomSpace
Napi::Value CreateAtom(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString()) {
        Napi::TypeError::New(env, "Expected (type: string, name: string, strength?: number, confidence?: number)")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string type = info[0].As<Napi::String>().Utf8Value();
    std::string name = info[1].As<Napi::String>().Utf8Value();
    double strength = info.Length() > 2 && info[2].IsNumber() ? info[2].As<Napi::Number>().DoubleValue() : 1.0;
    double confidence = info.Length() > 3 && info[3].IsNumber() ? info[3].As<Napi::Number>().DoubleValue() : 0.9;

    std::lock_guard<std::mutex> lock(g_mutex);

    // Check for duplicate (same type + name) - update TV if exists
    for (const auto& pair : g_atoms) {
        if (pair.second.type == type && pair.second.name == name) {
            g_atoms[pair.first].tv = TruthValue(strength, confidence);
            std::cout << "[AtomSpace] Updated existing atom: " << type << " \"" << name << "\" [" << pair.first << "]" << std::endl;
            return Napi::Number::New(env, pair.first);
        }
    }

    // Create new atom
    Atom atom;
    atom.id = g_next_id++;
    atom.type = type;
    atom.name = name;
    atom.tv = TruthValue(strength, confidence);
    atom.av = AttentionValue();
    atom.timestamp = static_cast<int64_t>(std::time(nullptr));

    g_atoms[atom.id] = atom;

    std::cout << "[AtomSpace] Created: (" << type << " \"" << name << "\") [ID:" << atom.id
              << " TV:(" << strength << "," << confidence << ")]" << std::endl;

    return Napi::Number::New(env, atom.id);
}

// Get an atom by ID
Napi::Value GetAtom(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Expected atomId as number")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    int atomId = info[0].As<Napi::Number>().Int32Value();

    std::lock_guard<std::mutex> lock(g_mutex);

    auto it = g_atoms.find(atomId);
    if (it == g_atoms.end()) {
        return env.Null();
    }

    const Atom& atom = it->second;

    Napi::Object result = Napi::Object::New(env);
    result.Set("id", Napi::Number::New(env, atom.id));
    result.Set("type", Napi::String::New(env, atom.type));
    result.Set("name", Napi::String::New(env, atom.name));

    Napi::Object tv = Napi::Object::New(env);
    tv.Set("strength", Napi::Number::New(env, atom.tv.strength));
    tv.Set("confidence", Napi::Number::New(env, atom.tv.confidence));
    result.Set("tv", tv);

    Napi::Object av = Napi::Object::New(env);
    av.Set("sti", Napi::Number::New(env, atom.av.sti));
    av.Set("lti", Napi::Number::New(env, atom.av.lti));
    av.Set("vlti", Napi::Number::New(env, atom.av.vlti));
    result.Set("av", av);

    // Include outgoing set for links
    if (atom.type.find("Link") != std::string::npos && !atom.outgoing.empty()) {
        Napi::Array outgoing = Napi::Array::New(env, atom.outgoing.size());
        for (size_t i = 0; i < atom.outgoing.size(); i++) {
            outgoing.Set(static_cast<uint32_t>(i), Napi::Number::New(env, atom.outgoing[i]));
        }
        result.Set("outgoing", outgoing);
    }

    return result;
}

// Query AtomSpace by type/name pattern
Napi::Value QueryAtomSpace(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    std::string pattern = "";
    if (info.Length() > 0 && info[0].IsString()) {
        pattern = info[0].As<Napi::String>().Utf8Value();
    }

    std::lock_guard<std::mutex> lock(g_mutex);

    std::cout << "[AtomSpace] Querying with pattern: " << (pattern.empty() ? "(all)" : pattern) << std::endl;

    Napi::Array results = Napi::Array::New(env);
    uint32_t index = 0;

    for (const auto& pair : g_atoms) {
        const Atom& atom = pair.second;

        // If pattern is empty, return all atoms
        // Otherwise filter by type or name match
        bool matches = pattern.empty();
        if (!matches) {
            matches = (atom.type.find(pattern) != std::string::npos) ||
                      (atom.name.find(pattern) != std::string::npos) ||
                      (pattern.find(atom.type) != std::string::npos);
        }

        if (matches) {
            Napi::Object binding = Napi::Object::New(env);
            binding.Set("var", Napi::String::New(env, "$X"));

            Napi::Object atomObj = Napi::Object::New(env);
            atomObj.Set("id", Napi::Number::New(env, atom.id));
            atomObj.Set("type", Napi::String::New(env, atom.type));
            atomObj.Set("name", Napi::String::New(env, atom.name));

            Napi::Object tv = Napi::Object::New(env);
            tv.Set("strength", Napi::Number::New(env, atom.tv.strength));
            tv.Set("confidence", Napi::Number::New(env, atom.tv.confidence));
            atomObj.Set("tv", tv);

            binding.Set("atom", atomObj);
            results.Set(index++, binding);

            if (index >= 100) break; // Limit results
        }
    }

    return results;
}

// Get AtomSpace statistics
Napi::Value GetAtomSpaceStats(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    std::lock_guard<std::mutex> lock(g_mutex);

    std::map<std::string, int> typeCounts;
    int nodeCount = 0;
    int linkCount = 0;
    double avgSTI = 0.0;
    int focusCount = 0;

    for (const auto& pair : g_atoms) {
        const Atom& atom = pair.second;
        typeCounts[atom.type]++;

        if (atom.type.find("Node") != std::string::npos) {
            nodeCount++;
        } else if (atom.type.find("Link") != std::string::npos) {
            linkCount++;
        }

        avgSTI += atom.av.sti;
        if (atom.av.sti >= g_attention_threshold) {
            focusCount++;
        }
    }

    if (!g_atoms.empty()) {
        avgSTI /= static_cast<double>(g_atoms.size());
    }

    Napi::Object stats = Napi::Object::New(env);
    stats.Set("totalAtoms", Napi::Number::New(env, static_cast<int>(g_atoms.size())));
    stats.Set("nodes", Napi::Number::New(env, nodeCount));
    stats.Set("links", Napi::Number::New(env, linkCount));
    stats.Set("averageSTI", Napi::Number::New(env, avgSTI));
    stats.Set("attentionalFocusSize", Napi::Number::New(env, focusCount));
    stats.Set("attentionThreshold", Napi::Number::New(env, g_attention_threshold));

    Napi::Object types = Napi::Object::New(env);
    for (const auto& pair : typeCounts) {
        types.Set(pair.first, Napi::Number::New(env, pair.second));
    }
    stats.Set("types", types);

    return stats;
}

// Delete an atom by ID
Napi::Value DeleteAtom(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Expected atomId as number")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    int atomId = info[0].As<Napi::Number>().Int32Value();

    std::lock_guard<std::mutex> lock(g_mutex);

    auto it = g_atoms.find(atomId);
    if (it != g_atoms.end()) {
        std::cout << "[AtomSpace] Deleted: (" << it->second.type << " \"" << it->second.name << "\") [ID:" << atomId << "]" << std::endl;
        g_atoms.erase(it);
        return Napi::Boolean::New(env, true);
    }

    return Napi::Boolean::New(env, false);
}

// Clear the entire AtomSpace
Napi::Value ClearAtomSpace(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    std::lock_guard<std::mutex> lock(g_mutex);

    int count = static_cast<int>(g_atoms.size());
    g_atoms.clear();
    g_next_id = 1;

    std::cout << "[AtomSpace] Cleared " << count << " atoms" << std::endl;

    return Napi::Number::New(env, count);
}

} // namespace atomspace_bridge
