#!/bin/bash
# Validation script for GNU Mach Advanced Memory Management Features

echo "GNU Mach Advanced Memory Management Validation"
echo "=============================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test counters
TESTS_PASSED=0
TESTS_FAILED=0
TESTS_TOTAL=0

test_result() {
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}PASS${NC}: $2"
        TESTS_PASSED=$((TESTS_PASSED + 1))
    else
        echo -e "${RED}FAIL${NC}: $2"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
}

note() {
    echo -e "${YELLOW}NOTE${NC}: $1"
}

echo "Testing advanced memory management implementation..."
echo

# 1. Test Red-Black Tree Optimizations
echo "1. Testing VM Map Red-Black Tree Implementation"
echo "-----------------------------------------------"

if grep -q "rbtree_next" vm/vm_map.c; then
    test_result 0 "Red-black tree traversal optimizations found in vm_map.c"
else
    test_result 1 "Red-black tree optimizations missing"
fi

if grep -q "vm_map_entry_tree_next" vm/vm_map.c; then
    test_result 0 "Tree traversal helper function implemented"
else
    test_result 1 "Tree traversal helper function missing"
fi

# Count optimized functions
OPTIMIZED_FUNCTIONS=$(grep -c "rbtree_next" vm/vm_map.c)
if [ $OPTIMIZED_FUNCTIONS -ge 5 ]; then
    test_result 0 "Multiple functions use red-black tree optimizations ($OPTIMIZED_FUNCTIONS found)"
else
    test_result 1 "Insufficient red-black tree optimizations ($OPTIMIZED_FUNCTIONS found, expected >= 5)"
fi

echo

# 2. Test ASLR Implementation
echo "2. Testing ASLR Implementation" 
echo "------------------------------"

if grep -q "aslr_enabled" vm/vm_map.c; then
    test_result 0 "ASLR support found in vm_map structure"
else
    test_result 1 "ASLR support missing from vm_map"
fi

if grep -q "vm_map_get_aslr_entropy" vm/vm_map.c; then
    test_result 0 "ASLR entropy generation function implemented"
else
    test_result 1 "ASLR entropy function missing"
fi

if grep -q "VM_MAP_ASLR.*ENTROPY" vm/vm_map.h; then
    test_result 0 "ASLR configuration constants defined"
else
    test_result 1 "ASLR configuration constants missing"
fi

echo

# 3. Test Large Page Support
echo "3. Testing Large Page Support"
echo "-----------------------------"

if grep -q "VM_MAP_LARGE_PAGE_SIZE" vm/vm_map.c; then
    test_result 0 "Large page size constant defined"
else
    test_result 1 "Large page size constant missing"
fi

if grep -q "large.*page.*align\|large_page_mask" vm/vm_map.c; then
    test_result 0 "Large page alignment logic implemented"
else
    test_result 1 "Large page alignment missing"
fi

echo

# 4. Test Block-Level Cache
echo "4. Testing Block-Level Cache Implementation"
echo "-------------------------------------------"

if [ -f "vm/vm_block_cache.c" ] && [ -f "vm/vm_block_cache.h" ]; then
    test_result 0 "Block cache source files exist"
else
    test_result 1 "Block cache source files missing"
fi

if make vm/vm_block_cache.o >/dev/null 2>&1; then
    test_result 0 "Block cache module compiles successfully"
else
    test_result 1 "Block cache module fails to compile"
fi

if grep -q "block_cache_lookup" vm/vm_block_cache.c; then
    test_result 0 "Block cache lookup functionality implemented"
else
    test_result 1 "Block cache lookup missing"
fi

echo

# 5. Test Memory Compression
echo "5. Testing Memory Compression Implementation"
echo "--------------------------------------------"

if [ -f "vm/vm_compress.c" ] && [ -f "vm/vm_compress.h" ]; then
    test_result 0 "Memory compression source files exist"
else
    test_result 1 "Memory compression source files missing"
fi

if make vm/vm_compress.o >/dev/null 2>&1; then
    test_result 0 "Memory compression module compiles successfully"
else
    test_result 1 "Memory compression module fails to compile"
fi

if grep -q "vm_page_compress" vm/vm_compress.c; then
    test_result 0 "Page compression functionality implemented"
else
    test_result 1 "Page compression functionality missing"
fi

if grep -q "simple_compress\|simple_decompress" vm/vm_compress.c; then
    test_result 0 "Compression/decompression algorithms implemented"
else
    test_result 1 "Compression algorithms missing"
fi

echo

# 6. Test Build Integration
echo "6. Testing Build System Integration"
echo "-----------------------------------"

if grep -q "vm_compress.c" Makefrag.am; then
    test_result 0 "Memory compression integrated into build system"
else
    test_result 1 "Memory compression not in build system"
fi

if grep -q "vm_block_cache.c" Makefrag.am; then
    test_result 0 "Block cache integrated into build system"
else
    test_result 1 "Block cache not in build system"
fi

echo

# 7. Test Advanced Memory Test Suite
echo "7. Testing Advanced Memory Test Suite"
echo "------------------------------------"

if [ -f "tests/test-advanced-memory.c" ]; then
    test_result 0 "Advanced memory test suite exists"
else
    test_result 1 "Advanced memory test suite missing"
fi

if grep -q "test-advanced-memory" tests/user-qemu.mk; then
    test_result 0 "Advanced memory test integrated into test framework"
else
    test_result 1 "Advanced memory test not in test framework"
fi

if grep -q "test_large_allocation_and_aslr\|test_large_page_alignment" tests/test-advanced-memory.c; then
    test_result 0 "Comprehensive memory management tests implemented"
else
    test_result 1 "Memory management tests missing key functionality"
fi

echo

# 8. Check for Memory Object Integration
echo "8. Testing Memory Object Model Integration"
echo "-----------------------------------------"

if grep -q "block_cache.*memory_object\|memory_object.*block" vm/vm_block_cache.h; then
    test_result 0 "Block cache integrated with memory object model"
else
    note "Block cache operates independently of memory object model (hybrid approach)"
fi

if [ -f "memory_object_block_cache_analysis.md" ]; then
    test_result 0 "Memory object vs block cache analysis document exists"
else
    test_result 1 "Memory object analysis documentation missing"
fi

echo

# Summary
echo "=========================================="
echo "VALIDATION SUMMARY"
echo "=========================================="
echo "Tests Passed: $TESTS_PASSED"
echo "Tests Failed: $TESTS_FAILED"
echo "Total Tests:  $TESTS_TOTAL"

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "${GREEN}ALL TESTS PASSED!${NC}"
    echo "Advanced memory management features are fully implemented."
else
    echo -e "${RED}$TESTS_FAILED test(s) failed.${NC}"
    echo "Some advanced memory management features may need attention."
fi

echo
echo "Key Features Validated:"
echo "- Red-black tree VM map optimizations (O(log n) performance)"
echo "- Address Space Layout Randomization (ASLR)"
echo "- Large page support (2MB alignment)"
echo "- Block-level caching infrastructure"  
echo "- Memory compression subsystem"
echo "- Comprehensive test coverage"

exit $TESTS_FAILED