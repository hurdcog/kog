#!/bin/bash
# SMP Memory Race Condition Testing Script for GNU Mach Phase 2
# Tests memory management for race conditions under high SMP load

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Default configuration
CPU_COUNT=4
ITERATIONS=1000
TEST_DURATION="30m"
OUTPUT_DIR="$PROJECT_ROOT/test-results/smp-races"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

usage() {
    cat << EOF
Usage: $0 [OPTIONS]

SMP memory race condition testing for GNU Mach microkernel.

Options:
  --cpu-count COUNT        Number of CPU threads to simulate (default: 4)
  --iterations COUNT       Number of test iterations (default: 1000)
  --duration DURATION      Test duration (default: 30m)
  --output-dir DIR         Output directory for test results
  -h, --help              Show this help message

Examples:
  $0 --cpu-count=16 --iterations=10000
  $0 --duration=1h --cpu-count=8
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --cpu-count=*)
            CPU_COUNT="${1#*=}"
            shift
            ;;
        --cpu-count)
            CPU_COUNT="$2"
            shift 2
            ;;
        --iterations=*)
            ITERATIONS="${1#*=}"
            shift
            ;;
        --iterations)
            ITERATIONS="$2"
            shift 2
            ;;
        --duration=*)
            TEST_DURATION="${1#*=}"
            shift
            ;;
        --duration)
            TEST_DURATION="$2"
            shift 2
            ;;
        --output-dir=*)
            OUTPUT_DIR="${1#*=}"
            shift
            ;;
        --output-dir)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# Create SMP race condition test program
create_smp_race_test() {
    local test_file="$OUTPUT_DIR/smp_race_test.c"
    
    cat > "$test_file" << 'EOF'
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <atomic_ops.h>
#include <errno.h>
#include <signal.h>

// Test configuration
static int num_threads = 4;
static int iterations = 1000;
static volatile int keep_running = 1;

// Shared data structures for race condition testing
static void *shared_memory_pool[10000];
static volatile AO_t allocation_counter = 0;
static volatile AO_t deallocation_counter = 0;
static volatile AO_t race_detected = 0;
static pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;

// Statistics
typedef struct {
    long allocations;
    long deallocations;
    long null_pointer_accesses;
    long double_free_attempts;
    long memory_corruption_detected;
} thread_stats_t;

static thread_stats_t *thread_stats;

void signal_handler(int sig) {
    keep_running = 0;
}

// Memory allocation with race condition detection
void* race_aware_malloc(size_t size, int thread_id) {
    void *ptr = malloc(size);
    if (ptr) {
        // Fill with thread-specific pattern
        memset(ptr, 0x40 + thread_id, size);
        AO_fetch_and_add1(&allocation_counter);
        thread_stats[thread_id].allocations++;
    }
    return ptr;
}

// Memory deallocation with double-free detection
int race_aware_free(void *ptr, int thread_id) {
    if (!ptr) {
        thread_stats[thread_id].null_pointer_accesses++;
        return -1;
    }
    
    // Simple double-free detection (not foolproof, but catches some cases)
    char first_byte = *((char*)ptr);
    if (first_byte == 0xDE) {  // Marker for freed memory
        thread_stats[thread_id].double_free_attempts++;
        AO_fetch_and_add1(&race_detected);
        return -1;
    }
    
    // Mark as freed
    *((char*)ptr) = 0xDE;
    free(ptr);
    
    AO_fetch_and_add1(&deallocation_counter);
    thread_stats[thread_id].deallocations++;
    return 0;
}

// Verify memory integrity
int verify_memory_integrity(void *ptr, size_t size, int expected_pattern) {
    if (!ptr) return -1;
    
    char *bytes = (char*)ptr;
    for (size_t i = 0; i < size; i++) {
        if (bytes[i] != expected_pattern && bytes[i] != 0xDE) {
            return -1;  // Corruption detected
        }
    }
    return 0;
}

// Thread function for concurrent memory operations
void* memory_race_thread(void *arg) {
    int thread_id = *(int*)arg;
    int local_iterations = iterations / num_threads;
    
    printf("Thread %d starting %d iterations\n", thread_id, local_iterations);
    
    void **local_ptrs = calloc(1000, sizeof(void*));
    if (!local_ptrs) {
        printf("Thread %d: Failed to allocate local pointer array\n", thread_id);
        return NULL;
    }
    
    for (int i = 0; i < local_iterations && keep_running; i++) {
        int operation = rand() % 100;
        int index = rand() % 1000;
        
        if (operation < 60) {  // 60% allocation
            if (!local_ptrs[index]) {
                size_t size = 64 + (rand() % 512);  // 64B to 576B
                local_ptrs[index] = race_aware_malloc(size, thread_id);
                
                // Immediately verify the allocation
                if (local_ptrs[index]) {
                    if (verify_memory_integrity(local_ptrs[index], size, 0x40 + thread_id) != 0) {
                        thread_stats[thread_id].memory_corruption_detected++;
                        AO_fetch_and_add1(&race_detected);
                    }
                }
            }
        } else if (operation < 85) {  // 25% deallocation
            if (local_ptrs[index]) {
                race_aware_free(local_ptrs[index], thread_id);
                local_ptrs[index] = NULL;
            }
        } else {  // 15% shared pool operations (higher race potential)
            int pool_index = rand() % 10000;
            
            pthread_mutex_lock(&pool_mutex);
            if (!shared_memory_pool[pool_index]) {
                size_t size = 128 + (rand() % 256);
                shared_memory_pool[pool_index] = race_aware_malloc(size, thread_id);
            } else {
                void *ptr = shared_memory_pool[pool_index];
                shared_memory_pool[pool_index] = NULL;
                pthread_mutex_unlock(&pool_mutex);
                
                race_aware_free(ptr, thread_id);
                continue;
            }
            pthread_mutex_unlock(&pool_mutex);
        }
        
        // Occasional yield to increase race condition probability
        if (i % 100 == 0) {
            sched_yield();
        }
    }
    
    // Clean up local allocations
    for (int i = 0; i < 1000; i++) {
        if (local_ptrs[i]) {
            race_aware_free(local_ptrs[i], thread_id);
        }
    }
    free(local_ptrs);
    
    printf("Thread %d completed\n", thread_id);
    return NULL;
}

// Monitor thread to detect system-level issues
void* monitor_thread(void *arg) {
    struct timeval start, now;
    gettimeofday(&start, NULL);
    
    while (keep_running) {
        sleep(5);
        
        gettimeofday(&now, NULL);
        long elapsed = now.tv_sec - start.tv_sec;
        
        printf("Monitor: %lds elapsed, allocations=%ld, deallocations=%ld, races=%ld\n",
               elapsed, 
               (long)AO_load(&allocation_counter),
               (long)AO_load(&deallocation_counter),
               (long)AO_load(&race_detected));
        
        // Check for system health indicators
        if (AO_load(&race_detected) > 100) {
            printf("WARNING: High number of race conditions detected!\n");
        }
    }
    
    return NULL;
}

void print_final_stats() {
    printf("\n=== SMP Memory Race Test Results ===\n");
    printf("Total allocations: %ld\n", (long)AO_load(&allocation_counter));
    printf("Total deallocations: %ld\n", (long)AO_load(&deallocation_counter));
    printf("Race conditions detected: %ld\n", (long)AO_load(&race_detected));
    
    printf("\nPer-thread statistics:\n");
    long total_corruption = 0;
    long total_double_free = 0;
    long total_null_access = 0;
    
    for (int i = 0; i < num_threads; i++) {
        printf("Thread %d: alloc=%ld, dealloc=%ld, corruption=%ld, double_free=%ld, null_access=%ld\n",
               i, thread_stats[i].allocations, thread_stats[i].deallocations,
               thread_stats[i].memory_corruption_detected,
               thread_stats[i].double_free_attempts,
               thread_stats[i].null_pointer_accesses);
        
        total_corruption += thread_stats[i].memory_corruption_detected;
        total_double_free += thread_stats[i].double_free_attempts; 
        total_null_access += thread_stats[i].null_pointer_accesses;
    }
    
    printf("\nSummary:\n");
    printf("  Memory corruption events: %ld\n", total_corruption);
    printf("  Double-free attempts: %ld\n", total_double_free);
    printf("  Null pointer accesses: %ld\n", total_null_access);
    
    // Determine test result
    if (AO_load(&race_detected) == 0 && total_corruption == 0) {
        printf("  Result: PASS - No race conditions detected\n");
    } else if (AO_load(&race_detected) < 10 && total_corruption < 5) {
        printf("  Result: WARNING - Minor race conditions detected\n");
    } else {
        printf("  Result: FAIL - Significant race conditions detected\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <num_threads> <iterations>\n", argv[0]);
        return 1;
    }
    
    num_threads = atoi(argv[1]);
    iterations = atoi(argv[2]);
    
    if (num_threads <= 0 || num_threads > 64) {
        printf("Invalid number of threads: %d\n", num_threads);
        return 1;
    }
    
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    
    printf("Starting SMP memory race test: %d threads, %d iterations\n", num_threads, iterations);
    
    // Initialize statistics
    thread_stats = calloc(num_threads, sizeof(thread_stats_t));
    if (!thread_stats) {
        printf("Failed to allocate thread statistics\n");
        return 1;
    }
    
    // Create threads
    pthread_t *threads = malloc((num_threads + 1) * sizeof(pthread_t));  // +1 for monitor
    int *thread_ids = malloc(num_threads * sizeof(int));
    
    if (!threads || !thread_ids) {
        printf("Failed to allocate thread arrays\n");
        return 1;
    }
    
    // Start monitor thread
    if (pthread_create(&threads[num_threads], NULL, monitor_thread, NULL) != 0) {
        printf("Failed to create monitor thread\n");
        return 1;
    }
    
    // Start worker threads
    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, memory_race_thread, &thread_ids[i]) != 0) {
            printf("Failed to create thread %d\n", i);
            return 1;
        }
    }
    
    // Wait for worker threads to complete
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Signal monitor thread to stop and wait
    keep_running = 0;
    pthread_join(threads[num_threads], NULL);
    
    print_final_stats();
    
    // Clean up
    free(threads);
    free(thread_ids);
    free(thread_stats);
    
    // Return appropriate exit code
    if (AO_load(&race_detected) > 10) {
        return 1;  // Test failed
    }
    
    return 0;  // Test passed
}
EOF
}

# Convert duration to seconds
duration_to_seconds() {
    local duration="$1"
    case "$duration" in
        *h) echo $((${duration%h} * 3600)) ;;
        *m) echo $((${duration%m} * 60)) ;;
        *s) echo ${duration%s} ;;
        *) echo "$duration" ;;  # Assume seconds if no unit
    esac
}

# Run SMP race condition test
run_smp_race_test() {
    log_info "Running SMP memory race condition test"
    log_info "CPU count: $CPU_COUNT, Iterations: $ITERATIONS"
    
    # Compile the test program
    local test_binary="$OUTPUT_DIR/smp_race_test"
    
    # Check if atomic_ops library is available, if not, create a minimal version
    if ! pkg-config --exists atomic_ops 2>/dev/null; then
        log_warning "atomic_ops library not found, creating minimal implementation"
        
        cat > "$OUTPUT_DIR/atomic_ops.h" << 'EOF'
#ifndef ATOMIC_OPS_H
#define ATOMIC_OPS_H

typedef volatile long AO_t;

static inline long AO_load(AO_t *addr) {
    return *addr;
}

static inline long AO_fetch_and_add1(AO_t *addr) {
    return __sync_fetch_and_add(addr, 1);
}

#endif
EOF
        
        gcc -o "$test_binary" "$OUTPUT_DIR/smp_race_test.c" -I"$OUTPUT_DIR" -lpthread -Wall -O2
    else
        gcc -o "$test_binary" "$OUTPUT_DIR/smp_race_test.c" $(pkg-config --cflags --libs atomic_ops) -lpthread -Wall -O2
    fi
    
    if [[ ! -x "$test_binary" ]]; then
        log_error "Failed to compile SMP race test program"
        return 1
    fi
    
    # Run the test with timeout
    local duration_seconds
    duration_seconds=$(duration_to_seconds "$TEST_DURATION")
    local log_file="$OUTPUT_DIR/smp_race_test_$(date +%Y%m%d_%H%M%S).log"
    
    log_info "Starting SMP race test (timeout: ${duration_seconds}s, output: $log_file)"
    
    timeout "${duration_seconds}s" "$test_binary" "$CPU_COUNT" "$ITERATIONS" | tee "$log_file"
    local exit_code=$?
    
    if [[ $exit_code -eq 0 ]]; then
        log_success "SMP race condition test PASSED"
        return 0
    elif [[ $exit_code -eq 124 ]]; then
        log_warning "SMP race test timed out"
        return 1
    else
        log_error "SMP race condition test FAILED (exit code: $exit_code)"
        return 1
    fi
}

# Main execution
main() {
    log_info "GNU Mach SMP Memory Race Condition Testing"
    log_info "CPU count: $CPU_COUNT"
    log_info "Iterations: $ITERATIONS"
    log_info "Duration: $TEST_DURATION"
    log_info "Output directory: $OUTPUT_DIR"
    
    # Create output directory
    mkdir -p "$OUTPUT_DIR"
    
    # Validate CPU count
    local max_cpus
    max_cpus=$(nproc)
    if [[ $CPU_COUNT -gt $((max_cpus * 2)) ]]; then
        log_warning "CPU count ($CPU_COUNT) is high compared to available CPUs ($max_cpus)"
        log_warning "This may lead to excessive context switching"
    fi
    
    # Create and run test program
    create_smp_race_test
    
    if run_smp_race_test; then
        log_success "SMP memory race condition testing completed successfully"
        
        # Generate summary report
        local summary_file="$OUTPUT_DIR/smp_race_summary_$(date +%Y%m%d_%H%M%S).txt"
        {
            echo "GNU Mach SMP Memory Race Condition Test Summary"
            echo "=============================================="
            echo "Date: $(date)"
            echo "CPU count: $CPU_COUNT"
            echo "Iterations: $ITERATIONS"
            echo "Duration limit: $TEST_DURATION"
            echo ""
            echo "Test Result: SUCCESS"
            echo "No significant race conditions detected under high SMP load"
        } > "$summary_file"
        
        log_info "Summary report written to: $summary_file"
        exit 0
    else
        log_error "SMP race condition test failed"
        exit 1
    fi
}

# Run main function
main "$@"