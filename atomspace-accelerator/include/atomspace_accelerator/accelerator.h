/**
 * AtomSpace Accelerator
 * Accelerates AtomSpace operations using optimized algorithms
 */

#ifndef _ATOMSPACE_ACCELERATOR_ACCELERATOR_H
#define _ATOMSPACE_ACCELERATOR_ACCELERATOR_H

#include <memory>
#include <string>

namespace atomspace_accelerator {

/**
 * Accelerator - Optimizes AtomSpace operations
 */
class Accelerator {
public:
    Accelerator();
    ~Accelerator();

    // Enable acceleration
    void enable();

    // Disable acceleration
    void disable();

    // Get acceleration factor
    double getAccelerationFactor() const;

    // Update acceleration parameters
    void updateParameters(const std::string& key, double value);

    // Get performance metrics
    std::string getPerformanceMetrics() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace atomspace_accelerator

#endif // _ATOMSPACE_ACCELERATOR_ACCELERATOR_H
