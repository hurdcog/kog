/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 */

/*
 * security_monitor.c - Runtime security monitoring for GNU Mach
 * 
 * Implements runtime security event monitoring, exploit detection,
 * and security statistics collection for the GNU Mach microkernel.
 */

#include "security_monitor.h"
#include <mach/mach_security.h>
#include <mach/mach_safety.h>
#include <kern/printf.h>
#include <kern/lock.h>
#include <mach/time_value.h>
#include <mach/machine.h>
#include <string.h>

/* Forward declare missing functions for compilation test */
extern void clock_get_uptime(time_value_t *);

/* Global security monitoring state */
boolean_t security_monitoring_enabled = FALSE;
static struct security_stats global_security_stats;
static decl_simple_lock_data(, security_stats_lock)

/* Security event ring buffer */
#define SECURITY_EVENT_BUFFER_SIZE 1024
static struct security_event {
    security_event_t type;
    uintptr_t address;
    time_value_t timestamp;
    char context[64];
} security_event_buffer[SECURITY_EVENT_BUFFER_SIZE];

static int security_event_head = 0;
static int security_event_count = 0;
static decl_simple_lock_data(, security_event_lock)

/* Stack canary value */
static uint32_t stack_canary_value = 0;

/*
 * Initialize security monitoring subsystem
 */
void
security_monitor_init(void)
{
    /* Initialize locks */
    simple_lock_init(&security_stats_lock);
    simple_lock_init(&security_event_lock);
    
    /* Clear statistics */
    memset(&global_security_stats, 0, sizeof(global_security_stats));
    
    /* Initialize stack canary with random value */
    stack_canary_init();
    
    /* Clear event buffer */
    memset(security_event_buffer, 0, sizeof(security_event_buffer));
    security_event_head = 0;
    security_event_count = 0;
    
    /* Enable monitoring by default */
    security_monitoring_enabled = TRUE;
    
    printf("Security monitoring initialized\n");
}

/*
 * Log a security event
 */
void
security_event_log(security_event_t event, uintptr_t addr, const char *context)
{
    struct security_event *entry;
    time_value_t current_time;
    
    if (!security_monitoring_enabled) {
        return;
    }
    
    /* Get current time */
    clock_get_uptime(&current_time);
    
    /* Update statistics */
    simple_lock(&security_stats_lock);
    
    switch (event) {
        case SEC_EVENT_CFI_VIOLATION:
            global_security_stats.cfi_violations++;
            break;
        case SEC_EVENT_BUFFER_OVERFLOW:
            global_security_stats.buffer_overflows++;
            break;
        case SEC_EVENT_STACK_SMASH:
            global_security_stats.stack_smashes++;
            break;
        case SEC_EVENT_ROP_ATTEMPT:
            global_security_stats.rop_attempts++;
            break;
        case SEC_EVENT_PRIVILEGE_ESCALATION:
            global_security_stats.privilege_escalations++;
            break;
        case SEC_EVENT_RESOURCE_EXHAUSTION:
            global_security_stats.resource_exhaustions++;
            break;
    }
    
    global_security_stats.total_events++;
    global_security_stats.last_event_time = current_time.seconds;
    
    simple_unlock(&security_stats_lock);
    
    /* Add to event buffer */
    simple_lock(&security_event_lock);
    
    entry = &security_event_buffer[security_event_head];
    entry->type = event;
    entry->address = addr;
    entry->timestamp = current_time;
    
    if (context) {
        strncpy(entry->context, context, sizeof(entry->context) - 1);
        entry->context[sizeof(entry->context) - 1] = '\0';
    } else {
        entry->context[0] = '\0';
    }
    
    security_event_head = (security_event_head + 1) % SECURITY_EVENT_BUFFER_SIZE;
    if (security_event_count < SECURITY_EVENT_BUFFER_SIZE) {
        security_event_count++;
    }
    
    simple_unlock(&security_event_lock);
    
    /* Log critical events to console */
    if (event == SEC_EVENT_CFI_VIOLATION || 
        event == SEC_EVENT_BUFFER_OVERFLOW ||
        event == SEC_EVENT_STACK_SMASH) {
        printf("SECURITY: Event %d at 0x%lx in %s\n", event, addr, 
               context ? context : "unknown");
    }
}

/*
 * Get security statistics
 */
kern_return_t
security_get_stats(struct security_stats *stats)
{
    if (!stats) {
        return KERN_INVALID_ARGUMENT;
    }
    
    simple_lock(&security_stats_lock);
    *stats = global_security_stats;
    simple_unlock(&security_stats_lock);
    
    return KERN_SUCCESS;
}

/*
 * Reset security statistics
 */
void
security_reset_stats(void)
{
    simple_lock(&security_stats_lock);
    memset(&global_security_stats, 0, sizeof(global_security_stats));
    simple_unlock(&security_stats_lock);
    
    simple_lock(&security_event_lock);
    security_event_head = 0;
    security_event_count = 0;
    simple_unlock(&security_event_lock);
}

/*
 * Detect ROP chain patterns
 */
boolean_t
security_detect_rop_chain(uintptr_t *addresses, int count)
{
    int i;
    int suspicious_patterns = 0;
    
    if (!addresses || count < 3) {
        return FALSE;
    }
    
    /* Look for patterns indicating ROP gadgets */
    for (i = 0; i < count - 1; i++) {
        uintptr_t current = addresses[i];
        uintptr_t next = addresses[i + 1];
        
        /* Check for small jumps (typical of ROP gadgets) */
        if (current > next && current - next < 32) {
            suspicious_patterns++;
        }
        
        /* Check for unaligned addresses (unusual for normal calls) */
        if ((current & 0x3) != 0) {
            suspicious_patterns++;
        }
    }
    
    /* If more than 50% of transitions look suspicious, flag as ROP */
    return (suspicious_patterns > count / 2);
}

/*
 * Detect stack pivot attacks
 */
boolean_t
security_detect_stack_pivot(uintptr_t old_sp, uintptr_t new_sp)
{
    uintptr_t diff;
    
    if (new_sp > old_sp) {
        diff = new_sp - old_sp;
    } else {
        diff = old_sp - new_sp;
    }
    
    /* Flag suspicious if stack pointer moved more than 1MB */
    /* or if new stack is in an unexpected memory region */
    return (diff > 0x100000) || (new_sp < 0x1000);
}

/*
 * Initialize buffer guard with stack canary
 */
void
buffer_guard_init(buffer_guard_t *guard, void *buffer, size_t size)
{
    if (!guard || !buffer || size == 0) {
        return;
    }
    
    guard->buffer_start = (uintptr_t)buffer;
    guard->buffer_size = size;
    guard->canary_value = stack_canary_get();
    
    /* Place canary at end of buffer */
    if (size >= sizeof(uint32_t)) {
        uint32_t *canary_location = (uint32_t *)((char *)buffer + size - sizeof(uint32_t));
        *canary_location = guard->canary_value;
    }
}

/*
 * Check if buffer guard has been compromised
 */
boolean_t
buffer_guard_check(buffer_guard_t *guard)
{
    uint32_t *canary_location;
    
    if (!guard || guard->buffer_size < sizeof(uint32_t)) {
        return FALSE;
    }
    
    canary_location = (uint32_t *)((char *)guard->buffer_start + 
                                   guard->buffer_size - sizeof(uint32_t));
    
    return (*canary_location == guard->canary_value);
}

/*
 * Memory safety validation
 */
kern_return_t
memory_safety_check(void *ptr, size_t size, int access_type)
{
    uintptr_t addr = (uintptr_t)ptr;
    
    /* Basic NULL pointer check */
    if (!ptr) {
        return KERN_INVALID_ADDRESS;
    }
    
    /* Check for integer overflow in address calculation */
    if (!MACH_VALIDATE_REGION(addr, size)) {
        return KERN_INVALID_ARGUMENT;
    }
    
    /* Check for suspicious low addresses (NULL pointer dereference) */
    if (addr < 0x1000) {
        return KERN_INVALID_ADDRESS;
    }
    
    /* Check for kernel/user space boundary violations */
    /* This is architecture-specific and simplified for demonstration */
    if (addr >= 0xC0000000 && access_type == 1 /* user access */) {
        return KERN_PROTECTION_FAILURE;
    }
    
    return KERN_SUCCESS;
}

/*
 * Initialize stack canary system
 */
void
stack_canary_init(void)
{
    time_value_t current_time;
    
    /* Generate pseudo-random canary based on current time */
    clock_get_uptime(&current_time);
    stack_canary_value = (uint32_t)(current_time.seconds ^ current_time.microseconds);
    
    /* Mix in some additional entropy if available */
    stack_canary_value ^= 0xDEADBEEF;
    
    /* Ensure canary is never zero */
    if (stack_canary_value == 0) {
        stack_canary_value = 0xCAFEBABE;
    }
}

/*
 * Get current stack canary value
 */
uint32_t
stack_canary_get(void)
{
    return stack_canary_value;
}

/*
 * Validate stack canary value
 */
boolean_t
stack_canary_validate(uint32_t canary)
{
    return (canary == stack_canary_value);
}