#!/bin/bash
set -e

echo "=== OCC Windows vcpkg Builder Setup ==="
echo ""

# Install build essentials
echo "Installing build tools..."
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    git

# Bootstrap vcpkg if not already done
if [ ! -f "third_party/vcpkg/vcpkg" ]; then
    echo "Bootstrapping vcpkg..."
    cd third_party/vcpkg
    ./bootstrap-vcpkg.sh
    cd ../..
else
    echo "vcpkg already bootstrapped"
fi

# Create test vcpkg.json with first 10 dependencies
echo "Creating test vcpkg.json with first 10 dependencies..."
cat > vcpkg-test-batch1.json << 'EOF'
{
  "name": "occ-test-batch1",
  "version-string": "1.0.0",
  "dependencies": [
    "boost-system",
    "boost-filesystem",
    "boost-program-options",
    "boost-regex",
    "boost-thread",
    "boost-date-time",
    "boost-serialization",
    "boost-random",
    "cxxopts",
    "asio"
  ]
}
EOF

echo ""
echo "=== Setup Complete ==="
echo ""
echo "To test building first 10 dependencies:"
echo "  ./third_party/vcpkg/vcpkg install --x-manifest-root=. --x-manifest=vcpkg-test-batch1.json"
echo ""
