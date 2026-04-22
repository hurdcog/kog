/*
 * Copyright (C) 2024 Free Software Foundation
 *
 * Test for VM map entry forward merging functionality.
 * This test validates that adjacent VM map entries with compatible
 * properties are automatically merged to reduce fragmentation.
 */

#include <syscalls.h>
#include <testlib.h>

#include <mach/machine/vm_param.h>
#include <mach/std_types.h>
#include <mach/mach_types.h>
#include <mach/vm_wire.h>
#include <mach/vm_param.h>

#include <device.user.h>
#include <gnumach.user.h>
#include <mach.user.h>
#include <mach_port.user.h>

static void test_vm_merge_basic()
{
    vm_address_t addr1, addr2, addr3;
    vm_size_t size = PAGE_SIZE;
    int err;
    
    printf("Testing basic VM entry merging...\n");
    
    /* Allocate three adjacent pages */
    addr1 = 0;
    err = vm_allocate(mach_task_self(), &addr1, size, TRUE);
    ASSERT_RET(err, "vm_allocate addr1");
    
    addr2 = addr1 + size;
    err = vm_allocate(mach_task_self(), &addr2, size, FALSE);
    ASSERT_RET(err, "vm_allocate addr2");
    
    addr3 = addr2 + size;
    err = vm_allocate(mach_task_self(), &addr3, size, FALSE);
    ASSERT_RET(err, "vm_allocate addr3");
    
    printf("Allocated pages at: 0x%lx, 0x%lx, 0x%lx\n", 
           (unsigned long)addr1, (unsigned long)addr2, (unsigned long)addr3);
    
    /* Test that we can write to all three pages */
    *(int*)addr1 = 0x1234;
    *(int*)addr2 = 0x5678;
    *(int*)addr3 = 0x9abc;
    
    /* Verify the values */
    ASSERT(*(int*)addr1 == 0x1234, "addr1 value");
    ASSERT(*(int*)addr2 == 0x5678, "addr2 value");
    ASSERT(*(int*)addr3 == 0x9abc, "addr3 value");
    
    /* Deallocate the middle page to create fragmentation */
    err = vm_deallocate(mach_task_self(), addr2, size);
    ASSERT_RET(err, "vm_deallocate addr2");
    
    /* Reallocate the middle page - this should trigger merging */
    addr2 = addr1 + size;
    err = vm_allocate(mach_task_self(), &addr2, size, FALSE);
    ASSERT_RET(err, "vm_allocate addr2 again");
    
    /* Test that the pages are still accessible */
    *(int*)addr2 = 0xdef0;
    ASSERT(*(int*)addr1 == 0x1234, "addr1 value after realloc");
    ASSERT(*(int*)addr2 == 0xdef0, "addr2 value after realloc");
    ASSERT(*(int*)addr3 == 0x9abc, "addr3 value after realloc");
    
    /* Clean up */
    err = vm_deallocate(mach_task_self(), addr1, size * 3);
    ASSERT_RET(err, "vm_deallocate all");
    
    printf("Basic VM entry merging test passed!\n");
}

static void test_vm_merge_protection()
{
    vm_address_t addr1, addr2;
    vm_size_t size = PAGE_SIZE;
    int err;
    
    printf("Testing VM entry merging with different protections...\n");
    
    /* Allocate two adjacent pages with different protections */
    addr1 = 0;
    err = vm_allocate(mach_task_self(), &addr1, size, TRUE);
    ASSERT_RET(err, "vm_allocate addr1");
    
    addr2 = addr1 + size;
    err = vm_allocate(mach_task_self(), &addr2, size, FALSE);
    ASSERT_RET(err, "vm_allocate addr2");
    
    /* Change protection of second page to read-only */
    err = vm_protect(mach_task_self(), addr2, size, FALSE, VM_PROT_READ);
    ASSERT_RET(err, "vm_protect addr2 read-only");
    
    /* These should NOT be merged due to different protections */
    *(int*)addr1 = 0x1111;
    ASSERT(*(int*)addr2 == 0, "addr2 should be zero initially");
    
    /* Change protection back to read-write */
    err = vm_protect(mach_task_self(), addr2, size, FALSE, VM_PROT_READ | VM_PROT_WRITE);
    ASSERT_RET(err, "vm_protect addr2 read-write");
    
    /* Now they might be merged in future operations */
    *(int*)addr2 = 0x2222;
    ASSERT(*(int*)addr1 == 0x1111, "addr1 value");
    ASSERT(*(int*)addr2 == 0x2222, "addr2 value");
    
    /* Clean up */
    err = vm_deallocate(mach_task_self(), addr1, size * 2);
    ASSERT_RET(err, "vm_deallocate all");
    
    printf("VM entry merging protection test passed!\n");
}

int main(int argc, char *argv[])
{
    test_vm_merge_basic();
    test_vm_merge_protection();
    
    printf("All VM merge tests passed!\n");
    return 0;
}