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

/* Stress Test - Boundary Conditions and Resource Limits */

void test_port_exhaustion(void)
{
    printf("=== Port Exhaustion Stress Test ===\n");
    
    mach_port_t ports[1000];
    int allocated = 0;
    kern_return_t kr;
    
    /* Try to allocate many ports */
    for (int i = 0; i < 1000; i++) {
        kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &ports[i]);
        if (kr != KERN_SUCCESS) {
            printf("Port allocation failed at %d ports: %s\n", i, e2s(kr));
            break;
        }
        allocated = i + 1;
    }
    
    printf("Successfully allocated %d ports\n", allocated);
    ASSERT(allocated > 100, "Should be able to allocate at least 100 ports");
    
    /* Clean up allocated ports */
    for (int i = 0; i < allocated; i++) {
        mach_port_deallocate(mach_task_self(), ports[i]);
    }
    
    printf("Port exhaustion test completed\n");
}

void test_memory_fragmentation(void)
{
    printf("=== Memory Fragmentation Stress Test ===\n");
    
    vm_address_t allocations[100];
    int allocated = 0;
    kern_return_t kr;
    
    /* Allocate many small memory chunks to test fragmentation */
    for (int i = 0; i < 100; i++) {
        vm_size_t size = vm_page_size * (1 + (i % 8)); /* Variable sizes */
        kr = vm_allocate(mach_task_self(), &allocations[i], size, TRUE);
        if (kr != KERN_SUCCESS) {
            printf("Memory allocation failed at iteration %d: %s\n", i, e2s(kr));
            break;
        }
        allocated = i + 1;
        
        /* Write to allocated memory to ensure it's really allocated */
        volatile char *ptr = (volatile char *)allocations[i];
        for (vm_size_t j = 0; j < size; j += vm_page_size) {
            ptr[j] = (char)(i & 0xFF);
        }
    }
    
    printf("Successfully allocated %d memory chunks\n", allocated);
    ASSERT(allocated > 50, "Should be able to allocate at least 50 memory chunks");
    
    /* Clean up allocated memory */
    for (int i = 0; i < allocated; i++) {
        vm_size_t size = vm_page_size * (1 + (i % 8));
        vm_deallocate(mach_task_self(), allocations[i], size);
    }
    
    printf("Memory fragmentation test completed\n");
}

void test_large_message_ipc(void)
{
    printf("=== Large Message IPC Stress Test ===\n");
    
    mach_port_t port;
    kern_return_t kr;
    
    /* Create a port for testing */
    kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &port);
    ASSERT_RET(kr, "failed to create test port");
    
    /* Test increasingly large messages */
    for (int size = 64; size <= 8192; size *= 2) {
        char *msg_buffer = (char *)vm_page_size; /* Use page-aligned address */
        vm_size_t buffer_size = size + sizeof(mach_msg_header_t);
        
        kr = vm_allocate(mach_task_self(), (vm_address_t *)&msg_buffer, 
                        buffer_size, TRUE);
        if (kr != KERN_SUCCESS) {
            printf("Failed to allocate buffer for size %d: %s\n", size, e2s(kr));
            continue;
        }
        
        mach_msg_header_t *msg = (mach_msg_header_t *)msg_buffer;
        msg->msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
        msg->msgh_size = buffer_size;
        msg->msgh_remote_port = port;
        msg->msgh_local_port = MACH_PORT_NULL;
        msg->msgh_id = 2000 + size;
        
        /* Try to send the message (with timeout) */
        kr = mach_msg(msg, MACH_SEND_MSG | MACH_SEND_TIMEOUT,
                      buffer_size, 0, MACH_PORT_NULL, 100, MACH_PORT_NULL);
        
        printf("Message size %d bytes: %s\n", size, 
               (kr == MACH_SEND_TIMED_OUT) ? "OK (timeout)" : e2s(kr));
        
        vm_deallocate(mach_task_self(), (vm_address_t)msg_buffer, buffer_size);
    }
    
    /* Clean up */
    mach_port_deallocate(mach_task_self(), port);
    
    printf("Large message IPC test completed\n");
}

void test_rapid_task_operations(void)
{
    printf("=== Rapid Task Operations Stress Test ===\n");
    
    struct task_basic_info info;
    mach_msg_type_number_t count;
    kern_return_t kr;
    int successful_calls = 0;
    
    /* Rapidly call task_info many times */
    for (int i = 0; i < 10000; i++) {
        count = TASK_BASIC_INFO_COUNT;
        kr = task_info(mach_task_self(), TASK_BASIC_INFO, 
                      (task_info_t)&info, &count);
        if (kr == KERN_SUCCESS) {
            successful_calls++;
        } else {
            printf("task_info failed at iteration %d: %s\n", i, e2s(kr));
            break;
        }
        
        /* Brief check every 1000 calls */
        if (i % 1000 == 999) {
            printf("Completed %d task_info calls\n", i + 1);
        }
    }
    
    printf("Successfully completed %d task_info calls\n", successful_calls);
    ASSERT(successful_calls >= 1000, "Should complete at least 1000 task_info calls");
    
    printf("Rapid task operations test completed\n");
}

void test_boundary_conditions(void)
{
    printf("=== Boundary Conditions Test ===\n");
    
    kern_return_t kr;
    vm_address_t addr;
    
    /* Test zero-size allocation */
    kr = vm_allocate(mach_task_self(), &addr, 0, TRUE);
    printf("Zero-size allocation: %s\n", e2s(kr));
    
    /* Test very large allocation (should fail gracefully) */
    vm_size_t huge_size = (vm_size_t)(-1) / 2; /* Very large size */
    kr = vm_allocate(mach_task_self(), &addr, huge_size, TRUE);
    printf("Huge allocation (%lu bytes): %s\n", huge_size, e2s(kr));
    ASSERT(kr != KERN_SUCCESS, "Huge allocation should fail");
    
    /* Test invalid port operations */
    mach_port_t invalid_port = MACH_PORT_NAME_DEAD;
    mach_port_type_t port_type;
    kr = mach_port_type(mach_task_self(), invalid_port, &port_type);
    printf("Invalid port type query: %s\n", e2s(kr));
    ASSERT(kr != KERN_SUCCESS, "Invalid port operations should fail");
    
    printf("Boundary conditions test completed\n");
}

int main(int argc, char *argv[], int envc, char *envp[])
{
    printf("Starting Stress Tests - Boundary Conditions and Resource Limits\n");
    printf("Page size: %lu bytes\n", vm_page_size);
    
    test_port_exhaustion();
    test_memory_fragmentation();
    test_large_message_ipc();
    test_rapid_task_operations();
    test_boundary_conditions();
    
    printf("All stress tests completed successfully\n");
    return 0;
}