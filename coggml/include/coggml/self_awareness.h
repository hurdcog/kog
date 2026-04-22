/**
 * Self-Awareness Module
 * Implements self-awareness capabilities for cognitive processes
 */

#ifndef _COGGML_SELF_AWARENESS_H
#define _COGGML_SELF_AWARENESS_H

#include <string>
#include <map>
#include <memory>

namespace coggml {

/**
 * SelfAwareness - Introspection and self-monitoring for cognitive processes
 */
class SelfAwareness {
public:
    SelfAwareness();
    ~SelfAwareness();

    // Update internal state
    void updateState(const std::string& key, const std::string& value);

    // Get internal state
    std::string getState(const std::string& key) const;

    // Get all states
    std::map<std::string, std::string> getAllStates() const;

    // Perform self-reflection
    std::string reflect() const;

    // Assess cognitive performance
    double assessPerformance() const;

    // Generate self-improvement suggestions
    std::string suggestImprovements() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace coggml

#endif // _COGGML_SELF_AWARENESS_H
