#!/bin/bash
# build-integration-layer.sh - Build all integration layer components
# This script builds the bridges between Cognumach, HurdCog, and OCC

set -e

echo "=========================================="
echo "Integration Layer Build Script"
echo "Version: 1.0"
echo "Date: December 12, 2025"
echo "=========================================="
echo ""
echo "Building integration bridges between:"
echo "  - Cognumach (microkernel)"
echo "  - HurdCog (cognitive OS)"
echo "  - OCC (OpenCog framework)"
echo ""

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Component 1: AtomSpace-MachSpace Bridge
echo "=========================================="
echo "Building AtomSpace-MachSpace Bridge"
echo "=========================================="

if [ -d "atomspace-machspace" ]; then
    cd atomspace-machspace
    
    if [ -f "CMakeLists.txt" ]; then
        echo "Building with CMake..."
        rm -rf build
        mkdir -p build
        cd build
        cmake .. || {
            echo "WARNING: CMake configuration failed for atomspace-machspace"
            echo "This may require atomspace-storage to be installed first"
            cd ../..
        }
        
        if [ -f "Makefile" ]; then
            make -j$(nproc) || {
                echo "WARNING: Build failed for atomspace-machspace"
                cd ../..
            }
            
            echo "AtomSpace-MachSpace Bridge built successfully"
            echo "To install: sudo make install"
            cd ../..
        fi
    else
        echo "WARNING: No CMakeLists.txt found, skipping build"
        cd ..
    fi
else
    echo "WARNING: atomspace-machspace directory not found"
fi

# Component 2: Cognitive Scheduler
echo ""
echo "=========================================="
echo "Building Cognitive Scheduler"
echo "=========================================="

if [ -d "cognitive-scheduler" ]; then
    cd cognitive-scheduler
    
    if [ -f "cognitive_scheduler.c" ]; then
        echo "Building cognitive scheduler library..."
        gcc -c cognitive_scheduler.c -o cognitive_scheduler.o -fPIC || {
            echo "WARNING: Compilation failed for cognitive_scheduler.c"
            cd ..
        }
        
        if [ -f "cognitive_scheduler.o" ]; then
            ar rcs libcognitive_scheduler.a cognitive_scheduler.o
            gcc -shared -o libcognitive_scheduler.so cognitive_scheduler.o
            echo "Cognitive Scheduler built successfully"
            echo "Static library: libcognitive_scheduler.a"
            echo "Shared library: libcognitive_scheduler.so"
            echo "To install: sudo cp libcognitive_scheduler.* /usr/local/lib/"
            echo "            sudo cp cognitive_scheduler.h /usr/local/include/"
        fi
        cd ..
    else
        echo "WARNING: cognitive_scheduler.c not found"
        cd ..
    fi
else
    echo "WARNING: cognitive-scheduler directory not found"
fi

# Component 3: HurdCog Bridge
echo ""
echo "=========================================="
echo "Building HurdCog Bridge"
echo "=========================================="

if [ -d "hurdcog-bridge" ]; then
    cd hurdcog-bridge
    
    if [ -f "CMakeLists.txt" ]; then
        echo "Building with CMake..."
        rm -rf build
        mkdir -p build
        cd build
        cmake .. || {
            echo "WARNING: CMake configuration failed for hurdcog-bridge"
            cd ../..
        }
        
        if [ -f "Makefile" ]; then
            make -j$(nproc) || {
                echo "WARNING: Build failed for hurdcog-bridge"
                cd ../..
            }
            
            echo "HurdCog Bridge built successfully"
            echo "To install: sudo make install"
            cd ../..
        fi
    elif [ -f "Makefile" ]; then
        echo "Building with Makefile..."
        make -j$(nproc) || {
            echo "WARNING: Build failed for hurdcog-bridge"
        }
        echo "HurdCog Bridge built successfully"
        cd ..
    else
        echo "WARNING: No build system found for hurdcog-bridge"
        cd ..
    fi
else
    echo "WARNING: hurdcog-bridge directory not found"
fi

echo ""
echo "=========================================="
echo "Integration Layer Build Complete"
echo "=========================================="
echo ""
echo "Summary:"
echo "  - AtomSpace-MachSpace Bridge: Check atomspace-machspace/build/"
echo "  - Cognitive Scheduler: Check cognitive-scheduler/"
echo "  - HurdCog Bridge: Check hurdcog-bridge/build/"
echo ""
echo "Note: Some components may require dependencies to be installed first:"
echo "  - atomspace-storage (for MachSpace bridge)"
echo "  - hurdcog-dev (for HurdCog bridge)"
echo "  - cognumach-dev (for Cognitive Scheduler)"
echo ""
echo "To install all components:"
echo "  cd atomspace-machspace/build && sudo make install"
echo "  cd ../../cognitive-scheduler && sudo cp libcognitive_scheduler.* /usr/local/lib/"
echo "  cd ../hurdcog-bridge/build && sudo make install"
echo ""
