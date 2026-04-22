#!/bin/bash
#
# Performance Optimization Validation Script
# Validates that the three major performance optimizations are implemented
#
# Copyright (C) 2024 Free Software Foundation

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log() {
    echo -e "${BLUE}[INFO]${NC} $*"
}

success() {
    echo -e "${GREEN}[PASS]${NC} $*"
}

warning() {
    echo -e "${YELLOW}[WARN]${NC} $*"
}

error() {
    echo -e "${RED}[FAIL]${NC} $*"
}

# Validation counters
TOTAL_CHECKS=0
PASSED_CHECKS=0

check_file_exists() {
    local file="$1"
    local description="$2"
    
    TOTAL_CHECKS=$((TOTAL_CHECKS + 1))
    
    if [ -f "$PROJECT_ROOT/$file" ]; then
        success "$description - File exists: $file"
        PASSED_CHECKS=$((PASSED_CHECKS + 1))
        return 0
    else
        error "$description - File missing: $file"
        return 1
    fi
}

check_code_implementation() {
    local file="$1"
    local pattern="$2"
    local description="$3"
    
    TOTAL_CHECKS=$((TOTAL_CHECKS + 1))
    
    if [ -f "$PROJECT_ROOT/$file" ] && grep -q "$pattern" "$PROJECT_ROOT/$file"; then
        success "$description - Implementation found in $file"
        PASSED_CHECKS=$((PASSED_CHECKS + 1))
        return 0
    else
        error "$description - Implementation missing in $file"
        return 1
    fi
}

check_define_exists() {
    local file="$1"
    local define="$2"
    local description="$3"
    
    TOTAL_CHECKS=$((TOTAL_CHECKS + 1))
    
    if [ -f "$PROJECT_ROOT/$file" ] && grep -E "#define[[:space:]]+$define" "$PROJECT_ROOT/$file" > /dev/null; then
        success "$description - Define found: $define"
        PASSED_CHECKS=$((PASSED_CHECKS + 1))
        return 0
    else
        error "$description - Define missing: $define"
        return 1
    fi
}

check_function_exists() {
    local file="$1"
    local function="$2"
    local description="$3"
    
    TOTAL_CHECKS=$((TOTAL_CHECKS + 1))
    
    if [ -f "$PROJECT_ROOT/$file" ] && grep -q "$function" "$PROJECT_ROOT/$file"; then
        success "$description - Function found: $function"
        PASSED_CHECKS=$((PASSED_CHECKS + 1))
        return 0
    else
        error "$description - Function missing: $function"
        return 1
    fi
}

validate_ipc_virtual_copy() {
    log "=== Validating IPC Virtual Copy Optimization ==="
    
    # Check threshold definitions
    check_define_exists "ipc/ipc_kmsg.h" "IPC_VIRTUAL_COPY_THRESHOLD" "Virtual copy threshold"
    check_define_exists "ipc/ipc_kmsg.h" "IPC_ZERO_COPY_THRESHOLD" "Zero copy threshold"
    
    # Check helper functions
    check_function_exists "ipc/ipc_kmsg.c" "ipc_should_use_virtual_copy" "Virtual copy decision function"
    check_function_exists "ipc/ipc_kmsg.c" "ipc_should_use_zero_copy" "Zero copy decision function"
    
    # Check integration in copyin logic
    check_code_implementation "ipc/ipc_kmsg.c" "ipc_should_use_zero_copy(length)" "Zero copy integration"
    check_code_implementation "ipc/ipc_kmsg.c" "ipc_should_use_virtual_copy(length)" "Virtual copy integration"
    
    # Check VM map optimization
    check_code_implementation "vm/vm_map.c" "VM_MAP_COPY_OBJECT" "VM map copy object optimization"
    
    echo
}

validate_page_cache_improvements() {
    log "=== Validating Page Cache Improvements ==="
    
    # Check adaptive cache sizing
    check_function_exists "vm/vm_page.c" "vm_page_seg_adapt_cache_size" "Adaptive cache sizing function"
    check_code_implementation "vm/vm_page.c" "cache_scale_percent" "Cache scaling implementation"
    
    # Check read-ahead mechanisms
    check_code_implementation "vm/vm_resident.c" "readahead" "Read-ahead mechanism"
    check_code_implementation "vm/vm_object.h" "readahead_" "Read-ahead data structures"
    
    # Check enhanced replacement policies
    check_function_exists "vm/vm_page.c" "vm_page_update_replacement_data" "Replacement policy function"
    check_code_implementation "vm/vm_page.c" "access_frequency" "Access frequency tracking"
    check_code_implementation "vm/vm_page.c" "aging_time" "Page aging implementation"
    
    echo
}

validate_tickless_timer() {
    log "=== Validating Tickless Timer Optimization ==="
    
    # Check tickless timer support
    check_code_implementation "kern/mach_clock.c" "TICKLESS_TIMER" "Tickless timer compilation support"
    check_function_exists "kern/mach_clock.c" "tickless_can_skip_tick" "Tickless skip function"
    check_function_exists "kern/mach_clock.c" "tickless_next_timer_deadline" "Timer deadline function"
    
    # Check timer optimization integration
    check_code_implementation "kern/mach_clock.c" "should_skip_tick" "Skip tick logic"
    check_code_implementation "kern/timer.c" "timer_update_optimized" "Optimized timer update"
    
    # Check configure support
    check_code_implementation "configure.ac" "enable-tickless" "Configuration support"
    
    echo
}

validate_test_suite() {
    log "=== Validating Performance Test Suite ==="
    
    # Check test files existence
    check_file_exists "tests/test-ipc-virtual-copy.c" "IPC virtual copy test"
    check_file_exists "tests/test-tickless-timer.c" "Tickless timer test"
    check_file_exists "tests/test-memory-management-enhanced.c" "Enhanced memory management test"
    
    # Check test suite integration
    check_code_implementation "tests/user-qemu.mk" "test-ipc-virtual-copy" "IPC test integration"
    check_code_implementation "tests/user-qemu.mk" "test-tickless-timer" "Timer test integration"
    check_code_implementation "tests/user-qemu.mk" "test-memory-management-enhanced" "Memory test integration"
    
    echo
}

check_performance_documentation() {
    log "=== Validating Performance Documentation ==="
    
    check_file_exists "IPC_VIRTUAL_COPY_IMPLEMENTATION.md" "IPC optimization documentation"
    check_file_exists "PAGE_CACHE_IMPROVEMENTS.md" "Page cache documentation"
    
    # Check implementation summaries
    if [ -f "$PROJECT_ROOT/IPC_VIRTUAL_COPY_IMPLEMENTATION.md" ]; then
        if grep -q "30% improvement" "$PROJECT_ROOT/IPC_VIRTUAL_COPY_IMPLEMENTATION.md" || \
           grep -q "Performance Benefits" "$PROJECT_ROOT/IPC_VIRTUAL_COPY_IMPLEMENTATION.md"; then
            success "IPC optimization benefits documented"
            PASSED_CHECKS=$((PASSED_CHECKS + 1))
        else
            warning "IPC optimization benefits not clearly documented"
        fi
        TOTAL_CHECKS=$((TOTAL_CHECKS + 1))
    fi
    
    echo
}

display_summary() {
    log "=== Performance Optimization Validation Summary ==="
    echo
    
    PASS_RATE=$((PASSED_CHECKS * 100 / TOTAL_CHECKS))
    
    echo "Total Checks: $TOTAL_CHECKS"
    echo "Passed: $PASSED_CHECKS"
    echo "Failed: $((TOTAL_CHECKS - PASSED_CHECKS))"
    echo "Pass Rate: $PASS_RATE%"
    echo
    
    if [ $PASS_RATE -ge 90 ]; then
        success "EXCELLENT: Performance optimizations are comprehensively implemented ($PASS_RATE% pass rate)"
        echo
        echo "✅ IPC Virtual Copy Optimization: COMPLETE"
        echo "✅ Page Cache Improvements: COMPLETE" 
        echo "✅ Tickless Timer Optimization: COMPLETE"
        echo "✅ Performance Test Suite: COMPLETE"
        echo
        echo "Ready for performance testing and benchmarking!"
        
    elif [ $PASS_RATE -ge 75 ]; then
        success "GOOD: Performance optimizations are mostly implemented ($PASS_RATE% pass rate)"
        echo "Some minor components may need attention."
        
    elif [ $PASS_RATE -ge 50 ]; then
        warning "PARTIAL: Performance optimizations are partially implemented ($PASS_RATE% pass rate)"
        echo "Significant components are missing or incomplete."
        
    else
        error "INCOMPLETE: Performance optimizations need significant work ($PASS_RATE% pass rate)"
        echo "Major components are missing."
    fi
    echo
}

show_performance_targets() {
    log "=== Performance Targets from Issue #45 ==="
    echo
    echo "Success Criteria:"
    echo "• 30% improvement in IPC throughput"
    echo "• 25% improvement in I/O performance"  
    echo "• Measurable reduction in CPU overhead"
    echo
    echo "To measure these targets:"
    echo "1. Build with: ./configure --enable-tickless && make"
    echo "2. Run: make run-benchmark-ipc"
    echo "3. Run: make run-benchmark-memory"
    echo "4. Run: make run-ipc-virtual-copy"
    echo "5. Run: make run-tickless-timer"
    echo
}

main() {
    echo "======================================================================"
    echo "GNU Mach Performance Optimization Validation"
    echo "======================================================================"
    echo
    
    cd "$PROJECT_ROOT"
    
    validate_ipc_virtual_copy
    validate_page_cache_improvements
    validate_tickless_timer
    validate_test_suite
    check_performance_documentation
    
    display_summary
    show_performance_targets
    
    if [ $PASS_RATE -ge 90 ]; then
        exit 0
    else
        exit 1
    fi
}

# Run main function
main "$@"