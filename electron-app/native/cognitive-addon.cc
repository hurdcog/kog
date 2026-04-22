// OpenCog Inferno AGI - Native Cognitive Addon
// Self-contained AtomSpace implementation for Electron desktop app
// Bridges Electron with Inferno Kernel Cognitive Services
//
// This addon provides a fully functional in-process AtomSpace with:
// - Atom creation, retrieval, and querying
// - Truth value management (strength + confidence)
// - Attention value management (STI/LTI)
// - Pattern matching via type-based queries
// - PLN-style probabilistic inference
// - URE-style unified rule engine inference
// - Attentional focus management
//
// Uses the built-in implementation with real cognitive processing.

#include <napi.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Forward declarations for bridge functions
namespace atomspace_bridge {
    Napi::Value CreateAtom(const Napi::CallbackInfo& info);
    Napi::Value GetAtom(const Napi::CallbackInfo& info);
    Napi::Value QueryAtomSpace(const Napi::CallbackInfo& info);
    Napi::Value GetAtomSpaceStats(const Napi::CallbackInfo& info);
    Napi::Value DeleteAtom(const Napi::CallbackInfo& info);
    Napi::Value ClearAtomSpace(const Napi::CallbackInfo& info);
}

namespace attention_bridge {
    Napi::Value StimulateAtom(const Napi::CallbackInfo& info);
    Napi::Value GetAttentionalFocus(const Napi::CallbackInfo& info);
    Napi::Value SetAttentionThreshold(const Napi::CallbackInfo& info);
}

namespace inference_bridge {
    Napi::Value InferPLN(const Napi::CallbackInfo& info);
    Napi::Value InferURE(const Napi::CallbackInfo& info);
}

// Initialize the addon
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    std::cout << "[OCC] Initializing OpenCog Cognitive Addon (built-in AtomSpace)..." << std::endl;

    // AtomSpace operations
    exports.Set("createAtom", Napi::Function::New(env, atomspace_bridge::CreateAtom));
    exports.Set("getAtom", Napi::Function::New(env, atomspace_bridge::GetAtom));
    exports.Set("queryAtomSpace", Napi::Function::New(env, atomspace_bridge::QueryAtomSpace));
    exports.Set("getAtomSpaceStats", Napi::Function::New(env, atomspace_bridge::GetAtomSpaceStats));
    exports.Set("deleteAtom", Napi::Function::New(env, atomspace_bridge::DeleteAtom));
    exports.Set("clearAtomSpace", Napi::Function::New(env, atomspace_bridge::ClearAtomSpace));

    // Attention operations
    exports.Set("stimulateAtom", Napi::Function::New(env, attention_bridge::StimulateAtom));
    exports.Set("getAttentionalFocus", Napi::Function::New(env, attention_bridge::GetAttentionalFocus));
    exports.Set("setAttentionThreshold", Napi::Function::New(env, attention_bridge::SetAttentionThreshold));

    // Inference operations
    exports.Set("inferPLN", Napi::Function::New(env, inference_bridge::InferPLN));
    exports.Set("inferURE", Napi::Function::New(env, inference_bridge::InferURE));

    // Version info
    exports.Set("version", Napi::String::New(env, "1.0.0"));
    exports.Set("engine", Napi::String::New(env, "built-in"));

    std::cout << "[OCC] Cognitive Addon initialized successfully (engine: built-in)" << std::endl;

    return exports;
}

NODE_API_MODULE(cognitive_addon, Init)
