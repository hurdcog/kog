/*
 * cognitive_scheduler.h
 * 
 * Cognitive-aware process scheduler for Cognumach microkernel
 * Integrates attention values from ECAN into CPU allocation
 *
 * Copyright (C) 2025 OpenCog Foundation
 */

#ifndef _COGNUMACH_COGNITIVE_SCHEDULER_H
#define _COGNUMACH_COGNITIVE_SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Attention value structure */
typedef struct {
    float sti;          /* Short-term importance */
    float lti;          /* Long-term importance */
    float vlti;         /* Very long-term importance */
    uint64_t timestamp; /* Last update timestamp */
} attention_value_t;

/* Cognitive scheduling policy */
typedef enum {
    SCHED_POLICY_TRADITIONAL,   /* Traditional priority-based */
    SCHED_POLICY_ATTENTION,     /* Attention-based allocation */
    SCHED_POLICY_LEARNING,      /* Learning-based adaptive */
    SCHED_POLICY_HYBRID         /* Hybrid approach */
} sched_policy_t;

/* Cognitive workload classification */
typedef enum {
    WORKLOAD_PERCEPTION,    /* Sensory input processing */
    WORKLOAD_REASONING,     /* Inference and reasoning */
    WORKLOAD_LEARNING,      /* Learning and adaptation */
    WORKLOAD_ACTION,        /* Action generation and control */
    WORKLOAD_UNKNOWN        /* Unclassified */
} workload_type_t;

/* Scheduler statistics */
typedef struct {
    uint64_t total_switches;
    uint64_t attention_switches;
    uint64_t learning_adjustments;
    float avg_attention_value;
    float cpu_utilization;
} sched_stats_t;

/**
 * Initialize cognitive scheduler
 * 
 * @param policy Initial scheduling policy
 * @return 0 on success, negative on error
 */
int cognitive_sched_init(sched_policy_t policy);

/**
 * Set attention value for a task
 * 
 * @param task_id Task identifier
 * @param av Attention value
 * @return 0 on success, negative on error
 */
int cognitive_sched_set_attention(uint64_t task_id, const attention_value_t* av);

/**
 * Get attention value for a task
 * 
 * @param task_id Task identifier
 * @param av Output attention value
 * @return 0 on success, negative on error
 */
int cognitive_sched_get_attention(uint64_t task_id, attention_value_t* av);

/**
 * Update scheduling policy
 * 
 * @param policy New scheduling policy
 * @return 0 on success, negative on error
 */
int cognitive_sched_set_policy(sched_policy_t policy);

/**
 * Get current scheduling policy
 * 
 * @return Current policy
 */
sched_policy_t cognitive_sched_get_policy(void);

/**
 * Classify workload type for a task
 * 
 * @param task_id Task identifier
 * @param workload Workload classification
 * @return 0 on success, negative on error
 */
int cognitive_sched_classify_workload(uint64_t task_id, workload_type_t workload);

/**
 * Get workload classification for a task
 * 
 * @param task_id Task identifier
 * @return Workload type
 */
workload_type_t cognitive_sched_get_workload(uint64_t task_id);

/**
 * Update scheduler based on learning
 * 
 * This function allows the scheduler to adapt based on observed behavior
 * 
 * @return 0 on success, negative on error
 */
int cognitive_sched_learn(void);

/**
 * Get scheduler statistics
 * 
 * @param stats Output statistics
 * @return 0 on success, negative on error
 */
int cognitive_sched_get_stats(sched_stats_t* stats);

/**
 * Reset scheduler statistics
 * 
 * @return 0 on success, negative on error
 */
int cognitive_sched_reset_stats(void);

/**
 * Enable/disable cognitive scheduling
 * 
 * @param enable true to enable, false to disable
 * @return 0 on success, negative on error
 */
int cognitive_sched_enable(bool enable);

/**
 * Check if cognitive scheduling is enabled
 * 
 * @return true if enabled, false otherwise
 */
bool cognitive_sched_is_enabled(void);

/**
 * Tensor Thread Fiber scheduling support
 * 
 * Schedule parallel execution of cognitive operations
 * 
 * @param fiber_count Number of fibers
 * @param fiber_ids Array of fiber identifiers
 * @param attention_values Array of attention values for each fiber
 * @return 0 on success, negative on error
 */
int cognitive_sched_schedule_fibers(
    int fiber_count,
    const uint64_t* fiber_ids,
    const attention_value_t* attention_values
);

#ifdef __cplusplus
}
#endif

#endif /* _COGNUMACH_COGNITIVE_SCHEDULER_H */
