#!/bin/bash

set -e  # Exit on any error

BUILD_DIR="build"

echo "🔧 Creating build directory..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "⚙️ Running CMake..."
cmake ..

echo "🧱 Building all targets..."
make -j$(nproc)

echo "✅ Build complete!"
