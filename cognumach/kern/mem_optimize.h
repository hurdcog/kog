/*
 * Copyright (c) 2024 GNU Project.
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
 *
 *
 * Memory optimization and compaction algorithms for GNU Mach.
 */

#ifndef _KERN_MEM_OPTIMIZE_H_
#define _KERN_MEM_OPTIMIZE_H_

#include <mach/machine/vm_types.h>
#include <vm/vm_types.h>
#include <sys/types.h>
#include <mach/boolean.h>

/* Forward declarations */
typedef int kern_return_t;

/*
 * Memory optimization policies.
 */
typedef enum {
    MEM_OPT_CONSERVATIVE = 0,  /* Conservative optimization */
    MEM_OPT_AGGRESSIVE,        /* Aggressive optimization */
    MEM_OPT_BALANCED           /* Balanced approach */
} mem_opt_policy_t;

/*
 * Memory optimization statistics.
 */
struct mem_opt_stats {
    uint32_t compaction_runs;      /* Number of compaction cycles */
    uint32_t pages_moved;          /* Pages moved during compaction */
    uint32_t fragments_merged;     /* Memory fragments merged */
    uint32_t allocation_failures;  /* Allocation failures before optimization */
    uint32_t allocation_successes; /* Successes after optimization */
    uint64_t bytes_reclaimed;      /* Total bytes reclaimed */
    uint32_t optimization_time_ms; /* Time spent in optimization (approx) */
};

/*
 * Memory fragmentation analysis.
 */
struct mem_fragmentation_info {
    uint32_t total_free_blocks;    /* Total free memory blocks */
    uint32_t largest_free_block;   /* Size of largest free block */
    uint32_t smallest_free_block;  /* Size of smallest free block */
    uint32_t avg_free_block_size;  /* Average free block size */
    uint32_t fragmentation_ratio;  /* Fragmentation ratio (0-100%) */
    boolean_t needs_compaction;    /* Whether compaction is recommended */
};

/*
 * Memory optimization functions.
 */
extern void mem_opt_init(void);
extern void mem_opt_set_policy(mem_opt_policy_t policy);
extern mem_opt_policy_t mem_opt_get_policy(void);

/*
 * Memory compaction and defragmentation.
 */
extern kern_return_t mem_opt_compact_memory(void);
extern kern_return_t mem_opt_defragment_slabs(void);
extern boolean_t mem_opt_should_compact(void);

/*
 * Memory allocation optimization.
 */
extern vm_offset_t mem_opt_allocate_best_fit(vm_size_t size);
extern void mem_opt_optimize_allocation_order(void);
extern void mem_opt_merge_adjacent_free_blocks(void);

/*
 * Memory fragmentation analysis.
 */
extern kern_return_t mem_opt_analyze_fragmentation(struct mem_fragmentation_info *info);
extern uint32_t mem_opt_calculate_fragmentation_ratio(void);
extern boolean_t mem_opt_is_heavily_fragmented(void);

/*
 * Memory pressure response.
 */
extern void mem_opt_handle_memory_pressure(void);
extern void mem_opt_emergency_reclaim(void);
extern void mem_opt_preemptive_cleanup(void);

/*
 * Statistics and reporting.
 */
extern void mem_opt_get_stats(struct mem_opt_stats *stats);
extern void mem_opt_report_optimization(void);
extern void mem_opt_reset_stats(void);

/*
 * Advanced optimization features.
 */
extern void mem_opt_background_optimize(void);
extern void mem_opt_adaptive_threshold_adjustment(void);
extern boolean_t mem_opt_predict_allocation_failure(vm_size_t size);

/*
 * Proactive memory management integration.
 */
extern void mem_opt_proactive_management(void);
extern boolean_t mem_opt_predict_allocation_failure_enhanced(vm_size_t size);

#endif /* _KERN_MEM_OPTIMIZE_H_ */