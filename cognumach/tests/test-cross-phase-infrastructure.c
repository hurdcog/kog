/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 */

/*
 * test-cross-phase-infrastructure.c - Test for cross-phase infrastructure improvements
 * 
 * Tests the robustness improvements implemented to address critical bug categories:
 * - Integer overflow protection
 * - Bounds checking 
 * - Resource validation
 * - VM map safety checks
 */

#include <testlib.h>
#include <mach/mach_safety.h>

/* Forward declarations for test functions */
static void test_integer_overflow_protection(void);
static void test_bounds_checking(void);
static void test_resource_validation(void);
static void test_vm_safety_checks(void);

/*
 * Main test function
 */
void
test_cross_phase_infrastructure(void)
{
    test_msg("=== Cross-Phase Infrastructure Tests ===");
    
    /* Test Integer Overflow Protection */
    test_msg("Testing Integer Overflow Protection...");
    test_integer_overflow_protection();
    
    /* Test Bounds Checking */
    test_msg("Testing Bounds Checking...");
    test_bounds_checking();
    
    /* Test Resource Validation */
    test_msg("Testing Resource Validation...");
    test_resource_validation();
    
    /* Test VM Safety Checks */
    test_msg("Testing VM Safety Checks...");
    test_vm_safety_checks();
    
    test_msg("=== Cross-Phase Infrastructure Tests Complete ===");
}

/*
 * Test integer overflow protection macros
 */
static void
test_integer_overflow_protection(void)
{
    uint32_t a, b, result;
    kern_return_t ret;
    
    /* Test safe addition with valid values */
    a = 1000;
    b = 2000;
    ret = MACH_SAFE_ADD(a, b, &result, UINT32);
    ASSERT(ret == KERN_SUCCESS, "Safe addition should succeed for valid values");
    ASSERT(result == 3000, "Safe addition result should be correct");
    
    /* Test safe addition overflow detection */
    a = 0xFFFFFFFF;
    b = 1;
    ret = MACH_SAFE_ADD(a, b, &result, UINT32);
    ASSERT(ret == KERN_INVALID_ARGUMENT, "Safe addition should detect overflow");
    
    /* Test safe multiplication with valid values */
    a = 10;
    b = 20;
    ret = MACH_SAFE_MUL(a, b, &result, UINT32);
    ASSERT(ret == KERN_SUCCESS, "Safe multiplication should succeed for valid values");
    ASSERT(result == 200, "Safe multiplication result should be correct");
    
    /* Test safe multiplication overflow detection */
    a = 0x10000;
    b = 0x10000;
    ret = MACH_SAFE_MUL(a, b, &result, UINT32);
    ASSERT(ret == KERN_INVALID_ARGUMENT, "Safe multiplication should detect overflow");
    
    test_msg("Integer overflow protection tests passed");
}

/*
 * Test bounds checking macros
 */
static void
test_bounds_checking(void)
{
    char buffer[100];
    uintptr_t base = (uintptr_t)buffer;
    uintptr_t limit = base + sizeof(buffer);
    
    /* Test valid bounds check */
    ASSERT(MACH_BOUNDS_CHECK(base, 50, sizeof(buffer), limit), 
           "Bounds check should pass for valid offset");
    
    /* Test invalid bounds check - offset too large */
    ASSERT(!MACH_BOUNDS_CHECK(base, 150, sizeof(buffer), limit), 
           "Bounds check should fail for offset beyond buffer");
    
    /* Test range check with valid range */
    ASSERT(MACH_RANGE_CHECK(base, 50, limit), 
           "Range check should pass for valid range");
    
    /* Test range check with invalid range */
    ASSERT(!MACH_RANGE_CHECK(base, 200, limit), 
           "Range check should fail for range beyond limit");
    
    test_msg("Bounds checking tests passed");
}

/*
 * Test resource validation macros
 */
static void
test_resource_validation(void)
{
    char buffer[100];
    char *valid_ptr = buffer;
    char *null_ptr = NULL;
    uintptr_t min_addr = (uintptr_t)buffer;
    uintptr_t max_addr = min_addr + sizeof(buffer);
    
    /* Test valid pointer validation */
    ASSERT(MACH_VALIDATE_PTR(valid_ptr, (void*)min_addr, (void*)max_addr), 
           "Pointer validation should pass for valid pointer");
    
    /* Test NULL pointer validation */
    ASSERT(!MACH_VALIDATE_PTR(null_ptr, (void*)min_addr, (void*)max_addr), 
           "Pointer validation should fail for NULL pointer");
    
    /* Test valid memory region */
    ASSERT(MACH_VALIDATE_REGION(buffer, sizeof(buffer)), 
           "Region validation should pass for valid region");
    
    /* Test invalid memory region (zero size) */
    ASSERT(!MACH_VALIDATE_REGION(buffer, 0), 
           "Region validation should fail for zero-size region");
    
    test_msg("Resource validation tests passed");
}

/*
 * Test VM safety check macros
 */
static void
test_vm_safety_checks(void)
{
    /* Mock VM entry structure for testing */
    struct {
        uintptr_t start;
        uintptr_t end;
    } valid_entry = { 0x1000, 0x2000 };
    
    struct {
        uintptr_t start;
        uintptr_t end;
    } invalid_entry = { 0x2000, 0x1000 };
    
    /* Test valid VM entry */
    ASSERT(MACH_VM_ENTRY_VALID(&valid_entry), 
           "VM entry validation should pass for valid entry");
    
    /* Test invalid VM entry (start > end) */
    ASSERT(!MACH_VM_ENTRY_VALID(&invalid_entry), 
           "VM entry validation should fail for invalid entry");
    
    /* Test VM alignment check */
    ASSERT(MACH_VM_ALIGNED((void*)0x1000, 0x1000), 
           "VM alignment check should pass for aligned address");
    
    ASSERT(!MACH_VM_ALIGNED((void*)0x1001, 0x1000), 
           "VM alignment check should fail for unaligned address");
    
    test_msg("VM safety check tests passed");
}

/*
 * Entry point
 */
int
main(int argc, char *argv[], int envc, char *envp[])
{
    test_cross_phase_infrastructure();
    return 0;
}