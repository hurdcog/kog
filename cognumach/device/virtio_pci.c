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
 * Virtio PCI Transport Layer
 * 
 * PCI transport layer implementation for virtio devices, handling
 * PCI device discovery, configuration, and communication.
 */

#include <device/virtio.h>
#include <device/ds_routines.h>
#include <kern/printf.h>
#include <kern/kalloc.h>
#include <machine/pio.h>

/* Virtio PCI vendor and device IDs */
#define VIRTIO_PCI_VENDOR_ID    0x1AF4
#define VIRTIO_PCI_DEVICE_MIN   0x1000
#define VIRTIO_PCI_DEVICE_MAX   0x103F

/* PCI configuration space */
struct pci_dev {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    uint32_t bar[6];
    uint32_t cardbus_cis;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_device_id;
    uint32_t expansion_rom_base;
    uint8_t capabilities_ptr;
    uint8_t reserved[7];
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_gnt;
    uint8_t max_lat;
};

/* PCI access functions (simplified) */
static uint32_t pci_config_read32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t address = (uint32_t)((bus << 16) | (slot << 11) | 
                                  (func << 8) | (offset & 0xFC) | 0x80000000);
    
    outl(0xCF8, address);
    return inl(0xCFC);
}

static uint16_t pci_config_read16(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t data = pci_config_read32(bus, slot, func, offset & 0xFC);
    return (uint16_t)((data >> ((offset & 2) * 8)) & 0xFFFF);
}

static void pci_config_write32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value)
{
    uint32_t address = (uint32_t)((bus << 16) | (slot << 11) | 
                                  (func << 8) | (offset & 0xFC) | 0x80000000);
    
    outl(0xCF8, address);
    outl(0xCFC, value);
}

static void pci_config_write16(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t value)
{
    uint32_t address = (uint32_t)((bus << 16) | (slot << 11) | 
                                  (func << 8) | (offset & 0xFC) | 0x80000000);
    uint32_t data;
    
    outl(0xCF8, address);
    data = inl(0xCFC);
    
    /* Modify the appropriate 16-bit portion */
    if (offset & 2) {
        data = (data & 0x0000FFFF) | ((uint32_t)value << 16);
    } else {
        data = (data & 0xFFFF0000) | value;
    }
    
    outl(0xCFC, data);
}

/*
 * Initialize a virtio device from PCI configuration
 */
static kern_return_t virtio_pci_init_device(uint8_t bus, uint8_t slot, uint8_t func, 
                                          uint16_t device_id)
{
    struct virtio_device *vdev;
    uint32_t bar0;
    uint16_t command;
    
    printf("VIRTIO-PCI: Initializing device at %02x:%02x.%x (ID=0x%04x)\n",
           bus, slot, func, device_id);
    
    /* Allocate virtio device */
    vdev = virtio_alloc_device();
    if (!vdev) {
        printf("VIRTIO-PCI: Failed to allocate device structure\n");
        return KERN_RESOURCE_SHORTAGE;
    }
    
    /* Set device type based on PCI device ID */
    vdev->device_id = device_id - VIRTIO_PCI_DEVICE_MIN;  /* Convert to virtio device ID */
    vdev->vendor_id = VIRTIO_PCI_VENDOR_ID;
    
    /* Read BAR0 for I/O base address */
    bar0 = pci_config_read32(bus, slot, func, 0x10);
    if (bar0 & 1) {
        /* I/O port space */
        vdev->config_base = bar0 & ~3;
        printf("VIRTIO-PCI: I/O base at 0x%lx\n", vdev->config_base);
    } else {
        /* Memory space - not supported in this simple implementation */
        printf("VIRTIO-PCI: Memory-mapped I/O not supported\n");
        virtio_free_device(vdev);
        return KERN_FAILURE;
    }
    
    /* Enable PCI device */
    command = pci_config_read16(bus, slot, func, 0x04);
    command |= 0x05;  /* Enable I/O space and bus mastering */
    pci_config_write16(bus, slot, func, 0x04, command);
    
    /* Read interrupt line */
    vdev->irq = pci_config_read32(bus, slot, func, 0x3C) & 0xFF;
    printf("VIRTIO-PCI: IRQ line %d\n", vdev->irq);
    
    /* Initialize device */
    virtio_config_writeb(vdev, VIRTIO_PCI_STATUS, VIRTIO_STATUS_RESET);
    virtio_config_writeb(vdev, VIRTIO_PCI_STATUS, VIRTIO_STATUS_ACKNOWLEDGE);
    virtio_config_writeb(vdev, VIRTIO_PCI_STATUS, VIRTIO_STATUS_DRIVER);
    
    /* Read host features */
    vdev->features = virtio_get_features(vdev);
    printf("VIRTIO-PCI: Host features: 0x%x\n", vdev->features);
    
    /* Register the device with virtio subsystem */
    if (virtio_register_device(vdev) != KERN_SUCCESS) {
        printf("VIRTIO-PCI: Failed to register device\n");
        virtio_free_device(vdev);
        return KERN_FAILURE;
    }
    
    printf("VIRTIO-PCI: Device initialized successfully\n");
    return KERN_SUCCESS;
}

/*
 * Probe for virtio devices on PCI bus
 */
static void virtio_pci_scan_bus(void)
{
    uint8_t bus, slot, func;
    uint16_t vendor_id, device_id;
    int device_count = 0;
    
    printf("VIRTIO-PCI: Scanning PCI bus for virtio devices\n");
    
    /* Scan all PCI buses, slots, and functions */
    for (bus = 0; bus < 256; bus++) {
        for (slot = 0; slot < 32; slot++) {
            for (func = 0; func < 8; func++) {
                /* Read vendor ID */
                vendor_id = pci_config_read16(bus, slot, func, 0x00);
                
                /* Skip if no device present */
                if (vendor_id == 0xFFFF) {
                    continue;
                }
                
                /* Check if this is a virtio device */
                if (vendor_id == VIRTIO_PCI_VENDOR_ID) {
                    device_id = pci_config_read16(bus, slot, func, 0x02);
                    
                    if (device_id >= VIRTIO_PCI_DEVICE_MIN && 
                        device_id <= VIRTIO_PCI_DEVICE_MAX) {
                        
                        printf("VIRTIO-PCI: Found virtio device at %02x:%02x.%x "
                               "(vendor=0x%04x, device=0x%04x)\n",
                               bus, slot, func, vendor_id, device_id);
                        
                        if (virtio_pci_init_device(bus, slot, func, device_id) == KERN_SUCCESS) {
                            device_count++;
                        }
                    }
                }
                
                /* For single-function devices, don't check other functions */
                if (func == 0) {
                    uint8_t header_type = pci_config_read32(bus, slot, func, 0x0C) >> 16;
                    if ((header_type & 0x80) == 0) {
                        break;  /* Single function device */
                    }
                }
            }
        }
    }
    
    printf("VIRTIO-PCI: Found %d virtio devices\n", device_count);
}

/*
 * Initialize virtio PCI transport
 */
kern_return_t virtio_pci_init(void)
{
    printf("VIRTIO-PCI: Initializing PCI transport layer\n");
    
    /* Scan for virtio devices */
    virtio_pci_scan_bus();
    
    printf("VIRTIO-PCI: PCI transport layer initialized\n");
    return KERN_SUCCESS;
}

/*
 * Probe a specific PCI device for virtio compatibility
 */
kern_return_t virtio_pci_probe_device(struct pci_dev *pci_dev)
{
    if (!pci_dev) {
        return KERN_INVALID_ARGUMENT;
    }
    
    /* Check if this is a virtio device */
    if (pci_dev->vendor_id != VIRTIO_PCI_VENDOR_ID ||
        pci_dev->device_id < VIRTIO_PCI_DEVICE_MIN ||
        pci_dev->device_id > VIRTIO_PCI_DEVICE_MAX) {
        return KERN_FAILURE;
    }
    
    printf("VIRTIO-PCI: Probing virtio device (vendor=0x%04x, device=0x%04x)\n",
           pci_dev->vendor_id, pci_dev->device_id);
    
    /* Initialize the device - simplified since we don't have full PCI coordinates */
    return KERN_SUCCESS;
}