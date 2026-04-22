#!/bin/bash
# build-all-packages.sh - Build all OpenCog Debian packages in correct dependency order
# This script automates the complete build process for the AGI-OS stack

set -e  # Exit on error

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="${BUILD_DIR:-$(pwd)}"
PARALLEL_JOBS="${PARALLEL_JOBS:-$(nproc)}"
INSTALL_PACKAGES="${INSTALL_PACKAGES:-yes}"
BUILD_AGI_OS="${BUILD_AGI_OS:-no}"

# Logging
LOG_DIR="$BUILD_DIR/build-logs"
mkdir -p "$LOG_DIR"
MAIN_LOG="$LOG_DIR/build-all-$(date +%Y%m%d-%H%M%S).log"

log() {
    echo -e "${GREEN}[$(date +%H:%M:%S)]${NC} $*" | tee -a "$MAIN_LOG"
}

log_error() {
    echo -e "${RED}[$(date +%H:%M:%S)] ERROR:${NC} $*" | tee -a "$MAIN_LOG"
}

log_warn() {
    echo -e "${YELLOW}[$(date +%H:%M:%S)] WARNING:${NC} $*" | tee -a "$MAIN_LOG"
}

log_info() {
    echo -e "${BLUE}[$(date +%H:%M:%S)] INFO:${NC} $*" | tee -a "$MAIN_LOG"
}

# Package build order (following dependency chain)
STAGE_0_PACKAGES=()  # Microkernel (optional)
STAGE_1_PACKAGES=("cogutil")  # Foundation
STAGE_2_PACKAGES=("atomspace")  # Core AtomSpace
STAGE_3_PACKAGES=("atomspace-cog" "atomspace-rocks" "atomspace-pgres")  # Storage backends
STAGE_4_PACKAGES=("cogserver" "ure")  # Core services
STAGE_5_PACKAGES=("attention" "pln" "miner" "unify" "spacetime")  # Cognitive components
STAGE_6_PACKAGES=("learn" "generate")  # Learning and generation
STAGE_7_PACKAGES=("lg-atomese" "relex")  # NLP components
STAGE_8_PACKAGES=("moses" "asmoses" "agi-bio" "vision")  # Specialized systems
STAGE_9_PACKAGES=("opencog")  # Meta-package

# AGI-OS specific packages (only if BUILD_AGI_OS=yes)
AGI_OS_STAGE_0=("cognumach")  # Microkernel
AGI_OS_STAGE_10=("hurdcog" "hurdcog-machspace" "hurdcog-cogkernel-core" "hurdcog-occ-bridge")  # HurdCog Layer 1 Core
AGI_OS_STAGE_13=("agi-os-unified")  # Complete AGI-OS stack

# Statistics
TOTAL_PACKAGES=0
BUILT_PACKAGES=0
FAILED_PACKAGES=()
SKIPPED_PACKAGES=()
START_TIME=$(date +%s)

# Function to build a single package
build_package() {
    local pkg=$1
    local stage=$2
    
    log_info "Building $pkg (Stage $stage)"
    
    local pkg_log="$LOG_DIR/${pkg}-$(date +%Y%m%d-%H%M%S).log"
    
    # Check if package directory exists
    if [ ! -d "$pkg" ]; then
        log_error "Package directory $pkg not found"
        FAILED_PACKAGES+=("$pkg")
        return 1
    fi
    
    cd "$pkg"
    
    # Run update script
    if [ -f "update-${pkg}.sh" ]; then
        log_info "Running update script for $pkg"
        if ! bash "update-${pkg}.sh" >> "$pkg_log" 2>&1; then
            log_error "Update script failed for $pkg"
            cd "$BUILD_DIR"
            FAILED_PACKAGES+=("$pkg")
            return 1
        fi
    else
        log_warn "No update script found for $pkg, skipping"
        cd "$BUILD_DIR"
        SKIPPED_PACKAGES+=("$pkg")
        return 0
    fi
    
    # Find the source directory
    local src_dir=$(find . -maxdepth 1 -type d -name "${pkg}-*" | head -1)
    
    if [ -z "$src_dir" ]; then
        log_error "Source directory not found for $pkg"
        cd "$BUILD_DIR"
        FAILED_PACKAGES+=("$pkg")
        return 1
    fi
    
    cd "$src_dir"
    
    # Install build dependencies
    log_info "Installing build dependencies for $pkg"
    if ! sudo apt-get build-dep -y . >> "$pkg_log" 2>&1; then
        log_warn "Some build dependencies could not be installed for $pkg"
    fi
    
    # Build the package
    log_info "Building package $pkg"
    if ! dpkg-buildpackage -rfakeroot -us -uc -j${PARALLEL_JOBS} >> "$pkg_log" 2>&1; then
        log_error "Build failed for $pkg (see $pkg_log)"
        cd "$BUILD_DIR"
        FAILED_PACKAGES+=("$pkg")
        return 1
    fi
    
    cd "$BUILD_DIR/$pkg"
    
    # Install the package if requested
    if [ "$INSTALL_PACKAGES" = "yes" ]; then
        log_info "Installing $pkg"
        if ! sudo dpkg -i ../*.deb >> "$pkg_log" 2>&1; then
            log_warn "Package installation failed, attempting to fix dependencies"
            if ! sudo apt-get install -f -y >> "$pkg_log" 2>&1; then
                log_error "Failed to fix dependencies for $pkg"
                cd "$BUILD_DIR"
                FAILED_PACKAGES+=("$pkg")
                return 1
            fi
        fi
    fi
    
    cd "$BUILD_DIR"
    BUILT_PACKAGES=$((BUILT_PACKAGES + 1))
    log "Successfully built $pkg ($BUILT_PACKAGES/$TOTAL_PACKAGES)"
    
    return 0
}

# Function to build packages in parallel
build_stage_parallel() {
    local stage=$1
    shift
    local packages=("$@")
    
    log "Building Stage $stage (${#packages[@]} packages in parallel)"
    
    local pids=()
    for pkg in "${packages[@]}"; do
        build_package "$pkg" "$stage" &
        pids+=($!)
    done
    
    # Wait for all parallel builds to complete
    local failed=0
    for pid in "${pids[@]}"; do
        if ! wait $pid; then
            failed=1
        fi
    done
    
    return $failed
}

# Function to build packages sequentially
build_stage_sequential() {
    local stage=$1
    shift
    local packages=("$@")
    
    log "Building Stage $stage (${#packages[@]} packages sequentially)"
    
    for pkg in "${packages[@]}"; do
        if ! build_package "$pkg" "$stage"; then
            return 1
        fi
    done
    
    return 0
}

# Print header
echo ""
echo "=========================================="
echo "OpenCog Debian Package Build System"
echo "=========================================="
echo ""
log_info "Build directory: $BUILD_DIR"
log_info "Parallel jobs: $PARALLEL_JOBS"
log_info "Install packages: $INSTALL_PACKAGES"
log_info "Build AGI-OS: $BUILD_AGI_OS"
log_info "Log directory: $LOG_DIR"
echo ""

# Calculate total packages
TOTAL_PACKAGES=$((
    ${#STAGE_1_PACKAGES[@]} +
    ${#STAGE_2_PACKAGES[@]} +
    ${#STAGE_3_PACKAGES[@]} +
    ${#STAGE_4_PACKAGES[@]} +
    ${#STAGE_5_PACKAGES[@]} +
    ${#STAGE_6_PACKAGES[@]} +
    ${#STAGE_7_PACKAGES[@]} +
    ${#STAGE_8_PACKAGES[@]} +
    ${#STAGE_9_PACKAGES[@]}
))

if [ "$BUILD_AGI_OS" = "yes" ]; then
    TOTAL_PACKAGES=$((
        TOTAL_PACKAGES +
        ${#AGI_OS_STAGE_0[@]} +
        ${#AGI_OS_STAGE_10[@]} +
        ${#AGI_OS_STAGE_13[@]}
    ))
fi

log_info "Total packages to build: $TOTAL_PACKAGES"
echo ""

# Build AGI-OS Stage 0 (Microkernel) if requested
if [ "$BUILD_AGI_OS" = "yes" ] && [ ${#AGI_OS_STAGE_0[@]} -gt 0 ]; then
    build_stage_sequential "AGI-OS-0" "${AGI_OS_STAGE_0[@]}" || log_error "Stage AGI-OS-0 failed"
fi

# Build Stage 1 (Foundation)
build_stage_sequential "1" "${STAGE_1_PACKAGES[@]}" || { log_error "Stage 1 failed"; exit 1; }

# Build Stage 2 (Core AtomSpace)
build_stage_sequential "2" "${STAGE_2_PACKAGES[@]}" || { log_error "Stage 2 failed"; exit 1; }

# Build Stage 3 (Storage backends) - can be parallel
build_stage_parallel "3" "${STAGE_3_PACKAGES[@]}" || log_warn "Some Stage 3 packages failed"

# Build Stage 4 (Core services) - can be parallel
build_stage_parallel "4" "${STAGE_4_PACKAGES[@]}" || { log_error "Stage 4 failed"; exit 1; }

# Build Stage 10 (HurdCog Layer 1 Core) if requested - sequential for dependencies
if [ "$BUILD_AGI_OS" = "yes" ] && [ ${#AGI_OS_STAGE_10[@]} -gt 0 ]; then
    build_stage_sequential "10" "${AGI_OS_STAGE_10[@]}" || log_error "Stage 10 (HurdCog Core) failed"
fi

# Build Stage 5 (Cognitive components) - can be parallel
build_stage_parallel "5" "${STAGE_5_PACKAGES[@]}" || log_warn "Some Stage 5 packages failed"

# Build Stage 6 (Learning and generation) - can be parallel
build_stage_parallel "6" "${STAGE_6_PACKAGES[@]}" || log_warn "Some Stage 6 packages failed"

# Build Stage 7 (NLP) - can be parallel
build_stage_parallel "7" "${STAGE_7_PACKAGES[@]}" || log_warn "Some Stage 7 packages failed"

# Build Stage 8 (Specialized systems) - can be parallel
build_stage_parallel "8" "${STAGE_8_PACKAGES[@]}" || log_warn "Some Stage 8 packages failed"

# Build Stage 9 (Meta-package)
build_stage_sequential "9" "${STAGE_9_PACKAGES[@]}" || log_warn "Stage 9 failed"

# Build Stage 13 (AGI-OS Unified) if requested
if [ "$BUILD_AGI_OS" = "yes" ] && [ ${#AGI_OS_STAGE_13[@]} -gt 0 ]; then
    build_stage_sequential "13" "${AGI_OS_STAGE_13[@]}" || log_error "Stage 13 (AGI-OS Unified) failed"
fi

# Calculate statistics
END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))
HOURS=$((DURATION / 3600))
MINUTES=$(((DURATION % 3600) / 60))
SECONDS=$((DURATION % 60))

# Print summary
echo ""
echo "=========================================="
echo "Build Summary"
echo "=========================================="
log_info "Total packages: $TOTAL_PACKAGES"
log_info "Successfully built: $BUILT_PACKAGES"
log_info "Failed: ${#FAILED_PACKAGES[@]}"
log_info "Skipped: ${#SKIPPED_PACKAGES[@]}"
log_info "Build time: ${HOURS}h ${MINUTES}m ${SECONDS}s"
echo ""

if [ ${#FAILED_PACKAGES[@]} -gt 0 ]; then
    log_error "Failed packages:"
    for pkg in "${FAILED_PACKAGES[@]}"; do
        echo "  - $pkg"
    done
    echo ""
fi

if [ ${#SKIPPED_PACKAGES[@]} -gt 0 ]; then
    log_warn "Skipped packages:"
    for pkg in "${SKIPPED_PACKAGES[@]}"; do
        echo "  - $pkg"
    done
    echo ""
fi

log_info "Main log: $MAIN_LOG"
log_info "Package logs: $LOG_DIR/"
echo ""

if [ ${#FAILED_PACKAGES[@]} -eq 0 ]; then
    log "All packages built successfully!"
    exit 0
else
    log_error "Some packages failed to build"
    exit 1
fi
