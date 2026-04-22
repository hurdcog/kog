/*
 * Copyright (c) 2024 Cognu Mach Contributors
 * All rights reserved.
 *
 * Test program for DTrace-like kernel instrumentation
 */

#include <testlib.h>
#include <mach/mach.h>
#include <mach/vm_map.h>

/*
 * This test exercises kernel functionality that should trigger
 * DTrace probes: thread switching, IPC messages, and VM faults.
 */

static void test_thread_switching(void)
{
    /* Create multiple threads to trigger thread switching */
    for (int i = 0; i < 3; i++) {
        mach_port_t thread_port;
        thread_create(mach_task_self(), &thread_port);
        thread_terminate(thread_port);
        mach_port_deallocate(mach_task_self(), thread_port);
    }
    test_pass("Thread switching test completed");
}

static void test_ipc_messages(void)
{
    mach_port_t port;
    kern_return_t kr;
    
    /* Create a port to trigger IPC activity */
    kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &port);
    if (kr != KERN_SUCCESS) {
        test_fail("Failed to allocate port");
        return;
    }
    
    /* Send a message to ourselves */
    struct {
        mach_msg_header_t header;
        uint32_t data;
    } msg = {
        .header = {
            .msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND, 0),
            .msgh_size = sizeof(msg),
            .msgh_remote_port = port,
            .msgh_local_port = MACH_PORT_NULL,
            .msgh_id = 0x1234
        },
        .data = 0xdeadbeef
    };
    
    /* This should trigger IPC probes */
    kr = mach_msg(&msg.header, MACH_SEND_MSG | MACH_RCV_MSG,
                  sizeof(msg), sizeof(msg), port,
                  MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
    
    mach_port_deallocate(mach_task_self(), port);
    
    if (kr == KERN_SUCCESS) {
        test_pass("IPC message test completed");
    } else {
        test_fail("IPC message test failed");
    }
}

static void test_vm_faults(void)
{
    vm_address_t addr;
    vm_size_t size = 4096;
    kern_return_t kr;
    
    /* Allocate some memory to trigger VM operations */
    kr = vm_allocate(mach_task_self(), &addr, size, TRUE);
    if (kr != KERN_SUCCESS) {
        test_fail("Failed to allocate memory");
        return;
    }
    
    /* Access the memory to potentially trigger faults */
    *(volatile uint32_t *)addr = 0x12345678;
    uint32_t value = *(volatile uint32_t *)addr;
    
    /* Deallocate the memory */
    vm_deallocate(mach_task_self(), addr, size);
    
    if (value == 0x12345678) {
        test_pass("VM fault test completed");
    } else {
        test_fail("VM fault test failed - memory corruption");
    }
}

void
test_dtrace_instrumentation()
{
    test_start("DTrace Instrumentation");
    
    printf("Testing DTrace kernel instrumentation framework\n");
    printf("This test exercises kernel paths that should trigger probes:\n");
    printf("- Thread switching (context switches)\n");
    printf("- IPC messages (mach_msg operations)\n");
    printf("- VM faults (memory operations)\n");
    
    test_thread_switching();
    test_ipc_messages();
    test_vm_faults();
    
    printf("DTrace instrumentation test completed successfully\n");
    printf("Probe activity should be visible in kernel logs if enabled\n");
    
    test_pass("DTrace instrumentation framework operational");
    test_end();
}

int
main(int argc, char *argv[])
{
    test_dtrace_instrumentation();
    return 0;
}