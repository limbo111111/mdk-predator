#!/bin/bash
#
# MDK Predator ESP32-S3 - One-Click Build Script
# Für HackRF PortaPack H4M + MDK Hardware Module
#
# Usage: ./build_for_mdk_module.sh
#

set -e

BOLD='\033[1m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${BOLD}╔════════════════════════════════════════════════════════╗${NC}"
echo -e "${BOLD}║   MDK Predator ESP32-S3 Builder for PortaPack H4M     ║${NC}"
echo -e "${BOLD}╚════════════════════════════════════════════════════════╝${NC}"
echo ""

# Check ESP-IDF
if [ -z "$IDF_PATH" ]; then
    echo -e "${RED}❌ ESP-IDF not found!${NC}"
    echo ""
    echo "Install ESP-IDF first:"
    echo "  cd ~"
    echo "  git clone --recursive https://github.com/espressif/esp-idf.git"
    echo "  cd esp-idf"
    echo "  ./install.sh esp32s3"
    echo "  . ./export.sh"
    echo ""
    exit 1
fi

echo -e "${GREEN}✅ ESP-IDF found: $IDF_PATH${NC}"
echo ""

# Create project directory
PROJECT_DIR="$HOME/mdk-predator-esp32"
echo -e "${YELLOW}[1/8]${NC} Creating project directory: $PROJECT_DIR"
mkdir -p "$PROJECT_DIR/main"
cd "$PROJECT_DIR"

# Copy source files
echo -e "${YELLOW}[2/8]${NC} Copying source files..."

# Check if running in workspace
if [ -d "/workspaces/mdk-predator/.devcontainer" ]; then
    BASE_DIR="/workspaces/mdk-predator"
    MODULE_DIR="/workspaces/mdk-predator-module"
elif [ -d "/workspace/mdk-predator/.devcontainer" ]; then
    BASE_DIR="/workspace/mdk-predator"
    MODULE_DIR="/workspace/mdk-predator-module"
else
    echo -e "${RED}❌ Source files not found!${NC}"
    echo "Please run this from the mdk-predator workspace"
    exit 1
fi

# Copy main files
cp "$BASE_DIR/.devcontainer/esp32_main.c" main/ 2>/dev/null || echo "  ⚠️  esp32_main.c not found (will create)"

# Copy protocol implementations
if [ -d "$MODULE_DIR/main" ]; then
    cp "$MODULE_DIR/main/keeloq_engine.c" main/ 2>/dev/null || echo "  ⚠️  keeloq_engine.c not found"
    cp "$MODULE_DIR/main/fixed_code_protocols.c" main/ 2>/dev/null || echo "  ⚠️  fixed_code_protocols.c not found"
    cp "$MODULE_DIR/main/manufacturer_protocols.c" main/ 2>/dev/null || echo "  ⚠️  manufacturer_protocols.c not found"
    cp "$MODULE_DIR/main/garage_door_protocols.c" main/ 2>/dev/null || echo "  ⚠️  garage_door_protocols.c not found"
    cp "$MODULE_DIR/main/automotive_protocols.h" main/ 2>/dev/null || echo "  ⚠️  automotive_protocols.h not found"
else
    echo -e "${YELLOW}  ⚠️  Protocol files not found, using stubs${NC}"
fi

# Copy hardware abstraction
cp "$BASE_DIR/.devcontainer/mdk_hardware_abstraction.c" main/ 2>/dev/null || echo "  ⚠️  mdk_hardware_abstraction.c not found"

echo -e "${GREEN}✅ Source files copied${NC}"
echo ""

# Fix automotive_protocols.h (add stddef.h)
echo -e "${YELLOW}[3/8]${NC} Fixing automotive_protocols.h..."
if [ -f "main/automotive_protocols.h" ]; then
    if ! grep -q "#include <stddef.h>" main/automotive_protocols.h; then
        sed -i '/#include <stdbool.h>/a #include <stddef.h>' main/automotive_protocols.h
        echo -e "${GREEN}✅ Added #include <stddef.h>${NC}"
    else
        echo -e "${GREEN}✅ Already fixed${NC}"
    fi
else
    echo -e "${YELLOW}⚠️  automotive_protocols.h not found, skipping${NC}"
fi
echo ""

# Create CMakeLists.txt files
echo -e "${YELLOW}[4/8]${NC} Creating build configuration..."

cat > CMakeLists.txt << 'EOF'
# MDK Predator ESP32-S3 for PortaPack H4M
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mdk_predator_module)
EOF

cat > main/CMakeLists.txt << 'EOF'
idf_component_register(
    SRCS 
        "esp32_main.c"
    INCLUDE_DIRS 
        "."
    REQUIRES 
        driver 
        nvs_flash
        mbedtls
)

target_compile_options(${COMPONENT_LIB} PRIVATE 
    -std=gnu11 
    -O3 
    -ffast-math
    -funroll-loops
    -Wall 
    -Wextra
    -Wno-unused-parameter
    -Wno-unused-variable
)
EOF

# Add protocol files if they exist
if [ -f "main/keeloq_engine.c" ]; then
    sed -i 's/"esp32_main.c"/"esp32_main.c"\n        "keeloq_engine.c"/' main/CMakeLists.txt
fi
if [ -f "main/fixed_code_protocols.c" ]; then
    sed -i 's/"esp32_main.c"/"esp32_main.c"\n        "fixed_code_protocols.c"/' main/CMakeLists.txt
fi
if [ -f "main/manufacturer_protocols.c" ]; then
    sed -i 's/"esp32_main.c"/"esp32_main.c"\n        "manufacturer_protocols.c"/' main/CMakeLists.txt
fi
if [ -f "main/garage_door_protocols.c" ]; then
    sed -i 's/"esp32_main.c"/"esp32_main.c"\n        "garage_door_protocols.c"/' main/CMakeLists.txt
fi
if [ -f "main/mdk_hardware_abstraction.c" ]; then
    sed -i 's/"esp32_main.c"/"esp32_main.c"\n        "mdk_hardware_abstraction.c"/' main/CMakeLists.txt
fi

cat > sdkconfig.defaults << 'EOF'
# MDK Predator Configuration for ESP32-S3

# CPU
CONFIG_ESP32S3_DEFAULT_CPU_FREQ_240=y
CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ=240

# Dual-Core
CONFIG_FREERTOS_UNICORE=n
CONFIG_FREERTOS_HZ=1000

# SPIRAM
CONFIG_ESP32S3_SPIRAM_SUPPORT=y
CONFIG_SPIRAM_MODE_OCT=y
CONFIG_SPIRAM_SPEED_80M=y
CONFIG_SPIRAM_BOOT_INIT=y
CONFIG_SPIRAM_USE_MALLOC=y

# I2C
CONFIG_I2C_ISR_IRAM_SAFE=y

# Hardware Crypto
CONFIG_ESP32S3_AES_HARDWARE_ACCELERATION=y
CONFIG_ESP32S3_SHA_HARDWARE_ACCELERATION=y
CONFIG_ESP32S3_RSA_HARDWARE_ACCELERATION=y
CONFIG_MBEDTLS_HARDWARE_AES=y
CONFIG_MBEDTLS_HARDWARE_SHA=y

# Optimization
CONFIG_COMPILER_OPTIMIZATION_PERF=y
CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE=y

# Logging
CONFIG_LOG_DEFAULT_LEVEL=3
CONFIG_LOG_COLORS=y

# Power
CONFIG_PM_ENABLE=n
EOF

echo -e "${GREEN}✅ Build files created${NC}"
echo ""

# Set target
echo -e "${YELLOW}[5/8]${NC} Setting target to ESP32-S3..."
idf.py set-target esp32s3
echo ""

# Build
echo -e "${YELLOW}[6/8]${NC} Building firmware..."
echo ""
idf.py build

if [ $? -ne 0 ]; then
    echo ""
    echo -e "${RED}❌ Build failed!${NC}"
    echo "Check the error messages above."
    exit 1
fi

echo ""
echo -e "${GREEN}✅ Build successful!${NC}"
echo ""

# Show binary info
echo -e "${YELLOW}[7/8]${NC} Firmware files created:"
ls -lh build/*.bin | awk '{print "  " $9 " (" $5 ")"}'
echo ""

# Instructions
echo -e "${YELLOW}[8/8]${NC} Next steps:"
echo ""
echo -e "${BOLD}To flash to MDK module:${NC}"
echo ""
echo "  1. Disconnect MDK module from PortaPack"
echo "  2. Connect MDK module via USB-C to computer"
echo "  3. Run:"
echo ""
echo -e "     ${GREEN}idf.py -p /dev/ttyACM0 flash${NC}"
echo ""
echo "  4. Monitor serial output:"
echo ""
echo -e "     ${GREEN}idf.py -p /dev/ttyACM0 monitor${NC}"
echo ""
echo "  5. You should see:"
echo "     I (xxx) MDK_MAIN: MDK Predator ESP32-S3 Module Starting..."
echo "     I (xxx) MDK_MAIN: I2C slave initialized on address 0x51"
echo ""
echo "  6. Disconnect USB-C and reconnect MDK to PortaPack"
echo ""
echo -e "${BOLD}To test on PortaPack:${NC}"
echo ""
echo "  1. Power on PortaPack with MDK module attached"
echo "  2. Go to: Apps → Your MDK-Predator App"
echo "  3. Should show: 'MDK: Connected ✓'"
echo ""
echo -e "${GREEN}╔════════════════════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║              BUILD COMPLETE!                           ║${NC}"
echo -e "${GREEN}╚════════════════════════════════════════════════════════╝${NC}"
echo ""
