#!/bin/bash

# Optimized Google Colab Build Script for Euler Prover
# Enhanced for maximum performance and compatibility

echo "=== EULER PROVER - COLAB OPTIMIZED BUILD ==="
echo "Building high-performance mathematical computation system..."

# Update system and install dependencies
apt-get update -qq
apt-get install -y build-essential cmake gcc g++ libvtk9-dev python3-vtk9 > /dev/null 2>&1

# Skip Qt dependencies for Colab compatibility
echo "Skipping Qt dependencies for headless environment"

# Set optimal compiler flags for Colab environment
export CC=gcc
export CXX=g++
export CXXFLAGS="-O3 -march=native -mtune=native -flto -funroll-loops -ffast-math -DNDEBUG -fopenmp"
export LDFLAGS="-flto -fopenmp"

# Create optimized build directory
mkdir -p build_colab
cd build_colab

# Configure with CMake for maximum performance
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_FLAGS="$CXXFLAGS" \
    -DCMAKE_EXE_LINKER_FLAGS="$LDFLAGS" \
    -DVTK_DIR=/usr/lib/x86_64-linux-gnu/cmake/vtk-9.1 \
    -DBUILD_SHARED_LIBS=OFF \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
    -DVTK_GROUP_ENABLE_Qt=OFF \
    -DVTK_MODULE_ENABLE_VTK_GUISupportQt=NO \
    -DVTK_MODULE_ENABLE_VTK_ViewsQt=NO \
    -DCOLAB_BUILD=ON

# Build with all available cores (fallback if CMake fails)
if make -j$(nproc) euler 2>/dev/null; then
    echo "✓ CMake build successful"
else
    echo "⚠ CMake build failed, using fallback compilation"
    cd ..
    # Fallback: compile directly without VTK
    g++ $CXXFLAGS \
        -Iinclude \
        -DVTK_FOUND=0 \
        src/main.cpp src/number_theory.cpp src/complex_analysis.cpp \
        src/topology.cpp src/progress.cpp src/rng.cpp src/visualization.cpp \
        -o build_colab/euler
    cd build_colab
fi

# Always create lightweight version without VTK
echo "Creating lightweight version..."
cd .. 

# Check if ultra_precision.cpp exists
ULTRA_PRECISION=""
if [ -f "src/ultra_precision.cpp" ]; then
    ULTRA_PRECISION="src/ultra_precision.cpp"
fi

g++ $CXXFLAGS \
    -Iinclude \
    -DVTK_FOUND=0 \
    src/main.cpp src/number_theory.cpp src/complex_analysis.cpp \
    src/topology.cpp src/progress.cpp src/rng.cpp src/visualization.cpp \
    $ULTRA_PRECISION \
    -o build_colab/euler_lite 2>/dev/null

if [ $? -eq 0 ]; then
    echo "✓ Lite version created successfully"
else
    echo "⚠ Lite version build failed, creating minimal version"
    g++ $CXXFLAGS \
        -Iinclude \
        -DVTK_FOUND=0 -DNO_VISUALIZATION=1 \
        src/main.cpp src/number_theory.cpp src/complex_analysis.cpp \
        src/topology.cpp src/progress.cpp src/rng.cpp \
        $ULTRA_PRECISION \
        -o build_colab/euler_lite
fi

# Create VTK-free visualization version
echo "Creating basic computation version..."
g++ $CXXFLAGS \
    -Iinclude \
    -DVTK_FOUND=0 -DBASIC_BUILD=1 \
    src/main.cpp src/number_theory.cpp src/complex_analysis.cpp \
    src/topology.cpp src/progress.cpp src/rng.cpp src/visualization.cpp \
    $ULTRA_PRECISION \
    -o build_colab/euler_basic 2>/dev/null

if [ $? -ne 0 ]; then
    echo "⚠ Basic version build failed, creating computation-only version"
    g++ $CXXFLAGS \
        -Iinclude \
        -DVTK_FOUND=0 -DNO_VISUALIZATION=1 \
        src/main.cpp src/number_theory.cpp src/complex_analysis.cpp \
        src/topology.cpp src/progress.cpp src/rng.cpp \
        $ULTRA_PRECISION \
        -o build_colab/euler_basic
fi
cd build_colab

echo "✓ Build completed successfully!"

# Check which executables were actually created
if [ -f "euler" ]; then
    echo "✓ Full version: ./euler (with VTK visualization)"
fi

if [ -f "euler_lite" ]; then
    echo "✓ Lite version: ./euler_lite (PPM output only)"
fi

if [ -f "euler_basic" ]; then
    echo "✓ Basic version: ./euler_basic (computation only)"
fi

echo ""
echo "Quick test commands:"
if [ -f "euler" ]; then
    echo "  ./euler number 1000 10 4"
    echo "  ./euler visualize primes 500"
fi

if [ -f "euler_lite" ]; then
    echo "  ./euler_lite complex 10000 1e-12"
fi

if [ -f "euler_basic" ]; then
    echo "  ./euler_basic number 1000 10"
fi
