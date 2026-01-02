#!/bin/bash

set -e

DIRNAME=$(realpath "$(dirname "$0")/..")
TOOLCHAIN_DIR="${DIRNAME}/toolchain/build_toolchain"

rm -rf "$TOOLCHAIN_DIR"

# Create build directory for toolchain
mkdir -p "$TOOLCHAIN_DIR"
cd "$TOOLCHAIN_DIR"

# # Run CMake to configure and build the toolchain
# cmake ..
# cmake --build .
# TODO go back to make

EXPORT PATH="$TOOLCHAIN_DIR/install/bin:$PATH"