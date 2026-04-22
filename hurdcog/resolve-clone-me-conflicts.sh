#!/bin/bash
# Script to automatically resolve merge conflicts between master and clone-me branches
# This script implements the recommended resolution strategy for known conflicts

set -euo pipefail

# Function to log with timestamp
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Function to handle errors
error_exit() {
    log "ERROR: $1"
    exit 1
}

# Validate we're in a git repository
if ! git rev-parse --git-dir >/dev/null 2>&1; then
    error_exit "Not in a git repository"
fi

# Check if we're in the middle of a merge
if ! git ls-files -u >/dev/null 2>&1 || [ ! -f .git/MERGE_HEAD ]; then
    error_exit "No merge in progress. Start the merge first with: git merge master --allow-unrelated-histories"
fi

log "Starting automated conflict resolution for clone-me merge..."

# Get list of conflicted files
conflicted_files=$(git diff --name-only --diff-filter=U)

if [ -z "$conflicted_files" ]; then
    log "No conflicts found. Merge may already be resolved."
    exit 0
fi

log "Found conflicted files:"
echo "$conflicted_files"

# Function to resolve conflicts by keeping the clone-me version (HEAD)
resolve_keep_head() {
    local file="$1"
    log "Resolving $file - keeping clone-me version"
    git checkout --ours "$file"
    git add "$file"
}

# Function to resolve conflicts by keeping the master version
resolve_keep_master() {
    local file="$1"
    log "Resolving $file - keeping master version"
    git checkout --theirs "$file"
    git add "$file"
}

# Function to manually resolve complex conflicts
resolve_manual() {
    local file="$1"
    log "Manual resolution required for $file"
    
    case "$file" in
        "Makefile")
            # For Makefile, try to merge both versions intelligently
            log "Attempting intelligent merge of Makefile..."
            
            # Create a temporary file with the merged content
            {
                echo "# Merged Makefile - combining both versions"
                echo "# Base configuration from master branch"
                git show :3:"$file" 2>/dev/null || true
                echo ""
                echo "# Additional configuration from clone-me branch"
                git show :2:"$file" 2>/dev/null || true
            } > "${file}.merged"
            
            # Remove conflict markers and duplicate entries
            sed -e '/^<<<<<<<\|^======\|^>>>>>>>/d' \
                -e '/^$/N;/^\n$/d' \
                "${file}.merged" > "$file"
            
            rm -f "${file}.merged"
            git add "$file"
            ;;
            
        "external/README.md")
            # For documentation, combine both versions
            log "Merging documentation from both branches..."
            
            {
                echo "# External Repositories"
                echo ""
                echo "This directory contains external repository clones and related documentation."
                echo ""
                echo "## From Master Branch"
                git show :3:"$file" 2>/dev/null | tail -n +2 || true
                echo ""
                echo "## From Clone-Me Branch"
                git show :2:"$file" 2>/dev/null | tail -n +2 || true
            } > "${file}.merged"
            
            # Clean up and remove duplicates
            sed -e '/^<<<<<<<\|^======\|^>>>>>>>/d' \
                -e '/^$/N;/^\n$/d' \
                "${file}.merged" > "$file"
            
            rm -f "${file}.merged"
            git add "$file"
            ;;
            
        *)
            log "Unknown file for manual resolution: $file"
            log "Please resolve manually and run: git add $file"
            return 1
            ;;
    esac
}

# Resolve conflicts based on file type and location
for file in $conflicted_files; do
    case "$file" in
        ".github/scripts/"*.py)
            # Keep clone-me version for Python scripts (new automation)
            resolve_keep_head "$file"
            ;;
            
        ".github/workflows/"*.yml)
            # Keep clone-me version for GitHub workflows (new functionality)
            resolve_keep_head "$file"
            ;;
            
        ".github/scripts/requirements.txt")
            # Keep clone-me version for Python requirements
            resolve_keep_head "$file"
            ;;
            
        "clone-repos.sh")
            # Keep clone-me version (enhanced security and features)
            resolve_keep_head "$file"
            ;;
            
        "Makefile"|"external/README.md")
            # These need manual/intelligent resolution
            resolve_manual "$file" || {
                log "Manual resolution failed for $file"
                log "Please resolve manually and run: git add $file"
                continue
            }
            ;;
            
        *)
            log "Unknown conflict file: $file"
            log "Defaulting to keep clone-me version"
            resolve_keep_head "$file"
            ;;
    esac
done

# Check if all conflicts are resolved
remaining_conflicts=$(git diff --name-only --diff-filter=U)

if [ -n "$remaining_conflicts" ]; then
    log "WARNING: Some conflicts still need manual resolution:"
    echo "$remaining_conflicts"
    log ""
    log "After resolving manually, run:"
    log "  git add <filename>"
    log "  git commit"
    exit 1
fi

log "All conflicts resolved successfully!"
log ""
log "Next steps:"
log "1. Review the changes: git diff --cached"
log "2. Commit the merge: git commit"
log "3. Push the changes: git push -u origin clone-me"
log ""
log "Recommended commit message:"
cat << 'EOF'
Merge master into clone-me - resolve conflicts

- Resolved conflicts in GitHub workflows and scripts
- Preserved automation enhancements from clone-me branch
- Merged documentation from both branches
- Updated build configuration

Fixes conflicts in:
- .github/scripts/ (Python automation scripts)
- .github/workflows/ (GitHub Actions)
- clone-repos.sh (enhanced repository cloning)
- Makefile (build configuration)
- external/README.md (documentation)
EOF