/**
 * @file performance-optimizer.h
 * @brief Performance Optimization and Tuning Header
 * 
 * Header for the comprehensive performance optimization module implementing
 * Phase 5: Month 17 requirements for algorithmic optimization, resource tuning,
 * and performance validation.
 * 
 * @author HurdCog Development Team
 * @date 2025
 * @version 1.0
 */

#ifndef HURDCOG_PERFORMANCE_OPTIMIZER_H
#define HURDCOG_PERFORMANCE_OPTIMIZER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize performance optimization system
 * @return 1 on success, 0 on failure
 */
int performance_optimizer_init(void);

/**
 * @brief Start performance optimization
 * @return 1 on success, 0 on failure
 */
int performance_optimizer_start(void);

/**
 * @brief Stop performance optimization
 * @return 1 on success, 0 on failure
 */
int performance_optimizer_stop(void);

/**
 * @brief Run performance benchmark and validation
 * @return 1 if validation passed, 0 if failed
 */
int performance_optimizer_benchmark(void);

/**
 * @brief Cleanup performance optimization system
 */
void performance_optimizer_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif // HURDCOG_PERFORMANCE_OPTIMIZER_H