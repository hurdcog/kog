#!/bin/bash
# Enhanced OpenCog Debian Package Builder
# Builds all packages in correct dependency order with parallel support

set -e

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
PARALLEL_JOBS=${PARALLEL_JOBS:-$(nproc)}
BUILD_DIR=${BUILD_DIR:-$(pwd)/build}
LOG_DIR=${LOG_DIR:-$(pwd)/logs}
SKIP_TESTS=${SKIP_TESTS:-0}
BUILD_AGI_OS=${BUILD_AGI_OS:-0}

# Create directories
mkdir -p "$BUILD_DIR" "$LOG_DIR"

# Logging function
log() {
    local level=$1
    shift
    local message="$@"
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S')
    
    case $level in
        INFO)
            echo -e "${CYAN}[INFO]${NC} $message"
            ;;
        SUCCESS)
            echo -e "${GREEN}[SUCCESS]${NC} $message"
            ;;
        WARNING)
            echo -e "${YELLOW}[WARNING]${NC} $message"
            ;;
        ERROR)
            echo -e "${RED}[ERROR]${NC} $message"
            ;;
        HEADER)
            echo -e "\n${BLUE}========================================${NC}"
            echo -e "${BLUE}$message${NC}"
            echo -e "${BLUE}========================================${NC}\n"
            ;;
    esac
    
    echo "[$timestamp] [$level] $message" >> "$LOG_DIR/build.log"
}

# Build a single package
build_package() {
    local pkg=$1
    local stage=$2
    
    log HEADER "Building $pkg (Stage $stage)"
    
    local pkg_log="$LOG_DIR/${pkg}.log"
    
    # Navigate to package directory
    if [ ! -d "$pkg" ]; then
        log ERROR "Package directory $pkg not found"
        return 1
    fi
    
    cd "$pkg"
    
    # Run update script
    if [ -f "update-${pkg}.sh" ]; then
        log INFO "Running update script for $pkg..."
        ./update-${pkg}.sh > "$pkg_log" 2>&1
        if [ $? -ne 0 ]; then
            log ERROR "Update script failed for $pkg (see $pkg_log)"
            cd ..
            return 1
        fi
    else
        log WARNING "No update script found for $pkg"
    fi
    
    # Find source directory
    local src_dir=$(find . -maxdepth 1 -type d -name "${pkg}-*" | head -1)
    if [ -z "$src_dir" ]; then
        log ERROR "Source directory not found for $pkg"
        cd ..
        return 1
    fi
    
    cd "$src_dir"
    
    # Install build dependencies
    log INFO "Installing build dependencies for $pkg..."
    sudo apt-get build-dep -y . >> "$pkg_log" 2>&1
    
    # Build package
    log INFO "Building $pkg with $PARALLEL_JOBS parallel jobs..."
    dpkg-buildpackage -rfakeroot -us -uc -j${PARALLEL_JOBS} >> "$pkg_log" 2>&1
    
    if [ $? -ne 0 ]; then
        log ERROR "Build failed for $pkg (see $pkg_log)"
        cd ../..
        return 1
    fi
    
    # Install package
    log INFO "Installing $pkg..."
    cd ..
    sudo dpkg -i *.deb >> "$pkg_log" 2>&1 || sudo apt-get install -f -y >> "$pkg_log" 2>&1
    
    if [ $? -eq 0 ]; then
        log SUCCESS "$pkg built and installed successfully"
    else
        log ERROR "Installation failed for $pkg (see $pkg_log)"
        cd ..
        return 1
    fi
    
    cd ..
    return 0
}

# Build packages in parallel
build_parallel() {
    local stage=$1
    shift
    local packages=("$@")
    
    log HEADER "Stage $stage: Building ${#packages[@]} packages in parallel"
    
    local pids=()
    local failed=()
    
    for pkg in "${packages[@]}"; do
        build_package "$pkg" "$stage" &
        pids+=($!)
    done
    
    # Wait for all background jobs
    for i in "${!pids[@]}"; do
        wait ${pids[$i]}
        if [ $? -ne 0 ]; then
            failed+=("${packages[$i]}")
        fi
    done
    
    if [ ${#failed[@]} -gt 0 ]; then
        log ERROR "Failed packages in stage $stage: ${failed[*]}"
        return 1
    fi
    
    log SUCCESS "Stage $stage completed successfully"
    return 0
}

# Main build sequence
main() {
    log HEADER "OpenCog Debian Package Build System"
    log INFO "Parallel jobs: $PARALLEL_JOBS"
    log INFO "Build directory: $BUILD_DIR"
    log INFO "Log directory: $LOG_DIR"
    log INFO "Build AGI-OS components: $BUILD_AGI_OS"
    
    # Stage 0: Microkernel (optional)
    if [ $BUILD_AGI_OS -eq 1 ]; then
        build_package "cognumach" "0" || exit 1
    fi
    
    # Stage 1: Foundation
    build_package "cogutil" "1" || exit 1
    
    # Stage 2: Core AtomSpace
    build_package "atomspace" "2" || exit 1
    
    # Stage 3: Storage Backends (parallel)
    build_parallel "3" "atomspace-cog" "atomspace-rocks" "atomspace-pgres" || exit 1
    
    # Stage 4: Core Services (parallel)
    build_parallel "4" "cogserver" "ure" || exit 1
    
    # Stage 4.5: Cognitive OS Layer (optional)
    if [ $BUILD_AGI_OS -eq 1 ]; then
        build_parallel "4.5" "hurdcog" "hurdcog-cogkernel-core" "hurdcog-machspace" "hurdcog-occ-bridge" || exit 1
    fi
    
    # Stage 5: Cognitive Components (parallel)
    build_parallel "5" "attention" "pln" "miner" "unify" "spacetime" || exit 1
    
    # Stage 6: Learning and Generation (parallel)
    build_parallel "6" "learn" "generate" || exit 1
    
    # Stage 7: NLP (parallel)
    build_parallel "7" "lg-atomese" "relex" || exit 1
    
    # Stage 8: Evolutionary and Specialized (parallel)
    build_parallel "8" "moses" "asmoses" "agi-bio" "vision" || exit 1
    
    # Stage 9: Meta-package
    build_package "opencog" "9" || exit 1
    
    # Stage 10: AGI-OS Unified (optional)
    if [ $BUILD_AGI_OS -eq 1 ]; then
        build_package "agi-os-unified" "10" || exit 1
    fi
    
    log HEADER "Build Complete!"
    log SUCCESS "All packages built and installed successfully"
    log INFO "Build logs available in: $LOG_DIR"
    
    # Generate build report
    generate_report
}

# Generate build report
generate_report() {
    local report="$LOG_DIR/build-report.txt"
    
    {
        echo "OpenCog Debian Package Build Report"
        echo "===================================="
        echo ""
        echo "Build Date: $(date)"
        echo "Parallel Jobs: $PARALLEL_JOBS"
        echo "Build AGI-OS: $BUILD_AGI_OS"
        echo ""
        echo "Installed Packages:"
        echo "-------------------"
        dpkg -l | grep -E "(opencog|cogutil|cognumach|hurdcog|agi-os)" || echo "No packages found"
        echo ""
        echo "Build Logs:"
        echo "-----------"
        ls -lh "$LOG_DIR"/*.log
    } > "$report"
    
    log INFO "Build report generated: $report"
}

# Cleanup function
cleanup() {
    log INFO "Cleaning up build artifacts..."
    find . -name "*.deb" -o -name "*.changes" -o -name "*.buildinfo" | xargs rm -f
    log SUCCESS "Cleanup complete"
}

# Help function
show_help() {
    cat << EOF
OpenCog Debian Package Build System

Usage: $0 [OPTIONS]

Options:
    -h, --help              Show this help message
    -j, --jobs N            Number of parallel jobs (default: $(nproc))
    -a, --agi-os            Build AGI-OS components (cognumach, hurdcog)
    -t, --skip-tests        Skip running tests
    -c, --clean             Clean build artifacts
    -l, --log-dir DIR       Log directory (default: ./logs)

Environment Variables:
    PARALLEL_JOBS           Number of parallel jobs
    BUILD_DIR               Build directory
    LOG_DIR                 Log directory
    SKIP_TESTS              Skip tests (0 or 1)
    BUILD_AGI_OS            Build AGI-OS components (0 or 1)

Examples:
    # Build all OpenCog packages with 8 parallel jobs
    $0 -j 8

    # Build including AGI-OS components
    $0 --agi-os

    # Clean build artifacts
    $0 --clean

EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -j|--jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        -a|--agi-os)
            BUILD_AGI_OS=1
            shift
            ;;
        -t|--skip-tests)
            SKIP_TESTS=1
            shift
            ;;
        -c|--clean)
            cleanup
            exit 0
            ;;
        -l|--log-dir)
            LOG_DIR="$2"
            shift 2
            ;;
        *)
            log ERROR "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Run main build
main

exit 0
