#!/bin/bash
#
# Sync script to push changes from o9nn/occ to cogpy/occ
# Usage: ./scripts/sync-to-cogpy.sh [branch]
#
# This script:
# 1. Checks current repository status
# 2. Ensures we're on the correct branch
# 3. Pushes changes to cogpy/occ repository
# 4. Syncs tags
#
# Requirements:
# - GIT_PAT environment variable must be set
# - Must be run from repository root
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
SOURCE_REPO="o9nn/occ"
TARGET_REPO="cogpy/occ"
BRANCH="${1:-main}"

echo -e "${CYAN}=== OCC Repository Sync Script ===${NC}"
echo ""

# Check if GIT_PAT is set
if [ -z "$GIT_PAT" ]; then
    echo -e "${RED}Error: GIT_PAT environment variable is not set${NC}"
    echo "Please set your GitHub Personal Access Token:"
    echo "  export GIT_PAT=your_token_here"
    exit 1
fi

# Check if we're in the repository root
if [ ! -d ".git" ]; then
    echo -e "${RED}Error: Must be run from repository root${NC}"
    exit 1
fi

# Check current branch
CURRENT_BRANCH=$(git branch --show-current)
echo -e "${CYAN}Current branch: ${YELLOW}$CURRENT_BRANCH${NC}"

if [ "$CURRENT_BRANCH" != "$BRANCH" ]; then
    echo -e "${YELLOW}Warning: Not on $BRANCH branch${NC}"
    read -p "Continue anyway? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Check for uncommitted changes
if ! git diff-index --quiet HEAD --; then
    echo -e "${RED}Error: You have uncommitted changes${NC}"
    echo "Please commit or stash your changes before syncing"
    git status --short
    exit 1
fi

# Add target remote if it doesn't exist
if ! git remote | grep -q "^cogpy$"; then
    echo -e "${CYAN}Adding cogpy remote...${NC}"
    git remote add cogpy "https://${GIT_PAT}@github.com/${TARGET_REPO}.git"
else
    echo -e "${CYAN}Updating cogpy remote URL...${NC}"
    git remote set-url cogpy "https://${GIT_PAT}@github.com/${TARGET_REPO}.git"
fi

# Fetch from target
echo -e "${CYAN}Fetching from cogpy/occ...${NC}"
git fetch cogpy "$BRANCH"

# Check for differences
echo -e "${CYAN}Checking for differences...${NC}"
if git diff --quiet HEAD "cogpy/$BRANCH" 2>/dev/null; then
    echo -e "${GREEN}✓ Repositories are already in sync${NC}"
    exit 0
fi

# Show summary of changes
echo -e "${YELLOW}Changes to be synced:${NC}"
git log --oneline "cogpy/$BRANCH..HEAD" | head -10

# Confirm sync
echo ""
read -p "Push these changes to cogpy/occ? (y/N) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo -e "${YELLOW}Sync cancelled${NC}"
    exit 0
fi

# Push to target
echo -e "${CYAN}Pushing to cogpy/occ...${NC}"
if git push cogpy "$BRANCH:$BRANCH" --force-with-lease; then
    echo -e "${GREEN}✓ Successfully pushed to cogpy/occ${NC}"
else
    echo -e "${RED}✗ Failed to push to cogpy/occ${NC}"
    exit 1
fi

# Sync tags
echo -e "${CYAN}Syncing tags...${NC}"
if git push cogpy --tags --force; then
    echo -e "${GREEN}✓ Tags synced successfully${NC}"
else
    echo -e "${YELLOW}⚠ Warning: Failed to sync tags${NC}"
fi

echo ""
echo -e "${GREEN}=== Sync Complete ===${NC}"
echo -e "Source: ${CYAN}$SOURCE_REPO${NC}"
echo -e "Target: ${CYAN}$TARGET_REPO${NC}"
echo -e "Branch: ${CYAN}$BRANCH${NC}"
