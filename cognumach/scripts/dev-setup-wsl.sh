#!/usr/bin/env bash
set -euo pipefail

sudo apt update
sudo apt install -y build-essential gcc-multilib binutils binutils-multiarch \
  autoconf automake libtool pkg-config gawk bison flex nasm \
  xorriso grub-pc-bin mtools qemu-system-x86 \
  git python3 cppcheck clang-tools || true

if ! command -v mig >/dev/null 2>&1; then
  sudo apt install -y mig || true
fi

if ! command -v mig >/dev/null 2>&1; then
  sudo apt install -y texinfo
  # Use the local mig directory instead of downloading
  SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
  MIG_DIR="$SCRIPT_DIR/../mig"
  
  if [ ! -d "$MIG_DIR" ]; then
    echo "Error: Local mig directory not found at $MIG_DIR"
    exit 1
  fi
  
  pushd "$MIG_DIR"
  # Check if bootstrap has already been run
  if [ ! -f "configure" ]; then
    ./bootstrap
  fi
  # Check if already configured
  if [ ! -f "Makefile" ]; then
    ./configure
  fi
  make -j"$(nproc)"
  sudo make install
  popd
fi

echo "WSL dev dependencies installed."

