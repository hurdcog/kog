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
 * User-space Device Driver Framework - Kernel Implementation
 * 
 * This header defines the kernel-side implementation of the user-space
 * device driver framework, including driver registration, proxy management,
 * and security isolation.
 */

#ifndef _DEVICE_USERSPACE_DRIVER_H_
#define _DEVICE_USERSPACE_DRIVER_H_

#include <device/userspace_driver_types.h>
#include <device/dev_hdr.h>
#include <kern/lock.h>
#include <kern/queue.h>
#include <kern/kalloc.h>
#include <mach/kern_return.h>
#include <stddef.h>

/*
 * Driver registry structure - manages all registered user-space drivers
 */
struct usrd_registry_impl {
    decl_simple_lock_data(, lock)       /* Registry lock */
    queue_head_t driver_list;           /* List of registered drivers */
    usrd_proxy_t driver_table[USRD_MAX_DRIVERS]; /* Driver lookup table */
    unsigned int next_token;            /* Next registration token */
    unsigned int active_count;          /* Number of active drivers */
    mach_port_t registry_port;          /* Registry service port */
    boolean_t initialized;              /* Registry initialization flag */
};

/*
 * Driver proxy implementation - kernel representation of user-space driver
 */
struct usrd_proxy_impl {
    struct usrd_proxy public;           /* Public proxy structure */
    queue_chain_t chain;                /* Chain for registry list */
    decl_simple_lock_data(, lock)       /* Proxy lock */
    
    /* Security and isolation */
    task_t driver_task;                 /* Driver task */
    vm_map_t driver_map;                /* Driver address space */
    boolean_t is_sandboxed;             /* Sandbox enforcement flag */
    
    /* Resource tracking */
    unsigned int memory_allocated;      /* Allocated memory (bytes) */
    unsigned int memory_peak;           /* Peak memory usage */
    unsigned int io_operations_active;  /* Active I/O operations */
    unsigned int interrupt_count;       /* Interrupt count */
    
    /* Device management */
    queue_head_t device_list;           /* Devices handled by this driver */
    unsigned int device_count;          /* Number of devices */
    
    /* Health monitoring */
    unsigned int last_heartbeat;        /* Last heartbeat timestamp */
    unsigned int crash_count;           /* Number of crashes */
    unsigned int restart_count;         /* Number of restarts */
    boolean_t is_healthy;               /* Health status */
    
    /* Communication */
    mach_port_t driver_port;            /* Driver communication port */
    mach_port_t reply_port;             /* Reply port for async operations */
};

/*
 * Device proxy implementation - represents device handled by user-space driver
 */
struct usrd_device_proxy_impl {
    struct usrd_device_proxy public;    /* Public device proxy structure */
    queue_chain_t chain;                /* Chain for driver's device list */
    decl_simple_lock_data(, lock)       /* Device proxy lock */
    
    /* Device state */
    struct mach_device device_header;   /* Standard device header */
    dev_ops_t original_ops;             /* Original device operations */
    
    /* Usage tracking */
    unsigned int total_reads;           /* Total read operations */
    unsigned int total_writes;          /* Total write operations */
    unsigned int total_ioctls;          /* Total ioctl operations */
    unsigned int error_count;           /* Error count */
    
    /* Timeout and safety */
    unsigned int operation_timeout;     /* Operation timeout (ms) */
    unsigned int last_operation;        /* Last operation timestamp */
    boolean_t is_responsive;            /* Driver responsiveness flag */
};

/*
 * Global registry instance
 */
extern struct usrd_registry_impl usrd_global_registry;

/*
 * Driver registry operations
 */
extern kern_return_t usrd_registry_init(void);
extern void usrd_registry_shutdown(void);
extern kern_return_t usrd_driver_register(struct usrd_driver_info *info,
                                         mach_port_t driver_port,
                                         usrd_token_t *token_out);
extern kern_return_t usrd_driver_unregister(usrd_token_t token);
extern usrd_proxy_t usrd_driver_lookup(usrd_token_t token);
extern usrd_proxy_t usrd_driver_lookup_by_name(const char *name);

/*
 * Driver proxy operations
 */
extern usrd_proxy_t usrd_proxy_create(struct usrd_driver_info *info,
                                     mach_port_t driver_port,
                                     task_t driver_task);
extern void usrd_proxy_destroy(usrd_proxy_t proxy);
extern kern_return_t usrd_proxy_update_status(usrd_proxy_t proxy,
                                             unsigned int status_flags,
                                             struct usrd_resource_usage *usage);
extern boolean_t usrd_proxy_check_health(usrd_proxy_t proxy);
extern kern_return_t usrd_proxy_enforce_limits(usrd_proxy_t proxy);

/*
 * Device proxy operations
 */
extern usrd_device_proxy_t usrd_device_proxy_create(usrd_proxy_t driver_proxy,
                                                   const char *device_name,
                                                   mach_port_t device_port);
extern void usrd_device_proxy_destroy(usrd_device_proxy_t device_proxy);
extern kern_return_t usrd_device_proxy_forward_operation(usrd_device_proxy_t device_proxy,
                                                        unsigned int operation,
                                                        void *data);

/*
 * Security and isolation
 */
extern kern_return_t usrd_create_sandbox(usrd_proxy_t proxy);
extern kern_return_t usrd_enforce_sandbox(usrd_proxy_t proxy);
extern boolean_t usrd_validate_driver_port(mach_port_t port, task_t task);
extern boolean_t usrd_check_resource_limits(usrd_proxy_t proxy,
                                           unsigned int resource_type,
                                           unsigned int amount);

/*
 * Device operations forwarding - these replace standard device operations
 * for devices handled by user-space drivers
 */
extern io_return_t usrd_device_open(device_t dev, dev_mode_t mode, io_req_t ior);
extern void usrd_device_close(device_t dev);
extern io_return_t usrd_device_read(device_t dev, io_req_t ior);
extern io_return_t usrd_device_write(device_t dev, io_req_t ior);
extern io_return_t usrd_device_get_status(device_t dev, dev_flavor_t flavor,
                                         dev_status_t status, natural_t *count);
extern io_return_t usrd_device_set_status(device_t dev, dev_flavor_t flavor,
                                         dev_status_t status, natural_t count);

/*
 * Debugging and diagnostics
 */
extern void usrd_dump_registry_info(void);
extern void usrd_dump_driver_info(usrd_proxy_t proxy);
extern void usrd_dump_device_info(usrd_device_proxy_t device_proxy);
extern kern_return_t usrd_get_statistics(void *buffer, size_t *size);

/*
 * Utility macros
 */
#define USRD_PROXY_LOCK(proxy) \
    simple_lock(&(proxy)->lock)

#define USRD_PROXY_UNLOCK(proxy) \
    simple_unlock(&(proxy)->lock)

#define USRD_DEVICE_PROXY_LOCK(device) \
    simple_lock(&(device)->lock)

#define USRD_DEVICE_PROXY_UNLOCK(device) \
    simple_unlock(&(device)->lock)

#define USRD_REGISTRY_LOCK() \
    simple_lock(&usrd_global_registry.lock)

#define USRD_REGISTRY_UNLOCK() \
    simple_unlock(&usrd_global_registry.lock)

/*
 * Validation macros
 */
#define USRD_VALIDATE_TOKEN(token) \
    ((token) != USRD_TOKEN_INVALID && (token) < USRD_MAX_DRIVERS)

#define USRD_VALIDATE_PROXY(proxy) \
    ((proxy) != NULL && (proxy)->public.token != USRD_TOKEN_INVALID)

#define USRD_VALIDATE_DEVICE_PROXY(device) \
    ((device) != NULL && (device)->public.driver_proxy != NULL)

/*
 * Error handling
 */
#define USRD_LOG_ERROR(proxy, fmt, ...) \
    printf("USRD[%s]: " fmt "\n", (proxy)->public.info.name, ##__VA_ARGS__)

#define USRD_LOG_WARNING(proxy, fmt, ...) \
    printf("USRD[%s]: WARNING: " fmt "\n", (proxy)->public.info.name, ##__VA_ARGS__)

#define USRD_LOG_INFO(proxy, fmt, ...) \
    printf("USRD[%s]: " fmt "\n", (proxy)->public.info.name, ##__VA_ARGS__)

#endif /* _DEVICE_USERSPACE_DRIVER_H_ */