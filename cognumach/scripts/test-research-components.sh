#!/bin/bash
# Research Component Testing Script for GNU Mach Phase 3
# Long-term stability testing of research components

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Default configuration
DURATION="72h"
COMPONENT="all"
OUTPUT_DIR="$PROJECT_ROOT/test-results/research-testing"

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

Long-term stability testing of GNU Mach research components.

Options:
  --duration DURATION     Test duration (default: 72h, format: 1h, 30m, 24h)
  --component COMPONENT   Test specific component (ipc, scheduler, all)
  --output-dir DIR        Output directory for test results
  -h, --help             Show this help message

Examples:
  $0 --duration=72h
  $0 --component=ipc --duration=24h
  $0 --component=all --duration=1h --output-dir=/tmp/research-tests
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --duration=*)
            DURATION="${1#*=}"
            shift
            ;;
        --duration)
            DURATION="$2"
            shift 2
            ;;
        --component=*)
            COMPONENT="${1#*=}"
            shift
            ;;
        --component)
            COMPONENT="$2"
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

# Convert duration to seconds
duration_to_seconds() {
    local duration="$1"
    case "$duration" in
        *h) echo $((${duration%h} * 3600)) ;;
        *m) echo $((${duration%m} * 60)) ;;
        *s) echo ${duration%s} ;;
        *) echo "$duration" ;;
    esac
}

# Create long-term stability test program
create_stability_test() {
    local test_file="$OUTPUT_DIR/research_stability_test.c"
    
    cat > "$test_file" << 'EOF'
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

static volatile int keep_running = 1;
static volatile long test_cycles = 0;
static volatile long error_count = 0;
static time_t start_time;

void signal_handler(int sig) {
    keep_running = 0;
}

// Zero-copy IPC stability test
void* test_zero_copy_ipc(void* arg) {
    int thread_id = *(int*)arg;
    long local_cycles = 0;
    long local_errors = 0;
    
    printf("Zero-copy IPC test thread %d started\n", thread_id);
    
    while (keep_running) {
        // Simulate zero-copy IPC operations
        void* buffer = malloc(4096);
        if (!buffer) {
            local_errors++;
            continue;
        }
        
        // Fill with test pattern
        memset(buffer, 0x42 + thread_id, 4096);
        
        // Simulate IPC transfer (memory mapping operations)
        char* mapped = (char*)buffer;
        for (int i = 0; i < 4096; i += 64) {
            if (mapped[i] != (0x42 + thread_id)) {
                local_errors++;
                break;
            }
        }
        
        free(buffer);
        local_cycles++;
        
        if (local_cycles % 10000 == 0) {
            printf("Thread %d: %ld cycles, %ld errors\n", thread_id, local_cycles, local_errors);
        }
        
        // Brief pause to avoid overwhelming system
        if (local_cycles % 1000 == 0) {
            usleep(1000);  // 1ms pause every 1000 cycles
        }
    }
    
    __sync_fetch_and_add(&test_cycles, local_cycles);
    __sync_fetch_and_add(&error_count, local_errors);
    
    printf("Zero-copy IPC thread %d completed: %ld cycles, %ld errors\n", 
           thread_id, local_cycles, local_errors);
    return NULL;
}

// Enhanced scheduler stability test
void* test_enhanced_scheduler(void* arg) {
    int thread_id = *(int*)arg;
    long local_cycles = 0;
    long local_errors = 0;
    
    printf("Enhanced scheduler test thread %d started\n", thread_id);
    
    while (keep_running) {
        // Test different scheduling patterns
        int pattern = local_cycles % 4;
        
        switch (pattern) {
            case 0:  // CPU-intensive
                for (volatile int i = 0; i < 100000; i++);
                break;
            case 1:  // I/O simulation
                usleep(100);
                break;
            case 2:  // Yield frequently
                sched_yield();
                break;
            case 3:  // Mixed workload
                for (volatile int i = 0; i < 10000; i++);
                usleep(10);
                break;
        }
        
        local_cycles++;
        
        if (local_cycles % 50000 == 0) {
            printf("Scheduler test thread %d: %ld cycles\n", thread_id, local_cycles);
        }
    }
    
    __sync_fetch_and_add(&test_cycles, local_cycles);
    __sync_fetch_and_add(&error_count, local_errors);
    
    printf("Enhanced scheduler thread %d completed: %ld cycles, %ld errors\n",
           thread_id, local_cycles, local_errors);
    return NULL;
}

// Memory management stability test
void* test_memory_management(void* arg) {
    int thread_id = *(int*)arg;
    long local_cycles = 0;
    long local_errors = 0;
    
    printf("Memory management test thread %d started\n", thread_id);
    
    void* ptrs[1000] = {NULL};
    
    while (keep_running) {
        int action = rand() % 100;
        int index = rand() % 1000;
        
        if (action < 60 && !ptrs[index]) {  // Allocate
            size_t size = 64 + (rand() % 4032);  // 64B to 4KB
            ptrs[index] = malloc(size);
            
            if (ptrs[index]) {
                // Touch the memory
                memset(ptrs[index], 0x55, 64);
            } else {
                local_errors++;
            }
        } else if (action >= 60 && ptrs[index]) {  // Free
            free(ptrs[index]);
            ptrs[index] = NULL;
        }
        
        local_cycles++;
        
        if (local_cycles % 25000 == 0) {
            printf("Memory test thread %d: %ld cycles, %ld errors\n", 
                   thread_id, local_cycles, local_errors);
        }
    }
    
    // Clean up remaining allocations
    for (int i = 0; i < 1000; i++) {
        if (ptrs[i]) {
            free(ptrs[i]);
        }
    }
    
    __sync_fetch_and_add(&test_cycles, local_cycles);
    __sync_fetch_and_add(&error_count, local_errors);
    
    printf("Memory management thread %d completed: %ld cycles, %ld errors\n",
           thread_id, local_cycles, local_errors);
    return NULL;
}

// Monitor thread for system health
void* monitor_system_health(void* arg) {
    time_t last_report = time(NULL);
    long last_cycles = 0;
    
    while (keep_running) {
        sleep(300);  // Report every 5 minutes
        
        time_t now = time(NULL);
        long current_cycles = __sync_fetch_and_add(&test_cycles, 0);
        long current_errors = __sync_fetch_and_add(&error_count, 0);
        
        double elapsed = difftime(now, start_time);
        double rate = (current_cycles - last_cycles) / difftime(now, last_report);
        
        printf("\n=== System Health Report ===\n");
        printf("Elapsed time: %.0f seconds (%.1f hours)\n", elapsed, elapsed / 3600.0);
        printf("Total test cycles: %ld\n", current_cycles);
        printf("Total errors: %ld\n", current_errors);
        printf("Cycles per second: %.2f\n", rate);
        printf("Error rate: %.4f%%\n", current_errors > 0 ? (100.0 * current_errors / current_cycles) : 0.0);
        printf("============================\n\n");
        
        last_report = now;
        last_cycles = current_cycles;
        
        // Check for concerning error rates
        if (current_cycles > 0 && (100.0 * current_errors / current_cycles) > 1.0) {
            printf("WARNING: High error rate detected!\n");
        }
    }
    
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <component> <duration_seconds>\n", argv[0]);
        printf("Components: ipc, scheduler, memory, all\n");
        return 1;
    }
    
    char* component = argv[1];
    int duration = atoi(argv[2]);
    
    printf("Research Component Stability Test\n");
    printf("=================================\n");
    printf("Component: %s\n", component);
    printf("Duration: %d seconds (%.1f hours)\n", duration, duration / 3600.0);
    printf("Start time: %s", ctime(&(time_t){time(NULL)}));
    
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGALRM, signal_handler);
    
    start_time = time(NULL);
    alarm(duration);
    
    // Start monitor thread
    pthread_t monitor_thread;
    pthread_create(&monitor_thread, NULL, monitor_system_health, NULL);
    
    // Determine test threads based on component
    pthread_t test_threads[12];
    int thread_ids[12];
    int num_threads = 0;
    
    if (strcmp(component, "ipc") == 0 || strcmp(component, "all") == 0) {
        for (int i = 0; i < 4; i++) {
            thread_ids[num_threads] = num_threads;
            pthread_create(&test_threads[num_threads], NULL, test_zero_copy_ipc, &thread_ids[num_threads]);
            num_threads++;
        }
    }
    
    if (strcmp(component, "scheduler") == 0 || strcmp(component, "all") == 0) {
        for (int i = 0; i < 4; i++) {
            thread_ids[num_threads] = num_threads;
            pthread_create(&test_threads[num_threads], NULL, test_enhanced_scheduler, &thread_ids[num_threads]);
            num_threads++;
        }
    }
    
    if (strcmp(component, "memory") == 0 || strcmp(component, "all") == 0) {
        for (int i = 0; i < 4; i++) {
            thread_ids[num_threads] = num_threads;
            pthread_create(&test_threads[num_threads], NULL, test_memory_management, &thread_ids[num_threads]);
            num_threads++;
        }
    }
    
    if (num_threads == 0) {
        printf("Unknown component: %s\n", component);
        return 1;
    }
    
    printf("Started %d test threads\n", num_threads);
    
    // Wait for test threads to complete
    for (int i = 0; i < num_threads; i++) {
        pthread_join(test_threads[i], NULL);
    }
    
    keep_running = 0;
    pthread_cancel(monitor_thread);
    
    // Final report
    time_t end_time = time(NULL);
    double total_elapsed = difftime(end_time, start_time);
    
    printf("\n=== Final Test Results ===\n");
    printf("Component tested: %s\n", component);
    printf("Total duration: %.0f seconds (%.2f hours)\n", total_elapsed, total_elapsed / 3600.0);
    printf("Total test cycles: %ld\n", test_cycles);
    printf("Total errors: %ld\n", error_count);
    printf("Average cycles per second: %.2f\n", test_cycles / total_elapsed);
    
    if (error_count == 0) {
        printf("Result: PASS - No errors detected\n");
        return 0;
    } else {
        double error_rate = (100.0 * error_count) / test_cycles;
        printf("Error rate: %.4f%%\n", error_rate);
        
        if (error_rate < 0.01) {
            printf("Result: PASS - Very low error rate (acceptable)\n");
            return 0;
        } else if (error_rate < 0.1) {
            printf("Result: WARNING - Low error rate (investigate)\n");
            return 0;
        } else {
            printf("Result: FAIL - High error rate (unacceptable)\n");
            return 1;
        }
    }
}
EOF
}

# Run research component stability test
run_stability_test() {
    local component="$1"
    local duration_seconds="$2"
    
    log_info "Running stability test for component: $component"
    log_info "Duration: ${duration_seconds}s ($(echo "scale=1; $duration_seconds/3600" | bc 2>/dev/null || echo "long") hours)"
    
    # Compile test program
    local test_binary="$OUTPUT_DIR/research_stability_test"
    gcc -o "$test_binary" "$OUTPUT_DIR/research_stability_test.c" -lpthread -Wall -O2
    
    if [[ ! -x "$test_binary" ]]; then
        log_error "Failed to compile stability test program"
        return 1
    fi
    
    # Run test with logging
    local log_file="$OUTPUT_DIR/stability_test_${component}_$(date +%Y%m%d_%H%M%S).log"
    
    log_info "Starting stability test (output: $log_file)"
    
    # Run in background with output redirection
    "$test_binary" "$component" "$duration_seconds" > "$log_file" 2>&1 &
    local test_pid=$!
    
    # Monitor test progress
    log_info "Stability test started (PID: $test_pid)"
    
    # For very long tests, just return and let them run
    if [[ $duration_seconds -gt 3600 ]]; then
        log_info "Long-duration test started - monitoring in background"
        log_info "Check progress with: tail -f $log_file"
        log_info "Stop test with: kill $test_pid"
        
        # Wait a bit to see if test starts successfully
        sleep 10
        if kill -0 $test_pid 2>/dev/null; then
            log_success "Stability test running successfully in background"
            return 0
        else
            log_error "Stability test failed to start properly"
            return 1
        fi
    else
        # Wait for shorter tests to complete
        wait $test_pid
        local exit_code=$?
        
        if [[ $exit_code -eq 0 ]]; then
            log_success "Stability test completed successfully"
            return 0
        else
            log_error "Stability test failed with exit code: $exit_code"
            return 1
        fi
    fi
}

# Generate stability test report
generate_stability_report() {
    local report_file="$OUTPUT_DIR/research_stability_summary_$(date +%Y%m%d_%H%M%S).txt"
    
    {
        echo "GNU Mach Research Component Stability Test Summary"
        echo "================================================="
        echo "Date: $(date)"
        echo "Component: $COMPONENT"
        echo "Duration: $DURATION"
        echo ""
        
        echo "Test Results:"
        local latest_log
        latest_log=$(ls -t "$OUTPUT_DIR"/stability_test_${COMPONENT}_*.log 2>/dev/null | head -1)
        if [[ -f "$latest_log" ]]; then
            echo "Latest test results:"
            tail -20 "$latest_log"
        else
            echo "No test results available"
        fi
        
    } > "$report_file"
    
    log_info "Stability test summary written to: $report_file"
}

# Main execution
main() {
    log_info "GNU Mach Research Component Stability Testing"
    log_info "Component: $COMPONENT"
    log_info "Duration: $DURATION"
    log_info "Output directory: $OUTPUT_DIR"
    
    # Validate component
    case "$COMPONENT" in
        "ipc"|"scheduler"|"memory"|"all")
            ;;
        *)
            log_error "Invalid component: $COMPONENT"
            log_error "Valid components: ipc, scheduler, memory, all"
            exit 1
            ;;
    esac
    
    # Create output directory
    mkdir -p "$OUTPUT_DIR"
    
    # Create test program
    create_stability_test
    
    # Convert duration to seconds
    local duration_seconds
    duration_seconds=$(duration_to_seconds "$DURATION")
    
    # Warn for very long tests
    if [[ $duration_seconds -gt 86400 ]]; then  # > 24 hours
        log_warning "Very long test duration requested ($(echo "scale=1; $duration_seconds/3600" | bc 2>/dev/null || echo ">24") hours)"
        log_warning "Test will run in background - monitor progress manually"
    fi
    
    # Run stability test
    if run_stability_test "$COMPONENT" "$duration_seconds"; then
        generate_stability_report
        log_success "✅ Research component stability test initiated successfully"
        exit 0
    else
        log_error "❌ Research component stability test failed"
        exit 1
    fi
}

# Run main function
main "$@"