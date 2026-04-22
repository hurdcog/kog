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
 * Virtio Device Framework for GNU Mach
 * 
 * This header defines the virtio device framework infrastructure,
 * supporting virtio devices in virtualized environments for improved
 * performance and compatibility.
 */

#ifndef _DEVICE_VIRTIO_H_
#define _DEVICE_VIRTIO_H_

#include <device/dev_hdr.h>
#include <kern/lock.h>
#include <kern/queue.h>
#include <mach/kern_return.h>
#include <stddef.h>

/* Virtio device types */
#define VIRTIO_ID_NET        1  /* Network device */
#define VIRTIO_ID_BLOCK      2  /* Block device */
#define VIRTIO_ID_CONSOLE    3  /* Console device */
#define VIRTIO_ID_SCSI       8  /* SCSI host device */
#define VIRTIO_ID_BALLOON    5  /* Memory balloon device */

/* Virtio feature bits (common) */
#define VIRTIO_F_RING_INDIRECT_DESC  28
#define VIRTIO_F_RING_EVENT_IDX      29
#define VIRTIO_F_VERSION_1           32

/* Virtio PCI configuration space offsets */
#define VIRTIO_PCI_HOST_FEATURES     0
#define VIRTIO_PCI_GUEST_FEATURES    4
#define VIRTIO_PCI_QUEUE_PFN         8
#define VIRTIO_PCI_QUEUE_NUM         12
#define VIRTIO_PCI_QUEUE_SEL         14
#define VIRTIO_PCI_QUEUE_NOTIFY      16
#define VIRTIO_PCI_STATUS            18
#define VIRTIO_PCI_ISR               19
#define VIRTIO_PCI_CONFIG            20

/* Virtio status bits */
#define VIRTIO_STATUS_RESET          0x00
#define VIRTIO_STATUS_ACKNOWLEDGE    0x01
#define VIRTIO_STATUS_DRIVER         0x02
#define VIRTIO_STATUS_DRIVER_OK      0x04
#define VIRTIO_STATUS_FEATURES_OK    0x08
#define VIRTIO_STATUS_FAILED         0x80

/* Virtio queue descriptor flags */
#define VRING_DESC_F_NEXT     1
#define VRING_DESC_F_WRITE    2
#define VRING_DESC_F_INDIRECT 4

/* Virtio ring descriptor */
struct vring_desc {
    uint64_t addr;   /* Address (guest-physical) */
    uint32_t len;    /* Length */
    uint16_t flags;  /* Flags as above */
    uint16_t next;   /* Next field if flags & VRING_DESC_F_NEXT */
};

/* Virtio ring available */
struct vring_avail {
    uint16_t flags;
    uint16_t idx;
    uint16_t ring[];
};

/* Virtio ring used element */
struct vring_used_elem {
    uint32_t id;   /* Index of start of used descriptor chain */
    uint32_t len;  /* Total length of descriptor chain written to */
};

/* Virtio ring used */
struct vring_used {
    uint16_t flags;
    uint16_t idx;
    struct vring_used_elem ring[];
};

/* Virtio queue structure */
struct virtqueue {
    unsigned int num;                /* Number of descriptors */
    struct vring_desc *desc;         /* Descriptor table */
    struct vring_avail *avail;       /* Available ring */
    struct vring_used *used;         /* Used ring */
    uint16_t last_used_idx;          /* Last processed used index */
    void *data;                      /* Per-queue data */
    simple_lock_data_t lock;         /* Queue lock */
};

/* Forward declaration */
struct virtio_device;

/* Virtio driver operations */
struct virtio_driver {
    queue_chain_t link;              /* Driver list linkage */
    const char *name;
    uint32_t device_id;
    uint32_t *feature_table;
    unsigned int feature_table_size;
    int (*probe)(struct virtio_device *dev);
    void (*remove)(struct virtio_device *dev);
    int (*suspend)(struct virtio_device *dev);
    int (*resume)(struct virtio_device *dev);
};

/* Virtio device structure */
struct virtio_device {
    queue_chain_t link;              /* Device list linkage */
    uint32_t device_id;              /* Device type ID */
    uint32_t vendor_id;              /* Vendor ID */
    uint32_t features;               /* Negotiated features */
    struct virtio_driver *driver;   /* Associated driver */
    void *priv;                      /* Driver private data */
    
    /* Hardware access */
    vm_offset_t config_base;         /* Configuration space base */
    int irq;                         /* Interrupt line */
    
    /* Virtqueues */
    struct virtqueue **vqs;          /* Virtqueue array */
    unsigned int nvqs;               /* Number of virtqueues */
    
    simple_lock_data_t lock;         /* Device lock */
    
    /* Status */
    int status;                      /* Device status */
};

/* Virtio subsystem structure */
struct virtio_subsystem {
    queue_head_t devices;            /* List of virtio devices */
    queue_head_t drivers;            /* List of virtio drivers */
    simple_lock_data_t lock;         /* Subsystem lock */
    boolean_t initialized;           /* Initialization flag */
};

/*
 * Virtio subsystem functions
 */
extern void virtio_init(void);
extern kern_return_t virtio_register_driver(struct virtio_driver *driver);
extern void virtio_unregister_driver(struct virtio_driver *driver);
extern kern_return_t virtio_register_device(struct virtio_device *dev);
extern void virtio_unregister_device(struct virtio_device *dev);

/*
 * Virtio device functions
 */
extern struct virtio_device *virtio_alloc_device(void);
extern void virtio_free_device(struct virtio_device *dev);
extern kern_return_t virtio_setup_vqs(struct virtio_device *dev, 
                                     unsigned int nvqs,
                                     const char **names);
extern void virtio_cleanup_vqs(struct virtio_device *dev);

/*
 * Virtio queue functions
 */
extern struct virtqueue *virtio_find_vq(struct virtio_device *dev, unsigned int index);
extern kern_return_t virtio_add_buf(struct virtqueue *vq, 
                                   struct vring_desc *desc_list,
                                   unsigned int out_num,
                                   unsigned int in_num,
                                   void *data);
extern void *virtio_get_buf(struct virtqueue *vq, uint32_t *len);
extern void virtio_kick(struct virtqueue *vq);
extern void virtio_disable_cb(struct virtqueue *vq);
extern boolean_t virtio_enable_cb(struct virtqueue *vq);

/*
 * Virtio configuration functions
 */
extern uint32_t virtio_config_readl(struct virtio_device *dev, unsigned int offset);
extern uint16_t virtio_config_readw(struct virtio_device *dev, unsigned int offset);
extern uint8_t virtio_config_readb(struct virtio_device *dev, unsigned int offset);
extern void virtio_config_writel(struct virtio_device *dev, unsigned int offset, uint32_t val);
extern void virtio_config_writew(struct virtio_device *dev, unsigned int offset, uint16_t val);
extern void virtio_config_writeb(struct virtio_device *dev, unsigned int offset, uint8_t val);

/*
 * Feature negotiation
 */
extern boolean_t virtio_has_feature(struct virtio_device *dev, uint32_t feature);
extern void virtio_finalize_features(struct virtio_device *dev);
extern uint32_t virtio_get_features(struct virtio_device *dev);

/*
 * PCI transport layer
 */
struct pci_dev;  /* Forward declaration */
extern kern_return_t virtio_pci_init(void);
extern kern_return_t virtio_pci_probe_device(struct pci_dev *pci_dev);

/*
 * Debugging and statistics
 */
extern void virtio_dump_device_info(struct virtio_device *dev);
extern void virtio_dump_queue_info(struct virtqueue *vq);
extern void virtio_dump_subsystem_info(void);

/*
 * Utility macros
 */
#define VIRTIO_DEVICE_LOCK(dev) \
    simple_lock(&(dev)->lock)

#define VIRTIO_DEVICE_UNLOCK(dev) \
    simple_unlock(&(dev)->lock)

#define VIRTIO_QUEUE_LOCK(vq) \
    simple_lock(&(vq)->lock)

#define VIRTIO_QUEUE_UNLOCK(vq) \
    simple_unlock(&(vq)->lock)

#endif /* _DEVICE_VIRTIO_H_ */