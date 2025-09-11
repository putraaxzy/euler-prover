#!/bin/bash

# Optimized Google Colab Build Script for Euler Prover
# Enhanced for maximum performance and compatibility

echo "=== EULER PROVER - COLAB OPTIMIZED BUILD ==="
echo "Building high-performance mathematical computation system..."

# Update system and install dependencies
apt-get update -qq > /dev/null 2>&1
apt-get install -y build-essential cmake libvtk9-dev libvtk9-qt-dev qtbase5-dev libqt5widgets5-dev > /dev/null 2>&1

# Set optimal compiler flags for Colab environment
export CC=gcc-9
export CXX=g++-9
export CXXFLAGS="-O3 -march=native -mtune=native -flto -funroll-loops -ffast-math -DNDEBUG -fopenmp"
export LDFLAGS="-flto -fopenmp"

# Create optimized build directory
mkdir -p build_colab
cd build_colab

# Configure with CMake for maximum performance
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=g++-9 \
    -DCMAKE_C_COMPILER=gcc-9 \
    -DCMAKE_CXX_FLAGS="$CXXFLAGS" \
    -DCMAKE_EXE_LINKER_FLAGS="$LDFLAGS" \
    -DVTK_DIR=/usr/lib/x86_64-linux-gnu/cmake/vtk-9.1 \
    -DBUILD_SHARED_LIBS=OFF \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON

# Build with all available cores
make -j$(nproc) euler

# Create lightweight version without VTK for basic operations
echo "Creating lightweight version..."
g++-9 $CXXFLAGS \
    -I../include \
    -DVTK_FOUND=0 \
    ../src/main.cpp ../src/number_theory.cpp ../src/complex_analysis.cpp \
    ../src/topology.cpp ../src/progress.cpp ../src/visualization.cpp \
    ../src/rng.cpp \
    -o euler_lite

echo "✓ Build completed successfully!"
echo "✓ Full version: ./euler (with VTK visualization)"
echo "✓ Lite version: ./euler_lite (PPM output only)"
echo ""
echo "Quick test commands:"
echo "  ./euler proof number 1000 10 4"
echo "  ./euler visualize primes 500"
echo "  ./euler_lite proof complex 10000 15"
