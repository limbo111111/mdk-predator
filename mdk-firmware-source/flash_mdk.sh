#!/bin/bash
# MDK Predator - Automated Flash Script for ESP32-S3
# Compatible with PortaPack H4M + MDK Module

set -e

# Configuration
PORT="${1:-/dev/ttyUSB0}"
BAUD="${2:-921600}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}   MDK PREDATOR - ESP32-S3 Flash Script${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""
echo -e "${YELLOW}Configuration:${NC}"
echo -e "  Port: ${GREEN}$PORT${NC}"
echo -e "  Baud: ${GREEN}$BAUD${NC}"
echo ""

# Check if esptool is installed
if ! command -v esptool.py &> /dev/null; then
    echo -e "${RED}❌ Error: esptool.py not found!${NC}"
    echo ""
    echo -e "${YELLOW}Install with:${NC}"
    echo "  pip install esptool"
    echo ""
    exit 1
fi

# Check if port exists
if [ ! -e "$PORT" ]; then
    echo -e "${RED}❌ Error: Port $PORT not found!${NC}"
    echo ""
    echo -e "${YELLOW}Available serial ports:${NC}"
    ls -1 /dev/ttyUSB* /dev/ttyACM* 2>/dev/null || echo "  No USB serial devices found"
    echo ""
    echo -e "${YELLOW}Usage:${NC}"
    echo "  $0 [PORT] [BAUD]"
    echo ""
    echo -e "${YELLOW}Examples:${NC}"
    echo "  $0 /dev/ttyUSB0"
    echo "  $0 /dev/ttyACM0 921600"
    echo ""
    exit 1
fi

# Check if binary files exist
if [ ! -f "mdk-predator.bin" ] || [ ! -f "bootloader.bin" ] || [ ! -f "partition-table.bin" ]; then
    echo -e "${RED}❌ Error: Binary files not found!${NC}"
    echo ""
    echo -e "${YELLOW}Required files:${NC}"
    echo "  - mdk-predator.bin"
    echo "  - bootloader.bin"
    echo "  - partition-table.bin"
    echo ""
    echo -e "${YELLOW}Current directory:${NC}"
    ls -lh *.bin 2>/dev/null || echo "  No .bin files found"
    echo ""
    exit 1
fi

echo -e "${YELLOW}📋 Binary files:${NC}"
ls -lh mdk-predator.bin bootloader.bin partition-table.bin
echo ""

echo -e "${YELLOW}⚡ Erasing flash...${NC}"
esptool.py --chip esp32s3 --port "$PORT" --baud "$BAUD" erase_flash || {
    echo -e "${RED}❌ Erase failed!${NC}"
    exit 1
}

echo ""
echo -e "${YELLOW}📡 Flashing firmware...${NC}"
echo ""

esptool.py --chip esp32s3 \
    --port "$PORT" \
    --baud "$BAUD" \
    --before default_reset \
    --after hard_reset \
    write_flash --flash_mode dio --flash_freq 80m --flash_size 8MB \
    0x0 bootloader.bin \
    0x8000 partition-table.bin \
    0x10000 mdk-predator.bin || {
    echo ""
    echo -e "${RED}❌ Flash failed!${NC}"
    exit 1
}

echo ""
echo -e "${GREEN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${GREEN}✅ FLASH COMPLETE!${NC}"
echo -e "${GREEN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""
echo -e "${YELLOW}🔌 Next steps:${NC}"
echo -e "  1. Disconnect USB cable from ESP32-S3"
echo -e "  2. Connect to PortaPack H4M:"
echo -e "     ${BLUE}SDA${NC}: GPIO8 (ESP32-S3) ↔ SDA (PortaPack)"
echo -e "     ${BLUE}SCL${NC}: GPIO9 (ESP32-S3) ↔ SCL (PortaPack)"
echo -e "     ${BLUE}GND${NC}: GND (ESP32-S3) ↔ GND (PortaPack)"
echo -e "     ${BLUE}VCC${NC}: 3.3V (ESP32-S3) ↔ 3.3V (PortaPack)"
echo -e "  3. Power on PortaPack"
echo ""
echo -e "${YELLOW}📡 I2C Configuration:${NC}"
echo -e "  Address: ${GREEN}0x51${NC}"
echo -e "  Speed: ${GREEN}400 kHz${NC}"
echo ""
echo -e "${YELLOW}🧪 Test Connection:${NC}"
echo -e "  On PortaPack: Apps → Debug → I2C Scan"
echo -e "  Should detect device at ${GREEN}0x51${NC}"
echo ""
echo -e "${GREEN}🎯 MDK Predator ready for 500x faster KeeLoq bruteforce!${NC}"
echo ""
