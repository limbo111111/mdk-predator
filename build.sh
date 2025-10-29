#!/bin/bash
#
# Simple build script for MDK-Predator PortaPack Application
# Based on the mayhem-firmware standard build process
#

set -e

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Directories
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MAYHEM_DIR="${MAYHEM_DIR:-${SCRIPT_DIR}/build/mayhem-firmware}"
OUTPUT_DIR="${SCRIPT_DIR}/build/output"

print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

show_usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Simple build script for MDK-Predator external PortaPack application.

OPTIONS:
    -m PATH     Path to mayhem-firmware (default: build/mayhem-firmware)
    -d          Download mayhem-firmware automatically
    -c          Clean build
    -h          Show this help

EXAMPLES:
    # Download firmware and build
    $0 -d

    # Build with existing firmware
    $0 -m /path/to/mayhem-firmware

    # Clean build
    $0 -c -d

EOF
}

# Parse arguments
DOWNLOAD_FIRMWARE=0
CLEAN_BUILD=0

while getopts "m:dch" opt; do
    case $opt in
        m) MAYHEM_DIR="$OPTARG" ;;
        d) DOWNLOAD_FIRMWARE=1 ;;
        c) CLEAN_BUILD=1 ;;
        h) show_usage; exit 0 ;;
        *) show_usage; exit 1 ;;
    esac
done

# Download mayhem-firmware if requested
if [ $DOWNLOAD_FIRMWARE -eq 1 ]; then
    if [ ! -d "$MAYHEM_DIR" ]; then
        print_info "Downloading mayhem-firmware..."
        mkdir -p "$(dirname "$MAYHEM_DIR")"
        git clone --depth 1 https://github.com/portapack-mayhem/mayhem-firmware.git "$MAYHEM_DIR"
        print_info "Initializing submodules..."
        cd "$MAYHEM_DIR"
        git submodule update --init --recursive --depth 1 hackrf
    else
        print_info "Mayhem firmware already exists at $MAYHEM_DIR"
    fi
fi

# Verify mayhem-firmware exists
if [ ! -d "$MAYHEM_DIR" ]; then
    print_error "Mayhem firmware not found at: $MAYHEM_DIR"
    print_error "Use -d to download automatically or -m to specify path"
    exit 1
fi

print_info "Using mayhem-firmware at: $MAYHEM_DIR"

# Copy app files to external directory
print_info "Copying app files to mayhem-firmware..."
APP_DIR="$MAYHEM_DIR/firmware/application/external/mdk_predator"
mkdir -p "$APP_DIR"

# Copy all necessary files
cp -r "$SCRIPT_DIR/app/"* "$APP_DIR/"
cp -r "$SCRIPT_DIR/src" "$APP_DIR/"
cp -r "$SCRIPT_DIR/include" "$APP_DIR/"

print_info "Files copied to $APP_DIR"

# Register app in external.cmake
print_info "Registering app in external.cmake..."
EXTERNAL_CMAKE="$MAYHEM_DIR/firmware/application/external/external.cmake"

# Check if already registered
if ! grep -q "mdk_predator" "$EXTERNAL_CMAKE"; then
    # Add source files
    cat >> "$EXTERNAL_CMAKE" << 'EOF'

	#mdk_predator
	external/mdk_predator/main.cpp
	external/mdk_predator/mdk_predator_app.cpp
	external/mdk_predator/src/mdk_predator.c
	external/mdk_predator/src/automotive/key_fob_analyzer.c
	external/mdk_predator/src/automotive/rolling_code_tester.c
	external/mdk_predator/src/wireless/wifi_analyzer.c
	external/mdk_predator/src/wireless/bluetooth_analyzer.c
	external/mdk_predator/src/wireless/subghz_analyzer.c
	external/mdk_predator/src/crypto/crypto_analyzer.c
EOF

    # Add app to list (append before the closing parenthesis)
    sed -i '/^set(EXTAPPLIST$/,/^)$/s/)$/\tmdk_predator\n)/' "$EXTERNAL_CMAKE"
    print_info "App registered in external.cmake"
else
    print_info "App already registered in external.cmake"
fi

# Build libopencm3 first (required)
print_info "Building libopencm3..."
cd "$MAYHEM_DIR/hackrf/firmware/libopencm3"
make TARGETS=lpc43xx -j$(nproc)

# Configure and build
cd "$MAYHEM_DIR"

if [ $CLEAN_BUILD -eq 1 ]; then
    print_info "Cleaning build directory..."
    rm -rf build
fi

print_info "Configuring build..."
mkdir -p build
cd build
cmake ..

print_info "Building application..."
make application -j$(nproc)

# Find and copy output
print_info "Copying output files..."
mkdir -p "$OUTPUT_DIR"

PPMA_FILE=$(find . -name "mdk_predator.ppma" 2>/dev/null | head -1)
if [ -n "$PPMA_FILE" ]; then
    cp "$PPMA_FILE" "$OUTPUT_DIR/"
    cp "$SCRIPT_DIR/mdk_predator.conf" "$OUTPUT_DIR/" 2>/dev/null || true
    print_info "Build successful!"
    print_info "Output: $OUTPUT_DIR/mdk_predator.ppma"
    echo ""
    print_info "Next steps:"
    echo "  1. Copy $OUTPUT_DIR/mdk_predator.ppma to SD card /APPS/ directory"
    echo "  2. Insert SD card into PortaPack and launch from Apps menu"
else
    print_error "Build failed - mdk_predator.ppma not found"
    exit 1
fi
