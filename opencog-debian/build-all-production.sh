#!/bin/bash
# OpenCog Debian Package Build Script - Production Ready
# Builds all packages in correct dependency order with full AGI-OS support

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="${BUILD_DIR:-/tmp/opencog-build}"
INSTALL_DEBS="${INSTALL_DEBS:-yes}"
PARALLEL_JOBS="${PARALLEL_JOBS:-$(nproc)}"
BUILD_AGI_OS="${BUILD_AGI_OS:-no}"  # Set to 'yes' to build full AGI-OS stack

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}OpenCog Debian Package Build System${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""
echo -e "${BLUE}Configuration:${NC}"
echo -e "  Build Directory: ${BUILD_DIR}"
echo -e "  Parallel Jobs: ${PARALLEL_JOBS}"
echo -e "  Auto-Install: ${INSTALL_DEBS}"
echo -e "  Build AGI-OS Stack: ${BUILD_AGI_OS}"
echo ""

# Create build directory
mkdir -p "${BUILD_DIR}"

# Function to build a package
build_package() {
    local pkg=$1
    local pkg_dir="${pkg}"
    
    echo ""
    echo -e "${CYAN}========================================${NC}"
    echo -e "${CYAN}Building: ${pkg}${NC}"
    echo -e "${CYAN}========================================${NC}"
    
    if [ ! -d "${pkg_dir}" ]; then
        echo -e "${RED}ERROR: Package directory not found: ${pkg_dir}${NC}"
        return 1
    fi
    
    if [ ! -d "${pkg_dir}/debian" ]; then
        echo -e "${YELLOW}WARNING: No debian/ directory in ${pkg_dir}, skipping${NC}"
        return 0
    fi
    
    cd "${pkg_dir}"
    
    # Update package if update script exists
    if [ -f "./update-${pkg}.sh" ]; then
        echo -e "${BLUE}Updating ${pkg}...${NC}"
        ./update-${pkg}.sh || true
    fi
    
    # Install build dependencies
    echo -e "${BLUE}Installing build dependencies...${NC}"
    sudo apt-get build-dep -y . || echo -e "${YELLOW}Warning: Some build deps may be missing${NC}"
    
    # Build package
    echo -e "${BLUE}Building package...${NC}"
    dpkg-buildpackage -rfakeroot -us -uc -j${PARALLEL_JOBS}
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ ${pkg} built successfully${NC}"
        
        # Install if requested
        if [ "${INSTALL_DEBS}" = "yes" ]; then
            echo -e "${BLUE}Installing ${pkg}...${NC}"
            sudo dpkg -i ../*.deb 2>/dev/null || sudo apt-get install -f -y
            echo -e "${GREEN}✓ ${pkg} installed${NC}"
        fi
        
        # Move packages to build directory
        mv ../*.deb "${BUILD_DIR}/" 2>/dev/null || true
        mv ../*.dsc "${BUILD_DIR}/" 2>/dev/null || true
        mv ../*.changes "${BUILD_DIR}/" 2>/dev/null || true
        mv ../*.tar.* "${BUILD_DIR}/" 2>/dev/null || true
    else
        echo -e "${RED}✗ ${pkg} build failed${NC}"
        cd ..
        return 1
    fi
    
    cd ..
    return 0
}

# Stage 0: AGI-OS Microkernel (Optional)
if [ "${BUILD_AGI_OS}" = "yes" ]; then
    echo ""
    echo -e "${CYAN}========================================${NC}"
    echo -e "${CYAN}Stage 0: AGI-OS Microkernel${NC}"
    echo -e "${CYAN}========================================${NC}"
    
    build_package "cognumach" || exit 1
    build_package "cognumach-cognitive-scheduler" || exit 1
fi

# Stage 1: Foundation Library
echo ""
echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Stage 1: Foundation Library${NC}"
echo -e "${CYAN}========================================${NC}"

build_package "cogutil" || exit 1

# Stage 2: Core AtomSpace
echo ""
echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Stage 2: Core AtomSpace${NC}"
echo -e "${CYAN}========================================${NC}"

build_package "atomspace" || exit 1

# Stage 3: Storage Backends (CRITICAL: Before CogServer)
echo ""
echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Stage 3: Storage Backends${NC}"
echo -e "${CYAN}========================================${NC}"

build_package "atomspace-storage" || exit 1
build_package "atomspace-cog" || exit 1
build_package "atomspace-rocks" || exit 1
build_package "atomspace-pgres" || true  # Optional

# Stage 3.5: AGI-OS MachSpace Integration (Optional)
if [ "${BUILD_AGI_OS}" = "yes" ]; then
    echo ""
    echo -e "${CYAN}========================================${NC}"
    echo -e "${CYAN}Stage 3.5: AGI-OS MachSpace Integration${NC}"
    echo -e "${CYAN}========================================${NC}"
    
    build_package "atomspace-machspace" || true
fi

# Stage 4: Core Services
echo ""
echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Stage 4: Core Services${NC}"
echo -e "${CYAN}========================================${NC}"

build_package "cogserver" || exit 1
build_package "ure" || exit 1

# Stage 4.5: AGI-OS Operating System Layer (Optional)
if [ "${BUILD_AGI_OS}" = "yes" ]; then
    echo ""
    echo -e "${CYAN}========================================${NC}"
    echo -e "${CYAN}Stage 4.5: AGI-OS Operating System${NC}"
    echo -e "${CYAN}========================================${NC}"
    
    build_package "hurdcog" || exit 1
    build_package "hurdcog-cogkernel-core" || true
    build_package "hurdcog-atomspace-bridge" || true
    build_package "hurdcog-machspace" || true
    build_package "hurdcog-occ-bridge" || true
fi

# Stage 5: Cognitive Components
echo ""
echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Stage 5: Cognitive Components${NC}"
echo -e "${CYAN}========================================${NC}"

build_package "attention" || true
build_package "pln" || exit 1
build_package "miner" || true
build_package "unify" || true
build_package "spacetime" || true

# Stage 6: Learning and Generation
echo ""
echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Stage 6: Learning and Generation${NC}"
echo -e "${CYAN}========================================${NC}"

build_package "learn" || true
build_package "generate" || true

# Stage 7: Natural Language Processing
echo ""
echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Stage 7: Natural Language Processing${NC}"
echo -e "${CYAN}========================================${NC}"

build_package "lg-atomese" || true
build_package "relex" || true

# Stage 8: Evolutionary and Specialized Systems
echo ""
echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Stage 8: Specialized Systems${NC}"
echo -e "${CYAN}========================================${NC}"

build_package "moses" || true
build_package "asmoses" || true
build_package "agi-bio" || true
build_package "vision" || true

# Stage 9: Meta-Package
echo ""
echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Stage 9: Meta-Package${NC}"
echo -e "${CYAN}========================================${NC}"

build_package "opencog" || true

# Stage 10: AGI-OS Unified Package (Optional)
if [ "${BUILD_AGI_OS}" = "yes" ]; then
    echo ""
    echo -e "${CYAN}========================================${NC}"
    echo -e "${CYAN}Stage 10: AGI-OS Unified Package${NC}"
    echo -e "${CYAN}========================================${NC}"
    
    build_package "agi-os-unified" || true
    build_package "agi-os-cognitive-init" || true
    build_package "agi-os-monitoring" || true
fi

# Summary
echo ""
echo -e "${CYAN}========================================${NC}"
echo -e "${GREEN}Build Complete!${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""
echo -e "${BLUE}Build artifacts saved to:${NC} ${BUILD_DIR}"
echo ""
echo -e "${BLUE}Package count:${NC}"
ls -1 "${BUILD_DIR}"/*.deb 2>/dev/null | wc -l
echo ""
echo -e "${BLUE}Total size:${NC}"
du -sh "${BUILD_DIR}" | cut -f1
echo ""

if [ "${INSTALL_DEBS}" = "yes" ]; then
    echo -e "${GREEN}All packages have been installed.${NC}"
else
    echo -e "${YELLOW}Packages built but not installed.${NC}"
    echo -e "${YELLOW}To install, run:${NC}"
    echo -e "  sudo dpkg -i ${BUILD_DIR}/*.deb"
    echo -e "  sudo apt-get install -f"
fi

echo ""
echo -e "${CYAN}========================================${NC}"
