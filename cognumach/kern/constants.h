/*
 * Copyright (C) 2024 Free Software Foundation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _KERN_CONSTANTS_H_
#define _KERN_CONSTANTS_H_

/*
 * Common kernel constants to replace magic numbers
 */

/* Lock monitoring constants */
#define LOCK_INFO_MAX_ENTRIES       (1024 * 32)  /* Maximum lock info entries */
#define LOCK_INFO_HASH_BUCKETS      1024         /* Number of hash buckets for lock info */
#define LOCK_DEFAULT_WAIT_TIME      100          /* Default lock wait time */
#define LOCK_MAX_HELD               1000         /* Maximum number of locks held */
#define LOCK_SPIN_LIMIT             1000000      /* Maximum spin iterations before panic */

/* IPC constants */
#define IPC_REPLY_SIZE_DEFAULT      8192         /* Default IPC reply message size */
#define IPC_STAT_COUNT_SIZE         512          /* Size of IPC statistics count array */
#define IPC_CALL_STATS_MASK         0x7fff       /* Mask for IPC call statistics */

/* Exception handling constants */
#define MACH_EXCEPTION_BASE_ID      2400         /* Base ID for Mach exceptions */
#define MACH_EXCEPTION_REPLY_OFFSET 100          /* Offset for exception reply IDs */

/* Thread state constants */
#define THREAD_STATE_MAX_SIZE       150          /* Maximum thread state array size */

/* Time conversion constants */
#define MICROSECONDS_PER_MILLISEC   1000         /* Microseconds in a millisecond */
#define NANOSECONDS_PER_MILLISEC    1000000      /* Nanoseconds in a millisecond */
#define MICROSECONDS_PER_SECOND     1000000      /* Microseconds in a second */
#define NANOSECONDS_PER_SECOND      1000000000   /* Nanoseconds in a second */

/* Performance monitoring */
#define PERF_PERCENTAGE_SCALE       100          /* Scale factor for percentage calculations */
#define PERF_MIN_SAMPLES_FOR_REGRESSION  10      /* Minimum samples needed for regression analysis */
#define PERF_DEFAULT_PROFILE_INTERVAL_MS 100     /* Default profiling interval in milliseconds */

/* Scheduler constants */
#define SCHED_WAIT_HASH_SIZE        1031         /* Size of wait event hash table (prime number) */
#define SCHED_CPU_USAGE_RESET_TICKS 30           /* Ticks after which to reset CPU usage stats */

/* Console output constants */
#define CONSOLE_TIMESTAMP_WIDTH     12           /* Width of console timestamp field */

/* General formatting limits */
#define PRINTF_MAX_PRECISION        0x7fffffff   /* Max precision fallback for printf */

#endif /* _KERN_CONSTANTS_H_ */