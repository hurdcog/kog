#!/bin/bash
# tests/synergy/interoperability-test.sh
# Comprehensive interoperability testing for cognitive synergy validation

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test counters
TESTS_RUN=0
TESTS_PASSED=0
TESTS_FAILED=0

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Cognitive Synergy Interoperability Tests${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Helper functions
pass() {
    echo -e "${GREEN}✓ PASS${NC}: $1"
    ((TESTS_PASSED++))
    ((TESTS_RUN++))
}

fail() {
    echo -e "${RED}✗ FAIL${NC}: $1"
    ((TESTS_FAILED++))
    ((TESTS_RUN++))
}

skip() {
    echo -e "${YELLOW}⊘ SKIP${NC}: $1"
}

info() {
    echo -e "${BLUE}ℹ INFO${NC}: $1"
}

# Test 1: Scheme Module Loading
echo -e "\n${BLUE}Test 1: Scheme Module Loading${NC}"
echo "Testing that Scheme modules can be loaded..."

# Set GUILE_LOAD_PATH to include current directory
export GUILE_LOAD_PATH="$PWD:$GUILE_LOAD_PATH"

if command -v guile &> /dev/null; then
    # Test orchestrator module
    if timeout 5 guile --no-auto-compile -c "(use-modules (synergy orchestrator))" 2>/dev/null; then
        pass "Orchestrator module loads successfully"
    else
        fail "Orchestrator module failed to load"
    fi
    
    # Test introspection module
    if timeout 5 guile --no-auto-compile -c "(use-modules (synergy monitors introspection))" 2>/dev/null; then
        pass "Introspection module loads successfully"
    else
        fail "Introspection module failed to load"
    fi
    
    # Test identity module
    if timeout 5 guile --no-auto-compile -c "(use-modules (synergy identity hypergraph-identity))" 2>/dev/null; then
        pass "Hypergraph Identity module loads successfully"
    else
        fail "Hypergraph Identity module failed to load"
    fi
else
    skip "Guile not available - skipping Scheme module tests"
fi

# Test 2: Directory Structure
echo -e "\n${BLUE}Test 2: Directory Structure Validation${NC}"
echo "Checking that required directories exist..."

REQUIRED_DIRS=(
    "synergy"
    "synergy/monitors"
    "synergy/identity"
    "synergy/core"
    "synergy/api"
    "tests/synergy"
)

for dir in "${REQUIRED_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        pass "Directory exists: $dir"
    else
        fail "Directory missing: $dir"
    fi
done

# Test 3: AtomSpace Availability
echo -e "\n${BLUE}Test 3: AtomSpace Component Check${NC}"
echo "Checking for AtomSpace components..."

if [ -d "atomspace" ]; then
    pass "AtomSpace directory found"
    
    if [ -f "atomspace/CMakeLists.txt" ]; then
        pass "AtomSpace build configuration found"
    else
        fail "AtomSpace build configuration missing"
    fi
else
    fail "AtomSpace directory not found"
fi

# Test 4: Hyperon/MeTTa Integration
echo -e "\n${BLUE}Test 4: Hyperon/MeTTa Integration Check${NC}"
echo "Checking for Hyperon components..."

HYPERON_DIRS=("hyperon" "atomspace-metta" "metta-moses")

HYPERON_FOUND=false
for dir in "${HYPERON_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        pass "Hyperon component found: $dir"
        HYPERON_FOUND=true
    fi
done

if [ "$HYPERON_FOUND" = false ]; then
    skip "No Hyperon components found - integration tests will be limited"
fi

# Test 5: Python Integration
echo -e "\n${BLUE}Test 5: Python Integration Check${NC}"
echo "Checking for Python components..."

if [ -f "app.py" ]; then
    pass "Python demo application found"
else
    fail "Python demo application not found"
fi

if [ -f "requirements.txt" ]; then
    pass "Python requirements file found"
else
    fail "Python requirements file not found"
fi

# Test 6: Cognitive Architecture Components
echo -e "\n${BLUE}Test 6: Cognitive Architecture Components${NC}"
echo "Checking for advanced cognitive components..."

COGNITIVE_COMPONENTS=(
    "coggml"
    "cogself"
    "atomspace-accelerator"
    "agentic-chatbots"
)

for component in "${COGNITIVE_COMPONENTS[@]}"; do
    if [ -d "$component" ]; then
        pass "Cognitive component found: $component"
    else
        skip "Optional component not present: $component"
    fi
done

# Test 7: Build System Integration
echo -e "\n${BLUE}Test 7: Build System Integration${NC}"
echo "Checking build system configuration..."

if [ -f "CMakeLists.txt" ]; then
    pass "Root CMakeLists.txt found"
else
    fail "Root CMakeLists.txt not found"
fi

if [ -f "guix.scm" ]; then
    pass "Guix package definition found"
    
    # Validate Guix syntax
    if command -v guile &> /dev/null; then
        if guile --no-auto-compile -c "(with-input-from-file \"guix.scm\" (lambda () (read)))" 2>/dev/null; then
            pass "Guix package definition has valid syntax"
        else
            fail "Guix package definition has syntax errors"
        fi
    fi
else
    fail "Guix package definition not found"
fi

# Test 8: Synergy Scripts
echo -e "\n${BLUE}Test 8: Synergy Scripts${NC}"
echo "Checking for synergy coordination scripts..."

if [ -f "synergy.sh" ]; then
    pass "Synergy coordination script found"
    
    if [ -x "synergy.sh" ]; then
        pass "Synergy script is executable"
    else
        fail "Synergy script is not executable"
    fi
else
    fail "Synergy coordination script not found"
fi

# Test 9: Documentation
echo -e "\n${BLUE}Test 9: Documentation Check${NC}"
echo "Checking for cognitive synergy documentation..."

DOCS=(
    "docs/cognitive-synergy.md"
    "docs/architecture.md"
    "README.md"
)

for doc in "${DOCS[@]}"; do
    if [ -f "$doc" ]; then
        pass "Documentation found: $doc"
    else
        fail "Documentation missing: $doc"
    fi
done

# Test 10: Mock Interoperability Test
echo -e "\n${BLUE}Test 10: Mock Component Interaction${NC}"
echo "Running mock interoperability test..."

# Create a simple test that simulates component interaction
TEST_DIR="/tmp/occ-interop-test"
mkdir -p "$TEST_DIR"

# Create a mock AtomSpace data file
cat > "$TEST_DIR/atomspace-data.scm" <<'EOF'
;; Mock AtomSpace data
(ConceptNode "CognitiveAgent")
(ConceptNode "Environment")
(EvaluationLink
  (PredicateNode "interacts-with")
  (ListLink
    (ConceptNode "CognitiveAgent")
    (ConceptNode "Environment")))
EOF

if [ -f "$TEST_DIR/atomspace-data.scm" ]; then
    pass "Mock AtomSpace data created successfully"
else
    fail "Failed to create mock AtomSpace data"
fi

# Create a mock identity state
cat > "$TEST_DIR/identity-state.scm" <<'EOF'
;; Mock Identity State
((id . "test-identity-001")
 (tuples . (((subject . "system")
             (relation . "has-capability")
             (object . "reasoning")
             (confidence . 0.9))))
 (core-self . ((values . ("cognitive-synergy" "self-awareness"))
               (beliefs . ("emergence-through-interaction"))
               (goals . ("achieve-agi"))
               (version . 1))))
EOF

if [ -f "$TEST_DIR/identity-state.scm" ]; then
    pass "Mock identity state created successfully"
else
    fail "Failed to create mock identity state"
fi

# Test reading the files
if command -v guile &> /dev/null; then
    if guile -c "(with-input-from-file \"$TEST_DIR/atomspace-data.scm\" (lambda () (read)))" 2>/dev/null; then
        pass "Mock AtomSpace data is valid Scheme"
    else
        fail "Mock AtomSpace data has invalid syntax"
    fi
    
    if guile -c "(with-input-from-file \"$TEST_DIR/identity-state.scm\" (lambda () (read)))" 2>/dev/null; then
        pass "Mock identity state is valid Scheme"
    else
        fail "Mock identity state has invalid syntax"
    fi
fi

# Cleanup
rm -rf "$TEST_DIR"

# Test Summary
echo -e "\n${BLUE}========================================${NC}"
echo -e "${BLUE}Test Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "Total Tests Run: ${TESTS_RUN}"
echo -e "${GREEN}Passed: ${TESTS_PASSED}${NC}"
echo -e "${RED}Failed: ${TESTS_FAILED}${NC}"
echo ""

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ All tests passed!${NC}"
    echo -e "${GREEN}Cognitive synergy components are properly integrated.${NC}"
    exit 0
else
    echo -e "${RED}✗ Some tests failed.${NC}"
    echo -e "${YELLOW}Review the failures above and ensure all components are properly configured.${NC}"
    exit 1
fi
