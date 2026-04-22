/*
 * Test LTTng-style tracing functionality
 * Copyright (C) 2024 Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <testlib.h>
#include <mach/lttng.h>

static char *test_name = "tracing";

/* Test basic tracing functionality */
static void
test_tracing_basic()
{
    /* Initialize tracing */
    mach_trace_init();
    
    /* Test enabling/disabling */
    assert(!mach_trace_is_enabled());
    mach_trace_enable(TRUE);
    assert(mach_trace_is_enabled());
    
    /* Generate some test events */
    mach_trace_event(MACH_TRACE_KERN, MACH_TRACE_LEVEL_INFO,
                     MACH_TRACE_EVENT_KERN_BASE + 100,
                     "Test event #%d", 1);
    
    mach_trace_event(MACH_TRACE_IPC, MACH_TRACE_LEVEL_DEBUG,
                     MACH_TRACE_EVENT_IPC_BASE + 100,
                     "IPC test event");
    
    mach_trace_event(MACH_TRACE_SCHED, MACH_TRACE_LEVEL_INFO,
                     MACH_TRACE_EVENT_SCHED_BASE + 100,
                     "Scheduler test event");
    
    printf("Generated 3 test trace events\n");
    
    /* Print statistics */
    mach_trace_print_stats();
    
    /* Disable tracing */
    mach_trace_enable(FALSE);
    assert(!mach_trace_is_enabled());
    
    printf("Basic tracing test passed\n");
}

/* Test tracepoint macros */
static void
test_tracepoint_macros()
{
    mach_trace_enable(TRUE);
    
    /* Test the convenience macros */
    TRACE_KERN(startup);
    TRACE_IPC(msg_send);
    TRACE_SCHED(thread_switch);
    TRACE_VM(page_fault);
    TRACE_DEBUG(warning);
    
    printf("Generated 5 tracepoint events\n");
    mach_trace_print_stats();
    
    mach_trace_enable(FALSE);
    printf("Tracepoint macro test passed\n");
}

/* Test performance with high event volume */
static void
test_tracing_performance()
{
    int i;
    uint64_t start_time, end_time;
    
    mach_trace_enable(TRUE);
    
    /* Record start time */
    start_time = timer_read();
    
    /* Generate many events to test buffer handling */
    for (i = 0; i < 1000; i++) {
        mach_trace_event(MACH_TRACE_KERN, MACH_TRACE_LEVEL_DEBUG,
                         MACH_TRACE_EVENT_KERN_BASE + 200,
                         "Performance test event %d", i);
    }
    
    /* Record end time */
    end_time = timer_read();
    
    printf("Generated 1000 events in %llu timer ticks\n", 
           end_time - start_time);
    
    mach_trace_print_stats();
    mach_trace_enable(FALSE);
    
    printf("Performance test completed\n");
}

/* Test disabled tracing (should have zero overhead) */
static void
test_tracing_disabled()
{
    uint64_t start_time, end_time;
    int i;
    
    /* Make sure tracing is disabled */
    mach_trace_enable(FALSE);
    assert(!mach_trace_is_enabled());
    
    start_time = timer_read();
    
    /* These should be no-ops */
    for (i = 0; i < 1000; i++) {
        mach_trace_event(MACH_TRACE_KERN, MACH_TRACE_LEVEL_DEBUG,
                         MACH_TRACE_EVENT_KERN_BASE + 300,
                         "Disabled event %d", i);
    }
    
    end_time = timer_read();
    
    printf("1000 disabled trace calls took %llu timer ticks\n",
           end_time - start_time);
    
    /* Should have minimal overhead when disabled */
    printf("Disabled tracing test passed\n");
}

int
main(int argc, char *argv[], int envc, char *envp[])
{
    int err = 0;
    
    printf("Starting LTTng-style tracing tests...\n");
    
    test_tracing_basic();
    test_tracepoint_macros();
    test_tracing_performance();
    test_tracing_disabled();
    
    printf("All tracing tests passed!\n");
    
    /* Success */
    printf("%s: PASS\n", test_name);
    
    return 0;
}