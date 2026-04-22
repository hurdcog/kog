/*
 * Kernel Feature Integration Test
 * Test for the kernel feature integration module
 *
 * Copyright (C) 2024 Free Software Foundation
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

#include <testlib.h>
#include <mach.h>

int main(int argc, char *argv[], int envc, char *envp[])
{
    kern_return_t ret;
    boolean_t enabled;
    uint32_t state;
    uint64_t init_calls, enable_calls, disable_calls;
    uint64_t total_operations, avg_latency_us, max_latency_us;
    host_t host_port;

    printf("Kernel Feature Integration Test\n");

    /* Get host port for system calls */
    host_port = mach_host_self();
    if (host_port == MACH_PORT_NULL) {
        printf("Failed to get host port\n");
        return 1;
    }

    /* Test 1: Check initial state */
    ret = kernel_feature_get_state(host_port, &state);
    if (ret == KERN_SUCCESS) {
        printf("✓ Initial feature state: %u\n", state);
    } else {
        printf("✗ Failed to get initial state (ret=%d)\n", ret);
        return 1;
    }

    /* Test 2: Enable feature */
    ret = kernel_feature_enable(host_port);
    if (ret == KERN_SUCCESS) {
        printf("✓ Feature enabled successfully\n");
    } else {
        printf("✗ Failed to enable feature (ret=%d)\n", ret);
        return 1;
    }

    /* Test 3: Check if enabled */
    ret = kernel_feature_is_enabled(host_port, &enabled);
    if (ret == KERN_SUCCESS && enabled) {
        printf("✓ Feature is enabled: %s\n", enabled ? "TRUE" : "FALSE");
    } else {
        printf("✗ Feature is not enabled or check failed (ret=%d, enabled=%d)\n", 
               ret, enabled);
        return 1;
    }

    /* Test 4: Get statistics */
    ret = kernel_feature_get_stats(host_port, &init_calls, &enable_calls,
                                  &disable_calls, &total_operations,
                                  &avg_latency_us, &max_latency_us);
    if (ret == KERN_SUCCESS) {
        printf("✓ Statistics retrieved:\n");
        printf("  Init calls: %llu\n", init_calls);
        printf("  Enable calls: %llu\n", enable_calls);
        printf("  Disable calls: %llu\n", disable_calls);
        printf("  Total operations: %llu\n", total_operations);
        printf("  Avg latency: %llu µs\n", avg_latency_us);
        printf("  Max latency: %llu µs\n", max_latency_us);
    } else {
        printf("✗ Failed to get statistics (ret=%d)\n", ret);
        return 1;
    }

    /* Test 5: Disable feature */
    ret = kernel_feature_disable(host_port);
    if (ret == KERN_SUCCESS) {
        printf("✓ Feature disabled successfully\n");
    } else {
        printf("✗ Failed to disable feature (ret=%d)\n", ret);
        return 1;
    }

    /* Test 6: Verify disabled state */
    ret = kernel_feature_is_enabled(host_port, &enabled);
    if (ret == KERN_SUCCESS && !enabled) {
        printf("✓ Feature is disabled: %s\n", enabled ? "TRUE" : "FALSE");
    } else {
        printf("✗ Feature is still enabled or check failed (ret=%d, enabled=%d)\n", 
               ret, enabled);
        return 1;
    }

    /* Test 7: Reset statistics */
    ret = kernel_feature_reset_stats(host_port);
    if (ret == KERN_SUCCESS) {
        printf("✓ Statistics reset successfully\n");
    } else {
        printf("✗ Failed to reset statistics (ret=%d)\n", ret);
        return 1;
    }

    printf("✓ All kernel feature integration tests passed!\n");
    printf("gnumach-test-success-and-reboot\n");
    return 0;
}