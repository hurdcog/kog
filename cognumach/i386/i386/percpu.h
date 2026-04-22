/*
 * Copyright (c) 2023 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _PERCPU_H_
#define _PERCPU_H_

struct percpu;

#if NCPUS > 1

#define percpu_assign(stm, val)     \
    asm("mov %[src], %%gs:%c[offs]" \
         : /* No outputs */         \
         : [src] "r" (val), [offs] "e" (__builtin_offsetof(struct percpu, stm)) \
         : );

#define percpu_get(typ, stm)        \
MACRO_BEGIN                         \
    typ val_;                       \
                                    \
    asm("mov %%gs:%c[offs], %[dst]" \
         : [dst] "=r" (val_)        \
         : [offs] "e" (__builtin_offsetof(struct percpu, stm)) \
         : );                       \
                                    \
    val_;                           \
MACRO_END

#define percpu_ptr(typ, stm)        \
MACRO_BEGIN                         \
    typ *ptr_ = (typ *)__builtin_offsetof(struct percpu, stm); \
                                    \
    asm("add %%gs:0, %[pointer]"    \
         : [pointer] "+r" (ptr_)    \
         : /* No inputs */          \
         : );                       \
                                    \
    ptr_;                           \
MACRO_END

#else

#define percpu_assign(stm, val)     \
MACRO_BEGIN                         \
        percpu_array[0].stm = val;  \
MACRO_END
#define percpu_get(typ, stm)        \
        (percpu_array[0].stm)
#define percpu_ptr(typ, stm)        \
        (&percpu_array[0].stm)

#endif

#include <kern/processor.h>
#include <kern/lock.h>
#include <kern/ast.h>
#include <mach/mach_types.h>
#include <mach/machine.h>

struct percpu {
    struct percpu	*self;
    int			apic_id;
    int			cpu_id;
    struct processor	processor;
    thread_t		active_thread;
    vm_offset_t		active_stack;
    /* SMP-essential per-CPU data */
    struct machine_slot	machine_slot;
    vm_offset_t		int_stack_top;
    vm_offset_t		int_stack_base;
    ast_t			need_ast;
    unsigned long		in_interrupt;
    /* Scheduling-related per-CPU data */
    thread_t		idle_thread;
    int			sched_balance_policy;
    unsigned long		runq_lock_count;
    /* Memory management per-CPU data */
    vm_offset_t		kernel_stack;
    /* Cache for frequently accessed items */
    vm_offset_t		cached_stack;
    thread_t		cached_thread;
    /* SMP synchronization support */
    simple_lock_data_t	cpu_lock;
    unsigned long		ipi_count;
    /* CPU state tracking */
    boolean_t		is_running;
    boolean_t		is_idle;
    unsigned long		context_switches;
};

extern struct percpu percpu_array[NCPUS];

void init_percpu(int cpu);
void smp_percpu_init(int cpu);
void smp_percpu_setup_stack(int cpu, vm_offset_t stack_base, vm_offset_t stack_top);
boolean_t smp_cpu_is_idle(int cpu);
void smp_cpu_set_idle(int cpu, boolean_t idle);
thread_t smp_get_idle_thread(int cpu);
void smp_set_idle_thread(int cpu, thread_t thread);

#endif /* _PERCPU_H_ */
