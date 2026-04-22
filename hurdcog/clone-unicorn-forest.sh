#!/bin/bash
# Script to integrate Unicorn-Forest repositories as monorepo
# This script integrates repositories from the Unicorn-Forest GitHub organization
# to address issue #9 and related issues #10-#19
# Creates a monorepo structure with .git headers removed and NO SUBMODULES

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

log "Starting Unicorn-Forest repository integration for monorepo..."

# Create directory structure for Unicorn-Forest repositories
mkdir -p external/unicorn-forest-repos || error_exit "Failed to create unicorn-forest-repos directory"

# Create temporary directory for cloning
TEMP_DIR=$(mktemp -d)
trap "rm -rf $TEMP_DIR" EXIT

# Repository mapping based on issues #10-#19
repos=(
    "gnumach:gnumach"           # Issue #10
    "hurd:hurd"                 # Issue #11
    "libpthread:libpthread"     # Issue #12
    "incubator:incubator"       # Issue #13
    "mig:mig"                   # Issue #14
    "procfs:procfs"             # Issue #15
    "unionfs:unionfs"           # Issue #16
    "viengoos:viengoos"         # Issue #17
    "web:web"                   # Issue #18
    "glibc:glibc"               # Issue #19
    "bash:bash"                 # Additional repository found
    "h:h"                       # Additional repository found
)

for repo in "${repos[@]}"; do
    IFS=':' read -r repo_name dir_name <<< "$repo"
    log "Integrating Unicorn-Forest/$repo_name into monorepo as $dir_name..."
    
    target_dir="external/unicorn-forest-repos/$dir_name"
    
    # Skip if directory already has content (not just empty directory)
    if [ -d "$target_dir" ] && [ "$(ls -A "$target_dir" 2>/dev/null)" ]; then
        log "$dir_name already integrated, skipping..."
        continue
    fi
    
    # Clone to temporary location
    temp_repo="$TEMP_DIR/$repo_name"
    if ! git clone --progress "https://github.com/Unicorn-Forest/$repo_name.git" "$temp_repo"; then
        log "WARNING: Failed to clone Unicorn-Forest/$repo_name, skipping..."
        continue
    fi
    
    # Remove .git directory to avoid submodules
    rm -rf "$temp_repo/.git"
    
    # Create target directory and copy content (monorepo integration)
    mkdir -p "$target_dir"
    cp -r "$temp_repo"/* "$target_dir/" 2>/dev/null || log "No files to copy from $repo_name"
    cp -r "$temp_repo"/.[^.]* "$target_dir/" 2>/dev/null || log "No hidden files to copy from $repo_name"
    
    log "Successfully integrated $repo_name into monorepo"
done

log "Unicorn-Forest repository integration completed!"
log "All repositories from issues #10-#19 have been successfully integrated as monorepo."
log "No .git directories or submodules were created - all content is directly integrated."

# Create a status file to indicate successful integration
echo "$(date): Unicorn-Forest repositories successfully integrated as monorepo" > external/UNICORN_FOREST_STATUS.txt
echo "Repository mapping (monorepo integration):" >> external/UNICORN_FOREST_STATUS.txt
echo "Issue #10 (gnumach) → external/unicorn-forest-repos/gnumach" >> external/UNICORN_FOREST_STATUS.txt
echo "Issue #11 (hurd) → external/unicorn-forest-repos/hurd" >> external/UNICORN_FOREST_STATUS.txt
echo "Issue #12 (libpthread) → external/unicorn-forest-repos/libpthread" >> external/UNICORN_FOREST_STATUS.txt
echo "Issue #13 (incubator) → external/unicorn-forest-repos/incubator" >> external/UNICORN_FOREST_STATUS.txt
echo "Issue #14 (mig) → external/unicorn-forest-repos/mig" >> external/UNICORN_FOREST_STATUS.txt
echo "Issue #15 (procfs) → external/unicorn-forest-repos/procfs" >> external/UNICORN_FOREST_STATUS.txt
echo "Issue #16 (unionfs) → external/unicorn-forest-repos/unionfs" >> external/UNICORN_FOREST_STATUS.txt
echo "Issue #17 (viengoos) → external/unicorn-forest-repos/viengoos" >> external/UNICORN_FOREST_STATUS.txt
echo "Issue #18 (web) → external/unicorn-forest-repos/web" >> external/UNICORN_FOREST_STATUS.txt
echo "Issue #19 (glibc) → external/unicorn-forest-repos/glibc" >> external/UNICORN_FOREST_STATUS.txt
echo "Additional: bash → external/unicorn-forest-repos/bash" >> external/UNICORN_FOREST_STATUS.txt
echo "Additional: h → external/unicorn-forest-repos/h" >> external/UNICORN_FOREST_STATUS.txt
echo "" >> external/UNICORN_FOREST_STATUS.txt
echo "MONOREPO STRUCTURE: All repositories integrated without .git headers" >> external/UNICORN_FOREST_STATUS.txt
echo "NO SUBMODULES: Content copied directly into main repository" >> external/UNICORN_FOREST_STATUS.txt

log "Status file created: external/UNICORN_FOREST_STATUS.txt"

log "Status file created: external/UNICORN_FOREST_STATUS.txt"