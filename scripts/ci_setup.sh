#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

# Install Conan
command -v conan >/dev/null 2>&1 || {
  python3 -m pip install --user conan || {
    echo "Error: Conan is not installed and could not be installed via pip."
    exit 1
  }
}

# Install dependencies + generate toolchain
conan profile detect --force >/dev/null 2>&1 || true
conan install . \
  --build=missing

# Configure CMake using preset
cmake --preset conan-release

# Build
cmake --build --preset conan-release --parallel
