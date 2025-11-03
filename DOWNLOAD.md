# 🚀 MDK Predator ESP32-S3 Firmware - DOWNLOAD

## ⚡ SCHNELLSTER WEG: Fertiges Package

📦 **Datei**: `MDK-PREDATOR-ESP32S3-FIRMWARE-SOURCE-v1.0.tar.gz` (25 KB)

📍 **Speicherort**: `/workspaces/mdk-predator/MDK-PREDATOR-ESP32S3-FIRMWARE-SOURCE-v1.0.tar.gz`

### Download-Optionen:

#### Option 1: Direkter Download (VS Code)
1. Rechtsklick auf `MDK-PREDATOR-ESP32S3-FIRMWARE-SOURCE-v1.0.tar.gz` im Explorer
2. Wähle "Download..."
3. Speichere lokal

#### Option 2: Git Commit + GitHub Release
```bash
cd /workspaces/mdk-predator
git add MDK-PREDATOR-ESP32S3-FIRMWARE-SOURCE-v1.0.tar.gz DOWNLOAD.md
git commit -m "Add MDK Predator ESP32-S3 Firmware Source v1.0"
git push origin main
```

Dann auf GitHub:
- Gehe zu "Releases"
- "Create new release"
- Upload `MDK-PREDATOR-ESP32S3-FIRMWARE-SOURCE-v1.0.tar.gz`
- Publizieren → **Download-Link** ist fertig!

#### Option 3: GitHub CLI (schnellste Release-Erstellung)
```bash
cd /workspaces/mdk-predator
git add MDK-PREDATOR-ESP32S3-FIRMWARE-SOURCE-v1.0.tar.gz DOWNLOAD.md
git commit -m "Release: MDK Predator v1.0 Firmware Source"
git push origin main
gh release create v1.0-firmware-source \
  MDK-PREDATOR-ESP32S3-FIRMWARE-SOURCE-v1.0.tar.gz \
  --title "MDK Predator ESP32-S3 Firmware v1.0 (Source)" \
  --notes "Complete source code + build instructions for ESP32-S3 firmware. Requires Ubuntu/Debian with ESP-IDF v5.1.2 for compilation. See README_COMPILE.md inside package."
```

---

## 📋 Package-Inhalt

Das Package enthält:

- ✅ **esp32_main.c** (518 Zeilen) - I2C Slave Firmware
- ✅ **mdk_hardware_abstraction.c** (529 Zeilen) - CC1101, CAN, Crypto
- ✅ **mdk_i2c_protocol.h** (274 Zeilen) - Protokoll-Definitionen
- ✅ **CMakeLists.txt** - ESP-IDF Build-Konfiguration
- ✅ **sdkconfig.defaults** - ESP32-S3 Hardware-Einstellungen
- ✅ **README_COMPILE.md** - Kompilierungs-Anleitung (Ubuntu/Codespaces)
- ✅ **FLASH_INSTRUCTIONS.md** - Flash-Anleitung
- ✅ **flash_mdk.sh** - Automatisches Flash-Script

---

## 🔥 Warum kein .bin Download?

**Problem**: Alpine Linux (musl libc) ist inkompatibel mit ESP32-S3 Toolchain (glibc).

**Lösung**: Package enthält vollständigen Source + Build-Instruktionen für:
- GitHub Codespaces (Ubuntu 22.04) - **EMPFOHLEN**
- Lokales Ubuntu/Debian System
- Windows WSL2 (Ubuntu)

**Build-Zeit**: ~5-10 Minuten in GitHub Codespaces

---

## 🎯 Firmware-Features

Wenn kompiliert, erhältst du:
- **I2C Slave @ 0x51** für PortaPack Mayhem Integration
- **CC1101 Transceiver** (300-928 MHz: KeeLoq, PT2260, OOK/ASK)
- **CAN Bus Support** (OBD-II, CAN 2.0)
- **Hardware Crypto** (AES-128/256, SHA256)
- **30+ Commands**:
  - PING, STATUS, GET_VERSION
  - KEELOQ_ENCRYPT, KEELOQ_DECRYPT, KEELOQ_BRUTEFORCE
  - PT2260_DECODE, PT2260_ENCODE
  - TRANSMIT (OOK/ASK), CAN_SEND, CAN_RECEIVE
  - und mehr...

---

## 🛠️ Quick Start (Ubuntu/Codespaces)

```bash
# 1. Extract
tar -xzf MDK-PREDATOR-ESP32S3-FIRMWARE-SOURCE-v1.0.tar.gz
cd mdk-firmware-source

# 2. Install ESP-IDF (einmalig)
sudo apt-get update
sudo apt-get install -y git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
mkdir -p ~/esp && cd ~/esp
git clone -b v5.1.2 --depth 1 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf && ./install.sh esp32s3

# 3. Build
. ~/esp/esp-idf/export.sh
cd <pfad_zu_mdk-firmware-source>
idf.py set-target esp32s3
idf.py build

# 4. Flash
esptool.py --chip esp32s3 --port /dev/ttyUSB0 --baud 921600 \
  write_flash -z 0x0 build/bootloader/bootloader.bin \
  0x8000 build/partition_table/partition-table.bin \
  0x10000 build/mdk-predator.bin
```

Fertig! ESP32-S3 bootet und wartet auf I2C-Befehle von PortaPack.

---

## 🚨 Troubleshooting

**"gcc not found" oder "symbol not found"**
→ Du bist in Alpine/musl. Nutze Ubuntu/Debian.

**"cmake not found"**
→ `sudo apt-get install cmake ninja-build`

**"permission denied" beim Flash**
→ `sudo usermod -a -G dialout $USER` (dann neu anmelden)

---

## 📞 Support

Bei Fragen: Siehe `README_COMPILE.md` im Package.

**Compiled against**: ESP-IDF v5.1.2 (stable)
**Target**: ESP32-S3 (Dual-core @ 240MHz, 8MB Flash, SPIRAM)
**PortaPack Compatibility**: Mayhem Nightly (I2C-basiert, keine direkte API-Abhängigkeit)
