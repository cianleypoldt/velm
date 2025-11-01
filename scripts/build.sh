#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="build/dev-debug"

echo "Creating build directory..."
mkdir -p "$BUILD_DIR"

# Detect CPU cores
if command -v nproc >/dev/null 2>&1; then
    CORES=$(nproc)
elif [[ "$(uname)" == "Darwin" ]] && command -v sysctl >/dev/null 2>&1; then
    CORES=$(sysctl -n hw.ncpu)
else
    CORES=2
fi
CORES=$((CORES > 0 ? CORES : 1))
echo "Using $CORES cores"

# Use ccache if available
if command -v ccache >/dev/null 2>&1; then
    echo "Using ccache for faster rebuilds"
    export CCACHE_DIR="${CCACHE_DIR:-$HOME/.ccache}"
    mkdir -p "$CCACHE_DIR"
    EXTRA_CMAKE_ARGS="-DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
else
    EXTRA_CMAKE_ARGS="-DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
fi

# Configure project using preset
echo "Configuring project with preset dev-debug..."
if ! cmake --preset dev-debug ${EXTRA_CMAKE_ARGS} -S . -B "$BUILD_DIR"
then
    echo "CMake configuration failed"
    exit 1
fi
echo "CMake configuration successful"

# Build project using preset
echo "Building project..."
if ! cmake --build "$BUILD_DIR" --preset dev-debug-build --parallel "$CORES"
then
    echo "Build failed"
    exit 1
fi
echo "Build completed successfully"

# Run main executable if it exists
cd "$BUILD_DIR/bin" || exit 1
MAIN_BIN="./velm_dev"
if [[ -x "$MAIN_BIN" ]]; then
    echo "Running $MAIN_BIN..."
    if ! "$MAIN_BIN"; then
        echo "Program execution failed"
        exit 1
    fi
else
    echo "Executable $MAIN_BIN not found, skipping run"
fi
cd - > /dev/null
#./build/dev-debug/bin/velm_tests

echo "Velm build script finished"
