#!/bin/bash
# Script to update vcpkg in the monorepo
# This script clones the latest vcpkg, removes .git, and replaces the existing vcpkg folder

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VCPKG_DIR="$SCRIPT_DIR/vcpkg"
TEMP_DIR="$SCRIPT_DIR/vcpkg-update-tmp"

echo "=== vcpkg Update Script ==="
echo "Current directory: $SCRIPT_DIR"
echo "Target vcpkg directory: $VCPKG_DIR"
echo ""

# Check if vcpkg directory exists
if [ ! -d "$VCPKG_DIR" ]; then
    echo "ERROR: vcpkg directory not found at $VCPKG_DIR"
    exit 1
fi

# Get current vcpkg version info
echo "Current vcpkg version:"
if [ -f "$VCPKG_DIR/README.md" ]; then
    head -5 "$VCPKG_DIR/README.md"
else
    echo "  (version info not available)"
fi
echo ""

# Confirm update
read -p "Do you want to update vcpkg? This will replace the existing vcpkg folder. (y/N): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Update cancelled."
    exit 0
fi

# Clone latest vcpkg
echo "=== Cloning latest vcpkg ==="
rm -rf "$TEMP_DIR"
git clone --depth 1 https://github.com/microsoft/vcpkg.git "$TEMP_DIR"

# Get commit hash before removing .git
cd "$TEMP_DIR"
VCPKG_COMMIT=$(git rev-parse HEAD)
VCPKG_DATE=$(git log -1 --format=%cd --date=short)
echo "Cloned vcpkg commit: $VCPKG_COMMIT"
echo "Commit date: $VCPKG_DATE"
echo ""

# Remove .git directory
echo "=== Removing .git directory ==="
rm -rf .git
echo "vcpkg is now a regular folder (no git history)"
echo ""

# Backup old vcpkg
echo "=== Backing up old vcpkg ==="
BACKUP_DIR="$SCRIPT_DIR/vcpkg-backup-$(date +%Y%m%d-%H%M%S)"
mv "$VCPKG_DIR" "$BACKUP_DIR"
echo "Old vcpkg backed up to: $BACKUP_DIR"
echo ""

# Move new vcpkg into place
echo "=== Installing new vcpkg ==="
mv "$TEMP_DIR" "$VCPKG_DIR"
echo "New vcpkg installed successfully"
echo ""

# Create version info file
echo "=== Creating version info ==="
cat > "$VCPKG_DIR/VCPKG_VERSION.txt" << EOF
vcpkg Version Information
=========================

Commit: $VCPKG_COMMIT
Date: $VCPKG_DATE
Updated: $(date)
Updated by: $(git config user.name) <$(git config user.email)>

This vcpkg installation is part of the OCC monorepo and does not
contain git history. To update vcpkg, run this script again:
  ./third_party/update-vcpkg.sh

Source: https://github.com/microsoft/vcpkg
EOF
echo "Version info saved to: $VCPKG_DIR/VCPKG_VERSION.txt"
echo ""

# Show new version
echo "=== New vcpkg version ==="
cat "$VCPKG_DIR/VCPKG_VERSION.txt"
echo ""

# Instructions
echo "=== Next Steps ==="
echo "1. Test the new vcpkg version:"
echo "   cd $VCPKG_DIR"
echo "   ./bootstrap-vcpkg.sh"
echo "   ./vcpkg install boost-system"
echo ""
echo "2. If everything works, commit the changes:"
echo "   cd $(git rev-parse --show-toplevel)"
echo "   git add third_party/vcpkg"
echo "   git commit -m \"Update vcpkg to $VCPKG_COMMIT ($VCPKG_DATE)\""
echo "   git push origin main"
echo ""
echo "3. If there are issues, restore the backup:"
echo "   rm -rf $VCPKG_DIR"
echo "   mv $BACKUP_DIR $VCPKG_DIR"
echo ""
echo "Update complete!"
