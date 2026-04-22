/*
 * Copyright (c) 2024 Cognu Mach Contributors
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * DTrace-like kernel instrumentation framework implementation
 */

#include <kern/dtrace.h>

#if MACH_DTRACE

#include <mach/time_value.h>
#include <kern/thread.h>
#include <kern/task.h>
#include <kern/cpu_number.h>
#include <kern/kalloc.h>
#include <kern/sched_prim.h>
#include <kern/printf.h>
#include <kern/mach_clock.h>
#include <string.h>

/*
 * Global DTrace state
 */
static dtrace_state_t dtrace_state;

/*
 * Maximum number of probes we support
 */
#define DTRACE_MAX_PROBES 512

/*
 * Initialize the DTrace subsystem
 */
void
dtrace_init(void)
{
    /* Initialize the global state */
    memset(&dtrace_state, 0, sizeof(dtrace_state));
    
    /* Allocate probe table */
    dtrace_state.max_probes = DTRACE_MAX_PROBES;
    dtrace_state.probes = (dtrace_probe_t *)kalloc(
        dtrace_state.max_probes * sizeof(dtrace_probe_t));
    
    if (dtrace_state.probes == NULL) {
        printf("dtrace_init: failed to allocate probe table\n");
        return;
    }
    
    memset(dtrace_state.probes, 0, 
           dtrace_state.max_probes * sizeof(dtrace_probe_t));
    
    /* Initialize locks */
    simple_lock_init_irq(&dtrace_state.probe_lock);
    simple_lock_init_irq(&dtrace_state.buffer.lock);
    
    /* Initialize buffer */
    dtrace_state.buffer.head = 0;
    dtrace_state.buffer.tail = 0;
    dtrace_state.buffer.count = 0;
    dtrace_state.buffer.overruns = 0;
    
    /* Initialize metrics */
    dtrace_state.metrics.max_probes = DTRACE_MAX_PROBES;
    
    /* Enable DTrace by default */
    dtrace_state.enabled = TRUE;
    
    /* Register some default probes */
    dtrace_probe_register(DTRACE_PROBE_THREAD_SWITCH, "thread_switch", 
                         "thread_invoke", NULL);
    dtrace_probe_register(DTRACE_PROBE_IPC_SEND, "ipc_send", 
                         "mach_msg_trap", NULL);
    dtrace_probe_register(DTRACE_PROBE_VM_FAULT, "vm_fault", 
                         "vm_fault", NULL);
    
    /* Enable the default probes */
    for (uint32_t i = 1; i <= 3; i++) {
        dtrace_probe_enable(i);
    }
    
    printf("DTrace instrumentation framework initialized (%d probes max)\n",
           DTRACE_MAX_PROBES);
    printf("Default probes registered: thread_switch, ipc_send, vm_fault\n");
}

/*
 * Shutdown the DTrace subsystem
 */
void
dtrace_shutdown(void)
{
    if (dtrace_state.probes) {
        kfree((vm_offset_t)dtrace_state.probes, 
              dtrace_state.max_probes * sizeof(dtrace_probe_t));
        dtrace_state.probes = NULL;
    }
    
    dtrace_state.enabled = FALSE;
    dtrace_state.probe_count = 0;
    
    printf("DTrace instrumentation framework shutdown\n");
}

/*
 * Get high-resolution timestamp in nanoseconds
 */
uint64_t
dtrace_gethrtime(void)
{
    /* Use record_time_stamp for high resolution */
    time_value64_t tv;
    record_time_stamp(&tv);
    
    /* Convert to nanoseconds */
    return ((uint64_t)tv.seconds * 1000000000ULL) + 
           ((uint64_t)tv.nanoseconds);
}

/*
 * Register a new probe
 */
uint32_t
dtrace_probe_register(dtrace_probe_type_t type, const char *name, 
                     const char *function, dtrace_handler_t handler)
{
    spl_t s;
    uint32_t probe_id = 0;
    dtrace_probe_t *probe;
    
    if (!dtrace_state.probes || !name) {
        return 0;
    }
    
    s = simple_lock_irq(&dtrace_state.probe_lock);
    
    /* Find an empty slot */
    for (uint32_t i = 1; i < dtrace_state.max_probes; i++) {
        if (dtrace_state.probes[i].id == 0) {
            probe_id = i;
            break;
        }
    }
    
    if (probe_id == 0) {
        simple_unlock_irq(s, &dtrace_state.probe_lock);
        return 0; /* No free slots */
    }
    
    probe = &dtrace_state.probes[probe_id];
    probe->id = probe_id;
    probe->type = type;
    probe->name = name;        /* Assumes static string */
    probe->function = function; /* Assumes static string */
    probe->enabled = FALSE;    /* Disabled by default */
    probe->fire_count = 0;
    probe->total_time = 0;
    probe->arg_count = 6;      /* Support up to 6 arguments */
    probe->handler = (void *)handler;
    
    dtrace_state.probe_count++;
    dtrace_state.metrics.active_probes = dtrace_state.probe_count;
    
    simple_unlock_irq(s, &dtrace_state.probe_lock);
    
    return probe_id;
}

/*
 * Enable a probe
 */
boolean_t
dtrace_probe_enable(uint32_t probe_id)
{
    spl_t s;
    boolean_t result = FALSE;
    
    if (probe_id >= dtrace_state.max_probes) {
        return FALSE;
    }
    
    s = simple_lock_irq(&dtrace_state.probe_lock);
    
    if (dtrace_state.probes[probe_id].id == probe_id) {
        dtrace_state.probes[probe_id].enabled = TRUE;
        result = TRUE;
    }
    
    simple_unlock_irq(s, &dtrace_state.probe_lock);
    
    return result;
}

/*
 * Disable a probe
 */
boolean_t
dtrace_probe_disable(uint32_t probe_id)
{
    spl_t s;
    boolean_t result = FALSE;
    
    if (probe_id >= dtrace_state.max_probes) {
        return FALSE;
    }
    
    s = simple_lock_irq(&dtrace_state.probe_lock);
    
    if (dtrace_state.probes[probe_id].id == probe_id) {
        dtrace_state.probes[probe_id].enabled = FALSE;
        result = TRUE;
    }
    
    simple_unlock_irq(s, &dtrace_state.probe_lock);
    
    return result;
}

/*
 * Remove a probe
 */
boolean_t
dtrace_probe_remove(uint32_t probe_id)
{
    spl_t s;
    boolean_t result = FALSE;
    
    if (probe_id >= dtrace_state.max_probes) {
        return FALSE;
    }
    
    s = simple_lock_irq(&dtrace_state.probe_lock);
    
    if (dtrace_state.probes[probe_id].id == probe_id) {
        memset(&dtrace_state.probes[probe_id], 0, sizeof(dtrace_probe_t));
        dtrace_state.probe_count--;
        dtrace_state.metrics.active_probes = dtrace_state.probe_count;
        result = TRUE;
    }
    
    simple_unlock_irq(s, &dtrace_state.probe_lock);
    
    return result;
}

/*
 * Fire a probe (core instrumentation function)
 */
void
dtrace_probe_fire(dtrace_probe_type_t type, const char *name,
                 uint64_t arg0, uint64_t arg1, uint64_t arg2,
                 uint64_t arg3, uint64_t arg4, uint64_t arg5)
{
    spl_t s;
    dtrace_event_t *event;
    dtrace_probe_t *probe = NULL;
    uint64_t start_time, end_time;
    thread_t thread;
    task_t task;
    
    /* Quick check if DTrace is enabled globally */
    if (!dtrace_state.enabled || !dtrace_state.probes) {
        return;
    }
    
    start_time = dtrace_gethrtime();
    
    /* Find the probe by type and name */
    for (uint32_t i = 1; i < dtrace_state.max_probes; i++) {
        if (dtrace_state.probes[i].id == i &&
            dtrace_state.probes[i].type == type &&
            dtrace_state.probes[i].enabled &&
            dtrace_state.probes[i].name &&
            strcmp(dtrace_state.probes[i].name, name) == 0) {
            probe = &dtrace_state.probes[i];
            break;
        }
    }
    
    if (!probe) {
        return; /* No matching enabled probe */
    }
    
    /* Update probe statistics */
    probe->fire_count++;
    dtrace_state.metrics.total_probes_fired++;
    
    /* Get current context */
    thread = current_thread();
    task = (thread != THREAD_NULL) ? thread->task : TASK_NULL;
    
    /* Add event to buffer */
    s = simple_lock_irq(&dtrace_state.buffer.lock);
    
    if (dtrace_state.buffer.count < DTRACE_BUFFER_SIZE) {
        event = &dtrace_state.buffer.events[dtrace_state.buffer.head];
        
        event->probe_id = probe->id;
        event->timestamp = start_time;
        event->cpu_id = cpu_number();
        event->thread_id = (thread != THREAD_NULL) ? (uint32_t)(uintptr_t)thread : 0;
        event->task_id = (task != TASK_NULL) ? (uint32_t)(uintptr_t)task : 0;
        event->args[0] = arg0;
        event->args[1] = arg1;
        event->args[2] = arg2;
        event->args[3] = arg3;
        event->args[4] = arg4;
        event->args[5] = arg5;
        
        dtrace_state.buffer.head = (dtrace_state.buffer.head + 1) % DTRACE_BUFFER_SIZE;
        dtrace_state.buffer.count++;
        dtrace_state.metrics.total_events_captured++;
    } else {
        /* Buffer is full, record overrun */
        dtrace_state.buffer.overruns++;
        dtrace_state.metrics.buffer_overruns++;
    }
    
    simple_unlock_irq(s, &dtrace_state.buffer.lock);
    
    /* Call probe handler if present */
    if (probe->handler) {
        dtrace_handler_t handler = (dtrace_handler_t)probe->handler;
        handler(probe, arg0, arg1, arg2, arg3, arg4, arg5);
    }
    
    /* Update timing statistics */
    end_time = dtrace_gethrtime();
    probe->total_time += (end_time - start_time);
    dtrace_state.metrics.probe_overhead_ns += (end_time - start_time);
}

/*
 * Read events from the buffer
 */
uint32_t
dtrace_buffer_read(dtrace_event_t *events, uint32_t max_events)
{
    spl_t s;
    uint32_t count = 0;
    
    if (!events || max_events == 0) {
        return 0;
    }
    
    s = simple_lock_irq(&dtrace_state.buffer.lock);
    
    while (count < max_events && dtrace_state.buffer.count > 0) {
        events[count] = dtrace_state.buffer.events[dtrace_state.buffer.tail];
        dtrace_state.buffer.tail = (dtrace_state.buffer.tail + 1) % DTRACE_BUFFER_SIZE;
        dtrace_state.buffer.count--;
        count++;
    }
    
    simple_unlock_irq(s, &dtrace_state.buffer.lock);
    
    return count;
}

/*
 * Clear the event buffer
 */
void
dtrace_buffer_clear(void)
{
    spl_t s;
    
    s = simple_lock_irq(&dtrace_state.buffer.lock);
    dtrace_state.buffer.head = 0;
    dtrace_state.buffer.tail = 0;
    dtrace_state.buffer.count = 0;
    simple_unlock_irq(s, &dtrace_state.buffer.lock);
}

/*
 * Get performance metrics
 */
void
dtrace_get_metrics(dtrace_metrics_t *metrics)
{
    if (metrics) {
        *metrics = dtrace_state.metrics;
    }
}

/*
 * Get number of registered probes
 */
uint32_t
dtrace_get_probe_count(void)
{
    return dtrace_state.probe_count;
}

/*
 * Get probe information by index
 */
boolean_t
dtrace_get_probe_info(uint32_t index, dtrace_probe_t *probe_info)
{
    spl_t s;
    boolean_t result = FALSE;
    
    if (!probe_info || index >= dtrace_state.max_probes) {
        return FALSE;
    }
    
    s = simple_lock_irq(&dtrace_state.probe_lock);
    
    if (dtrace_state.probes[index].id != 0) {
        *probe_info = dtrace_state.probes[index];
        /* Don't copy the handler pointer for security */
        probe_info->handler = NULL;
        result = TRUE;
    }
    
    simple_unlock_irq(s, &dtrace_state.probe_lock);
    
    return result;
}

/*
 * Enable DTrace globally
 */
void
dtrace_enable(void)
{
    dtrace_state.enabled = TRUE;
}

/*
 * Disable DTrace globally
 */
void
dtrace_disable(void)
{
    dtrace_state.enabled = FALSE;
}

/*
 * Check if DTrace is enabled
 */
boolean_t
dtrace_is_enabled(void)
{
    return dtrace_state.enabled;
}

#endif /* MACH_DTRACE */