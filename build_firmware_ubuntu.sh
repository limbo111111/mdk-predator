#!/bin/bash
set -e

echo "🚀 MDK Predator ESP32-S3 Firmware Builder"
echo "=========================================="
echo ""
echo "⚠️  This script MUST run on Ubuntu/Debian (glibc)!"
echo "    Alpine Linux (musl) is NOT supported."
echo ""

# Check if running on glibc system
if ! ldd --version 2>&1 | grep -q "GLIBC"; then
    echo "❌ ERROR: Not running on glibc system (found musl?)"
    echo "   Please run this on:"
    echo "   - Ubuntu 22.04 / Debian"
    echo "   - GitHub Codespaces"
    echo "   - WSL2 (Ubuntu)"
    exit 1
fi

echo "✅ glibc detected, continuing..."
echo ""

# Install dependencies
echo "📦 Installing dependencies..."
sudo apt-get update -qq
sudo apt-get install -y -qq git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0

# Setup ESP-IDF
if [ ! -d "$HOME/esp/esp-idf" ]; then
    echo "📥 Cloning ESP-IDF v5.1.2..."
    mkdir -p ~/esp
    cd ~/esp
    git clone -b v5.1.2 --depth 1 --recursive https://github.com/espressif/esp-idf.git
    cd esp-idf
    echo "🔧 Installing ESP-IDF tools..."
    ./install.sh esp32s3
else
    echo "✅ ESP-IDF already installed"
fi

# Build firmware
echo "🔨 Building firmware..."
source ~/esp/esp-idf/export.sh
cd /workspaces/mdk-predator/firmware-build
idf.py set-target esp32s3
idf.py build

# Copy binaries
echo "📦 Copying binaries..."
mkdir -p /workspaces/mdk-predator/firmware-binaries
cp build/mdk-predator.bin /workspaces/mdk-predator/firmware-binaries/
cp build/bootloader/bootloader.bin /workspaces/mdk-predator/firmware-binaries/
cp build/partition_table/partition-table.bin /workspaces/mdk-predator/firmware-binaries/

# Create flash script
cat > /workspaces/mdk-predator/firmware-binaries/flash.sh << 'FLASH_EOF'
#!/bin/bash
PORT=${1:-/dev/ttyUSB0}
esptool.py --chip esp32s3 --port $PORT --baud 921600 \
  write_flash -z 0x0 bootloader.bin \
  0x8000 partition-table.bin \
  0x10000 mdk-predator.bin
FLASH_EOF
chmod +x /workspaces/mdk-predator/firmware-binaries/flash.sh

# Package
cd /workspaces/mdk-predator
tar -czf MDK-PREDATOR-ESP32S3-FIRMWARE-BINARIES-v1.0.tar.gz firmware-binaries/

echo ""
echo "🎉 BUILD COMPLETE!"
echo "=================="
echo ""
echo "📦 Binary package: MDK-PREDATOR-ESP32S3-FIRMWARE-BINARIES-v1.0.tar.gz"
echo ""
echo "📥 Flash to ESP32-S3:"
echo "   cd firmware-binaries"
echo "   ./flash.sh /dev/ttyUSB0"
echo ""
