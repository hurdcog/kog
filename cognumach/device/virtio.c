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
 * Virtio Device Framework - Core Implementation
 * 
 * Core implementation of the virtio device framework for GNU Mach,
 * providing support for virtio devices in virtualized environments.
 */

#include <device/virtio.h>
#include <device/ds_routines.h>
#include <kern/printf.h>
#include <kern/task.h>
#include <kern/kalloc.h>
#include <vm/vm_map.h>
#include <vm/vm_kern.h>
#include <ipc/ipc_port.h>
#include <string.h>
#include <machine/pio.h>

/* Global virtio subsystem */
static struct virtio_subsystem virtio_subsys;

/*
 * Initialize the virtio subsystem
 */
void virtio_init(void)
{
    printf("VIRTIO: Initializing virtio subsystem\n");
    
    /* Initialize device and driver lists */
    queue_init(&virtio_subsys.devices);
    queue_init(&virtio_subsys.drivers);
    simple_lock_init(&virtio_subsys.lock);
    virtio_subsys.initialized = TRUE;
    
    printf("VIRTIO: Subsystem initialized successfully\n");
}

/*
 * Register a virtio driver
 */
kern_return_t virtio_register_driver(struct virtio_driver *driver)
{
    struct virtio_device *dev;
    
    if (!driver || !driver->name) {
        return KERN_INVALID_ARGUMENT;
    }
    
    printf("VIRTIO: Registering driver '%s' for device ID %u\n",
           driver->name, driver->device_id);
    
    simple_lock(&virtio_subsys.lock);
    
    /* Add driver to list */
    queue_enter(&virtio_subsys.drivers, driver, struct virtio_driver *, link);
    
    /* Try to match with existing devices */
    queue_iterate(&virtio_subsys.devices, dev, struct virtio_device *, link) {
        if (dev->device_id == driver->device_id && !dev->driver) {
            dev->driver = driver;
            simple_unlock(&virtio_subsys.lock);
            
            /* Probe the device */
            if (driver->probe && driver->probe(dev) == 0) {
                printf("VIRTIO: Driver '%s' successfully probed device\n", 
                       driver->name);
            } else {
                dev->driver = NULL;
                printf("VIRTIO: Driver '%s' failed to probe device\n", 
                       driver->name);
            }
            
            simple_lock(&virtio_subsys.lock);
        }
    }
    
    simple_unlock(&virtio_subsys.lock);
    return KERN_SUCCESS;
}

/*
 * Unregister a virtio driver
 */
void virtio_unregister_driver(struct virtio_driver *driver)
{
    struct virtio_device *dev;
    
    if (!driver) {
        return;
    }
    
    printf("VIRTIO: Unregistering driver '%s'\n", driver->name);
    
    simple_lock(&virtio_subsys.lock);
    
    /* Remove from devices using this driver */
    queue_iterate(&virtio_subsys.devices, dev, struct virtio_device *, link) {
        if (dev->driver == driver) {
            if (driver->remove) {
                driver->remove(dev);
            }
            dev->driver = NULL;
        }
    }
    
    /* Remove from driver list */
    queue_remove(&virtio_subsys.drivers, driver, struct virtio_driver *, link);
    
    simple_unlock(&virtio_subsys.lock);
}

/*
 * Register a virtio device
 */
kern_return_t virtio_register_device(struct virtio_device *dev)
{
    struct virtio_driver *driver;
    
    if (!dev) {
        return KERN_INVALID_ARGUMENT;
    }
    
    printf("VIRTIO: Registering device ID %u\n", dev->device_id);
    
    simple_lock(&virtio_subsys.lock);
    
    /* Add device to list */
    queue_enter(&virtio_subsys.devices, dev, struct virtio_device *, link);
    
    /* Try to match with existing drivers */
    queue_iterate(&virtio_subsys.drivers, driver, struct virtio_driver *, link) {
        if (driver->device_id == dev->device_id && !dev->driver) {
            dev->driver = driver;
            simple_unlock(&virtio_subsys.lock);
            
            /* Probe the device */
            if (driver->probe && driver->probe(dev) == 0) {
                printf("VIRTIO: Device ID %u successfully probed by '%s'\n",
                       dev->device_id, driver->name);
                simple_lock(&virtio_subsys.lock);
                break;
            } else {
                dev->driver = NULL;
                printf("VIRTIO: Device ID %u failed probe by '%s'\n",
                       dev->device_id, driver->name);
            }
            
            simple_lock(&virtio_subsys.lock);
        }
    }
    
    simple_unlock(&virtio_subsys.lock);
    return KERN_SUCCESS;
}

/*
 * Unregister a virtio device
 */
void virtio_unregister_device(struct virtio_device *dev)
{
    if (!dev) {
        return;
    }
    
    printf("VIRTIO: Unregistering device ID %u\n", dev->device_id);
    
    simple_lock(&virtio_subsys.lock);
    
    /* Remove driver if attached */
    if (dev->driver && dev->driver->remove) {
        dev->driver->remove(dev);
    }
    
    /* Clean up virtqueues */
    virtio_cleanup_vqs(dev);
    
    /* Remove from device list */
    queue_remove(&virtio_subsys.devices, dev, struct virtio_device *, link);
    
    simple_unlock(&virtio_subsys.lock);
}

/*
 * Allocate a new virtio device
 */
struct virtio_device *virtio_alloc_device(void)
{
    struct virtio_device *dev;
    
    dev = (struct virtio_device *)kalloc(sizeof(struct virtio_device));
    if (!dev) {
        return NULL;
    }
    
    /* Initialize device structure */
    memset(dev, 0, sizeof(struct virtio_device));
    simple_lock_init(&dev->lock);
    
    return dev;
}

/*
 * Free a virtio device
 */
void virtio_free_device(struct virtio_device *dev)
{
    if (!dev) {
        return;
    }
    
    /* Clean up virtqueues if any */
    virtio_cleanup_vqs(dev);
    
    kfree((vm_offset_t)dev, sizeof(struct virtio_device));
}

/*
 * Setup virtqueues for a device
 */
kern_return_t virtio_setup_vqs(struct virtio_device *dev, 
                               unsigned int nvqs,
                               const char **names)
{
    unsigned int i;
    
    if (!dev || nvqs == 0) {
        return KERN_INVALID_ARGUMENT;
    }
    
    printf("VIRTIO: Setting up %u virtqueues for device ID %u\n", 
           nvqs, dev->device_id);
    
    /* Allocate virtqueue array */
    dev->vqs = (struct virtqueue **)kalloc(nvqs * sizeof(struct virtqueue *));
    if (!dev->vqs) {
        return KERN_RESOURCE_SHORTAGE;
    }
    
    dev->nvqs = nvqs;
    
    /* Initialize each virtqueue */
    for (i = 0; i < nvqs; i++) {
        dev->vqs[i] = (struct virtqueue *)kalloc(sizeof(struct virtqueue));
        if (!dev->vqs[i]) {
            /* Clean up previously allocated queues */
            while (i > 0) {
                kfree((vm_offset_t)dev->vqs[--i], sizeof(struct virtqueue));
            }
            kfree((vm_offset_t)dev->vqs, nvqs * sizeof(struct virtqueue *));
            dev->vqs = NULL;
            dev->nvqs = 0;
            return KERN_RESOURCE_SHORTAGE;
        }
        
        /* Initialize virtqueue structure */
        memset(dev->vqs[i], 0, sizeof(struct virtqueue));
        simple_lock_init(&dev->vqs[i]->lock);
        
        printf("VIRTIO: Initialized virtqueue %u (%s)\n", 
               i, names ? names[i] : "unnamed");
    }
    
    return KERN_SUCCESS;
}

/*
 * Clean up virtqueues for a device
 */
void virtio_cleanup_vqs(struct virtio_device *dev)
{
    unsigned int i;
    
    if (!dev || !dev->vqs) {
        return;
    }
    
    printf("VIRTIO: Cleaning up %u virtqueues for device ID %u\n",
           dev->nvqs, dev->device_id);
    
    /* Free each virtqueue */
    for (i = 0; i < dev->nvqs; i++) {
        if (dev->vqs[i]) {
            /* Free descriptor tables if allocated */
            if (dev->vqs[i]->desc) {
                kfree((vm_offset_t)dev->vqs[i]->desc, 
                      dev->vqs[i]->num * sizeof(struct vring_desc));
            }
            
            kfree((vm_offset_t)dev->vqs[i], sizeof(struct virtqueue));
        }
    }
    
    /* Free virtqueue array */
    kfree((vm_offset_t)dev->vqs, dev->nvqs * sizeof(struct virtqueue *));
    dev->vqs = NULL;
    dev->nvqs = 0;
}

/*
 * Find a virtqueue by index
 */
struct virtqueue *virtio_find_vq(struct virtio_device *dev, unsigned int index)
{
    if (!dev || !dev->vqs || index >= dev->nvqs) {
        return NULL;
    }
    
    return dev->vqs[index];
}

/*
 * Configuration space access functions
 */
uint32_t virtio_config_readl(struct virtio_device *dev, unsigned int offset)
{
    if (!dev || !dev->config_base) {
        return 0;
    }
    
    return inl(dev->config_base + offset);
}

uint16_t virtio_config_readw(struct virtio_device *dev, unsigned int offset)
{
    if (!dev || !dev->config_base) {
        return 0;
    }
    
    return inw(dev->config_base + offset);
}

uint8_t virtio_config_readb(struct virtio_device *dev, unsigned int offset)
{
    if (!dev || !dev->config_base) {
        return 0;
    }
    
    return inb(dev->config_base + offset);
}

void virtio_config_writel(struct virtio_device *dev, unsigned int offset, uint32_t val)
{
    if (!dev || !dev->config_base) {
        return;
    }
    
    outl(dev->config_base + offset, val);
}

void virtio_config_writew(struct virtio_device *dev, unsigned int offset, uint16_t val)
{
    if (!dev || !dev->config_base) {
        return;
    }
    
    outw(dev->config_base + offset, val);
}

void virtio_config_writeb(struct virtio_device *dev, unsigned int offset, uint8_t val)
{
    if (!dev || !dev->config_base) {
        return;
    }
    
    outb(dev->config_base + offset, val);
}

/*
 * Feature negotiation
 */
boolean_t virtio_has_feature(struct virtio_device *dev, uint32_t feature)
{
    if (!dev) {
        return FALSE;
    }
    
    return (dev->features & (1U << feature)) != 0;
}

uint32_t virtio_get_features(struct virtio_device *dev)
{
    if (!dev) {
        return 0;
    }
    
    return virtio_config_readl(dev, VIRTIO_PCI_HOST_FEATURES);
}

void virtio_finalize_features(struct virtio_device *dev)
{
    if (!dev) {
        return;
    }
    
    /* Write guest features back to device */
    virtio_config_writel(dev, VIRTIO_PCI_GUEST_FEATURES, dev->features);
    
    printf("VIRTIO: Finalized features 0x%x for device ID %u\n",
           dev->features, dev->device_id);
}

/*
 * Queue operations (simplified implementations)
 */
kern_return_t virtio_add_buf(struct virtqueue *vq, 
                            struct vring_desc *desc_list,
                            unsigned int out_num,
                            unsigned int in_num,
                            void *data)
{
    /* Simplified implementation - would need full ring management */
    if (!vq || !desc_list) {
        return KERN_INVALID_ARGUMENT;
    }
    
    /* For now, just store the data pointer */
    vq->data = data;
    
    return KERN_SUCCESS;
}

void *virtio_get_buf(struct virtqueue *vq, uint32_t *len)
{
    void *data;
    
    if (!vq) {
        return NULL;
    }
    
    /* Simplified implementation */
    data = vq->data;
    vq->data = NULL;
    
    if (len) {
        *len = 0;  /* Would return actual length */
    }
    
    return data;
}

void virtio_kick(struct virtqueue *vq)
{
    /* Simplified implementation - would notify device */
    if (vq) {
        printf("VIRTIO: Kicked virtqueue\n");
    }
}

void virtio_disable_cb(struct virtqueue *vq)
{
    /* Simplified implementation */
    (void)vq;
}

boolean_t virtio_enable_cb(struct virtqueue *vq)
{
    /* Simplified implementation */
    (void)vq;
    return TRUE;
}

/*
 * Debugging functions
 */
void virtio_dump_device_info(struct virtio_device *dev)
{
    if (!dev) {
        printf("VIRTIO: NULL device\n");
        return;
    }
    
    printf("VIRTIO Device Info:\n");
    printf("  Device ID: %u\n", dev->device_id);
    printf("  Vendor ID: %u\n", dev->vendor_id);
    printf("  Features: 0x%x\n", dev->features);
    printf("  Status: %d\n", dev->status);
    printf("  Config Base: 0x%x\n", dev->config_base);
    printf("  IRQ: %d\n", dev->irq);
    printf("  Virtqueues: %u\n", dev->nvqs);
    printf("  Driver: %s\n", dev->driver ? dev->driver->name : "none");
}

void virtio_dump_subsystem_info(void)
{
    struct virtio_device *dev;
    struct virtio_driver *driver;
    int dev_count = 0, driver_count = 0;
    
    printf("VIRTIO Subsystem Info:\n");
    printf("  Initialized: %s\n", virtio_subsys.initialized ? "yes" : "no");
    
    simple_lock(&virtio_subsys.lock);
    
    printf("  Registered devices:\n");
    queue_iterate(&virtio_subsys.devices, dev, struct virtio_device *, link) {
        printf("    Device %d: ID=%u, Driver=%s\n", 
               dev_count++, dev->device_id,
               dev->driver ? dev->driver->name : "none");
    }
    
    printf("  Registered drivers:\n");
    queue_iterate(&virtio_subsys.drivers, driver, struct virtio_driver *, link) {
        printf("    Driver %d: %s (ID=%u)\n",
               driver_count++, driver->name, driver->device_id);
    }
    
    simple_unlock(&virtio_subsys.lock);
    
    printf("  Total: %d devices, %d drivers\n", dev_count, driver_count);
}