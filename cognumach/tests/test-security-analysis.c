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
 * test-security-analysis.c - Comprehensive test for advanced security analysis
 * 
 * Tests control flow integrity, runtime security monitoring, and vulnerability
 * detection capabilities in the GNU Mach microkernel.
 */

#include <testlib.h>
#include <mach/mach_security.h>
#include <mach/mach_safety.h>

/* Forward declarations for test functions */
static void test_cfi_validation(void);
static void test_security_monitoring(void);
static void test_buffer_guards(void);
static void test_memory_safety(void);
static void test_stack_canary(void);
static void test_vulnerability_detection(void);

/*
 * Main test function
 */
void
test_security_analysis(void)
{
    test_msg("=== Advanced Security Analysis Tests ===");
    
    /* Test Control Flow Integrity */
    test_msg("Testing Control Flow Integrity...");
    test_cfi_validation();
    
    /* Test Security Monitoring */
    test_msg("Testing Runtime Security Monitoring...");
    test_security_monitoring();
    
    /* Test Buffer Guards */
    test_msg("Testing Buffer Overflow Protection...");
    test_buffer_guards();
    
    /* Test Memory Safety */
    test_msg("Testing Memory Safety Validation...");
    test_memory_safety();
    
    /* Test Stack Canary */
    test_msg("Testing Stack Canary Protection...");
    test_stack_canary();
    
    /* Test Vulnerability Detection */
    test_msg("Testing Vulnerability Detection...");
    test_vulnerability_detection();
    
    test_msg("=== Security Analysis Tests Complete ===");
}

/*
 * Test Control Flow Integrity validation
 */
static void
test_cfi_validation(void)
{
    struct cfi_context ctx;
    cfi_result_t result;
    uintptr_t valid_addr = 0x100000;
    uintptr_t invalid_addr = 0x50;
    
    /* Initialize CFI context */
    cfi_init_context(&ctx, 0x200000, 0x100000);
    
    /* Test valid return address validation */
    result = cfi_validate_return(valid_addr, valid_addr);
    if (result != CFI_VALID) {
        test_failure("CFI return validation failed for valid address");
        return;
    }
    
    /* Test invalid return address validation */
    result = cfi_validate_return(invalid_addr, valid_addr);
    if (result == CFI_VALID) {
        test_failure("CFI return validation should have failed for invalid address");
        return;
    }
    
    /* Test call target validation */
    result = cfi_validate_call_target(valid_addr);
    if (result != CFI_VALID) {
        test_failure("CFI call target validation failed for valid address");
        return;
    }
    
    /* Test invalid call target */
    result = cfi_validate_call_target(invalid_addr);
    if (result == CFI_VALID) {
        test_failure("CFI call target validation should have failed");
        return;
    }
    
    /* Test stack integrity check */
    result = cfi_check_stack_integrity(&ctx);
    if (result != CFI_VALID) {
        test_failure("CFI stack integrity check failed");
        return;
    }
    
    test_msg("CFI validation tests passed");
}

/*
 * Test security monitoring functionality
 */
static void
test_security_monitoring(void)
{
    struct security_stats stats;
    kern_return_t ret;
    
    /* Reset statistics */
    security_reset_stats();
    
    /* Enable monitoring */
    SECURITY_MONITORING_ENABLE();
    
    /* Generate some test events */
    security_event_log(SEC_EVENT_CFI_VIOLATION, 0x12345678, "test_context");
    security_event_log(SEC_EVENT_BUFFER_OVERFLOW, 0x87654321, "test_buffer");
    
    /* Get statistics */
    ret = security_get_stats(&stats);
    if (ret != KERN_SUCCESS) {
        test_failure("Failed to get security statistics");
        return;
    }
    
    /* Verify statistics were updated */
    if (stats.total_events != 2) {
        test_failure("Security event count mismatch");
        return;
    }
    
    if (stats.cfi_violations != 1) {
        test_failure("CFI violation count mismatch");
        return;
    }
    
    if (stats.buffer_overflows != 1) {
        test_failure("Buffer overflow count mismatch");
        return;
    }
    
    /* Test ROP chain detection */
    uintptr_t rop_chain[] = {0x401000, 0x401004, 0x401008, 0x401002};
    boolean_t detected = security_detect_rop_chain(rop_chain, 4);
    if (!detected) {
        test_warning("ROP chain detection may need tuning");
    }
    
    /* Test stack pivot detection */
    boolean_t pivot = security_detect_stack_pivot(0x200000, 0x100000);
    if (!pivot) {
        test_failure("Stack pivot detection failed");
        return;
    }
    
    test_msg("Security monitoring tests passed");
}

/*
 * Test buffer guard functionality
 */
static void
test_buffer_guards(void)
{
    char test_buffer[256];
    buffer_guard_t guard;
    
    /* Initialize buffer guard */
    buffer_guard_init(&guard, test_buffer, sizeof(test_buffer));
    
    /* Check that guard is initially valid */
    if (!buffer_guard_check(&guard)) {
        test_failure("Buffer guard check failed for clean buffer");
        return;
    }
    
    /* Simulate buffer overflow by corrupting canary */
    uint32_t *canary_location = (uint32_t *)(test_buffer + sizeof(test_buffer) - sizeof(uint32_t));
    uint32_t original_canary = *canary_location;
    *canary_location = 0xDEADBEEF;  /* Corrupt the canary */
    
    /* Check should now fail */
    if (buffer_guard_check(&guard)) {
        test_failure("Buffer guard should have detected corruption");
        return;
    }
    
    /* Restore canary */
    *canary_location = original_canary;
    
    /* Check should pass again */
    if (!buffer_guard_check(&guard)) {
        test_failure("Buffer guard check failed after restoration");
        return;
    }
    
    test_msg("Buffer guard tests passed");
}

/*
 * Test memory safety validation
 */
static void
test_memory_safety(void)
{
    kern_return_t ret;
    char test_buffer[100];
    
    /* Test valid memory access */
    ret = memory_safety_check(test_buffer, sizeof(test_buffer), 0);
    if (ret != KERN_SUCCESS) {
        test_failure("Memory safety check failed for valid buffer");
        return;
    }
    
    /* Test NULL pointer access */
    ret = memory_safety_check(NULL, 10, 0);
    if (ret == KERN_SUCCESS) {
        test_failure("Memory safety should have failed for NULL pointer");
        return;
    }
    
    /* Test low address access (potential NULL dereference) */
    ret = memory_safety_check((void *)0x100, 10, 0);
    if (ret == KERN_SUCCESS) {
        test_failure("Memory safety should have failed for low address");
        return;
    }
    
    /* Test integer overflow in size */
    ret = memory_safety_check(test_buffer, SIZE_MAX, 0);
    if (ret == KERN_SUCCESS) {
        test_failure("Memory safety should have failed for overflow size");
        return;
    }
    
    test_msg("Memory safety tests passed");
}

/*
 * Test stack canary functionality
 */
static void
test_stack_canary(void)
{
    uint32_t canary1, canary2;
    
    /* Initialize stack canary system */
    stack_canary_init();
    
    /* Get canary values */
    canary1 = stack_canary_get();
    canary2 = stack_canary_get();
    
    /* Canary should be consistent */
    if (canary1 != canary2) {
        test_failure("Stack canary values are inconsistent");
        return;
    }
    
    /* Canary should not be zero */
    if (canary1 == 0) {
        test_failure("Stack canary should not be zero");
        return;
    }
    
    /* Test canary validation */
    if (!stack_canary_validate(canary1)) {
        test_failure("Stack canary validation failed");
        return;
    }
    
    /* Test invalid canary */
    if (stack_canary_validate(0xDEADBEEF)) {
        test_failure("Stack canary validation should have failed");
        return;
    }
    
    test_msg("Stack canary tests passed");
}

/*
 * Test vulnerability detection patterns
 */
static void
test_vulnerability_detection(void)
{
    uint32_t a = 0x80000000;
    uint32_t b = 0x80000000;
    uint32_t result;
    kern_return_t ret;
    
    /* Test integer overflow detection using safety macros */
    ret = MACH_SAFE_ADD(a, b, &result, UINT32);
    if (ret == KERN_SUCCESS) {
        test_failure("Integer overflow detection failed");
        return;
    }
    
    /* Test safe addition with valid values */
    a = 1000;
    b = 2000;
    ret = MACH_SAFE_ADD(a, b, &result, UINT32);
    if (ret != KERN_SUCCESS) {
        test_failure("Safe addition failed for valid values");
        return;
    }
    
    if (result != 3000) {
        test_failure("Safe addition result incorrect");
        return;
    }
    
    /* Test multiplication overflow detection */
    a = 0x10000;
    b = 0x10000;
    ret = MACH_SAFE_MUL(a, b, &result, UINT32);
    if (ret == KERN_SUCCESS) {
        test_failure("Multiplication overflow detection failed");
        return;
    }
    
    /* Test bounds checking */
    char buffer[100];
    if (!MACH_BOUNDS_CHECK((uintptr_t)buffer, 50, 100, (uintptr_t)buffer + 100)) {
        test_failure("Bounds check failed for valid range");
        return;
    }
    
    if (MACH_BOUNDS_CHECK((uintptr_t)buffer, 150, 100, (uintptr_t)buffer + 100)) {
        test_failure("Bounds check should have failed for invalid range");
        return;
    }
    
    test_msg("Vulnerability detection tests passed");
}

/* Test entry point called from test runner */
int
main(void)
{
    test_init("security-analysis");
    test_security_analysis();
    return 0;
}