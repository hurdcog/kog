/*
 * Valgrind compatibility interface for GNU Mach
 * Copyright (C) 2024 Free Software Foundation, Inc.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _MACH_VALGRIND_H_
#define _MACH_VALGRIND_H_

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/std_types.h>
#include <mach/machine/vm_types.h>
#include <vm/vm_types.h>

/*
 * GNU Mach Valgrind compatibility layer
 * 
 * This provides kernel-level support for memory error detection tools
 * like Valgrind, by exposing kernel memory allocation patterns and
 * implementing kernel-aware memory tracking mechanisms.
 */

/* Enable/disable Valgrind support at compile time */
#ifndef CONFIG_VALGRIND_SUPPORT
#define CONFIG_VALGRIND_SUPPORT 1
#endif

/* Valgrind memory state tracking */
typedef enum {
    VALGRIND_MEM_UNDEFINED = 0,  /* Uninitialized memory */
    VALGRIND_MEM_DEFINED,        /* Initialized/valid memory */
    VALGRIND_MEM_ADDRESSABLE,    /* Allocated but not initialized */
    VALGRIND_MEM_NOACCESS        /* Not accessible (freed/invalid) */
} valgrind_mem_state_t;

/* Memory tracking record for Valgrind integration */
struct valgrind_mem_record {
    vm_address_t start_addr;      /* Starting address */
    vm_size_t size;              /* Size in bytes */
    valgrind_mem_state_t state;  /* Current memory state */
    uint32_t alloc_id;           /* Allocation ID for tracking */
    vm_address_t caller_pc;      /* PC of allocating function */
    uint32_t timestamp;          /* Allocation timestamp */
} __attribute__((packed));

/* Valgrind operation types for kernel notifications */
typedef enum {
    VALGRIND_OP_MALLOC = 0,      /* Memory allocation */
    VALGRIND_OP_FREE,            /* Memory deallocation */  
    VALGRIND_OP_REALLOC,         /* Memory reallocation */
    VALGRIND_OP_CALLOC,          /* Zero-initialized allocation */
    VALGRIND_OP_MMAP,            /* Memory mapping */
    VALGRIND_OP_MUNMAP,          /* Memory unmapping */
    VALGRIND_OP_STACK_ALLOC,     /* Stack allocation */
    VALGRIND_OP_STACK_FREE,      /* Stack deallocation */
    VALGRIND_OP_MAKE_DEFINED,    /* Mark memory as defined */
    VALGRIND_OP_MAKE_UNDEFINED,  /* Mark memory as undefined */
    VALGRIND_OP_MAKE_NOACCESS    /* Mark memory as not accessible */
} valgrind_op_type_t;

/* Valgrind notification structure */
struct valgrind_notification {
    valgrind_op_type_t operation;
    vm_address_t address;
    vm_size_t size;
    vm_address_t old_address;    /* For realloc operations */
    vm_size_t old_size;          /* For realloc operations */
    uint32_t flags;              /* Operation-specific flags */
};

/* Maximum number of memory records to track */
#define VALGRIND_MAX_RECORDS 4096

/* Valgrind interface functions */
extern kern_return_t valgrind_init(void);
extern kern_return_t valgrind_enable(boolean_t enable);
extern boolean_t valgrind_is_enabled(void);

/* Memory state tracking functions */
extern kern_return_t valgrind_track_alloc(vm_address_t addr, vm_size_t size, 
                                          vm_address_t caller_pc);
extern kern_return_t valgrind_track_free(vm_address_t addr);
extern kern_return_t valgrind_track_realloc(vm_address_t old_addr, vm_address_t new_addr, 
                                            vm_size_t new_size);

/* Memory state manipulation */
extern kern_return_t valgrind_make_mem_defined(vm_address_t addr, vm_size_t size);
extern kern_return_t valgrind_make_mem_undefined(vm_address_t addr, vm_size_t size);
extern kern_return_t valgrind_make_mem_noaccess(vm_address_t addr, vm_size_t size);

/* Valgrind memory state queries */
extern valgrind_mem_state_t valgrind_check_mem_state(vm_address_t addr);
extern boolean_t valgrind_is_mem_defined(vm_address_t addr, vm_size_t size);
extern boolean_t valgrind_is_mem_addressable(vm_address_t addr, vm_size_t size);

/* Integration with existing debugging tools */
extern void valgrind_report_error(const char *error_type, vm_address_t addr, 
                                  vm_size_t size, const char *description);
extern void valgrind_print_backtrace(vm_address_t pc);

#if CONFIG_VALGRIND_SUPPORT

/* Client request mechanism (similar to Valgrind's client requests) */
#define VALGRIND_CLIENT_REQUEST_BASE 0x1000

enum {
    VG_USERREQ_MAKE_MEM_NOACCESS = VALGRIND_CLIENT_REQUEST_BASE,
    VG_USERREQ_MAKE_MEM_UNDEFINED,
    VG_USERREQ_MAKE_MEM_DEFINED, 
    VG_USERREQ_DISCARD_TRANSLATIONS,
    VG_USERREQ_CHECK_MEM_IS_ADDRESSABLE,
    VG_USERREQ_CHECK_MEM_IS_DEFINED,
    VG_USERREQ_DO_LEAK_CHECK,
    VG_USERREQ_COUNT_ERRORS
};

/* Macro for client requests from user space */
#define VALGRIND_DO_CLIENT_REQUEST(result, default_val, request_code, \
                                   arg1, arg2, arg3, arg4, arg5) \
    do { \
        (result) = valgrind_handle_client_request((request_code), \
                   (vm_address_t)(arg1), (vm_address_t)(arg2), \
                   (vm_address_t)(arg3), (vm_address_t)(arg4), \
                   (vm_address_t)(arg5)); \
        if ((result) == KERN_INVALID_ARGUMENT) \
            (result) = (default_val); \
    } while (0)

/* Client request handler */
extern kern_return_t valgrind_handle_client_request(uint32_t request, 
                                                   vm_address_t arg1, vm_address_t arg2,
                                                   vm_address_t arg3, vm_address_t arg4, 
                                                   vm_address_t arg5);

/* Convenience macros for common operations */
#define VALGRIND_MAKE_MEM_NOACCESS(addr, size) \
    valgrind_make_mem_noaccess((vm_address_t)(addr), (size))

#define VALGRIND_MAKE_MEM_UNDEFINED(addr, size) \
    valgrind_make_mem_undefined((vm_address_t)(addr), (size))

#define VALGRIND_MAKE_MEM_DEFINED(addr, size) \
    valgrind_make_mem_defined((vm_address_t)(addr), (size))

#define VALGRIND_CHECK_MEM_IS_DEFINED(addr, size) \
    valgrind_is_mem_defined((vm_address_t)(addr), (size))

#define VALGRIND_CHECK_MEM_IS_ADDRESSABLE(addr, size) \
    valgrind_is_mem_addressable((vm_address_t)(addr), (size))

#else /* !CONFIG_VALGRIND_SUPPORT */

/* No-op versions when Valgrind support is disabled */
#define VALGRIND_MAKE_MEM_NOACCESS(addr, size) do { } while (0)
#define VALGRIND_MAKE_MEM_UNDEFINED(addr, size) do { } while (0)
#define VALGRIND_MAKE_MEM_DEFINED(addr, size) do { } while (0)
#define VALGRIND_CHECK_MEM_IS_DEFINED(addr, size) TRUE
#define VALGRIND_CHECK_MEM_IS_ADDRESSABLE(addr, size) TRUE

#endif /* CONFIG_VALGRIND_SUPPORT */

/* Syscall interface numbers (to be added to mach.defs) */
#define MACH_VALGRIND_ENABLE_CALL     3600
#define MACH_VALGRIND_TRACK_ALLOC     3601
#define MACH_VALGRIND_TRACK_FREE      3602
#define MACH_VALGRIND_MAKE_DEFINED    3603
#define MACH_VALGRIND_MAKE_UNDEFINED  3604
#define MACH_VALGRIND_MAKE_NOACCESS   3605
#define MACH_VALGRIND_CHECK_MEM       3606
#define MACH_VALGRIND_CLIENT_REQUEST  3607

#endif /* _MACH_VALGRIND_H_ */