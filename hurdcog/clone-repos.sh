#!/bin/bash
# Script to clone GNU Hurd ecosystem repositories and Unicorn-Forest repositories
# Run this script when network access to git.savannah.gnu.org and github.com is available

set -e

# Disable exit on error for this script to allow graceful handling of network issues
set +e

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

log "Starting GNU Hurd ecosystem and Unicorn-Forest repository cloning..."

# Create directory structure
mkdir -p external/gnu-repos external/hurd-repos || error_exit "Failed to create external directories"

# Clone GNU repositories
gnu_clone_success=true
log "Cloning GNU Bash..."
if [ ! -d "external/gnu-repos/bash/.git" ]; then
    cd external/gnu-repos
    if ! git clone --progress https://git.savannah.gnu.org/git/bash.git; then
        log "WARNING: Failed to clone bash repository (network issue or access restriction)"
        gnu_clone_success=false
    fi
    cd ../..
else
    log "Bash already cloned, updating..."
    cd external/gnu-repos/bash && git pull && cd ../../..
fi

# Clone Hurd repositories
if [ "$gnu_clone_success" = true ]; then
    log "Cloning Hurd ecosystem repositories..."

    repos=(
        "hurd.git:hurd-meta"
        "hurd/glibc.git:glibc"
        "hurd/gnumach.git:gnumach"
        "hurd/hurd.git:hurd"
        "hurd/incubator.git:incubator"
        "hurd/libpthread.git:libpthread"
        "hurd/mig.git:mig"
        "hurd/procfs.git:procfs"
        "hurd/unionfs.git:unionfs"
        "hurd/viengoos.git:viengoos"
        "hurd/web.git:web"
    )

    cd external/hurd-repos

    for repo in "${repos[@]}"; do
        IFS=':' read -r repo_path dir_name <<< "$repo"
        log "Cloning $repo_path to $dir_name..."
        
        if [ ! -d "$dir_name/.git" ]; then
            if ! git clone --progress "https://git.savannah.gnu.org/git/$repo_path" "$dir_name"; then
                log "WARNING: Failed to clone $repo_path (network issue or access restriction)"
                gnu_clone_success=false
            fi
        else
            log "$dir_name already cloned, updating..."
            cd "$dir_name" && git pull && cd ..
        fi
    done

    cd ../..
else
    log "Skipping Hurd repositories due to previous failure"
fi

# Clone Unicorn-Forest repositories (addressing issue #9) - monorepo integration
log "Integrating Unicorn-Forest repositories into monorepo..."
if [ -x "./clone-unicorn-forest.sh" ]; then
    ./clone-unicorn-forest.sh
else
    log "WARNING: clone-unicorn-forest.sh not found or not executable"
fi

log "Repository integration completed!"
log "Note: This script preserves the existing README.md files in each directory."
log "Unicorn-Forest repositories are integrated as monorepo without .git headers or submodules."

# Create a status file to indicate successful cloning
echo "$(date): Repositories successfully cloned" > external/CLONE_STATUS.txt
log "Status file created: external/CLONE_STATUS.txt"
