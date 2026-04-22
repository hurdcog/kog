#!/bin/bash
#
# Update script for opencog-atomspace-pgres Debian package
# This script prepares the source package for building
#

set -e

PACKAGE_NAME="atomspace-pgres"
DEBIAN_PACKAGE_NAME="opencog-atomspace-pgres"
SOURCE_DIR="../${PACKAGE_NAME}"
VERSION=$(grep -m1 "^${DEBIAN_PACKAGE_NAME}" debian/changelog | sed 's/.*(\(.*\)).*/\1/' | cut -d'-' -f1)

echo "=========================================="
echo "Updating ${DEBIAN_PACKAGE_NAME} package"
echo "Version: ${VERSION}"
echo "=========================================="

# Check if source directory exists
if [ ! -d "${SOURCE_DIR}" ]; then
    echo "Error: Source directory ${SOURCE_DIR} not found"
    echo "Please ensure the atomspace-pgres repository is cloned in the parent directory"
    exit 1
fi

# Create the source tarball
TARBALL="${DEBIAN_PACKAGE_NAME}_${VERSION}.orig.tar.gz"
echo "Creating source tarball: ${TARBALL}"

cd ..
tar czf "${TARBALL}" \
    --exclude=".git" \
    --exclude="debian" \
    --exclude="*.swp" \
    --exclude="*~" \
    --exclude="build" \
    --exclude="*.o" \
    --exclude="*.so" \
    "${PACKAGE_NAME}"

# Extract to build directory
BUILD_DIR="${DEBIAN_PACKAGE_NAME}-${VERSION}"
echo "Extracting to build directory: ${BUILD_DIR}"

rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"
tar xzf "${TARBALL}" -C "${BUILD_DIR}" --strip-components=1

# Copy debian directory
echo "Copying debian/ directory"
cp -r "opencog-debian/${PACKAGE_NAME}/debian" "${BUILD_DIR}/"

echo "=========================================="
echo "Package preparation complete!"
echo "=========================================="
echo ""
echo "To build the package, run:"
echo "  cd ${BUILD_DIR}"
echo "  sudo apt-get build-dep ."
echo "  dpkg-buildpackage -rfakeroot -us -uc"
echo ""
