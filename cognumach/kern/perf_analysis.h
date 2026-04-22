/*
 * Performance Analysis Framework for GNU Mach
 * System-wide profiling, monitoring, and regression detection
 * 
 * Copyright (C) 2024 Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any later
 * version.
 */

#ifndef _KERN_PERF_ANALYSIS_H_
#define _KERN_PERF_ANALYSIS_H_

#include <kern/kern_types.h>
#include <kern/lock.h>
#include <mach/kern_return.h>
#include <mach/time_value.h>

/*
 * Performance event types for system-wide profiling
 */
typedef enum {
    PERF_EVENT_IPC_SEND = 0,
    PERF_EVENT_IPC_RECEIVE,
    PERF_EVENT_VM_ALLOC,
    PERF_EVENT_VM_FREE,
    PERF_EVENT_TASK_CREATE,
    PERF_EVENT_TASK_TERMINATE,
    PERF_EVENT_THREAD_CREATE,
    PERF_EVENT_THREAD_TERMINATE,
    PERF_EVENT_CONTEXT_SWITCH,
    PERF_EVENT_INTERRUPT,
    PERF_EVENT_SYSCALL,
    PERF_EVENT_PAGE_FAULT,
    PERF_EVENT_MAX
} perf_event_type_t;

/*
 * Performance sample data structure
 */
struct perf_sample {
    uint64_t timestamp;         /* High-resolution timestamp */
    perf_event_type_t event;    /* Event type */
    uint32_t cpu_id;           /* CPU where event occurred */
    uint32_t task_id;          /* Task ID (if applicable) */
    uint32_t thread_id;        /* Thread ID (if applicable) */
    uint64_t data1;            /* Event-specific data */
    uint64_t data2;            /* Additional event data */
    uint32_t duration_us;      /* Duration in microseconds */
};

/*
 * Performance statistics per event type
 */
struct perf_event_stats {
    uint64_t count;            /* Total event count */
    uint64_t total_time_us;    /* Total time spent */
    uint64_t min_time_us;      /* Minimum duration */
    uint64_t max_time_us;      /* Maximum duration */
    uint64_t avg_time_us;      /* Average duration */
    uint64_t last_timestamp;   /* Last event timestamp */
};

/*
 * System-wide performance monitoring state
 */
struct perf_monitor {
    boolean_t enabled;                     /* Monitoring enabled flag */
    boolean_t sampling_enabled;            /* Sample collection enabled */
    uint32_t sample_rate;                  /* Samples per second */
    uint32_t buffer_size;                  /* Sample buffer size */
    uint32_t buffer_head;                  /* Buffer write position */
    uint32_t buffer_tail;                  /* Buffer read position */
    uint32_t samples_dropped;              /* Dropped sample count */
    simple_lock_data_t lock;               /* Synchronization lock */
    
    struct perf_sample *sample_buffer;     /* Circular sample buffer */
    struct perf_event_stats event_stats[PERF_EVENT_MAX];
    
    /* Real-time monitoring thresholds */
    uint32_t latency_threshold_us;         /* Alert threshold for latency */
    uint32_t throughput_threshold;         /* Alert threshold for throughput */
    uint32_t error_rate_threshold;         /* Alert threshold for errors */
    
    /* Regression detection */
    uint64_t baseline_timestamp;           /* Baseline measurement time */
    struct perf_event_stats baseline_stats[PERF_EVENT_MAX];
    boolean_t regression_detected;         /* Regression alert flag */
};

/*
 * Performance analysis control structure
 */
struct perf_analysis_control {
    boolean_t profiling_enabled;          /* System profiling enabled */
    boolean_t trace_enabled;              /* Event tracing enabled */
    uint32_t trace_mask;                  /* Event mask for tracing */
    uint32_t profile_interval_ms;         /* Profiling interval */
    
    /* Instrumentation hooks */
    void (*pre_event_hook)(perf_event_type_t event, void *data);
    void (*post_event_hook)(perf_event_type_t event, void *data, uint32_t duration);
};

/* Global performance monitor instance */
extern struct perf_monitor global_perf_monitor;
extern struct perf_analysis_control perf_control;

/* Performance analysis framework functions */

/*
 * Initialize the performance analysis framework
 */
void perf_analysis_init(void);

/*
 * Enable/disable system-wide performance monitoring
 */
kern_return_t perf_monitor_enable(boolean_t enable);

/*
 * Configure monitoring parameters
 */
kern_return_t perf_monitor_configure(uint32_t sample_rate, uint32_t buffer_size);

/*
 * Record a performance event sample
 */
void perf_record_event(perf_event_type_t event, uint32_t task_id, 
                      uint32_t thread_id, uint64_t data1, uint64_t data2);

/*
 * Start timing an event (returns start timestamp)
 */
uint64_t perf_event_start(perf_event_type_t event);

/*
 * End timing an event and record sample
 */
void perf_event_end(perf_event_type_t event, uint64_t start_time, 
                   uint32_t task_id, uint32_t thread_id, 
                   uint64_t data1, uint64_t data2);

/*
 * Get performance statistics for an event type
 */
kern_return_t perf_get_event_stats(perf_event_type_t event, 
                                  struct perf_event_stats *stats);

/*
 * Get system-wide performance summary
 */
kern_return_t perf_get_system_stats(struct perf_event_stats *summary, 
                                   uint32_t *total_events);

/*
 * Set performance regression baseline
 */
kern_return_t perf_set_baseline(void);

/*
 * Check for performance regressions
 */
boolean_t perf_check_regression(perf_event_type_t event, uint32_t threshold_percent);

/*
 * Reset performance statistics
 */
void perf_reset_stats(void);

/*
 * Read performance samples from buffer
 */
kern_return_t perf_read_samples(struct perf_sample *buffer, uint32_t max_samples, 
                               uint32_t *samples_read);

/*
 * Real-time monitoring functions
 */
boolean_t perf_monitor_check_thresholds(void);
void perf_monitor_set_thresholds(uint32_t latency_us, uint32_t throughput, 
                                uint32_t error_rate);

/*
 * Performance instrumentation macros for easy integration
 */
#define PERF_EVENT_RECORD(event, task, thread, d1, d2) \
    do { \
        if (global_perf_monitor.enabled) { \
            perf_record_event(event, task, thread, d1, d2); \
        } \
    } while (0)

#define PERF_EVENT_TIME_START(event) \
    (global_perf_monitor.enabled ? perf_event_start(event) : 0)

#define PERF_EVENT_TIME_END(event, start, task, thread, d1, d2) \
    do { \
        if (global_perf_monitor.enabled && (start) != 0) { \
            perf_event_end(event, start, task, thread, d1, d2); \
        } \
    } while (0)

/*
 * Convenient timing wrapper macro
 */
#define PERF_TIME_BLOCK(event, task, thread, d1, d2, code_block) \
    do { \
        uint64_t _start = PERF_EVENT_TIME_START(event); \
        code_block; \
        PERF_EVENT_TIME_END(event, _start, task, thread, d1, d2); \
    } while (0)

#endif /* _KERN_PERF_ANALYSIS_H_ */