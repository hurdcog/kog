/*
 * Enhanced spinlock implementations for SMP support in GNU Mach
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

#ifndef _KERN_SMP_LOCK_H_
#define _KERN_SMP_LOCK_H_

#include <kern/lock.h>
#include <kern/cpu_number.h>
#include <mach/boolean.h>

#if NCPUS > 1

/*
 * SMP-safe spinlock with backoff
 */
struct smp_spinlock {
    volatile unsigned int lock_data;
    volatile int owner_cpu;
    unsigned int spin_count;
    const char *name;
    struct {} is_smp_spinlock;
};

typedef struct smp_spinlock smp_spinlock_data_t;
typedef struct smp_spinlock *smp_spinlock_t;

/* SMP spinlock assertion macro */
#define smp_spinlock_assert(l) (void) &(l)->is_smp_spinlock

/* SMP spinlock initialization */
#define SMP_SPINLOCK_INITIALIZER(name) { 0, -1, 0, #name, {} }

#define smp_spinlock_init(lock, name) \
do { \
    (lock)->lock_data = 0; \
    (lock)->owner_cpu = -1; \
    (lock)->spin_count = 0; \
    (lock)->name = name; \
} while (0)

/*
 * SMP read-write spinlock for better scalability
 */
struct smp_rwlock {
    volatile int lock_data;     /* Positive = readers, -1 = writer, 0 = free */
    volatile int writer_cpu;    /* CPU ID of current writer */
    volatile int reader_count;  /* Number of active readers */
    volatile int waiting_writers; /* Number of waiting writers */
    const char *name;
    struct {} is_smp_rwlock;
};

typedef struct smp_rwlock smp_rwlock_data_t;
typedef struct smp_rwlock *smp_rwlock_t;

#define smp_rwlock_assert(l) (void) &(l)->is_smp_rwlock

#define SMP_RWLOCK_INITIALIZER(name) { 0, -1, 0, 0, #name, {} }

#define smp_rwlock_init(lock, name) \
do { \
    (lock)->lock_data = 0; \
    (lock)->writer_cpu = -1; \
    (lock)->reader_count = 0; \
    (lock)->waiting_writers = 0; \
    (lock)->name = name; \
} while (0)

/* Function declarations */
void smp_spinlock_lock(smp_spinlock_t lock);
boolean_t smp_spinlock_try_lock(smp_spinlock_t lock);
void smp_spinlock_unlock(smp_spinlock_t lock);
boolean_t smp_spinlock_locked(smp_spinlock_t lock);

void smp_rwlock_read_lock(smp_rwlock_t lock);
void smp_rwlock_read_unlock(smp_rwlock_t lock);
void smp_rwlock_write_lock(smp_rwlock_t lock);
void smp_rwlock_write_unlock(smp_rwlock_t lock);
boolean_t smp_rwlock_try_read_lock(smp_rwlock_t lock);
boolean_t smp_rwlock_try_write_lock(smp_rwlock_t lock);

/* Debugging and statistics */
void smp_lock_stats_init(void);
void smp_lock_stats_print(void);

#else /* NCPUS == 1 */

/* Single processor - no-op implementations */
struct smp_spinlock_empty { const char *name; struct {} is_smp_spinlock; };
struct smp_rwlock_empty { const char *name; struct {} is_smp_rwlock; };

typedef struct smp_spinlock_empty smp_spinlock_data_t;
typedef struct smp_spinlock_empty *smp_spinlock_t;
typedef struct smp_rwlock_empty smp_rwlock_data_t;
typedef struct smp_rwlock_empty *smp_rwlock_t;

#define smp_spinlock_assert(l) (void) &(l)->is_smp_spinlock
#define smp_rwlock_assert(l) (void) &(l)->is_smp_rwlock

#define smp_spinlock_init(l, n) do { (l)->name = n; } while(0)
#define smp_rwlock_init(l, n) do { (l)->name = n; } while(0)

#define smp_spinlock_lock(l) smp_spinlock_assert(l)
#define smp_spinlock_try_lock(l) (smp_spinlock_assert(l), TRUE)
#define smp_spinlock_unlock(l) smp_spinlock_assert(l)
#define smp_spinlock_locked(l) (smp_spinlock_assert(l), FALSE)

#define smp_rwlock_read_lock(l) smp_rwlock_assert(l)
#define smp_rwlock_read_unlock(l) smp_rwlock_assert(l)
#define smp_rwlock_write_lock(l) smp_rwlock_assert(l)
#define smp_rwlock_write_unlock(l) smp_rwlock_assert(l)
#define smp_rwlock_try_read_lock(l) (smp_rwlock_assert(l), TRUE)
#define smp_rwlock_try_write_lock(l) (smp_rwlock_assert(l), TRUE)

#define smp_lock_stats_init()
#define smp_lock_stats_print()

#endif /* NCPUS > 1 */

#endif /* _KERN_SMP_LOCK_H_ */