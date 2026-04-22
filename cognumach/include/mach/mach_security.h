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
 * mach_security.h - Advanced security analysis infrastructure for GNU Mach
 * 
 * Implements control flow integrity, runtime security monitoring,
 * and vulnerability detection capabilities for the GNU Mach microkernel.
 */

#ifndef _MACH_MACH_SECURITY_H_
#define _MACH_MACH_SECURITY_H_

#include <mach/kern_return.h>
#include <mach/mach_safety.h>
#include <mach/boolean.h>
#include <stdint.h>
#include <stddef.h>

/*
 * Control Flow Integrity (CFI) Infrastructure
 */

/* CFI magic values for return address validation */
#define CFI_RETURN_MAGIC    0xCAFEBABE
#define CFI_CALL_MAGIC      0xDEADBEEF

/* CFI validation result codes */
typedef enum {
    CFI_VALID = 0,
    CFI_INVALID_RETURN_ADDR,
    CFI_INVALID_CALL_TARGET,
    CFI_STACK_CORRUPTION,
    CFI_BUFFER_OVERFLOW
} cfi_result_t;

/* CFI context structure for validation */
struct cfi_context {
    uintptr_t expected_return;
    uintptr_t call_site;
    uintptr_t stack_base;
    uintptr_t stack_limit;
    uint32_t magic;
};

/*
 * Control Flow Integrity Functions
 */

/* Validate return address against expected value */
cfi_result_t cfi_validate_return(uintptr_t return_addr, uintptr_t expected);

/* Validate call target is legitimate function entry point */
cfi_result_t cfi_validate_call_target(uintptr_t target);

/* Check stack integrity for buffer overflows */
cfi_result_t cfi_check_stack_integrity(struct cfi_context *ctx);

/* Initialize CFI context for function entry */
void cfi_init_context(struct cfi_context *ctx, uintptr_t stack_base, 
                      uintptr_t stack_limit);

/*
 * Runtime Security Monitoring
 */

/* Security event types */
typedef enum {
    SEC_EVENT_CFI_VIOLATION = 1,
    SEC_EVENT_BUFFER_OVERFLOW,
    SEC_EVENT_STACK_SMASH,
    SEC_EVENT_ROP_ATTEMPT,
    SEC_EVENT_PRIVILEGE_ESCALATION,
    SEC_EVENT_RESOURCE_EXHAUSTION
} security_event_t;

/* Security monitoring statistics */
struct security_stats {
    uint64_t cfi_violations;
    uint64_t buffer_overflows;
    uint64_t stack_smashes;
    uint64_t rop_attempts;
    uint64_t privilege_escalations;
    uint64_t resource_exhaustions;
    uint64_t total_events;
    uint64_t last_event_time;
};

/* Runtime security monitoring functions */
void security_monitor_init(void);
void security_event_log(security_event_t event, uintptr_t addr, const char *context);
kern_return_t security_get_stats(struct security_stats *stats);
void security_reset_stats(void);

/* Check for runtime exploit patterns */
boolean_t security_detect_rop_chain(uintptr_t *addresses, int count);
boolean_t security_detect_stack_pivot(uintptr_t old_sp, uintptr_t new_sp);

/*
 * Vulnerability Detection Tools
 */

/* Buffer overflow detection */
typedef struct {
    uintptr_t buffer_start;
    size_t buffer_size;
    uint32_t canary_value;
} buffer_guard_t;

/* Initialize buffer guard with stack canary */
void buffer_guard_init(buffer_guard_t *guard, void *buffer, size_t size);

/* Check if buffer guard has been compromised */
boolean_t buffer_guard_check(buffer_guard_t *guard);

/* Memory safety validation */
kern_return_t memory_safety_check(void *ptr, size_t size, int access_type);

/* Stack canary functions */
void stack_canary_init(void);
uint32_t stack_canary_get(void);
boolean_t stack_canary_validate(uint32_t canary);

/*
 * Advanced Security Macros
 */

/* CFI-protected function entry */
#define CFI_FUNCTION_ENTER(ctx) \
    do { \
        uintptr_t __stack_base, __stack_limit; \
        asm volatile("movl %%esp, %0" : "=r"(__stack_base)); \
        __stack_limit = __stack_base - 8192; \
        cfi_init_context(&(ctx), __stack_base, __stack_limit); \
    } while (0)

/* CFI-protected function return */
#define CFI_FUNCTION_RETURN(ctx, ret_addr) \
    do { \
        if (cfi_validate_return((ret_addr), (ctx).expected_return) != CFI_VALID) { \
            security_event_log(SEC_EVENT_CFI_VIOLATION, (ret_addr), __FUNCTION__); \
        } \
    } while (0)

/* Buffer overflow protection macro */
#define BUFFER_OVERFLOW_CHECK(guard) \
    do { \
        if (!buffer_guard_check(&(guard))) { \
            security_event_log(SEC_EVENT_BUFFER_OVERFLOW, \
                              (uintptr_t)(guard).buffer_start, __FUNCTION__); \
        } \
    } while (0)

/* Runtime exploit detection */
#define ROP_CHAIN_DETECT(addrs, count) \
    do { \
        if (security_detect_rop_chain((addrs), (count))) { \
            security_event_log(SEC_EVENT_ROP_ATTEMPT, \
                              (uintptr_t)(addrs), __FUNCTION__); \
        } \
    } while (0)

/* Security monitoring enabled flag */
extern boolean_t security_monitoring_enabled;

/* Enable/disable security monitoring */
#define SECURITY_MONITORING_ENABLE()  (security_monitoring_enabled = TRUE)
#define SECURITY_MONITORING_DISABLE() (security_monitoring_enabled = FALSE)

/* Conditional security check - only if monitoring enabled */
#define SECURITY_CHECK(check) \
    do { \
        if (security_monitoring_enabled) { \
            (check); \
        } \
    } while (0)

#endif /* _MACH_MACH_SECURITY_H_ */