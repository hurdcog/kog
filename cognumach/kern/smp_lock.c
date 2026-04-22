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

#include <kern/smp_lock.h>
#include <kern/smp.h>
#include <kern/cpu_number.h>
#include <kern/printf.h>
#include <kern/debug.h>
#include <machine/spl.h>

#if NCPUS > 1

/* Lock statistics for debugging and performance analysis */
struct smp_lock_stats {
    unsigned long spinlock_acquisitions;
    unsigned long spinlock_contentions;
    unsigned long spinlock_spins;
    unsigned long rwlock_read_acquisitions;
    unsigned long rwlock_write_acquisitions;
    unsigned long rwlock_read_contentions;
    unsigned long rwlock_write_contentions;
} smp_lock_stats[NCPUS];

/* Exponential backoff parameters */
#define SMP_SPIN_MIN_DELAY 1
#define SMP_SPIN_MAX_DELAY 1024
#define SMP_SPIN_BACKOFF_FACTOR 2

/*
 * Exponential backoff with CPU pause
 */
static inline void smp_spin_delay(unsigned int delay)
{
    unsigned int i;
    for (i = 0; i < delay; i++) {
        cpu_pause(); /* Use CPU-specific pause instruction if available */
    }
}

/*
 * Atomic test-and-set operation
 * Returns TRUE if lock was acquired, FALSE if already locked
 */
static inline boolean_t atomic_test_and_set(volatile unsigned int *lock)
{
    unsigned int result;
    
    asm volatile(
        "lock xchgl %0, %1"
        : "=r" (result), "+m" (*lock)
        : "0" (1)
        : "memory"
    );
    
    return (result == 0);
}

/*
 * Atomic compare-and-swap operation
 */
static inline boolean_t atomic_cmpxchg(volatile int *ptr, int old_val, int new_val)
{
    unsigned char result;
    
    asm volatile(
        "lock cmpxchgl %2, %1; sete %0"
        : "=q" (result), "+m" (*ptr)
        : "r" (new_val), "a" (old_val)
        : "memory"
    );
    
    return result;
}

/*
 * SMP spinlock implementation with exponential backoff
 */
void smp_spinlock_lock(smp_spinlock_t lock)
{
    int current_cpu = cpu_number();
    unsigned int delay = SMP_SPIN_MIN_DELAY;
    unsigned int spins = 0;
    
    smp_spinlock_assert(lock);
    
    while (!atomic_test_and_set(&lock->lock_data)) {
        /* Exponential backoff to reduce bus traffic */
        smp_spin_delay(delay);
        
        if (delay < SMP_SPIN_MAX_DELAY) {
            delay *= SMP_SPIN_BACKOFF_FACTOR;
        }
        
        spins++;
        
        /* Check for potential deadlock (debugging) */
        if (spins > 10000) {
            printf("SMP: Potential deadlock in spinlock %s, CPU %d spinning\n",
                   lock->name ? lock->name : "unnamed", current_cpu);
            spins = 0; /* Reset counter to avoid spam */
        }
    }
    
    /* Lock acquired */
    lock->owner_cpu = current_cpu;
    lock->spin_count++;
    
    /* Update statistics */
    smp_lock_stats[current_cpu].spinlock_acquisitions++;
    if (spins > 0) {
        smp_lock_stats[current_cpu].spinlock_contentions++;
        smp_lock_stats[current_cpu].spinlock_spins += spins;
    }
}

/*
 * Try to acquire spinlock without blocking
 */
boolean_t smp_spinlock_try_lock(smp_spinlock_t lock)
{
    int current_cpu = cpu_number();
    
    smp_spinlock_assert(lock);
    
    if (atomic_test_and_set(&lock->lock_data)) {
        lock->owner_cpu = current_cpu;
        lock->spin_count++;
        smp_lock_stats[current_cpu].spinlock_acquisitions++;
        return TRUE;
    }
    
    return FALSE;
}

/*
 * Release spinlock
 */
void smp_spinlock_unlock(smp_spinlock_t lock)
{
    smp_spinlock_assert(lock);
    
    /* Verify we own the lock */
    if (lock->owner_cpu != cpu_number()) {
        panic("SMP: Attempting to unlock spinlock %s not owned by current CPU %d (owner: %d)",
              lock->name ? lock->name : "unnamed", cpu_number(), lock->owner_cpu);
    }
    
    lock->owner_cpu = -1;
    
    /* Memory barrier to ensure all stores complete before releasing lock */
    asm volatile("mfence" ::: "memory");
    
    /* Release the lock */
    lock->lock_data = 0;
}

/*
 * Check if spinlock is currently locked
 */
boolean_t smp_spinlock_locked(smp_spinlock_t lock)
{
    smp_spinlock_assert(lock);
    return (lock->lock_data != 0);
}

/*
 * SMP read-write lock - read lock
 */
void smp_rwlock_read_lock(smp_rwlock_t lock)
{
    int current_cpu = cpu_number();
    unsigned int spins = 0;
    
    smp_rwlock_assert(lock);
    
    while (1) {
        int current_val = lock->lock_data;
        
        /* If no writer and no waiting writers, try to acquire read lock */
        if (current_val >= 0 && lock->waiting_writers == 0) {
            if (atomic_cmpxchg(&lock->lock_data, current_val, current_val + 1)) {
                __sync_fetch_and_add(&lock->reader_count, 1);
                smp_lock_stats[current_cpu].rwlock_read_acquisitions++;
                return;
            }
        }
        
        /* Backoff and retry */
        smp_spin_delay(SMP_SPIN_MIN_DELAY);
        spins++;
        
        if (spins > 1000) {
            smp_lock_stats[current_cpu].rwlock_read_contentions++;
            spins = 0;
        }
    }
}

/*
 * SMP read-write lock - read unlock
 */
void smp_rwlock_read_unlock(smp_rwlock_t lock)
{
    smp_rwlock_assert(lock);
    
    __sync_fetch_and_sub(&lock->reader_count, 1);
    __sync_fetch_and_sub(&lock->lock_data, 1);
}

/*
 * SMP read-write lock - write lock
 */
void smp_rwlock_write_lock(smp_rwlock_t lock)
{
    int current_cpu = cpu_number();
    unsigned int spins = 0;
    
    smp_rwlock_assert(lock);
    
    __sync_fetch_and_add(&lock->waiting_writers, 1);
    
    while (1) {
        /* Try to acquire exclusive write lock */
        if (atomic_cmpxchg(&lock->lock_data, 0, -1)) {
            lock->writer_cpu = current_cpu;
            __sync_fetch_and_sub(&lock->waiting_writers, 1);
            smp_lock_stats[current_cpu].rwlock_write_acquisitions++;
            return;
        }
        
        /* Backoff and retry */
        smp_spin_delay(SMP_SPIN_MIN_DELAY);
        spins++;
        
        if (spins > 1000) {
            smp_lock_stats[current_cpu].rwlock_write_contentions++;
            spins = 0;
        }
    }
}

/*
 * SMP read-write lock - write unlock
 */
void smp_rwlock_write_unlock(smp_rwlock_t lock)
{
    smp_rwlock_assert(lock);
    
    /* Verify we own the write lock */
    if (lock->writer_cpu != cpu_number()) {
        panic("SMP: Attempting to unlock rwlock %s not owned by current CPU %d (owner: %d)",
              lock->name ? lock->name : "unnamed", cpu_number(), lock->writer_cpu);
    }
    
    lock->writer_cpu = -1;
    
    /* Memory barrier */
    asm volatile("mfence" ::: "memory");
    
    /* Release the lock */
    lock->lock_data = 0;
}

/*
 * Try to acquire read lock without blocking
 */
boolean_t smp_rwlock_try_read_lock(smp_rwlock_t lock)
{
    int current_cpu = cpu_number();
    int current_val = lock->lock_data;
    
    smp_rwlock_assert(lock);
    
    if (current_val >= 0 && lock->waiting_writers == 0) {
        if (atomic_cmpxchg(&lock->lock_data, current_val, current_val + 1)) {
            __sync_fetch_and_add(&lock->reader_count, 1);
            smp_lock_stats[current_cpu].rwlock_read_acquisitions++;
            return TRUE;
        }
    }
    
    return FALSE;
}

/*
 * Try to acquire write lock without blocking
 */
boolean_t smp_rwlock_try_write_lock(smp_rwlock_t lock)
{
    int current_cpu = cpu_number();
    
    smp_rwlock_assert(lock);
    
    if (atomic_cmpxchg(&lock->lock_data, 0, -1)) {
        lock->writer_cpu = current_cpu;
        smp_lock_stats[current_cpu].rwlock_write_acquisitions++;
        return TRUE;
    }
    
    return FALSE;
}

/*
 * Initialize lock statistics
 */
void smp_lock_stats_init(void)
{
    int i;
    
    for (i = 0; i < NCPUS; i++) {
        smp_lock_stats[i].spinlock_acquisitions = 0;
        smp_lock_stats[i].spinlock_contentions = 0;
        smp_lock_stats[i].spinlock_spins = 0;
        smp_lock_stats[i].rwlock_read_acquisitions = 0;
        smp_lock_stats[i].rwlock_write_acquisitions = 0;
        smp_lock_stats[i].rwlock_read_contentions = 0;
        smp_lock_stats[i].rwlock_write_contentions = 0;
    }
    
    printf("SMP lock statistics initialized\n");
}

/*
 * Print lock statistics for debugging
 */
void smp_lock_stats_print(void)
{
    int i;
    
    printf("SMP Lock Statistics:\n");
    for (i = 0; i < smp_get_numcpus(); i++) {
        printf("CPU %d:\n", i);
        printf("  Spinlock acq: %lu, cont: %lu, spins: %lu\n",
               smp_lock_stats[i].spinlock_acquisitions,
               smp_lock_stats[i].spinlock_contentions,
               smp_lock_stats[i].spinlock_spins);
        printf("  RWLock r_acq: %lu, w_acq: %lu, r_cont: %lu, w_cont: %lu\n",
               smp_lock_stats[i].rwlock_read_acquisitions,
               smp_lock_stats[i].rwlock_write_acquisitions,
               smp_lock_stats[i].rwlock_read_contentions,
               smp_lock_stats[i].rwlock_write_contentions);
    }
}

#endif /* NCPUS > 1 */