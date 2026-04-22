/**
 * CogGML Microkernel
 * Self-aware cognitive microkernel for distributed cognitive processing
 */

#ifndef _COGGML_MICROKERNEL_H
#define _COGGML_MICROKERNEL_H

#include <memory>
#include <string>
#include <vector>

namespace coggml {

/**
 * CogGML Microkernel - Core cognitive processing unit
 * Manages cognitive shards and coordinates self-aware processes
 */
class Microkernel {
public:
    Microkernel();
    ~Microkernel();

    // Initialize the microkernel
    bool initialize();

    // Shutdown the microkernel
    void shutdown();

    // Process cognitive tasks
    void process();

    // Get microkernel status
    std::string getStatus() const;

    // Check if microkernel is running
    bool isRunning() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace coggml

#endif // _COGGML_MICROKERNEL_H
