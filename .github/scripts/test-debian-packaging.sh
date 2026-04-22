#!/bin/bash
#
# Test debian packaging preparation for all components
# This script verifies that all components can be prepared for building
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

echo "================================================"
echo "Testing Debian Packaging Preparation"
echo "================================================"
echo ""

# Components to test (from the workflow)
COMPONENTS=(
    "cogutil"
    "atomspace"
    "unify"
    "ure"
    "cogserver"
    "attention"
    "moses"
    "asmoses"
    "miner"
    "pln"
)

PASSED=0
FAILED=0
FAILED_COMPONENTS=()

for component in "${COMPONENTS[@]}"; do
    echo "-------------------------------------------"
    echo "Testing: $component"
    echo "-------------------------------------------"
    
    # Test parse-package-info.sh
    if ! "$SCRIPT_DIR/parse-package-info.sh" "$component" > /dev/null 2>&1; then
        echo "❌ FAILED: parse-package-info.sh failed for $component"
        FAILED=$((FAILED + 1))
        FAILED_COMPONENTS+=("$component")
        continue
    fi
    
    # Source the package info safely
    if ! PARSE_OUTPUT=$("$SCRIPT_DIR/parse-package-info.sh" "$component" 2>&1); then
        echo "❌ FAILED: Failed to parse package info for $component"
        echo "  $PARSE_OUTPUT"
        FAILED=$((FAILED + 1))
        FAILED_COMPONENTS+=("$component")
        continue
    fi
    
    # Validate output format before sourcing
    if ! echo "$PARSE_OUTPUT" | grep -q "^REPO_NAME="; then
        echo "❌ FAILED: Invalid output format from parse-package-info.sh"
        FAILED=$((FAILED + 1))
        FAILED_COMPONENTS+=("$component")
        continue
    fi
    
    source <(echo "$PARSE_OUTPUT")
    
    echo "  Package: $DEB_NAME"
    echo "  Version: $VERSION"
    
    # Check if source directory exists
    if [ ! -d "$SOURCE_DIR" ]; then
        echo "  ⚠️  WARNING: Source directory not found: $SOURCE_DIR"
        echo "  (This is expected in a fresh clone, packages will fail to build)"
    else
        echo "  ✅ Source directory exists"
    fi
    
    # Check if debian directory exists in opencog-debian
    if [ ! -d "$DEBIAN_DIR" ]; then
        echo "  ❌ FAILED: Debian directory not found: $DEBIAN_DIR"
        FAILED=$((FAILED + 1))
        FAILED_COMPONENTS+=("$component")
        continue
    fi
    echo "  ✅ Debian directory exists"
    
    # Check required debian files
    REQUIRED_FILES=(
        "$DEBIAN_DIR/control"
        "$DEBIAN_DIR/rules"
        "$DEBIAN_DIR/changelog"
        "$DEBIAN_DIR/compat"
        "$DEBIAN_DIR/copyright"
    )
    
    ALL_FILES_PRESENT=true
    for file in "${REQUIRED_FILES[@]}"; do
        if [ ! -f "$file" ]; then
            echo "  ❌ Missing: $(basename "$file")"
            ALL_FILES_PRESENT=false
        fi
    done
    
    if [ "$ALL_FILES_PRESENT" = true ]; then
        echo "  ✅ All required debian files present"
        PASSED=$((PASSED + 1))
    else
        echo "  ❌ FAILED: Some required files missing"
        FAILED=$((FAILED + 1))
        FAILED_COMPONENTS+=("$component")
    fi
    
    echo ""
done

echo "================================================"
echo "Test Summary"
echo "================================================"
echo "Total Components: ${#COMPONENTS[@]}"
echo "Passed: $PASSED"
echo "Failed: $FAILED"

if [ $FAILED -gt 0 ]; then
    echo ""
    echo "Failed Components:"
    for comp in "${FAILED_COMPONENTS[@]}"; do
        echo "  - $comp"
    done
    echo ""
    exit 1
else
    echo ""
    echo "✅ All components are properly configured!"
    echo ""
    exit 0
fi
