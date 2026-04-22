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
 * User-space Device Driver SDK
 * 
 * This header provides the API for developing user-space device drivers
 * for GNU Mach. It includes registration functions, device operations,
 * and utility functions for driver development.
 */

#ifndef _DEVICE_USERSPACE_DRIVER_SDK_H_
#define _DEVICE_USERSPACE_DRIVER_SDK_H_

#include <mach/mach.h>
#include <device/userspace_driver_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Driver instance structure - maintains driver state
 */
struct usrd_driver_instance {
    /* Registration information */
    usrd_token_t registration_token;
    mach_port_t registry_port;
    mach_port_t driver_port;
    
    /* Driver information */
    struct usrd_driver_info info;
    struct usrd_resource_usage usage;
    unsigned int status_flags;
    
    /* Device management */
    unsigned int max_devices;
    unsigned int active_devices;
    void **device_contexts;  /* Driver-specific device contexts */
    
    /* Callbacks */
    struct usrd_driver_callbacks *callbacks;
    
    /* Runtime state */
    boolean_t is_running;
    boolean_t should_exit;
    pthread_t message_thread;
    mach_port_t reply_port;
};

typedef struct usrd_driver_instance *usrd_driver_t;

/*
 * Driver callback functions - implemented by the driver developer
 */
struct usrd_driver_callbacks {
    /* Device lifecycle */
    kern_return_t (*device_open)(usrd_driver_t driver, const char *device_name,
                                dev_mode_t mode, void **device_context);
    kern_return_t (*device_close)(usrd_driver_t driver, void *device_context);
    
    /* I/O operations */
    kern_return_t (*device_read)(usrd_driver_t driver, void *device_context,
                               dev_mode_t mode, recnum_t recnum,
                               vm_size_t bytes_wanted, void **data,
                               vm_size_t *bytes_read);
    kern_return_t (*device_write)(usrd_driver_t driver, void *device_context,
                                dev_mode_t mode, recnum_t recnum,
                                void *data, vm_size_t bytes_to_write,
                                vm_size_t *bytes_written);
    
    /* Device control */
    kern_return_t (*device_get_status)(usrd_driver_t driver, void *device_context,
                                     dev_flavor_t flavor, dev_status_t status,
                                     natural_t *status_count);
    kern_return_t (*device_set_status)(usrd_driver_t driver, void *device_context,
                                     dev_flavor_t flavor, dev_status_t status,
                                     natural_t status_count);
    
    /* Driver lifecycle callbacks */
    kern_return_t (*driver_init)(usrd_driver_t driver);
    void (*driver_shutdown)(usrd_driver_t driver);
    
    /* Health monitoring */
    kern_return_t (*driver_health_check)(usrd_driver_t driver);
    
    /* Power management (optional) */
    kern_return_t (*device_suspend)(usrd_driver_t driver, void *device_context);
    kern_return_t (*device_resume)(usrd_driver_t driver, void *device_context);
};

/*
 * Driver registration and lifecycle
 */

/*
 * Create a new driver instance
 */
extern usrd_driver_t usrd_driver_create(const char *name,
                                       const char *description,
                                       unsigned int version_major,
                                       unsigned int version_minor,
                                       unsigned int capabilities,
                                       struct usrd_driver_callbacks *callbacks);

/*
 * Destroy a driver instance
 */
extern void usrd_driver_destroy(usrd_driver_t driver);

/*
 * Register the driver with the kernel
 */
extern kern_return_t usrd_driver_register(usrd_driver_t driver,
                                        mach_port_t registry_port);

/*
 * Unregister the driver from the kernel
 */
extern kern_return_t usrd_driver_unregister(usrd_driver_t driver);

/*
 * Start the driver message loop
 */
extern kern_return_t usrd_driver_run(usrd_driver_t driver);

/*
 * Stop the driver message loop
 */
extern kern_return_t usrd_driver_stop(usrd_driver_t driver);

/*
 * Resource management
 */

/*
 * Set resource limits for the driver
 */
extern kern_return_t usrd_driver_set_resource_limits(usrd_driver_t driver,
                                                    unsigned int resource_type,
                                                    unsigned int limit);

/*
 * Update resource usage statistics
 */
extern kern_return_t usrd_driver_update_resource_usage(usrd_driver_t driver,
                                                      unsigned int resource_type,
                                                      unsigned int usage);

/*
 * Check if a resource operation would exceed limits
 */
extern boolean_t usrd_driver_check_resource_limit(usrd_driver_t driver,
                                                 unsigned int resource_type,
                                                 unsigned int amount);

/*
 * Device management utilities
 */

/*
 * Register a device with the driver
 */
extern kern_return_t usrd_driver_add_device(usrd_driver_t driver,
                                           const char *device_name,
                                           unsigned int device_type,
                                           void *device_context);

/*
 * Remove a device from the driver
 */
extern kern_return_t usrd_driver_remove_device(usrd_driver_t driver,
                                              const char *device_name);

/*
 * Get device context by name
 */
extern void *usrd_driver_get_device_context(usrd_driver_t driver,
                                           const char *device_name);

/*
 * Status and health monitoring
 */

/*
 * Update driver status
 */
extern kern_return_t usrd_driver_set_status(usrd_driver_t driver,
                                           unsigned int status_flags);

/*
 * Send heartbeat to kernel
 */
extern kern_return_t usrd_driver_heartbeat(usrd_driver_t driver);

/*
 * Report an error to the kernel
 */
extern kern_return_t usrd_driver_report_error(usrd_driver_t driver,
                                             unsigned int error_code,
                                             const char *error_description);

/*
 * Memory management utilities for drivers
 */

/*
 * Allocate memory for driver use (tracked for resource limits)
 */
extern void *usrd_malloc(usrd_driver_t driver, size_t size);

/*
 * Free memory allocated with usrd_malloc
 */
extern void usrd_free(usrd_driver_t driver, void *ptr, size_t size);

/*
 * Allocate DMA-coherent memory (if driver has DMA capability)
 */
extern void *usrd_dma_alloc(usrd_driver_t driver, size_t size,
                          vm_offset_t *physical_addr);

/*
 * Free DMA-coherent memory
 */
extern void usrd_dma_free(usrd_driver_t driver, void *ptr, size_t size,
                        vm_offset_t physical_addr);

/*
 * Debugging and logging utilities
 */

/*
 * Log levels
 */
#define USRD_LOG_ERROR      0
#define USRD_LOG_WARNING    1
#define USRD_LOG_INFO       2
#define USRD_LOG_DEBUG      3

/*
 * Log a message (driver name is automatically prefixed)
 */
extern void usrd_log(usrd_driver_t driver, int level, const char *fmt, ...);

/*
 * Dump driver statistics
 */
extern void usrd_dump_stats(usrd_driver_t driver);

/*
 * Interrupt handling (for drivers with interrupt capability)
 */

/*
 * Register for interrupt notifications
 */
extern kern_return_t usrd_driver_register_interrupt(usrd_driver_t driver,
                                                   int irq_line,
                                                   void (*handler)(usrd_driver_t, int));

/*
 * Unregister interrupt notifications
 */
extern kern_return_t usrd_driver_unregister_interrupt(usrd_driver_t driver,
                                                     int irq_line);

/*
 * Acknowledge interrupt (re-enable IRQ line)
 */
extern kern_return_t usrd_driver_ack_interrupt(usrd_driver_t driver,
                                              int irq_line);

/*
 * Utility macros for driver development
 */

/* Helper macros for common driver patterns */
#define USRD_DRIVER_INFO(name, desc, major, minor, caps) \
    { .name = (name), .description = (desc), .version_major = (major), \
      .version_minor = (minor), .capabilities = (caps) }

#define USRD_CALLBACKS(init_fn, shutdown_fn, open_fn, close_fn, read_fn, write_fn) \
    { .driver_init = (init_fn), .driver_shutdown = (shutdown_fn), \
      .device_open = (open_fn), .device_close = (close_fn), \
      .device_read = (read_fn), .device_write = (write_fn) }

/* Error handling */
#define USRD_CHECK_RETURN(call) \
    do { kern_return_t _ret = (call); if (_ret != KERN_SUCCESS) return _ret; } while(0)

#define USRD_LOG_ERROR_IF(driver, condition, fmt, ...) \
    do { if (condition) usrd_log(driver, USRD_LOG_ERROR, fmt, ##__VA_ARGS__); } while(0)

/* Resource management helpers */
#define USRD_SET_MEMORY_LIMIT(driver, kb) \
    usrd_driver_set_resource_limits(driver, USRD_RESOURCE_MEMORY, kb)

#define USRD_SET_IO_LIMIT(driver, ops) \
    usrd_driver_set_resource_limits(driver, USRD_RESOURCE_IO_OPS, ops)

#define USRD_UPDATE_MEMORY_USAGE(driver, kb) \
    usrd_driver_update_resource_usage(driver, USRD_RESOURCE_MEMORY, kb)

/*
 * Example driver skeleton - developers can copy and modify this
 */
#ifdef USRD_EXAMPLE_DRIVER

/* Example device context structure */
struct my_device_context {
    char device_name[64];
    boolean_t is_open;
    unsigned int open_count;
    /* Add device-specific fields here */
};

/* Example driver callbacks */
static kern_return_t my_driver_init(usrd_driver_t driver);
static void my_driver_shutdown(usrd_driver_t driver);
static kern_return_t my_device_open(usrd_driver_t driver, const char *device_name,
                                   dev_mode_t mode, void **device_context);
static kern_return_t my_device_close(usrd_driver_t driver, void *device_context);
static kern_return_t my_device_read(usrd_driver_t driver, void *device_context,
                                   dev_mode_t mode, recnum_t recnum,
                                   vm_size_t bytes_wanted, void **data,
                                   vm_size_t *bytes_read);
static kern_return_t my_device_write(usrd_driver_t driver, void *device_context,
                                    dev_mode_t mode, recnum_t recnum,
                                    void *data, vm_size_t bytes_to_write,
                                    vm_size_t *bytes_written);

/* Example driver main function */
int main(int argc, char **argv)
{
    usrd_driver_t driver;
    struct usrd_driver_callbacks callbacks = USRD_CALLBACKS(
        my_driver_init, my_driver_shutdown,
        my_device_open, my_device_close,
        my_device_read, my_device_write
    );
    
    /* Create driver instance */
    driver = usrd_driver_create("example-driver", "Example driver",
                               1, 0, USRD_CAP_CHAR_DEVICE, &callbacks);
    if (!driver) {
        fprintf(stderr, "Failed to create driver instance\n");
        return 1;
    }
    
    /* Set resource limits */
    USRD_SET_MEMORY_LIMIT(driver, 1024);  /* 1MB memory limit */
    USRD_SET_IO_LIMIT(driver, 10);        /* Max 10 concurrent I/O ops */
    
    /* Register with kernel */
    if (usrd_driver_register(driver, mach_task_self()) != KERN_SUCCESS) {
        fprintf(stderr, "Failed to register driver\n");
        usrd_driver_destroy(driver);
        return 1;
    }
    
    printf("Driver registered successfully\n");
    
    /* Run driver message loop */
    usrd_driver_run(driver);
    
    /* Cleanup */
    usrd_driver_unregister(driver);
    usrd_driver_destroy(driver);
    
    return 0;
}

#endif /* USRD_EXAMPLE_DRIVER */

#ifdef __cplusplus
}
#endif

#endif /* _DEVICE_USERSPACE_DRIVER_SDK_H_ */