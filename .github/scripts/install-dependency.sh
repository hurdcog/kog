#!/bin/bash
# install-dependency.sh
# Modular dependency installer with fallback to source build
# Optimized for distributed cognitive synergy in CI/CD environments
#
# Usage: install-dependency.sh <package-name> [source-url] [build-commands]
#
# Examples:
#   install-dependency.sh libsparsehash-dev https://github.com/sparsehash/sparsehash.git "autoreconf -i && ./configure && make && sudo make install"
#   install-dependency.sh cmake
#
# Features:
# - Updates apt repositories before attempting installation
# - Checks for package existence in apt cache
# - Attempts apt installation first (fastest path)
# - Falls back to source build if package not found
# - Rigorous error checking and logging
# - Modular design for easy extension

set -euo pipefail

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $*"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $*"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $*"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*"
}

# Function to update apt repositories
update_apt() {
    log_info "Updating apt repositories..."
    if sudo apt-get update -qq > /dev/null 2>&1; then
        log_success "Apt repositories updated successfully"
        return 0
    else
        log_warning "Failed to update apt repositories, continuing anyway..."
        return 1
    fi
}

# Function to check if a package exists in apt cache
package_exists() {
    local package=$1
    log_info "Checking if package '$package' exists in apt cache..."
    
    if apt-cache show "$package" > /dev/null 2>&1; then
        log_success "Package '$package' found in apt cache"
        return 0
    else
        log_warning "Package '$package' not found in apt cache"
        return 1
    fi
}

# Function to install package via apt
install_via_apt() {
    local package=$1
    log_info "Attempting to install '$package' via apt..."
    
    if sudo apt-get install -y "$package"; then
        log_success "Successfully installed '$package' via apt"
        return 0
    else
        log_error "Failed to install '$package' via apt"
        return 1
    fi
}

# Function to build and install from source
install_from_source() {
    local package=$1
    local source_url=$2
    local build_commands=$3
    
    log_info "Attempting to build '$package' from source..."
    log_info "Source URL: $source_url"
    
    # Create temporary directory for build
    local build_dir=$(mktemp -d)
    log_info "Build directory: $build_dir"
    
    # Clone or download source
    cd "$build_dir"
    
    if [[ $source_url == *.git ]]; then
        log_info "Cloning git repository..."
        if ! git clone --depth 1 "$source_url" source; then
            log_error "Failed to clone repository"
            rm -rf "$build_dir"
            return 1
        fi
        cd source
    else
        log_info "Downloading source archive..."
        local filename=$(basename "$source_url")
        if ! wget -q "$source_url" -O "$filename"; then
            log_error "Failed to download source"
            rm -rf "$build_dir"
            return 1
        fi
        
        # Extract based on file type
        if [[ $filename == *.tar.gz ]] || [[ $filename == *.tgz ]]; then
            tar -xzf "$filename"
        elif [[ $filename == *.tar.bz2 ]]; then
            tar -xjf "$filename"
        elif [[ $filename == *.zip ]]; then
            unzip -q "$filename"
        else
            log_error "Unsupported archive format: $filename"
            rm -rf "$build_dir"
            return 1
        fi
        
        # Enter the extracted directory
        cd "$(find . -maxdepth 1 -type d ! -name . | head -n 1)"
    fi
    
    # Execute build commands
    log_info "Executing build commands..."
    if eval "$build_commands"; then
        log_success "Successfully built and installed '$package' from source"
        cd /
        rm -rf "$build_dir"
        return 0
    else
        log_error "Failed to build '$package' from source"
        cd /
        rm -rf "$build_dir"
        return 1
    fi
}

# Function to verify installation
verify_installation() {
    local package=$1
    local verify_command=$2
    
    if [ -n "$verify_command" ]; then
        log_info "Verifying installation with custom command: $verify_command"
        if eval "$verify_command"; then
            log_success "Installation verified successfully"
            return 0
        else
            log_error "Installation verification failed"
            return 1
        fi
    fi
    
    # Default verification: check if package is installed
    if dpkg -l "$package" 2>/dev/null | grep -q "^ii"; then
        log_success "Package '$package' is installed"
        return 0
    else
        log_warning "Cannot verify installation of '$package' (not a .deb package)"
        return 0  # Don't fail if we can't verify
    fi
}

# Main installation logic
main() {
    local package=${1:-}
    local source_url=${2:-}
    local build_commands=${3:-}
    local verify_command=${4:-}
    
    if [ -z "$package" ]; then
        log_error "Usage: $0 <package-name> [source-url] [build-commands] [verify-command]"
        exit 1
    fi
    
    log_info "=== Installing dependency: $package ==="
    
    # Step 1: Update apt repositories
    update_apt || true  # Don't fail if update fails
    
    # Step 2: Try apt installation first
    if package_exists "$package"; then
        if install_via_apt "$package"; then
            verify_installation "$package" "$verify_command"
            log_success "=== Successfully installed $package via apt ==="
            exit 0
        fi
    fi
    
    # Step 3: Fall back to source installation if provided
    if [ -n "$source_url" ] && [ -n "$build_commands" ]; then
        log_warning "Falling back to source installation..."
        
        # Install build dependencies
        log_info "Installing build dependencies..."
        sudo apt-get install -y build-essential git wget unzip autoconf automake libtool || true
        
        if install_from_source "$package" "$source_url" "$build_commands"; then
            verify_installation "$package" "$verify_command"
            log_success "=== Successfully installed $package from source ==="
            exit 0
        fi
    fi
    
    # Step 4: If all fails, exit with error
    log_error "=== Failed to install $package ==="
    log_error "Attempted methods:"
    log_error "  1. apt-get install"
    if [ -n "$source_url" ]; then
        log_error "  2. Build from source ($source_url)"
    fi
    exit 1
}

# Run main function
main "$@"
