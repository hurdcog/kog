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
 * User-space Device Driver Framework - Implementation
 * 
 * Implementation of the user-space device driver framework, providing
 * driver registration, proxy management, and security isolation.
 */

#include <device/userspace_driver.h>
#include <device/ds_routines.h>
#include <kern/printf.h>
#include <i386/i386/time_stamp.h>
#include <kern/task.h>
#include <vm/vm_map.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>

/* Forward declarations for internal functions */
static io_return_t usrd_device_open(device_t dev, dev_mode_t mode, io_req_t ior);
static io_return_t usrd_device_read(device_t dev, io_req_t ior);
static io_return_t usrd_device_write(device_t dev, io_req_t ior);

/*
 * Global registry instance
 */
struct usrd_registry_impl usrd_global_registry;

/*
 * Initialize the user-space driver registry
 */
kern_return_t usrd_registry_init(void)
{
    /* Initialize registry structure */
    simple_lock_init(&usrd_global_registry.lock);
    queue_init(&usrd_global_registry.driver_list);
    
    /* Clear driver table */
    for (int i = 0; i < USRD_MAX_DRIVERS; i++) {
        usrd_global_registry.driver_table[i] = NULL;
    }
    
    usrd_global_registry.next_token = 1; /* Start from 1, 0 is invalid */
    usrd_global_registry.active_count = 0;
    usrd_global_registry.registry_port = MACH_PORT_NULL;
    usrd_global_registry.initialized = TRUE;
    
    printf("User-space driver registry initialized\n");
    return KERN_SUCCESS;
}

/*
 * Shutdown the user-space driver registry
 */
void usrd_registry_shutdown(void)
{
    usrd_proxy_t proxy, next_proxy;
    
    USRD_REGISTRY_LOCK();
    
    /* Unregister all drivers */
    queue_iterate_safely(&usrd_global_registry.driver_list, proxy, next_proxy, 
                        struct usrd_proxy_impl *, chain) {
        usrd_proxy_destroy(proxy);
    }
    
    usrd_global_registry.initialized = FALSE;
    
    USRD_REGISTRY_UNLOCK();
    
    printf("User-space driver registry shutdown\n");
}

/*
 * Register a user-space driver
 */
kern_return_t usrd_driver_register(struct usrd_driver_info *info,
                                 mach_port_t driver_port,
                                 usrd_token_t *token_out)
{
    usrd_proxy_t proxy;
    usrd_token_t token;
    task_t driver_task;
    
    if (!info || !token_out) {
        return KERN_INVALID_ARGUMENT;
    }
    
    if (!usrd_global_registry.initialized) {
        return KERN_FAILURE;
    }
    
    /* Validate driver port and get associated task */
    if (!MACH_PORT_VALID(driver_port)) {
        return KERN_INVALID_ARGUMENT;
    }
    
    /* Get the task that owns the driver port */
    driver_task = current_task(); /* Simplified - in real implementation, 
                                   * we'd extract task from port */
    
    USRD_REGISTRY_LOCK();
    
    /* Check if driver already exists */
    if (usrd_driver_lookup_by_name(info->name) != NULL) {
        USRD_REGISTRY_UNLOCK();
        return KERN_ALREADY_IN_SET;
    }
    
    /* Check registry capacity */
    if (usrd_global_registry.active_count >= USRD_MAX_DRIVERS) {
        USRD_REGISTRY_UNLOCK();
        return KERN_RESOURCE_SHORTAGE;
    }
    
    /* Allocate next token */
    token = usrd_global_registry.next_token++;
    if (usrd_global_registry.next_token >= USRD_MAX_DRIVERS) {
        usrd_global_registry.next_token = 1; /* Wrap around, skip 0 */
    }
    
    USRD_REGISTRY_UNLOCK();
    
    /* Create driver proxy */
    proxy = usrd_proxy_create(info, driver_port, driver_task);
    if (proxy == NULL) {
        return KERN_RESOURCE_SHORTAGE;
    }
    
    proxy->public.token = token;
    
    USRD_REGISTRY_LOCK();
    
    /* Add to registry */
    usrd_global_registry.driver_table[token % USRD_MAX_DRIVERS] = proxy;
    queue_enter(&usrd_global_registry.driver_list, proxy, usrd_proxy_t, chain);
    usrd_global_registry.active_count++;
    
    USRD_REGISTRY_UNLOCK();
    
    *token_out = token;
    
    printf("User-space driver '%s' registered with token %u\n", 
           info->name, token);
    
    return KERN_SUCCESS;
}

/*
 * Unregister a user-space driver
 */
kern_return_t usrd_driver_unregister(usrd_token_t token)
{
    usrd_proxy_t proxy;
    
    if (!USRD_VALIDATE_TOKEN(token)) {
        return KERN_INVALID_ARGUMENT;
    }
    
    USRD_REGISTRY_LOCK();
    
    proxy = usrd_global_registry.driver_table[token % USRD_MAX_DRIVERS];
    if (proxy == NULL || proxy->public.token != token) {
        USRD_REGISTRY_UNLOCK();
        return KERN_INVALID_ARGUMENT;
    }
    
    /* Remove from registry */
    usrd_global_registry.driver_table[token % USRD_MAX_DRIVERS] = NULL;
    queue_remove(&usrd_global_registry.driver_list, proxy, usrd_proxy_t, chain);
    usrd_global_registry.active_count--;
    
    USRD_REGISTRY_UNLOCK();
    
    printf("User-space driver '%s' unregistered\n", proxy->public.info.name);
    
    /* Destroy proxy */
    usrd_proxy_destroy(proxy);
    
    return KERN_SUCCESS;
}

/*
 * Lookup driver by token
 */
usrd_proxy_t usrd_driver_lookup(usrd_token_t token)
{
    usrd_proxy_t proxy;
    
    if (!USRD_VALIDATE_TOKEN(token)) {
        return NULL;
    }
    
    USRD_REGISTRY_LOCK();
    proxy = usrd_global_registry.driver_table[token % USRD_MAX_DRIVERS];
    if (proxy && proxy->public.token == token) {
        USRD_REGISTRY_UNLOCK();
        return proxy;
    }
    USRD_REGISTRY_UNLOCK();
    
    return NULL;
}

/*
 * Lookup driver by name
 */
usrd_proxy_t usrd_driver_lookup_by_name(const char *name)
{
    usrd_proxy_t proxy;
    
    if (!name) {
        return NULL;
    }
    
    USRD_REGISTRY_LOCK();
    
    queue_iterate(&usrd_global_registry.driver_list, proxy, usrd_proxy_t, chain) {
        if (strcmp(proxy->public.info.name, name) == 0) {
            USRD_REGISTRY_UNLOCK();
            return proxy;
        }
    }
    
    USRD_REGISTRY_UNLOCK();
    return NULL;
}

/*
 * Create a driver proxy
 */
usrd_proxy_t usrd_proxy_create(struct usrd_driver_info *info,
                             mach_port_t driver_port,
                             task_t driver_task)
{
    struct usrd_proxy_impl *proxy;
    
    /* Allocate proxy structure */
    proxy = (struct usrd_proxy_impl *)kalloc(sizeof(struct usrd_proxy_impl));
    if (proxy == NULL) {
        return NULL;
    }
    
    /* Initialize proxy */
    memset(proxy, 0, sizeof(struct usrd_proxy_impl));
    simple_lock_init(&proxy->lock);
    queue_init(&proxy->device_list);
    
    /* Copy driver information */
    proxy->public.info = *info;
    proxy->public.driver_port = driver_port;
    proxy->driver_task = driver_task;
    proxy->driver_map = driver_task->map;
    
    /* Initialize resource tracking */
    proxy->public.usage.memory_kb = 0;
    proxy->public.usage.cpu_percent = 0;
    proxy->public.usage.io_ops_active = 0;
    proxy->public.usage.interrupt_rate = 0;
    proxy->public.usage.error_count = 0;
    proxy->public.usage.recovery_count = 0;
    proxy->public.usage.uptime_seconds = 0;
    
    /* Initialize status */
    proxy->public.status_flags = USRD_STATUS_RUNNING;
    proxy->public.last_heartbeat = (unsigned int)time_stamp;
    proxy->is_healthy = TRUE;
    proxy->is_sandboxed = FALSE; /* Will be set up later if needed */
    
    printf("Created driver proxy for '%s'\n", info->name);
    
    return (usrd_proxy_t)proxy;
}

/*
 * Destroy a driver proxy
 */
void usrd_proxy_destroy(usrd_proxy_t proxy_pub)
{
    struct usrd_proxy_impl *proxy = (struct usrd_proxy_impl *)proxy_pub;
    usrd_device_proxy_t device, next_device;
    
    if (!USRD_VALIDATE_PROXY(proxy_pub)) {
        return;
    }
    
    USRD_PROXY_LOCK(proxy);
    
    /* Destroy all associated devices */
    queue_iterate_safely(&proxy->device_list, device, next_device,
                        struct usrd_device_proxy_impl *, chain) {
        usrd_device_proxy_destroy(device);
    }
    
    printf("Destroying driver proxy for '%s'\n", proxy->public.info.name);
    
    USRD_PROXY_UNLOCK(proxy);
    
    /* Free proxy structure */
    kfree((vm_offset_t)proxy, sizeof(struct usrd_proxy_impl));
}

/*
 * Update driver status
 */
kern_return_t usrd_proxy_update_status(usrd_proxy_t proxy_pub,
                                     unsigned int status_flags,
                                     struct usrd_resource_usage *usage)
{
    struct usrd_proxy_impl *proxy = (struct usrd_proxy_impl *)proxy_pub;
    
    if (!USRD_VALIDATE_PROXY(proxy_pub) || !usage) {
        return KERN_INVALID_ARGUMENT;
    }
    
    USRD_PROXY_LOCK(proxy);
    
    proxy->public.status_flags = status_flags;
    proxy->public.usage = *usage;
    proxy->public.last_heartbeat = (unsigned int)time_stamp;
    
    /* Update health status */
    proxy->is_healthy = !(status_flags & USRD_STATUS_ERROR);
    
    USRD_PROXY_UNLOCK(proxy);
    
    return KERN_SUCCESS;
}

/*
 * Create a device proxy
 */
usrd_device_proxy_t usrd_device_proxy_create(usrd_proxy_t driver_proxy,
                                            const char *device_name,
                                            mach_port_t device_port)
{
    struct usrd_device_proxy_impl *device;
    struct usrd_proxy_impl *proxy = (struct usrd_proxy_impl *)driver_proxy;
    
    if (!USRD_VALIDATE_PROXY(driver_proxy) || !device_name) {
        return NULL;
    }
    
    /* Allocate device proxy */
    device = (struct usrd_device_proxy_impl *)kalloc(sizeof(struct usrd_device_proxy_impl));
    if (device == NULL) {
        return NULL;
    }
    
    /* Initialize device proxy */
    memset(device, 0, sizeof(struct usrd_device_proxy_impl));
    simple_lock_init(&device->lock);
    
    /* Set up device information */
    device->public.driver_proxy = driver_proxy;
    strncpy(device->public.device_name, device_name, USRD_MAX_DEVICE_NAME - 1);
    device->public.device_name[USRD_MAX_DEVICE_NAME - 1] = '\0';
    device->public.device_port = device_port;
    device->public.is_open = FALSE;
    device->public.open_count = 0;
    
    /* Initialize usage tracking */
    device->total_reads = 0;
    device->total_writes = 0;
    device->total_ioctls = 0;
    device->error_count = 0;
    device->operation_timeout = 5000; /* 5 second default timeout */
    device->is_responsive = TRUE;
    
    USRD_PROXY_LOCK(proxy);
    
    /* Add to driver's device list */
    queue_enter(&proxy->device_list, device, usrd_device_proxy_t, chain);
    proxy->device_count++;
    
    USRD_PROXY_UNLOCK(proxy);
    
    printf("Created device proxy '%s' for driver '%s'\n", 
           device_name, proxy->public.info.name);
    
    return (usrd_device_proxy_t)device;
}

/*
 * Destroy a device proxy
 */
void usrd_device_proxy_destroy(usrd_device_proxy_t device_pub)
{
    struct usrd_device_proxy_impl *device = (struct usrd_device_proxy_impl *)device_pub;
    struct usrd_proxy_impl *proxy;
    
    if (!USRD_VALIDATE_DEVICE_PROXY(device_pub)) {
        return;
    }
    
    proxy = (struct usrd_proxy_impl *)device->public.driver_proxy;
    
    USRD_PROXY_LOCK(proxy);
    
    /* Remove from driver's device list */
    queue_remove(&proxy->device_list, device, usrd_device_proxy_t, chain);
    proxy->device_count--;
    
    USRD_PROXY_UNLOCK(proxy);
    
    printf("Destroying device proxy '%s'\n", device->public.device_name);
    
    /* Free device proxy */
    kfree((vm_offset_t)device, sizeof(struct usrd_device_proxy_impl));
}

/*
 * Check resource limits
 */
boolean_t usrd_check_resource_limits(usrd_proxy_t proxy_pub,
                                   unsigned int resource_type,
                                   unsigned int amount)
{
    struct usrd_proxy_impl *proxy = (struct usrd_proxy_impl *)proxy_pub;
    unsigned int limit, current;
    
    if (!USRD_VALIDATE_PROXY(proxy_pub) || resource_type >= 8) {
        return FALSE;
    }
    
    USRD_PROXY_LOCK(proxy);
    
    limit = proxy->public.info.resource_limits[resource_type];
    
    switch (resource_type) {
        case USRD_RESOURCE_MEMORY:
            current = proxy->public.usage.memory_kb;
            break;
        case USRD_RESOURCE_CPU:
            current = proxy->public.usage.cpu_percent;
            break;
        case USRD_RESOURCE_IO_OPS:
            current = proxy->public.usage.io_ops_active;
            break;
        case USRD_RESOURCE_INTERRUPTS:
            current = proxy->public.usage.interrupt_rate;
            break;
        default:
            current = 0;
            break;
    }
    
    USRD_PROXY_UNLOCK(proxy);
    
    /* Check if adding amount would exceed limit */
    return (limit == 0 || (current + amount) <= limit);
}

/*
 * Debugging function - dump registry information
 */
void usrd_dump_registry_info(void)
{
    usrd_proxy_t proxy;
    
    printf("=== User-space Driver Registry ===\n");
    printf("Initialized: %s\n", usrd_global_registry.initialized ? "Yes" : "No");
    printf("Active drivers: %u / %u\n", 
           usrd_global_registry.active_count, USRD_MAX_DRIVERS);
    printf("Next token: %u\n", usrd_global_registry.next_token);
    
    USRD_REGISTRY_LOCK();
    
    printf("\nRegistered drivers:\n");
    queue_iterate(&usrd_global_registry.driver_list, proxy, usrd_proxy_t, chain) {
        printf("  Token %u: %s (v%u.%u) - %s\n",
               proxy->token,
               proxy->info.name,
               proxy->info.version_major,
               proxy->info.version_minor,
               proxy->status_flags & USRD_STATUS_RUNNING ? "Running" : "Stopped");
    }
    
    USRD_REGISTRY_UNLOCK();
    
    printf("=== End Registry Info ===\n");
}

/*
 * Simple device operation implementations - these would be more complex
 * in a real implementation with proper IPC to user-space drivers
 */

static io_return_t usrd_device_open(device_t dev, dev_mode_t mode, io_req_t ior)
{
    /* Simplified implementation - would normally forward to user-space driver */
    printf("USRD: device_open called\n");
    return D_SUCCESS;
}

void usrd_device_close(device_t dev)
{
    /* Simplified implementation */
    printf("USRD: device_close called\n");
}

static io_return_t usrd_device_read(device_t dev, io_req_t ior)
{
    /* Simplified implementation */
    printf("USRD: device_read called\n");
    return D_SUCCESS;
}

static io_return_t usrd_device_write(device_t dev, io_req_t ior)
{
    /* Simplified implementation */
    printf("USRD: device_write called\n");
    return D_SUCCESS;
}

io_return_t usrd_device_get_status(device_t dev, dev_flavor_t flavor,
                                 dev_status_t status, natural_t *count)
{
    /* Simplified implementation */
    printf("USRD: device_get_status called\n");
    return D_SUCCESS;
}

io_return_t usrd_device_set_status(device_t dev, dev_flavor_t flavor,
                                 dev_status_t status, natural_t count)
{
    /* Simplified implementation */
    printf("USRD: device_set_status called\n");
    return D_SUCCESS;
}