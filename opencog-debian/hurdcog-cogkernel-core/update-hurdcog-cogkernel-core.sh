#!/bin/bash
set -e

PACKAGE_NAME="hurdcog-cogkernel-core"
VERSION="1.0.0"
HURDCOG_ROOT="../../hurdcog"

echo "Preparing $PACKAGE_NAME source package..."

# Clean previous builds
rm -rf ${PACKAGE_NAME}-${VERSION}
rm -f ${PACKAGE_NAME}_${VERSION}.orig.tar.gz

# Create source directory
mkdir -p ${PACKAGE_NAME}-${VERSION}

# Copy core bootstrap files
echo "  Copying core bootstrap files..."
cp ${HURDCOG_ROOT}/cogkernel/hurdcog-bootstrap.scm ${PACKAGE_NAME}-${VERSION}/
cp ${HURDCOG_ROOT}/cogkernel/microkernel-integration.scm ${PACKAGE_NAME}-${VERSION}/
cp ${HURDCOG_ROOT}/cogkernel/namespace-minimal.scm ${PACKAGE_NAME}-${VERSION}/
cp ${HURDCOG_ROOT}/cogkernel/plan9-namespace.scm ${PACKAGE_NAME}-${VERSION}/
cp ${HURDCOG_ROOT}/cogkernel/scheme-adapters.scm ${PACKAGE_NAME}-${VERSION}/

# Copy cognitive interface
echo "  Copying cognitive interface..."
cp -r ${HURDCOG_ROOT}/cogkernel/cognitive-interface ${PACKAGE_NAME}-${VERSION}/

# Copy phase integration modules
echo "  Copying phase integration modules..."
cp ${HURDCOG_ROOT}/cogkernel/phase1-integration.scm ${PACKAGE_NAME}-${VERSION}/
cp ${HURDCOG_ROOT}/cogkernel/phase2-integration.scm ${PACKAGE_NAME}-${VERSION}/
cp ${HURDCOG_ROOT}/cogkernel/phase3-full-integration.scm ${PACKAGE_NAME}-${VERSION}/

# Copy examples
echo "  Copying examples..."
cp -r ${HURDCOG_ROOT}/cogkernel/examples ${PACKAGE_NAME}-${VERSION}/ || mkdir -p ${PACKAGE_NAME}-${VERSION}/examples

# Copy documentation
echo "  Copying documentation..."
cp ${HURDCOG_ROOT}/cogkernel/README.md ${PACKAGE_NAME}-${VERSION}/
cp -r ${HURDCOG_ROOT}/cogkernel/docs ${PACKAGE_NAME}-${VERSION}/ || mkdir -p ${PACKAGE_NAME}-${VERSION}/docs

# Copy debian directory
echo "  Copying debian packaging files..."
cp -r debian ${PACKAGE_NAME}-${VERSION}/

# Create source tarball
echo "  Creating source tarball..."
tar czf ${PACKAGE_NAME}_${VERSION}.orig.tar.gz ${PACKAGE_NAME}-${VERSION}

echo "✓ Source package prepared: ${PACKAGE_NAME}_${VERSION}.orig.tar.gz"
echo ""
echo "To build the package:"
echo "  cd ${PACKAGE_NAME}-${VERSION}"
echo "  dpkg-buildpackage -rfakeroot -us -uc"
