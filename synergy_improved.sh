#!/bin/bash
# Enhanced Synergy Script for OpenCog Collection
# Orchestrates build, test, and validation of cognitive synergy

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LOG_DIR="${SCRIPT_DIR}/logs"
SYNERGY_REPORT="${SCRIPT_DIR}/synergy_report_enhanced.json"

# Create log directory
mkdir -p "${LOG_DIR}"

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] [INFO] $1" >> "${LOG_DIR}/synergy.log"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] [SUCCESS] $1" >> "${LOG_DIR}/synergy.log"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] [WARNING] $1" >> "${LOG_DIR}/synergy.log"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] [ERROR] $1" >> "${LOG_DIR}/synergy.log"
}

# Banner
echo "=========================================="
echo "  OpenCog Collection - Synergy Check"
echo "  Enhanced Cognitive Architecture Build"
echo "=========================================="
echo ""

# Start timing
START_TIME=$(date +%s)

# Phase 1: Environment Setup
log_info "Phase 1: Setting up environment..."

# Check for required tools
check_tool() {
    if command -v "$1" &> /dev/null; then
        log_success "$1 found"
        return 0
    else
        log_warning "$1 not found"
        return 1
    fi
}

log_info "Checking for required tools..."
check_tool python3 || log_warning "Python3 not available"
check_tool guile || log_warning "Guile not available"
check_tool cmake || log_warning "CMake not available"
check_tool cargo || log_warning "Cargo (Rust) not available"

# Phase 2: Syntax Validation
log_info "Phase 2: Validating package definitions..."

if [ -f "${SCRIPT_DIR}/test-guix-syntax.sh" ]; then
    log_info "Running Guix syntax tests..."
    if bash "${SCRIPT_DIR}/test-guix-syntax.sh" > "${LOG_DIR}/guix-syntax.log" 2>&1; then
        log_success "Guix syntax validation passed"
    else
        log_warning "Guix syntax validation had issues (see ${LOG_DIR}/guix-syntax.log)"
    fi
else
    log_warning "test-guix-syntax.sh not found, skipping syntax validation"
fi

# Phase 3: Synergy Module Tests
log_info "Phase 3: Testing synergy modules..."

if [ -f "${SCRIPT_DIR}/tests/synergy/test_synergy_integration.py" ]; then
    log_info "Running synergy integration tests..."
    cd "${SCRIPT_DIR}"
    if python3 tests/synergy/test_synergy_integration.py > "${LOG_DIR}/synergy-tests.log" 2>&1; then
        log_success "Synergy integration tests passed"
        SYNERGY_TESTS_PASSED=true
    else
        log_error "Synergy integration tests failed (see ${LOG_DIR}/synergy-tests.log)"
        SYNERGY_TESTS_PASSED=false
    fi
else
    log_warning "Synergy integration tests not found"
    SYNERGY_TESTS_PASSED=false
fi

# Phase 4: Component Verification
log_info "Phase 4: Verifying cognitive components..."

# Check for key directories
COMPONENTS=(
    "atomspace"
    "cogserver"
    "cogutil"
    "learn"
    "matrix"
    "agents"
    "sensory"
)

FOUND_COMPONENTS=0
TOTAL_COMPONENTS=${#COMPONENTS[@]}

for component in "${COMPONENTS[@]}"; do
    if [ -d "${SCRIPT_DIR}/${component}" ]; then
        log_success "Component found: ${component}"
        ((FOUND_COMPONENTS++))
    else
        log_warning "Component not found: ${component}"
    fi
done

COMPONENT_COVERAGE=$(echo "scale=2; ${FOUND_COMPONENTS} / ${TOTAL_COMPONENTS} * 100" | bc)
log_info "Component coverage: ${COMPONENT_COVERAGE}% (${FOUND_COMPONENTS}/${TOTAL_COMPONENTS})"

# Phase 5: Hypergraph Bridge Test
log_info "Phase 5: Testing hypergraph bridge..."

if [ -f "${SCRIPT_DIR}/synergy/bridges/hypergraph_bridge.py" ]; then
    log_info "Running hypergraph bridge test..."
    if python3 "${SCRIPT_DIR}/synergy/bridges/hypergraph_bridge.py" > "${LOG_DIR}/hypergraph-bridge.log" 2>&1; then
        log_success "Hypergraph bridge test passed"
        BRIDGE_TEST_PASSED=true
    else
        log_error "Hypergraph bridge test failed (see ${LOG_DIR}/hypergraph-bridge.log)"
        BRIDGE_TEST_PASSED=false
    fi
else
    log_warning "Hypergraph bridge not found"
    BRIDGE_TEST_PASSED=false
fi

# Phase 6: Cognitive Monitor Test
log_info "Phase 6: Testing cognitive monitor..."

if [ -f "${SCRIPT_DIR}/synergy/monitors/cognitive_monitor.py" ]; then
    log_info "Running cognitive monitor test..."
    if python3 "${SCRIPT_DIR}/synergy/monitors/cognitive_monitor.py" > "${LOG_DIR}/cognitive-monitor.log" 2>&1; then
        log_success "Cognitive monitor test passed"
        MONITOR_TEST_PASSED=true
    else
        log_error "Cognitive monitor test failed (see ${LOG_DIR}/cognitive-monitor.log)"
        MONITOR_TEST_PASSED=false
    fi
else
    log_warning "Cognitive monitor not found"
    MONITOR_TEST_PASSED=false
fi

# Phase 7: Synergy Orchestrator Test
log_info "Phase 7: Testing synergy orchestrator..."

if [ -f "${SCRIPT_DIR}/synergy/core/synergy_orchestrator.py" ]; then
    log_info "Running synergy orchestrator test..."
    if python3 "${SCRIPT_DIR}/synergy/core/synergy_orchestrator.py" > "${LOG_DIR}/synergy-orchestrator.log" 2>&1; then
        log_success "Synergy orchestrator test passed"
        ORCHESTRATOR_TEST_PASSED=true
    else
        log_error "Synergy orchestrator test failed (see ${LOG_DIR}/synergy-orchestrator.log)"
        ORCHESTRATOR_TEST_PASSED=false
    fi
else
    log_warning "Synergy orchestrator not found"
    ORCHESTRATOR_TEST_PASSED=false
fi

# Calculate timing
END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))

# Phase 8: Generate Report
log_info "Phase 8: Generating synergy report..."

cat > "${SYNERGY_REPORT}" <<EOF
{
  "timestamp": "$(date -Iseconds)",
  "duration_seconds": ${DURATION},
  "component_coverage": {
    "found": ${FOUND_COMPONENTS},
    "total": ${TOTAL_COMPONENTS},
    "percentage": ${COMPONENT_COVERAGE}
  },
  "test_results": {
    "synergy_integration": ${SYNERGY_TESTS_PASSED:-false},
    "hypergraph_bridge": ${BRIDGE_TEST_PASSED:-false},
    "cognitive_monitor": ${MONITOR_TEST_PASSED:-false},
    "synergy_orchestrator": ${ORCHESTRATOR_TEST_PASSED:-false}
  },
  "components_found": [
EOF

# Add found components
FIRST=true
for component in "${COMPONENTS[@]}"; do
    if [ -d "${SCRIPT_DIR}/${component}" ]; then
        if [ "$FIRST" = true ]; then
            echo "    \"${component}\"" >> "${SYNERGY_REPORT}"
            FIRST=false
        else
            echo "    ,\"${component}\"" >> "${SYNERGY_REPORT}"
        fi
    fi
done

cat >> "${SYNERGY_REPORT}" <<EOF
  ],
  "synergy_enhancements": {
    "orchestrator_available": $([ -f "${SCRIPT_DIR}/synergy/core/synergy_orchestrator.py" ] && echo "true" || echo "false"),
    "hypergraph_bridge_available": $([ -f "${SCRIPT_DIR}/synergy/bridges/hypergraph_bridge.py" ] && echo "true" || echo "false"),
    "cognitive_monitor_available": $([ -f "${SCRIPT_DIR}/synergy/monitors/cognitive_monitor.py" ] && echo "true" || echo "false"),
    "integration_tests_available": $([ -f "${SCRIPT_DIR}/tests/synergy/test_synergy_integration.py" ] && echo "true" || echo "false")
  },
  "logs": {
    "directory": "${LOG_DIR}",
    "synergy_log": "${LOG_DIR}/synergy.log",
    "guix_syntax_log": "${LOG_DIR}/guix-syntax.log",
    "synergy_tests_log": "${LOG_DIR}/synergy-tests.log",
    "hypergraph_bridge_log": "${LOG_DIR}/hypergraph-bridge.log",
    "cognitive_monitor_log": "${LOG_DIR}/cognitive-monitor.log",
    "synergy_orchestrator_log": "${LOG_DIR}/synergy-orchestrator.log"
  }
}
EOF

log_success "Synergy report generated: ${SYNERGY_REPORT}"

# Final Summary
echo ""
echo "=========================================="
echo "  Synergy Check Complete"
echo "=========================================="
echo ""
echo "Duration: ${DURATION} seconds"
echo "Component Coverage: ${COMPONENT_COVERAGE}%"
echo ""
echo "Test Results:"
echo "  - Synergy Integration: $([ "$SYNERGY_TESTS_PASSED" = true ] && echo -e "${GREEN}PASSED${NC}" || echo -e "${RED}FAILED${NC}")"
echo "  - Hypergraph Bridge: $([ "$BRIDGE_TEST_PASSED" = true ] && echo -e "${GREEN}PASSED${NC}" || echo -e "${RED}FAILED${NC}")"
echo "  - Cognitive Monitor: $([ "$MONITOR_TEST_PASSED" = true ] && echo -e "${GREEN}PASSED${NC}" || echo -e "${RED}FAILED${NC}")"
echo "  - Synergy Orchestrator: $([ "$ORCHESTRATOR_TEST_PASSED" = true ] && echo -e "${GREEN}PASSED${NC}" || echo -e "${RED}FAILED${NC}")"
echo ""
echo "Logs available in: ${LOG_DIR}"
echo "Report available at: ${SYNERGY_REPORT}"
echo ""

# Determine overall success
if [ "$SYNERGY_TESTS_PASSED" = true ] && [ "$BRIDGE_TEST_PASSED" = true ] && [ "$MONITOR_TEST_PASSED" = true ] && [ "$ORCHESTRATOR_TEST_PASSED" = true ]; then
    log_success "All synergy checks passed! ✓"
    exit 0
else
    log_warning "Some synergy checks failed or were skipped"
    exit 1
fi
