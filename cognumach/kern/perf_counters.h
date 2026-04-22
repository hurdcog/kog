/*
 * Enhanced Performance Counters for GNU Mach
 * Advanced metrics collection and analysis
 * 
 * Copyright (c) 2024 Cognu Mach Contributors
 * All rights reserved.
 */

#ifndef _KERN_PERF_COUNTERS_H_
#define _KERN_PERF_COUNTERS_H_

#include <kern/kern_types.h>
#include <kern/perf_analysis.h>

/*
 * Extended performance counter types
 */
typedef enum {
    PERF_COUNTER_INSTRUCTIONS = 0,      /* CPU instructions executed */
    PERF_COUNTER_CACHE_MISSES,          /* Cache miss events */
    PERF_COUNTER_BRANCH_MISPREDICTS,    /* Branch mispredictions */
    PERF_COUNTER_TLB_MISSES,            /* TLB miss events */
    PERF_COUNTER_MEMORY_BANDWIDTH,      /* Memory bandwidth usage */
    PERF_COUNTER_LOCK_CONTENTION,       /* Lock contention events */
    PERF_COUNTER_INTERRUPT_OVERHEAD,    /* Time spent in interrupts */
    PERF_COUNTER_SCHEDULE_LATENCY,      /* Scheduling latency */
    PERF_COUNTER_IPC_LATENCY,           /* IPC message latency */
    PERF_COUNTER_VM_PRESSURE,           /* Virtual memory pressure */
    PERF_COUNTER_MAX
} perf_counter_type_t;

/*
 * Performance counter data
 */
typedef struct perf_counter_data {
    uint64_t value;                /* Current counter value */
    uint64_t max_value;            /* Maximum observed value */
    uint64_t min_value;            /* Minimum observed value */
    uint64_t total;                /* Total accumulated value */
    uint64_t samples;              /* Number of samples */
    uint64_t last_update;          /* Timestamp of last update */
} perf_counter_data_t;

/*
 * System-wide performance snapshot
 */
typedef struct perf_system_snapshot {
    uint64_t timestamp;                                     /* When taken */
    uint32_t active_tasks;                                  /* Active task count */
    uint32_t active_threads;                                /* Active thread count */
    uint64_t total_memory_used;                             /* Memory usage */
    uint64_t cpu_utilization;                               /* CPU utilization % */
    perf_counter_data_t counters[PERF_COUNTER_MAX];        /* All counters */
    struct {
        uint64_t user_time;        /* Time in user mode */
        uint64_t kernel_time;      /* Time in kernel mode */
        uint64_t idle_time;        /* Idle time */
        uint64_t interrupt_time;   /* Time processing interrupts */
    } cpu_time;
} perf_system_snapshot_t;

/*
 * Performance trend analysis
 */
typedef struct perf_trend_data {
    perf_counter_type_t counter_type;
    uint64_t trend_period;             /* Analysis period in microseconds */
    double growth_rate;                /* Growth rate (positive/negative) */
    double volatility;                 /* Volatility measure */
    boolean_t anomaly_detected;        /* Anomaly flag */
    uint64_t prediction_next;          /* Predicted next value */
} perf_trend_data_t;

/*
 * Function prototypes
 */

/* Initialize enhanced performance counters */
void perf_counters_init(void);

/* Update a performance counter */
void perf_counter_update(perf_counter_type_t type, uint64_t value);

/* Get counter data */
boolean_t perf_counter_get(perf_counter_type_t type, perf_counter_data_t *data);

/* Take system performance snapshot */
boolean_t perf_take_system_snapshot(perf_system_snapshot_t *snapshot);

/* Analyze performance trends */
boolean_t perf_analyze_trends(perf_counter_type_t type, perf_trend_data_t *trend);

/* Detect performance anomalies */
boolean_t perf_detect_anomalies(perf_system_snapshot_t *snapshots, 
                                uint32_t count, 
                                perf_counter_type_t **anomalous_counters,
                                uint32_t *anomaly_count);

/* Get performance counter name */
const char *perf_counter_name(perf_counter_type_t type);

/* Reset all performance counters */
void perf_counters_reset(void);

/* Enable/disable specific counters */
boolean_t perf_counter_enable(perf_counter_type_t type, boolean_t enable);

/* Get system performance health score (0-100) */
uint32_t perf_get_system_health_score(void);

#endif /* _KERN_PERF_COUNTERS_H_ */