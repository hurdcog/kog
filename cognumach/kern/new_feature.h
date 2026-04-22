/*
 * Kernel Feature Integration Module
 * Core kernel functionality for integrated feature management
 * 
 * Copyright (C) 2024 Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any later
 * version.
 */

#ifndef _KERN_NEW_FEATURE_H_
#define _KERN_NEW_FEATURE_H_

#include <kern/kern_types.h>
#include <kern/lock.h>
#include <mach/kern_return.h>

/*
 * Feature states for kernel integration
 */
typedef enum {
    FEATURE_STATE_DISABLED = 0,
    FEATURE_STATE_INITIALIZING,
    FEATURE_STATE_ENABLED,
    FEATURE_STATE_ERROR,
    FEATURE_STATE_MAX
} feature_state_t;

/*
 * Feature capabilities and configuration flags
 */
#define FEATURE_CAP_MEMORY_EFFICIENT    0x01
#define FEATURE_CAP_LOW_LATENCY        0x02
#define FEATURE_CAP_X86_SUPPORT        0x04
#define FEATURE_CAP_X86_64_SUPPORT     0x08
#define FEATURE_CAP_QEMU_SUPPORT       0x10

/*
 * Core feature data structure for kernel integration
 */
struct kernel_feature {
    feature_state_t state;              /* Current feature state */
    uint32_t capabilities;              /* Feature capability flags */
    uint32_t enabled_count;             /* Number of enable calls */
    uint32_t error_count;               /* Number of errors */
    uint64_t last_operation_time;       /* Last operation timestamp */
    simple_lock_data_t lock;            /* Feature synchronization */
};

/*
 * Feature operation statistics
 */
struct feature_stats {
    uint64_t init_calls;                /* Initialization call count */
    uint64_t enable_calls;              /* Enable operation count */
    uint64_t disable_calls;             /* Disable operation count */
    uint64_t total_operations;          /* Total operation count */
    uint64_t avg_latency_us;            /* Average operation latency */
    uint64_t max_latency_us;            /* Maximum operation latency */
};

/*
 * Global feature instance
 */
extern struct kernel_feature global_kernel_feature;
extern struct feature_stats global_feature_stats;

/*
 * Feature management function prototypes
 */

/*
 * Initialize the kernel feature subsystem
 * Called during kernel startup
 */
void feature_init(void);

/*
 * Enable kernel feature functionality
 * Returns KERN_SUCCESS on success, error code on failure
 */
kern_return_t feature_enable(void);

/*
 * Disable kernel feature functionality
 * Returns KERN_SUCCESS on success, error code on failure
 */
kern_return_t feature_disable(void);

/*
 * Get current feature state
 * Returns current feature_state_t value
 */
feature_state_t feature_get_state(void);

/*
 * Get feature statistics
 * Returns pointer to current statistics structure
 */
struct feature_stats *feature_get_stats(void);

/*
 * Check if feature is enabled and operational
 * Returns TRUE if enabled, FALSE otherwise
 */
boolean_t feature_is_enabled(void);

/*
 * Reset feature statistics
 * Clears all counters and statistics
 */
void feature_reset_stats(void);

#endif /* _KERN_NEW_FEATURE_H_ */