#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="build"
PRESET="${1:-dev-debug}"

echo "========== Velm Build Script =========="
echo "Preset: $PRESET"

# Create build directory
mkdir -p "$BUILD_DIR"
echo "Creating build directory..."

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
    echo "Using ccache for compiler caching"
    export CCACHE_DIR="${CCACHE_DIR:-$HOME/.ccache}"
    mkdir -p "$CCACHE_DIR"
    EXTRA_CMAKE_ARGS="-DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache"
else
    EXTRA_CMAKE_ARGS=""
fi

# Configure project
echo "Configuring project with preset $PRESET..."
cmake --preset "$PRESET" -B "$BUILD_DIR" $EXTRA_CMAKE_ARGS -Wno-dev
echo "CMake configuration successful"

cd "$BUILD_DIR"

# Build project
echo "Building project..."
cmake --build . --parallel "$CORES"
echo "Build completed"

# Run tests
if command -v ctest >/dev/null 2>&1; then
    echo "=== Running tests ==="
    ctest --output-on-failure
else
    echo "CTest not found, skipping tests"
fi

# Run sanitizer checks if this preset is sanitizer
if [[ "$PRESET" == *sanitize* ]]; then
    echo "=== Running sanitizer tests ==="
    # Tests are already instrumented by preset; output shown via ctest above
    echo "Sanitizer build executed; check logs for errors"
fi

# Collect coverage if preset indicates coverage
if [[ "$PRESET" == *coverage* ]]; then
    echo "=== Generating coverage report ==="
    COVERAGE_DIR="coverage"
    mkdir -p "$COVERAGE_DIR"

    if command -v lcov >/dev/null 2>&1 && command -v genhtml >/dev/null 2>&1; then
        lcov --capture --directory . --output-file "$COVERAGE_DIR/coverage.info"
        lcov --remove "$COVERAGE_DIR/coverage.info" "/usr/*" --output-file "$COVERAGE_DIR/coverage.info"
        genhtml "$COVERAGE_DIR/coverage.info" --output-directory "$COVERAGE_DIR/html"
        echo "Coverage report generated: $BUILD_DIR/$COVERAGE_DIR/html/index.html"
    else
        echo "lcov/genhtml not found, coverage report skipped"
    fi
fi

# Optionally run main dev binary
MAIN_BIN="./bin/velm_dev"
if [[ -x "$MAIN_BIN" ]]; then
    echo "=== Running main binary ==="
    "$MAIN_BIN"
else
    echo "Executable $MAIN_BIN not found, skipping run"
fi

echo "========== Velm Build Script Finished =========="
