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
 * Memory usage tracking and reporting for GNU Mach.
 */

#ifndef _KERN_MEM_TRACK_H_
#define _KERN_MEM_TRACK_H_

#include <mach/machine/vm_types.h>
#include <vm/vm_types.h>
#include <sys/types.h>
#include <kern/lock.h>

/* Forward declarations */
typedef int kern_return_t;

/*
 * Memory allocation type identifiers for tracking.
 */
typedef enum {
    MEM_TYPE_GENERAL = 0,    /* General purpose kalloc */
    MEM_TYPE_VM_OBJECTS,     /* VM objects and pages */
    MEM_TYPE_IPC,            /* IPC ports, spaces, etc. */
    MEM_TYPE_THREADS,        /* Thread structures */
    MEM_TYPE_TASKS,          /* Task structures */
    MEM_TYPE_DEVICE,         /* Device drivers */
    MEM_TYPE_NETWORK,        /* Network buffers */
    MEM_TYPE_MAX
} mem_type_t;

/*
 * Memory allocation statistics per type.
 */
struct mem_stats {
    uint64_t alloc_count;     /* Number of allocations */
    uint64_t free_count;      /* Number of frees */
    uint64_t alloc_bytes;     /* Total bytes allocated */
    uint64_t free_bytes;      /* Total bytes freed */
    uint64_t current_bytes;   /* Current bytes in use */
    uint64_t peak_bytes;      /* Peak memory usage */
    uint64_t failed_allocs;   /* Failed allocation attempts */
    uint32_t large_allocs;    /* Allocations > PAGE_SIZE */
};

/*
 * System-wide memory tracking structure.
 */
struct mem_tracker {
    simple_lock_data_t lock;
    struct mem_stats stats[MEM_TYPE_MAX];
    struct mem_stats total_stats;
    
    /* Memory pressure indicators */
    uint32_t low_memory_warnings;
    uint32_t out_of_memory_events;
    vm_size_t memory_threshold_low;   /* Low memory warning threshold */
    vm_size_t memory_threshold_critical; /* Critical memory threshold */
    
    /* Pool statistics */
    uint32_t slab_cache_hits;
    uint32_t slab_cache_misses;
    uint32_t page_alloc_slow;         /* Slow path page allocations */
    uint32_t page_alloc_failed;       /* Failed page allocations */
};

/*
 * Memory allocation tracking functions.
 */
extern void mem_track_init(void);
extern void mem_track_alloc(mem_type_t type, vm_size_t size);
extern void mem_track_free(mem_type_t type, vm_size_t size);
extern void mem_track_alloc_failed(mem_type_t type, vm_size_t size);
extern void mem_track_update_cache_stats(int hits, int misses);
extern void mem_track_page_alloc_slow(void);
extern void mem_track_page_alloc_failed(void);

/*
 * Memory pressure detection.
 */
extern boolean_t mem_track_check_pressure(void);
extern void mem_track_memory_warning(void);
extern void mem_track_vm_object_inconsistency(void);
extern void mem_track_out_of_memory(void);

/*
 * Memory usage reporting.
 */
extern void mem_track_report_usage(void);
extern void mem_track_report_detailed(void);
extern kern_return_t mem_track_get_stats(mem_type_t type, struct mem_stats *stats);

/*
 * Memory thresholds configuration.
 */
extern void mem_track_set_thresholds(vm_size_t low_threshold, vm_size_t critical_threshold);

/*
 * Debug and verification functions.
 */
#ifdef MACH_DEBUG
extern void mem_track_verify_stats(void);
extern void mem_track_dump_state(void);
#endif

#endif /* _KERN_MEM_TRACK_H_ */