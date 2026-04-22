/*
 * System-wide debugging infrastructure for GNU Mach
 * Copyright (C) 2024 Free Software Foundation
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

#ifndef _MACH_SYSTEM_DEBUG_H_
#define _MACH_SYSTEM_DEBUG_H_

#include <mach/boolean.h>
#include <mach/mach_types.h>
#include <mach/kern_return.h>

/*
 * System-wide debugging subsystem identifiers
 * Used for coordinating debugging across different kernel components
 */
typedef enum {
    SYSDEBUG_SUBSYSTEM_KERNEL  = 0,  /* Core kernel services */
    SYSDEBUG_SUBSYSTEM_VM      = 1,  /* Virtual memory management */
    SYSDEBUG_SUBSYSTEM_IPC     = 2,  /* Inter-process communication */
    SYSDEBUG_SUBSYSTEM_SCHED   = 3,  /* Scheduler and threads */
    SYSDEBUG_SUBSYSTEM_DEVICE  = 4,  /* Device drivers and I/O */
    SYSDEBUG_SUBSYSTEM_NET     = 5,  /* Network stack */
    SYSDEBUG_SUBSYSTEM_MAX     = 6   /* Maximum number of subsystems */
} sysdebug_subsystem_t;

/*
 * System-wide debugging event types
 * Events that can be tracked across multiple subsystems
 */
typedef enum {
    SYSDEBUG_EVENT_NONE        = 0,
    SYSDEBUG_EVENT_INIT        = 1,  /* Subsystem initialization */
    SYSDEBUG_EVENT_SHUTDOWN    = 2,  /* Subsystem shutdown */
    SYSDEBUG_EVENT_ERROR       = 3,  /* Error conditions */
    SYSDEBUG_EVENT_WARNING     = 4,  /* Warning conditions */
    SYSDEBUG_EVENT_TRACE       = 5,  /* Execution traces */
    SYSDEBUG_EVENT_INTERACTION = 6,  /* Cross-subsystem interactions */
    SYSDEBUG_EVENT_RESOURCE    = 7,  /* Resource allocation/deallocation */
    SYSDEBUG_EVENT_STATE       = 8,  /* State changes */
    SYSDEBUG_EVENT_MAX         = 9
} sysdebug_event_t;

/*
 * System-wide debugging levels
 * Control granularity of debugging information
 */
typedef enum {
    SYSDEBUG_LEVEL_NONE    = 0,  /* No debugging */
    SYSDEBUG_LEVEL_MINIMAL = 1,  /* Critical events only */
    SYSDEBUG_LEVEL_NORMAL  = 2,  /* Normal debugging */
    SYSDEBUG_LEVEL_VERBOSE = 3,  /* Detailed debugging */
    SYSDEBUG_LEVEL_TRACE   = 4   /* Full trace debugging */
} sysdebug_level_t;

/*
 * System debugging context
 * Maintains state for coordinated debugging across subsystems
 */
struct sysdebug_context {
    boolean_t enabled;                           /* System debugging enabled */
    sysdebug_level_t global_level;              /* Global debugging level */
    sysdebug_level_t subsystem_levels[SYSDEBUG_SUBSYSTEM_MAX];  /* Per-subsystem levels */
    unsigned long event_count[SYSDEBUG_EVENT_MAX];  /* Event counters */
    unsigned long subsystem_events[SYSDEBUG_SUBSYSTEM_MAX][SYSDEBUG_EVENT_MAX];
    void *cross_component_context;               /* Cross-component debugging state */
};

/*
 * Cross-component debugging coordination
 * Enables debugging of interactions between different subsystems
 */
struct sysdebug_cross_component {
    boolean_t tracking_enabled;
    struct {
        sysdebug_subsystem_t from_subsystem;
        sysdebug_subsystem_t to_subsystem;
        sysdebug_event_t event_type;
        unsigned long timestamp;
        void *context_data;
    } interactions[32];  /* Ring buffer of recent interactions */
    int interaction_head;
    int interaction_count;
};

/*
 * System-wide debugging operations
 */

/* Initialize the system-wide debugging infrastructure */
extern void sysdebug_init(void);

/* Enable/disable system-wide debugging */
extern void sysdebug_enable(boolean_t enable);
extern boolean_t sysdebug_is_enabled(void);

/* Set debugging levels */
extern void sysdebug_set_global_level(sysdebug_level_t level);
extern void sysdebug_set_subsystem_level(sysdebug_subsystem_t subsystem, 
                                         sysdebug_level_t level);
extern sysdebug_level_t sysdebug_get_subsystem_level(sysdebug_subsystem_t subsystem);

/* Event reporting and tracking */
extern void sysdebug_report_event(sysdebug_subsystem_t subsystem,
                                  sysdebug_event_t event,
                                  const char *message,
                                  ...);

/* Cross-component debugging support */
extern void sysdebug_track_interaction(sysdebug_subsystem_t from_subsystem,
                                       sysdebug_subsystem_t to_subsystem,
                                       sysdebug_event_t event_type,
                                       const void *context_data);

extern void sysdebug_enable_cross_component_tracking(boolean_t enable);
extern boolean_t sysdebug_is_cross_component_tracking_enabled(void);

/* Unified debugging interface */
extern void sysdebug_dump_system_state(void);
extern void sysdebug_dump_subsystem_state(sysdebug_subsystem_t subsystem);
extern void sysdebug_dump_cross_component_interactions(void);

/* Statistics and monitoring */
extern void sysdebug_get_statistics(sysdebug_subsystem_t subsystem,
                                    unsigned long *event_counts);
extern void sysdebug_reset_statistics(void);

/*
 * Convenience macros for system-wide debugging
 */
#define SYSDEBUG_ENABLED() sysdebug_is_enabled()

#define SYSDEBUG_LOG(subsystem, event, message, ...) \
    do { \
        if (sysdebug_is_enabled()) { \
            sysdebug_report_event((subsystem), (event), (message), ##__VA_ARGS__); \
        } \
    } while (0)

#define SYSDEBUG_TRACE_INTERACTION(from, to, event, context) \
    do { \
        if (sysdebug_is_cross_component_tracking_enabled()) { \
            sysdebug_track_interaction((from), (to), (event), (context)); \
        } \
    } while (0)

/* Subsystem-specific convenience macros */
#define SYSDEBUG_VM_LOG(event, message, ...) \
    SYSDEBUG_LOG(SYSDEBUG_SUBSYSTEM_VM, (event), (message), ##__VA_ARGS__)

#define SYSDEBUG_IPC_LOG(event, message, ...) \
    SYSDEBUG_LOG(SYSDEBUG_SUBSYSTEM_IPC, (event), (message), ##__VA_ARGS__)

#define SYSDEBUG_SCHED_LOG(event, message, ...) \
    SYSDEBUG_LOG(SYSDEBUG_SUBSYSTEM_SCHED, (event), (message), ##__VA_ARGS__)

#define SYSDEBUG_DEVICE_LOG(event, message, ...) \
    SYSDEBUG_LOG(SYSDEBUG_SUBSYSTEM_DEVICE, (event), (message), ##__VA_ARGS__)

/* Integration with existing debugging infrastructure */
#ifdef DEBUG
#define SYSDEBUG_DEBUG_ASSERT(condition, subsystem, message) \
    do { \
        if (!(condition)) { \
            SYSDEBUG_LOG((subsystem), SYSDEBUG_EVENT_ERROR, \
                        "Debug assertion failed: %s: %s", #condition, (message)); \
        } \
    } while (0)
#else
#define SYSDEBUG_DEBUG_ASSERT(condition, subsystem, message) do {} while (0)
#endif

/*
 * Integration with GDB stub for system-wide debugging
 */
extern void sysdebug_gdb_integration_init(void);
extern void sysdebug_gdb_break_on_event(sysdebug_subsystem_t subsystem,
                                        sysdebug_event_t event);
extern void sysdebug_gdb_break_on_interaction(sysdebug_subsystem_t from,
                                              sysdebug_subsystem_t to);

#endif /* _MACH_SYSTEM_DEBUG_H_ */