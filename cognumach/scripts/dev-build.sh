#!/usr/bin/env bash
set -euo pipefail

if [ ! -f configure ]; then
  echo "configure not found; did you run from repo root?" >&2
  exit 1
fi

./configure --host=i686-gnu CC='gcc -m32' LD='ld -melf_i386' MIG='mig'
make -j"$(nproc)"
make check

