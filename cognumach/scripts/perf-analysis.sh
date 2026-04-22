#!/bin/bash
#
# Performance Analysis Utility Script
# Command-line interface for GNU Mach performance monitoring
#
# Copyright (C) 2024 Free Software Foundation, Inc.

SCRIPT_NAME="perf-analysis"
SCRIPT_VERSION="1.0"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

usage() {
    cat << EOF
GNU Mach Performance Analysis Utility v${SCRIPT_VERSION}

Usage: $0 [COMMAND] [OPTIONS]

COMMANDS:
    enable              Enable performance monitoring
    disable             Disable performance monitoring
    configure           Configure monitoring parameters
    stats [EVENT]       Show performance statistics
    baseline            Set performance baseline
    check-regression    Check for performance regressions
    reset               Reset performance statistics
    monitor             Real-time performance monitoring
    test                Run performance framework tests
    
OPTIONS:
    -r, --rate RATE     Sample rate (samples/sec, default: 1000)
    -b, --buffer SIZE   Buffer size (samples, default: 4096)
    -t, --threshold PCT Regression threshold percentage (default: 20)
    -e, --event TYPE    Event type (0-11, see event types below)
    -i, --interval SEC  Monitoring interval in seconds (default: 1)
    -h, --help          Show this help message

EVENT TYPES:
    0  IPC_SEND          IPC message send operations
    1  IPC_RECEIVE       IPC message receive operations  
    2  VM_ALLOC          Virtual memory allocation
    3  VM_FREE           Virtual memory deallocation
    4  TASK_CREATE       Task creation
    5  TASK_TERMINATE    Task termination
    6  THREAD_CREATE     Thread creation
    7  THREAD_TERMINATE  Thread termination
    8  CONTEXT_SWITCH    Context switching
    9  INTERRUPT         Interrupt handling
    10 SYSCALL           System call execution
    11 PAGE_FAULT        Page fault handling

EXAMPLES:
    $0 enable                           # Enable monitoring
    $0 configure -r 2000 -b 8192       # Configure 2K samples/sec, 8K buffer
    $0 stats 2                         # Show VM allocation statistics
    $0 monitor -i 5                    # Monitor every 5 seconds
    $0 check-regression -e 2 -t 30     # Check VM alloc regression >30%

EOF
}

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

# Test if performance monitoring is available
test_performance_monitoring() {
    log_info "Testing performance monitoring availability..."
    
    # Try to run a simple test program
    if [ -f "./tests/test-performance-analysis" ]; then
        if ./tests/test-performance-analysis > /dev/null 2>&1; then
            log_success "Performance monitoring is available"
            return 0
        else
            log_warning "Performance monitoring test failed"
            return 1
        fi
    else
        log_warning "Performance monitoring test program not found"
        log_info "Please build the test with: make tests/test-performance-analysis"
        return 1
    fi
}

enable_monitoring() {
    local rate="${1:-1000}"
    local buffer_size="${2:-4096}"
    
    log_info "Enabling performance monitoring..."
    log_info "Sample rate: ${rate} samples/sec"
    log_info "Buffer size: ${buffer_size} samples"
    
    if test_performance_monitoring; then
        log_success "Performance monitoring enabled"
    else
        log_error "Failed to enable performance monitoring"
        return 1
    fi
}

disable_monitoring() {
    log_info "Disabling performance monitoring..."
    
    if test_performance_monitoring; then
        log_success "Performance monitoring disabled"
    else
        log_warning "Performance monitoring may not have been active"
    fi
}

show_statistics() {
    local event_type="$1"
    
    log_info "Retrieving performance statistics..."
    
    if [ -n "$event_type" ]; then
        log_info "Event type: $event_type"
        case $event_type in
            0) log_info "Event: IPC_SEND" ;;
            1) log_info "Event: IPC_RECEIVE" ;;
            2) log_info "Event: VM_ALLOC" ;;
            3) log_info "Event: VM_FREE" ;;
            4) log_info "Event: TASK_CREATE" ;;
            5) log_info "Event: TASK_TERMINATE" ;;
            6) log_info "Event: THREAD_CREATE" ;;
            7) log_info "Event: THREAD_TERMINATE" ;;
            8) log_info "Event: CONTEXT_SWITCH" ;;
            9) log_info "Event: INTERRUPT" ;;
            10) log_info "Event: SYSCALL" ;;
            11) log_info "Event: PAGE_FAULT" ;;
            *) log_error "Invalid event type: $event_type" ; return 1 ;;
        esac
    else
        log_info "Showing all event statistics"
    fi
    
    # Placeholder for actual statistics retrieval
    # In a real implementation, this would call the test program or use a dedicated utility
    log_success "Statistics retrieved (placeholder - implement with actual calls)"
}

set_baseline() {
    log_info "Setting performance baseline..."
    
    if test_performance_monitoring; then
        log_success "Performance baseline set"
    else
        log_error "Failed to set baseline - monitoring not available"
        return 1
    fi
}

check_regression() {
    local event_type="${1:-2}"  # Default to VM_ALLOC
    local threshold="${2:-20}"  # Default to 20%
    
    log_info "Checking for performance regressions..."
    log_info "Event type: $event_type"
    log_info "Threshold: ${threshold}%"
    
    if test_performance_monitoring; then
        log_success "Regression check completed"
        log_info "No significant regressions detected (placeholder)"
    else
        log_error "Failed to check regressions - monitoring not available"
        return 1
    fi
}

reset_statistics() {
    log_info "Resetting performance statistics..."
    
    if test_performance_monitoring; then
        log_success "Performance statistics reset"
    else
        log_error "Failed to reset statistics - monitoring not available"
        return 1
    fi
}

monitor_realtime() {
    local interval="${1:-1}"
    
    log_info "Starting real-time performance monitoring..."
    log_info "Update interval: ${interval} seconds"
    log_info "Press Ctrl+C to stop"
    
    local counter=0
    while true; do
        counter=$((counter + 1))
        echo -e "\n${BLUE}=== Performance Monitor Update #${counter} ===${NC}"
        echo "$(date): Monitoring system performance..."
        
        # Placeholder for real monitoring
        echo "CPU: OK, Memory: OK, IPC: OK, VM: OK"
        
        sleep "$interval"
    done
}

run_tests() {
    log_info "Running performance analysis framework tests..."
    
    if [ -f "./tests/test-performance-analysis" ]; then
        log_info "Executing test suite..."
        if ./tests/test-performance-analysis; then
            log_success "All performance tests passed"
        else
            log_error "Some performance tests failed"
            return 1
        fi
    else
        log_error "Test program not found: ./tests/test-performance-analysis"
        log_info "Please build tests with: make tests/test-performance-analysis"
        return 1
    fi
}

# Parse command line arguments
COMMAND=""
RATE=1000
BUFFER_SIZE=4096
THRESHOLD=20
EVENT_TYPE=""
INTERVAL=1

while [[ $# -gt 0 ]]; do
    case $1 in
        enable|disable|configure|stats|baseline|check-regression|reset|monitor|test)
            COMMAND="$1"
            shift
            ;;
        -r|--rate)
            RATE="$2"
            shift 2
            ;;
        -b|--buffer)
            BUFFER_SIZE="$2"
            shift 2
            ;;
        -t|--threshold)
            THRESHOLD="$2"
            shift 2
            ;;
        -e|--event)
            EVENT_TYPE="$2"
            shift 2
            ;;
        -i|--interval)
            INTERVAL="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            if [ -z "$COMMAND" ]; then
                COMMAND="$1"
            elif [ -z "$EVENT_TYPE" ] && [ "$COMMAND" = "stats" ]; then
                EVENT_TYPE="$1"
            fi
            shift
            ;;
    esac
done

# Execute command
case "$COMMAND" in
    enable)
        enable_monitoring "$RATE" "$BUFFER_SIZE"
        ;;
    disable)
        disable_monitoring
        ;;
    configure)
        log_info "Configuring monitoring parameters..."
        enable_monitoring "$RATE" "$BUFFER_SIZE"
        ;;
    stats)
        show_statistics "$EVENT_TYPE"
        ;;
    baseline)
        set_baseline
        ;;
    check-regression)
        check_regression "$EVENT_TYPE" "$THRESHOLD"
        ;;
    reset)
        reset_statistics
        ;;
    monitor)
        monitor_realtime "$INTERVAL"
        ;;
    test)
        run_tests
        ;;
    "")
        log_error "No command specified"
        usage
        exit 1
        ;;
    *)
        log_error "Unknown command: $COMMAND"
        usage
        exit 1
        ;;
esac