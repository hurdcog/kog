#!/bin/bash
# Performance Target Validation Script for GNU Mach Phase 3
# Validates that performance benchmarks meet or exceed targets

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Default configuration
ALL_BENCHMARKS=false
BENCHMARK_TYPE="basic"
OUTPUT_DIR="$PROJECT_ROOT/test-results/performance"
BASELINE_FILE="$PROJECT_ROOT/performance-baselines.txt"

# Performance targets (baseline values)
declare -A PERFORMANCE_TARGETS=(
    ["ipc_latency_us"]=50        # IPC latency in microseconds
    ["context_switch_us"]=10     # Context switch time in microseconds  
    ["memory_alloc_ns"]=5000     # Memory allocation time in nanoseconds
    ["page_fault_us"]=100        # Page fault handling time in microseconds
    ["throughput_ops_sec"]=10000 # Operations per second
)

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

Validate performance benchmarks against targets for GNU Mach.

Options:
  --all-benchmarks        Run all available benchmarks
  --benchmark TYPE        Run specific benchmark type (ipc, memory, context, all)
  --output-dir DIR        Output directory for results
  --baseline-file FILE    Baseline performance targets file
  -h, --help             Show this help message

Examples:
  $0 --all-benchmarks
  $0 --benchmark ipc
  $0 --all-benchmarks --output-dir=/tmp/perf-results
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --all-benchmarks)
            ALL_BENCHMARKS=true
            BENCHMARK_TYPE="all"
            shift
            ;;
        --benchmark=*)
            BENCHMARK_TYPE="${1#*=}"
            shift
            ;;
        --benchmark)
            BENCHMARK_TYPE="$2"
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
        --baseline-file=*)
            BASELINE_FILE="${1#*=}"
            shift
            ;;
        --baseline-file)
            BASELINE_FILE="$2"
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

# Create performance benchmark program
create_perf_benchmark() {
    local bench_file="$OUTPUT_DIR/performance_benchmark.c"
    
    cat > "$bench_file" << 'EOF'
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sched.h>

// High resolution timing
static inline uint64_t get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

static inline uint64_t get_time_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000ULL + tv.tv_usec;
}

// IPC Latency Benchmark
typedef struct {
    int pipe_fd[2];
    char data[64];
} ipc_test_data_t;

void* ipc_sender(void* arg) {
    ipc_test_data_t* data = (ipc_test_data_t*)arg;
    
    for (int i = 0; i < 1000; i++) {
        write(data->pipe_fd[1], data->data, sizeof(data->data));
    }
    return NULL;
}

void* ipc_receiver(void* arg) {
    ipc_test_data_t* data = (ipc_test_data_t*)arg;
    char buffer[64];
    
    for (int i = 0; i < 1000; i++) {
        read(data->pipe_fd[0], buffer, sizeof(buffer));
    }
    return NULL;
}

double benchmark_ipc_latency() {
    printf("Running IPC latency benchmark...\n");
    
    ipc_test_data_t test_data;
    if (pipe(test_data.pipe_fd) == -1) {
        return -1.0;
    }
    
    memset(test_data.data, 0x42, sizeof(test_data.data));
    
    pthread_t sender_thread, receiver_thread;
    
    uint64_t start_time = get_time_us();
    
    pthread_create(&receiver_thread, NULL, ipc_receiver, &test_data);
    pthread_create(&sender_thread, NULL, ipc_sender, &test_data);
    
    pthread_join(sender_thread, NULL);
    pthread_join(receiver_thread, NULL);
    
    uint64_t end_time = get_time_us();
    
    close(test_data.pipe_fd[0]);
    close(test_data.pipe_fd[1]);
    
    double avg_latency = (double)(end_time - start_time) / 1000.0;  // microseconds per operation
    printf("IPC latency: %.2f microseconds per round-trip\n", avg_latency);
    
    return avg_latency;
}

// Context Switch Benchmark
volatile int context_switch_counter = 0;

void* context_switch_thread(void* arg) {
    int thread_id = *(int*)arg;
    
    for (int i = 0; i < 10000; i++) {
        context_switch_counter++;
        sched_yield();
    }
    
    return NULL;
}

double benchmark_context_switch() {
    printf("Running context switch benchmark...\n");
    
    const int num_threads = 4;
    pthread_t threads[num_threads];
    int thread_ids[num_threads];
    
    context_switch_counter = 0;
    
    uint64_t start_time = get_time_us();
    
    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, context_switch_thread, &thread_ids[i]);
    }
    
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    uint64_t end_time = get_time_us();
    
    double avg_context_switch = (double)(end_time - start_time) / context_switch_counter;
    printf("Context switch time: %.2f microseconds per switch\n", avg_context_switch);
    
    return avg_context_switch;
}

// Memory Allocation Benchmark
double benchmark_memory_allocation() {
    printf("Running memory allocation benchmark...\n");
    
    const int iterations = 10000;
    void* ptrs[iterations];
    
    uint64_t start_time = get_time_ns();
    
    for (int i = 0; i < iterations; i++) {
        size_t size = 1024 + (rand() % 4096);  // 1KB to 5KB
        ptrs[i] = malloc(size);
        if (ptrs[i]) {
            // Touch the memory to ensure it's actually allocated
            memset(ptrs[i], 0x55, 64);
        }
    }
    
    uint64_t end_time = get_time_ns();
    
    // Free all allocations
    for (int i = 0; i < iterations; i++) {
        if (ptrs[i]) {
            free(ptrs[i]);
        }
    }
    
    double avg_alloc_time = (double)(end_time - start_time) / iterations;
    printf("Memory allocation time: %.2f nanoseconds per allocation\n", avg_alloc_time);
    
    return avg_alloc_time;
}

// Page Fault Benchmark (simulated)
double benchmark_page_fault() {
    printf("Running page fault benchmark...\n");
    
    const size_t total_size = 100 * 1024 * 1024;  // 100MB
    const size_t page_size = 4096;
    const int iterations = 1000;
    
    char* memory = mmap(NULL, total_size, PROT_READ | PROT_WRITE, 
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (memory == MAP_FAILED) {
        printf("Failed to mmap memory\n");
        return -1.0;
    }
    
    uint64_t start_time = get_time_us();
    
    for (int i = 0; i < iterations; i++) {
        size_t offset = (rand() % (total_size / page_size)) * page_size;
        memory[offset] = 0x77;  // Touch page to force allocation
    }
    
    uint64_t end_time = get_time_us();
    
    munmap(memory, total_size);
    
    double avg_page_fault = (double)(end_time - start_time) / iterations;
    printf("Page fault handling: %.2f microseconds per fault\n", avg_page_fault);
    
    return avg_page_fault;
}

// Throughput benchmark
double benchmark_throughput() {
    printf("Running throughput benchmark...\n");
    
    const int duration_seconds = 5;
    volatile long operations = 0;
    
    uint64_t start_time = get_time_us();
    uint64_t end_time = start_time + (duration_seconds * 1000000ULL);
    
    while (get_time_us() < end_time) {
        // Simple CPU-bound operations
        for (int i = 0; i < 1000; i++) {
            operations++;
        }
    }
    
    double ops_per_second = (double)operations / duration_seconds;
    printf("Throughput: %.0f operations per second\n", ops_per_second);
    
    return ops_per_second;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <benchmark_type>\n", argv[0]);
        printf("Types: ipc, context, memory, pagefault, throughput, all\n");
        return 1;
    }
    
    char* benchmark_type = argv[1];
    
    printf("GNU Mach Performance Benchmark\n");
    printf("==============================\n");
    printf("Benchmark type: %s\n", benchmark_type);
    printf("Date: %s", ctime(&(time_t){time(NULL)}));
    printf("\n");
    
    if (strcmp(benchmark_type, "ipc") == 0) {
        double result = benchmark_ipc_latency();
        printf("RESULT: ipc_latency_us=%.2f\n", result);
    } else if (strcmp(benchmark_type, "context") == 0) {  
        double result = benchmark_context_switch();
        printf("RESULT: context_switch_us=%.2f\n", result);
    } else if (strcmp(benchmark_type, "memory") == 0) {
        double result = benchmark_memory_allocation();
        printf("RESULT: memory_alloc_ns=%.2f\n", result);
    } else if (strcmp(benchmark_type, "pagefault") == 0) {
        double result = benchmark_page_fault();
        printf("RESULT: page_fault_us=%.2f\n", result);
    } else if (strcmp(benchmark_type, "throughput") == 0) {
        double result = benchmark_throughput();
        printf("RESULT: throughput_ops_sec=%.0f\n", result);
    } else if (strcmp(benchmark_type, "all") == 0) {
        printf("Running all benchmarks...\n\n");
        
        double ipc_result = benchmark_ipc_latency();
        printf("RESULT: ipc_latency_us=%.2f\n", ipc_result);
        
        double context_result = benchmark_context_switch();
        printf("RESULT: context_switch_us=%.2f\n", context_result);
        
        double memory_result = benchmark_memory_allocation();
        printf("RESULT: memory_alloc_ns=%.2f\n", memory_result);
        
        double pagefault_result = benchmark_page_fault();
        printf("RESULT: page_fault_us=%.2f\n", pagefault_result);
        
        double throughput_result = benchmark_throughput();
        printf("RESULT: throughput_ops_sec=%.0f\n", throughput_result);
    } else {
        printf("Unknown benchmark type: %s\n", benchmark_type);
        return 1;
    }
    
    return 0;
}
EOF
}

# Extract performance result from benchmark output
extract_result() {
    local benchmark_output="$1"
    local metric="$2"
    
    echo "$benchmark_output" | grep "RESULT: ${metric}=" | sed "s/RESULT: ${metric}=//g"
}

# Compare result against target
validate_performance_target() {
    local metric="$1"
    local actual_value="$2"
    local target_value="${PERFORMANCE_TARGETS[$metric]}"
    
    # Convert to numbers for comparison
    local actual_int=$(printf "%.0f" "$actual_value" 2>/dev/null || echo "0")
    local target_int=$(printf "%.0f" "$target_value" 2>/dev/null || echo "0")
    
    if [[ $actual_int -le $target_int ]]; then
        log_success "✅ $metric: $actual_value (target: ≤$target_value) - PASS"
        return 0
    else
        # Check if within 20% tolerance
        local tolerance_limit=$((target_int * 120 / 100))
        if [[ $actual_int -le $tolerance_limit ]]; then
            log_warning "⚠️  $metric: $actual_value (target: ≤$target_value, within 20% tolerance) - WARN"
            return 0
        else
            log_error "❌ $metric: $actual_value (target: ≤$target_value) - FAIL"
            return 1
        fi
    fi
}

# Run performance benchmark
run_benchmark() {
    local bench_type="$1"
    
    log_info "Running performance benchmark: $bench_type"
    
    # Compile benchmark program
    local bench_binary="$OUTPUT_DIR/performance_benchmark"
    gcc -o "$bench_binary" "$OUTPUT_DIR/performance_benchmark.c" -lpthread -lrt -Wall -O2
    
    if [[ ! -x "$bench_binary" ]]; then
        log_error "Failed to compile benchmark program"
        return 1
    fi
    
    # Run benchmark with timeout
    local bench_output
    bench_output=$(timeout 300 "$bench_binary" "$bench_type" 2>&1)
    local exit_code=$?
    
    if [[ $exit_code -ne 0 ]]; then
        log_error "Benchmark failed with exit code: $exit_code"
        return 1
    fi
    
    echo "$bench_output"
    
    # Validate results against targets
    local overall_pass=true
    
    case "$bench_type" in
        "ipc")
            local ipc_result
            ipc_result=$(extract_result "$bench_output" "ipc_latency_us")
            if [[ -n "$ipc_result" ]]; then
                validate_performance_target "ipc_latency_us" "$ipc_result" || overall_pass=false
            fi
            ;;
        "context")
            local context_result
            context_result=$(extract_result "$bench_output" "context_switch_us")
            if [[ -n "$context_result" ]]; then
                validate_performance_target "context_switch_us" "$context_result" || overall_pass=false
            fi
            ;;
        "memory")
            local memory_result
            memory_result=$(extract_result "$bench_output" "memory_alloc_ns")
            if [[ -n "$memory_result" ]]; then
                validate_performance_target "memory_alloc_ns" "$memory_result" || overall_pass=false
            fi
            ;;
        "pagefault")
            local pagefault_result
            pagefault_result=$(extract_result "$bench_output" "page_fault_us")
            if [[ -n "$pagefault_result" ]]; then
                validate_performance_target "page_fault_us" "$pagefault_result" || overall_pass=false
            fi
            ;;
        "throughput")
            local throughput_result
            throughput_result=$(extract_result "$bench_output" "throughput_ops_sec")
            if [[ -n "$throughput_result" ]]; then
                validate_performance_target "throughput_ops_sec" "$throughput_result" || overall_pass=false
            fi
            ;;
        "all")
            # Validate all metrics
            for metric in "${!PERFORMANCE_TARGETS[@]}"; do
                local result
                result=$(extract_result "$bench_output" "$metric")
                if [[ -n "$result" ]]; then
                    validate_performance_target "$metric" "$result" || overall_pass=false
                fi
            done
            ;;
    esac
    
    if [[ "$overall_pass" == "true" ]]; then
        return 0
    else
        return 1
    fi
}

# Generate performance report
generate_performance_report() {
    local report_file="$OUTPUT_DIR/performance_validation_report_$(date +%Y%m%d_%H%M%S).txt"
    
    {
        echo "GNU Mach Performance Validation Report"
        echo "====================================="
        echo "Date: $(date)"
        echo "Benchmark type: $BENCHMARK_TYPE"
        echo ""
        
        echo "Performance Targets:"
        for metric in "${!PERFORMANCE_TARGETS[@]}"; do
            echo "  $metric: ≤${PERFORMANCE_TARGETS[$metric]}"
        done
        echo ""
        
        echo "Benchmark Results:"
        if [[ -f "$OUTPUT_DIR/benchmark_results.log" ]]; then
            cat "$OUTPUT_DIR/benchmark_results.log"
        else
            echo "  No benchmark results available"
        fi
        
    } > "$report_file"
    
    log_info "Performance validation report written to: $report_file"
}

# Main execution
main() {
    log_info "GNU Mach Performance Target Validation"
    log_info "Benchmark type: $BENCHMARK_TYPE"
    log_info "All benchmarks: $ALL_BENCHMARKS"
    log_info "Output directory: $OUTPUT_DIR"
    
    # Create output directory
    mkdir -p "$OUTPUT_DIR"
    
    # Create benchmark program
    create_perf_benchmark
    
    # Run benchmarks
    local benchmark_success=true
    local log_file="$OUTPUT_DIR/benchmark_results.log"
    
    {
        echo "=== Performance Benchmark Results ==="
        echo "Date: $(date)"
        echo "Benchmark type: $BENCHMARK_TYPE"
        echo ""
        
        if [[ "$BENCHMARK_TYPE" == "all" || "$ALL_BENCHMARKS" == "true" ]]; then
            run_benchmark "all" || benchmark_success=false
        else
            run_benchmark "$BENCHMARK_TYPE" || benchmark_success=false
        fi
        
    } | tee "$log_file"
    
    # Generate report
    generate_performance_report
    
    # Final result
    if [[ "$benchmark_success" == "true" ]]; then
        log_success "✅ Performance validation completed successfully"
        log_success "All performance targets met or within acceptable tolerance"
        exit 0
    else
        log_error "❌ Performance validation failed"
        log_error "Some performance targets not met - optimization required"
        exit 1
    fi
}

# Run main function
main "$@"