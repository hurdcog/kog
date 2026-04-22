#!/bin/bash
# ci-build.sh - Continuous Integration build script for OpenCog packages
# Designed for GitHub Actions, GitLab CI, or other CI/CD systems

set -e

# CI Environment detection
if [ -n "$GITHUB_ACTIONS" ]; then
    CI_SYSTEM="GitHub Actions"
    CI_COMMIT="${GITHUB_SHA}"
    CI_BRANCH="${GITHUB_REF_NAME}"
elif [ -n "$GITLAB_CI" ]; then
    CI_SYSTEM="GitLab CI"
    CI_COMMIT="${CI_COMMIT_SHA}"
    CI_BRANCH="${CI_COMMIT_REF_NAME}"
else
    CI_SYSTEM="Local"
    CI_COMMIT=$(git rev-parse HEAD 2>/dev/null || echo "unknown")
    CI_BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "unknown")
fi

echo "=========================================="
echo "OpenCog CI/CD Build System"
echo "=========================================="
echo "CI System: $CI_SYSTEM"
echo "Commit: $CI_COMMIT"
echo "Branch: $CI_BRANCH"
echo "=========================================="
echo ""

# Install required build tools
echo "Installing build dependencies..."
sudo apt-get update -qq
sudo apt-get install -y -qq \
    build-essential \
    debhelper \
    cmake \
    devscripts \
    fakeroot \
    lintian \
    git \
    wget \
    curl

# Set build parameters for CI
export BUILD_DIR=$(pwd)
export PARALLEL_JOBS=$(nproc)
export INSTALL_PACKAGES="yes"
export BUILD_AGI_OS="no"  # Skip AGI-OS in CI by default

# Run the main build script
echo ""
echo "Starting package builds..."
echo ""

if ! bash build-all-packages.sh; then
    echo ""
    echo "Build failed!"
    exit 1
fi

echo ""
echo "=========================================="
echo "CI Build Completed Successfully"
echo "=========================================="

# Collect artifacts
ARTIFACT_DIR="artifacts"
mkdir -p "$ARTIFACT_DIR"

echo "Collecting build artifacts..."
find . -maxdepth 2 -name "*.deb" -exec cp {} "$ARTIFACT_DIR/" \;
find . -maxdepth 2 -name "*.dsc" -exec cp {} "$ARTIFACT_DIR/" \;
find . -maxdepth 2 -name "*.changes" -exec cp {} "$ARTIFACT_DIR/" \;

echo "Artifacts saved to: $ARTIFACT_DIR/"
ls -lh "$ARTIFACT_DIR/"

echo ""
echo "Build summary:"
echo "  Packages: $(ls -1 $ARTIFACT_DIR/*.deb 2>/dev/null | wc -l)"
echo "  Total size: $(du -sh $ARTIFACT_DIR | cut -f1)"
echo ""
