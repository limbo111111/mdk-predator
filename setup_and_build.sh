#!/bin/bash
# Complete setup and build script for MDK-Predator

set -e

echo "=== MDK-Predator Complete Setup & Build ==="
echo ""

# 1. Install system dependencies
echo "[1/7] Installing system dependencies..."
sudo apk add --no-cache \
    py3-yaml py3-pip \
    cmake make git lz4 \
    ccache dfu-util \
    gcompat \
    wget tar bzip2

# 2. Install PyYAML via pip
echo "[2/7] Installing PyYAML..."
python3 -m pip install pyyaml --break-system-packages --quiet

# 3. Download and install ARM GCC Toolchain
if [ ! -d "/opt/gcc-arm-none-eabi-9-2019-q4-major" ]; then
    echo "[3/7] Downloading ARM GCC Toolchain..."
    cd /tmp
    wget -q https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2
    echo "Extracting..."
    sudo tar -xjf gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2 -C /opt/
    rm gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2
else
    echo "[3/7] ARM GCC Toolchain already installed"
fi

# 4. Set PATH
export PATH=/opt/gcc-arm-none-eabi-9-2019-q4-major/bin:$PATH
echo "export PATH=/opt/gcc-arm-none-eabi-9-2019-q4-major/bin:\$PATH" >> ~/.bashrc

# 5. Verify tools
echo "[4/7] Verifying tools..."
arm-none-eabi-gcc --version | head -1
cmake --version | head -1
python3 -c "import yaml; print('PyYAML: OK')"
echo "✅ All tools verified"

# 6. Fix libopencm3 Makefile if needed
echo "[5/7] Checking libopencm3..."
LIBOPENCM3_DIR="/workspaces/mdk-predator/build/mayhem-firmware/hackrf/firmware/libopencm3"
if [ -d "$LIBOPENCM3_DIR/lib/lpc43xx" ] && [ ! -f "$LIBOPENCM3_DIR/lib/lpc43xx/Makefile" ]; then
    echo "Creating dummy Makefile for lpc43xx..."
    printf 'all:\n\t@true\n\nclean:\n\t@true\n' > "$LIBOPENCM3_DIR/lib/lpc43xx/Makefile"
fi

# 7. Build
echo "[6/7] Starting build..."
cd /workspaces/mdk-predator
./build.sh -d

echo ""
echo "[7/7] Checking results..."
if [ -f "build/mayhem-firmware/build/firmware/application/application.elf" ]; then
    echo "✅ application.elf created ($(du -h build/mayhem-firmware/build/firmware/application/application.elf | cut -f1))"
fi

# Look for PPMA files
PPMA_FILES=$(find build/mayhem-firmware/build/firmware/application -name "*mdk_predator*.ppma" 2>/dev/null || true)
if [ -n "$PPMA_FILES" ]; then
    echo "✅ PPMA file(s) found:"
    echo "$PPMA_FILES" | while read file; do
        ls -lh "$file"
    done
    
    # Copy to output
    mkdir -p build/output
    echo "$PPMA_FILES" | while read file; do
        cp "$file" build/output/
    done
    echo ""
    echo "✅ PPMA copied to build/output/"
    ls -lh build/output/*.ppma
else
    echo "⚠️  No PPMA files found"
    echo "Checking external app files..."
    find build/mayhem-firmware/build/firmware/application -name "*mdk_predator*" | head -10
fi

echo ""
echo "=== Setup & Build Complete ==="
