#!/bin/bash
#
# test-generation.sh - Test the component generation system
#
# This script tests the CogArc component generation by creating a sample
# component and verifying its structure.
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_DIR="/tmp/cogarc-test-$$"

echo "CogArc Component Generation Test"
echo "================================="
echo ""
echo "Test directory: $TEST_DIR"
echo ""

# Create test directory
mkdir -p "$TEST_DIR"
cd "$TEST_DIR"

# Test 1: Basic component generation
echo "Test 1: Basic component generation"
echo "-----------------------------------"
"$SCRIPT_DIR/generate-component.sh" test-component "Test component for validation" \
  --output-dir "$TEST_DIR"

if [ -d "$TEST_DIR/test-component" ]; then
    echo "✓ Component directory created"
else
    echo "✗ Component directory not found"
    exit 1
fi

# Check directory structure
echo ""
echo "Checking directory structure..."

REQUIRED_DIRS=(
    "test-component/.circleci"
    "test-component/cmake"
    "test-component/debian"
    "test-component/opencog/test-component"
    "test-component/tests/test-component"
    "test-component/examples"
    "test-component/scripts"
)

for dir in "${REQUIRED_DIRS[@]}"; do
    if [ -d "$TEST_DIR/$dir" ]; then
        echo "  ✓ $dir"
    else
        echo "  ✗ $dir (missing)"
        exit 1
    fi
done

# Check required files
echo ""
echo "Checking required files..."

REQUIRED_FILES=(
    "test-component/CMakeLists.txt"
    "test-component/README.md"
    "test-component/LICENSE"
    "test-component/AUTHORS"
    "test-component/.gitignore"
    "test-component/opencog/test-component/version.h"
    "test-component/opencog/test-component/CMakeLists.txt"
    "test-component/.circleci/config.yml"
    "test-component/debian/control"
    "test-component/debian/rules"
)

for file in "${REQUIRED_FILES[@]}"; do
    if [ -f "$TEST_DIR/$file" ]; then
        echo "  ✓ $file"
    else
        echo "  ✗ $file (missing)"
        exit 1
    fi
done

# Test 2: Verify substitution
echo ""
echo "Test 2: Verify variable substitution"
echo "-------------------------------------"

# Check that variables were substituted in CMakeLists.txt
if grep -q "test-component" "$TEST_DIR/test-component/CMakeLists.txt"; then
    echo "  ✓ Component name substituted in CMakeLists.txt"
else
    echo "  ✗ Component name not substituted"
    exit 1
fi

if grep -q "TEST_COMPONENT" "$TEST_DIR/test-component/opencog/test-component/version.h"; then
    echo "  ✓ Component name uppercase substituted in version.h"
else
    echo "  ✗ Component name uppercase not substituted"
    exit 1
fi

if grep -q "TestComponent" "$TEST_DIR/test-component/CMakeLists.txt"; then
    echo "  ✓ Component name CamelCase substituted"
else
    echo "  ✗ Component name CamelCase not substituted"
    exit 1
fi

# Test 3: Test with dependencies
echo ""
echo "Test 3: Component with dependencies"
echo "------------------------------------"

"$SCRIPT_DIR/generate-component.sh" test-component-deps "Test with dependencies" \
  --output-dir "$TEST_DIR" \
  --dependencies cogutil,atomspace

if grep -qi "atomspace" "$TEST_DIR/test-component-deps/CMakeLists.txt"; then
    echo "  ✓ AtomSpace dependency added to CMakeLists.txt"
else
    echo "  ✗ AtomSpace dependency not found"
    exit 1
fi

if grep -q "libatomspace-dev" "$TEST_DIR/test-component-deps/debian/control"; then
    echo "  ✓ AtomSpace dependency added to debian/control"
else
    echo "  ✗ AtomSpace dependency not found in debian/control"
    exit 1
fi

# Test 4: GitHub Actions option
echo ""
echo "Test 4: GitHub Actions workflow"
echo "--------------------------------"

"$SCRIPT_DIR/generate-component.sh" test-component-gha "Test with GitHub Actions" \
  --output-dir "$TEST_DIR" \
  --with-github-actions

if [ -f "$TEST_DIR/test-component-gha/.github/workflows/build.yml" ]; then
    echo "  ✓ GitHub Actions workflow created"
else
    echo "  ✗ GitHub Actions workflow not found"
    exit 1
fi

# Test 5: Custom version and author
echo ""
echo "Test 5: Custom version and author"
echo "----------------------------------"

"$SCRIPT_DIR/generate-component.sh" test-component-custom "Test with custom metadata" \
  --output-dir "$TEST_DIR" \
  --version 2.5.3 \
  --author "Test Author" \
  --email test@example.com

if grep -q "2.5.3" "$TEST_DIR/test-component-custom/opencog/test-component-custom/version.h"; then
    echo "  ✓ Custom version set"
else
    echo "  ✗ Custom version not found"
    exit 1
fi

if grep -q "Test Author" "$TEST_DIR/test-component-custom/AUTHORS"; then
    echo "  ✓ Custom author set"
else
    echo "  ✗ Custom author not found"
    exit 1
fi

# Summary
echo ""
echo "================================="
echo "All tests passed! ✓"
echo "================================="
echo ""
echo "Generated test components in: $TEST_DIR"
echo ""
echo "You can inspect them:"
echo "  cd $TEST_DIR"
echo "  ls -la"
echo ""
echo "To build and test a generated component:"
echo "  cd $TEST_DIR/test-component"
echo "  mkdir build && cd build"
echo "  cmake .."
echo "  make"
echo ""
echo "To clean up test directory:"
echo "  rm -rf $TEST_DIR"
echo ""
