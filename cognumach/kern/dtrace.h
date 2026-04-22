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
 * DTrace-like kernel instrumentation framework for GNU Mach
 * 
 * This module provides dynamic kernel instrumentation capabilities
 * similar to DTrace, allowing for minimal-overhead probing of kernel
 * functions and events for performance analysis and debugging.
 */

#ifndef _KERN_DTRACE_H_
#define _KERN_DTRACE_H_

#include <kern/kern_types.h>
#include <kern/lock.h>

/*
 * Configuration flags
 */
#ifndef MACH_DTRACE
#define MACH_DTRACE 1  /* Enable DTrace-like instrumentation by default */
#endif

#if MACH_DTRACE

/*
 * Probe types
 */
typedef enum {
    DTRACE_PROBE_FUNCTION_ENTRY = 1,
    DTRACE_PROBE_FUNCTION_EXIT  = 2,
    DTRACE_PROBE_SYSCALL_ENTRY  = 3,
    DTRACE_PROBE_SYSCALL_EXIT   = 4,
    DTRACE_PROBE_IPC_SEND       = 5,
    DTRACE_PROBE_IPC_RECEIVE    = 6,
    DTRACE_PROBE_VM_FAULT       = 7,
    DTRACE_PROBE_THREAD_SWITCH  = 8,
    DTRACE_PROBE_TIMER_TICK     = 9,
    DTRACE_PROBE_CUSTOM         = 10
} dtrace_probe_type_t;

/*
 * Probe data structure
 */
typedef struct dtrace_probe {
    uint32_t             id;           /* Unique probe identifier */
    dtrace_probe_type_t  type;         /* Type of probe */
    const char          *name;         /* Human-readable name */
    const char          *function;     /* Function name */
    boolean_t            enabled;      /* Is probe enabled? */
    uint64_t             fire_count;   /* Number of times fired */
    uint64_t             total_time;   /* Total time spent in probe (ns) */
    uint32_t             arg_count;    /* Number of arguments */
    void                *handler;      /* Probe handler function */
} dtrace_probe_t;

/*
 * Probe event data
 */
typedef struct dtrace_event {
    uint32_t    probe_id;       /* Which probe fired */
    uint64_t    timestamp;      /* When it fired (nanoseconds) */
    uint32_t    cpu_id;         /* Which CPU */
    uint32_t    thread_id;      /* Which thread */
    uint32_t    task_id;        /* Which task */
    uint64_t    args[6];        /* Probe arguments */
} dtrace_event_t;

/*
 * Circular buffer for storing events
 */
#define DTRACE_BUFFER_SIZE 1024
typedef struct dtrace_buffer {
    dtrace_event_t   events[DTRACE_BUFFER_SIZE];
    uint32_t         head;          /* Next write position */
    uint32_t         tail;          /* Next read position */
    uint32_t         count;         /* Number of events */
    uint32_t         overruns;      /* Number of buffer overruns */
    simple_lock_irq_data_t lock;    /* Buffer lock */
} dtrace_buffer_t;

/*
 * Performance metrics
 */
typedef struct dtrace_metrics {
    uint64_t total_probes_fired;
    uint64_t total_events_captured;
    uint64_t buffer_overruns;
    uint64_t probe_overhead_ns;
    uint32_t active_probes;
    uint32_t max_probes;
} dtrace_metrics_t;

/*
 * Main DTrace state
 */
typedef struct dtrace_state {
    dtrace_probe_t     *probes;         /* Array of probes */
    uint32_t            probe_count;    /* Number of registered probes */
    uint32_t            max_probes;     /* Maximum number of probes */
    dtrace_buffer_t     buffer;         /* Event buffer */
    dtrace_metrics_t    metrics;        /* Performance metrics */
    boolean_t           enabled;        /* Global enable/disable */
    simple_lock_irq_data_t probe_lock;  /* Probe table lock */
} dtrace_state_t;

/*
 * Probe handler function type
 */
typedef void (*dtrace_handler_t)(dtrace_probe_t *probe, uint64_t arg0, 
                                uint64_t arg1, uint64_t arg2, uint64_t arg3,
                                uint64_t arg4, uint64_t arg5);

/*
 * Macros for instrumenting code
 */
#define DTRACE_FUNCTION_ENTRY(name) \
    dtrace_probe_fire(DTRACE_PROBE_FUNCTION_ENTRY, name, \
                      (uint64_t)__builtin_return_address(0), 0, 0, 0, 0, 0)

#define DTRACE_FUNCTION_EXIT(name) \
    dtrace_probe_fire(DTRACE_PROBE_FUNCTION_EXIT, name, \
                      (uint64_t)__builtin_return_address(0), 0, 0, 0, 0, 0)

#define DTRACE_SYSCALL_ENTRY(name, arg0) \
    dtrace_probe_fire(DTRACE_PROBE_SYSCALL_ENTRY, name, \
                      (uint64_t)(arg0), 0, 0, 0, 0, 0)

#define DTRACE_SYSCALL_EXIT(name, retval) \
    dtrace_probe_fire(DTRACE_PROBE_SYSCALL_EXIT, name, \
                      (uint64_t)(retval), 0, 0, 0, 0, 0)

#define DTRACE_IPC_SEND(port, size) \
    dtrace_probe_fire(DTRACE_PROBE_IPC_SEND, "ipc_send", \
                      (uint64_t)(port), (uint64_t)(size), 0, 0, 0, 0)

#define DTRACE_IPC_RECEIVE(port, size) \
    dtrace_probe_fire(DTRACE_PROBE_IPC_RECEIVE, "ipc_receive", \
                      (uint64_t)(port), (uint64_t)(size), 0, 0, 0, 0)

#define DTRACE_VM_FAULT(addr, type) \
    dtrace_probe_fire(DTRACE_PROBE_VM_FAULT, "vm_fault", \
                      (uint64_t)(addr), (uint64_t)(type), 0, 0, 0, 0)

#define DTRACE_THREAD_SWITCH(old_thread, new_thread) \
    dtrace_probe_fire(DTRACE_PROBE_THREAD_SWITCH, "thread_switch", \
                      (uint64_t)(old_thread), (uint64_t)(new_thread), 0, 0, 0, 0)

#define DTRACE_CUSTOM(name, arg0, arg1, arg2, arg3, arg4, arg5) \
    dtrace_probe_fire(DTRACE_PROBE_CUSTOM, name, \
                      (uint64_t)(arg0), (uint64_t)(arg1), (uint64_t)(arg2), \
                      (uint64_t)(arg3), (uint64_t)(arg4), (uint64_t)(arg5))

/*
 * Function prototypes
 */
void dtrace_init(void);
void dtrace_shutdown(void);

/* Probe management */
uint32_t dtrace_probe_register(dtrace_probe_type_t type, const char *name, 
                              const char *function, dtrace_handler_t handler);
boolean_t dtrace_probe_enable(uint32_t probe_id);
boolean_t dtrace_probe_disable(uint32_t probe_id);
boolean_t dtrace_probe_remove(uint32_t probe_id);

/* Event handling */
void dtrace_probe_fire(dtrace_probe_type_t type, const char *name,
                      uint64_t arg0, uint64_t arg1, uint64_t arg2,
                      uint64_t arg3, uint64_t arg4, uint64_t arg5);

/* Buffer management */
uint32_t dtrace_buffer_read(dtrace_event_t *events, uint32_t max_events);
void dtrace_buffer_clear(void);

/* Metrics and information */
void dtrace_get_metrics(dtrace_metrics_t *metrics);
uint32_t dtrace_get_probe_count(void);
boolean_t dtrace_get_probe_info(uint32_t index, dtrace_probe_t *probe_info);

/* Global enable/disable */
void dtrace_enable(void);
void dtrace_disable(void);
boolean_t dtrace_is_enabled(void);

/* Internal functions */
uint64_t dtrace_gethrtime(void);  /* Get high-resolution time */

#else /* !MACH_DTRACE */

/* No-op macros when DTrace is disabled */
#define DTRACE_FUNCTION_ENTRY(name)              do { } while (0)
#define DTRACE_FUNCTION_EXIT(name)               do { } while (0)
#define DTRACE_SYSCALL_ENTRY(name, arg0)         do { } while (0)
#define DTRACE_SYSCALL_EXIT(name, retval)        do { } while (0)
#define DTRACE_IPC_SEND(port, size)              do { } while (0)
#define DTRACE_IPC_RECEIVE(port, size)           do { } while (0)
#define DTRACE_VM_FAULT(addr, type)              do { } while (0)
#define DTRACE_THREAD_SWITCH(old_thread, new_thread) do { } while (0)
#define DTRACE_CUSTOM(name, arg0, arg1, arg2, arg3, arg4, arg5) do { } while (0)

/* No-op functions when DTrace is disabled */
static inline void dtrace_init(void) { }
static inline void dtrace_shutdown(void) { }
static inline void dtrace_enable(void) { }
static inline void dtrace_disable(void) { }
static inline boolean_t dtrace_is_enabled(void) { return FALSE; }

#endif /* MACH_DTRACE */

#endif /* _KERN_DTRACE_H_ */