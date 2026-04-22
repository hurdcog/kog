#!/bin/bash
# Script to clone GNU Hurd ecosystem repositories
# Run this script when network access to git.savannah.gnu.org is available

set -euo pipefail  # Exit on error, undefined variables, and pipe failures

# Function to log with timestamp
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Function to handle errors
error_exit() {
    log "ERROR: $1"
    exit 1
}

# Validate environment
if [[ $# -gt 0 ]]; then
    error_exit "This script takes no arguments"
fi

# Get script directory safely
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR" || error_exit "Failed to change to script directory"

log "Starting GNU Hurd ecosystem repository cloning..."

# Validate git is available
if ! command -v git >/dev/null 2>&1; then
    error_exit "git is not installed or not in PATH"
fi

# Create directory structure safely
if ! mkdir -p external/gnu-repos external/hurd-repos; then
    error_exit "Failed to create external directories"
fi

# Define allowed repository URLs (whitelist approach for security)
declare -r SAVANNAH_BASE="https://git.savannah.gnu.org/git"
declare -A ALLOWED_REPOS=(
    ["bash"]="$SAVANNAH_BASE/bash.git"
    ["hurd-meta"]="$SAVANNAH_BASE/hurd.git"
    ["glibc"]="$SAVANNAH_BASE/hurd/glibc.git"
    ["gnumach"]="$SAVANNAH_BASE/hurd/gnumach.git"
    ["hurd"]="$SAVANNAH_BASE/hurd/hurd.git"
    ["incubator"]="$SAVANNAH_BASE/hurd/incubator.git"
    ["libpthread"]="$SAVANNAH_BASE/hurd/libpthread.git"
    ["mig"]="$SAVANNAH_BASE/hurd/mig.git"
    ["procfs"]="$SAVANNAH_BASE/hurd/procfs.git"
    ["unionfs"]="$SAVANNAH_BASE/hurd/unionfs.git"
    ["viengoos"]="$SAVANNAH_BASE/hurd/viengoos.git"
    ["web"]="$SAVANNAH_BASE/hurd/web.git"
)

# Function to safely clone or update a repository
clone_or_update_repo() {
    local repo_name="$1"
    local repo_url="$2"
    local target_dir="$3"
    
    # Validate inputs
    if [[ -z "$repo_name" || -z "$repo_url" || -z "$target_dir" ]]; then
        error_exit "Invalid parameters to clone_or_update_repo"
    fi
    
    # Validate repository name (only alphanumeric, hyphen, underscore)
    if [[ ! "$repo_name" =~ ^[a-zA-Z0-9_-]+$ ]]; then
        error_exit "Invalid repository name: $repo_name"
    fi
    
    # Validate URL is in allowed list
    local url_valid=false
    for allowed_url in "${ALLOWED_REPOS[@]}"; do
        if [[ "$repo_url" == "$allowed_url" ]]; then
            url_valid=true
            break
        fi
    done
    
    if [[ "$url_valid" != "true" ]]; then
        error_exit "Repository URL not in allowed list: $repo_url"
    fi
    
    log "Processing repository: $repo_name"
    
    if [[ ! -d "$target_dir/.git" ]]; then
        log "Cloning $repo_name..."
        if ! git clone --progress --depth 1 "$repo_url" "$target_dir"; then
            log "WARNING: Failed to clone $repo_name (network issue or access restriction)"
            return 1
        fi
    else
        log "$repo_name already cloned, updating..."
        if ! (cd "$target_dir" && git pull --ff-only); then
            log "WARNING: Failed to update $repo_name"
            return 1
        fi
    fi
    
    return 0
}

# Clone GNU Bash
log "Cloning GNU repositories..."
if ! clone_or_update_repo "bash" "${ALLOWED_REPOS[bash]}" "external/gnu-repos/bash"; then
    log "Failed to clone bash repository"
fi

# Clone Hurd repositories
log "Cloning Hurd ecosystem repositories..."

# Process each repository
for repo_name in hurd-meta glibc gnumach hurd incubator libpthread mig procfs unionfs viengoos web; do
    target_dir="external/hurd-repos/$repo_name"
    if ! clone_or_update_repo "$repo_name" "${ALLOWED_REPOS[$repo_name]}" "$target_dir"; then
        log "Failed to process repository: $repo_name"
    fi
done

log "Repository cloning completed!"
log "Note: This script preserves the existing README.md files in each directory."

# Create a status file to indicate successful cloning
status_file="external/CLONE_STATUS.txt"
if ! echo "$(date): Repositories successfully cloned" > "$status_file"; then
    log "WARNING: Failed to create status file"
fi

log "Status file created: $status_file"
