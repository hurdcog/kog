#!/bin/bash
#
# Enhanced Cognitive Synergy Test Script (v2)
# 
# Comprehensive orchestration of build, test, and validation for cognitive synergy
# in the OpenCog Collection with real component testing and detailed diagnostics.
#

set -e

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Logging
log_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
log_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# Counters
TESTS_PASSED=0
TESTS_FAILED=0
TESTS_SKIPPED=0

# Report setup
REPORT_DIR="synergy_reports"
mkdir -p "$REPORT_DIR"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
REPORT_FILE="$REPORT_DIR/synergy_report_$TIMESTAMP.md"

cat > "$REPORT_FILE" << 'EOF'
# Cognitive Synergy Test Report
Generated: $(date)

## Executive Summary
Comprehensive cognitive synergy testing across OpenCog Collection components.

---

EOF

echo "=============================================="
echo "  OpenCog Collection - Cognitive Synergy Test"
echo "=============================================="
echo ""

# Phase 1: Environment
log_info "Phase 1: Environment Setup"
echo "-------------------------------------------"

if command -v python3 &> /dev/null; then
    PYTHON_VERSION=$(python3 --version)
    log_success "Python: $PYTHON_VERSION"
    echo "- ✓ Python: $PYTHON_VERSION" >> "$REPORT_FILE"
else
    log_error "Python 3 not found"
    echo "- ✗ Python: Not found" >> "$REPORT_FILE"
    ((TESTS_FAILED++))
fi

log_info "Checking Python dependencies..."
if python3 -c "import numpy, pandas, sklearn" 2>/dev/null; then
    log_success "Python ML dependencies available"
    echo "- ✓ Python ML dependencies" >> "$REPORT_FILE"
else
    log_warning "Installing Python ML dependencies..."
    pip3 install -q numpy pandas scikit-learn 2>&1 | tail -3
    echo "- ⚠ Python ML dependencies: Installed" >> "$REPORT_FILE"
fi

if command -v cargo &> /dev/null; then
    log_success "Rust: $(cargo --version)"
    echo "- ✓ Rust: $(cargo --version)" >> "$REPORT_FILE"
else
    log_warning "Rust not found"
    echo "- ⚠ Rust: Not found" >> "$REPORT_FILE"
fi

echo "" >> "$REPORT_FILE"
echo "## Test Results" >> "$REPORT_FILE"
echo "" >> "$REPORT_FILE"

# Phase 2: Component Tests
log_info "Phase 2: Component Validation"
echo "-------------------------------------------"

# Python ML
log_info "Test 2.1: Python ML Component"
if [ -f "app.py" ]; then
    START=$(date +%s)
    if python3 app.py > /tmp/ml_test.txt 2>&1; then
        DUR=$(($(date +%s) - START))
        SCORE=$(grep "Test set score" /tmp/ml_test.txt | awk '{print $4}')
        log_success "Python ML passed (${DUR}s, score: $SCORE)"
        echo "### Python ML: ✓ PASS (${DUR}s, score: $SCORE)" >> "$REPORT_FILE"
        ((TESTS_PASSED++))
    else
        log_error "Python ML failed"
        echo "### Python ML: ✗ FAIL" >> "$REPORT_FILE"
        ((TESTS_FAILED++))
    fi
else
    log_warning "app.py not found"
    echo "### Python ML: ⊘ SKIP" >> "$REPORT_FILE"
    ((TESTS_SKIPPED++))
fi

# Orchestrator
log_info "Test 2.2: Synergy Orchestrator"
if [ -f "synergy/core/orchestrator.py" ]; then
    START=$(date +%s)
    if python3 synergy/core/orchestrator.py > /tmp/orch_test.txt 2>&1; then
        DUR=$(($(date +%s) - START))
        log_success "Orchestrator passed (${DUR}s)"
        echo "### Synergy Orchestrator: ✓ PASS (${DUR}s)" >> "$REPORT_FILE"
        ((TESTS_PASSED++))
    else
        log_error "Orchestrator failed"
        echo "### Synergy Orchestrator: ✗ FAIL" >> "$REPORT_FILE"
        ((TESTS_FAILED++))
    fi
else
    log_warning "orchestrator.py not found"
    echo "### Synergy Orchestrator: ⊘ SKIP" >> "$REPORT_FILE"
    ((TESTS_SKIPPED++))
fi

# Monitor
log_info "Test 2.3: Cognitive Monitor"
if [ -f "synergy/monitoring/cognitive_monitor.py" ]; then
    START=$(date +%s)
    if timeout 15 python3 synergy/monitoring/cognitive_monitor.py > /tmp/mon_test.txt 2>&1; then
        DUR=$(($(date +%s) - START))
        log_success "Monitor passed (${DUR}s)"
        echo "### Cognitive Monitor: ✓ PASS (${DUR}s)" >> "$REPORT_FILE"
        ((TESTS_PASSED++))
    else
        log_error "Monitor failed"
        echo "### Cognitive Monitor: ✗ FAIL" >> "$REPORT_FILE"
        ((TESTS_FAILED++))
    fi
else
    log_warning "cognitive_monitor.py not found"
    echo "### Cognitive Monitor: ⊘ SKIP" >> "$REPORT_FILE"
    ((TESTS_SKIPPED++))
fi

# Bridge
log_info "Test 2.4: Hypergraph Bridge"
if [ -f "synergy/core/hypergraph_bridge.py" ]; then
    START=$(date +%s)
    if python3 synergy/core/hypergraph_bridge.py > /tmp/bridge_test.txt 2>&1; then
        DUR=$(($(date +%s) - START))
        log_success "Bridge passed (${DUR}s)"
        echo "### Hypergraph Bridge: ✓ PASS (${DUR}s)" >> "$REPORT_FILE"
        ((TESTS_PASSED++))
    else
        log_error "Bridge failed"
        echo "### Hypergraph Bridge: ✗ FAIL" >> "$REPORT_FILE"
        ((TESTS_FAILED++))
    fi
else
    log_warning "hypergraph_bridge.py not found"
    echo "### Hypergraph Bridge: ⊘ SKIP" >> "$REPORT_FILE"
    ((TESTS_SKIPPED++))
fi

# Summary
TOTAL=$((TESTS_PASSED + TESTS_FAILED + TESTS_SKIPPED))
SUCCESS_RATE=$((TOTAL > 0 ? TESTS_PASSED * 100 / TOTAL : 0))

if [ $SUCCESS_RATE -ge 90 ]; then LEVEL="Excellent"; COLOR=$GREEN
elif [ $SUCCESS_RATE -ge 70 ]; then LEVEL="Good"; COLOR=$BLUE
elif [ $SUCCESS_RATE -ge 50 ]; then LEVEL="Moderate"; COLOR=$YELLOW
else LEVEL="Poor"; COLOR=$RED
fi

echo ""
echo "=============================================="
echo "  Test Summary"
echo "=============================================="
echo -e "Passed:  ${GREEN}$TESTS_PASSED${NC}"
echo -e "Failed:  ${RED}$TESTS_FAILED${NC}"
echo -e "Skipped: ${YELLOW}$TESTS_SKIPPED${NC}"
echo -e "Success: ${COLOR}${SUCCESS_RATE}%${NC}"
echo -e "Synergy: ${COLOR}${LEVEL}${NC}"
echo ""
echo "Report: $REPORT_FILE"
echo "=============================================="

cat >> "$REPORT_FILE" << EOF

---

## Summary

| Metric | Value |
|--------|-------|
| Passed | $TESTS_PASSED |
| Failed | $TESTS_FAILED |
| Skipped | $TESTS_SKIPPED |
| Success Rate | ${SUCCESS_RATE}% |
| Synergy Level | **${LEVEL}** |

*Generated by synergy_enhanced.sh*
EOF

[ $TESTS_FAILED -eq 0 ] && exit 0 || exit 1
