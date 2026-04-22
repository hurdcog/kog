/*
 * Kernel Instrumentation Integration Module Header
 * Unified interface for DTrace, dynamic probes, performance counters, and LTTng
 * 
 * Copyright (c) 2024 Cognu Mach Contributors
 * All rights reserved.
 */

#ifndef _KERN_INSTRUMENTATION_INTEGRATION_H_
#define _KERN_INSTRUMENTATION_INTEGRATION_H_

#include <kern/kern_types.h>
#include <kern/dtrace.h>

/*
 * Instrumentation component types
 */
typedef enum {
    INSTRUMENTATION_DTRACE = 0,
    INSTRUMENTATION_DYNAMIC_PROBES,
    INSTRUMENTATION_PERFORMANCE_COUNTERS,
    INSTRUMENTATION_LTTNG,
    INSTRUMENTATION_COMPONENT_MAX
} instrumentation_component_t;

/*
 * Comprehensive instrumentation configuration
 */
typedef struct instrumentation_config {
    boolean_t enable_dtrace;
    boolean_t enable_dynamic_probes;
    boolean_t enable_performance_counters;
    boolean_t enable_lttng;
    uint32_t max_overhead_percent;      /* Maximum allowed overhead (default: 5%) */
    uint32_t health_check_interval_ms;  /* Health check frequency */
} instrumentation_config_t;

/*
 * Instrumentation system status
 */
typedef struct instrumentation_status {
    boolean_t active;
    uint32_t active_probes;
    uint32_t overhead_percent;
    uint32_t health_score;
    uint64_t total_events;
    uint64_t dropped_events;
} instrumentation_status_t;

/*
 * Function prototypes
 */

/* Core initialization and management */
void instrumentation_integration_init(void);
boolean_t instrumentation_set_component_state(instrumentation_component_t component, 
                                             boolean_t enabled);

/* Probe management */
uint32_t instrumentation_create_comprehensive_probe(const char *name, 
                                                   const char *function,
                                                   dtrace_probe_type_t type);

/* Monitoring and analysis */
uint32_t instrumentation_get_overhead_percent(void);
uint32_t instrumentation_system_health_check(void);
boolean_t instrumentation_real_time_monitor_active(void);
boolean_t instrumentation_check_regression(void);

/* Reporting */
void instrumentation_generate_report(void);

/*
 * Convenience macros for integrated instrumentation
 */
#define INSTRUMENT_FUNCTION_ENTRY(func) \
    instrumentation_create_comprehensive_probe(#func "_entry", #func, DTRACE_PROBE_FUNCTION_ENTRY)

#define INSTRUMENT_FUNCTION_EXIT(func) \
    instrumentation_create_comprehensive_probe(#func "_exit", #func, DTRACE_PROBE_FUNCTION_EXIT)

#define INSTRUMENT_SYSCALL(syscall) \
    instrumentation_create_comprehensive_probe(#syscall "_syscall", #syscall, DTRACE_PROBE_SYSCALL_ENTRY)

#define INSTRUMENT_IPC_OPERATION(op) \
    instrumentation_create_comprehensive_probe(#op "_ipc", #op, DTRACE_PROBE_IPC_SEND)

/*
 * Runtime health check macro - checks if overhead is within limits
 */
#define INSTRUMENTATION_HEALTH_CHECK() \
    do { \
        if (instrumentation_get_overhead_percent() > 5) { \
            printf("WARNING: Instrumentation overhead exceeds 5%%\n"); \
        } \
    } while (0)

/*
 * Regression detection macro
 */
#define INSTRUMENTATION_REGRESSION_CHECK() \
    do { \
        if (instrumentation_check_regression()) { \
            printf("ALERT: Performance regression detected in instrumentation\n"); \
        } \
    } while (0)

#endif /* _KERN_INSTRUMENTATION_INTEGRATION_H_ */