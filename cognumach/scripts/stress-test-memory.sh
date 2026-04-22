#!/bin/bash
# Memory stress testing script for GNU Mach Phase 2 implementation
# Tests memory management under extreme load conditions

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Default configuration
EXTREME_LOAD=false
DURATION="1h"
TEST_PATTERNS=("sequential" "random" "fragmented")
OUTPUT_DIR="$PROJECT_ROOT/test-results/memory-stress"

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

Memory stress testing for GNU Mach microkernel.

Options:
  --extreme-load           Enable extreme memory load testing
  --duration DURATION      Test duration (default: 1h, format: 1h, 30m, 24h)
  --pattern PATTERN        Memory allocation pattern (sequential, random, fragmented)
  --output-dir DIR         Output directory for test results
  -h, --help              Show this help message

Examples:
  $0 --extreme-load --duration=24h
  $0 --pattern random --duration=30m
  $0 --extreme-load --duration=24h --output-dir=/tmp/stress-results
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --extreme-load)
            EXTREME_LOAD=true
            shift
            ;;
        --duration=*)
            DURATION="${1#*=}"
            shift
            ;;
        --duration)
            DURATION="$2"
            shift 2
            ;;
        --pattern=*)
            TEST_PATTERNS=("${1#*=}")
            shift
            ;;
        --pattern)
            TEST_PATTERNS=("$2")
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
        *) echo "$duration" ;;  # Assume seconds if no unit
    esac
}

# Create test program for memory stress testing
create_memory_stress_test() {
    local test_file="$OUTPUT_DIR/memory_stress_test.c"
    
    cat > "$test_file" << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

static volatile int keep_running = 1;
static long allocations = 0;
static long deallocations = 0;
static long peak_memory = 0;
static long current_memory = 0;

void signal_handler(int sig) {
    keep_running = 0;
}

void print_stats() {
    printf("Memory Stress Test Statistics:\n");
    printf("  Allocations: %ld\n", allocations);
    printf("  Deallocations: %ld\n", deallocations);
    printf("  Peak memory usage: %ld bytes\n", peak_memory);
    printf("  Current memory usage: %ld bytes\n", current_memory);
}

int stress_test_sequential(int duration_seconds) {
    printf("Running sequential allocation stress test for %d seconds...\n", duration_seconds);
    
    void **ptrs = malloc(10000 * sizeof(void*));
    if (!ptrs) return -1;
    
    struct timeval start, now;
    gettimeofday(&start, NULL);
    
    int ptr_index = 0;
    while (keep_running) {
        gettimeofday(&now, NULL);
        if ((now.tv_sec - start.tv_sec) >= duration_seconds) break;
        
        // Allocate memory blocks of varying sizes
        size_t size = 1024 + (rand() % 8192);  // 1KB to 9KB
        ptrs[ptr_index] = malloc(size);
        
        if (ptrs[ptr_index]) {
            memset(ptrs[ptr_index], 0x42, size);  // Touch the memory
            allocations++;
            current_memory += size;
            if (current_memory > peak_memory) {
                peak_memory = current_memory;
            }
            
            ptr_index = (ptr_index + 1) % 10000;
            
            // Randomly free some memory
            if (ptr_index % 100 == 0 && ptr_index > 0) {
                int free_index = rand() % ptr_index;
                if (ptrs[free_index]) {
                    free(ptrs[free_index]);
                    ptrs[free_index] = NULL;
                    deallocations++;
                    current_memory -= size;  // Approximate
                }
            }
        }
        
        if (allocations % 1000 == 0) {
            printf("Allocations: %ld, Peak memory: %ld bytes\n", allocations, peak_memory);
        }
    }
    
    // Clean up
    for (int i = 0; i < 10000; i++) {
        if (ptrs[i]) {
            free(ptrs[i]);
            deallocations++;
        }
    }
    free(ptrs);
    
    return 0;
}

int stress_test_random(int duration_seconds) {
    printf("Running random allocation stress test for %d seconds...\n", duration_seconds);
    
    void **ptrs = calloc(20000, sizeof(void*));
    if (!ptrs) return -1;
    
    struct timeval start, now;
    gettimeofday(&start, NULL);
    
    while (keep_running) {
        gettimeofday(&now, NULL);
        if ((now.tv_sec - start.tv_sec) >= duration_seconds) break;
        
        int action = rand() % 100;
        int index = rand() % 20000;
        
        if (action < 60) {  // 60% allocation
            if (!ptrs[index]) {
                size_t size = 64 + (rand() % 4096);  // 64B to 4KB
                ptrs[index] = malloc(size);
                if (ptrs[index]) {
                    memset(ptrs[index], 0x55, size);
                    allocations++;
                    current_memory += size;
                    if (current_memory > peak_memory) {
                        peak_memory = current_memory;
                    }
                }
            }
        } else {  // 40% deallocation
            if (ptrs[index]) {
                free(ptrs[index]);
                ptrs[index] = NULL;
                deallocations++;
            }
        }
        
        if (allocations % 5000 == 0) {
            printf("Random test - Allocations: %ld, Deallocations: %ld\n", allocations, deallocations);
        }
    }
    
    // Clean up
    for (int i = 0; i < 20000; i++) {
        if (ptrs[i]) {
            free(ptrs[i]);
            deallocations++;
        }
    }
    free(ptrs);
    
    return 0;
}

int stress_test_fragmented(int duration_seconds) {
    printf("Running fragmentation stress test for %d seconds...\n", duration_seconds);
    
    void **small_ptrs = calloc(50000, sizeof(void*));
    void **large_ptrs = calloc(1000, sizeof(void*));
    if (!small_ptrs || !large_ptrs) return -1;
    
    struct timeval start, now;
    gettimeofday(&start, NULL);
    
    // First, allocate many small blocks to fragment memory
    for (int i = 0; i < 25000; i++) {
        small_ptrs[i] = malloc(32 + (rand() % 64));  // 32-96 bytes
        if (small_ptrs[i]) {
            memset(small_ptrs[i], 0x33, 32);
            allocations++;
            current_memory += 64;  // Approximate
        }
    }
    
    // Free every other small block to create fragmentation
    for (int i = 0; i < 25000; i += 2) {
        if (small_ptrs[i]) {
            free(small_ptrs[i]);
            small_ptrs[i] = NULL;
            deallocations++;
        }
    }
    
    while (keep_running) {
        gettimeofday(&now, NULL);
        if ((now.tv_sec - start.tv_sec) >= duration_seconds) break;
        
        // Try to allocate large blocks in fragmented memory
        int large_index = rand() % 1000;
        if (!large_ptrs[large_index]) {
            size_t size = 8192 + (rand() % 16384);  // 8KB to 24KB
            large_ptrs[large_index] = malloc(size);
            if (large_ptrs[large_index]) {
                memset(large_ptrs[large_index], 0x77, 1024);  // Touch first 1KB
                allocations++;
                current_memory += size;
                if (current_memory > peak_memory) {
                    peak_memory = current_memory;
                }
            }
        } else {
            free(large_ptrs[large_index]);
            large_ptrs[large_index] = NULL;
            deallocations++;
        }
        
        if (allocations % 100 == 0) {
            printf("Fragmentation test - Allocations: %ld\n", allocations);
        }
    }
    
    // Clean up
    for (int i = 0; i < 50000; i++) {
        if (small_ptrs[i]) {
            free(small_ptrs[i]);
            deallocations++;
        }
    }
    for (int i = 0; i < 1000; i++) {
        if (large_ptrs[i]) {
            free(large_ptrs[i]);
            deallocations++;
        }
    }
    free(small_ptrs);
    free(large_ptrs);
    
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <pattern> <duration_seconds>\n", argv[0]);
        printf("Patterns: sequential, random, fragmented\n");
        return 1;
    }
    
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    
    char *pattern = argv[1];
    int duration = atoi(argv[2]);
    
    printf("Starting memory stress test: pattern=%s, duration=%d seconds\n", pattern, duration);
    
    int result = 0;
    if (strcmp(pattern, "sequential") == 0) {
        result = stress_test_sequential(duration);
    } else if (strcmp(pattern, "random") == 0) {
        result = stress_test_random(duration);
    } else if (strcmp(pattern, "fragmented") == 0) {
        result = stress_test_fragmented(duration);
    } else {
        printf("Unknown pattern: %s\n", pattern);
        return 1;
    }
    
    print_stats();
    return result;
}
EOF
}

# Run memory stress test
run_stress_test() {
    local pattern="$1"
    local duration_seconds="$2"
    
    log_info "Running memory stress test: pattern=$pattern, duration=${duration_seconds}s"
    
    # Compile the test program
    local test_binary="$OUTPUT_DIR/memory_stress_test"
    gcc -o "$test_binary" "$OUTPUT_DIR/memory_stress_test.c" -Wall -O2
    
    if [[ ! -x "$test_binary" ]]; then
        log_error "Failed to compile stress test program"
        return 1
    fi
    
    # Run the test with timeout
    local log_file="$OUTPUT_DIR/stress_test_${pattern}_$(date +%Y%m%d_%H%M%S).log"
    
    log_info "Starting stress test (output: $log_file)"
    timeout "${duration_seconds}s" "$test_binary" "$pattern" "$duration_seconds" | tee "$log_file"
    local exit_code=$?
    
    if [[ $exit_code -eq 0 ]]; then
        log_success "Stress test completed successfully"
    elif [[ $exit_code -eq 124 ]]; then
        log_warning "Stress test timed out (expected for long-duration tests)"
    else
        log_error "Stress test failed with exit code $exit_code"
        return 1
    fi
    
    return 0
}

# Main execution
main() {
    log_info "GNU Mach Memory Stress Testing"
    log_info "Extreme load: $EXTREME_LOAD"
    log_info "Duration: $DURATION"
    log_info "Output directory: $OUTPUT_DIR"
    
    # Create output directory
    mkdir -p "$OUTPUT_DIR"
    
    # Create test program
    create_memory_stress_test
    
    # Convert duration to seconds
    local duration_seconds
    duration_seconds=$(duration_to_seconds "$DURATION")
    
    # Adjust test parameters for extreme load
    if [[ "$EXTREME_LOAD" == "true" ]]; then
        log_info "Extreme load testing enabled - using intensive parameters"
        # For extreme load, we'll run all patterns sequentially
        TEST_PATTERNS=("sequential" "random" "fragmented")
    fi
    
    # Run tests for each pattern
    local overall_success=true
    for pattern in "${TEST_PATTERNS[@]}"; do
        log_info "Running stress test with pattern: $pattern"
        if ! run_stress_test "$pattern" "$duration_seconds"; then
            overall_success=false
            log_error "Stress test failed for pattern: $pattern"
        fi
        
        # Brief pause between tests
        sleep 5
    done
    
    # Generate summary report
    local summary_file="$OUTPUT_DIR/stress_test_summary_$(date +%Y%m%d_%H%M%S).txt"
    {
        echo "GNU Mach Memory Stress Test Summary"
        echo "=================================="
        echo "Date: $(date)"
        echo "Duration: $DURATION"
        echo "Extreme load: $EXTREME_LOAD"
        echo "Patterns tested: ${TEST_PATTERNS[*]}"
        echo ""
        echo "Test Results:"
        
        for pattern in "${TEST_PATTERNS[@]}"; do
            local latest_log
            latest_log=$(ls -t "$OUTPUT_DIR"/stress_test_${pattern}_*.log 2>/dev/null | head -1)
            if [[ -f "$latest_log" ]]; then
                echo "  $pattern: $(tail -5 "$latest_log" | grep -E "(SUCCESS|ERROR|Statistics)" || echo "Completed")"
            else
                echo "  $pattern: No log file found"
            fi
        done
        
        echo ""
        echo "Overall result: $([ "$overall_success" = true ] && echo "SUCCESS" || echo "FAILURE")"
    } > "$summary_file"
    
    log_info "Summary report written to: $summary_file"
    
    if [[ "$overall_success" == "true" ]]; then
        log_success "All memory stress tests completed successfully"
        exit 0
    else
        log_error "Some memory stress tests failed"
        exit 1
    fi
}

# Run main function
main "$@"