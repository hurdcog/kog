/**
 * @file kokkos-hurd-bridge.h
 * @brief Kokkos-GNU Hurd Integration Bridge Header
 * 
 * Provides C and C++ interfaces for integrating Kokkos parallel computing
 * capabilities with GNU Hurd microkernel architecture.
 *
 * Part of Phase 2: Microkernel Integration - SKZ Framework
 */

#ifndef KOKKOS_HURD_BRIDGE_H
#define KOKKOS_HURD_BRIDGE_H

#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize Kokkos for GNU Hurd environment
 * @return 0 on success, -1 on failure
 */
int kokkos_hurd_initialize(void);

/**
 * @brief Finalize Kokkos and clean up resources
 */
void kokkos_hurd_finalize(void);

/**
 * @brief Test parallel operations functionality
 * @return 0 on success, -1 on failure
 */
int kokkos_hurd_test_parallel(void);

/**
 * @brief Test memory management functionality
 * @return 0 on success, -1 on failure
 */
int kokkos_hurd_test_memory(void);

#ifdef __cplusplus
}

// C++ namespace interface
namespace HurdCog {
namespace Kokkos {

/**
 * @brief Memory space abstractions for GNU Hurd
 */
enum class HurdMemorySpace {
    HOST_SPACE,      ///< Host memory space
    DEVICE_SPACE,    ///< Device memory space (if available)
    REMOTE_SPACE     ///< Remote/distributed memory space
};

/**
 * @brief Execution space abstractions for GNU Hurd
 */
enum class HurdExecutionSpace {
    SERIAL,          ///< Serial execution
    OPENMP,          ///< OpenMP parallel execution
    THREADS,         ///< C++ threads execution
    CUDA,            ///< CUDA execution (if available)
    HIP              ///< HIP execution (if available)
};

/**
 * @brief Configuration for Kokkos-Hurd integration
 */
struct KokkosHurdConfig {
    HurdMemorySpace default_memory_space;
    HurdExecutionSpace default_execution_space;
    bool enable_deep_copy;
    bool enable_profiling;
    size_t default_team_size;
    
    KokkosHurdConfig() : 
        default_memory_space(HurdMemorySpace::HOST_SPACE),
        default_execution_space(HurdExecutionSpace::SERIAL),
        enable_deep_copy(true),
        enable_profiling(false),
        default_team_size(1) {}
};

} // namespace Kokkos
} // namespace HurdCog

#endif // __cplusplus

#endif // KOKKOS_HURD_BRIDGE_H