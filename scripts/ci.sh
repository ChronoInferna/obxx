#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

BUILD_DIR="build"

# 1. Install Conan (if not already installed in CI environment)
command -v conan >/dev/null 2>&1 || {
  python3 -m pip install --user conan || {
    echo "Error: Conan is not installed and could not be installed via pip."
    exit 1
  }
}

# 2. Clean build (CI should be stateless)
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# 3. Install dependencies + generate toolchain
conan install . \
  --build=missing \
  -s build_type=Release

# 4. Configure CMake using preset (preferred)
cmake --preset conan-release

# 5. Build
cmake --build --preset conan-release --parallel
