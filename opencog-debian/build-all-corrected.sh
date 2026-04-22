#!/bin/bash
# build-all-corrected.sh - Build all AGI-OS packages in CORRECT order
# This script includes the critical atomspace-storage package that was missing
# from previous build scripts.

set -e

echo "=========================================="
echo "OpenCog AGI-OS Build Script (Corrected)"
echo "Version: 3.0"
echo "Date: December 12, 2025"
echo "=========================================="
echo ""
echo "CRITICAL FIX: This build includes atomspace-storage"
echo "Previous builds were missing this mandatory component!"
echo ""

PACKAGES=(
    "cogutil"
    "atomspace"
    "atomspace-storage"          # CRITICAL: Added to build order
    "atomspace-cog"
    "atomspace-rocks"
    "atomspace-pgres"
    "cogserver"
    "ure"
    "hurdcog"
    "hurdcog-machspace"
    "hurdcog-cogkernel-core"
    "hurdcog-atomspace-bridge"
    "cognumach-cognitive-scheduler"
    "hurdcog-occ-bridge"
    "attention"
    "pln"
    "miner"
    "unify"
    "spacetime"
    "learn"
    "generate"
    "lg-atomese"
    "relex"
    "moses"
    "asmoses"
    "agi-bio"
    "vision"
    "opencog"
    "agi-os-unified"
    "agi-os-monitoring"
    "agi-os-cognitive-init"
)

TOTAL=${#PACKAGES[@]}
CURRENT=0

for pkg in "${PACKAGES[@]}"; do
    CURRENT=$((CURRENT + 1))
    echo ""
    echo "=========================================="
    echo "Building package $CURRENT of $TOTAL: $pkg"
    echo "=========================================="
    
    if [ ! -d "$pkg" ]; then
        echo "WARNING: Package directory $pkg not found, skipping..."
        continue
    fi
    
    cd "$pkg"
    
    if [ ! -f "update-$pkg.sh" ]; then
        echo "WARNING: update-$pkg.sh not found, skipping..."
        cd ..
        continue
    fi
    
    ./update-$pkg.sh
    
    PKG_DIR=$(ls -d ${pkg}-* 2>/dev/null | head -1)
    if [ -z "$PKG_DIR" ]; then
        echo "ERROR: Package directory not created by update script"
        cd ..
        continue
    fi
    
    cd "$PKG_DIR"
    
    echo "Installing build dependencies..."
    sudo apt-get build-dep -y . || true
    
    echo "Building package..."
    dpkg-buildpackage -rfakeroot -us -uc -j$(nproc) || {
        echo "ERROR: Build failed for $pkg"
        cd ../..
        exit 1
    }
    
    echo "Installing package..."
    sudo dpkg -i ../*.deb || sudo apt-get install -f -y
    
    cd ../..
    
    echo "Package $pkg built and installed successfully"
done

echo ""
echo "=========================================="
echo "All AGI-OS packages built successfully!"
echo "=========================================="
echo ""
echo "Total packages built: $TOTAL"
echo ""
echo "Next steps:"
echo "  1. Run integration tests"
echo "  2. Verify all services start correctly"
echo "  3. Test AGI-OS functionality"
echo ""
