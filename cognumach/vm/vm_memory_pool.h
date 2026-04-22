/*
 * Copyright (c) 2024-2025 GNU Mach project
 *
 * Phase 2: Memory Management Enhancements
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * Enhanced Memory Pool Management for GNU Mach
 *
 * This module provides improved memory pool management with:
 * - Better memory allocation strategies
 * - Reduced memory leaks through improved tracking
 * - Memory usage reporting
 * - Page allocation optimization
 */

#ifndef _VM_MEMORY_POOL_H_
#define _VM_MEMORY_POOL_H_

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <kern/lock.h>
#include <kern/queue.h>
#include <vm/vm_types.h>

/*
 * Memory pool configuration
 */
#define VM_POOL_MIN_SIZE        PAGE_SIZE
#define VM_POOL_MAX_SIZE        (256 * PAGE_SIZE)
#define VM_POOL_SIZE_CLASSES    8
#define VM_POOL_HASH_SIZE       64

/*
 * Memory pool allocation flags
 */
#define VM_POOL_WAIT            0x0001  /* Wait for memory if unavailable */
#define VM_POOL_NOWAIT          0x0002  /* Don't wait, return NULL */
#define VM_POOL_ZERO            0x0004  /* Zero-fill allocated memory */
#define VM_POOL_NOTRACK         0x0008  /* Don't track allocation (internal) */

/*
 * Pool allocation states
 */
typedef enum {
    VM_POOL_STATE_FREE = 0,
    VM_POOL_STATE_ALLOCATED = 1,
    VM_POOL_STATE_RESERVED = 2
} vm_pool_state_t;

/*
 * Memory pool allocation header
 *
 * Each allocation is prefixed with this header for tracking
 */
struct vm_pool_header {
    unsigned int        magic;          /* Magic number for validation */
    vm_size_t           size;           /* Actual allocation size */
    unsigned int        flags;          /* Allocation flags */
    vm_pool_state_t     state;          /* Current state */
    const char          *caller;        /* Allocation site (debug) */
    unsigned int        line;           /* Line number (debug) */
    queue_chain_t       track_link;     /* Link for allocation tracking */
};

typedef struct vm_pool_header *vm_pool_header_t;

#define VM_POOL_MAGIC           0xCAFEBABE
#define VM_POOL_HEADER_SIZE     (sizeof(struct vm_pool_header))

/*
 * Memory pool statistics
 */
struct vm_pool_stats {
    unsigned long       total_allocations;
    unsigned long       total_frees;
    unsigned long       current_allocations;
    vm_size_t           current_bytes;
    vm_size_t           peak_bytes;
    unsigned long       pool_hits;
    unsigned long       pool_misses;
    unsigned long       failed_allocations;
};

typedef struct vm_pool_stats *vm_pool_stats_t;

/*
 * Size class for pooled allocations
 */
struct vm_pool_size_class {
    vm_size_t           size;           /* Size of allocations in this class */
    queue_head_t        free_list;      /* Free allocations */
    unsigned int        free_count;     /* Number of free entries */
    unsigned int        max_free;       /* Max free to keep */
    simple_lock_data_t  lock;           /* Per-class lock */
};

typedef struct vm_pool_size_class *vm_pool_size_class_t;

/*
 * Memory pool structure
 */
struct vm_memory_pool {
    const char          *name;          /* Pool name for debugging */

    /* Size-classed allocation pools */
    struct vm_pool_size_class size_classes[VM_POOL_SIZE_CLASSES];

    /* Statistics */
    struct vm_pool_stats stats;

    /* Allocation tracking */
    boolean_t           track_allocations;
    queue_head_t        active_allocations;

    /* Synchronization */
    simple_lock_data_t  lock;
};

typedef struct vm_memory_pool *vm_memory_pool_t;

/*
 * Memory pressure notification callback
 */
typedef void (*vm_pool_pressure_callback_t)(vm_size_t requested_free);

/*
 * Memory pool operations
 */

/* System initialization */
void vm_memory_pool_init(void);

/* Pool creation and destruction */
vm_memory_pool_t vm_memory_pool_create(const char *name, boolean_t track);
void vm_memory_pool_destroy(vm_memory_pool_t pool);

/* Allocation and deallocation */
void *vm_pool_alloc(vm_memory_pool_t pool, vm_size_t size, unsigned int flags);
void *vm_pool_alloc_tracked(vm_memory_pool_t pool, vm_size_t size,
                            unsigned int flags, const char *caller,
                            unsigned int line);
void vm_pool_free(vm_memory_pool_t pool, void *addr);

/* Size class management */
vm_size_t vm_pool_size_class_for_size(vm_size_t size);
void vm_pool_trim_size_class(vm_memory_pool_t pool, unsigned int class_index);

/* Statistics and reporting */
void vm_pool_get_stats(vm_memory_pool_t pool, vm_pool_stats_t stats);
void vm_pool_print_stats(vm_memory_pool_t pool);
void vm_pool_print_allocations(vm_memory_pool_t pool);

/* Memory pressure handling */
void vm_pool_register_pressure_callback(vm_pool_pressure_callback_t callback);
void vm_pool_memory_pressure(vm_size_t target_free);

/* Global memory pool (for general kernel allocations) */
extern vm_memory_pool_t kernel_memory_pool;

/*
 * Convenience macros for tracked allocations
 */
#ifdef VM_POOL_TRACK_ALLOCS
#define vm_pool_malloc(pool, size) \
    vm_pool_alloc_tracked(pool, size, VM_POOL_WAIT, __func__, __LINE__)
#define vm_pool_calloc(pool, size) \
    vm_pool_alloc_tracked(pool, size, VM_POOL_WAIT | VM_POOL_ZERO, __func__, __LINE__)
#else
#define vm_pool_malloc(pool, size) \
    vm_pool_alloc(pool, size, VM_POOL_WAIT)
#define vm_pool_calloc(pool, size) \
    vm_pool_alloc(pool, size, VM_POOL_WAIT | VM_POOL_ZERO)
#endif

/*
 * Memory usage monitoring
 */
struct vm_memory_usage {
    vm_size_t           total_physical;     /* Total physical memory */
    vm_size_t           used_physical;      /* Used physical memory */
    vm_size_t           free_physical;      /* Free physical memory */
    vm_size_t           wired_memory;       /* Wired (non-pageable) memory */
    vm_size_t           kernel_memory;      /* Kernel memory usage */
    vm_size_t           cache_memory;       /* Block cache memory */
    vm_size_t           pool_memory;        /* Memory pool usage */
    unsigned int        page_faults;        /* Total page faults */
    unsigned int        page_ins;           /* Pages paged in */
    unsigned int        page_outs;          /* Pages paged out */
};

typedef struct vm_memory_usage *vm_memory_usage_t;

/* Memory usage operations */
void vm_get_memory_usage(vm_memory_usage_t usage);
void vm_print_memory_usage(void);

/*
 * Page allocation optimization
 */

/* Page coloring for cache optimization */
#define VM_PAGE_COLORS      16
void vm_page_set_color_preference(vm_object_t object, unsigned int color);

/* Page clustering for sequential access */
kern_return_t vm_page_cluster_alloc(vm_object_t object, vm_offset_t offset,
                                    unsigned int count, vm_page_t *pages);
void vm_page_cluster_free(vm_page_t *pages, unsigned int count);

/* Large page support */
boolean_t vm_large_page_supported(void);
kern_return_t vm_large_page_alloc(vm_object_t object, vm_offset_t offset,
                                  vm_page_t *large_page);
void vm_large_page_free(vm_page_t large_page);

#endif /* _VM_MEMORY_POOL_H_ */
