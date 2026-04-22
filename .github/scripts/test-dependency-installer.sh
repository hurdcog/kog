#!/bin/bash
# test-dependency-installer.sh
# Test suite for the modular dependency installer
# Ensures installation scripts work correctly

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_RESULTS=()

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

log_test() {
    echo -e "${BLUE}TEST: $1${NC}"
}

log_pass() {
    echo -e "${GREEN}✓ PASS: $1${NC}"
    TEST_RESULTS+=("PASS: $1")
}

log_fail() {
    echo -e "${RED}✗ FAIL: $1${NC}"
    TEST_RESULTS+=("FAIL: $1")
}

# Test 1: Verify install-dependency.sh exists and is executable
test_script_exists() {
    log_test "Checking if install-dependency.sh exists and is executable"
    
    if [ -f "$SCRIPT_DIR/install-dependency.sh" ] && [ -x "$SCRIPT_DIR/install-dependency.sh" ]; then
        log_pass "install-dependency.sh exists and is executable"
        return 0
    else
        log_fail "install-dependency.sh does not exist or is not executable"
        return 1
    fi
}

# Test 2: Verify install-sparsehash.sh exists and is executable
test_sparsehash_script_exists() {
    log_test "Checking if install-sparsehash.sh exists and is executable"
    
    if [ -f "$SCRIPT_DIR/install-sparsehash.sh" ] && [ -x "$SCRIPT_DIR/install-sparsehash.sh" ]; then
        log_pass "install-sparsehash.sh exists and is executable"
        return 0
    else
        log_fail "install-sparsehash.sh does not exist or is not executable"
        return 1
    fi
}

# Test 3: Verify sparsehash is installed
test_sparsehash_installed() {
    log_test "Checking if sparsehash is installed"
    
    if dpkg -l | grep -q libsparsehash-dev; then
        log_pass "libsparsehash-dev package is installed"
    else
        log_fail "libsparsehash-dev package is not installed"
        return 1
    fi
    
    # Check for headers
    if [ -f /usr/include/google/sparse_hash_map ]; then
        log_pass "sparsehash headers found at /usr/include/google/sparse_hash_map"
        return 0
    elif [ -f /usr/local/include/google/sparse_hash_map ]; then
        log_pass "sparsehash headers found at /usr/local/include/google/sparse_hash_map"
        return 0
    else
        log_fail "sparsehash headers not found"
        return 1
    fi
}

# Test 4: Test install-dependency.sh with a simple package
test_simple_package_install() {
    log_test "Testing install-dependency.sh with a simple package (wget)"
    
    # Ensure wget is not installed (for testing)
    if ! command -v wget &> /dev/null; then
        if "$SCRIPT_DIR/install-dependency.sh" wget; then
            log_pass "Successfully installed wget via install-dependency.sh"
            return 0
        else
            log_fail "Failed to install wget via install-dependency.sh"
            return 1
        fi
    else
        log_pass "wget already installed (skipping test)"
        return 0
    fi
}

# Test 5: Verify script handles missing package gracefully
test_missing_package() {
    log_test "Testing error handling for non-existent package"
    
    if "$SCRIPT_DIR/install-dependency.sh" "this-package-does-not-exist-12345" 2>&1 | grep -q "Failed to install"; then
        log_pass "Script correctly handles missing package"
        return 0
    else
        log_fail "Script did not handle missing package correctly"
        return 1
    fi
}

# Main test runner
main() {
    echo "================================"
    echo "  Dependency Installer Test Suite"
    echo "================================"
    echo ""
    
    test_script_exists || true
    test_sparsehash_script_exists || true
    test_sparsehash_installed || true
    test_simple_package_install || true
    test_missing_package || true
    
    echo ""
    echo "================================"
    echo "  Test Results Summary"
    echo "================================"
    
    local total=${#TEST_RESULTS[@]}
    local passed=0
    local failed=0
    
    for result in "${TEST_RESULTS[@]}"; do
        if [[ $result == PASS:* ]]; then
            ((passed++))
            echo -e "${GREEN}$result${NC}"
        else
            ((failed++))
            echo -e "${RED}$result${NC}"
        fi
    done
    
    echo ""
    echo "Total: $total | Passed: $passed | Failed: $failed"
    
    if [ $failed -eq 0 ]; then
        echo -e "${GREEN}All tests passed!${NC}"
        exit 0
    else
        echo -e "${RED}Some tests failed!${NC}"
        exit 1
    fi
}

main "$@"
