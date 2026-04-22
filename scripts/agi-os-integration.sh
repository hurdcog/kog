#!/bin/bash
#
# AGI-OS Integration Script
# Integrates Cognumach, HurdCog, and OpenCog Collection into unified AGI-OS
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OCC_ROOT="$(dirname "$SCRIPT_DIR")"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}AGI-OS Integration System${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Function to check if a component is available
check_component() {
    local component=$1
    local path=$2
    
    if [ -d "$path" ]; then
        echo -e "${GREEN}✓${NC} $component: Available at $path"
        return 0
    else
        echo -e "${YELLOW}⚠${NC} $component: Not found at $path"
        return 1
    fi
}

# Function to check if a package is installed
check_package() {
    local package=$1
    
    if dpkg -l | grep -q "^ii  $package"; then
        echo -e "${GREEN}✓${NC} Package $package: Installed"
        return 0
    else
        echo -e "${YELLOW}⚠${NC} Package $package: Not installed"
        return 1
    fi
}

echo "Checking Layer 1: Cognumach Microkernel"
echo "----------------------------------------"
check_component "Cognumach Source" "$OCC_ROOT/cognumach"
check_package "cognumach" || true
echo ""

echo "Checking Layer 2: HurdCog Cognitive OS"
echo "---------------------------------------"
check_component "HurdCog Source" "$OCC_ROOT/hurdcog"
check_package "hurdcog" || true
check_package "hurdcog-dashboard" || true
echo ""

echo "Checking Layer 3: OpenCog Collection"
echo "-------------------------------------"
check_component "CogUtil" "$OCC_ROOT/cogutil"
check_component "AtomSpace" "$OCC_ROOT/atomspace"
check_component "CogServer" "$OCC_ROOT/cogserver"
check_component "URE" "$OCC_ROOT/ure"
check_component "PLN" "$OCC_ROOT/pln"
check_package "libcogutil-dev" || true
check_package "opencog-atomspace" || true
check_package "opencog" || true
echo ""

echo "Checking Integration Components"
echo "--------------------------------"
check_component "Integration Docs" "$OCC_ROOT/hurdcog-integration"
if [ -f "$OCC_ROOT/occ-hurdcog-unified.scm" ]; then
    echo -e "${GREEN}✓${NC} Guix Integration: Available at $OCC_ROOT/occ-hurdcog-unified.scm"
else
    echo -e "${YELLOW}⚠${NC} Guix Integration: Not found"
fi
check_component "Debian Packaging" "$OCC_ROOT/opencog-debian"
echo ""

echo "Checking Cognitive Synergy Infrastructure"
echo "------------------------------------------"
if [ -f "$OCC_ROOT/synergy_improved.sh" ]; then
    echo -e "${GREEN}✓${NC} Synergy test script: Available"
else
    echo -e "${YELLOW}⚠${NC} Synergy test script: Not found"
fi

if [ -d "$OCC_ROOT/synergy" ]; then
    echo -e "${GREEN}✓${NC} Synergy components: Available"
else
    echo -e "${YELLOW}⚠${NC} Synergy components: Not found"
fi
echo ""

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Integration Status Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Count available components
TOTAL_COMPONENTS=15
AVAILABLE_COMPONENTS=0

[ -d "$OCC_ROOT/cognumach" ] && ((AVAILABLE_COMPONENTS++))
[ -d "$OCC_ROOT/hurdcog" ] && ((AVAILABLE_COMPONENTS++))
[ -d "$OCC_ROOT/cogutil" ] && ((AVAILABLE_COMPONENTS++))
[ -d "$OCC_ROOT/atomspace" ] && ((AVAILABLE_COMPONENTS++))
[ -d "$OCC_ROOT/cogserver" ] && ((AVAILABLE_COMPONENTS++))
[ -d "$OCC_ROOT/ure" ] && ((AVAILABLE_COMPONENTS++))
[ -d "$OCC_ROOT/pln" ] && ((AVAILABLE_COMPONENTS++))
[ -d "$OCC_ROOT/miner" ] && ((AVAILABLE_COMPONENTS++))
[ -d "$OCC_ROOT/learn" ] && ((AVAILABLE_COMPONENTS++))
[ -d "$OCC_ROOT/generate" ] && ((AVAILABLE_COMPONENTS++))
[ -d "$OCC_ROOT/moses" ] && ((AVAILABLE_COMPONENTS++))
[ -d "$OCC_ROOT/opencog-debian" ] && ((AVAILABLE_COMPONENTS++))
[ -f "$OCC_ROOT/occ-hurdcog-unified.scm" ] && ((AVAILABLE_COMPONENTS++))
[ -f "$OCC_ROOT/synergy_improved.sh" ] && ((AVAILABLE_COMPONENTS++))
[ -d "$OCC_ROOT/synergy" ] && ((AVAILABLE_COMPONENTS++))

PERCENTAGE=$((AVAILABLE_COMPONENTS * 100 / TOTAL_COMPONENTS))

echo "Source Components: $AVAILABLE_COMPONENTS/$TOTAL_COMPONENTS ($PERCENTAGE%)"
echo ""

if [ $PERCENTAGE -ge 90 ]; then
    echo -e "${GREEN}Status: Excellent - Ready for full integration${NC}"
elif [ $PERCENTAGE -ge 70 ]; then
    echo -e "${YELLOW}Status: Good - Most components available${NC}"
elif [ $PERCENTAGE -ge 50 ]; then
    echo -e "${YELLOW}Status: Fair - Some components missing${NC}"
else
    echo -e "${RED}Status: Incomplete - Many components missing${NC}"
fi

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Next Steps${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo "1. Build Debian packages:"
echo "   cd $OCC_ROOT/opencog-debian"
echo "   See BUILD_ORDER.md for complete instructions"
echo ""
echo "2. Install AGI-OS unified stack:"
echo "   sudo apt-get install agi-os-unified"
echo ""
echo "3. Start HurdCog dashboard:"
echo "   agi-os-start"
echo ""
echo "4. Run cognitive synergy tests:"
echo "   cd $OCC_ROOT && ./synergy_improved.sh"
echo ""
echo "5. Test integration:"
echo "   agi-os-test"
echo ""
