/*
 * Microkernel Optimization Framework Header for GNU Mach
 * 
 * Copyright (C) 2024 Free Software Foundation, Inc.
 */

#ifndef _KERN_MICROKERNEL_OPTIMIZATIONS_H_
#define _KERN_MICROKERNEL_OPTIMIZATIONS_H_

#include <mach/kern_return.h>
#include <mach/mach_types.h>
#include <mach/vm_types.h>
#include <mach/boolean.h>
#include <kern/thread.h>

/* Microkernel optimization statistics */
struct microkernel_opt_stats {
    uint64_t ipc_optimizations;
    uint64_t scheduler_optimizations;
    uint64_t memory_optimizations;
    uint64_t cache_optimizations;
    boolean_t enabled;
    uint32_t flags;
};

/* Function prototypes */
extern void microkernel_optimization_init(void);

extern kern_return_t microkernel_optimize_ipc_fastpath(
    mach_port_name_t dest_port,
    mach_port_name_t reply_port,
    mach_msg_size_t msg_size);

extern void microkernel_optimize_scheduler_priority(thread_t thread);

extern void microkernel_optimize_memory_allocation(vm_size_t size, vm_offset_t *addr);

extern boolean_t microkernel_optimize_port_lookup(mach_port_name_t name);

extern void microkernel_get_optimization_stats(struct microkernel_opt_stats *stats);

extern kern_return_t microkernel_set_optimization_flags(uint32_t flags, boolean_t enable);

extern void microkernel_print_optimization_status(void);

/* Optimization flag definitions */
#define MK_OPT_IPC_FASTPATH      0x0001
#define MK_OPT_SCHED_PRIORITY    0x0002
#define MK_OPT_MEMORY_CACHE      0x0004
#define MK_OPT_PORT_LOOKUP       0x0008
#define MK_OPT_THREAD_SWITCH     0x0010
#define MK_OPT_ALL               0xFFFF

#endif /* _KERN_MICROKERNEL_OPTIMIZATIONS_H_ */