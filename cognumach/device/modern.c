/*
 * GNU Mach Operating System
 * Copyright (c) 2024 Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * Modern Device Driver API Implementation
 * 
 * Implementation of enhanced driver capabilities with isolation,
 * resource management, and improved error handling.
 */

#include <device/modern.h>
#include <device/ds_routines.h>
#include <kern/printf.h>
#include <kern/debug.h>
#include <kern/time_out.h>
#include <kern/slab.h>
#include <string.h>

/*
 * Memory cache for modern device structures
 */
static struct kmem_cache modern_device_cache;
static boolean_t modern_device_cache_initialized = FALSE;

/*
 * Global lock for modern device operations
 */
decl_simple_lock_data(static, modern_device_lock);
static boolean_t modern_device_lock_initialized = FALSE;

/*
 * Statistics and monitoring
 */
static struct {
    unsigned int total_devices;
    unsigned int active_devices;
    unsigned int total_errors;
    unsigned int recoveries_attempted;
    unsigned int recoveries_successful;
} modern_device_stats;

/*
 * Initialize the modern device subsystem
 */
void
mach_device_modern_init(void)
{
    if (!modern_device_cache_initialized) {
        kmem_cache_init(&modern_device_cache, "mach_device_modern",
                       sizeof(struct mach_device_modern), 0, NULL, 0);
        modern_device_cache_initialized = TRUE;
    }
    
    if (!modern_device_lock_initialized) {
        simple_lock_init(&modern_device_lock);
        modern_device_lock_initialized = TRUE;
    }
    
    /* Initialize statistics */
    memset(&modern_device_stats, 0, sizeof(modern_device_stats));
    
    printf("Modern device driver subsystem initialized\n");
}

/*
 * Create a new modern device structure
 */
mach_device_modern_t
mach_device_modern_create(dev_modern_ops_t ops)
{
    mach_device_modern_t device;
    
    if (!modern_device_cache_initialized) {
        panic("Modern device cache not initialized");
    }
    if (ops == NULL) {
        panic("NULL operations pointer");
    }
    
    /* Allocate device structure */
    device = (mach_device_modern_t)kmem_cache_alloc(&modern_device_cache);
    if (device == MACH_DEVICE_MODERN_NULL) {
        printf("mach_device_modern_create: allocation failed\n");
        return MACH_DEVICE_MODERN_NULL;
    }
    
    /* Initialize basic device structure */
    memset(device, 0, sizeof(struct mach_device_modern));
    
    /* Initialize locks */
    simple_lock_init(&device->basic.ref_lock);
    simple_lock_init(&device->basic.lock);
    simple_lock_init(&device->safety_lock);
    simple_lock_init(&ops->resource_lock);
    
    /* Set up basic fields */
    device->basic.ref_count = 1;
    device->basic.state = DEV_STATE_INIT;
    device->basic.dev_ops = &ops->basic;
    
    /* Set up modern extensions */
    device->modern_ops = ops;
    device->is_modern = TRUE;
    device->validation_cookie = DEV_VALIDATION_COOKIE;
    device->last_validated = 0; /* Will be set by first validation */
    
    /* Initialize health information */
    ops->health.state = DEV_STATE_INITIALIZING;
    ops->health.uptime = 0;
    ops->health.error_count = 0;
    ops->health.recovery_count = 0;
    ops->health.last_health_check = 0;
    
    /* Update statistics */
    simple_lock(&modern_device_lock);
    modern_device_stats.total_devices++;
    modern_device_stats.active_devices++;
    simple_unlock(&modern_device_lock);
    
    printf("Created modern device: vendor=%s, version=%u.%u.%u\n",
           ops->vendor ? ops->vendor : "unknown",
           ops->version.major, ops->version.minor, ops->version.patch);
    
    return device;
}

/*
 * Destroy a modern device structure
 */
void
mach_device_modern_destroy(mach_device_modern_t device)
{
    DEV_VALIDATE_MODERN_DEVICE(device);
    
    /* Mark device as being removed */
    simple_lock(&device->safety_lock);
    device->modern_ops->health.state = DEV_STATE_REMOVING;
    device->validation_cookie = 0; /* Invalidate */
    simple_unlock(&device->safety_lock);
    
    /* Update statistics */
    simple_lock(&modern_device_lock);
    modern_device_stats.active_devices--;
    simple_unlock(&modern_device_lock);
    
    /* Free the device structure */
    kmem_cache_free(&modern_device_cache, (vm_offset_t)device);
}

/*
 * Check resource limits for a device
 */
boolean_t
dev_check_resource_limits(mach_device_modern_t device, 
                         unsigned int resource_type, 
                         unsigned int amount)
{
    boolean_t allowed = TRUE;
    struct dev_resource_limits *limits;
    struct dev_resource_usage *usage;
    
    DEV_VALIDATE_MODERN_DEVICE(device);
    
    limits = &device->modern_ops->limits;
    usage = &device->modern_ops->usage;
    
    simple_lock(&device->modern_ops->resource_lock);
    
    switch (resource_type) {
        case DEV_RESOURCE_MEMORY:
            if (limits->max_memory > 0 && 
                (usage->current_memory + amount) > limits->max_memory) {
                allowed = FALSE;
            }
            break;
            
        case DEV_RESOURCE_IO_OPS:
            if (limits->max_io_ops > 0 && 
                (usage->current_io_ops + amount) > limits->max_io_ops) {
                allowed = FALSE;
            }
            break;
            
        case DEV_RESOURCE_INTERRUPTS:
            if (limits->max_interrupts > 0 && 
                (usage->current_interrupts + amount) > limits->max_interrupts) {
                allowed = FALSE;
            }
            break;
            
        default:
            printf("dev_check_resource_limits: unknown resource type %u\n", resource_type);
            allowed = FALSE;
            break;
    }
    
    simple_unlock(&device->modern_ops->resource_lock);
    
    if (!allowed) {
        printf("Resource limit exceeded for device: type=%u, amount=%u\n", 
               resource_type, amount);
    }
    
    return allowed;
}

/*
 * Update resource usage for a device
 */
void
dev_update_resource_usage(mach_device_modern_t device, 
                         unsigned int resource_type, 
                         int delta)
{
    struct dev_resource_usage *usage;
    
    DEV_VALIDATE_MODERN_DEVICE(device);
    
    usage = &device->modern_ops->usage;
    
    simple_lock(&device->modern_ops->resource_lock);
    
    switch (resource_type) {
        case DEV_RESOURCE_MEMORY:
            if (delta > 0) {
                usage->current_memory += delta;
            } else {
                /* Prevent underflow */
                if (usage->current_memory >= (unsigned int)(-delta)) {
                    usage->current_memory += delta;
                } else {
                    usage->current_memory = 0;
                }
            }
            break;
            
        case DEV_RESOURCE_IO_OPS:
            if (delta > 0) {
                usage->current_io_ops += delta;
            } else {
                if (usage->current_io_ops >= (unsigned int)(-delta)) {
                    usage->current_io_ops += delta;
                } else {
                    usage->current_io_ops = 0;
                }
            }
            break;
            
        case DEV_RESOURCE_INTERRUPTS:
            /* Interrupts are rate-based, handle differently */
            usage->current_interrupts = delta > 0 ? delta : 0;
            break;
            
        default:
            printf("dev_update_resource_usage: unknown resource type %u\n", resource_type);
            break;
    }
    
    simple_unlock(&device->modern_ops->resource_lock);
}

/*
 * Perform health check on a device
 */
kern_return_t
dev_health_check(mach_device_modern_t device)
{
    kern_return_t result = KERN_SUCCESS;
    dev_modern_ops_t ops;
    
    DEV_VALIDATE_MODERN_DEVICE(device);
    
    ops = device->modern_ops;
    
    simple_lock(&device->safety_lock);
    
    /* Update last health check time */
    ops->health.last_health_check = (unsigned int)time.seconds;
    
    /* Call driver-specific health check if available */
    if (ops->d_health_check) {
        int driver_result = ops->d_health_check((dev_t)device->basic.dev_number);
        if (driver_result != 0) {
            result = KERN_FAILURE;
            ops->health.error_count++;
            dev_update_health_state(device, DEV_STATE_ERROR);
        }
    }
    
    /* Check resource usage for anomalies */
    if (ops->usage.current_memory > ops->limits.max_memory * 0.9) {
        printf("Warning: Device memory usage at %u%% of limit\n",
               (ops->usage.current_memory * 100) / ops->limits.max_memory);
    }
    
    simple_unlock(&device->safety_lock);
    
    return result;
}

/*
 * Update device health state
 */
void
dev_update_health_state(mach_device_modern_t device, dev_health_state_t new_state)
{
    dev_health_state_t old_state;
    
    DEV_VALIDATE_MODERN_DEVICE(device);
    
    simple_lock(&device->safety_lock);
    old_state = device->modern_ops->health.state;
    device->modern_ops->health.state = new_state;
    simple_unlock(&device->safety_lock);
    
    if (old_state != new_state) {
        printf("Device health state changed: %u -> %u\n", old_state, new_state);
    }
}

/*
 * Validate a modern device structure
 */
boolean_t
dev_validate_modern_device(mach_device_modern_t device)
{
    boolean_t valid = TRUE;
    
    if (device == MACH_DEVICE_MODERN_NULL) {
        return FALSE;
    }
    
    simple_lock(&device->safety_lock);
    
    /* Check validation cookie */
    if (device->validation_cookie != DEV_VALIDATION_COOKIE) {
        printf("Device validation failed: invalid cookie 0x%x\n", device->validation_cookie);
        valid = FALSE;
        goto out;
    }
    
    /* Check modern flag */
    if (!device->is_modern) {
        printf("Device validation failed: not marked as modern\n");
        valid = FALSE;
        goto out;
    }
    
    /* Check operations pointer */
    if (device->modern_ops == NULL) {
        printf("Device validation failed: NULL operations pointer\n");
        valid = FALSE;
        goto out;
    }
    
    /* Update last validated time */
    device->last_validated = (unsigned int)time.seconds;
    
out:
    simple_unlock(&device->safety_lock);
    return valid;
}

/*
 * Mark device as corrupted
 */
void
dev_mark_device_corrupted(mach_device_modern_t device)
{
    if (device == MACH_DEVICE_MODERN_NULL) {
        return;
    }
    
    simple_lock(&device->safety_lock);
    device->validation_cookie = 0xDEADBEEF; /* Mark as corrupted */
    device->modern_ops->health.state = DEV_STATE_ERROR;
    device->modern_ops->health.error_count++;
    simple_unlock(&device->safety_lock);
    
    printf("Device marked as corrupted\n");
    
    /* Update global statistics */
    simple_lock(&modern_device_lock);
    modern_device_stats.total_errors++;
    simple_unlock(&modern_device_lock);
}

/*
 * Log detailed error information
 */
void
dev_log_error(mach_device_modern_t device, struct dev_error_info *error)
{
    DEV_VALIDATE_MODERN_DEVICE(device);
    MACH_ASSERT(error != NULL, "NULL error info pointer");
    
    simple_lock(&device->safety_lock);
    device->modern_ops->health.error_count++;
    device->modern_ops->usage.total_errors++;
    device->modern_ops->usage.last_error_time = (unsigned int)time.seconds;
    simple_unlock(&device->safety_lock);
    
    printf("Device error: code=%d, extended=0x%x, desc='%s'\n",
           error->basic_error, error->extended_code, 
           error->description[0] ? error->description : "no description");
    
    /* Update global statistics */
    simple_lock(&modern_device_lock);
    modern_device_stats.total_errors++;
    simple_unlock(&modern_device_lock);
}

/*
 * Attempt device recovery
 */
kern_return_t
dev_attempt_recovery(mach_device_modern_t device, unsigned int recovery_type)
{
    kern_return_t result = KERN_FAILURE;
    dev_modern_ops_t ops;
    
    DEV_VALIDATE_MODERN_DEVICE(device);
    
    ops = device->modern_ops;
    
    simple_lock(&device->safety_lock);
    ops->health.recovery_count++;
    dev_update_health_state(device, DEV_STATE_RECOVERY);
    simple_unlock(&device->safety_lock);
    
    /* Update global statistics */
    simple_lock(&modern_device_lock);
    modern_device_stats.recoveries_attempted++;
    simple_unlock(&modern_device_lock);
    
    /* Call driver-specific recovery function if available */
    if (ops->d_recovery) {
        int driver_result = ops->d_recovery((dev_t)device->basic.dev_number, recovery_type);
        if (driver_result == 0) {
            result = KERN_SUCCESS;
            dev_update_health_state(device, DEV_STATE_READY);
            
            simple_lock(&modern_device_lock);
            modern_device_stats.recoveries_successful++;
            simple_unlock(&modern_device_lock);
            
            printf("Device recovery successful: type=%u\n", recovery_type);
        } else {
            printf("Device recovery failed: type=%u, error=%d\n", recovery_type, driver_result);
        }
    } else {
        printf("Device recovery not supported: type=%u\n", recovery_type);
    }
    
    return result;
}

/*
 * Dump device diagnostics
 */
void
dev_dump_diagnostics(mach_device_modern_t device)
{
    dev_modern_ops_t ops;
    struct dev_resource_usage *usage;
    struct dev_health_info *health;
    
    DEV_VALIDATE_MODERN_DEVICE(device);
    
    ops = device->modern_ops;
    usage = &ops->usage;
    health = &ops->health;
    
    printf("\n=== Device Diagnostics ===\n");
    printf("Vendor: %s\n", ops->vendor ? ops->vendor : "unknown");
    printf("Description: %s\n", ops->description ? ops->description : "none");
    printf("Version: %u.%u.%u.%u\n", ops->version.major, ops->version.minor, 
           ops->version.patch, ops->version.build);
    printf("Capabilities: 0x%08x\n", ops->capabilities);
    printf("Health State: %u\n", health->state);
    printf("Uptime: %u seconds\n", health->uptime);
    printf("Error Count: %u\n", health->error_count);
    printf("Recovery Count: %u\n", health->recovery_count);
    printf("Memory Usage: %u KB\n", usage->current_memory);
    printf("I/O Operations: %u\n", usage->current_io_ops);
    printf("Total Operations: %u\n", device->total_operations);
    printf("Failed Operations: %u\n", device->failed_operations);
    printf("Avg Response Time: %u us\n", device->avg_response_time);
    printf("========================\n\n");
}

/*
 * Get performance statistics
 */
kern_return_t
dev_get_performance_stats(mach_device_modern_t device, void *buffer, size_t *size)
{
    struct {
        unsigned int total_ops;
        unsigned int failed_ops;
        unsigned int avg_response_time;
        unsigned int error_count;
        dev_health_state_t health_state;
        struct dev_resource_usage usage;
    } stats;
    
    DEV_VALIDATE_MODERN_DEVICE(device);
    MACH_ASSERT(buffer != NULL && size != NULL, "NULL buffer or size pointer");
    
    if (*size < sizeof(stats)) {
        *size = sizeof(stats);
        return KERN_INVALID_ARGUMENT;
    }
    
    /* Collect statistics */
    simple_lock(&device->safety_lock);
    stats.total_ops = device->total_operations;
    stats.failed_ops = device->failed_operations;
    stats.avg_response_time = device->avg_response_time;
    stats.error_count = device->modern_ops->health.error_count;
    stats.health_state = device->modern_ops->health.state;
    stats.usage = device->modern_ops->usage;
    simple_unlock(&device->safety_lock);
    
    /* Copy to user buffer */
    memcpy(buffer, &stats, sizeof(stats));
    *size = sizeof(stats);
    
    return KERN_SUCCESS;
}