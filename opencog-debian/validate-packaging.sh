#!/bin/bash
#
# Validate Debian Packaging Structure
# Checks that all packages have required files and proper structure
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Debian Packaging Validation${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

TOTAL_PACKAGES=0
VALID_PACKAGES=0
INVALID_PACKAGES=0

# Required files for each package
REQUIRED_FILES=(
    "debian/control"
    "debian/rules"
    "debian/changelog"
    "debian/compat"
    "debian/copyright"
    "debian/source/format"
)

# Iterate through all package directories
for package_dir in */; do
    if [ "$package_dir" = "./" ]; then
        continue
    fi
    
    TOTAL_PACKAGES=$((TOTAL_PACKAGES + 1))
    package_name="${package_dir%/}"
    
    echo -e "${BLUE}Checking: ${package_name}${NC}"
    
    all_files_present=true
    
    for required_file in "${REQUIRED_FILES[@]}"; do
        file_path="${package_dir}${required_file}"
        
        if [ -f "$file_path" ]; then
            echo -e "  ${GREEN}✓${NC} ${required_file}"
        else
            echo -e "  ${RED}✗${NC} ${required_file} - MISSING"
            all_files_present=false
        fi
    done
    
    # Check if rules is executable
    if [ -f "${package_dir}debian/rules" ]; then
        if [ -x "${package_dir}debian/rules" ]; then
            echo -e "  ${GREEN}✓${NC} debian/rules is executable"
        else
            echo -e "  ${YELLOW}⚠${NC} debian/rules is not executable"
        fi
    fi
    
    # Check if update script exists
    if [ -f "${package_dir}update-${package_name}.sh" ]; then
        echo -e "  ${GREEN}✓${NC} update-${package_name}.sh exists"
        if [ -x "${package_dir}update-${package_name}.sh" ]; then
            echo -e "  ${GREEN}✓${NC} update script is executable"
        else
            echo -e "  ${YELLOW}⚠${NC} update script is not executable"
        fi
    else
        echo -e "  ${YELLOW}⚠${NC} update-${package_name}.sh missing"
    fi
    
    if [ "$all_files_present" = true ]; then
        VALID_PACKAGES=$((VALID_PACKAGES + 1))
        echo -e "  ${GREEN}Status: VALID${NC}"
    else
        INVALID_PACKAGES=$((INVALID_PACKAGES + 1))
        echo -e "  ${RED}Status: INVALID${NC}"
    fi
    
    echo ""
done

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Validation Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo "Total Packages: $TOTAL_PACKAGES"
echo -e "${GREEN}Valid Packages: $VALID_PACKAGES${NC}"
echo -e "${RED}Invalid Packages: $INVALID_PACKAGES${NC}"
echo ""

if [ $INVALID_PACKAGES -eq 0 ]; then
    echo -e "${GREEN}✓ All packages have valid structure${NC}"
    exit 0
else
    echo -e "${RED}✗ Some packages have missing files${NC}"
    exit 1
fi
