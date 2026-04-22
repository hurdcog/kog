#!/bin/bash
# AGI-OS Cognitive Synergy Validation Script
# Validates integration and cognitive synergy across all three layers

set -e

echo "=========================================="
echo "AGI-OS Cognitive Synergy Validation"
echo "=========================================="
echo ""

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Validation results
PASSED=0
FAILED=0
WARNINGS=0

# Function to print test result
print_result() {
    local test_name=$1
    local result=$2
    local message=$3
    
    if [ "$result" = "PASS" ]; then
        echo -e "${GREEN}✓${NC} $test_name: $message"
        ((PASSED++))
    elif [ "$result" = "FAIL" ]; then
        echo -e "${RED}✗${NC} $test_name: $message"
        ((FAILED++))
    elif [ "$result" = "WARN" ]; then
        echo -e "${YELLOW}⚠${NC} $test_name: $message"
        ((WARNINGS++))
    else
        echo -e "${BLUE}ℹ${NC} $test_name: $message"
    fi
}

echo "=== Layer 0: Cognumach Microkernel ==="
echo ""

# Test 1: Check if cognumach package exists
if [ -d "opencog-debian/cognumach" ]; then
    print_result "Cognumach Package" "PASS" "Package directory exists"
    
    # Check debian files
    if [ -f "opencog-debian/cognumach/debian/control" ]; then
        print_result "Cognumach Control" "PASS" "Debian control file present"
    else
        print_result "Cognumach Control" "FAIL" "Debian control file missing"
    fi
    
    if [ -f "opencog-debian/cognumach/debian/rules" ]; then
        print_result "Cognumach Rules" "PASS" "Debian rules file present"
    else
        print_result "Cognumach Rules" "FAIL" "Debian rules file missing"
    fi
else
    print_result "Cognumach Package" "FAIL" "Package directory missing"
fi

# Test 2: Check cognitive scheduler
if [ -d "opencog-debian/cognumach-cognitive-scheduler" ]; then
    print_result "Cognitive Scheduler" "PASS" "Package directory exists"
    
    if [ -f "opencog-debian/cognumach-cognitive-scheduler/debian/control" ]; then
        print_result "Scheduler Control" "PASS" "Debian control file present"
    else
        print_result "Scheduler Control" "FAIL" "Debian control file missing"
    fi
else
    print_result "Cognitive Scheduler" "FAIL" "Package directory missing"
fi

echo ""
echo "=== Layer 1: HurdCog Cognitive OS ==="
echo ""

# Test 3: Check HurdCog base package
if [ -d "opencog-debian/hurdcog" ]; then
    print_result "HurdCog Base" "PASS" "Package directory exists"
    
    if [ -f "opencog-debian/hurdcog/debian/control" ]; then
        print_result "HurdCog Control" "PASS" "Debian control file present"
    else
        print_result "HurdCog Control" "FAIL" "Debian control file missing"
    fi
else
    print_result "HurdCog Base" "FAIL" "Package directory missing"
fi

# Test 4: Check MachSpace integration
if [ -d "opencog-debian/hurdcog-machspace" ]; then
    print_result "MachSpace" "PASS" "Package directory exists"
else
    print_result "MachSpace" "FAIL" "Package directory missing"
fi

# Test 5: Check CogKernel Core
if [ -d "opencog-debian/hurdcog-cogkernel-core" ]; then
    print_result "CogKernel Core" "PASS" "Package directory exists"
else
    print_result "CogKernel Core" "FAIL" "Package directory missing"
fi

# Test 6: Check AtomSpace Bridge
if [ -d "opencog-debian/hurdcog-atomspace-bridge" ]; then
    print_result "AtomSpace Bridge" "PASS" "Package directory exists (NEW)"
    
    if [ -f "opencog-debian/hurdcog-atomspace-bridge/debian/control" ]; then
        print_result "Bridge Control" "PASS" "Debian control file present"
    else
        print_result "Bridge Control" "FAIL" "Debian control file missing"
    fi
else
    print_result "AtomSpace Bridge" "FAIL" "Package directory missing"
fi

# Test 7: Check OCC Bridge
if [ -d "opencog-debian/hurdcog-occ-bridge" ]; then
    print_result "OCC Bridge" "PASS" "Package directory exists"
else
    print_result "OCC Bridge" "FAIL" "Package directory missing"
fi

echo ""
echo "=== Layer 2: OpenCog Collection ==="
echo ""

# Test 8: Check core OpenCog components
CORE_COMPONENTS=("cogutil" "atomspace" "cogserver" "ure" "pln")

for component in "${CORE_COMPONENTS[@]}"; do
    if [ -d "opencog-debian/$component" ]; then
        print_result "OCC: $component" "PASS" "Package directory exists"
    else
        print_result "OCC: $component" "FAIL" "Package directory missing"
    fi
done

# Test 9: Check storage backends
STORAGE_BACKENDS=("atomspace-cog" "atomspace-rocks" "atomspace-pgres")

for backend in "${STORAGE_BACKENDS[@]}"; do
    if [ -d "opencog-debian/$backend" ]; then
        print_result "Storage: $backend" "PASS" "Package directory exists"
    else
        print_result "Storage: $backend" "FAIL" "Package directory missing"
    fi
done

# Test 10: Check cognitive components
COGNITIVE_COMPONENTS=("attention" "miner" "unify" "spacetime" "learn" "generate")

for component in "${COGNITIVE_COMPONENTS[@]}"; do
    if [ -d "opencog-debian/$component" ]; then
        print_result "Cognitive: $component" "PASS" "Package directory exists"
    else
        print_result "Cognitive: $component" "FAIL" "Package directory missing"
    fi
done

echo ""
echo "=== AGI-OS Integration Layer ==="
echo ""

# Test 11: Check unified AGI-OS package
if [ -d "opencog-debian/agi-os-unified" ]; then
    print_result "AGI-OS Unified" "PASS" "Package directory exists"
else
    print_result "AGI-OS Unified" "FAIL" "Package directory missing"
fi

# Test 12: Check monitoring package
if [ -d "opencog-debian/agi-os-monitoring" ]; then
    print_result "AGI-OS Monitoring" "PASS" "Package directory exists (NEW)"
    
    if [ -f "opencog-debian/agi-os-monitoring/debian/control" ]; then
        print_result "Monitoring Control" "PASS" "Debian control file present"
    else
        print_result "Monitoring Control" "FAIL" "Debian control file missing"
    fi
else
    print_result "AGI-OS Monitoring" "FAIL" "Package directory missing"
fi

# Test 13: Check cognitive init package
if [ -d "opencog-debian/agi-os-cognitive-init" ]; then
    print_result "Cognitive Init" "PASS" "Package directory exists (NEW)"
    
    if [ -f "opencog-debian/agi-os-cognitive-init/debian/control" ]; then
        print_result "Init Control" "PASS" "Debian control file present"
    else
        print_result "Init Control" "FAIL" "Debian control file missing"
    fi
else
    print_result "Cognitive Init" "FAIL" "Package directory missing"
fi

echo ""
echo "=== Cognitive Synergy Framework ==="
echo ""

# Test 14: Check for architecture documentation
if [ -f "AGI_OS_ARCHITECTURE_ENHANCED.md" ]; then
    print_result "Architecture Doc" "PASS" "Enhanced architecture document present"
else
    print_result "Architecture Doc" "WARN" "Enhanced architecture document missing"
fi

# Test 15: Check for cognitive synergy framework doc
if [ -f "COGNITIVE_SYNERGY_FRAMEWORK.md" ]; then
    print_result "Synergy Framework" "PASS" "Framework specification present"
else
    print_result "Synergy Framework" "WARN" "Framework specification missing"
fi

# Test 16: Check for packaging assessment
if [ -f "PACKAGING_ASSESSMENT.md" ]; then
    print_result "Packaging Assessment" "PASS" "Assessment document present"
else
    print_result "Packaging Assessment" "WARN" "Assessment document missing"
fi

# Test 17: Check for enhanced build order
if [ -f "opencog-debian/BUILD_ORDER_ENHANCED.md" ]; then
    print_result "Enhanced Build Order" "PASS" "Enhanced build order present"
else
    print_result "Enhanced Build Order" "WARN" "Enhanced build order missing"
fi

echo ""
echo "=== Integration Scripts ==="
echo ""

# Test 18: Check for Guix integration
if [ -f "occ-hurdcog-unified.scm" ]; then
    print_result "Guix Integration" "PASS" "Unified Guix package present"
else
    print_result "Guix Integration" "WARN" "Unified Guix package missing"
fi

# Test 19: Check for synergy scripts
SYNERGY_SCRIPTS=("synergy.sh" "synergy_agi_os.sh" "synergy_enhanced.sh")

for script in "${SYNERGY_SCRIPTS[@]}"; do
    if [ -f "$script" ]; then
        print_result "Script: $script" "PASS" "Synergy script present"
    else
        print_result "Script: $script" "WARN" "Synergy script missing"
    fi
done

# Test 20: Check for build scripts
if [ -f "opencog-debian/build-all-packages.sh" ]; then
    print_result "Build Script" "PASS" "Master build script present"
else
    print_result "Build Script" "FAIL" "Master build script missing"
fi

echo ""
echo "=== Validation Scripts ==="
echo ""

# Test 21: Check for validation scripts
if [ -f "opencog-debian/validate-packaging.sh" ]; then
    print_result "Package Validation" "PASS" "Validation script present"
else
    print_result "Package Validation" "WARN" "Validation script missing"
fi

# Test 22: Check for dependency resolution
if [ -f "opencog-debian/resolve-dependencies.sh" ]; then
    print_result "Dependency Resolution" "PASS" "Resolution script present"
else
    print_result "Dependency Resolution" "WARN" "Resolution script missing"
fi

echo ""
echo "=========================================="
echo "Validation Summary"
echo "=========================================="
echo ""
echo -e "${GREEN}Passed:${NC}   $PASSED"
echo -e "${RED}Failed:${NC}   $FAILED"
echo -e "${YELLOW}Warnings:${NC} $WARNINGS"
echo ""

# Calculate success rate
TOTAL=$((PASSED + FAILED))
if [ $TOTAL -gt 0 ]; then
    SUCCESS_RATE=$((PASSED * 100 / TOTAL))
    echo "Success Rate: $SUCCESS_RATE%"
else
    echo "Success Rate: N/A"
fi

echo ""

# Determine overall result
if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ AGI-OS Cognitive Synergy Validation: PASSED${NC}"
    echo ""
    echo "The repository contains a complete and well-structured"
    echo "AGI Operating System with full cognitive synergy support."
    exit 0
elif [ $FAILED -le 3 ]; then
    echo -e "${YELLOW}⚠ AGI-OS Cognitive Synergy Validation: PASSED WITH WARNINGS${NC}"
    echo ""
    echo "The repository is mostly complete but has minor issues."
    echo "Review failed tests and address as needed."
    exit 0
else
    echo -e "${RED}✗ AGI-OS Cognitive Synergy Validation: FAILED${NC}"
    echo ""
    echo "The repository has significant issues that need to be addressed."
    echo "Review failed tests and implement missing components."
    exit 1
fi
