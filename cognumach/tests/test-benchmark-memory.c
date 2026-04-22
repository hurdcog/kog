/*
 *  Copyright (C) 2024 Free Software Foundation
 *
 * This program is free software ; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation ; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY ; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the program ; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <mach/message.h>
#include <mach/mach_types.h>
#include <mach/vm_param.h>

#include <syscalls.h>
#include <testlib.h>

#include <mach.user.h>
#include <mach_port.user.h>

/* Memory Management Performance Benchmark Test */

static vm_address_t test_memory = 0;
static vm_size_t test_size = 0;

/* Test function for memory allocation benchmark */
void memory_allocation_test(void *arg)
{
    vm_address_t addr;
    kern_return_t kr;
    vm_size_t size = vm_page_size * 4; /* 4 pages */
    
    kr = vm_allocate(mach_task_self(), &addr, size, TRUE);
    if (kr == KERN_SUCCESS) {
        vm_deallocate(mach_task_self(), addr, size);
    }
}

/* Test function for memory protection benchmark */
void memory_protection_test(void *arg)
{
    kern_return_t kr;
    
    if (test_memory && test_size) {
        /* Toggle protection */
        kr = vm_protect(mach_task_self(), test_memory, test_size, 
                       FALSE, VM_PROT_READ);
        if (kr == KERN_SUCCESS) {
            kr = vm_protect(mach_task_self(), test_memory, test_size, 
                           FALSE, VM_PROT_READ | VM_PROT_WRITE);
        }
    }
}

/* Test function for memory read benchmark */
void memory_read_test(void *arg)
{
    if (test_memory && test_size) {
        volatile char *ptr = (volatile char *)test_memory;
        volatile char sum = 0;
        
        /* Read through allocated memory */
        for (vm_size_t i = 0; i < test_size; i += vm_page_size) {
            sum += ptr[i];
        }
    }
}

/* Test function for memory write benchmark */
void memory_write_test(void *arg)
{
    if (test_memory && test_size) {
        volatile char *ptr = (volatile char *)test_memory;
        
        /* Write through allocated memory */
        for (vm_size_t i = 0; i < test_size; i += vm_page_size) {
            ptr[i] = (char)(i & 0xFF);
        }
    }
}

void benchmark_memory_allocation(void)
{
    benchmark_t bench;
    
    printf("=== Memory Allocation Benchmark ===\n");
    
    benchmark_start(&bench, "Memory Allocation/Deallocation");
    benchmark_iterations(&bench, 500, memory_allocation_test, NULL);
    benchmark_end(&bench);
    benchmark_report(&bench, "alloc/dealloc ops/sec");
    
    printf("Memory allocation benchmark completed\n");
}

void benchmark_memory_protection(void)
{
    benchmark_t bench;
    kern_return_t kr;
    
    printf("=== Memory Protection Benchmark ===\n");
    
    /* Allocate test memory */
    test_size = vm_page_size * 16; /* 16 pages */
    kr = vm_allocate(mach_task_self(), &test_memory, test_size, TRUE);
    ASSERT_RET(kr, "failed to allocate test memory");
    
    benchmark_start(&bench, "Memory Protection Changes");
    benchmark_iterations(&bench, 1000, memory_protection_test, NULL);
    benchmark_end(&bench);
    benchmark_report(&bench, "protection ops/sec");
    
    /* Clean up */
    vm_deallocate(mach_task_self(), test_memory, test_size);
    test_memory = 0;
    test_size = 0;
    
    printf("Memory protection benchmark completed\n");
}

void benchmark_memory_access(void)
{
    benchmark_t bench;
    kern_return_t kr;
    
    printf("=== Memory Access Benchmark ===\n");
    
    /* Allocate test memory */
    test_size = vm_page_size * 32; /* 32 pages */
    kr = vm_allocate(mach_task_self(), &test_memory, test_size, TRUE);
    ASSERT_RET(kr, "failed to allocate test memory");
    
    /* Benchmark memory reads */
    benchmark_start(&bench, "Memory Read Operations");
    benchmark_iterations(&bench, 1000, memory_read_test, NULL);
    benchmark_end(&bench);
    benchmark_report(&bench, "read ops/sec");
    
    /* Benchmark memory writes */
    benchmark_start(&bench, "Memory Write Operations");
    benchmark_iterations(&bench, 1000, memory_write_test, NULL);
    benchmark_end(&bench);
    benchmark_report(&bench, "write ops/sec");
    
    /* Clean up */
    vm_deallocate(mach_task_self(), test_memory, test_size);
    test_memory = 0;
    test_size = 0;
    
    printf("Memory access benchmark completed\n");
}

void benchmark_vm_statistics(void)
{
    benchmark_t bench;
    vm_statistics_data_t stats;
    kern_return_t kr;
    
    printf("=== VM Statistics Benchmark ===\n");
    
    benchmark_start(&bench, "VM Statistics Retrieval");
    
    /* Benchmark vm_statistics calls */
    for (uint64_t i = 0; i < 1000; i++) {
        kr = vm_statistics(mach_task_self(), &stats);
        if (kr != KERN_SUCCESS) break;
    }
    bench.iterations = 1000;
    
    benchmark_end(&bench);
    benchmark_report(&bench, "vm_statistics calls/sec");
    
    printf("VM statistics benchmark completed\n");
}

int main(int argc, char *argv[], int envc, char *envp[])
{
    printf("Starting Memory Management Performance Benchmarks\n");
    printf("Page size: %lu bytes\n", vm_page_size);
    
    benchmark_memory_allocation();
    benchmark_memory_protection();
    benchmark_memory_access();
    benchmark_vm_statistics();
    
    printf("All memory management benchmarks completed successfully\n");
    return 0;
}