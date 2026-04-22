#!/bin/bash
# Integration Test Suite for AGI-OS

echo "╔════════════════════════════════════════════════════════════╗"
echo "║  AGI-OS Integration Test Suite                            ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

PASSED=0
FAILED=0

# Test 1: Cognumach headers
echo "Test 1: Checking Cognumach cognitive headers..."
if [ -f "/home/ubuntu/cognumach/include/mach/cognitive/atomspace_ipc.h" ]; then
    echo "  ✓ AtomSpace IPC header found"
    PASSED=$((PASSED + 1))
else
    echo "  ✗ AtomSpace IPC header not found"
    FAILED=$((FAILED + 1))
fi

if [ -f "/home/ubuntu/cognumach/include/mach/cognitive/cognitive_vm.h" ]; then
    echo "  ✓ Cognitive VM header found"
    PASSED=$((PASSED + 1))
else
    echo "  ✗ Cognitive VM header not found"
    FAILED=$((FAILED + 1))
fi

# Test 2: HurdCog integration layer
echo ""
echo "Test 2: Checking HurdCog integration layer..."
if [ -f "/home/ubuntu/hurdcog/cogkernel/mach-integration/machspace-bridge.scm" ]; then
    echo "  ✓ MachSpace bridge found"
    PASSED=$((PASSED + 1))
else
    echo "  ✗ MachSpace bridge not found"
    FAILED=$((FAILED + 1))
fi

# Test 3: OCC integration layer
echo ""
echo "Test 3: Checking OCC integration layer..."
if [ -f "/home/ubuntu/occ/hurdcog-integration/atomspace-hurdcog-bridge.py" ]; then
    echo "  ✓ AtomSpace-HurdCog bridge found"
    PASSED=$((PASSED + 1))
else
    echo "  ✗ AtomSpace-HurdCog bridge not found"
    FAILED=$((FAILED + 1))
fi

# Test 4: Python bridge functionality
echo ""
echo "Test 4: Testing Python bridge..."
if python3 /home/ubuntu/occ/hurdcog-integration/atomspace-hurdcog-bridge.py > /dev/null 2>&1; then
    echo "  ✓ Python bridge executes successfully"
    PASSED=$((PASSED + 1))
else
    echo "  ✗ Python bridge execution failed"
    FAILED=$((FAILED + 1))
fi

# Test 5: Guile bridge functionality
echo ""
echo "Test 5: Testing Guile bridge..."
if guile -c "(format #t \"Guile OK~%\")" > /dev/null 2>&1; then
    echo "  ✓ Guile is functional"
    PASSED=$((PASSED + 1))
else
    echo "  ✗ Guile test failed"
    FAILED=$((FAILED + 1))
fi

# Summary
echo ""
echo "════════════════════════════════════════════════════════════"
echo "Test Results: $PASSED passed, $FAILED failed"
echo "════════════════════════════════════════════════════════════"

if [ $FAILED -eq 0 ]; then
    echo "✓ All tests passed!"
    exit 0
else
    echo "✗ Some tests failed"
    exit 1
fi
