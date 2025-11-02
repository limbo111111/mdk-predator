#!/bin/bash
# Quick fix and build script for MDK-Predator

echo "Applying fixes..."

# Fix 1: Ensure include directory is in CMakeLists.txt
if ! grep -q "include_directories(external/mdk_predator/include)" /workspaces/mdk-predator/build/mayhem-firmware/firmware/application/CMakeLists.txt; then
    echo "Adding include directory to CMakeLists.txt..."
    sed -i '/^include_directories(. ${INCDIR})/a include_directories(external/mdk_predator/include)' \
        /workspaces/mdk-predator/build/mayhem-firmware/firmware/application/CMakeLists.txt
fi

# Fix 2: Ensure sources are in external.cmake
if ! grep -q "external/mdk_predator/main.cpp" /workspaces/mdk-predator/build/mayhem-firmware/firmware/application/external/external.cmake; then
    echo "Adding sources to external.cmake..."
    # Find the line with adult_toys_controller
    LINE=$(grep -n "external/adult_toys_controller/ui_adult_toys_controller.cpp" /workspaces/mdk-predator/build/mayhem-firmware/firmware/application/external/external.cmake | cut -d: -f1)
    if [ ! -z "$LINE" ]; then
        sed -i "${LINE}i\\        #mdk_predator\\n        external/mdk_predator/main.cpp\\n        external/mdk_predator/mdk_predator_app.cpp\\n        external/mdk_predator/src/mdk_predator.c\\n        external/mdk_predator/src/automotive/key_fob_analyzer.c\\n        external/mdk_predator/src/automotive/rolling_code_tester.c\\n        external/mdk_predator/src/wireless/wifi_analyzer.c\\n        external/mdk_predator/src/wireless/bluetooth_analyzer.c\\n        external/mdk_predator/src/wireless/subghz_analyzer.c\\n        external/mdk_predator/src/crypto/crypto_analyzer.c\\n" \
            /workspaces/mdk-predator/build/mayhem-firmware/firmware/application/external/external.cmake
    fi
fi

# Fix 3: Clean CMake cache
echo "Cleaning CMake cache..."
rm -rf /workspaces/mdk-predator/build/mayhem-firmware/build/CMakeCache.txt
rm -rf /workspaces/mdk-predator/build/mayhem-firmware/build/CMakeFiles

echo "✓ Fixes applied!"
echo ""
echo "Starting build..."
cd /workspaces/mdk-predator && ./build.sh -c -d
