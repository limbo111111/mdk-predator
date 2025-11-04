# 🚀 GitHub Release erstellen - ANLEITUNG

## Option 1: GitHub Web Interface (einfachste Methode)

1. **Gehe zu**: https://github.com/limbo111111/mdk-predator/releases/new

2. **Tag**: `v1.0-firmware-source`

3. **Title**: `MDK Predator ESP32-S3 Firmware v1.0 (Source)`

4. **Upload Binary**: 
   - Klicke auf "Attach binaries by dropping them here or selecting them"
   - Wähle: `MDK-PREDATOR-ESP32S3-FIRMWARE-SOURCE-v1.0.tar.gz` (25 KB)

5. **Release Notes** (kopiere folgenden Text):

```markdown
# MDK Predator ESP32-S3 Firmware v1.0 - Complete Source Package

## 🚀 Ready-to-Build Firmware Package

This release contains the **complete ESP32-S3 firmware source code** for the MDK Predator I2C slave module, designed for **PortaPack Mayhem** integration.

---

## 📦 Download & Extract

```bash
wget https://github.com/limbo111111/mdk-predator/releases/download/v1.0-firmware-source/MDK-PREDATOR-ESP32S3-FIRMWARE-SOURCE-v1.0.tar.gz
tar -xzf MDK-PREDATOR-ESP32S3-FIRMWARE-SOURCE-v1.0.tar.gz
cd mdk-firmware-source
```

---

## 🔧 Quick Build (Ubuntu/Debian/Codespaces)

```bash
# Install ESP-IDF v5.1.2 (one-time setup)
sudo apt-get update
sudo apt-get install -y git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
mkdir -p ~/esp && cd ~/esp
git clone -b v5.1.2 --depth 1 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf && ./install.sh esp32s3

# Build firmware
. ~/esp/esp-idf/export.sh
cd <path_to_mdk-firmware-source>
idf.py set-target esp32s3
idf.py build
```

**Build outputs**:
- `build/mdk-predator.bin` (main firmware)
- `build/bootloader/bootloader.bin`
- `build/partition_table/partition-table.bin`

---

## ⚡ Flash to ESP32-S3

```bash
esptool.py --chip esp32s3 --port /dev/ttyUSB0 --baud 921600 \
  write_flash -z 0x0 build/bootloader/bootloader.bin \
  0x8000 build/partition_table/partition-table.bin \
  0x10000 build/mdk-predator.bin
```

---

## 🎯 Firmware Features

- **I2C Slave @ 0x51** - Seamless PortaPack Mayhem integration
- **CC1101 Transceiver** - 300-928 MHz (KeeLoq, PT2260, OOK/ASK)
- **CAN Bus** - OBD-II, CAN 2.0 support
- **Hardware Crypto** - AES-128/256, SHA256 (mbedtls)
- **30+ Commands**:
  - `PING`, `STATUS`, `GET_VERSION`
  - `KEELOQ_ENCRYPT`, `KEELOQ_DECRYPT`, `KEELOQ_BRUTEFORCE`
  - `PT2260_DECODE`, `PT2260_ENCODE`
  - `TRANSMIT` (OOK/ASK), `CAN_SEND`, `CAN_RECEIVE`
  - And more...

---

## 📋 Package Contents

- `esp32_main.c` (518 lines) - I2C slave firmware
- `mdk_hardware_abstraction.c` (529 lines) - CC1101, CAN, crypto drivers
- `mdk_i2c_protocol.h` (274 lines) - Protocol definitions
- `CMakeLists.txt` - ESP-IDF build configuration
- `sdkconfig.defaults` - ESP32-S3 hardware settings
- `README_COMPILE.md` - Compilation guide
- `FLASH_INSTRUCTIONS.md` - Flashing instructions
- `flash_mdk.sh` - Automated flash script

---

## 🔥 Why Source-Only Release?

**Alpine Linux (musl libc)** used in the devcontainer is **incompatible** with the ESP32-S3 toolchain (glibc). This package provides:
- ✅ Complete source code
- ✅ Build instructions for Ubuntu/Debian
- ✅ GitHub Codespaces support (recommended)
- ✅ ~5-10 minute build time

---

## 🛠️ Hardware Requirements

- **ESP32-S3** (Dual-core @ 240MHz, 8MB Flash, SPIRAM)
- **CC1101** transceiver (optional)
- **CAN transceiver** (optional)
- **I2C connection** to PortaPack (SDA, SCL, GND, 3.3V)

---

## 📚 Documentation

Full documentation in package:
- `README_COMPILE.md` - Build guide
- `FLASH_INSTRUCTIONS.md` - Flash + hardware setup
- Source code comments (1,320 lines total)

---

## ⚙️ Technical Specs

- **ESP-IDF**: v5.1.2 (stable)
- **Target**: ESP32-S3
- **I2C Address**: 0x51 (slave mode)
- **PortaPack Compatibility**: Mayhem Nightly (I2C-based, no direct API dependency)

---

## 🚀 Getting Started

1. Download `MDK-PREDATOR-ESP32S3-FIRMWARE-SOURCE-v1.0.tar.gz`
2. Extract and follow `README_COMPILE.md`
3. Build in Ubuntu/Codespaces (~5-10 min)
4. Flash to ESP32-S3
5. Connect to PortaPack via I2C
6. Done! 🎉

---

**Questions?** Check `README_COMPILE.md` or open an issue.
```

6. **Publish**: Klicke auf "Publish release"

---

## Option 2: GitHub CLI (falls `gh` installiert wird)

```bash
# Installiere gh (falls nicht vorhanden)
sudo apt-get install gh

# Login
gh auth login

# Release erstellen
cd /workspaces/mdk-predator
gh release create v1.0-firmware-source \
  MDK-PREDATOR-ESP32S3-FIRMWARE-SOURCE-v1.0.tar.gz \
  --title "MDK Predator ESP32-S3 Firmware v1.0 (Source)" \
  --notes-file RELEASE_NOTES.md
```

---

## ✅ Nach dem Release

**Dein Download-Link ist**:
```
https://github.com/limbo111111/mdk-predator/releases/download/v1.0-firmware-source/MDK-PREDATOR-ESP32S3-FIRMWARE-SOURCE-v1.0.tar.gz
```

**Direct Link zum Release**:
```
https://github.com/limbo111111/mdk-predator/releases/tag/v1.0-firmware-source
```

---

## 🎉 Fertig!

User können jetzt direkt downloaden:
```bash
wget https://github.com/limbo111111/mdk-predator/releases/download/v1.0-firmware-source/MDK-PREDATOR-ESP32S3-FIRMWARE-SOURCE-v1.0.tar.gz
```
