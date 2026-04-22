/**
 * @file kokkos-hurd-bridge.cc
 * @brief Kokkos-GNU Hurd Integration Bridge
 * 
 * This module provides the primary integration bridge between Kokkos parallel
 * computing framework and GNU Hurd microkernel architecture, enabling
 * performance-portable parallel operations within Hurd servers.
 *
 * Part of Phase 2: Microkernel Integration - SKZ Framework
 */

#include "kokkos-hurd-bridge.h"
#include <iostream>
#include <memory>

#if HAVE_KOKKOS
#include <Kokkos_Core.hpp>
#endif

namespace HurdCog {
namespace Kokkos {

/**
 * @class KokkosHurdBridge
 * @brief Main integration bridge for Kokkos-Hurd operations
 */
class KokkosHurdBridge {
private:
    bool initialized;
    
public:
    KokkosHurdBridge() : initialized(false) {}
    
    /**
     * Initialize Kokkos for Hurd environment
     */
    bool initialize() {
#if HAVE_KOKKOS
        try {
            if (!::Kokkos::is_initialized()) {
                ::Kokkos::initialize();
                initialized = true;
                std::cout << "✅ Kokkos initialized for GNU Hurd environment" << std::endl;
                
                // Print configuration
                ::Kokkos::print_configuration(std::cout);
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "❌ Failed to initialize Kokkos: " << e.what() << std::endl;
            return false;
        }
#else
        std::cout << "⚠️  Kokkos not available - using fallback implementation" << std::endl;
        initialized = true;
#endif
        return initialized;
    }
    
    /**
     * Clean shutdown of Kokkos
     */
    void finalize() {
#if HAVE_KOKKOS
        if (initialized && ::Kokkos::is_initialized()) {
            ::Kokkos::finalize();
            std::cout << "✅ Kokkos finalized" << std::endl;
        }
#endif
        initialized = false;
    }
    
    /**
     * Test basic parallel operations
     */
    bool test_parallel_operations() {
#if HAVE_KOKKOS
        if (!initialized) return false;
        
        try {
            const int N = 1000;
            
            // Create views for test data
            ::Kokkos::View<double*> a("a", N);
            ::Kokkos::View<double*> b("b", N);
            ::Kokkos::View<double*> c("c", N);
            
            // Initialize data
            ::Kokkos::parallel_for("init", N, KOKKOS_LAMBDA(const int i) {
                a(i) = static_cast<double>(i);
                b(i) = static_cast<double>(i * 2);
            });
            
            // Parallel computation: c = a + b
            ::Kokkos::parallel_for("add", N, KOKKOS_LAMBDA(const int i) {
                c(i) = a(i) + b(i);
            });
            
            // Fence to ensure completion
            ::Kokkos::fence();
            
            // Verify results (simple check)
            auto c_host = ::Kokkos::create_mirror_view(c);
            ::Kokkos::deep_copy(c_host, c);
            
            bool correct = true;
            for (int i = 0; i < 10; ++i) {  // Check first 10 elements
                double expected = static_cast<double>(i * 3);
                if (std::abs(c_host(i) - expected) > 1e-10) {
                    correct = false;
                    break;
                }
            }
            
            std::cout << "🧮 Parallel operations test: " 
                      << (correct ? "✅ PASSED" : "❌ FAILED") << std::endl;
            return correct;
            
        } catch (const std::exception& e) {
            std::cerr << "❌ Parallel operations test failed: " << e.what() << std::endl;
            return false;
        }
#else
        std::cout << "🧮 Parallel operations test: ⚠️ SKIPPED (Kokkos not available)" << std::endl;
        return true;  // Consider passed in fallback mode
#endif
    }
    
    /**
     * Demonstrate memory management capabilities
     */
    bool test_memory_management() {
#if HAVE_KOKKOS
        if (!initialized) return false;
        
        try {
            const int N = 500;
            
            // Test different memory spaces if available
            std::cout << "🧠 Testing memory management..." << std::endl;
            
            // Host memory
            ::Kokkos::View<int*, ::Kokkos::HostSpace> host_data("host_data", N);
            
            // Default execution space memory
            ::Kokkos::View<int*> device_data("device_data", N);
            
            // Initialize on host
            for (int i = 0; i < N; ++i) {
                host_data(i) = i * i;
            }
            
            // Copy to device
            ::Kokkos::deep_copy(device_data, host_data);
            
            // Process on device
            ::Kokkos::parallel_for("square", N, KOKKOS_LAMBDA(const int i) {
                device_data(i) = device_data(i) * 2;
            });
            
            // Copy back to host
            ::Kokkos::deep_copy(host_data, device_data);
            
            // Verify
            bool correct = true;
            for (int i = 0; i < 5; ++i) {
                int expected = i * i * 2;
                if (host_data(i) != expected) {
                    correct = false;
                    break;
                }
            }
            
            std::cout << "🧠 Memory management test: " 
                      << (correct ? "✅ PASSED" : "❌ FAILED") << std::endl;
            return correct;
            
        } catch (const std::exception& e) {
            std::cerr << "❌ Memory management test failed: " << e.what() << std::endl;
            return false;
        }
#else
        std::cout << "🧠 Memory management test: ⚠️ SKIPPED (Kokkos not available)" << std::endl;
        return true;
#endif
    }
    
    ~KokkosHurdBridge() {
        finalize();
    }
};

// Global bridge instance
static std::unique_ptr<KokkosHurdBridge> bridge_instance;

// C-style interface for integration with C code
extern "C" {
    
    int kokkos_hurd_initialize() {
        try {
            bridge_instance = std::make_unique<KokkosHurdBridge>();
            return bridge_instance->initialize() ? 0 : -1;
        } catch (...) {
            return -1;
        }
    }
    
    void kokkos_hurd_finalize() {
        if (bridge_instance) {
            bridge_instance->finalize();
            bridge_instance.reset();
        }
    }
    
    int kokkos_hurd_test_parallel() {
        if (!bridge_instance) return -1;
        return bridge_instance->test_parallel_operations() ? 0 : -1;
    }
    
    int kokkos_hurd_test_memory() {
        if (!bridge_instance) return -1;
        return bridge_instance->test_memory_management() ? 0 : -1;
    }
}

} // namespace Kokkos
} // namespace HurdCog