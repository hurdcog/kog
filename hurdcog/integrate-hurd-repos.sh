#!/bin/bash
# Script to clone GNU Hurd ecosystem repositories and integrate them into the monorepo
# This removes .git directories to fully integrate the code

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Function to log with timestamp
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Function to handle errors
error_exit() {
    log "ERROR: $1"
    exit 1
}

# Function to clone and integrate a repository
integrate_repo() {
    local repo_url="$1"
    local target_dir="$2"
    local temp_name="$3"
    
    log "Integrating $repo_url into $target_dir"
    
    # Create temporary directory
    temp_dir="/tmp/hurd-integration-$$"
    mkdir -p "$temp_dir"
    
    # Clone to temporary location
    log "Cloning $repo_url to temporary location..."
    if ! git clone --progress "$repo_url" "$temp_dir/$temp_name"; then
        rm -rf "$temp_dir"
        error_exit "Failed to clone $repo_url"
    fi
    
    # Remove .git directory
    log "Removing .git directory from $temp_name..."
    rm -rf "$temp_dir/$temp_name/.git"
    
    # Ensure target directory exists
    mkdir -p "$(dirname "$target_dir")"
    
    # Move content to target location
    log "Moving content to $target_dir..."
    if [ -d "$target_dir" ]; then
        log "Warning: $target_dir already exists, merging content..."
        cp -r "$temp_dir/$temp_name/"* "$target_dir/" || error_exit "Failed to merge content into $target_dir"
    else
        mv "$temp_dir/$temp_name" "$target_dir" || error_exit "Failed to move $temp_name to $target_dir"
    fi
    
    # Clean up temporary directory
    rm -rf "$temp_dir"
    
    log "Successfully integrated $temp_name into $target_dir"
}

log "Starting GNU Hurd ecosystem integration..."

# Define repositories and their target locations
declare -A repos=(
    ["https://git.savannah.gnu.org/git/hurd/gnumach.git"]="hurd-ecosystem/kernel/gnumach"
    ["https://git.savannah.gnu.org/git/hurd/mig.git"]="hurd-ecosystem/tools/mig"
    ["https://git.savannah.gnu.org/git/hurd/incubator.git"]="hurd-ecosystem/experimental/incubator"
    ["https://git.savannah.gnu.org/git/hurd/libpthread.git"]="hurd-ecosystem/libraries/libpthread"
    ["https://git.savannah.gnu.org/git/hurd/unionfs.git"]="hurd-ecosystem/servers/unionfs"
    ["https://git.savannah.gnu.org/git/hurd/viengoos.git"]="hurd-ecosystem/experimental/viengoos"
    ["https://git.savannah.gnu.org/git/hurd/web.git"]="hurd-ecosystem/documentation/web"
    ["https://git.savannah.gnu.org/git/hurd/procfs.git"]="hurd-ecosystem/servers/procfs-upstream"
)

# Process each repository
for repo_url in "${!repos[@]}"; do
    target_dir="${repos[$repo_url]}"
    temp_name=$(basename "$repo_url" .git)
    
    integrate_repo "$repo_url" "$target_dir" "$temp_name"
done

log "Integration completed for standard repositories!"
log "Note: hurd.git and glibc.git will be handled separately due to size and complexity"

# Create a status file
echo "$(date): Hurd ecosystem repositories integrated (excluding hurd.git and glibc.git)" > hurd-ecosystem/INTEGRATION_STATUS.txt
log "Status file created: hurd-ecosystem/INTEGRATION_STATUS.txt"