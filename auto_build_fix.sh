#!/bin/bash
#
# Automated build and fix script for MDK-Predator
#

set +e  # Don't exit on error

BUILD_LOG="/tmp/mdk_build.log"
MAX_ATTEMPTS=5
ATTEMPT=1

cd /workspaces/mdk-predator

echo "=== Automated Build & Fix Script ==="
echo "Will attempt up to $MAX_ATTEMPTS builds, fixing errors automatically"
echo ""

while [ $ATTEMPT -le $MAX_ATTEMPTS ]; do
    echo "========================================="
    echo "Build Attempt #$ATTEMPT"
    echo "========================================="
    
    # Run build
    ./build.sh -d > "$BUILD_LOG" 2>&1
    BUILD_EXIT_CODE=$?
    
    if [ $BUILD_EXIT_CODE -eq 0 ]; then
        echo "✅ BUILD SUCCESSFUL!"
        echo ""
        echo "Build output location:"
        find build/mayhem-firmware/build -name "*.bin" -o -name "*.elf" 2>/dev/null | grep -E "application|portapack"
        echo ""
        echo "Build completed in $ATTEMPT attempt(s)"
        exit 0
    fi
    
    echo "⚠️  Build failed with exit code $BUILD_EXIT_CODE"
    echo ""
    
    # Check for common errors and fix them
    if grep -q "mdk_predator.h: No such file or directory" "$BUILD_LOG"; then
        echo "🔧 Detected: Missing include path for mdk_predator.h"
        echo "   Fixing include directories..."
        
        # Ensure include directory is set
        if ! grep -q "include_directories(external/mdk_predator/include)" build/mayhem-firmware/firmware/application/CMakeLists.txt; then
            sed -i '/^include_directories(. ${INCDIR})/a include_directories(external/mdk_predator/include)' \
                build/mayhem-firmware/firmware/application/CMakeLists.txt
            echo "   ✓ Added include directory to CMakeLists.txt"
        fi
        
        # Force CMake reconfigure
        rm -rf build/mayhem-firmware/build/CMakeCache.txt build/mayhem-firmware/build/CMakeFiles
        echo "   ✓ Cleared CMake cache"
        
    elif grep -q "undefined reference\|multiple definition" "$BUILD_LOG"; then
        echo "🔧 Detected: Linker errors"
        tail -50 "$BUILD_LOG"
        echo ""
        echo "   Manual intervention may be required"
        break
        
    elif grep -q "fatal error:.*: No such file" "$BUILD_LOG"; then
        MISSING_HEADER=$(grep "fatal error:" "$BUILD_LOG" | head -1 | sed 's/.*fatal error: \(.*\): No such file.*/\1/')
        echo "🔧 Detected: Missing header file: $MISSING_HEADER"
        echo "   Manual intervention required"
        break
        
    else
        echo "❌ Unknown build error. Last 30 lines of build log:"
        tail -30 "$BUILD_LOG"
        break
    fi
    
    ATTEMPT=$((ATTEMPT + 1))
    echo ""
done

if [ $ATTEMPT -gt $MAX_ATTEMPTS ]; then
    echo "❌ Maximum build attempts ($MAX_ATTEMPTS) reached without success"
    echo "Last 50 lines of build log:"
    tail -50 "$BUILD_LOG"
    exit 1
fi

exit 1
