#!/bin/bash
# Basic build validation test for OCC repository

# Allow script to continue even if individual tests fail
# This enables us to collect all test results
set +e

echo "========================================="
echo "OCC Basic Build Validation Test"
echo "========================================="
echo ""

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

PASSED=0
FAILED=0

# Test 1: Check if build directory exists
echo "Test 1: Checking build directory..."
if [ -d "build" ]; then
    echo -e "${GREEN}✓ Build directory exists${NC}"
    ((PASSED++))
else
    echo -e "${RED}✗ Build directory not found${NC}"
    ((FAILED++))
fi

# Test 2: Check if CogUtil library was built
echo ""
echo "Test 2: Checking CogUtil library..."
if [ -f "build/cogutil/opencog/util/libcogutil.so" ]; then
    SIZE=$(ls -lh build/cogutil/opencog/util/libcogutil.so | awk '{print $5}')
    echo -e "${GREEN}✓ CogUtil library found (${SIZE})${NC}"
    ((PASSED++))
else
    echo -e "${RED}✗ CogUtil library not found${NC}"
    ((FAILED++))
fi

# Test 3: Check if CogGML library was built
echo ""
echo "Test 3: Checking CogGML library..."
if [ -f "build/coggml/libcoggml.so" ]; then
    SIZE=$(ls -lh build/coggml/libcoggml.so | awk '{print $5}')
    echo -e "${GREEN}✓ CogGML library found (${SIZE})${NC}"
    ((PASSED++))
else
    echo -e "${RED}✗ CogGML library not found${NC}"
    ((FAILED++))
fi

# Test 4: Check Python syntax
echo ""
echo "Test 4: Checking Python files..."
if python3 -m py_compile app.py >/dev/null 2>&1 && python3 -m py_compile autogenesis_engine.py >/dev/null 2>&1; then
    echo -e "${GREEN}✓ Python files compile correctly${NC}"
    ((PASSED++))
else
    echo -e "${RED}✗ Python files have syntax errors${NC}"
    ((FAILED++))
fi

# Test 5: Check CMakeLists.txt exists
echo ""
echo "Test 5: Checking build system configuration..."
if [ -f "CMakeLists.txt" ]; then
    echo -e "${GREEN}✓ CMakeLists.txt exists${NC}"
    ((PASSED++))
else
    echo -e "${RED}✗ CMakeLists.txt not found${NC}"
    ((FAILED++))
fi

# Test 6: Check Makefile exists
echo ""
echo "Test 6: Checking Makefile..."
if [ -f "Makefile" ]; then
    echo -e "${GREEN}✓ Makefile exists${NC}"
    ((PASSED++))
else
    echo -e "${RED}✗ Makefile not found${NC}"
    ((FAILED++))
fi

# Summary
echo ""
echo "========================================="
echo "Test Summary"
echo "========================================="
echo -e "Tests passed: ${GREEN}${PASSED}${NC}"
echo -e "Tests failed: ${RED}${FAILED}${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${YELLOW}Some tests failed. Review the output above.${NC}"
    exit 1
fi
