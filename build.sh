#!/bin/bash

BUILD_DIR="Build" # Directory for building

# Ensure build directory exists and create if not then enter it:

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR" || exit 1

# CMake:

echo ">>> Running CMake..."
cmake .. || exit 1

# Build project:

echo ">>> Building project..."
cmake --build . --parallel "$(nproc 2>/dev/null || echo 1)" || exit 1

# Find and run executable:

# Look for executables in Build/ (top-level only)
exe=$(find . -maxdepth 1 -type f ! -name "cmake*" ! -name "*.so" ! -name "*.a" -perm +111 | head -n 1)

if [ -n "$exe" ]; then
    echo ">>> Running program: $exe"
    "$exe"
else
    echo ">>> Build succeeded, but no executable found in ./Build"
fi

echo ">>> Done!"