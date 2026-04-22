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
 * User-space Device Driver Types
 * 
 * Defines types, constants, and structures for user-space device drivers.
 */

#ifndef _DEVICE_USERSPACE_DRIVER_TYPES_H_
#define _DEVICE_USERSPACE_DRIVER_TYPES_H_

#include <mach/mach_types.h>
#include <device/device_types.h>

/*
 * Driver capability flags
 */
#define USRD_CAP_NONE           0x00000000  /* No special capabilities */
#define USRD_CAP_BLOCK_DEVICE   0x00000001  /* Block device driver */
#define USRD_CAP_CHAR_DEVICE    0x00000002  /* Character device driver */
#define USRD_CAP_NETWORK        0x00000004  /* Network device driver */
#define USRD_CAP_GRAPHICS       0x00000008  /* Graphics device driver */
#define USRD_CAP_AUDIO          0x00000010  /* Audio device driver */
#define USRD_CAP_INPUT          0x00000020  /* Input device driver */
#define USRD_CAP_STORAGE        0x00000040  /* Storage device driver */
#define USRD_CAP_USB            0x00000080  /* USB device driver */
#define USRD_CAP_PCI            0x00000100  /* PCI device driver */
#define USRD_CAP_INTERRUPT      0x00000200  /* Interrupt handling capability */
#define USRD_CAP_DMA            0x00000400  /* DMA capability */
#define USRD_CAP_POWER_MGMT     0x00000800  /* Power management */
#define USRD_CAP_HOTPLUG        0x00001000  /* Hot plug/unplug support */

/*
 * Driver status flags
 */
#define USRD_STATUS_RUNNING     0x00000001  /* Driver is running */
#define USRD_STATUS_IDLE        0x00000002  /* Driver is idle */
#define USRD_STATUS_BUSY        0x00000004  /* Driver is busy */
#define USRD_STATUS_ERROR       0x00000008  /* Driver has an error */
#define USRD_STATUS_SUSPENDED   0x00000010  /* Driver is suspended */
#define USRD_STATUS_RECOVERING  0x00000020  /* Driver is recovering from error */

/*
 * Resource type indices for resource_limits array
 */
#define USRD_RESOURCE_MEMORY    0   /* Memory usage limit (KB) */
#define USRD_RESOURCE_CPU       1   /* CPU usage limit (percentage) */
#define USRD_RESOURCE_IO_OPS    2   /* Max concurrent I/O operations */
#define USRD_RESOURCE_INTERRUPTS 3  /* Max interrupt rate (per second) */
#define USRD_RESOURCE_TIMEOUT   4   /* Operation timeout (milliseconds) */
#define USRD_RESOURCE_BANDWIDTH 5   /* Bandwidth limit (KB/s) */
#define USRD_RESOURCE_RESERVED1 6   /* Reserved for future use */
#define USRD_RESOURCE_RESERVED2 7   /* Reserved for future use */

/*
 * Driver registration token type
 */
typedef unsigned int usrd_token_t;
#define USRD_TOKEN_INVALID      0

/*
 * Driver information structure (used in registration)
 */
struct usrd_driver_info {
    char        name[64];               /* Driver name */
    char        description[128];       /* Driver description */
    unsigned int version_major;         /* Major version */
    unsigned int version_minor;         /* Minor version */
    unsigned int capabilities;          /* Driver capabilities (USRD_CAP_*) */
    unsigned int resource_limits[8];    /* Resource limits */
};

typedef struct usrd_driver_info *usrd_driver_info_t;

/*
 * Driver resource usage structure
 */
struct usrd_resource_usage {
    unsigned int memory_kb;             /* Current memory usage (KB) */
    unsigned int cpu_percent;           /* Current CPU usage (percentage) */
    unsigned int io_ops_active;         /* Active I/O operations */
    unsigned int interrupt_rate;        /* Current interrupt rate */
    unsigned int avg_response_time;     /* Average response time (ms) */
    unsigned int error_count;           /* Total error count */
    unsigned int recovery_count;        /* Total recovery attempts */
    unsigned int uptime_seconds;        /* Driver uptime */
};

typedef struct usrd_resource_usage *usrd_resource_usage_t;

/*
 * Driver registry interface
 */
struct usrd_registry {
    mach_port_t registry_port;          /* Registry port */
    unsigned int max_drivers;           /* Maximum number of drivers */
    unsigned int active_drivers;        /* Currently active drivers */
};

typedef struct usrd_registry *usrd_registry_t;

/*
 * Driver proxy structure (kernel-side representation of user-space driver)
 */
struct usrd_proxy {
    usrd_token_t token;                 /* Registration token */
    mach_port_t driver_port;            /* Driver communication port */
    struct usrd_driver_info info;       /* Driver information */
    struct usrd_resource_usage usage;   /* Current resource usage */
    unsigned int status_flags;          /* Current status */
    unsigned int last_heartbeat;        /* Last heartbeat timestamp */
    boolean_t is_trusted;               /* Trusted driver flag */
};

typedef struct usrd_proxy *usrd_proxy_t;

/*
 * Device proxy structure (represents a device handled by user-space driver)
 */
struct usrd_device_proxy {
    mach_port_t device_port;            /* Device port in user space */
    usrd_proxy_t driver_proxy;          /* Associated driver proxy */
    char device_name[64];               /* Device name */
    unsigned int device_type;           /* Device type flags */
    boolean_t is_open;                  /* Device open state */
    unsigned int open_count;            /* Number of open handles */
};

typedef struct usrd_device_proxy *usrd_device_proxy_t;

/*
 * Driver development SDK constants
 */
#define USRD_MAX_DRIVER_NAME    64
#define USRD_MAX_DRIVER_DESC    128
#define USRD_MAX_DEVICE_NAME    64
#define USRD_MAX_DRIVERS        256
#define USRD_MAX_DEVICES_PER_DRIVER 32

/*
 * Error codes specific to user-space drivers
 */
#define USRD_SUCCESS            0
#define USRD_ERROR_INVALID_DRIVER    1
#define USRD_ERROR_DRIVER_EXISTS     2
#define USRD_ERROR_NO_RESOURCES      3
#define USRD_ERROR_NOT_REGISTERED    4
#define USRD_ERROR_DEVICE_BUSY       5
#define USRD_ERROR_TIMEOUT           6
#define USRD_ERROR_PERMISSION_DENIED 7
#define USRD_ERROR_DRIVER_CRASHED    8

#endif /* _DEVICE_USERSPACE_DRIVER_TYPES_H_ */