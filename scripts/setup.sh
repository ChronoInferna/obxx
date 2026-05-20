#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_ROOT"

echo "[1/3] Ensuring uv exists..."
command -v uv >/dev/null 2>&1 || {
  echo "uv not installed"
  exit 1
}

echo "[2/3] Creating venv + installing conan..."
uv venv .venv
uv pip install --python .venv/bin/python conan

source .venv/bin/activate

echo "[3/3] Running conan install..."
conan profile detect --force >/dev/null 2>&1 || true

conan install . \
  --build=missing

echo "Done. Now run:"
echo "  cmake --preset conan-release"
