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
 * cfi_integrity.c - Control Flow Integrity implementation for GNU Mach
 * 
 * Implements control flow integrity checking to detect and prevent
 * ROP/JOP attacks and control flow hijacking in the GNU Mach microkernel.
 */

#include "cfi_integrity.h"
#include <mach/mach_security.h>
#include "security_monitor.h"
#include <kern/printf.h>
#include <mach/machine.h>
#include <string.h>

/* Forward declare missing functions for compilation test */
extern void clock_get_uptime(time_value_t *);

/* CFI configuration constants */
#define CFI_MAX_CALL_DEPTH      64
#define CFI_VALID_CODE_START    0x100000  /* Start of valid code region */
#define CFI_VALID_CODE_END      0x800000  /* End of valid code region */

/* CFI call stack for return address validation */
static struct cfi_call_stack {
    uintptr_t addresses[CFI_MAX_CALL_DEPTH];
    int depth;
} cfi_call_stack;

/* Valid function entry points table (simplified implementation) */
static struct cfi_function_table {
    uintptr_t *entries;
    int count;
    int capacity;
} function_table;

/*
 * Initialize CFI context for function entry
 */
void
cfi_init_context(struct cfi_context *ctx, uintptr_t stack_base, uintptr_t stack_limit)
{
    if (!ctx) {
        return;
    }
    
    ctx->expected_return = 0;
    ctx->call_site = 0;
    ctx->stack_base = stack_base;
    ctx->stack_limit = stack_limit;
    ctx->magic = CFI_RETURN_MAGIC;
}

/*
 * Validate return address against expected value
 */
cfi_result_t
cfi_validate_return(uintptr_t return_addr, uintptr_t expected)
{
    /* Check if return address is in valid code region */
    if (return_addr < CFI_VALID_CODE_START || return_addr > CFI_VALID_CODE_END) {
        security_event_log(SEC_EVENT_CFI_VIOLATION, return_addr, "invalid_return_region");
        return CFI_INVALID_RETURN_ADDR;
    }
    
    /* Check if return address matches expected (if provided) */
    if (expected != 0 && return_addr != expected) {
        security_event_log(SEC_EVENT_CFI_VIOLATION, return_addr, "return_mismatch");
        return CFI_INVALID_RETURN_ADDR;
    }
    
    /* Validate return address is properly aligned */
    if ((return_addr & 0x3) != 0) {
        security_event_log(SEC_EVENT_CFI_VIOLATION, return_addr, "unaligned_return");
        return CFI_INVALID_RETURN_ADDR;
    }
    
    return CFI_VALID;
}

/*
 * Validate call target is legitimate function entry point
 */
cfi_result_t
cfi_validate_call_target(uintptr_t target)
{
    /* Check if target is in valid code region */
    if (target < CFI_VALID_CODE_START || target > CFI_VALID_CODE_END) {
        security_event_log(SEC_EVENT_CFI_VIOLATION, target, "invalid_call_target");
        return CFI_INVALID_CALL_TARGET;
    }
    
    /* Check alignment */
    if ((target & 0x3) != 0) {
        security_event_log(SEC_EVENT_CFI_VIOLATION, target, "unaligned_call");
        return CFI_INVALID_CALL_TARGET;
    }
    
    /* Check against function table if available */
    if (function_table.entries && function_table.count > 0) {
        for (int i = 0; i < function_table.count; i++) {
            if (function_table.entries[i] == target) {
                return CFI_VALID;
            }
        }
        security_event_log(SEC_EVENT_CFI_VIOLATION, target, "unlisted_function");
        return CFI_INVALID_CALL_TARGET;
    }
    
    return CFI_VALID;
}

/*
 * Check stack integrity for buffer overflows
 */
cfi_result_t
cfi_check_stack_integrity(struct cfi_context *ctx)
{
    uintptr_t current_sp;
    
    if (!ctx || ctx->magic != CFI_RETURN_MAGIC) {
        return CFI_STACK_CORRUPTION;
    }
    
    /* Get current stack pointer */
#if defined(__x86_64__)
    asm volatile("movq %%rsp, %0" : "=r"(current_sp));
#elif defined(__i386__)
    asm volatile("movl %%esp, %0" : "=r"(current_sp));
#else
    #error "Unsupported architecture for stack pointer read"
#endif
    
    /* Check if stack pointer is within expected bounds */
    if (current_sp < ctx->stack_limit || current_sp > ctx->stack_base) {
        security_event_log(SEC_EVENT_STACK_SMASH, current_sp, "stack_bounds");
        return CFI_STACK_CORRUPTION;
    }
    
    /* Check for stack overflow (growing downward) */
    if (current_sp < ctx->stack_limit + 1024) {  /* 1KB safety margin */
        security_event_log(SEC_EVENT_STACK_SMASH, current_sp, "stack_overflow");
        return CFI_BUFFER_OVERFLOW;
    }
    
    return CFI_VALID;
}

/*
 * Push call information to CFI call stack
 */
static void
cfi_push_call(uintptr_t return_addr)
{
    if (cfi_call_stack.depth < CFI_MAX_CALL_DEPTH) {
        cfi_call_stack.addresses[cfi_call_stack.depth] = return_addr;
        cfi_call_stack.depth++;
    } else {
        /* Call stack full - log warning but continue */
        security_event_log(SEC_EVENT_CFI_VIOLATION, return_addr, "call_stack_full");
    }
}

/*
 * Pop call information from CFI call stack
 */
static uintptr_t
cfi_pop_call(void)
{
    if (cfi_call_stack.depth > 0) {
        cfi_call_stack.depth--;
        return cfi_call_stack.addresses[cfi_call_stack.depth];
    }
    
    return 0;
}

/*
 * CFI-protected function call wrapper
 */
cfi_result_t
cfi_protected_call(uintptr_t target, uintptr_t return_site)
{
    cfi_result_t result;
    
    /* Validate call target */
    result = cfi_validate_call_target(target);
    if (result != CFI_VALID) {
        return result;
    }
    
    /* Push return information to call stack */
    cfi_push_call(return_site);
    
    return CFI_VALID;
}

/*
 * CFI-protected function return wrapper
 */
cfi_result_t
cfi_protected_return(uintptr_t return_addr)
{
    uintptr_t expected_return;
    cfi_result_t result;
    
    /* Pop expected return address */
    expected_return = cfi_pop_call();
    
    /* Validate return address */
    result = cfi_validate_return(return_addr, expected_return);
    if (result != CFI_VALID) {
        return result;
    }
    
    return CFI_VALID;
}

/*
 * Initialize CFI subsystem
 */
void
cfi_init(void)
{
    /* Initialize call stack */
    memset(&cfi_call_stack, 0, sizeof(cfi_call_stack));
    
    /* Initialize function table */
    function_table.entries = NULL;
    function_table.count = 0;
    function_table.capacity = 0;
    
    printf("CFI integrity checking initialized\n");
}

/*
 * Add function entry point to CFI table
 */
kern_return_t
cfi_add_function(uintptr_t entry_point)
{
    /* For simplified implementation, we don't maintain dynamic table */
    /* In production, this would manage a hash table of valid functions */
    return KERN_SUCCESS;
}

/*
 * Remove function entry point from CFI table
 */
kern_return_t
cfi_remove_function(uintptr_t entry_point)
{
    /* Simplified implementation */
    return KERN_SUCCESS;
}

/*
 * Get CFI call stack depth
 */
int
cfi_get_call_depth(void)
{
    return cfi_call_stack.depth;
}

/*
 * Dump CFI call stack for debugging
 */
void
cfi_dump_call_stack(void)
{
    int i;
    
    printf("CFI Call Stack (depth %d):\n", cfi_call_stack.depth);
    for (i = cfi_call_stack.depth - 1; i >= 0; i--) {
        printf("  %d: 0x%x\n", i, (unsigned int)cfi_call_stack.addresses[i]);
    }
}