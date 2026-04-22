/*
 * Unified debugging interface for GNU Mach
 * Provides a single point of access for all debugging functionality
 * Copyright (C) 2024 Free Software Foundation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 */

#ifndef _MACH_UNIFIED_DEBUG_H_
#define _MACH_UNIFIED_DEBUG_H_

#include <mach/system_debug.h>
#include <mach/message.h>
#include <kern/debug.h>

#if MACH_KDB
#include <gdb_stub.h>
#endif

/*
 * Unified debugging control structure
 * Coordinates between different debugging subsystems
 */
struct unified_debug_state {
    boolean_t initialized;
    boolean_t traditional_debug_enabled;    /* Original debug.c functionality */
    boolean_t system_debug_enabled;         /* New system-wide debugging */
    boolean_t gdb_debug_enabled;           /* GDB stub integration */
    boolean_t cross_component_tracking;    /* Cross-component interaction tracking */
};

/*
 * Global unified debugging operations
 */

/* Initialize all debugging subsystems */
extern void unified_debug_init(void);

/* Enable/disable all debugging */
extern void unified_debug_enable_all(boolean_t enable);
extern void unified_debug_disable_all(void);

/* Query debugging state */
extern boolean_t unified_debug_is_any_enabled(void);
extern void unified_debug_get_state(struct unified_debug_state *state);

/* Comprehensive debugging dump */
extern void unified_debug_dump_all_state(void);

/*
 * Cross-component debugging coordination
 * These functions help coordinate debugging across different kernel subsystems
 */

/* Virtual Memory debugging integration */
extern void unified_debug_vm_init(void);
extern void unified_debug_vm_operation(const char *operation, void *address, vm_size_t size);
extern void unified_debug_vm_fault(vm_offset_t fault_addr, kern_return_t result);

/* IPC debugging integration */  
extern void unified_debug_ipc_init(void);
extern void unified_debug_ipc_message(mach_msg_header_t *msg, boolean_t send);
extern void unified_debug_ipc_port_operation(const char *operation, mach_port_t port);

/* Thread/Task debugging integration */
extern void unified_debug_thread_init(void);
extern void unified_debug_thread_create(thread_t thread);
extern void unified_debug_thread_destroy(thread_t thread);
extern void unified_debug_thread_switch(thread_t old_thread, thread_t new_thread);
extern void unified_debug_task_create(task_t task);
extern void unified_debug_task_destroy(task_t task);

/* Device debugging integration */
extern void unified_debug_device_init(void);
extern void unified_debug_device_operation(const char *device_name, const char *operation);

/*
 * Advanced debugging features
 */

/* Debugging checkpoint and restore */
extern void unified_debug_checkpoint_create(const char *checkpoint_name);
extern void unified_debug_checkpoint_compare(const char *checkpoint_name);

/* Debugging session control */
extern void unified_debug_session_start(const char *session_name);
extern void unified_debug_session_end(const char *session_name);
extern void unified_debug_session_dump(const char *session_name);

/* Performance debugging */
extern void unified_debug_performance_start(const char *measurement_name);
extern void unified_debug_performance_end(const char *measurement_name);
extern void unified_debug_performance_dump(void);

/*
 * Integration with external debugging tools
 */

/* GDB integration */
extern void unified_debug_gdb_setup_system_breakpoints(void);
extern void unified_debug_gdb_enable_cross_component_breaks(boolean_t enable);

/* Debug script integration */
extern void unified_debug_script_command(const char *command);
extern void unified_debug_generate_gdb_scripts(void);

/*
 * Convenience macros for unified debugging
 */

/* System initialization debugging */
#define UNIFIED_DEBUG_INIT(subsystem_name) \
    do { \
        if (unified_debug_is_any_enabled()) { \
            sysdebug_report_event(SYSDEBUG_SUBSYSTEM_KERNEL, SYSDEBUG_EVENT_INIT, \
                                "Initializing %s subsystem", (subsystem_name)); \
        } \
    } while (0)

/* Function entry/exit tracing */
#define UNIFIED_DEBUG_FUNCTION_ENTRY(subsystem) \
    do { \
        if (unified_debug_is_any_enabled()) { \
            SYSDEBUG_ENTER_SUBSYSTEM(subsystem); \
        } \
    } while (0)

#define UNIFIED_DEBUG_FUNCTION_EXIT(subsystem) \
    do { \
        if (unified_debug_is_any_enabled()) { \
            SYSDEBUG_EXIT_SUBSYSTEM(subsystem); \
        } \
    } while (0)

/* Cross-subsystem call tracking */
#define UNIFIED_DEBUG_CROSS_CALL(from_subsystem, to_subsystem, function) \
    do { \
        if (unified_debug_is_any_enabled()) { \
            SYSDEBUG_CALL_SUBSYSTEM(from_subsystem, to_subsystem, function); \
        } \
    } while (0)

/* Error reporting that goes to all debugging subsystems */
#define UNIFIED_DEBUG_ERROR(subsystem, message, ...) \
    do { \
        if (unified_debug_is_any_enabled()) { \
            sysdebug_report_event((subsystem), SYSDEBUG_EVENT_ERROR, \
                                (message), ##__VA_ARGS__); \
            log(0, "[UNIFIED_DEBUG] Error in %s: " message, \
                sysdebug_subsystem_names[subsystem], ##__VA_ARGS__); \
        } \
    } while (0)

/* Warning reporting */
#define UNIFIED_DEBUG_WARNING(subsystem, message, ...) \
    do { \
        if (unified_debug_is_any_enabled()) { \
            sysdebug_report_event((subsystem), SYSDEBUG_EVENT_WARNING, \
                                (message), ##__VA_ARGS__); \
        } \
    } while (0)

/* Resource tracking */
#define UNIFIED_DEBUG_RESOURCE_ALLOC(subsystem, resource_type, size, address) \
    do { \
        if (unified_debug_is_any_enabled()) { \
            sysdebug_report_event((subsystem), SYSDEBUG_EVENT_RESOURCE, \
                                "Allocated %s: size=%zu, addr=%p", \
                                (resource_type), (size), (address)); \
        } \
    } while (0)

#define UNIFIED_DEBUG_RESOURCE_FREE(subsystem, resource_type, address) \
    do { \
        if (unified_debug_is_any_enabled()) { \
            sysdebug_report_event((subsystem), SYSDEBUG_EVENT_RESOURCE, \
                                "Freed %s: addr=%p", (resource_type), (address)); \
        } \
    } while (0)

/* State change tracking */
#define UNIFIED_DEBUG_STATE_CHANGE(subsystem, old_state, new_state) \
    do { \
        if (unified_debug_is_any_enabled()) { \
            sysdebug_report_event((subsystem), SYSDEBUG_EVENT_STATE, \
                                "State change: %s -> %s", (old_state), (new_state)); \
        } \
    } while (0)

/*
 * Debug helper functions for common patterns
 */

/* Check if debugging should be active for a given operation */
static inline boolean_t 
unified_debug_should_trace_subsystem(sysdebug_subsystem_t subsystem)
{
    return unified_debug_is_any_enabled() && 
           sysdebug_get_subsystem_level(subsystem) >= SYSDEBUG_LEVEL_TRACE;
}

static inline boolean_t
unified_debug_should_trace_interactions(void)
{
    return unified_debug_is_any_enabled() && 
           sysdebug_is_cross_component_tracking_enabled();
}

#endif /* _MACH_UNIFIED_DEBUG_H_ */