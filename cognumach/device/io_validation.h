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
 * Device I/O Request Validation and Isolation Headers
 */

#ifndef _DEVICE_IO_VALIDATION_H_
#define _DEVICE_IO_VALIDATION_H_

#include <device/io_req.h>
#include <device/dev_hdr.h>
#include <mach/kern_return.h>

/*
 * I/O validation flags
 */
#define IO_VALIDATE_BASIC   0x01
#define IO_VALIDATE_BOUNDS  0x02  
#define IO_VALIDATE_DEVICE  0x04
#define IO_VALIDATE_TIMEOUT 0x08
#define IO_VALIDATE_ALL     0xFF

/*
 * Enhanced I/O request with validation
 */
struct io_req_validated;
typedef struct io_req_validated *io_req_validated_t;

/*
 * Function prototypes
 */
extern io_req_validated_t io_req_create_validated(io_req_t original_ior, 
                                                 unsigned int validation_flags,
                                                 unsigned int timeout_ms);

extern kern_return_t io_req_validate(io_req_t ior, mach_device_t device, 
                                    unsigned int validation_flags);

extern kern_return_t device_io_validated(mach_device_t device, io_req_t ior, 
                                        unsigned int validation_flags);

extern void io_req_timeout_check(void);
extern void io_req_validated_cleanup(io_req_validated_t validated_ior);

#endif /* _DEVICE_IO_VALIDATION_H_ */