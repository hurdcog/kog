#!/bin/bash
#
# Performance Analysis Framework Integration Test
# Demonstrates the complete performance monitoring system
#
# Copyright (C) 2024 Free Software Foundation, Inc.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

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

# Test framework components
test_framework_files() {
    log_info "Testing performance analysis framework files..."
    
    local files=(
        "kern/perf_analysis.h"
        "kern/perf_analysis.c"
        "kern/ipc_perf_monitor.c"
        "include/mach/perf_monitor.h"
        "include/mach/perf_monitor.defs"
        "tests/test-performance-analysis.c"
        "scripts/perf-analysis.sh"
        "docs/performance-analysis-framework.md"
    )
    
    local missing_files=()
    for file in "${files[@]}"; do
        if [ -f "$PROJECT_ROOT/$file" ]; then
            log_success "✓ $file"
        else
            log_error "✗ Missing: $file"
            missing_files+=("$file")
        fi
    done
    
    if [ ${#missing_files[@]} -eq 0 ]; then
        log_success "All framework files present"
        return 0
    else
        log_error "Missing ${#missing_files[@]} framework files"
        return 1
    fi
}

# Test build system integration
test_build_integration() {
    log_info "Testing build system integration..."
    
    local makefrag="$PROJECT_ROOT/Makefrag.am"
    if [ -f "$makefrag" ]; then
        if grep -q "perf_analysis" "$makefrag"; then
            log_success "✓ Performance analysis files added to build system"
        else
            log_warning "Performance analysis files not found in Makefrag.am"
            return 1
        fi
        
        if grep -q "perf_monitor.defs" "$makefrag"; then
            log_success "✓ MIG interface files added to build system"
        else
            log_warning "MIG interface files not found in Makefrag.am"
            return 1
        fi
    else
        log_error "Makefrag.am not found"
        return 1
    fi
    
    return 0
}

# Test kernel integration
test_kernel_integration() {
    log_info "Testing kernel integration..."
    
    # Check IPC instrumentation
    local ipc_file="$PROJECT_ROOT/ipc/mach_msg.c"
    if [ -f "$ipc_file" ]; then
        if grep -q "perf_analysis.h" "$ipc_file"; then
            log_success "✓ IPC instrumentation added"
        else
            log_warning "IPC instrumentation not found"
            return 1
        fi
    fi
    
    # Check VM instrumentation
    local vm_file="$PROJECT_ROOT/vm/vm_user.c"
    if [ -f "$vm_file" ]; then
        if grep -q "perf_analysis.h" "$vm_file"; then
            log_success "✓ VM instrumentation added"
        else
            log_warning "VM instrumentation not found"
            return 1
        fi
    fi
    
    # Check startup initialization
    local startup_file="$PROJECT_ROOT/kern/startup.c"
    if [ -f "$startup_file" ]; then
        if grep -q "perf_analysis_init" "$startup_file"; then
            log_success "✓ Framework initialization added to startup"
        else
            log_warning "Framework initialization not found in startup"
            return 1
        fi
    fi
    
    return 0
}

# Test API completeness
test_api_completeness() {
    log_info "Testing API completeness..."
    
    local header_file="$PROJECT_ROOT/kern/perf_analysis.h"
    if [ -f "$header_file" ]; then
        local required_functions=(
            "perf_analysis_init"
            "perf_monitor_enable"
            "perf_record_event"
            "perf_event_start"
            "perf_event_end"
            "perf_get_event_stats"
            "perf_set_baseline"
            "perf_check_regression"
            "perf_reset_stats"
        )
        
        local missing_functions=()
        for func in "${required_functions[@]}"; do
            if grep -q "$func" "$header_file"; then
                log_success "✓ API function: $func"
            else
                log_warning "✗ Missing API function: $func"
                missing_functions+=("$func")
            fi
        done
        
        if [ ${#missing_functions[@]} -eq 0 ]; then
            log_success "All required API functions present"
            return 0
        else
            log_error "Missing ${#missing_functions[@]} API functions"
            return 1
        fi
    else
        log_error "Performance analysis header file not found"
        return 1
    fi
}

# Test documentation
test_documentation() {
    log_info "Testing documentation..."
    
    local doc_file="$PROJECT_ROOT/docs/performance-analysis-framework.md"
    if [ -f "$doc_file" ]; then
        local required_sections=(
            "Overview"
            "Features"
            "Architecture"
            "Usage"
            "Configuration"
            "Examples"
        )
        
        local missing_sections=()
        for section in "${required_sections[@]}"; do
            if grep -q "## $section" "$doc_file"; then
                log_success "✓ Documentation section: $section"
            else
                log_warning "✗ Missing documentation section: $section"
                missing_sections+=("$section")
            fi
        done
        
        if [ ${#missing_sections[@]} -eq 0 ]; then
            log_success "All required documentation sections present"
            return 0
        else
            log_warning "Missing ${#missing_sections[@]} documentation sections"
            return 1
        fi
    else
        log_error "Documentation file not found"
        return 1
    fi
}

# Test utility script
test_utility_script() {
    log_info "Testing utility script..."
    
    local script_file="$PROJECT_ROOT/scripts/perf-analysis.sh"
    if [ -f "$script_file" ]; then
        if [ -x "$script_file" ]; then
            log_success "✓ Utility script is executable"
        else
            log_warning "Utility script is not executable"
            return 1
        fi
        
        # Test help functionality
        if "$script_file" --help >/dev/null 2>&1; then
            log_success "✓ Utility script help works"
        else
            log_warning "Utility script help not working"
            return 1
        fi
    else
        log_error "Utility script not found"
        return 1
    fi
    
    return 0
}

# Test code quality
test_code_quality() {
    log_info "Testing code quality..."
    
    # Check for consistent style
    local c_files=(
        "$PROJECT_ROOT/kern/perf_analysis.c"
        "$PROJECT_ROOT/kern/ipc_perf_monitor.c"
        "$PROJECT_ROOT/tests/test-performance-analysis.c"
    )
    
    for file in "${c_files[@]}"; do
        if [ -f "$file" ]; then
            # Check for copyright header
            if head -10 "$file" | grep -q "Copyright.*Free Software Foundation"; then
                log_success "✓ Copyright header in $(basename "$file")"
            else
                log_warning "Missing copyright header in $(basename "$file")"
            fi
            
            # Check for basic error handling
            if grep -q "KERN_" "$file"; then
                log_success "✓ Error handling in $(basename "$file")"
            else
                log_warning "Limited error handling in $(basename "$file")"
            fi
        fi
    done
    
    return 0
}

# Main test execution
main() {
    echo "======================================"
    echo "Performance Analysis Framework Test"
    echo "======================================"
    echo
    
    local test_functions=(
        "test_framework_files"
        "test_build_integration"
        "test_kernel_integration"
        "test_api_completeness"
        "test_documentation"
        "test_utility_script"
        "test_code_quality"
    )
    
    local passed=0
    local total=${#test_functions[@]}
    
    for test_func in "${test_functions[@]}"; do
        echo
        if $test_func; then
            ((passed++))
        fi
    done
    
    echo
    echo "======================================"
    echo "Test Summary"
    echo "======================================"
    echo "Passed: $passed/$total tests"
    
    if [ $passed -eq $total ]; then
        log_success "🎉 All tests passed! Performance Analysis Framework is ready."
        echo
        echo "Next steps:"
        echo "1. Build the kernel: cd build-i686 && make -j\$(nproc)"
        echo "2. Run tests: make run-performance-analysis"
        echo "3. Start monitoring: ./scripts/perf-analysis.sh enable"
        echo
        return 0
    else
        local failed=$((total - passed))
        log_warning "⚠️  $failed tests failed. Please review the implementation."
        return 1
    fi
}

# Run the tests
main "$@"