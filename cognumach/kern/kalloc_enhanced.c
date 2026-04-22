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

#include <kern/kalloc_enhanced.h>
#include <kern/kalloc.h>
#include <kern/mem_track.h>
#include <kern/mem_optimize.h>
#include <kern/slab.h>
#include <kern/printf.h>
#include <mach/vm_param.h>
#include <mach/kern_return.h>

/*
 * Helper function to perform typed allocation with optimization.
 */
static vm_offset_t kalloc_typed(vm_size_t size, mem_type_t type)
{
    vm_offset_t addr;
    
    /* Use enhanced allocation failure prediction */
    if (mem_opt_predict_allocation_failure_enhanced(size)) {
        printf("Predicted allocation failure for size %u, running proactive management...\n", (unsigned)size);
        mem_opt_proactive_management();
    }
    
    addr = kalloc(size);
    
    if (addr != 0) {
        /* kalloc already tracked as GENERAL, update to specific type */
        mem_track_free(MEM_TYPE_GENERAL, size);
        mem_track_alloc(type, size);
    } else {
        /* Track failed allocation and try emergency optimization */
        mem_track_alloc_failed(type, size);
        
        /* Try emergency optimization and retry once */
        printf("Allocation failed for size %u, attempting emergency optimization\n", (unsigned)size);
        mem_opt_handle_memory_pressure();
        
        addr = kalloc(size);
        if (addr != 0) {
            mem_track_free(MEM_TYPE_GENERAL, size);
            mem_track_alloc(type, size);
            printf("Allocation succeeded after optimization\n");
        }
    }
    
    return addr;
}

/*
 * Helper function to perform typed free.
 */
static void kfree_typed(vm_offset_t data, vm_size_t size, mem_type_t type)
{
    if ((data == 0) || (size == 0))
        return;
    
    /* Track free for specific type */
    mem_track_free(type, size);
    
    /* Add back as general before calling kfree to maintain consistency */
    mem_track_alloc(MEM_TYPE_GENERAL, size);
    
    kfree(data, size);
}

/*
 * VM object allocations.
 */
vm_offset_t kalloc_vm(vm_size_t size)
{
    return kalloc_typed(size, MEM_TYPE_VM_OBJECTS);
}

void kfree_vm(vm_offset_t data, vm_size_t size)
{
    kfree_typed(data, size, MEM_TYPE_VM_OBJECTS);
}

/*
 * IPC allocations.
 */
vm_offset_t kalloc_ipc(vm_size_t size)
{
    return kalloc_typed(size, MEM_TYPE_IPC);
}

void kfree_ipc(vm_offset_t data, vm_size_t size)
{
    kfree_typed(data, size, MEM_TYPE_IPC);
}

/*
 * Thread allocations.
 */
vm_offset_t kalloc_thread(vm_size_t size)
{
    return kalloc_typed(size, MEM_TYPE_THREADS);
}

void kfree_thread(vm_offset_t data, vm_size_t size)
{
    kfree_typed(data, size, MEM_TYPE_THREADS);
}

/*
 * Task allocations.
 */
vm_offset_t kalloc_task(vm_size_t size)
{
    return kalloc_typed(size, MEM_TYPE_TASKS);
}

void kfree_task(vm_offset_t data, vm_size_t size)
{
    kfree_typed(data, size, MEM_TYPE_TASKS);
}

/*
 * Device driver allocations.
 */
vm_offset_t kalloc_device(vm_size_t size)
{
    return kalloc_typed(size, MEM_TYPE_DEVICE);
}

void kfree_device(vm_offset_t data, vm_size_t size)
{
    kfree_typed(data, size, MEM_TYPE_DEVICE);
}

/*
 * Network buffer allocations.
 */
vm_offset_t kalloc_network(vm_size_t size)
{
    return kalloc_typed(size, MEM_TYPE_NETWORK);
}

void kfree_network(vm_offset_t data, vm_size_t size)
{
    kfree_typed(data, size, MEM_TYPE_NETWORK);
}

/*
 * Optimize memory pools by triggering garbage collection and compaction.
 */
void kalloc_optimize_pools(void)
{
    printf("Optimizing memory pools...\n");
    
    /* Use memory optimizer for intelligent optimization */
    if (mem_opt_should_compact()) {
        mem_opt_compact_memory();
    } else {
        /* Light optimization */
        slab_collect();
        mem_opt_defragment_slabs();
    }
    
    /* Check for memory pressure and report if needed */
    if (mem_track_check_pressure()) {
        printf("Memory pressure detected during pool optimization\n");
        mem_track_report_usage();
        mem_opt_report_optimization();
    }
    
    printf("Memory pool optimization complete\n");
}

/*
 * Reclaim unused memory from pools with advanced optimization.
 */
void kalloc_reclaim_memory(void)
{
    printf("Reclaiming unused memory...\n");
    
    /* Use emergency reclamation for thorough cleanup */
    mem_opt_emergency_reclaim();
    
    mem_track_report_usage();
    mem_opt_report_optimization();
    printf("Memory reclamation complete\n");
}

/*
 * Check memory fragmentation status.
 */
boolean_t kalloc_check_fragmentation(void)
{
    /* Use the advanced fragmentation analysis from memory optimizer */
    return mem_opt_is_heavily_fragmented();
}

/*
 * Report memory usage with kalloc-specific information.
 */
void kalloc_report_usage(void)
{
    printf("\n=== Enhanced Kalloc Memory Report ===\n");
    
    /* Basic memory tracking report */
    mem_track_report_detailed();
    
    /* Slab allocator information */
    printf("\nSlab Allocator Statistics:\n");
    slab_info();
    
    printf("\n=== End Kalloc Memory Report ===\n");
}

/*
 * Report memory fragmentation status.
 */
void kalloc_report_fragmentation(void)
{
    boolean_t fragmented = kalloc_check_fragmentation();
    
    printf("\nMemory Fragmentation Analysis:\n");
    printf("  Fragmentation detected: %s\n", fragmented ? "YES" : "NO");
    
    if (fragmented) {
        printf("  Recommendation: Consider running kalloc_reclaim_memory()\n");
    }
    
    /* Get allocation distribution */
    struct mem_stats stats;
    if (mem_track_get_stats(MEM_TYPE_GENERAL, &stats) == KERN_SUCCESS) {
        uint32_t large_pct = 0;
        if (stats.alloc_count > 0) {
            large_pct = (stats.large_allocs * 100) / stats.alloc_count;
        }
        
        printf("  Large allocation ratio: %u%% (%u large / %llu total)\n",
               large_pct, stats.large_allocs, stats.alloc_count);
        
        if (stats.peak_bytes > 0) {
            uint32_t usage_pct = (uint32_t)((stats.current_bytes * 100) / stats.peak_bytes);
            printf("  Current vs peak usage: %u%% (%luk / %luk)\n",
                   usage_pct, 
                   (unsigned long)(stats.current_bytes >> 10),
                   (unsigned long)(stats.peak_bytes >> 10));
        }
    }
}