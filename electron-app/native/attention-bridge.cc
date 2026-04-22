// Attention Bridge - ECAN Attention Allocation over AtomSpace
// Provides real STI/LTI management and attentional focus queries
// Operates directly on the shared AtomSpace data from atomspace-bridge.cc

#include <napi.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <mutex>

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
    extern std::mutex g_mutex;
    extern int g_attention_threshold;
}

namespace attention_bridge {

// Stimulate atom with STI - updates the real AtomSpace
Napi::Value StimulateAtom(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsNumber()) {
        Napi::TypeError::New(env, "Expected atomId and amount as numbers")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    int atomId = info[0].As<Napi::Number>().Int32Value();
    int amount = info[1].As<Napi::Number>().Int32Value();

    std::lock_guard<std::mutex> lock(atomspace_bridge::g_mutex);

    auto it = atomspace_bridge::g_atoms.find(atomId);
    if (it == atomspace_bridge::g_atoms.end()) {
        std::cout << "[ECAN] Atom " << atomId << " not found" << std::endl;
        return Napi::Boolean::New(env, false);
    }

    it->second.av.sti += amount;
    // Clamp STI to reasonable range
    if (it->second.av.sti > 1000) it->second.av.sti = 1000;
    if (it->second.av.sti < -1000) it->second.av.sti = -1000;

    std::cout << "[ECAN] Stimulated atom " << atomId << " (\"" << it->second.name
              << "\") with " << amount << " STI -> new STI: " << it->second.av.sti << std::endl;

    return Napi::Boolean::New(env, true);
}

// Get attentional focus - returns atoms with STI above threshold, sorted by STI descending
Napi::Value GetAttentionalFocus(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    std::lock_guard<std::mutex> lock(atomspace_bridge::g_mutex);

    // Collect atoms above attention threshold
    struct FocusAtom {
        int id;
        std::string name;
        std::string type;
        int sti;
    };
    std::vector<FocusAtom> focusAtoms;

    for (const auto& pair : atomspace_bridge::g_atoms) {
        const auto& atom = pair.second;
        if (atom.av.sti >= atomspace_bridge::g_attention_threshold) {
            focusAtoms.push_back({atom.id, atom.name, atom.type, atom.av.sti});
        }
    }

    // Sort by STI descending
    std::sort(focusAtoms.begin(), focusAtoms.end(),
              [](const FocusAtom& a, const FocusAtom& b) { return a.sti > b.sti; });

    std::cout << "[ECAN] Attentional focus: " << focusAtoms.size() << " atoms above threshold "
              << atomspace_bridge::g_attention_threshold << std::endl;

    Napi::Array focus = Napi::Array::New(env, focusAtoms.size());
    for (size_t i = 0; i < focusAtoms.size(); i++) {
        Napi::Object atom = Napi::Object::New(env);
        atom.Set("id", Napi::Number::New(env, focusAtoms[i].id));
        atom.Set("name", Napi::String::New(env, focusAtoms[i].name));
        atom.Set("type", Napi::String::New(env, focusAtoms[i].type));
        atom.Set("sti", Napi::Number::New(env, focusAtoms[i].sti));
        focus.Set(static_cast<uint32_t>(i), atom);
    }

    return focus;
}

// Set attention threshold
Napi::Value SetAttentionThreshold(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Expected threshold as number")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    int threshold = info[0].As<Napi::Number>().Int32Value();

    std::lock_guard<std::mutex> lock(atomspace_bridge::g_mutex);
    atomspace_bridge::g_attention_threshold = threshold;

    std::cout << "[ECAN] Attention threshold set to " << threshold << std::endl;

    return Napi::Boolean::New(env, true);
}

} // namespace attention_bridge
