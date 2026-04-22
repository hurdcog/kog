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
 * Modern Device Driver API Extensions
 * 
 * This header provides enhanced driver capabilities while maintaining
 * backward compatibility with the existing device driver framework.
 */

#ifndef _DEVICE_MODERN_H_
#define _DEVICE_MODERN_H_

#include <device/conf.h>
#include <device/dev_hdr.h>
#include <kern/lock.h>

/*
 * Driver capability flags - indicates what modern features a driver supports
 */
#define DEV_CAP_NONE            0x00000000  /* No extended capabilities */
#define DEV_CAP_POWER_MGMT      0x00000001  /* Power management support */
#define DEV_CAP_HOTPLUG         0x00000002  /* Hot plug/unplug support */
#define DEV_CAP_MULTI_QUEUE     0x00000004  /* Multiple I/O queues */
#define DEV_CAP_ERROR_RECOVERY  0x00000008  /* Enhanced error recovery */
#define DEV_CAP_RESOURCE_LIMIT  0x00000010  /* Resource limit awareness */
#define DEV_CAP_DIAGNOSTICS     0x00000020  /* Built-in diagnostics */
#define DEV_CAP_ASYNC_NOTIFY    0x00000040  /* Advanced async notifications */

/*
 * Driver version information
 */
struct dev_version {
    unsigned int    major;          /* Major version number */
    unsigned int    minor;          /* Minor version number */
    unsigned int    patch;          /* Patch level */
    unsigned int    build;          /* Build number */
};

/*
 * Extended error information
 */
struct dev_error_info {
    io_return_t     basic_error;    /* Standard error code */
    unsigned int    extended_code;  /* Driver-specific error code */
    unsigned int    context_flags;  /* Error context information */
    char            description[64]; /* Human-readable error description */
    void           *debug_data;     /* Optional debug information */
    unsigned int    debug_size;     /* Size of debug data */
};

/*
 * Driver resource limits and usage tracking
 */
struct dev_resource_limits {
    unsigned int    max_memory;     /* Maximum memory usage (KB) */
    unsigned int    max_interrupts; /* Maximum interrupt rate (per second) */
    unsigned int    max_io_ops;     /* Maximum concurrent I/O operations */
    unsigned int    timeout_ms;     /* Operation timeout (milliseconds) */
};

struct dev_resource_usage {
    unsigned int    current_memory; /* Current memory usage (KB) */
    unsigned int    current_interrupts; /* Current interrupt rate */
    unsigned int    current_io_ops; /* Current I/O operations */
    unsigned int    total_errors;   /* Total error count */
    unsigned int    last_error_time; /* Timestamp of last error */
};

/*
 * Driver state and health monitoring
 */
typedef enum {
    DEV_STATE_UNKNOWN = 0,
    DEV_STATE_INITIALIZING,
    DEV_STATE_READY,
    DEV_STATE_BUSY,
    DEV_STATE_ERROR,
    DEV_STATE_RECOVERY,
    DEV_STATE_SUSPENDED,
    DEV_STATE_REMOVING
} dev_health_state_t;

struct dev_health_info {
    dev_health_state_t  state;      /* Current driver state */
    unsigned int        uptime;     /* Driver uptime in seconds */
    unsigned int        error_count; /* Number of errors since start */
    unsigned int        recovery_count; /* Number of recovery attempts */
    unsigned int        last_health_check; /* Timestamp of last health check */
};

/*
 * Modern device driver metadata - extends the basic dev_ops
 */
struct dev_modern_ops {
    struct dev_ops      basic;      /* Standard operations - MUST be first */
    
    /* Version and capability information */
    struct dev_version  version;    /* Driver version */
    unsigned int        capabilities; /* Capability flags */
    const char         *vendor;     /* Vendor name */
    const char         *description; /* Driver description */
    
    /* Resource management */
    struct dev_resource_limits limits; /* Resource limits */
    struct dev_resource_usage  usage;  /* Current usage */
    decl_simple_lock_data(, resource_lock) /* Lock for resource tracking */
    
    /* Health and diagnostics */
    struct dev_health_info health;  /* Health information */
    int (*d_health_check)(dev_t);   /* Health check callback */
    int (*d_diagnostics)(dev_t, void *buffer, size_t *size); /* Diagnostics */
    
    /* Enhanced error handling */
    int (*d_get_error_info)(dev_t, struct dev_error_info *); /* Get detailed error info */
    int (*d_recovery)(dev_t, unsigned int recovery_type); /* Error recovery */
    
    /* Power management */
    int (*d_suspend)(dev_t);        /* Suspend device */
    int (*d_resume)(dev_t);         /* Resume device */
    int (*d_power_state)(dev_t, unsigned int state); /* Set power state */
    
    /* Advanced I/O */
    int (*d_io_queue_setup)(dev_t, unsigned int num_queues); /* Setup multiple queues */
    int (*d_io_queue_submit)(dev_t, unsigned int queue_id, io_req_t); /* Queue-specific I/O */
};

typedef struct dev_modern_ops *dev_modern_ops_t;

/*
 * Enhanced device structure with modern capabilities
 */
struct mach_device_modern {
    struct mach_device  basic;      /* Standard device - MUST be first */
    
    /* Modern extensions */
    dev_modern_ops_t    modern_ops; /* Modern operations */
    boolean_t           is_modern;  /* TRUE if this is a modern driver */
    
    /* Isolation and safety */
    decl_simple_lock_data(, safety_lock) /* Lock for safety checks */
    unsigned int        validation_cookie; /* For detecting corruption */
    unsigned int        last_validated;   /* Last validation timestamp */
    
    /* Performance monitoring */
    unsigned int        total_operations; /* Total I/O operations */
    unsigned int        failed_operations; /* Failed operations */
    unsigned int        avg_response_time; /* Average response time (us) */
};

typedef struct mach_device_modern *mach_device_modern_t;
#define MACH_DEVICE_MODERN_NULL ((mach_device_modern_t)0)

/*
 * Validation cookie for detecting device structure corruption
 */
#define DEV_VALIDATION_COOKIE   0x44455643  /* 'DEVC' */

/*
 * Safety macros for driver validation - simplified version
 */
#define DEV_VALIDATE_DEVICE(dev) \
    do { if ((dev) == MACH_DEVICE_NULL) panic("NULL device pointer"); } while(0)

#define DEV_VALIDATE_MODERN_DEVICE(dev) \
    do { \
        DEV_VALIDATE_DEVICE((mach_device_t)(dev)); \
        if (!(dev)->is_modern) panic("Not a modern device"); \
        if ((dev)->validation_cookie != DEV_VALIDATION_COOKIE) panic("Device corruption detected"); \
    } while(0)

#define DEV_CHECK_RESOURCE_LIMITS(dev, resource_type, amount) \
    dev_check_resource_limits((dev), (resource_type), (amount))

/*
 * Resource type constants for limit checking
 */
#define DEV_RESOURCE_MEMORY     0
#define DEV_RESOURCE_INTERRUPTS 1
#define DEV_RESOURCE_IO_OPS     2

/*
 * Function prototypes for modern driver support
 */

/* Device creation and management */
extern mach_device_modern_t mach_device_modern_create(dev_modern_ops_t ops);
extern void mach_device_modern_destroy(mach_device_modern_t device);
extern kern_return_t mach_device_modern_register(mach_device_modern_t device, const char *name);

/* Resource management */
extern boolean_t dev_check_resource_limits(mach_device_modern_t device, 
                                          unsigned int resource_type, 
                                          unsigned int amount);
extern void dev_update_resource_usage(mach_device_modern_t device, 
                                     unsigned int resource_type, 
                                     int delta);

/* Health monitoring */
extern kern_return_t dev_health_check(mach_device_modern_t device);
extern void dev_update_health_state(mach_device_modern_t device, dev_health_state_t new_state);

/* Safety and validation */
extern boolean_t dev_validate_modern_device(mach_device_modern_t device);
extern void dev_mark_device_corrupted(mach_device_modern_t device);

/* Enhanced error handling */
extern void dev_log_error(mach_device_modern_t device, struct dev_error_info *error);
extern kern_return_t dev_attempt_recovery(mach_device_modern_t device, unsigned int recovery_type);

/* Debugging and diagnostics */
extern void dev_dump_diagnostics(mach_device_modern_t device);
extern kern_return_t dev_get_performance_stats(mach_device_modern_t device, void *buffer, size_t *size);

/* Compatibility helpers */
static inline boolean_t dev_is_modern(mach_device_t device) {
    mach_device_modern_t modern = (mach_device_modern_t)device;
    return (modern && modern->is_modern);
}

static inline dev_modern_ops_t dev_get_modern_ops(mach_device_t device) {
    if (dev_is_modern(device)) {
        return ((mach_device_modern_t)device)->modern_ops;
    }
    return NULL;
}

#endif /* _DEVICE_MODERN_H_ */