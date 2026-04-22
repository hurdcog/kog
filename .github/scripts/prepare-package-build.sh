#!/bin/bash
#
# Prepare Package for Debian Build
# This script prepares a package directory for dpkg-buildpackage
# by copying the debian/ directory from opencog-debian
#
# Usage: prepare-package-build.sh <component-name>
# Example: prepare-package-build.sh cogutil
#

set -e

COMPONENT=$1

if [ -z "$COMPONENT" ]; then
    echo "Error: Component name required"
    echo "Usage: $0 <component-name>"
    exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

echo "================================================"
echo "Preparing $COMPONENT for Debian package build"
echo "================================================"

# Parse package information from update script
UPDATE_SCRIPT="$REPO_ROOT/opencog-debian/$COMPONENT/update-$COMPONENT.sh"

if [ ! -f "$UPDATE_SCRIPT" ]; then
    echo "❌ Error: Update script not found: $UPDATE_SCRIPT"
    exit 1
fi

# Extract package metadata
echo "📋 Reading package metadata from $UPDATE_SCRIPT"
PARSE_OUTPUT=$("$SCRIPT_DIR/parse-package-info.sh" "$COMPONENT")
PARSE_EXIT_CODE=$?

if [ $PARSE_EXIT_CODE -ne 0 ]; then
    echo "❌ Error: Failed to parse package information"
    echo "$PARSE_OUTPUT"
    exit 1
fi

# Source the parsed variables
source <(echo "$PARSE_OUTPUT")

# Validate that required variables are set
if [ -z "$REPO_NAME" ] || [ -z "$DEB_NAME" ] || [ -z "$VERSION" ] || [ -z "$DEBIAN_DIR" ] || [ -z "$SOURCE_DIR" ]; then
    echo "❌ Error: Package information incomplete"
    echo "  REPO_NAME='$REPO_NAME'"
    echo "  DEB_NAME='$DEB_NAME'"
    echo "  VERSION='$VERSION'"
    echo "  DEBIAN_DIR='$DEBIAN_DIR'"
    echo "  SOURCE_DIR='$SOURCE_DIR'"
    exit 1
fi

echo "  REPO_NAME: $REPO_NAME"
echo "  DEB_NAME: $DEB_NAME"
echo "  VERSION: $VERSION"
echo "  DEBIAN_DIR: $DEBIAN_DIR"
echo "  SOURCE_DIR: $SOURCE_DIR"

# Verify source directory exists
if [ ! -d "$SOURCE_DIR" ]; then
    echo "❌ Error: Source directory not found: $SOURCE_DIR"
    echo "   Expected the $COMPONENT component directory to exist"
    exit 1
fi

# Verify debian directory exists
if [ ! -d "$DEBIAN_DIR" ]; then
    echo "❌ Error: Debian packaging directory not found: $DEBIAN_DIR"
    echo "   Expected opencog-debian/$COMPONENT/debian to exist"
    exit 1
fi

# Check if debian files are present
REQUIRED_FILES=(
    "$DEBIAN_DIR/control"
    "$DEBIAN_DIR/rules"
    "$DEBIAN_DIR/changelog"
    "$DEBIAN_DIR/compat"
    "$DEBIAN_DIR/copyright"
)

echo ""
echo "🔍 Verifying debian packaging files..."
ALL_FILES_PRESENT=true
for file in "${REQUIRED_FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "  ✅ $(basename "$file")"
    else
        echo "  ❌ $(basename "$file") - MISSING"
        ALL_FILES_PRESENT=false
    fi
done

if [ "$ALL_FILES_PRESENT" = false ]; then
    echo ""
    echo "❌ Error: Some required debian files are missing"
    exit 1
fi

# Copy debian directory to source directory
echo ""
echo "📦 Copying debian/ directory to $SOURCE_DIR"
if [ -d "$SOURCE_DIR/debian" ]; then
    echo "  ⚠️  Existing debian/ directory found, removing..."
    rm -rf "$SOURCE_DIR/debian"
fi

cp -r "$DEBIAN_DIR" "$SOURCE_DIR/"
echo "  ✅ Copied debian/ directory successfully"

# Make debian/rules executable if it isn't already
if [ -f "$SOURCE_DIR/debian/rules" ]; then
    chmod +x "$SOURCE_DIR/debian/rules"
    echo "  ✅ Made debian/rules executable"
fi

# Display control file summary
echo ""
echo "📄 Package Information from debian/control:"
if [ -f "$SOURCE_DIR/debian/control" ]; then
    echo "  Source: $(grep '^Source:' "$SOURCE_DIR/debian/control" | cut -d: -f2 | xargs)"
    echo "  Maintainer: $(grep '^Maintainer:' "$SOURCE_DIR/debian/control" | cut -d: -f2- | xargs)"
    PACKAGE_NAME=$(grep '^Package:' "$SOURCE_DIR/debian/control" | head -1 | cut -d: -f2 | xargs)
    echo "  Package: $PACKAGE_NAME"
fi

# Output summary
echo ""
echo "================================================"
echo "✅ Package preparation complete!"
echo "================================================"
echo ""
echo "Next steps:"
echo "  cd $SOURCE_DIR"
echo "  dpkg-buildpackage -us -uc -b"
echo ""
echo "Built packages will be available in:"
echo "  $(dirname "$SOURCE_DIR")/"
echo ""
