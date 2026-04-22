#!/bin/bash
# Update script for kogboldai-kernel package

set -e

PACKAGE_NAME="kogboldai-kernel"
VERSION="1.0.0"
REPO_URL="https://github.com/cogpy/KogboldAI.git"
BRANCH="united"

echo "========================================="
echo "Updating $PACKAGE_NAME package"
echo "========================================="

# Clone the repository if not already present
if [ ! -d "KogboldAI" ]; then
    echo "Cloning KogboldAI repository..."
    git clone --depth 1 --branch "$BRANCH" "$REPO_URL" KogboldAI
else
    echo "Updating existing KogboldAI repository..."
    cd KogboldAI
    git fetch origin "$BRANCH"
    git checkout "$BRANCH"
    git pull
    cd ..
fi

# Create source directory
SOURCE_DIR="${PACKAGE_NAME}-${VERSION}"
rm -rf "$SOURCE_DIR"
mkdir -p "$SOURCE_DIR"

# Copy kernel directory
echo "Copying kernel source files..."
cp -r KogboldAI/kernel/* "$SOURCE_DIR/"

# Copy debian directory
echo "Copying debian packaging files..."
cp -r debian "$SOURCE_DIR/"

# Create orig tarball
echo "Creating source tarball..."
tar czf "${PACKAGE_NAME}_${VERSION}.orig.tar.gz" "$SOURCE_DIR"

echo "========================================="
echo "Package: $PACKAGE_NAME"
echo "Version: $VERSION"
echo "Source: $SOURCE_DIR"
echo "Tarball: ${PACKAGE_NAME}_${VERSION}.orig.tar.gz"
echo "========================================="
