/*
 * SMP-aware scheduler enhancements for GNU Mach
 * Copyright (C) 2023 Free Software Foundation, Inc.
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

#include <kern/sched_prim.h>
#include <kern/smp.h>
#include <kern/processor.h>
#include <kern/thread.h>
#include <kern/cpu_number.h>
#include <kern/lock.h>
#include <kern/printf.h>
#include <machine/spl.h>

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

#if NCPUS > 1

/* SMP load balancing thresholds */
#define SMP_LOAD_BALANCE_THRESHOLD  2    /* Trigger when load diff > 2 */
#define SMP_IDLE_STEAL_THRESHOLD    1    /* Steal work when idle for > 1 thread */
#define SMP_MIGRATION_COST         100   /* Cost of migrating thread between CPUs */

/* SMP-specific scheduling statistics */
struct smp_sched_stats {
    unsigned long migrations;
    unsigned long load_balances;
    unsigned long idle_steals;
    unsigned long cache_misses;
} smp_sched_stats[NCPUS];

decl_simple_lock_data(static, smp_sched_lock);

/*
 * Initialize SMP scheduler enhancements
 */
void smp_sched_init(void)
{
    int i;
    
    simple_lock_init(&smp_sched_lock);
    
    for (i = 0; i < NCPUS; i++) {
        smp_sched_stats[i].migrations = 0;
        smp_sched_stats[i].load_balances = 0;
        smp_sched_stats[i].idle_steals = 0;
        smp_sched_stats[i].cache_misses = 0;
    }
    
    printf("SMP scheduler enhancements initialized\n");
}

/*
 * Find the best CPU for scheduling a thread
 * Considers CPU affinity, load balancing, and cache locality
 */
processor_t smp_choose_processor(thread_t thread, processor_set_t pset)
{
    processor_t processor, best_processor = PROCESSOR_NULL;
    int min_load = INT_MAX;
    int current_cpu = cpu_number();
    
    if (!thread || !pset) {
        return PROCESSOR_NULL;
    }
    
    /* First, check if thread has processor affinity */
    if (thread->bound_processor != PROCESSOR_NULL) {
        return thread->bound_processor;
    }
    
    /* Look for idle processors first */
    if (pset->idle_count > 0) {
        /* Try to find idle processor with best cache locality */
        processor_t idle_processor = (processor_t) queue_first(&pset->idle_queue);
        
#ifdef HW_FOOTPRINT
        /* Check if last processor is idle (cache locality) */
        if (thread->last_processor && 
            thread->last_processor->state == PROCESSOR_IDLE &&
            thread->last_processor->processor_set == pset) {
            return thread->last_processor;
        }
#endif
        
        /* Return first idle processor */
        return idle_processor;
    }
    
    /* No idle processors, find least loaded one */
    queue_iterate(&pset->processors, processor, processor_t, processors) {
        int cpu = processor_to_cpu_id(processor);
        int load = smp_get_cpu_load(cpu);
        
        /* Consider cache locality bonus */
        if (thread->last_processor == processor) {
            load -= 20; /* Bonus for cache locality */
        }
        
        if (load < min_load) {
            min_load = load;
            best_processor = processor;
        }
    }
    
    return best_processor;
}

/*
 * SMP-aware thread migration decision
 */
boolean_t smp_should_migrate_thread(thread_t thread, processor_t from_proc, processor_t to_proc)
{
    int from_cpu, to_cpu;
    int from_load, to_load;
    
    if (!thread || !from_proc || !to_proc || from_proc == to_proc) {
        return FALSE;
    }
    
    from_cpu = processor_to_cpu_id(from_proc);
    to_cpu = processor_to_cpu_id(to_proc);
    
    from_load = smp_get_cpu_load(from_cpu);
    to_load = smp_get_cpu_load(to_cpu);
    
    /* Don't migrate if load difference is small */
    if ((from_load - to_load) < SMP_LOAD_BALANCE_THRESHOLD) {
        return FALSE;
    }
    
    /* Don't migrate if thread has strong cache affinity */
    if (thread->last_processor == from_proc && 
        (from_load - to_load) < SMP_MIGRATION_COST) {
        return FALSE;
    }
    
    /* Migrate if destination CPU is much less loaded */
    return TRUE;
}

/*
 * Perform load balancing across CPUs in the processor set
 */
void smp_balance_pset_load(processor_set_t pset)
{
    processor_t busiest_proc = PROCESSOR_NULL, idlest_proc = PROCESSOR_NULL;
    processor_t processor;
    int max_load = 0, min_load = INT_MAX;
    int current_cpu = cpu_number();
    thread_t thread;
    
    if (!pset || pset->processor_count <= 1) {
        return;
    }
    
    simple_lock(&smp_sched_lock);
    
    /* Find busiest and idlest processors */
    queue_iterate(&pset->processors, processor, processor_t, processors) {
        int cpu = processor_to_cpu_id(processor);
        int load = smp_get_cpu_load(cpu);
        
        if (load > max_load) {
            max_load = load;
            busiest_proc = processor;
        }
        
        if (load < min_load) {
            min_load = load;
            idlest_proc = processor;
        }
    }
    
    /* Check if load balancing is needed */
    if (!busiest_proc || !idlest_proc || 
        (max_load - min_load) < SMP_LOAD_BALANCE_THRESHOLD) {
        simple_unlock(&smp_sched_lock);
        return;
    }
    
    /* Try to migrate a thread from busiest to idlest processor */
    if (busiest_proc->runq.count > 1) {
        /* Find a suitable thread to migrate */
        queue_iterate(&busiest_proc->runq.runq[NRQS-1], thread, thread_t, links) {
            if (smp_should_migrate_thread(thread, busiest_proc, idlest_proc)) {
                /* Record migration statistics */
                int from_cpu = processor_to_cpu_id(busiest_proc);
                smp_sched_stats[from_cpu].migrations++;
                smp_sched_stats[current_cpu].load_balances++;
                
                printf("SMP: Migrating thread from CPU %d to CPU %d (load: %d -> %d)\n",
                       from_cpu, processor_to_cpu_id(idlest_proc), max_load, min_load);
                
                /* Migration would be performed here */
                /* This is a simplified example - actual migration requires more complex logic */
                break;
            }
        }
    }
    
    simple_unlock(&smp_sched_lock);
}

/*
 * Work stealing for idle processors
 */
boolean_t smp_steal_work(processor_t idle_proc)
{
    processor_set_t pset;
    processor_t processor;
    thread_t thread;
    int idle_cpu;
    
    if (!idle_proc || idle_proc->state != PROCESSOR_IDLE) {
        return FALSE;
    }
    
    idle_cpu = processor_to_cpu_id(idle_proc);
    pset = idle_proc->processor_set;
    
    if (!pset || pset->processor_count <= 1) {
        return FALSE;
    }
    
    /* Look for processors with excess work */
    queue_iterate(&pset->processors, processor, processor_t, processors) {
        if (processor == idle_proc) continue;
        
        int cpu = processor_to_cpu_id(processor);
        int load = smp_get_cpu_load(cpu);
        
        /* Try to steal work if processor has enough threads */
        if (load > SMP_IDLE_STEAL_THRESHOLD && processor->runq.count > 1) {
            /* Find a thread to steal */
            queue_iterate(&processor->runq.runq[NRQS-1], thread, thread_t, links) {
                if (thread->bound_processor == PROCESSOR_NULL) {
                    /* Record work steal statistics */
                    smp_sched_stats[idle_cpu].idle_steals++;
                    
                    printf("SMP: CPU %d stealing work from CPU %d\n", idle_cpu, cpu);
                    
                    /* Work stealing would be performed here */
                    /* This is a simplified example */
                    return TRUE;
                }
            }
        }
    }
    
    return FALSE;
}

/*
 * Enhanced thread_setrun with SMP awareness
 * This function enhances the existing thread_setrun behavior
 */
void smp_enhanced_thread_setrun(thread_t th, boolean_t may_preempt)
{
    processor_t processor;
    processor_set_t pset;
    
    if (!th) return;
    
    /* Use SMP-aware processor selection */
    pset = th->processor_set;
    processor = smp_choose_processor(th, pset);
    
    if (processor && processor != current_processor()) {
        int target_cpu = processor_to_cpu_id(processor);
        smp_update_cpu_load(target_cpu);
        
        /* Update thread's last processor for cache affinity */
        th->last_processor = processor;
    }
    
    /* Trigger load balancing periodically */
    if ((sched_tick % 100) == 0) {
        smp_balance_pset_load(pset);
    }
}

/*
 * Utility function to convert processor to CPU ID
 * (This would need proper implementation based on processor structure)
 */
int processor_to_cpu_id(processor_t processor)
{
    /* Simplified implementation - would need proper mapping */
    return 0; /* Default to CPU 0 for now */
}

#endif /* NCPUS > 1 */