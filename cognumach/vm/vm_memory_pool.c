/*
 * Copyright (c) 2024-2025 GNU Mach project
 *
 * Phase 2: Memory Management Enhancements Implementation
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * Enhanced Memory Pool Management Implementation
 */

#include <string.h>
#include <kern/assert.h>
#include <kern/debug.h>
#include <kern/kalloc.h>
#include <kern/slab.h>
#include <kern/printf.h>
#include <mach/vm_param.h>
#include <vm/vm_memory_pool.h>
#include <vm/vm_page.h>
#include <vm/vm_object.h>
#include <vm/vm_kern.h>

/* Global kernel memory pool */
static struct vm_memory_pool kernel_pool_store;
vm_memory_pool_t kernel_memory_pool = &kernel_pool_store;

/* Size classes (powers of 2) */
static const vm_size_t size_class_sizes[VM_POOL_SIZE_CLASSES] = {
    32, 64, 128, 256, 512, 1024, 2048, 4096
};

/* Memory pressure callback */
static vm_pool_pressure_callback_t pressure_callback = NULL;

/* Global statistics */
static simple_lock_data_t global_stats_lock;
static unsigned long global_allocations = 0;
static unsigned long global_frees = 0;
static vm_size_t global_bytes_allocated = 0;
static vm_size_t global_peak_bytes = 0;

/*
 * Get size class index for a given size
 */
static unsigned int
size_class_index(vm_size_t size)
{
    unsigned int i;

    for (i = 0; i < VM_POOL_SIZE_CLASSES; i++) {
        if (size <= size_class_sizes[i])
            return i;
    }
    return VM_POOL_SIZE_CLASSES; /* Too large for pooling */
}

/*
 * Initialize the memory pool subsystem
 */
void
vm_memory_pool_init(void)
{
    unsigned int i;

    simple_lock_init(&global_stats_lock);

    /* Initialize kernel memory pool */
    kernel_memory_pool->name = "kernel_pool";
    kernel_memory_pool->track_allocations = FALSE;

    for (i = 0; i < VM_POOL_SIZE_CLASSES; i++) {
        kernel_memory_pool->size_classes[i].size = size_class_sizes[i];
        queue_init(&kernel_memory_pool->size_classes[i].free_list);
        kernel_memory_pool->size_classes[i].free_count = 0;
        kernel_memory_pool->size_classes[i].max_free = 32;
        simple_lock_init(&kernel_memory_pool->size_classes[i].lock);
    }

    memset(&kernel_memory_pool->stats, 0, sizeof(struct vm_pool_stats));
    queue_init(&kernel_memory_pool->active_allocations);
    simple_lock_init(&kernel_memory_pool->lock);
}

/*
 * Create a new memory pool
 */
vm_memory_pool_t
vm_memory_pool_create(const char *name, boolean_t track)
{
    vm_memory_pool_t pool;
    unsigned int i;

    pool = (vm_memory_pool_t)kalloc(sizeof(struct vm_memory_pool));
    if (pool == NULL)
        return NULL;

    pool->name = name;
    pool->track_allocations = track;

    for (i = 0; i < VM_POOL_SIZE_CLASSES; i++) {
        pool->size_classes[i].size = size_class_sizes[i];
        queue_init(&pool->size_classes[i].free_list);
        pool->size_classes[i].free_count = 0;
        pool->size_classes[i].max_free = 32;
        simple_lock_init(&pool->size_classes[i].lock);
    }

    memset(&pool->stats, 0, sizeof(struct vm_pool_stats));
    queue_init(&pool->active_allocations);
    simple_lock_init(&pool->lock);

    return pool;
}

/*
 * Destroy a memory pool
 */
void
vm_memory_pool_destroy(vm_memory_pool_t pool)
{
    unsigned int i;
    vm_pool_header_t header;

    assert(pool != NULL);
    assert(pool != kernel_memory_pool); /* Don't destroy kernel pool */

    simple_lock(&pool->lock);

    /* Free all entries in size classes */
    for (i = 0; i < VM_POOL_SIZE_CLASSES; i++) {
        simple_lock(&pool->size_classes[i].lock);
        while (!queue_empty(&pool->size_classes[i].free_list)) {
            queue_entry_t entry;
            entry = queue_first(&pool->size_classes[i].free_list);
            queue_remove(&pool->size_classes[i].free_list, entry,
                        queue_entry_t, next);
            kfree((vm_offset_t)entry,
                  size_class_sizes[i] + VM_POOL_HEADER_SIZE);
        }
        simple_unlock(&pool->size_classes[i].lock);
    }

    /* Free tracked allocations (leak detection) */
    if (pool->track_allocations && !queue_empty(&pool->active_allocations)) {
        printf("vm_memory_pool: %s has leaked allocations!\n", pool->name);
        queue_iterate(&pool->active_allocations, header,
                     vm_pool_header_t, track_link) {
            printf("  Leaked %lu bytes from %s:%u\n",
                   (unsigned long)header->size,
                   header->caller ? header->caller : "unknown",
                   header->line);
        }
    }

    simple_unlock(&pool->lock);
    kfree((vm_offset_t)pool, sizeof(struct vm_memory_pool));
}

/*
 * Allocate memory from pool
 */
void *
vm_pool_alloc(vm_memory_pool_t pool, vm_size_t size, unsigned int flags)
{
    return vm_pool_alloc_tracked(pool, size, flags, NULL, 0);
}

/*
 * Allocate memory with tracking
 */
void *
vm_pool_alloc_tracked(vm_memory_pool_t pool, vm_size_t size,
                      unsigned int flags, const char *caller,
                      unsigned int line)
{
    vm_pool_header_t header;
    vm_size_t total_size;
    unsigned int class_idx;
    void *result = NULL;

    assert(pool != NULL);
    assert(size > 0);

    total_size = size + VM_POOL_HEADER_SIZE;
    class_idx = size_class_index(total_size);

    /* Try to get from size class free list */
    if (class_idx < VM_POOL_SIZE_CLASSES) {
        struct vm_pool_size_class *sc = &pool->size_classes[class_idx];

        simple_lock(&sc->lock);
        if (!queue_empty(&sc->free_list)) {
            queue_entry_t entry;
            entry = queue_first(&sc->free_list);
            queue_remove(&sc->free_list, entry, queue_entry_t, next);
            sc->free_count--;
            simple_unlock(&sc->lock);

            header = (vm_pool_header_t)entry;
            pool->stats.pool_hits++;
        } else {
            simple_unlock(&sc->lock);

            /* Allocate new block */
            header = (vm_pool_header_t)kalloc(sc->size + VM_POOL_HEADER_SIZE);
            if (header == NULL) {
                if (flags & VM_POOL_NOWAIT) {
                    pool->stats.failed_allocations++;
                    return NULL;
                }
                /* TODO: Wait for memory or trigger pressure callback */
                pool->stats.failed_allocations++;
                return NULL;
            }
            pool->stats.pool_misses++;
        }
        total_size = sc->size;
    } else {
        /* Large allocation - use kalloc directly */
        header = (vm_pool_header_t)kalloc(total_size);
        if (header == NULL) {
            pool->stats.failed_allocations++;
            return NULL;
        }
    }

    /* Initialize header */
    header->magic = VM_POOL_MAGIC;
    header->size = size;
    header->flags = flags;
    header->state = VM_POOL_STATE_ALLOCATED;
    header->caller = caller;
    header->line = line;

    result = (void *)((char *)header + VM_POOL_HEADER_SIZE);

    /* Zero fill if requested */
    if (flags & VM_POOL_ZERO) {
        memset(result, 0, size);
    }

    /* Track allocation if enabled */
    if (pool->track_allocations && !(flags & VM_POOL_NOTRACK)) {
        simple_lock(&pool->lock);
        queue_enter(&pool->active_allocations, header,
                   vm_pool_header_t, track_link);
        simple_unlock(&pool->lock);
    }

    /* Update statistics */
    simple_lock(&pool->lock);
    pool->stats.total_allocations++;
    pool->stats.current_allocations++;
    pool->stats.current_bytes += size;
    if (pool->stats.current_bytes > pool->stats.peak_bytes)
        pool->stats.peak_bytes = pool->stats.current_bytes;
    simple_unlock(&pool->lock);

    /* Update global stats */
    simple_lock(&global_stats_lock);
    global_allocations++;
    global_bytes_allocated += size;
    if (global_bytes_allocated > global_peak_bytes)
        global_peak_bytes = global_bytes_allocated;
    simple_unlock(&global_stats_lock);

    return result;
}

/*
 * Free memory to pool
 */
void
vm_pool_free(vm_memory_pool_t pool, void *addr)
{
    vm_pool_header_t header;
    unsigned int class_idx;

    if (addr == NULL)
        return;

    assert(pool != NULL);

    /* Get header */
    header = (vm_pool_header_t)((char *)addr - VM_POOL_HEADER_SIZE);

    /* Validate magic */
    if (header->magic != VM_POOL_MAGIC) {
        printf("vm_pool_free: Invalid magic number! Corruption detected.\n");
        return;
    }

    assert(header->state == VM_POOL_STATE_ALLOCATED);

    /* Remove from tracking if enabled */
    if (pool->track_allocations) {
        simple_lock(&pool->lock);
        queue_remove(&pool->active_allocations, header,
                    vm_pool_header_t, track_link);
        simple_unlock(&pool->lock);
    }

    /* Update statistics */
    simple_lock(&pool->lock);
    pool->stats.total_frees++;
    pool->stats.current_allocations--;
    pool->stats.current_bytes -= header->size;
    simple_unlock(&pool->lock);

    /* Update global stats */
    simple_lock(&global_stats_lock);
    global_frees++;
    global_bytes_allocated -= header->size;
    simple_unlock(&global_stats_lock);

    /* Return to size class or free */
    class_idx = size_class_index(header->size + VM_POOL_HEADER_SIZE);

    if (class_idx < VM_POOL_SIZE_CLASSES) {
        struct vm_pool_size_class *sc = &pool->size_classes[class_idx];

        simple_lock(&sc->lock);
        if (sc->free_count < sc->max_free) {
            header->state = VM_POOL_STATE_FREE;
            queue_enter(&sc->free_list, header,
                       vm_pool_header_t, track_link);
            sc->free_count++;
            simple_unlock(&sc->lock);
        } else {
            simple_unlock(&sc->lock);
            kfree((vm_offset_t)header, sc->size + VM_POOL_HEADER_SIZE);
        }
    } else {
        kfree((vm_offset_t)header, header->size + VM_POOL_HEADER_SIZE);
    }
}

/*
 * Trim size class free lists
 */
void
vm_pool_trim_size_class(vm_memory_pool_t pool, unsigned int class_index)
{
    struct vm_pool_size_class *sc;
    unsigned int to_free;

    assert(pool != NULL);
    assert(class_index < VM_POOL_SIZE_CLASSES);

    sc = &pool->size_classes[class_index];

    simple_lock(&sc->lock);
    to_free = sc->free_count / 2; /* Free half of cached entries */

    while (to_free > 0 && !queue_empty(&sc->free_list)) {
        queue_entry_t entry;
        entry = queue_first(&sc->free_list);
        queue_remove(&sc->free_list, entry, queue_entry_t, next);
        kfree((vm_offset_t)entry, sc->size + VM_POOL_HEADER_SIZE);
        sc->free_count--;
        to_free--;
    }
    simple_unlock(&sc->lock);
}

/*
 * Get pool statistics
 */
void
vm_pool_get_stats(vm_memory_pool_t pool, vm_pool_stats_t stats)
{
    assert(pool != NULL);
    assert(stats != NULL);

    simple_lock(&pool->lock);
    memcpy(stats, &pool->stats, sizeof(struct vm_pool_stats));
    simple_unlock(&pool->lock);
}

/*
 * Print pool statistics
 */
void
vm_pool_print_stats(vm_memory_pool_t pool)
{
    struct vm_pool_stats stats;

    assert(pool != NULL);

    vm_pool_get_stats(pool, &stats);

    printf("Memory Pool: %s\n", pool->name);
    printf("  Total allocations: %lu\n", stats.total_allocations);
    printf("  Total frees: %lu\n", stats.total_frees);
    printf("  Current allocations: %lu\n", stats.current_allocations);
    printf("  Current bytes: %lu\n", (unsigned long)stats.current_bytes);
    printf("  Peak bytes: %lu\n", (unsigned long)stats.peak_bytes);
    printf("  Pool hits: %lu\n", stats.pool_hits);
    printf("  Pool misses: %lu\n", stats.pool_misses);
    printf("  Failed allocations: %lu\n", stats.failed_allocations);
}

/*
 * Print active allocations (for leak detection)
 */
void
vm_pool_print_allocations(vm_memory_pool_t pool)
{
    vm_pool_header_t header;

    assert(pool != NULL);

    if (!pool->track_allocations) {
        printf("Allocation tracking not enabled for pool: %s\n", pool->name);
        return;
    }

    printf("Active allocations for pool: %s\n", pool->name);

    simple_lock(&pool->lock);
    queue_iterate(&pool->active_allocations, header,
                 vm_pool_header_t, track_link) {
        printf("  %lu bytes at %p from %s:%u\n",
               (unsigned long)header->size,
               (void *)((char *)header + VM_POOL_HEADER_SIZE),
               header->caller ? header->caller : "unknown",
               header->line);
    }
    simple_unlock(&pool->lock);
}

/*
 * Register memory pressure callback
 */
void
vm_pool_register_pressure_callback(vm_pool_pressure_callback_t callback)
{
    pressure_callback = callback;
}

/*
 * Handle memory pressure
 */
void
vm_pool_memory_pressure(vm_size_t target_free)
{
    unsigned int i;

    /* Trim all size classes in kernel pool */
    for (i = 0; i < VM_POOL_SIZE_CLASSES; i++) {
        vm_pool_trim_size_class(kernel_memory_pool, i);
    }

    /* Invoke pressure callback if registered */
    if (pressure_callback != NULL) {
        pressure_callback(target_free);
    }
}

/*
 * Get overall memory usage
 */
void
vm_get_memory_usage(vm_memory_usage_t usage)
{
    assert(usage != NULL);

    memset(usage, 0, sizeof(struct vm_memory_usage));

    /* Get page-level statistics */
    /* These would typically come from vm_page.c counters */
    usage->total_physical = vm_page_mem_size;
    usage->free_physical = vm_page_free_count * PAGE_SIZE;
    usage->used_physical = usage->total_physical - usage->free_physical;

    /* Pool memory from global stats */
    simple_lock(&global_stats_lock);
    usage->pool_memory = global_bytes_allocated;
    simple_unlock(&global_stats_lock);
}

/*
 * Print memory usage report
 */
void
vm_print_memory_usage(void)
{
    struct vm_memory_usage usage;

    vm_get_memory_usage(&usage);

    printf("=== Memory Usage Report ===\n");
    printf("Total physical:  %lu KB\n",
           (unsigned long)(usage.total_physical / 1024));
    printf("Used physical:   %lu KB\n",
           (unsigned long)(usage.used_physical / 1024));
    printf("Free physical:   %lu KB\n",
           (unsigned long)(usage.free_physical / 1024));
    printf("Pool memory:     %lu KB\n",
           (unsigned long)(usage.pool_memory / 1024));

    printf("\nGlobal Pool Statistics:\n");
    simple_lock(&global_stats_lock);
    printf("  Total allocations: %lu\n", global_allocations);
    printf("  Total frees: %lu\n", global_frees);
    printf("  Current bytes: %lu\n", (unsigned long)global_bytes_allocated);
    printf("  Peak bytes: %lu\n", (unsigned long)global_peak_bytes);
    simple_unlock(&global_stats_lock);
}

/*
 * Page clustering for sequential access optimization
 */
kern_return_t
vm_page_cluster_alloc(vm_object_t object, vm_offset_t offset,
                      unsigned int count, vm_page_t *pages)
{
    unsigned int i;
    kern_return_t result = KERN_SUCCESS;

    assert(object != VM_OBJECT_NULL);
    assert(pages != NULL);
    assert(count > 0);

    for (i = 0; i < count; i++) {
        pages[i] = vm_page_grab();
        if (pages[i] == VM_PAGE_NULL) {
            /* Free already allocated pages */
            while (i > 0) {
                i--;
                vm_page_release(pages[i]);
            }
            return KERN_RESOURCE_SHORTAGE;
        }
    }

    return result;
}

/*
 * Free clustered pages
 */
void
vm_page_cluster_free(vm_page_t *pages, unsigned int count)
{
    unsigned int i;

    assert(pages != NULL);

    for (i = 0; i < count; i++) {
        if (pages[i] != VM_PAGE_NULL) {
            vm_page_release(pages[i]);
        }
    }
}

/*
 * Check for large page support
 */
boolean_t
vm_large_page_supported(void)
{
    /* Large page support depends on platform */
#ifdef MACH_LARGE_PAGES
    return TRUE;
#else
    return FALSE;
#endif
}

/*
 * Get size class for a given size
 */
vm_size_t
vm_pool_size_class_for_size(vm_size_t size)
{
    unsigned int idx = size_class_index(size);

    if (idx < VM_POOL_SIZE_CLASSES)
        return size_class_sizes[idx];

    return size; /* No pooling */
}
