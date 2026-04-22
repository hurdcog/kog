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
 * Device I/O Request Validation and Isolation
 * 
 * Enhanced validation for I/O requests to improve driver isolation
 * and prevent common driver bugs from affecting system stability.
 */

#include <device/io_req.h>
#include <device/modern.h>
#include <device/ds_routines.h>
#include <kern/printf.h>
#include <kern/debug.h>
#include <string.h>

#ifndef ULONG_MAX
#define ULONG_MAX (~0UL)
#endif

/*
 * I/O request validation flags
 */
#define IO_VALIDATE_BASIC   0x01    /* Basic parameter validation */
#define IO_VALIDATE_BOUNDS  0x02    /* Memory bounds checking */
#define IO_VALIDATE_DEVICE  0x04    /* Device state validation */
#define IO_VALIDATE_TIMEOUT 0x08    /* Timeout enforcement */
#define IO_VALIDATE_ALL     0xFF    /* All validation checks */

/*
 * Enhanced I/O request structure with validation
 */
struct io_req_validated {
    struct io_req   basic;          /* Standard I/O request - MUST be first */
    
    /* Validation fields */
    unsigned int    validation_flags; /* Validation level applied */
    unsigned int    submission_time;  /* When request was submitted */
    unsigned int    timeout_ms;       /* Request timeout in milliseconds */
    unsigned int    retry_count;      /* Number of retry attempts */
    unsigned int    max_retries;      /* Maximum retries allowed */
    
    /* Safety fields */
    unsigned int    safety_cookie;    /* Validation cookie */
    void           *original_data;    /* Original data pointer for bounds checking */
    vm_size_t       original_size;    /* Original data size */
    
    /* Performance monitoring */
    unsigned int    start_time_us;    /* Start time in microseconds */
    unsigned int    completion_time_us; /* Completion time in microseconds */
};

typedef struct io_req_validated *io_req_validated_t;

/*
 * Safety cookie for I/O request validation
 */
#define IO_REQ_VALIDATION_COOKIE    0x494F5245  /* 'IORE' */

/*
 * Maximum reasonable values for I/O operations
 */
#define IO_MAX_DATA_SIZE    (32 * 1024 * 1024)  /* 32MB max I/O */
#define IO_MAX_TIMEOUT_MS   (60 * 1000)         /* 60 second max timeout */
#define IO_MAX_RETRIES      5                   /* Maximum retry attempts */

/*
 * Validate basic I/O request parameters
 */
static boolean_t
io_req_validate_basic(io_req_t ior)
{
    /* Check for NULL pointer */
    if (ior == IO_REQ_NULL) {
        printf("io_req_validate_basic: NULL I/O request\n");
        return FALSE;
    }
    
    /* Check operation flags */
    if ((ior->io_op & ~(IO_READ | IO_WRITE | IO_OPEN | IO_DONE | IO_ERROR | 
                       IO_BUSY | IO_WANTED | IO_BAD | IO_CALL | IO_INBAND | IO_INTERNAL)) != 0) {
        printf("io_req_validate_basic: invalid operation flags 0x%x\n", ior->io_op);
        return FALSE;
    }
    
    /* Check data size reasonableness */
    if (ior->io_count > IO_MAX_DATA_SIZE) {
        printf("io_req_validate_basic: excessive data size %lu\n", ior->io_count);
        return FALSE;
    }
    
    /* Check for negative values that should be positive */
    if (ior->io_count < 0) {
        printf("io_req_validate_basic: negative I/O count %ld\n", ior->io_count);
        return FALSE;
    }
    
    return TRUE;
}

/*
 * Validate memory bounds for I/O request
 */
static boolean_t
io_req_validate_bounds(io_req_t ior)
{
    /* Skip bounds checking for zero-length operations */
    if (ior->io_count == 0) {
        return TRUE;
    }
    
    /* Check data pointer alignment for better performance */
    if (ior->io_data && ((vm_offset_t)ior->io_data & 0x3)) {
        /* Note: Misaligned I/O is not an error, just potentially slower */
    }
    
    /* Check for potential integer overflow in offset calculations */
    if (ior->io_recnum > 0 && ior->io_count > 0 && 
        ior->io_recnum > (ULONG_MAX - ior->io_count)) {
        printf("io_req_validate_bounds: overflow in offset calculation\n");
        return FALSE;
    }
    
    /* For inband operations, validate the data is reasonable */
    if (ior->io_op & IO_INBAND) {
        if (ior->io_count > sizeof(io_buf_ptr_inband_t)) {
            printf("io_req_validate_bounds: inband data too large %ld\n", ior->io_count);
            return FALSE;
        }
    }
    
    return TRUE;
}

/*
 * Validate device state for I/O request
 */
static boolean_t
io_req_validate_device(io_req_t ior, mach_device_t device)
{
    if (device == MACH_DEVICE_NULL) {
        printf("io_req_validate_device: NULL device\n");
        return FALSE;
    }
    
    /* Check device state */
    simple_lock(&device->lock);
    boolean_t valid = (device->state == DEV_STATE_OPEN);
    simple_unlock(&device->lock);
    
    if (!valid) {
        printf("io_req_validate_device: device not in open state (state=%d)\n", device->state);
        return FALSE;
    }
    
    /* For modern devices, perform additional validation */
    if (dev_is_modern(device)) {
        mach_device_modern_t modern_device = (mach_device_modern_t)device;
        
        if (!dev_validate_modern_device(modern_device)) {
            printf("io_req_validate_device: modern device validation failed\n");
            return FALSE;
        }
        
        /* Check resource limits */
        if (!dev_check_resource_limits(modern_device, DEV_RESOURCE_IO_OPS, 1)) {
            printf("io_req_validate_device: I/O operation would exceed limits\n");
            return FALSE;
        }
        
        /* Check device health */
        if (modern_device->modern_ops->health.state == DEV_STATE_ERROR) {
            printf("io_req_validate_device: device in error state\n");
            return FALSE;
        }
    }
    
    return TRUE;
}

/*
 * Create validated I/O request
 */
io_req_validated_t
io_req_create_validated(io_req_t original_ior, unsigned int validation_flags, 
                       unsigned int timeout_ms)
{
    io_req_validated_t validated_ior;
    
    /* Allocate validated I/O request structure */
    validated_ior = (io_req_validated_t)kalloc(sizeof(struct io_req_validated));
    if (validated_ior == NULL) {
        printf("io_req_create_validated: allocation failed\n");
        return NULL;
    }
    
    /* Copy original request */
    validated_ior->basic = *original_ior;
    
    /* Set up validation fields */
    validated_ior->validation_flags = validation_flags;
    validated_ior->submission_time = (unsigned int)time.seconds;
    validated_ior->timeout_ms = (timeout_ms > 0) ? timeout_ms : IO_MAX_TIMEOUT_MS;
    validated_ior->retry_count = 0;
    validated_ior->max_retries = IO_MAX_RETRIES;
    
    /* Set up safety fields */
    validated_ior->safety_cookie = IO_REQ_VALIDATION_COOKIE;
    validated_ior->original_data = original_ior->io_data;
    validated_ior->original_size = original_ior->io_count;
    
    /* Initialize performance monitoring */
    validated_ior->start_time_us = 0;  /* Set when I/O actually starts */
    validated_ior->completion_time_us = 0;
    
    return validated_ior;
}

/*
 * Validate I/O request comprehensively
 */
kern_return_t
io_req_validate(io_req_t ior, mach_device_t device, unsigned int validation_flags)
{
    if (validation_flags & IO_VALIDATE_BASIC) {
        if (!io_req_validate_basic(ior)) {
            return KERN_INVALID_ARGUMENT;
        }
    }
    
    if (validation_flags & IO_VALIDATE_BOUNDS) {
        if (!io_req_validate_bounds(ior)) {
            return KERN_INVALID_ADDRESS;
        }
    }
    
    if (validation_flags & IO_VALIDATE_DEVICE) {
        if (!io_req_validate_device(ior, device)) {
            return KERN_INVALID_CAPABILITY;
        }
    }
    
    return KERN_SUCCESS;
}

/*
 * Enhanced device I/O with validation
 */
kern_return_t
device_io_validated(mach_device_t device, io_req_t ior, unsigned int validation_flags)
{
    kern_return_t result;
    mach_device_modern_t modern_device = NULL;
    unsigned int start_time;
    
    /* Perform comprehensive validation */
    result = io_req_validate(ior, device, validation_flags);
    if (result != KERN_SUCCESS) {
        printf("device_io_validated: validation failed with result %d\n", result);
        return result;
    }
    
    /* Check if this is a modern device */
    if (dev_is_modern(device)) {
        modern_device = (mach_device_modern_t)device;
        
        /* Update resource usage */
        dev_update_resource_usage(modern_device, DEV_RESOURCE_IO_OPS, 1);
        
        /* Update performance counters */
        simple_lock(&modern_device->safety_lock);
        modern_device->total_operations++;
        simple_unlock(&modern_device->safety_lock);
        
        /* Record start time */
        start_time = (unsigned int)(time.seconds * 1000000 + time.microseconds);
    }
    
    /* Perform the actual I/O operation */
    if (ior->io_op & IO_READ) {
        result = (device->dev_ops->d_read)(device->dev_number, ior);
    } else if (ior->io_op & IO_WRITE) {
        result = (device->dev_ops->d_write)(device->dev_number, ior);
    } else {
        printf("device_io_validated: unsupported operation 0x%x\n", ior->io_op);
        result = KERN_INVALID_ARGUMENT;
    }
    
    /* Update statistics for modern devices */
    if (modern_device) {
        simple_lock(&modern_device->safety_lock);
        
        if (result != KERN_SUCCESS) {
            modern_device->failed_operations++;
        }
        
        /* Update resource usage (decrement active I/O count) */
        dev_update_resource_usage(modern_device, DEV_RESOURCE_IO_OPS, -1);
        
        /* Update average response time */
        if (result == KERN_SUCCESS) {
            unsigned int end_time = (unsigned int)(time.seconds * 1000000 + time.microseconds);
            unsigned int response_time = end_time - start_time;
            
            /* Simple exponential moving average */
            if (modern_device->avg_response_time == 0) {
                modern_device->avg_response_time = response_time;
            } else {
                modern_device->avg_response_time = 
                    (modern_device->avg_response_time * 7 + response_time) / 8;
            }
        }
        
        simple_unlock(&modern_device->safety_lock);
    }
    
    return result;
}

/*
 * Check for timed-out I/O requests
 */
void
io_req_timeout_check(void)
{
    /* This would be called periodically by a kernel thread to check for
     * timed-out I/O operations and take appropriate recovery actions.
     * Implementation would iterate through active I/O requests and
     * check their timeout values.
     */
}

/*
 * Cleanup validated I/O request
 */
void
io_req_validated_cleanup(io_req_validated_t validated_ior)
{
    if (validated_ior == NULL) {
        return;
    }
    
    /* Validate the structure before cleanup */
    if (validated_ior->safety_cookie != IO_REQ_VALIDATION_COOKIE) {
        printf("io_req_validated_cleanup: invalid safety cookie 0x%x\n", 
               validated_ior->safety_cookie);
        return;
    }
    
    /* Clear sensitive data */
    validated_ior->safety_cookie = 0;
    memset(validated_ior, 0, sizeof(struct io_req_validated));
    
    /* Free the structure */
    kfree((vm_offset_t)validated_ior, sizeof(struct io_req_validated));
}