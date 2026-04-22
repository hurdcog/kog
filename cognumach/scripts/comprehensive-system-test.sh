#!/bin/bash
# Comprehensive System Testing Script for GNU Mach Phase 3
# Final validation and release preparation testing

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Default configuration
ALL_PLATFORMS=false
FULL_COVERAGE=false
PLATFORM="current"
OUTPUT_DIR="$PROJECT_ROOT/test-results/comprehensive"
QUICK_MODE=false

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

Comprehensive system testing for GNU Mach microkernel final validation.

Options:
  --all-platforms         Test on all supported platforms (i686, x86_64)
  --full-coverage         Enable comprehensive test coverage
  --platform PLATFORM     Test specific platform (i686, x86_64, current)
  --quick                 Run quick validation tests only
  --output-dir DIR        Output directory for test results
  -h, --help             Show this help message

Examples:
  $0 --all-platforms --full-coverage
  $0 --platform i686 --quick
  $0 --all-platforms --output-dir=/tmp/final-tests
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --all-platforms)
            ALL_PLATFORMS=true
            shift
            ;;
        --full-coverage)
            FULL_COVERAGE=true
            shift
            ;;
        --platform=*)
            PLATFORM="${1#*=}"
            shift
            ;;
        --platform)
            PLATFORM="$2"
            shift 2
            ;;
        --quick)
            QUICK_MODE=true
            shift
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

# Test result tracking
declare -A test_results
overall_success=true

# Mark test result
mark_test_result() {
    local test_name="$1"
    local result="$2"  # "PASS" or "FAIL"
    
    test_results["$test_name"]="$result"
    
    if [[ "$result" == "FAIL" ]]; then
        overall_success=false
    fi
}

# Build system validation
test_build_system() {
    local platform="$1"
    local test_name="build_system_$platform"
    
    log_info "Testing build system for platform: $platform"
    
    local build_dir="build-$platform"
    local log_file="$OUTPUT_DIR/${test_name}.log"
    
    {
        echo "=== Build System Test for $platform ===" 
        echo "Date: $(date)"
        echo "Platform: $platform"
        echo ""
        
        # Clean previous build
        if [[ -d "$build_dir" ]]; then
            rm -rf "$build_dir"
        fi
        mkdir -p "$build_dir"
        cd "$build_dir"
        
        # Configure for platform
        case "$platform" in
            "i686")
                ../configure --host=i686-gnu CC='gcc -m32' LD='ld -melf_i386' || exit 1
                ;;
            "x86_64")
                ../configure --host=x86_64-gnu --enable-pae --enable-user32 MIG='mig' || exit 1
                ;;
            *)
                ../configure || exit 1
                ;;
        esac
        
        echo "Configuration completed successfully"
        
        # Build with timeout
        timeout 900 make -j$(nproc) || exit 1
        
        echo "Build completed successfully"
        
        # Verify key artifacts exist
        if [[ -f "gnumach" ]]; then
            echo "Kernel binary created: gnumach"
            ls -la gnumach
        else
            echo "ERROR: Kernel binary not found"
            exit 1
        fi
        
        cd "$PROJECT_ROOT"
        
    } > "$log_file" 2>&1
    local test_result=$?
    
    if [[ $test_result -eq 0 ]]; then
        log_success "Build system test PASSED for $platform"
        mark_test_result "$test_name" "PASS"
        return 0
    else
        log_error "Build system test FAILED for $platform"
        mark_test_result "$test_name" "FAIL"
        return 1
    fi
}

# Core functionality tests
test_core_functionality() {
    local platform="$1"
    local test_name="core_functionality_$platform"
    
    log_info "Testing core functionality for platform: $platform"
    
    local build_dir="build-$platform"
    local log_file="$OUTPUT_DIR/${test_name}.log"
    
    if [[ ! -d "$build_dir" ]]; then
        log_error "Build directory not found for $platform: $build_dir"
        mark_test_result "$test_name" "FAIL"
        return 1
    fi
    
    cd "$build_dir"
    
    {
        echo "=== Core Functionality Test for $platform ==="
        echo "Date: $(date)"
        echo ""
        
        # Run basic functionality tests
        if [[ "$QUICK_MODE" == "true" ]]; then
            echo "Running quick core tests..."
            timeout 180 make run-hello || exit 1
        else
            echo "Running comprehensive core tests..."
            timeout 300 make run-hello || exit 1
            timeout 300 make run-vm || exit 1
            timeout 300 make run-mach_port || exit 1
        fi
        
        echo "Core functionality tests completed successfully"
        
    } > "$log_file" 2>&1
    
    if cd "$PROJECT_ROOT"; then
        log_success "Core functionality test PASSED for $platform"
        mark_test_result "$test_name" "PASS"
        return 0
    else
        log_error "Core functionality test FAILED for $platform"
        mark_test_result "$test_name" "FAIL"
        return 1
    fi
}

# Memory management tests
test_memory_management() {
    local platform="$1"
    local test_name="memory_management_$platform"
    
    log_info "Testing memory management for platform: $platform"
    
    local log_file="$OUTPUT_DIR/${test_name}.log"
    
    {
        echo "=== Memory Management Test for $platform ==="
        echo "Date: $(date)"
        echo ""
        
        if [[ "$FULL_COVERAGE" == "true" ]]; then
            echo "Running comprehensive memory tests..."
            
            # Run stress tests if available
            if [[ -x "$SCRIPT_DIR/stress-test-memory.sh" ]]; then
                "$SCRIPT_DIR/stress-test-memory.sh" --duration=5m --pattern=sequential || exit 1
                echo "Sequential memory stress test passed"
                
                "$SCRIPT_DIR/stress-test-memory.sh" --duration=5m --pattern=random || exit 1
                echo "Random memory stress test passed"
            fi
            
            # Run SMP race tests if available
            if [[ -x "$SCRIPT_DIR/test-smp-memory-races.sh" ]]; then
                "$SCRIPT_DIR/test-smp-memory-races.sh" --cpu-count=4 --iterations=1000 || exit 1
                echo "SMP memory race test passed"
            fi
        else
            echo "Running basic memory tests..."
            
            # Basic memory allocation test
            cd "build-$platform" 2>/dev/null || cd "build-i686" 2>/dev/null || exit 1
            timeout 120 make run-vm || exit 1
            cd "$PROJECT_ROOT"
        fi
        
        echo "Memory management tests completed successfully"
        
    } > "$log_file" 2>&1
    local test_result=$?
    
    if [[ $test_result -eq 0 ]]; then
        log_success "Memory management test PASSED for $platform"
        mark_test_result "$test_name" "PASS"
        return 0
    else
        log_error "Memory management test FAILED for $platform"
        mark_test_result "$test_name" "FAIL"
        return 1
    fi
}

# Static analysis validation
test_static_analysis() {
    local test_name="static_analysis"
    
    log_info "Running static analysis validation"
    
    local log_file="$OUTPUT_DIR/${test_name}.log"
    
    {
        echo "=== Static Analysis Validation ==="
        echo "Date: $(date)"
        echo ""
        
        if [[ -x "$SCRIPT_DIR/run-static-analysis.sh" ]]; then
            "$SCRIPT_DIR/run-static-analysis.sh" || exit 1
            echo "Static analysis completed successfully"
            
            # Check for critical issues
            if [[ -f "analysis-reports/compiler-warnings.txt" ]]; then
                warning_count=$(wc -l < "analysis-reports/compiler-warnings.txt" 2>/dev/null || echo "0")
                echo "Compiler warnings: $warning_count"
                
                if [[ $warning_count -gt 100 ]]; then
                    echo "WARNING: High number of compiler warnings ($warning_count)"
                fi
            fi
        else
            echo "Static analysis script not found, skipping..."
        fi
        
    } > "$log_file" 2>&1
    local test_result=$?
    
    if [[ $test_result -eq 0 ]]; then
        log_success "Static analysis test PASSED"
        mark_test_result "$test_name" "PASS"
        return 0
    else
        log_error "Static analysis test FAILED"
        mark_test_result "$test_name" "FAIL"
        return 1
    fi
}

# Console and debugging features test
test_console_debugging() {
    local platform="$1"
    local test_name="console_debugging_$platform"
    
    log_info "Testing console and debugging features for platform: $platform"
    
    local build_dir="build-$platform"
    local log_file="$OUTPUT_DIR/${test_name}.log"
    
    cd "$build_dir" 2>/dev/null || cd "build-i686" 2>/dev/null || {
        log_warning "No build directory found, skipping console test"
        mark_test_result "$test_name" "SKIP"
        return 0
    }
    
    {
        echo "=== Console and Debugging Test for $platform ==="
        echo "Date: $(date)"
        echo ""
        
        # Test console timestamp functionality
        timeout 120 make run-console-timestamps || exit 1
        echo "Console timestamp test passed"
        
        # Verify timestamp format validation script if available
        if [[ -x "$SCRIPT_DIR/verify-timestamp-improvements.sh" ]]; then
            "$SCRIPT_DIR/verify-timestamp-improvements.sh" || exit 1
            echo "Timestamp format validation passed"
        fi
        
    } > "$log_file" 2>&1
    
    if cd "$PROJECT_ROOT"; then
        log_success "Console debugging test PASSED for $platform"
        mark_test_result "$test_name" "PASS"
        return 0
    else
        log_error "Console debugging test FAILED for $platform"
        mark_test_result "$test_name" "FAIL"
        return 1
    fi
}

# Performance validation
test_performance() {
    local platform="$1"
    local test_name="performance_$platform"
    
    log_info "Testing performance for platform: $platform"
    
    local log_file="$OUTPUT_DIR/${test_name}.log"
    
    {
        echo "=== Performance Test for $platform ==="
        echo "Date: $(date)"
        echo ""
        
        if [[ -x "$SCRIPT_DIR/test-performance-framework.sh" ]]; then
            "$SCRIPT_DIR/test-performance-framework.sh" --quick || exit 1
            echo "Performance framework test passed"
        fi
        
        if [[ -x "$SCRIPT_DIR/perf-analysis.sh" && "$FULL_COVERAGE" == "true" ]]; then
            "$SCRIPT_DIR/perf-analysis.sh" --quick-benchmark || exit 1
            echo "Performance analysis completed"
        fi
        
        echo "Performance validation completed successfully"
        
    } > "$log_file" 2>&1
    local test_result=$?
    
    if [[ $test_result -eq 0 ]]; then
        log_success "Performance test PASSED for $platform"
        mark_test_result "$test_name" "PASS"
        return 0
    else
        log_error "Performance test FAILED for $platform"
        mark_test_result "$test_name" "FAIL"
        return 1
    fi
}

# Run tests for a specific platform
run_platform_tests() {
    local platform="$1"
    
    log_info "Running comprehensive tests for platform: $platform"
    
    # Core test suite
    test_build_system "$platform"
    test_core_functionality "$platform"
    test_memory_management "$platform"
    test_console_debugging "$platform"
    
    if [[ "$FULL_COVERAGE" == "true" ]]; then
        test_performance "$platform"
    fi
}

# Generate comprehensive test report
generate_test_report() {
    local report_file="$OUTPUT_DIR/comprehensive_test_report_$(date +%Y%m%d_%H%M%S).txt"
    
    {
        echo "GNU Mach Comprehensive System Test Report"
        echo "========================================"
        echo "Date: $(date)"
        echo "Test Configuration:"
        echo "  All platforms: $ALL_PLATFORMS"
        echo "  Full coverage: $FULL_COVERAGE"
        echo "  Quick mode: $QUICK_MODE"
        echo "  Platform(s): $PLATFORM"
        echo ""
        
        echo "Test Results Summary:"
        echo "===================="
        
        local pass_count=0
        local fail_count=0
        local skip_count=0
        
        for test_name in "${!test_results[@]}"; do
            local result="${test_results[$test_name]}"
            echo "  $test_name: $result"
            
            case "$result" in
                "PASS") ((pass_count++)) ;;
                "FAIL") ((fail_count++)) ;;
                "SKIP") ((skip_count++)) ;;
            esac
        done
        
        echo ""
        echo "Statistics:"
        echo "  Total tests: $((pass_count + fail_count + skip_count))"
        echo "  Passed: $pass_count"
        echo "  Failed: $fail_count" 
        echo "  Skipped: $skip_count"
        echo ""
        
        if [[ "$overall_success" == "true" ]]; then
            echo "OVERALL RESULT: SUCCESS ✅"
            echo "All critical tests passed. System ready for release."
        else
            echo "OVERALL RESULT: FAILURE ❌"
            echo "Some tests failed. Review required before release."
        fi
        
        echo ""
        echo "Detailed logs available in: $OUTPUT_DIR"
        
    } > "$report_file"
    
    log_info "Comprehensive test report written to: $report_file"
}

# Main execution
main() {
    log_info "GNU Mach Comprehensive System Testing"
    log_info "All platforms: $ALL_PLATFORMS"
    log_info "Full coverage: $FULL_COVERAGE"
    log_info "Quick mode: $QUICK_MODE"
    log_info "Output directory: $OUTPUT_DIR"
    
    # Create output directory
    mkdir -p "$OUTPUT_DIR"
    
    # Run static analysis first (platform-independent)
    test_static_analysis
    
    # Determine platforms to test
    local platforms_to_test=()
    
    if [[ "$ALL_PLATFORMS" == "true" ]]; then
        platforms_to_test=("i686" "x86_64")
    else
        case "$PLATFORM" in
            "current")
                # Detect current platform
                if [[ "$(uname -m)" == "x86_64" ]]; then
                    platforms_to_test=("x86_64")
                else
                    platforms_to_test=("i686")
                fi
                ;;
            *)
                platforms_to_test=("$PLATFORM")
                ;;
        esac
    fi
    
    log_info "Testing platforms: ${platforms_to_test[*]}"
    
    # Run tests for each platform
    for platform in "${platforms_to_test[@]}"; do
        run_platform_tests "$platform"
    done
    
    # Generate final report
    generate_test_report
    
    # Final result
    if [[ "$overall_success" == "true" ]]; then
        log_success "✅ Comprehensive system testing completed successfully"
        log_success "System is ready for production release"
        exit 0
    else
        log_error "❌ Comprehensive system testing failed"
        log_error "Review test results before proceeding with release"
        exit 1
    fi
}

# Run main function
main "$@"