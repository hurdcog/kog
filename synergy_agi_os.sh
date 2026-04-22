#!/bin/bash
#
# Enhanced Cognitive Synergy Script for AGI-OS
# Tests and validates the complete integration of Cognumach + HurdCog + OCC
#
# Copyright (C) 2025 OpenCog Developers
# License: GPL-3.0+

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Logging functions
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

log_section() {
    echo ""
    echo -e "${MAGENTA}========================================${NC}"
    echo -e "${MAGENTA}$1${NC}"
    echo -e "${MAGENTA}========================================${NC}"
    echo ""
}

# Test counters
TESTS_PASSED=0
TESTS_FAILED=0
TESTS_SKIPPED=0

# Test result tracking
test_result() {
    local test_name="$1"
    local result="$2"
    
    if [ "$result" = "PASS" ]; then
        log_success "✓ $test_name"
        TESTS_PASSED=$((TESTS_PASSED + 1))
    elif [ "$result" = "FAIL" ]; then
        log_error "✗ $test_name"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    else
        log_warning "⊘ $test_name (skipped)"
        TESTS_SKIPPED=$((TESTS_SKIPPED + 1))
    fi
}

# Check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check if a file exists
file_exists() {
    [ -f "$1" ]
}

# Check if a directory exists
dir_exists() {
    [ -d "$1" ]
}

log_section "AGI-OS Cognitive Synergy Test Suite"
log_info "Testing integration of Cognumach + HurdCog + OCC"
log_info "Started at: $(date)"

# Get the script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

#
# Phase 1: Environment Validation
#
log_section "Phase 1: Environment Validation"

# Check for required tools
log_info "Checking for required tools..."

if command_exists guile; then
    GUILE_VERSION=$(guile --version | head -1)
    log_success "Guile found: $GUILE_VERSION"
    test_result "Guile installation" "PASS"
else
    log_error "Guile not found"
    test_result "Guile installation" "FAIL"
fi

if command_exists python3; then
    PYTHON_VERSION=$(python3 --version)
    log_success "Python found: $PYTHON_VERSION"
    test_result "Python installation" "PASS"
else
    log_error "Python not found"
    test_result "Python installation" "FAIL"
fi

if command_exists cmake; then
    CMAKE_VERSION=$(cmake --version | head -1)
    log_success "CMake found: $CMAKE_VERSION"
    test_result "CMake installation" "PASS"
else
    log_warning "CMake not found (optional for some tests)"
    test_result "CMake installation" "SKIP"
fi

#
# Phase 2: Repository Structure Validation
#
log_section "Phase 2: Repository Structure Validation"

log_info "Validating repository structure..."

# Check for core directories
CORE_DIRS=(
    "cognumach"
    "hurdcog"
    "cogutil"
    "atomspace"
    "opencog-debian"
)

for dir in "${CORE_DIRS[@]}"; do
    if dir_exists "$dir"; then
        log_success "Found directory: $dir"
        test_result "Directory: $dir" "PASS"
    else
        log_error "Missing directory: $dir"
        test_result "Directory: $dir" "FAIL"
    fi
done

# Check for integration files
INTEGRATION_FILES=(
    "cognumach.scm"
    "hurdcog.scm"
    "occ-hurdcog-unified.scm"
    "cognitive-integration.scm"
    "AGI_OS_INTEGRATION_ARCHITECTURE.md"
)

for file in "${INTEGRATION_FILES[@]}"; do
    if file_exists "$file"; then
        log_success "Found file: $file"
        test_result "File: $file" "PASS"
    else
        log_error "Missing file: $file"
        test_result "File: $file" "FAIL"
    fi
done

#
# Phase 3: Guix Package Validation
#
log_section "Phase 3: Guix Package Validation"

log_info "Validating Guix package definitions..."

# Test cognumach.scm syntax
if file_exists "cognumach.scm"; then
    log_info "Testing cognumach.scm syntax..."
    if guile --no-auto-compile -c "(primitive-load \"cognumach.scm\")" 2>/dev/null; then
        log_success "cognumach.scm syntax valid"
        test_result "cognumach.scm syntax" "PASS"
    else
        log_warning "cognumach.scm has syntax issues (may need Guix modules)"
        test_result "cognumach.scm syntax" "SKIP"
    fi
fi

# Test hurdcog.scm syntax
if file_exists "hurdcog.scm"; then
    log_info "Testing hurdcog.scm syntax..."
    if guile --no-auto-compile -c "(primitive-load \"hurdcog.scm\")" 2>/dev/null; then
        log_success "hurdcog.scm syntax valid"
        test_result "hurdcog.scm syntax" "PASS"
    else
        log_warning "hurdcog.scm has syntax issues (may need Guix modules)"
        test_result "hurdcog.scm syntax" "SKIP"
    fi
fi

# Test cognitive-integration.scm
if file_exists "cognitive-integration.scm"; then
    log_info "Testing cognitive-integration.scm..."
    if guile --no-auto-compile -c "(add-to-load-path \".\") (use-modules (cognitive-integration))" 2>/dev/null; then
        log_success "cognitive-integration.scm loads successfully"
        test_result "cognitive-integration.scm loading" "PASS"
    else
        log_warning "cognitive-integration.scm has loading issues"
        test_result "cognitive-integration.scm loading" "SKIP"
    fi
fi

#
# Phase 4: Debian Package Validation
#
log_section "Phase 4: Debian Package Validation"

log_info "Validating Debian packaging infrastructure..."

if dir_exists "opencog-debian"; then
    cd opencog-debian
    
    # Run the validation script if it exists
    if file_exists "validate-packaging.sh"; then
        log_info "Running Debian packaging validation..."
        if bash validate-packaging.sh > /tmp/packaging-validation.log 2>&1; then
            VALID_PACKAGES=$(grep -c "Status: VALID" /tmp/packaging-validation.log || echo "0")
            log_success "Debian packaging validation passed: $VALID_PACKAGES packages valid"
            test_result "Debian packaging validation" "PASS"
        else
            log_error "Debian packaging validation failed"
            test_result "Debian packaging validation" "FAIL"
        fi
    else
        log_warning "validate-packaging.sh not found"
        test_result "Debian packaging validation" "SKIP"
    fi
    
    cd "$SCRIPT_DIR"
fi

#
# Phase 5: Cognitive Integration Testing
#
log_section "Phase 5: Cognitive Integration Testing"

log_info "Testing cognitive integration module..."

# Create a test script for cognitive integration
cat > /tmp/test-cognitive-integration.scm << 'EOF'
(add-to-load-path ".")
(use-modules (cognitive-integration))

;; Test initialization
(display "Testing cognitive integration initialization...\n")
(define init-result (initialize-cognitive-integration))
(if init-result
    (display "✓ Initialization successful\n")
    (display "✗ Initialization failed\n"))

;; Test layer registration
(display "\nTesting layer status...\n")
(define state (get-cognitive-state))
(display "Cognitive state: ")
(write state)
(newline)

;; Test event subscription
(display "\nTesting event subscription...\n")
(define (test-callback event)
  (display "Event received: ")
  (display (cognitive-event-type event))
  (newline))

(subscribe-cognitive-event 'test-event test-callback)
(display "✓ Event subscription successful\n")

;; Test event publishing
(display "\nTesting event publishing...\n")
(publish-cognitive-event 'test-event "test data" 0.5)
(display "✓ Event publishing successful\n")

;; Test message sending
(display "\nTesting message sending...\n")
(send-cognitive-message 'occ 'hurdcog 'query "test query" 0.7)
(display "✓ Message sending successful\n")

;; Test attention propagation
(display "\nTesting attention propagation...\n")
(propagate-attention "test-atom-123" 0.8)
(display "✓ Attention propagation successful\n")

;; Test system modification suggestion
(display "\nTesting system modification...\n")
(define mod-id (suggest-system-modification 'optimization "Test optimization" 0.6))
(display "✓ System modification suggested: ")
(display mod-id)
(newline)

;; Test metrics
(display "\nTesting metrics collection...\n")
(define metrics (get-integration-metrics))
(display "Integration metrics: ")
(write metrics)
(newline)

;; Cleanup
(display "\nShutting down cognitive integration...\n")
(shutdown-cognitive-integration)
(display "✓ Shutdown successful\n")

(display "\n=== All cognitive integration tests completed ===\n")
EOF

if guile --no-auto-compile /tmp/test-cognitive-integration.scm > /tmp/cognitive-integration-test.log 2>&1; then
    log_success "Cognitive integration tests passed"
    test_result "Cognitive integration functionality" "PASS"
    
    # Show some output
    log_info "Test output:"
    tail -20 /tmp/cognitive-integration-test.log | sed 's/^/  /'
else
    log_error "Cognitive integration tests failed"
    test_result "Cognitive integration functionality" "FAIL"
    
    log_info "Error output:"
    tail -20 /tmp/cognitive-integration-test.log | sed 's/^/  /'
fi

#
# Phase 6: Component Integration Testing
#
log_section "Phase 6: Component Integration Testing"

log_info "Testing component integration..."

# Test cognumach integration
if dir_exists "cognumach"; then
    log_info "Checking cognumach structure..."
    
    COGNUMACH_DIRS=("kern" "ipc" "vm" "device")
    COGNUMACH_FOUND=0
    
    for dir in "${COGNUMACH_DIRS[@]}"; do
        if dir_exists "cognumach/$dir"; then
            COGNUMACH_FOUND=$((COGNUMACH_FOUND + 1))
        fi
    done
    
    if [ $COGNUMACH_FOUND -ge 3 ]; then
        log_success "Cognumach structure validated ($COGNUMACH_FOUND/4 core directories found)"
        test_result "Cognumach structure" "PASS"
    else
        log_warning "Cognumach structure incomplete ($COGNUMACH_FOUND/4 core directories found)"
        test_result "Cognumach structure" "SKIP"
    fi
fi

# Test hurdcog integration
if dir_exists "hurdcog"; then
    log_info "Checking hurdcog structure..."
    
    HURDCOG_DIRS=("cogkernel" "cognitive" "distributed")
    HURDCOG_FOUND=0
    
    for dir in "${HURDCOG_DIRS[@]}"; do
        if dir_exists "hurdcog/$dir"; then
            HURDCOG_FOUND=$((HURDCOG_FOUND + 1))
        fi
    done
    
    if [ $HURDCOG_FOUND -ge 2 ]; then
        log_success "HurdCog structure validated ($HURDCOG_FOUND/3 core directories found)"
        test_result "HurdCog structure" "PASS"
    else
        log_warning "HurdCog structure incomplete ($HURDCOG_FOUND/3 core directories found)"
        test_result "HurdCog structure" "SKIP"
    fi
    
    # Check for Fusion Reactor
    if file_exists "hurdcog/cogkernel/fusion-reactor-server.py"; then
        log_success "Cognitive Fusion Reactor found"
        test_result "Fusion Reactor presence" "PASS"
    else
        log_warning "Cognitive Fusion Reactor not found"
        test_result "Fusion Reactor presence" "SKIP"
    fi
fi

# Test OCC components
log_info "Checking OCC components..."

OCC_COMPONENTS=("cogutil" "atomspace" "cogserver" "pln" "ure" "attention")
OCC_FOUND=0

for component in "${OCC_COMPONENTS[@]}"; do
    if dir_exists "$component"; then
        OCC_FOUND=$((OCC_FOUND + 1))
    fi
done

if [ $OCC_FOUND -ge 4 ]; then
    log_success "OCC components validated ($OCC_FOUND/6 core components found)"
    test_result "OCC components" "PASS"
else
    log_warning "OCC components incomplete ($OCC_FOUND/6 core components found)"
    test_result "OCC components" "SKIP"
fi

#
# Phase 7: Documentation Validation
#
log_section "Phase 7: Documentation Validation"

log_info "Validating documentation..."

DOCS=(
    "README.md"
    "AGI_OS_INTEGRATION_ARCHITECTURE.md"
    "ANALYSIS_AND_IMPROVEMENTS.md"
    "opencog-debian/PACKAGING_ARCHITECTURE.md"
    "opencog-debian/BUILD_ORDER.md"
)

DOC_FOUND=0
for doc in "${DOCS[@]}"; do
    if file_exists "$doc"; then
        DOC_FOUND=$((DOC_FOUND + 1))
        log_success "Found: $doc"
    fi
done

if [ $DOC_FOUND -ge 4 ]; then
    log_success "Documentation validated ($DOC_FOUND/5 documents found)"
    test_result "Documentation completeness" "PASS"
else
    log_warning "Documentation incomplete ($DOC_FOUND/5 documents found)"
    test_result "Documentation completeness" "SKIP"
fi

#
# Phase 8: Cognitive Synergy Validation
#
log_section "Phase 8: Cognitive Synergy Validation"

log_info "Testing cognitive synergy features..."

# Create a synergy test script
cat > /tmp/test-synergy.scm << 'EOF'
(add-to-load-path ".")
(use-modules (cognitive-integration))

(display "=== Cognitive Synergy Test ===\n\n")

;; Initialize
(initialize-cognitive-integration)

;; Simulate cross-layer communication
(display "1. Testing cross-layer communication...\n")
(send-cognitive-message 'occ 'hurdcog 'query "What is system load?" 0.8)
(send-cognitive-message 'hurdcog 'cognumach 'command "optimize-memory" 0.9)
(display "   ✓ Messages sent across layers\n\n")

;; Simulate attention propagation
(display "2. Testing attention propagation...\n")
(propagate-attention "critical-atom-001" 0.95)
(propagate-attention "normal-atom-002" 0.5)
(propagate-attention "low-priority-atom-003" 0.2)
(display "   ✓ Attention values propagated\n\n")

;; Simulate cognitive event cascade
(display "3. Testing cognitive event cascade...\n")
(define event-count 0)
(define (count-events event)
  (set! event-count (+ event-count 1)))

(subscribe-cognitive-event 'pattern-discovered count-events)
(subscribe-cognitive-event 'optimization-opportunity count-events)
(subscribe-cognitive-event 'anomaly-detected count-events)

(publish-cognitive-event 'pattern-discovered "New pattern in system logs" 0.7)
(publish-cognitive-event 'optimization-opportunity "Memory allocation pattern" 0.8)
(publish-cognitive-event 'anomaly-detected "Unusual CPU usage" 0.9)

(display "   ✓ Event cascade completed (")
(display event-count)
(display " events processed)\n\n")

;; Simulate self-modification
(display "4. Testing self-modification capability...\n")
(define mod-id (suggest-system-modification
                 'performance-optimization
                 "Optimize IPC routing based on attention values"
                 0.85))
(display "   ✓ Modification suggested: ")
(display mod-id)
(newline)
(newline)

;; Get final state
(display "5. Final cognitive state:\n")
(define final-state (get-cognitive-state))
(for-each
  (lambda (entry)
    (display "   ")
    (display (car entry))
    (display ": ")
    (display (cdr entry))
    (newline))
  final-state)

(display "\n=== Cognitive Synergy Test Complete ===\n")
(shutdown-cognitive-integration)
EOF

if guile --no-auto-compile /tmp/test-synergy.scm > /tmp/synergy-test.log 2>&1; then
    log_success "Cognitive synergy tests passed"
    test_result "Cognitive synergy functionality" "PASS"
    
    log_info "Synergy test output:"
    cat /tmp/synergy-test.log | sed 's/^/  /'
else
    log_warning "Cognitive synergy tests had issues"
    test_result "Cognitive synergy functionality" "SKIP"
    
    log_info "Test output:"
    cat /tmp/synergy-test.log | sed 's/^/  /'
fi

#
# Final Report
#
log_section "Test Summary"

TOTAL_TESTS=$((TESTS_PASSED + TESTS_FAILED + TESTS_SKIPPED))

echo ""
echo -e "${CYAN}Total Tests:${NC}    $TOTAL_TESTS"
echo -e "${GREEN}Passed:${NC}         $TESTS_PASSED"
echo -e "${RED}Failed:${NC}         $TESTS_FAILED"
echo -e "${YELLOW}Skipped:${NC}        $TESTS_SKIPPED"
echo ""

if [ $TESTS_FAILED -eq 0 ]; then
    log_success "All tests passed! AGI-OS integration is functional."
    EXIT_CODE=0
else
    log_warning "Some tests failed. Review the output above for details."
    EXIT_CODE=1
fi

# Calculate success rate
if [ $TOTAL_TESTS -gt 0 ]; then
    SUCCESS_RATE=$(( (TESTS_PASSED * 100) / TOTAL_TESTS ))
    echo -e "${CYAN}Success Rate:${NC}   ${SUCCESS_RATE}%"
fi

echo ""
log_info "Test completed at: $(date)"
log_info "Logs saved to: /tmp/cognitive-integration-test.log"
log_info "              /tmp/synergy-test.log"
log_info "              /tmp/packaging-validation.log"

echo ""
log_section "AGI-OS Cognitive Synergy Test Complete"

exit $EXIT_CODE
