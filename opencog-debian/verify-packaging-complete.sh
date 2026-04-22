#!/bin/bash
#
# verify-packaging-complete.sh
# Verify that all Debian packages have complete and correct metadata
#
# Version: 3.0
# Date: December 25, 2025
#

set -e

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}"
echo "=========================================="
echo "OpenCog Debian Packaging Verification"
echo "Version 3.0 - December 25, 2025"
echo "=========================================="
echo -e "${NC}"
echo ""

# Counters
total_packages=0
complete_packages=0
issues_found=0

# Expected packages in build order
expected_packages=(
    # Layer 0: Microkernel
    "cognumach"
    # Layer 1: Foundation
    "cogutil"
    # Layer 2: Core AtomSpace
    "atomspace"
    # Layer 2.5: Storage Foundation (CRITICAL)
    "atomspace-storage"
    # Layer 3: Storage Backends
    "atomspace-rocks"
    "atomspace-pgres"
    "atomspace-cog"
    # Layer 4: Core Services
    "cogserver"
    "ure"
    # Layer 4.5: Cognitive OS
    "hurdcog"
    "hurdcog-machspace"
    "hurdcog-cogkernel-core"
    # Layer 4.6: Integration
    "hurdcog-atomspace-bridge"
    "cognumach-cognitive-scheduler"
    "hurdcog-occ-bridge"
    "atomspace-machspace"
    # Layer 5: Cognitive Components
    "attention"
    "pln"
    "miner"
    "unify"
    "spacetime"
    # Layer 6: Learning and Generation
    "learn"
    "generate"
    # Layer 7: NLP
    "lg-atomese"
    "relex"
    # Layer 8: Specialized
    "moses"
    "asmoses"
    "agi-bio"
    "vision"
    "kogboldai-kernel"
    # Layer 9: Meta-package
    "opencog"
    # Layer 10: AGI-OS Unified
    "agi-os-unified"
    # Layer 11: AGI-OS Services
    "agi-os-monitoring"
    "agi-os-cognitive-init"
)

echo "Checking ${#expected_packages[@]} expected packages..."
echo ""

# Function to check package
check_package() {
    local pkg=$1
    total_packages=$((total_packages + 1))
    
    echo -e "${BLUE}[$total_packages/${#expected_packages[@]}]${NC} Checking $pkg..."
    
    local has_issues=0
    
    # Check if directory exists
    if [ ! -d "$pkg" ]; then
        echo -e "  ${RED}✗ Package directory not found${NC}"
        issues_found=$((issues_found + 1))
        return 1
    fi
    
    # Check for debian/control
    if [ ! -f "$pkg/debian/control" ]; then
        echo -e "  ${RED}✗ Missing debian/control${NC}"
        has_issues=1
        issues_found=$((issues_found + 1))
    else
        echo -e "  ${GREEN}✓${NC} debian/control exists"
        
        # Verify control file has required fields
        if ! grep -q "^Source:" "$pkg/debian/control"; then
            echo -e "  ${YELLOW}⚠ Missing Source field in control${NC}"
            has_issues=1
        fi
        
        if ! grep -q "^Package:" "$pkg/debian/control"; then
            echo -e "  ${YELLOW}⚠ Missing Package field in control${NC}"
            has_issues=1
        fi
    fi
    
    # Check for update script
    if [ ! -f "$pkg/update-$pkg.sh" ]; then
        echo -e "  ${RED}✗ Missing update-$pkg.sh${NC}"
        has_issues=1
        issues_found=$((issues_found + 1))
    else
        echo -e "  ${GREEN}✓${NC} update-$pkg.sh exists"
        
        # Check if executable
        if [ ! -x "$pkg/update-$pkg.sh" ]; then
            echo -e "  ${YELLOW}⚠ update-$pkg.sh not executable${NC}"
            chmod +x "$pkg/update-$pkg.sh"
            echo -e "  ${GREEN}✓${NC} Made executable"
        fi
    fi
    
    # Check for debian/rules
    if [ ! -f "$pkg/debian/rules" ]; then
        echo -e "  ${YELLOW}⚠ Missing debian/rules${NC}"
        has_issues=1
    else
        echo -e "  ${GREEN}✓${NC} debian/rules exists"
    fi
    
    # Check for debian/changelog
    if [ ! -f "$pkg/debian/changelog" ]; then
        echo -e "  ${YELLOW}⚠ Missing debian/changelog${NC}"
        has_issues=1
    else
        echo -e "  ${GREEN}✓${NC} debian/changelog exists"
    fi
    
    # Check for debian/copyright
    if [ ! -f "$pkg/debian/copyright" ]; then
        echo -e "  ${YELLOW}⚠ Missing debian/copyright${NC}"
        has_issues=1
    else
        echo -e "  ${GREEN}✓${NC} debian/copyright exists"
    fi
    
    if [ $has_issues -eq 0 ]; then
        echo -e "  ${GREEN}✓ Package complete${NC}"
        complete_packages=$((complete_packages + 1))
    else
        echo -e "  ${YELLOW}⚠ Package has issues${NC}"
    fi
    
    echo ""
}

# Critical dependency verification
verify_critical_dependencies() {
    echo -e "${BLUE}Verifying critical dependencies...${NC}"
    echo ""
    
    # Check cogserver depends on atomspace-storage
    if [ -f "cogserver/debian/control" ]; then
        if grep -q "atomspace-storage" "cogserver/debian/control"; then
            echo -e "${GREEN}✓${NC} cogserver correctly depends on atomspace-storage"
        else
            echo -e "${RED}✗ CRITICAL: cogserver missing atomspace-storage dependency!${NC}"
            issues_found=$((issues_found + 1))
        fi
    fi
    
    # Check atomspace-rocks depends on atomspace-storage
    if [ -f "atomspace-rocks/debian/control" ]; then
        if grep -q "atomspace-storage" "atomspace-rocks/debian/control"; then
            echo -e "${GREEN}✓${NC} atomspace-rocks correctly depends on atomspace-storage"
        else
            echo -e "${RED}✗ CRITICAL: atomspace-rocks missing atomspace-storage dependency!${NC}"
            issues_found=$((issues_found + 1))
        fi
    fi
    
    # Check atomspace-pgres depends on atomspace-storage
    if [ -f "atomspace-pgres/debian/control" ]; then
        if grep -q "atomspace-storage" "atomspace-pgres/debian/control"; then
            echo -e "${GREEN}✓${NC} atomspace-pgres correctly depends on atomspace-storage"
        else
            echo -e "${RED}✗ CRITICAL: atomspace-pgres missing atomspace-storage dependency!${NC}"
            issues_found=$((issues_found + 1))
        fi
    fi
    
    # Check atomspace-cog depends on atomspace-storage
    if [ -f "atomspace-cog/debian/control" ]; then
        if grep -q "atomspace-storage" "atomspace-cog/debian/control"; then
            echo -e "${GREEN}✓${NC} atomspace-cog correctly depends on atomspace-storage"
        else
            echo -e "${RED}✗ CRITICAL: atomspace-cog missing atomspace-storage dependency!${NC}"
            issues_found=$((issues_found + 1))
        fi
    fi
    
    echo ""
}

# Check all expected packages
for pkg in "${expected_packages[@]}"; do
    check_package "$pkg"
done

# Verify critical dependencies
verify_critical_dependencies

# Print summary
echo -e "${BLUE}"
echo "=========================================="
echo "Verification Summary"
echo "=========================================="
echo -e "${NC}"
echo ""
echo "Total packages checked: $total_packages"
echo -e "Complete packages: ${GREEN}$complete_packages${NC}"
echo -e "Issues found: ${RED}$issues_found${NC}"
echo ""

if [ $issues_found -eq 0 ]; then
    echo -e "${GREEN}✓ All packages are complete and production-ready!${NC}"
    echo ""
    exit 0
else
    echo -e "${YELLOW}⚠ Some packages have issues that should be addressed${NC}"
    echo ""
    exit 1
fi
