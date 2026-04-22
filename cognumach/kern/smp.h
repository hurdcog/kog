/* smp.h - Template for generic SMP controller for Mach. Header file
   Copyright (C) 2020 Free Software Foundation, Inc.
   Written by Almudena Garcia Jurado-Centurion

   This file is part of GNU Mach.

   GNU Mach is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GNU Mach is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA. */

/*
 * smp.h - Template for generic SMP controller for Mach. Header file
 * Copyright (C) 2020 Free Software Foundation, Inc.
 * Written by Almudena Garcia Jurado-Centurion
 * Enhanced for improved SMP support by GNU Mach contributors
 * Enhanced for SMP threading by GNU Copilot
 *
 * This file is part of GNU Mach.
 *
 * GNU Mach is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * GNU Mach is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA. */

#ifndef _KERN_SMP_H_
#define _KERN_SMP_H_

#include <mach/boolean.h>
#include <kern/lock.h>
#include <kern/queue.h>
#include <mach/kern_return.h>

/* Forward declarations */
struct thread;
struct processor;
struct processor_set;
typedef struct thread *thread_t;
typedef struct processor *processor_t;
typedef struct processor_set *processor_set_t;

/* SMP synchronization and data structures */
struct smp_cpu_info {
    unsigned char cpu_id;
    boolean_t is_online;
    boolean_t is_active;
    unsigned int load_average;
    unsigned long long idle_time;
    unsigned long long busy_time;
};

/* Work queue item structure - SMP threading enhancement */
struct smp_work_item {
    queue_chain_t  chain;
    void (*func)(void *arg);
    void *arg;
    int flags;
};

/* Work queue structure - SMP threading enhancement */
struct smp_work_queue {
    queue_head_t work_items;
    decl_simple_lock_data(, lock)
    struct thread *worker_thread;
    int cpu_id;
    boolean_t active;
};

/* CPU affinity mask - SMP threading enhancement */
typedef unsigned int cpu_mask_t;

/* Core SMP functions */
void smp_set_numcpus(unsigned char numcpus);
unsigned char smp_get_numcpus(void);

/* SMP-aware scheduling support */
boolean_t smp_cpu_is_online(unsigned char cpu);
void smp_cpu_set_online(unsigned char cpu, boolean_t online);
unsigned char smp_find_idle_cpu(void);
unsigned char smp_select_cpu_for_thread(void);

/* SMP load balancing */
void smp_balance_load(void);
unsigned int smp_get_cpu_load(unsigned char cpu);
void smp_update_cpu_load(unsigned char cpu);

/* SMP synchronization barriers */
void smp_synchronize_cpus(void);
void smp_cpu_barrier(void);

/* Enhanced SMP scheduler functions */
void smp_sched_init(void);
processor_t smp_choose_processor(thread_t thread, processor_set_t pset);
boolean_t smp_should_migrate_thread(thread_t thread, processor_t from_proc, processor_t to_proc);
void smp_balance_pset_load(processor_set_t pset);
boolean_t smp_steal_work(processor_t idle_proc);
void smp_enhanced_thread_setrun(thread_t th, boolean_t may_preempt);
int processor_to_cpu_id(processor_t processor);

/* Work queue management - SMP threading enhancement */
void smp_work_queue_init(void);
kern_return_t smp_queue_work(int cpu, void (*func)(void *), void *arg);
kern_return_t smp_queue_work_on(int cpu, void (*func)(void *), void *arg);
void smp_work_thread(void);

/* CPU affinity support - SMP threading enhancement */
#define CPU_AFFINITY_ANY    ((cpu_mask_t)-1)
#define CPU_AFFINITY_NONE   ((cpu_mask_t)0)

static inline boolean_t cpu_affinity_test(cpu_mask_t mask, int cpu) {
    return (mask & (1U << cpu)) != 0;
}

static inline cpu_mask_t cpu_affinity_set(cpu_mask_t mask, int cpu) {
    return mask | (1U << cpu);
}

static inline cpu_mask_t cpu_affinity_clear(cpu_mask_t mask, int cpu) {
    return mask & ~(1U << cpu);
}

/* CPU state management */
#if NCPUS > 1
extern struct smp_cpu_info smp_cpu_info[NCPUS];
#else
extern struct smp_cpu_info smp_cpu_info[1];
#endif
decl_simple_lock_data(extern, smp_cpu_info_lock);

#endif /* _KERN_SMP_H_ */
