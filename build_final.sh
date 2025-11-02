#!/bin/bash
# Final build script with all dependencies

set -e

echo "=== MDK-Predator Final Build ==="

# Set PATH
export PATH=/opt/gcc-arm-none-eabi-9-2019-q4-major/bin:$PATH

# Verify tools
echo "Checking tools..."
which arm-none-eabi-gcc || { echo "ARM GCC not found!"; exit 1; }
which cmake || { export PATH=/usr/bin:$PATH; }
which python3 || { echo "Python3 not found!"; exit 1; }
python3 -c "import yaml" || { echo "PyYAML not installed!"; exit 1; }

echo "✅ All tools found"
echo ""

# Navigate to build directory
cd /workspaces/mdk-predator/build/mayhem-firmware/build

# Clean and reconfigure
echo "Reconfiguring CMake..."
rm -rf CMakeCache.txt CMakeFiles
cmake ..

# Build application
echo ""
echo "Building application..."
make application -j$(nproc)

BUILD_STATUS=$?

if [ $BUILD_STATUS -eq 0 ]; then
    echo ""
    echo "=== BUILD SUCCESSFUL ==="
    echo ""
    echo "Looking for PPMA files..."
    find firmware/application -name "*.ppma" 2>/dev/null | grep -i mdk || echo "No .ppma found yet"
    
    echo ""
    echo "External app files:"
    ls -lh firmware/application/external_app_mdk_predator.* 2>/dev/null || echo "No mdk_predator external files"
    
    echo ""
    echo "Checking if mdk_predator section exists in ELF..."
    strings firmware/application/application.elf | grep -i "mdk-predator" | head -3
    
else
    echo ""
    echo "❌ BUILD FAILED with exit code $BUILD_STATUS"
    echo ""
    echo "Last 50 lines of build output:"
    tail -50 /tmp/final_build.log 2>/dev/null || echo "No log file"
fi

exit $BUILD_STATUS
