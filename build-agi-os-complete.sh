#!/bin/bash
#
# build-agi-os-complete.sh
# Complete AGI-OS build script for all three layers
#
# Version: 3.0
# Date: December 25, 2025
# Purpose: Build Cognumach + HurdCog + OCC in correct dependency order
#

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="${BUILD_DIR:-$(pwd)/build}"
INSTALL_PREFIX="${INSTALL_PREFIX:-/usr/local}"
PARALLEL_JOBS="${PARALLEL_JOBS:-$(nproc)}"
BUILD_COGNUMACH="${BUILD_COGNUMACH:-yes}"
BUILD_HURDCOG="${BUILD_HURDCOG:-yes}"
BUILD_OCC_CORE="${BUILD_OCC_CORE:-yes}"
BUILD_INTEGRATION="${BUILD_INTEGRATION:-yes}"
BUILD_OCC_COGNITIVE="${BUILD_OCC_COGNITIVE:-yes}"
BUILD_TESTS="${BUILD_TESTS:-no}"

# Print banner
echo -e "${BLUE}"
echo "=========================================="
echo "AGI-OS Complete Build System"
echo "Version 3.0 - December 25, 2025"
echo "=========================================="
echo -e "${NC}"
echo ""
echo "Configuration:"
echo "  Build directory: $BUILD_DIR"
echo "  Install prefix: $INSTALL_PREFIX"
echo "  Parallel jobs: $PARALLEL_JOBS"
echo "  Build Cognumach: $BUILD_COGNUMACH"
echo "  Build HurdCog: $BUILD_HURDCOG"
echo "  Build OCC Core: $BUILD_OCC_CORE"
echo "  Build Integration: $BUILD_INTEGRATION"
echo "  Build OCC Cognitive: $BUILD_OCC_COGNITIVE"
echo "  Build Tests: $BUILD_TESTS"
echo ""

# Function to print section headers
print_section() {
    echo ""
    echo -e "${GREEN}=========================================="
    echo "$1"
    echo -e "==========================================${NC}"
    echo ""
}

# Function to print status
print_status() {
    echo -e "${BLUE}[$(date '+%H:%M:%S')]${NC} $1"
}

# Function to print warning
print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Function to print error
print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check dependencies
check_dependencies() {
    print_section "Checking Build Dependencies"
    
    local missing_deps=()
    
    # Check for basic build tools
    for cmd in gcc g++ make cmake autoconf automake libtool pkg-config; do
        if ! command -v $cmd &> /dev/null; then
            missing_deps+=($cmd)
        fi
    done
    
    # Check for Python
    if ! command -v python3 &> /dev/null; then
        missing_deps+=(python3)
    fi
    
    # Check for Guile
    if ! command -v guile &> /dev/null; then
        print_warning "Guile not found - some components may not build"
    fi
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_error "Missing dependencies: ${missing_deps[*]}"
        echo ""
        echo "Install with:"
        echo "  sudo apt-get install ${missing_deps[*]}"
        exit 1
    fi
    
    print_status "All required dependencies found"
}

# ============================================================================
# Layer 0: Build Tools Foundation
# ============================================================================

build_mig() {
    if [ "$BUILD_COGNUMACH" != "yes" ] && [ "$BUILD_HURDCOG" != "yes" ]; then
        print_status "Skipping MIG build (not needed)"
        return 0
    fi
    
    print_section "Layer 0: Building MIG (Mach Interface Generator)"
    
    cd build-tools/mig
    
    if [ ! -f configure ]; then
        print_status "Running autoreconf..."
        autoreconf -fi
    fi
    
    print_status "Configuring MIG..."
    ./configure --prefix="$INSTALL_PREFIX"
    
    print_status "Building MIG..."
    make -j"$PARALLEL_JOBS"
    
    print_status "Installing MIG..."
    sudo make install
    
    cd ../..
    print_status "MIG build complete"
}

# ============================================================================
# Layer 1: Microkernel Foundation (Cognumach)
# ============================================================================

build_cognumach() {
    if [ "$BUILD_COGNUMACH" != "yes" ]; then
        print_status "Skipping Cognumach build"
        return 0
    fi
    
    print_section "Layer 1: Building Cognumach Microkernel"
    
    cd cognumach
    
    if [ ! -f configure ]; then
        print_status "Running autoreconf..."
        autoreconf -fi
    fi
    
    print_status "Configuring Cognumach..."
    ./configure --prefix="$INSTALL_PREFIX"
    
    print_status "Building Cognumach (this may take ~30 minutes)..."
    make -j"$PARALLEL_JOBS"
    
    print_status "Installing Cognumach..."
    sudo make install
    
    cd ..
    print_status "Cognumach build complete"
}

# ============================================================================
# Layer 2: Cognitive Operating System (HurdCog)
# ============================================================================

build_hurdcog() {
    if [ "$BUILD_HURDCOG" != "yes" ]; then
        print_status "Skipping HurdCog build"
        return 0
    fi
    
    print_section "Layer 2: Building HurdCog Operating System"
    
    cd hurdcog
    
    if [ ! -f configure ]; then
        print_status "Running autoreconf..."
        autoreconf -fi
    fi
    
    print_status "Configuring HurdCog..."
    ./configure --prefix="$INSTALL_PREFIX"
    
    print_status "Building HurdCog (this may take ~45 minutes)..."
    make -j"$PARALLEL_JOBS"
    
    print_status "Installing HurdCog..."
    sudo make install
    
    # Install cognitive kernel components
    if [ -d cogkernel ]; then
        print_status "Installing cognitive kernel components..."
        sudo mkdir -p "$INSTALL_PREFIX/share/hurdcog/cogkernel"
        sudo cp -r cogkernel/* "$INSTALL_PREFIX/share/hurdcog/cogkernel/"
    fi
    
    cd ..
    print_status "HurdCog build complete"
}

# ============================================================================
# Layer 3: OCC Core Components
# ============================================================================

build_occ_core() {
    if [ "$BUILD_OCC_CORE" != "yes" ]; then
        print_status "Skipping OCC core build"
        return 0
    fi
    
    print_section "Layer 3: Building OCC Core Components"
    
    print_status "OCC components use Debian packaging system"
    print_status "Building packages in correct dependency order..."
    
    cd opencog-debian
    
    # Array of core packages in build order
    local core_packages=(
        "cogutil"
        "atomspace"
        "atomspace-storage"  # CRITICAL: Must be before cogserver
        "atomspace-rocks"
        "atomspace-pgres"
        "cogserver"
        "ure"
    )
    
    for pkg in "${core_packages[@]}"; do
        print_status "Building $pkg..."
        
        if [ ! -d "$pkg" ]; then
            print_warning "Package directory $pkg not found, skipping"
            continue
        fi
        
        cd "$pkg"
        
        # Run update script
        if [ -f "update-$pkg.sh" ]; then
            ./update-$pkg.sh
        else
            print_warning "Update script for $pkg not found"
        fi
        
        # Find source directory
        local src_dir=$(ls -d ${pkg}-* 2>/dev/null | head -1)
        
        if [ -z "$src_dir" ]; then
            print_warning "Source directory for $pkg not found, skipping"
            cd ..
            continue
        fi
        
        cd "$src_dir"
        
        # Install build dependencies
        print_status "Installing build dependencies for $pkg..."
        sudo apt-get build-dep -y . || true
        
        # Build package
        print_status "Building Debian package for $pkg..."
        dpkg-buildpackage -rfakeroot -us -uc -j"$PARALLEL_JOBS" || {
            print_error "Failed to build $pkg"
            cd ../..
            return 1
        }
        
        # Install package
        print_status "Installing $pkg..."
        sudo dpkg -i ../*.deb || sudo apt-get install -f -y
        
        cd ../..
    done
    
    cd ..
    print_status "OCC core build complete"
}

# ============================================================================
# Layer 4: Integration Bridges
# ============================================================================

build_integration() {
    if [ "$BUILD_INTEGRATION" != "yes" ]; then
        print_status "Skipping integration layer build"
        return 0
    fi
    
    print_section "Layer 4: Building Integration Bridges"
    
    if [ ! -d integration ]; then
        print_warning "Integration directory not found, skipping"
        return 0
    fi
    
    cd integration
    
    # Create build directory
    mkdir -p build
    cd build
    
    print_status "Configuring integration layer..."
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
        -DBUILD_ATOMSPACE_MACHSPACE=ON \
        -DBUILD_COGNITIVE_SCHEDULER=ON
    
    print_status "Building integration components..."
    make -j"$PARALLEL_JOBS"
    
    print_status "Installing integration components..."
    sudo make install
    
    cd ../..
    print_status "Integration layer build complete"
}

# ============================================================================
# Layer 5: OCC Cognitive Components
# ============================================================================

build_occ_cognitive() {
    if [ "$BUILD_OCC_COGNITIVE" != "yes" ]; then
        print_status "Skipping OCC cognitive components build"
        return 0
    fi
    
    print_section "Layer 5: Building OCC Cognitive Components"
    
    cd opencog-debian
    
    # Array of cognitive packages
    local cognitive_packages=(
        "attention"
        "pln"
        "miner"
        "unify"
        "spacetime"
        "learn"
        "generate"
    )
    
    for pkg in "${cognitive_packages[@]}"; do
        print_status "Building $pkg..."
        
        if [ ! -d "$pkg" ]; then
            print_warning "Package directory $pkg not found, skipping"
            continue
        fi
        
        cd "$pkg"
        
        if [ -f "update-$pkg.sh" ]; then
            ./update-$pkg.sh
        fi
        
        local src_dir=$(ls -d ${pkg}-* 2>/dev/null | head -1)
        
        if [ -z "$src_dir" ]; then
            print_warning "Source directory for $pkg not found, skipping"
            cd ..
            continue
        fi
        
        cd "$src_dir"
        
        sudo apt-get build-dep -y . || true
        dpkg-buildpackage -rfakeroot -us -uc -j"$PARALLEL_JOBS" || {
            print_warning "Failed to build $pkg, continuing..."
            cd ../..
            continue
        }
        
        sudo dpkg -i ../*.deb || sudo apt-get install -f -y
        
        cd ../..
    done
    
    cd ..
    print_status "OCC cognitive components build complete"
}

# ============================================================================
# Main Build Sequence
# ============================================================================

main() {
    local start_time=$(date +%s)
    
    # Check dependencies
    check_dependencies
    
    # Create build directory
    mkdir -p "$BUILD_DIR"
    
    # Build in correct order
    build_mig
    build_cognumach
    build_hurdcog
    build_occ_core
    build_integration
    build_occ_cognitive
    
    # Calculate build time
    local end_time=$(date +%s)
    local duration=$((end_time - start_time))
    local hours=$((duration / 3600))
    local minutes=$(((duration % 3600) / 60))
    local seconds=$((duration % 60))
    
    # Print summary
    echo ""
    echo -e "${GREEN}"
    echo "=========================================="
    echo "AGI-OS Build Complete!"
    echo "=========================================="
    echo -e "${NC}"
    echo ""
    echo "Build time: ${hours}h ${minutes}m ${seconds}s"
    echo ""
    echo "Installed to: $INSTALL_PREFIX"
    echo ""
    echo "Next steps:"
    echo "  1. Verify installation: ls -la $INSTALL_PREFIX"
    echo "  2. Run tests (if enabled): ctest"
    echo "  3. Read documentation: BUILD_DEPENDENCY_ORDER.md"
    echo ""
    echo -e "${GREEN}Cognitive synergy achieved across all three layers!${NC}"
    echo ""
}

# Run main build sequence
main "$@"
