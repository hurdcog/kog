#!/bin/bash
# IPC Performance Benchmark Script for GNU Mach Phase 2
# Tests IPC performance under different workload patterns

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Default configuration
WORKLOAD="database"
OUTPUT_DIR="$PROJECT_ROOT/test-results/ipc-benchmarks"
ITERATIONS=10000
DURATION="5m"

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

IPC performance benchmarking for GNU Mach with workload-specific patterns.

Options:
  --workload TYPE         Workload pattern (database, webserver, scientific)
  --iterations COUNT      Number of IPC operations (default: 10000)
  --duration DURATION     Test duration (default: 5m)
  --output-dir DIR        Output directory for results
  -h, --help             Show this help message

Examples:
  $0 --workload=database
  $0 --workload=webserver --iterations=50000
  $0 --workload=scientific --duration=10m
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --workload=*)
            WORKLOAD="${1#*=}"
            shift
            ;;
        --workload)
            WORKLOAD="$2"
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
            DURATION="${1#*=}"
            shift
            ;;
        --duration)
            DURATION="$2"
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

# Create IPC benchmark program
create_ipc_benchmark() {
    local bench_file="$OUTPUT_DIR/ipc_benchmark.c"
    
    cat > "$bench_file" << 'EOF'
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

static volatile int keep_running = 1;
static long total_operations = 0;
static long total_bytes = 0;
static struct timeval start_time, end_time;

void signal_handler(int sig) {
    keep_running = 0;
    gettimeofday(&end_time, NULL);
}

// High resolution timing
static inline uint64_t get_time_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000ULL + tv.tv_usec;
}

// Database workload pattern: Many small transactions
typedef struct {
    int request_type;
    int record_id;
    char data[64];
} db_request_t;

typedef struct {
    int status;
    char result[128];
} db_response_t;

void* database_client(void* arg) {
    int client_id = *(int*)arg;
    int sockfd;
    struct sockaddr_un addr;
    
    // Create socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return NULL;
    }
    
    // Connect to server
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), "/tmp/ipc_bench_%d", getpid());
    
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        close(sockfd);
        return NULL;
    }
    
    db_request_t request;
    db_response_t response;
    int operations = 0;
    
    while (keep_running && operations < 10000) {
        // Prepare database-like request
        request.request_type = rand() % 4;  // SELECT, INSERT, UPDATE, DELETE
        request.record_id = rand() % 100000;
        snprintf(request.data, sizeof(request.data), "client_%d_op_%d", client_id, operations);
        
        // Send request
        if (send(sockfd, &request, sizeof(request), 0) != sizeof(request)) {
            break;
        }
        
        // Receive response
        if (recv(sockfd, &response, sizeof(response), 0) != sizeof(response)) {
            break;
        }
        
        operations++;
        __sync_fetch_and_add(&total_operations, 1);
        __sync_fetch_and_add(&total_bytes, sizeof(request) + sizeof(response));
    }
    
    close(sockfd);
    return NULL;
}

// Web server workload pattern: HTTP-like request/response
typedef struct {
    char method[8];
    char path[256];
    int content_length;
} http_request_t;

typedef struct {
    int status_code;
    int content_length;
    char data[1024];
} http_response_t;

void* webserver_client(void* arg) {
    int client_id = *(int*)arg;
    int sockfd;
    struct sockaddr_un addr;
    
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        return NULL;
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), "/tmp/ipc_bench_%d", getpid());
    
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        close(sockfd);
        return NULL;
    }
    
    http_request_t request;
    http_response_t response;
    int operations = 0;
    
    while (keep_running && operations < 5000) {
        // Prepare HTTP-like request
        strcpy(request.method, (rand() % 10 == 0) ? "POST" : "GET");
        snprintf(request.path, sizeof(request.path), "/api/resource/%d", rand() % 1000);
        request.content_length = (strcmp(request.method, "POST") == 0) ? (rand() % 512) : 0;
        
        if (send(sockfd, &request, sizeof(request), 0) != sizeof(request)) {
            break;
        }
        
        if (recv(sockfd, &response, sizeof(response), 0) != sizeof(response)) {
            break;
        }
        
        operations++;
        __sync_fetch_and_add(&total_operations, 1);
        __sync_fetch_and_add(&total_bytes, sizeof(request) + sizeof(response));
    }
    
    close(sockfd);
    return NULL;
}

// Scientific workload pattern: Large data transfers
typedef struct {
    int job_id;
    int data_size;
    double parameters[16];
} sci_request_t;

typedef struct {
    int job_id;
    int result_size;
    double results[64];
} sci_response_t;

void* scientific_client(void* arg) {
    int client_id = *(int*)arg;
    int sockfd;
    struct sockaddr_un addr;
    
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        return NULL;
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), "/tmp/ipc_bench_%d", getpid());
    
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        close(sockfd);
        return NULL;
    }
    
    sci_request_t request;
    sci_response_t response;
    int operations = 0;
    
    while (keep_running && operations < 1000) {
        // Prepare scientific computation request
        request.job_id = operations;
        request.data_size = 1024 + (rand() % 4096);  // Variable data size
        
        for (int i = 0; i < 16; i++) {
            request.parameters[i] = (double)rand() / RAND_MAX;
        }
        
        if (send(sockfd, &request, sizeof(request), 0) != sizeof(request)) {
            break;
        }
        
        if (recv(sockfd, &response, sizeof(response), 0) != sizeof(response)) {
            break;
        }
        
        operations++;
        __sync_fetch_and_add(&total_operations, 1);
        __sync_fetch_and_add(&total_bytes, sizeof(request) + sizeof(response));
    }
    
    close(sockfd);
    return NULL;
}

// Simple echo server for all workload types
void* echo_server(void* arg) {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buffer[4096];
    
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("server socket");
        return NULL;
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), "/tmp/ipc_bench_%d", getpid());
    
    unlink(addr.sun_path);  // Remove if exists
    
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        return NULL;
    }
    
    if (listen(server_fd, 10) == -1) {
        perror("listen");
        close(server_fd);
        return NULL;
    }
    
    printf("Echo server listening on %s\n", addr.sun_path);
    
    while (keep_running) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            if (keep_running) perror("accept");
            continue;
        }
        
        // Handle client in simple echo mode
        while (keep_running) {
            ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) break;
            
            // Echo back (simulating processing)
            if (send(client_fd, buffer, bytes_received, 0) != bytes_received) {
                break;
            }
        }
        
        close(client_fd);
    }
    
    close(server_fd);
    unlink(addr.sun_path);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <workload> <num_clients> <duration_seconds>\n", argv[0]);
        printf("Workloads: database, webserver, scientific\n");
        return 1;
    }
    
    char* workload = argv[1];
    int num_clients = atoi(argv[2]);
    int duration = atoi(argv[3]);
    
    if (num_clients <= 0 || num_clients > 100) {
        printf("Invalid number of clients: %d\n", num_clients);
        return 1;
    }
    
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGALRM, signal_handler);
    
    printf("IPC Benchmark - Workload: %s, Clients: %d, Duration: %ds\n", 
           workload, num_clients, duration);
    
    // Start server thread
    pthread_t server_thread;
    if (pthread_create(&server_thread, NULL, echo_server, NULL) != 0) {
        perror("Failed to create server thread");
        return 1;
    }
    
    sleep(1);  // Give server time to start
    
    // Start client threads
    pthread_t* client_threads = malloc(num_clients * sizeof(pthread_t));
    int* client_ids = malloc(num_clients * sizeof(int));
    
    gettimeofday(&start_time, NULL);
    alarm(duration);  // Set duration alarm
    
    void* (*client_func)(void*) = NULL;
    if (strcmp(workload, "database") == 0) {
        client_func = database_client;
    } else if (strcmp(workload, "webserver") == 0) {
        client_func = webserver_client;
    } else if (strcmp(workload, "scientific") == 0) {
        client_func = scientific_client;
    } else {
        printf("Unknown workload: %s\n", workload);
        return 1;
    }
    
    for (int i = 0; i < num_clients; i++) {
        client_ids[i] = i;
        if (pthread_create(&client_threads[i], NULL, client_func, &client_ids[i]) != 0) {
            printf("Failed to create client thread %d\n", i);
        }
    }
    
    // Wait for clients to complete
    for (int i = 0; i < num_clients; i++) {
        pthread_join(client_threads[i], NULL);
    }
    
    keep_running = 0;
    pthread_cancel(server_thread);
    
    // Calculate and display results
    if (end_time.tv_sec == 0) {
        gettimeofday(&end_time, NULL);
    }
    
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + 
                         (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    
    printf("\nBenchmark Results:\n");
    printf("==================\n");
    printf("Workload: %s\n", workload);
    printf("Clients: %d\n", num_clients);
    printf("Duration: %.2f seconds\n", elapsed_time);
    printf("Total operations: %ld\n", total_operations);
    printf("Total bytes transferred: %ld\n", total_bytes);
    printf("Operations per second: %.2f\n", total_operations / elapsed_time);
    printf("Throughput: %.2f MB/s\n", (total_bytes / elapsed_time) / (1024 * 1024));
    printf("Average latency: %.2f microseconds\n", (elapsed_time * 1000000) / total_operations);
    
    free(client_threads);
    free(client_ids);
    
    return 0;
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
        *) echo "$duration" ;;
    esac
}

# Run IPC benchmark
run_ipc_benchmark() {
    local workload="$1"
    local duration_seconds="$2"
    
    log_info "Running IPC benchmark for workload: $workload"
    
    # Compile benchmark program
    local bench_binary="$OUTPUT_DIR/ipc_benchmark"
    gcc -o "$bench_binary" "$OUTPUT_DIR/ipc_benchmark.c" -lpthread -Wall -O2
    
    if [[ ! -x "$bench_binary" ]]; then
        log_error "Failed to compile IPC benchmark program"
        return 1
    fi
    
    # Determine client count based on workload
    local num_clients
    case "$workload" in
        "database")
            num_clients=20  # Many concurrent database connections
            ;;
        "webserver")
            num_clients=10  # Moderate HTTP connections
            ;;
        "scientific")
            num_clients=4   # Fewer but larger computation requests
            ;;
        *)
            num_clients=8   # Default
            ;;
    esac
    
    # Run benchmark
    local log_file="$OUTPUT_DIR/ipc_benchmark_${workload}_$(date +%Y%m%d_%H%M%S).log"
    
    log_info "Starting IPC benchmark (clients: $num_clients, duration: ${duration_seconds}s)"
    
    "$bench_binary" "$workload" "$num_clients" "$duration_seconds" | tee "$log_file"
    local exit_code=$?
    
    if [[ $exit_code -eq 0 ]]; then
        log_success "IPC benchmark completed successfully"
        
        # Extract key metrics from log
        local ops_per_sec
        ops_per_sec=$(grep "Operations per second:" "$log_file" | awk '{print $4}' || echo "0")
        local throughput
        throughput=$(grep "Throughput:" "$log_file" | awk '{print $2}' || echo "0")
        local avg_latency
        avg_latency=$(grep "Average latency:" "$log_file" | awk '{print $3}' || echo "0")
        
        log_info "📊 Performance Summary:"
        log_info "   Operations/sec: $ops_per_sec"
        log_info "   Throughput: $throughput MB/s"
        log_info "   Avg Latency: $avg_latency μs"
        
        return 0
    else
        log_error "IPC benchmark failed with exit code: $exit_code"
        return 1
    fi
}

# Generate benchmark report
generate_benchmark_report() {
    local report_file="$OUTPUT_DIR/ipc_benchmark_summary_$(date +%Y%m%d_%H%M%S).txt"
    
    {
        echo "GNU Mach IPC Performance Benchmark Summary"
        echo "==========================================="
        echo "Date: $(date)"
        echo "Workload: $WORKLOAD"
        echo "Iterations: $ITERATIONS"
        echo "Duration: $DURATION"
        echo ""
        
        echo "Benchmark Results:"
        local latest_log
        latest_log=$(ls -t "$OUTPUT_DIR"/ipc_benchmark_${WORKLOAD}_*.log 2>/dev/null | head -1)
        if [[ -f "$latest_log" ]]; then
            echo "Latest benchmark results:"
            tail -10 "$latest_log"
        else
            echo "No benchmark results available"
        fi
        
    } > "$report_file"
    
    log_info "IPC benchmark summary written to: $report_file"
}

# Main execution
main() {
    log_info "GNU Mach IPC Performance Benchmark"
    log_info "Workload: $WORKLOAD"
    log_info "Iterations: $ITERATIONS"
    log_info "Duration: $DURATION"
    log_info "Output directory: $OUTPUT_DIR"
    
    # Validate workload type
    case "$WORKLOAD" in
        "database"|"webserver"|"scientific")
            ;;
        *)
            log_error "Invalid workload type: $WORKLOAD"
            log_error "Valid types: database, webserver, scientific"
            exit 1
            ;;
    esac
    
    # Create output directory
    mkdir -p "$OUTPUT_DIR"
    
    # Create benchmark program
    create_ipc_benchmark
    
    # Convert duration to seconds
    local duration_seconds
    duration_seconds=$(duration_to_seconds "$DURATION")
    
    # Run benchmark
    if run_ipc_benchmark "$WORKLOAD" "$duration_seconds"; then
        generate_benchmark_report
        log_success "✅ IPC benchmark completed successfully"
        exit 0
    else
        log_error "❌ IPC benchmark failed"
        exit 1
    fi
}

# Run main function
main "$@"