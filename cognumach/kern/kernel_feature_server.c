/*
 * Kernel Feature Integration System Call Server Implementation
 * Server-side implementation of kernel feature management system calls
 * 
 * Copyright (C) 2024 Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any later
 * version.
 */

#include <kern/new_feature.h>
#include <kern/host.h>
#include <mach/kern_return.h>
#include <mach/mach_types.h>

/*
 * Enable kernel feature functionality
 */
kern_return_t
kernel_feature_enable(
    host_t host)
{
    if (host == HOST_NULL)
        return KERN_INVALID_ARGUMENT;
        
    return feature_enable();
}

/*
 * Disable kernel feature functionality  
 */
kern_return_t
kernel_feature_disable(
    host_t host)
{
    if (host == HOST_NULL)
        return KERN_INVALID_ARGUMENT;
        
    return feature_disable();
}

/*
 * Get current feature state
 */
kern_return_t
kernel_feature_get_state(
    host_t host,
    uint32_t *state)
{
    if (host == HOST_NULL || state == NULL)
        return KERN_INVALID_ARGUMENT;
        
    *state = (uint32_t)feature_get_state();
    return KERN_SUCCESS;
}

/*
 * Get feature statistics
 */
kern_return_t
kernel_feature_get_stats(
    host_t host,
    uint64_t *init_calls,
    uint64_t *enable_calls,
    uint64_t *disable_calls,
    uint64_t *total_operations,
    uint64_t *avg_latency_us,
    uint64_t *max_latency_us)
{
    struct feature_stats *stats;
    
    if (host == HOST_NULL)
        return KERN_INVALID_ARGUMENT;
        
    if (init_calls == NULL || enable_calls == NULL || 
        disable_calls == NULL || total_operations == NULL ||
        avg_latency_us == NULL || max_latency_us == NULL)
        return KERN_INVALID_ARGUMENT;
        
    stats = feature_get_stats();
    
    *init_calls = stats->init_calls;
    *enable_calls = stats->enable_calls;
    *disable_calls = stats->disable_calls;
    *total_operations = stats->total_operations;
    *avg_latency_us = stats->avg_latency_us;
    *max_latency_us = stats->max_latency_us;
    
    return KERN_SUCCESS;
}

/*
 * Check if feature is enabled
 */
kern_return_t
kernel_feature_is_enabled(
    host_t host,
    boolean_t *enabled)
{
    if (host == HOST_NULL || enabled == NULL)
        return KERN_INVALID_ARGUMENT;
        
    *enabled = feature_is_enabled();
    return KERN_SUCCESS;
}

/*
 * Reset feature statistics
 */
kern_return_t
kernel_feature_reset_stats(
    host_t host)
{
    if (host == HOST_NULL)
        return KERN_INVALID_ARGUMENT;
        
    feature_reset_stats();
    return KERN_SUCCESS;
}