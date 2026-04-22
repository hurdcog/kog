#!/bin/bash
#
# Update atomspace-storage package from upstream repository
#

set -e

PACKAGE_NAME="atomspace-storage"
REPO_NAME=atomspace-storage
DEB_NAME=opencog-atomspace-storage
UPSTREAM_REPO="../../${PACKAGE_NAME}"
VERSION="1.0.0"

echo "=========================================="
echo "Updating ${PACKAGE_NAME} package"
echo "=========================================="

# Check if upstream directory exists
if [ ! -d "${UPSTREAM_REPO}" ]; then
    echo "ERROR: Upstream repository not found at ${UPSTREAM_REPO}"
    exit 1
fi

# Clean up old builds
rm -rf ${PACKAGE_NAME}-*

# Create package directory
PACKAGE_DIR="${PACKAGE_NAME}-${VERSION}"
mkdir -p ${PACKAGE_DIR}

# Copy source files
echo "Copying source files from ${UPSTREAM_REPO}..."
cp -r ${UPSTREAM_REPO}/* ${PACKAGE_DIR}/

# Copy debian directory
echo "Copying debian packaging files..."
cp -r debian ${PACKAGE_DIR}/

# Clean up git files if present
rm -rf ${PACKAGE_DIR}/.git

echo "=========================================="
echo "${PACKAGE_NAME} package updated successfully"
echo "Package directory: ${PACKAGE_DIR}"
echo "=========================================="
echo ""
echo "Next steps:"
echo "  cd ${PACKAGE_DIR}"
echo "  dpkg-buildpackage -rfakeroot -us -uc"
echo "  sudo dpkg -i ../*.deb"
