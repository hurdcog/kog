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
 * security_monitor.h - Runtime security monitoring header for GNU Mach
 * 
 * Internal header file for security monitoring implementation.
 */

#ifndef _KERN_SECURITY_MONITOR_H_
#define _KERN_SECURITY_MONITOR_H_

#include <mach/mach_security.h>
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/time_value.h>

/* Internal function declarations */
extern void security_monitor_init(void);
extern void security_event_log(security_event_t event, uintptr_t addr, const char *context);
extern kern_return_t security_get_stats(struct security_stats *stats);
extern void security_reset_stats(void);
extern boolean_t security_detect_rop_chain(uintptr_t *addresses, int count);
extern boolean_t security_detect_stack_pivot(uintptr_t old_sp, uintptr_t new_sp);

/* Buffer guard functions */
extern void buffer_guard_init(buffer_guard_t *guard, void *buffer, size_t size);
extern boolean_t buffer_guard_check(buffer_guard_t *guard);

/* Memory safety functions */
extern kern_return_t memory_safety_check(void *ptr, size_t size, int access_type);

/* Stack canary functions */
extern void stack_canary_init(void);
extern uint32_t stack_canary_get(void);
extern boolean_t stack_canary_validate(uint32_t canary);

/* Global monitoring state */
extern boolean_t security_monitoring_enabled;

#endif /* _KERN_SECURITY_MONITOR_H_ */