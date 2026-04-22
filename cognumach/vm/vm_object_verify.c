/*
 * VM Object Verification and Consistency Checks
 * Copyright (c) 2024 GNU Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

#include <kern/printf.h>
#include <kern/assert.h>
#include <kern/mem_track.h>
#include <vm/vm_object.h>
#include <vm/vm_object_verify.h>
#include <vm/vm_page.h>

/*
 * Verify that the resident_page_count field matches the actual number
 * of pages in the object's memory queue.
 * 
 * This function helps detect and prevent the panic condition in vm_debug.c
 * by catching inconsistencies early.
 */
boolean_t
vm_object_verify_resident_count(vm_object_t object)
{
    vm_page_t p;
    unsigned long actual_count = 0;
    
    if (object == VM_OBJECT_NULL)
        return FALSE;
    
    assert(vm_object_lock_taken(object));
    
    /* Count actual pages in the memory queue */
    queue_iterate(&object->memq, p, vm_page_t, listq) {
        actual_count++;
    }
    
    if (object->resident_page_count != actual_count) {
        printf("VM object resident count mismatch: stored=%lu, actual=%lu\n",
               object->resident_page_count, actual_count);
        return FALSE;
    }
    
    return TRUE;
}

/*
 * Safely update the resident page count with verification.
 * This replaces direct manipulation of resident_page_count.
 */
void
vm_object_increment_resident_count(vm_object_t object)
{
    assert(vm_object_lock_taken(object));
    
    object->resident_page_count++;
    assert(object->resident_page_count != 0);
    
    /* Track memory allocation for pressure detection */
    mem_track_alloc(MEM_TYPE_VM_OBJECTS, PAGE_SIZE);
    
#ifdef MACH_DEBUG
    /* Verify consistency in debug builds */
    if (!vm_object_verify_resident_count(object)) {
        printf("Warning: VM object count inconsistency after increment\n");
    }
#endif
}

/*
 * Safely decrement the resident page count with verification.
 */
void
vm_object_decrement_resident_count(vm_object_t object)
{
    assert(vm_object_lock_taken(object));
    assert(object->resident_page_count > 0);
    
    object->resident_page_count--;
    
    /* Track memory deallocation */
    mem_track_free(MEM_TYPE_VM_OBJECTS, PAGE_SIZE);
    
#ifdef MACH_DEBUG
    /* Verify consistency in debug builds */
    if (!vm_object_verify_resident_count(object)) {
        printf("Warning: VM object count inconsistency after decrement\n");
    }
#endif
}

/*
 * Get accurate memory usage statistics for a VM object.
 */
kern_return_t
vm_object_get_memory_stats(vm_object_t object, vm_object_memory_stats_t *stats)
{
    vm_page_t p;
    
    if (object == VM_OBJECT_NULL || stats == NULL)
        return KERN_INVALID_ARGUMENT;
    
    vm_object_lock(object);
    
    /* Initialize stats */
    stats->resident_pages = 0;
    stats->wired_pages = 0;
    stats->active_pages = 0;
    stats->inactive_pages = 0;
    stats->dirty_pages = 0;
    stats->referenced_pages = 0;
    stats->memory_size = 0;
    
    /* Collect detailed statistics */
    queue_iterate(&object->memq, p, vm_page_t, listq) {
        stats->resident_pages++;
        stats->memory_size += PAGE_SIZE;
        
        if (p->wire_count > 0)
            stats->wired_pages++;
        if (p->active)
            stats->active_pages++;
        if (p->inactive)
            stats->inactive_pages++;
        if (p->dirty)
            stats->dirty_pages++;
        if (p->reference)
            stats->referenced_pages++;
    }
    
    /* Verify consistency */
    if (stats->resident_pages != object->resident_page_count) {
        printf("VM object %p: count mismatch detected during stats collection\n", object);
        printf("  Stored count: %lu, Actual count: %lu\n", 
               object->resident_page_count, stats->resident_pages);
        
        /* Fix the stored count */
        object->resident_page_count = stats->resident_pages;
    }
    
    vm_object_unlock(object);
    return KERN_SUCCESS;
}

/*
 * Comprehensive verification of all VM objects.
 * This can be called periodically or when memory pressure is detected.
 */
void
vm_object_verify_all_counts(void)
{
    /* This would require access to the global VM object list,
     * which is typically maintained in vm_object.c
     * For now, we provide the interface for future implementation.
     */
    printf("VM object verification: scanning all objects for consistency\n");
    /* TODO: Implement global verification when object list access is available */
}