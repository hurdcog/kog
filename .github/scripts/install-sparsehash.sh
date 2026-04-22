#!/bin/bash
# install-sparsehash.sh
# Specialized installer for Google Sparsehash library
# Handles both libsparsehash-dev and legacy libgoogle-sparsehash-dev names
# Falls back to source build if package not available

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Color codes
RED='\033[0;31m'
BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${BLUE}=== Installing Google Sparsehash ===${NC}"

# Try modern package name first
echo -e "${BLUE}Trying package: libsparsehash-dev${NC}"
if "$SCRIPT_DIR/install-dependency.sh" "libsparsehash-dev" \
   "https://github.com/sparsehash/sparsehash/archive/refs/tags/sparsehash-2.0.4.tar.gz" \
   "autoreconf -i && ./configure && make && sudo make install" \
   "test -f /usr/local/include/google/sparse_hash_map || test -f /usr/include/google/sparse_hash_map"; then
    echo -e "${GREEN}=== Sparsehash installed successfully ===${NC}"
    exit 0
fi

# Try legacy package name
echo -e "${YELLOW}Trying legacy package name: libgoogle-sparsehash-dev${NC}"
if "$SCRIPT_DIR/install-dependency.sh" "libgoogle-sparsehash-dev" \
   "https://github.com/sparsehash/sparsehash/archive/refs/tags/sparsehash-2.0.4.tar.gz" \
   "autoreconf -i && ./configure && make && sudo make install" \
   "test -f /usr/local/include/google/sparse_hash_map || test -f /usr/include/google/sparse_hash_map"; then
    echo -e "${GREEN}=== Sparsehash installed successfully ===${NC}"
    exit 0
fi

echo -e "${YELLOW}Failed to install via package manager, attempting direct source build...${NC}"

# Final fallback: Direct source build with all dependencies
sudo apt-get update -qq
sudo apt-get install -y build-essential git wget unzip autoconf automake libtool

BUILD_DIR=$(mktemp -d)
cd "$BUILD_DIR"

echo "Downloading sparsehash source..."
if wget -q https://github.com/sparsehash/sparsehash/archive/refs/tags/sparsehash-2.0.4.tar.gz; then
    tar -xzf sparsehash-2.0.4.tar.gz
    cd sparsehash-sparsehash-2.0.4
    
    echo "Building sparsehash..."
    autoreconf -i
    ./configure
    make
    sudo make install
    
    echo "Verifying installation..."
    if test -f /usr/local/include/google/sparse_hash_map || test -f /usr/include/google/sparse_hash_map; then
        echo -e "${GREEN}=== Sparsehash built and installed successfully from source ===${NC}"
        cd /
        rm -rf "$BUILD_DIR"
        exit 0
    fi
fi

cd /
rm -rf "$BUILD_DIR"
echo -e "${RED}ERROR: Failed to install sparsehash${NC}"
exit 1
