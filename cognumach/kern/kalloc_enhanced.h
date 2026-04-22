/*
 * Copyright (c) 2024 GNU Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Enhanced memory allocation functions with type tracking for GNU Mach.
 */

#ifndef _KERN_KALLOC_ENHANCED_H_
#define _KERN_KALLOC_ENHANCED_H_

#include <mach/machine/vm_types.h>
#include <vm/vm_types.h>
#include <kern/mem_track.h>

/*
 * Type-specific allocation functions for better memory tracking.
 */
extern vm_offset_t kalloc_vm(vm_size_t size);
extern void kfree_vm(vm_offset_t data, vm_size_t size);

extern vm_offset_t kalloc_ipc(vm_size_t size);
extern void kfree_ipc(vm_offset_t data, vm_size_t size);

extern vm_offset_t kalloc_thread(vm_size_t size);
extern void kfree_thread(vm_offset_t data, vm_size_t size);

extern vm_offset_t kalloc_task(vm_size_t size);
extern void kfree_task(vm_offset_t data, vm_size_t size);

extern vm_offset_t kalloc_device(vm_size_t size);
extern void kfree_device(vm_offset_t data, vm_size_t size);

extern vm_offset_t kalloc_network(vm_size_t size);
extern void kfree_network(vm_offset_t data, vm_size_t size);

/*
 * Memory pool optimization functions.
 */
extern void kalloc_optimize_pools(void);
extern void kalloc_reclaim_memory(void);
extern boolean_t kalloc_check_fragmentation(void);

/*
 * Enhanced memory reporting.
 */
extern void kalloc_report_usage(void);
extern void kalloc_report_fragmentation(void);

#endif /* _KERN_KALLOC_ENHANCED_H_ */