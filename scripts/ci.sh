#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

BUILD_DIR="build"

# 1. Ensure Conan is available (CI installs it via pip or package manager)
command -v conan >/dev/null 2>&1 || {
  echo "Conan not found. Install it in CI environment first."
  exit 1
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
