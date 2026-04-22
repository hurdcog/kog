/*
 * Kernel Instrumentation Integration Module
 * Unified interface for DTrace, dynamic probes, performance counters, and LTTng
 * 
 * Copyright (c) 2024 Cognu Mach Contributors
 * All rights reserved.
 */

#include <kern/instrumentation_integration.h>
#include <kern/dtrace.h>
#include <kern/dynamic_probes.h>
#include <kern/perf_counters.h>
#include <kern/perf_analysis.h>
#include <mach/lttng.h>
#include <kern/printf.h>
#include <kern/lock.h>
#include <machine/spl.h>
#include <string.h>

/*
 * Global instrumentation state
 */
static struct {
    boolean_t initialized;
    boolean_t dtrace_enabled;
    boolean_t dynamic_probes_enabled;
    boolean_t performance_counters_enabled;
    boolean_t lttng_enabled;
    simple_lock_data_t lock;
    uint64_t total_overhead_ns;
    uint32_t active_probes;
} instrumentation_state;

/*
 * Initialize the unified instrumentation framework
 */
void
instrumentation_integration_init(void)
{
    simple_lock_init(&instrumentation_state.lock);
    
    instrumentation_state.initialized = TRUE;
    instrumentation_state.dtrace_enabled = TRUE;
    instrumentation_state.dynamic_probes_enabled = TRUE;
    instrumentation_state.performance_counters_enabled = TRUE;
    instrumentation_state.lttng_enabled = TRUE;
    instrumentation_state.total_overhead_ns = 0;
    instrumentation_state.active_probes = 0;
    
    printf("Unified kernel instrumentation framework initialized\n");
}

/*
 * Get instrumentation overhead percentage
 */
uint32_t
instrumentation_get_overhead_percent(void)
{
    if (!instrumentation_state.initialized) {
        return 0;
    }
    
    /* Get system uptime in nanoseconds */
    uint64_t uptime_ns = dtrace_gethrtime();  /* DTrace provides high-res time */
    
    if (uptime_ns == 0) {
        return 0;
    }
    
    /* Calculate overhead as percentage */
    uint64_t overhead_percent = (instrumentation_state.total_overhead_ns * 100) / uptime_ns;
    
    /* Cap at 100% for safety */
    return (uint32_t)(overhead_percent > 100 ? 100 : overhead_percent);
}

/*
 * Create a comprehensive probe that integrates all frameworks
 */
uint32_t
instrumentation_create_comprehensive_probe(const char *name, 
                                          const char *function,
                                          dtrace_probe_type_t type)
{
    uint32_t probe_id = 0;
    spl_t s;
    
    if (!instrumentation_state.initialized || !name || !function) {
        return 0;
    }
    
    s = splhigh();
    simple_lock(&instrumentation_state.lock);
    
    /* Create DTrace probe */
    if (instrumentation_state.dtrace_enabled) {
        probe_id = dtrace_probe_register(type, name, function, NULL);
    }
    
    /* Create dynamic probe configuration */
    if (instrumentation_state.dynamic_probes_enabled && probe_id > 0) {
        dynamic_probe_config_t config = {
            .module_name = "kernel",
            .function_name = function,
            .probe_name = name,
            .type = type,
            .address = 0,  /* Let dynamic probes resolve */
            .enabled = TRUE
        };
        
        dynamic_probe_create(&config);
    }
    
    /* Set up performance counter tracking */
    if (instrumentation_state.performance_counters_enabled) {
        perf_counter_update(PERF_COUNTER_LOCK_CONTENTION, 1);
    }
    
    /* Add LTTng trace point */
    if (instrumentation_state.lttng_enabled) {
        mach_trace_event(MACH_TRACE_SCHED, MACH_TRACE_LEVEL_INFO, 0, "probe_created: %s", name);
    }
    
    if (probe_id > 0) {
        instrumentation_state.active_probes++;
    }
    
    simple_unlock(&instrumentation_state.lock);
    splx(s);
    
    return probe_id;
}

/*
 * System health check using all instrumentation data
 */
uint32_t
instrumentation_system_health_check(void)
{
    uint32_t health_score = 100;  /* Start with perfect health */
    
    if (!instrumentation_state.initialized) {
        return 0;
    }
    
    /* Check instrumentation overhead */
    uint32_t overhead = instrumentation_get_overhead_percent();
    if (overhead > 5) {  /* Requirement: <5% overhead */
        health_score -= (overhead - 5) * 10;  /* Penalize heavy overhead */
    }
    
    /* Check performance metrics */
    uint32_t perf_health = perf_get_system_health_score();
    if (perf_health < 100) {
        health_score = (health_score + perf_health) / 2;  /* Average with perf health */
    }
    
    /* Check for probe failures */
    if (instrumentation_state.active_probes == 0) {
        health_score -= 20;  /* No active probes is concerning */
    }
    
    /* Ensure minimum score */
    return health_score > 0 ? health_score : 0;
}

/*
 * Generate comprehensive instrumentation report
 */
void
instrumentation_generate_report(void)
{
    if (!instrumentation_state.initialized) {
        printf("Instrumentation not initialized\n");
        return;
    }
    
    printf("\n=== Kernel Instrumentation Framework Report ===\n");
    printf("Framework Status: %s\n", 
           instrumentation_state.initialized ? "ACTIVE" : "INACTIVE");
    
    printf("\nComponent Status:\n");
    printf("  DTrace Framework:      %s\n", 
           instrumentation_state.dtrace_enabled ? "ENABLED" : "DISABLED");
    printf("  Dynamic Probes:        %s\n", 
           instrumentation_state.dynamic_probes_enabled ? "ENABLED" : "DISABLED");
    printf("  Performance Counters:  %s\n", 
           instrumentation_state.performance_counters_enabled ? "ENABLED" : "DISABLED");
    printf("  LTTng Tracing:         %s\n", 
           instrumentation_state.lttng_enabled ? "ENABLED" : "DISABLED");
    
    printf("\nPerformance Metrics:\n");
    printf("  Active Probes:         %u\n", instrumentation_state.active_probes);
    printf("  Total Overhead:        %llu ns\n", instrumentation_state.total_overhead_ns);
    printf("  Overhead Percentage:   %u%%\n", instrumentation_get_overhead_percent());
    printf("  System Health Score:   %u/100\n", instrumentation_system_health_check());
    
    /* Generate detailed reports from each subsystem */
    printf("\n--- DTrace Statistics ---\n");
    printf("  Total probes: %u\n", instrumentation_state.active_probes);
    
    printf("\n--- Performance Analysis ---\n");
    printf("  Overhead: %u%%\n", instrumentation_get_overhead_percent());
    
    printf("\n--- LTTng Statistics ---\n");
    mach_trace_print_stats();
    
    printf("=== End Report ===\n\n");
}

/*
 * Enable/disable instrumentation components at runtime
 */
boolean_t
instrumentation_set_component_state(instrumentation_component_t component, 
                                   boolean_t enabled)
{
    spl_t s;
    
    if (!instrumentation_state.initialized) {
        return FALSE;
    }
    
    s = splhigh();
    simple_lock(&instrumentation_state.lock);
    
    switch (component) {
        case INSTRUMENTATION_DTRACE:
            instrumentation_state.dtrace_enabled = enabled;
            break;
            
        case INSTRUMENTATION_DYNAMIC_PROBES:
            instrumentation_state.dynamic_probes_enabled = enabled;
            break;
            
        case INSTRUMENTATION_PERFORMANCE_COUNTERS:
            instrumentation_state.performance_counters_enabled = enabled;
            break;
            
        case INSTRUMENTATION_LTTNG:
            instrumentation_state.lttng_enabled = enabled;
            mach_trace_enable(enabled);
            break;
            
        default:
            simple_unlock(&instrumentation_state.lock);
            splx(s);
            return FALSE;
    }
    
    simple_unlock(&instrumentation_state.lock);
    splx(s);
    
    printf("Instrumentation component %d %s\n", 
           component, enabled ? "enabled" : "disabled");
    
    return TRUE;
}

/*
 * Performance regression analysis across all frameworks
 */
boolean_t
instrumentation_check_regression(void)
{
    boolean_t regression_detected = FALSE;
    
    if (!instrumentation_state.initialized) {
        return FALSE;
    }
    
    /* Check DTrace performance regression */
    uint32_t overhead = instrumentation_get_overhead_percent();
    if (overhead > 5) {  /* Success criteria: <5% overhead */
        printf("REGRESSION: Instrumentation overhead %u%% exceeds 5%% limit\n", overhead);
        regression_detected = TRUE;
    }
    
    /* Check performance analysis regression */
    for (perf_event_type_t event = 0; event < PERF_EVENT_MAX; event++) {
        if (perf_check_regression(event, 20)) {  /* 20% regression threshold */
            printf("REGRESSION: Performance event %d shows >20%% degradation\n", event);
            regression_detected = TRUE;
        }
    }
    
    /* Check system health */
    uint32_t health = instrumentation_system_health_check();
    if (health < 80) {  /* Health score threshold */
        printf("REGRESSION: System health score %u below 80 threshold\n", health);
        regression_detected = TRUE;
    }
    
    return regression_detected;
}

/*
 * Real-time monitoring status check
 */
boolean_t
instrumentation_real_time_monitor_active(void)
{
    return instrumentation_state.initialized &&
           instrumentation_state.performance_counters_enabled &&
           instrumentation_state.active_probes > 0;
}