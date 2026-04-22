#!/bin/bash
# Example script showing the Debian package build commands
# This demonstrates the exact workflow that the debian-packages.yml GitHub Action executes

set -e

echo "================================"
echo "Debian Package Build Commands"
echo "================================"
echo ""

# Prerequisites
echo "# Prerequisites"
echo "sudo apt-get install -y debhelper dpkg-dev cmake g++ \\"
echo "  guile-3.0-dev libboost-all-dev binutils-dev libiberty-dev"
echo ""

# Build cogutil
echo "# Build cogutil"
echo "cd cogutil"
echo "dpkg-buildpackage -us -uc -b"
echo ""

# Build atomspace (requires cogutil installed)
echo "# Build atomspace (requires cogutil installed)"
echo "cd atomspace"
echo "dpkg-buildpackage -us -uc -b"
echo ""

# Build unify (requires cogutil, atomspace)
echo "# Build unify (requires cogutil, atomspace)"
echo "cd unify"
echo "dpkg-buildpackage -us -uc -b"
echo ""

# Build ure (requires cogutil, atomspace, unify)
echo "# Build ure (requires cogutil, atomspace, unify)"
echo "cd ure"
echo "dpkg-buildpackage -us -uc -b"
echo ""

# Build cogserver (requires cogutil, atomspace)
echo "# Build cogserver (requires cogutil, atomspace)"
echo "cd cogserver"
echo "dpkg-buildpackage -us -uc -b"
echo ""

# Build attention (requires cogutil, atomspace, cogserver)
echo "# Build attention (requires cogutil, atomspace, cogserver)"
echo "cd attention"
echo "dpkg-buildpackage -us -uc -b"
echo ""

# Build moses (requires cogutil)
echo "# Build moses (requires cogutil)"
echo "cd moses"
echo "dpkg-buildpackage -us -uc -b"
echo ""

# Build asmoses (requires moses, atomspace)
echo "# Build asmoses (requires moses, atomspace)"
echo "cd asmoses"
echo "dpkg-buildpackage -us -uc -b"
echo ""

# Build miner (requires cogutil, atomspace, ure)
echo "# Build miner (requires cogutil, atomspace, ure)"
echo "cd miner"
echo "dpkg-buildpackage -us -uc -b"
echo ""

# Build pln (requires cogutil, atomspace, ure)
echo "# Build pln (requires cogutil, atomspace, ure)"
echo "cd pln"
echo "dpkg-buildpackage -us -uc -b"
echo ""

echo "================================"
echo "Build complete!"
echo "Generated .deb packages can be installed with:"
echo "  sudo dpkg -i *.deb"
echo "  sudo apt-get install -f -y"
echo "================================"
