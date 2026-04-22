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
 * cfi_integrity.h - Control Flow Integrity header for GNU Mach
 * 
 * Internal header for CFI implementation.
 */

#ifndef _KERN_CFI_INTEGRITY_H_
#define _KERN_CFI_INTEGRITY_H_

#include <mach/mach_security.h>
#include <mach/boolean.h>
#include <mach/kern_return.h>

/* Internal CFI function declarations */
extern void cfi_init_context(struct cfi_context *ctx, uintptr_t stack_base, uintptr_t stack_limit);
extern cfi_result_t cfi_validate_return(uintptr_t return_addr, uintptr_t expected);
extern cfi_result_t cfi_validate_call_target(uintptr_t target);
extern cfi_result_t cfi_check_stack_integrity(struct cfi_context *ctx);

/* CFI protected call/return functions */
extern cfi_result_t cfi_protected_call(uintptr_t target, uintptr_t return_site);
extern cfi_result_t cfi_protected_return(uintptr_t return_addr);

/* CFI subsystem management */
extern void cfi_init(void);
extern kern_return_t cfi_add_function(uintptr_t entry_point);
extern kern_return_t cfi_remove_function(uintptr_t entry_point);

/* CFI debugging support */
extern int cfi_get_call_depth(void);
extern void cfi_dump_call_stack(void);

#endif /* _KERN_CFI_INTEGRITY_H_ */