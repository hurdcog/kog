/*
 * Performance Analysis Interface Definitions
 * Mach interface for performance monitoring and profiling
 * 
 * Copyright (C) 2024 Free Software Foundation, Inc.
 */

#ifndef _MACH_PERF_MONITOR_H_
#define _MACH_PERF_MONITOR_H_

#include <mach/kern_return.h>
#include <mach/mach_types.h>

/*
 * Performance monitoring operations
 */
#define PERF_MONITOR_ENABLE        1
#define PERF_MONITOR_DISABLE       2
#define PERF_MONITOR_CONFIGURE     3
#define PERF_MONITOR_GET_STATS     4
#define PERF_MONITOR_READ_SAMPLES  5
#define PERF_MONITOR_SET_BASELINE  6
#define PERF_MONITOR_CHECK_REGRESSION 7
#define PERF_MONITOR_RESET_STATS   8
#define PERF_MONITOR_SET_THRESHOLDS 9

/*
 * Performance event types (matching kern/perf_analysis.h)
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
 * Performance sample structure for user space
 */
struct perf_sample_user {
    uint64_t timestamp;         /* High-resolution timestamp */
    uint32_t event;            /* Event type */
    uint32_t cpu_id;           /* CPU where event occurred */
    uint32_t task_id;          /* Task ID */
    uint32_t thread_id;        /* Thread ID */
    uint64_t data1;            /* Event-specific data */
    uint64_t data2;            /* Additional event data */
    uint32_t duration_us;      /* Duration in microseconds */
};

/*
 * Performance statistics structure for user space
 */
struct perf_event_stats_user {
    uint64_t count;            /* Total event count */
    uint64_t total_time_us;    /* Total time spent */
    uint64_t min_time_us;      /* Minimum duration */
    uint64_t max_time_us;      /* Maximum duration */
    uint64_t avg_time_us;      /* Average duration */
    uint64_t last_timestamp;   /* Last event timestamp */
};

/*
 * Configuration structure
 */
struct perf_monitor_config {
    uint32_t sample_rate;      /* Samples per second */
    uint32_t buffer_size;      /* Sample buffer size */
    uint32_t latency_threshold_us;   /* Latency alert threshold */
    uint32_t throughput_threshold;   /* Throughput alert threshold */
    uint32_t error_rate_threshold;   /* Error rate alert threshold */
};

/*
 * System summary structure
 */
struct perf_system_summary {
    uint32_t total_events;     /* Total events processed */
    uint64_t monitoring_time_us; /* Total monitoring time */
    uint32_t samples_dropped;  /* Dropped samples count */
    boolean_t regression_detected; /* Regression flag */
    struct perf_event_stats_user overall_stats;
};

#endif /* _MACH_PERF_MONITOR_H_ */