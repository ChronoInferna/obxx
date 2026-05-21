#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

# Ensure uv is installed
echo "Ensuring uv exists..."
command -v uv >/dev/null 2>&1 || {
  echo "uv not installed"
  exit 1
}

# Install Conan
echo "Creating venv + installing conan..."
uv venv .venv
uv pip install --python .venv/bin/python conan

# Install dependencies + generate toolchain
echo "Running conan install..."
source .venv/bin/activate
conan profile detect --force >/dev/null 2>&1 || true
conan install . \
  --build=missing

# Final instructions
echo "Done. Now run:"
echo "  cmake --preset conan-release"
echo "and then:"
echo "  cmake --build build --preset conan-release"
