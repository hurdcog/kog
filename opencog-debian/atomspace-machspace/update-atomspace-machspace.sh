#!/bin/bash
# Update atomspace-machspace Debian package
# This creates a package for the AtomSpace-MachSpace storage backend

set -e

PACKAGE_NAME="atomspace-machspace"
VERSION="1.0.0"
DEBIAN_REVISION="1"

echo "=========================================="
echo "Updating $PACKAGE_NAME package"
echo "=========================================="

# Create package directory structure
mkdir -p ${PACKAGE_NAME}-${VERSION}
cd ${PACKAGE_NAME}-${VERSION}

# Copy source files from integration directory
echo "Copying source files..."
mkdir -p src
cp ../../integration/atomspace-machspace/*.h src/
cp ../../integration/atomspace-machspace/*.cc src/
cp ../../integration/atomspace-machspace/CMakeLists.txt src/

# Create debian directory
mkdir -p debian

# Create debian/control
cat > debian/control << 'EOF'
Source: atomspace-machspace
Section: devel
Priority: optional
Maintainer: OpenCog Developers <opencog@googlegroups.com>
Build-Depends: debhelper (>= 11),
               cmake (>= 3.12),
               build-essential,
               libcogutil-dev (>= 2.0.3),
               opencog-atomspace (>= 5.0.3),
               hurdcog-dev,
               libboost-dev (>= 1.65),
               libboost-system-dev,
               libboost-thread-dev,
               libboost-filesystem-dev,
               pkg-config
Standards-Version: 4.6.2
Homepage: https://github.com/cogpy/occ
Vcs-Git: https://github.com/cogpy/occ.git
Vcs-Browser: https://github.com/cogpy/occ

Package: atomspace-machspace
Architecture: any
Depends: ${shlibs:Depends},
         ${misc:Depends},
         opencog-atomspace (>= 5.0.3),
         libcogutil-dev (>= 2.0.3),
         hurdcog
Description: AtomSpace storage backend using HurdCog MachSpace
 This package provides a StorageNode implementation that allows AtomSpace
 to use HurdCog's MachSpace as a distributed storage backend.
 .
 MachSpace provides kernel-level distributed hypergraph storage with:
  - Shared memory atom access across processes
  - Copy-on-write atom management
  - Efficient IPC-based communication
  - Attention value synchronization with cognitive scheduler
  - Zero-copy atom transfer for performance
 .
 This enables seamless integration between OpenCog's AtomSpace and
 HurdCog's cognitive operating system layer.

Package: atomspace-machspace-dev
Architecture: any
Depends: ${misc:Depends},
         atomspace-machspace (= ${binary:Version}),
         opencog-atomspace (>= 5.0.3),
         libcogutil-dev (>= 2.0.3),
         hurdcog-dev
Description: Development files for AtomSpace-MachSpace backend
 This package contains header files and development libraries for
 building applications that use the AtomSpace-MachSpace storage backend.
EOF

# Create debian/rules
cat > debian/rules << 'EOF'
#!/usr/bin/make -f

%:
	dh $@ --buildsystem=cmake

override_dh_auto_configure:
	dh_auto_configure -- \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_INSTALL_PREFIX=/usr

override_dh_auto_test:
	# Skip tests for now
	true
EOF
chmod +x debian/rules

# Create debian/changelog
cat > debian/changelog << EOF
atomspace-machspace (${VERSION}-${DEBIAN_REVISION}) unstable; urgency=medium

  * Initial release of AtomSpace-MachSpace storage backend
  * Provides distributed hypergraph storage via HurdCog MachSpace
  * Implements AtomSpace StorageNode interface
  * Enables kernel-level atom access and synchronization
  * Integrates with cognitive scheduler for attention-aware operations

 -- OpenCog Developers <opencog@googlegroups.com>  $(date -R)
EOF

# Create debian/compat
echo "11" > debian/compat

# Create debian/copyright
cat > debian/copyright << 'EOF'
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: atomspace-machspace
Source: https://github.com/cogpy/occ

Files: *
Copyright: 2025 OpenCog Foundation
License: AGPL-3.0+

License: AGPL-3.0+
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 .
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.
 .
 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
EOF

# Create debian/install
cat > debian/install << 'EOF'
usr/lib/libatomspace-machspace.so* usr/lib/
EOF

# Create debian/atomspace-machspace-dev.install
cat > debian/atomspace-machspace-dev.install << 'EOF'
usr/include/* usr/include/
EOF

echo "✓ Package structure created"
echo ""
echo "To build the package:"
echo "  cd ${PACKAGE_NAME}-${VERSION}"
echo "  dpkg-buildpackage -rfakeroot -us -uc"
