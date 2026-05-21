# Ensure uv is installed
Write-Host "Ensuring uv exists..."
if (-not (Get-Command uv -ErrorAction SilentlyContinue)) {
    Write-Host "uv not installed"
    exit 1
}

# Install Conan in a virtual environment
Write-Host "Creating venv + installing conan..."
uv venv
uv pip install conan

# Install dependencies + generate toolchain
Write-Host "Running conan install..."
& ".venv\Scripts\Activate.ps1"

conan profile detect --force | Out-Null

conan install . --build=missing

# Configure CMake using preset
Write-Host "Configuring CMake using preset..."
cmake --preset conan-default

# Build
Write-Host "Building with CMake preset..."
ls build
cmake --build build --preset conan-release # For some reason, Windows has differing default configuration and build preset names
