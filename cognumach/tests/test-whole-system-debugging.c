/*
 * Test program for whole system debugging functionality
 * Copyright (C) 2024 Free Software Foundation
 *
 * This program tests the system-wide debugging infrastructure
 * by exercising various debugging features and cross-component interactions.
 */

#include <mach/unified_debug.h>
#include <mach/system_debug.h>
#include <kern/debug.h>
#include <kern/printf.h>
#include <mach/mach_types.h>

/*
 * Test the basic system debugging functionality
 */
static void test_basic_system_debugging(void)
{
    printf("\n=== Testing Basic System Debugging ===\n");
    
    /* Test enabling and disabling */
    unified_debug_enable_all(TRUE);
    
    if (unified_debug_is_any_enabled()) {
        printf("✓ System debugging enabled successfully\n");
    } else {
        printf("✗ Failed to enable system debugging\n");
        return;
    }
    
    /* Test event reporting */
    SYSDEBUG_LOG(SYSDEBUG_SUBSYSTEM_KERNEL, SYSDEBUG_EVENT_TRACE, 
                "Test trace event");
    SYSDEBUG_LOG(SYSDEBUG_SUBSYSTEM_VM, SYSDEBUG_EVENT_WARNING,
                "Test warning event");
    SYSDEBUG_LOG(SYSDEBUG_SUBSYSTEM_IPC, SYSDEBUG_EVENT_ERROR,
                "Test error event");
    
    printf("✓ Event reporting test completed\n");
    
    /* Test subsystem level control */
    sysdebug_set_subsystem_level(SYSDEBUG_SUBSYSTEM_VM, SYSDEBUG_LEVEL_TRACE);
    sysdebug_set_subsystem_level(SYSDEBUG_SUBSYSTEM_IPC, SYSDEBUG_LEVEL_VERBOSE);
    
    printf("✓ Subsystem level control test completed\n");
}

/*
 * Test cross-component debugging
 */
static void test_cross_component_debugging(void)
{
    printf("\n=== Testing Cross-Component Debugging ===\n");
    
    /* Enable cross-component tracking */
    sysdebug_enable_cross_component_tracking(TRUE);
    
    if (sysdebug_is_cross_component_tracking_enabled()) {
        printf("✓ Cross-component tracking enabled\n");
    } else {
        printf("✗ Failed to enable cross-component tracking\n");
        return;
    }
    
    /* Simulate cross-component interactions */
    SYSDEBUG_TRACE_INTERACTION(SYSDEBUG_SUBSYSTEM_VM, SYSDEBUG_SUBSYSTEM_IPC,
                              SYSDEBUG_EVENT_INTERACTION, "vm_to_ipc_test");
    
    SYSDEBUG_TRACE_INTERACTION(SYSDEBUG_SUBSYSTEM_IPC, SYSDEBUG_SUBSYSTEM_SCHED,
                              SYSDEBUG_EVENT_INTERACTION, "ipc_to_sched_test");
    
    SYSDEBUG_TRACE_INTERACTION(SYSDEBUG_SUBSYSTEM_SCHED, SYSDEBUG_SUBSYSTEM_VM,
                              SYSDEBUG_EVENT_INTERACTION, "sched_to_vm_test");
    
    printf("✓ Cross-component interaction tracking test completed\n");
    
    /* Test interaction dump */
    sysdebug_dump_cross_component_interactions();
    
    printf("✓ Cross-component interaction dump test completed\n");
}

/*
 * Test unified debugging interface
 */
static void test_unified_debugging_interface(void)
{
    printf("\n=== Testing Unified Debugging Interface ===\n");
    
    struct unified_debug_state state;
    
    /* Get current debugging state */
    unified_debug_get_state(&state);
    
    printf("Unified Debug State:\n");
    printf("  - Initialized: %s\n", state.initialized ? "YES" : "NO");
    printf("  - System Debug: %s\n", state.system_debug_enabled ? "enabled" : "disabled");
    printf("  - Cross-Component: %s\n", state.cross_component_tracking ? "enabled" : "disabled");
    
    if (state.initialized && state.system_debug_enabled) {
        printf("✓ Unified debugging state query successful\n");
    } else {
        printf("✗ Unified debugging state query failed\n");
        return;
    }
    
    /* Test subsystem debugging integration */
    unified_debug_vm_operation("test_allocation", (void*)0x1000000, 4096);
    unified_debug_device_operation("test_device", "read");
    
    /* Test performance measurement */
    unified_debug_performance_start("test_measurement");
    /* Simulate some work */
    for (int i = 0; i < 1000; i++) {
        /* Dummy work */
    }
    unified_debug_performance_end("test_measurement");
    
    printf("✓ Unified debugging interface test completed\n");
}

/*
 * Test debugging macro integration
 */
static void test_debugging_macros(void)
{
    printf("\n=== Testing Debugging Macros ===\n");
    
    /* Test resource tracking macros */
    void *test_ptr = (void*)0x12345678;
    UNIFIED_DEBUG_RESOURCE_ALLOC(SYSDEBUG_SUBSYSTEM_VM, "memory", 1024, test_ptr);
    UNIFIED_DEBUG_RESOURCE_FREE(SYSDEBUG_SUBSYSTEM_VM, "memory", test_ptr);
    
    /* Test state change tracking */
    UNIFIED_DEBUG_STATE_CHANGE(SYSDEBUG_SUBSYSTEM_SCHED, "RUNNING", "BLOCKED");
    UNIFIED_DEBUG_STATE_CHANGE(SYSDEBUG_SUBSYSTEM_SCHED, "BLOCKED", "READY");
    
    /* Test warning and error reporting */
    UNIFIED_DEBUG_WARNING(SYSDEBUG_SUBSYSTEM_IPC, "Test warning message");
    UNIFIED_DEBUG_ERROR(SYSDEBUG_SUBSYSTEM_DEVICE, "Test error message");
    
    /* Test cross-subsystem call tracking */
    UNIFIED_DEBUG_CROSS_CALL(SYSDEBUG_SUBSYSTEM_VM, SYSDEBUG_SUBSYSTEM_IPC, "vm_to_ipc_call");
    
    printf("✓ Debugging macro integration test completed\n");
}

/*
 * Test debugging session and statistics
 */
static void test_debugging_statistics(void)
{
    printf("\n=== Testing Debugging Statistics ===\n");
    
    /* Start a debug session */
    unified_debug_session_start("test_session");
    
    /* Generate some events for statistics */
    for (int i = 0; i < 5; i++) {
        SYSDEBUG_LOG(SYSDEBUG_SUBSYSTEM_VM, SYSDEBUG_EVENT_TRACE, "Test event %d", i);
        SYSDEBUG_LOG(SYSDEBUG_SUBSYSTEM_IPC, SYSDEBUG_EVENT_WARNING, "Test warning %d", i);
    }
    
    /* Get and display statistics */
    unsigned long vm_stats[SYSDEBUG_EVENT_MAX];
    sysdebug_get_statistics(SYSDEBUG_SUBSYSTEM_VM, vm_stats);
    
    printf("VM Subsystem Statistics:\n");
    for (int i = 0; i < SYSDEBUG_EVENT_MAX; i++) {
        if (vm_stats[i] > 0) {
            printf("  Event %d: %lu occurrences\n", i, vm_stats[i]);
        }
    }
    
    /* End the debug session */
    unified_debug_session_end("test_session");
    
    printf("✓ Debugging statistics test completed\n");
}

/*
 * Main test function
 */
void test_whole_system_debugging(void)
{
    printf("\n==========================================\n");
    printf("GNU Mach Whole System Debugging Test\n");
    printf("==========================================\n");
    
    /* Initialize unified debugging if not already done */
    unified_debug_init();
    
    /* Run all tests */
    test_basic_system_debugging();
    test_cross_component_debugging();
    test_unified_debugging_interface();
    test_debugging_macros();
    test_debugging_statistics();
    
    /* Final comprehensive dump */
    printf("\n=== Final State Dump ===\n");
    unified_debug_dump_all_state();
    
    /* Reset statistics for clean state */
    sysdebug_reset_statistics();
    
    printf("\n==========================================\n");
    printf("Whole System Debugging Test Completed\n");
    printf("==========================================\n");
}

/*
 * Test kernel integration points
 */
void test_kernel_integration(void)
{
    printf("\n=== Testing Kernel Integration ===\n");
    
    /* Test thread debugging integration */
    thread_t dummy_thread = (thread_t)0x12345678;
    unified_debug_thread_create(dummy_thread);
    unified_debug_thread_destroy(dummy_thread);
    
    /* Test task debugging integration */
    task_t dummy_task = (task_t)0x87654321;
    unified_debug_task_create(dummy_task);
    unified_debug_task_destroy(dummy_task);
    
    /* Test VM debugging integration */
    unified_debug_vm_operation("page_fault", (void*)0x40000000, 4096);
    unified_debug_vm_fault(0x40000000, KERN_SUCCESS);
    
    /* Test device debugging integration */
    unified_debug_device_operation("console", "write");
    
    printf("✓ Kernel integration test completed\n");
}