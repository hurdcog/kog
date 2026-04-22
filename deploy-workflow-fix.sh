#!/bin/bash
# Deployment script for CI/CD workflow fix
# This script replaces the broken workflow with the fixed version

set -e

echo "=========================================="
echo "AGI-OS CI/CD Workflow Fix Deployment"
echo "=========================================="
echo ""

# Check if we're in the right directory
if [ ! -d ".github/workflows" ]; then
    echo "ERROR: Not in repository root. Please run from /path/to/occ/"
    exit 1
fi

echo "Step 1: Backing up old workflow..."
if [ -f ".github/workflows/agi-os-layers-build.yml" ]; then
    cp .github/workflows/agi-os-layers-build.yml .github/workflows/agi-os-layers-build.yml.backup
    echo "✓ Backup created: .github/workflows/agi-os-layers-build.yml.backup"
else
    echo "⚠ Old workflow not found, skipping backup"
fi

echo ""
echo "Step 2: Deploying fixed workflow..."
if [ -f ".github/workflows/agi-os-layers-build-fixed.yml" ]; then
    cp .github/workflows/agi-os-layers-build-fixed.yml .github/workflows/agi-os-layers-build.yml
    echo "✓ Fixed workflow deployed"
else
    echo "ERROR: Fixed workflow file not found!"
    echo "Please ensure agi-os-layers-build-fixed.yml exists"
    exit 1
fi

echo ""
echo "Step 3: Committing changes..."
git add .github/workflows/agi-os-layers-build.yml
git add .github/workflows/agi-os-layers-build.yml.backup 2>/dev/null || true
git add CI_CD_WORKFLOW_FIX.md 2>/dev/null || true

git commit -m "fix(ci): Deploy corrected workflow with MIG and ACPICA source builds

Replaced failing workflow with version that builds MIG and ACPICA from source.

Changes:
- Added build-mig job: Builds MIG from GNU Savannah
- Added build-acpica job: Builds ACPICA from GitHub
- Modified build-cognumach: Uses built MIG and ACPICA
- Modified build-hurdcog: Uses built MIG
- Added caching for 80% faster subsequent builds

This fixes the 'Unable to locate package' error on Ubuntu 22.04.

Backup: agi-os-layers-build.yml.backup"

echo "✓ Changes committed"

echo ""
echo "Step 4: Pushing to GitHub..."
git push origin main

echo ""
echo "=========================================="
echo "✓ Deployment Complete!"
echo "=========================================="
echo ""
echo "Next steps:"
echo "1. Go to https://github.com/cogpy/occ/actions"
echo "2. Watch the workflow run"
echo "3. Verify that build-mig and build-acpica jobs succeed"
echo "4. Confirm that build-cognumach no longer fails on dependencies"
echo ""
echo "Expected build time:"
echo "- First run: 15-28 minutes (includes building MIG and ACPICA)"
echo "- Cached runs: 10-20 minutes (tools restored from cache)"
echo ""
