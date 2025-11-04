# MDK Predator ESP32-S3 Firmware - Kompilierung

## Warum muss ich selbst kompilieren?

Die ESP32-S3 Toolchain benötigt **glibc** (GNU C Library). Dieses Repository läuft in einem **Alpine Linux Container** (musl libc), was inkompatibel ist.

## Schnellste Methode: GitHub Codespaces (Ubuntu)

1. Fork dieses Repo
2. Öffne es in GitHub Codespaces (automatisch Ubuntu 22.04)
3. Führe aus:
```bash
sudo apt-get update
sudo apt-get install -y git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
mkdir -p ~/esp && cd ~/esp
git clone -b v5.1.2 --depth 1 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf && ./install.sh esp32s3
. ./export.sh
cd /workspaces/mdk-predator/mdk-firmware-source
mkdir build && cd build
cmake .. -G Ninja
idf.py set-target esp32s3
idf.py build
```

4. Binaries sind in: `build/mdk-predator.bin`, `build/bootloader/bootloader.bin`, `build/partition_table/partition-table.bin`

## Alternative: Lokales Ubuntu/Debian System

Falls du Ubuntu/Debian lokal hast:
```bash
sudo apt-get install git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
mkdir -p ~/esp && cd ~/esp
git clone -b v5.1.2 --depth 1 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf && ./install.sh esp32s3
. ./export.sh
cd <pfad_zu_diesem_ordner>
idf.py set-target esp32s3
idf.py build
```

## Flashen

Nach dem Build:
```bash
esptool.py --chip esp32s3 --port /dev/ttyUSB0 --baud 921600 \
  write_flash -z 0x0 build/bootloader/bootloader.bin \
  0x8000 build/partition_table/partition-table.bin \
  0x10000 build/mdk-predator.bin
```

## Firmware-Funktionen

- **I2C Slave @ 0x51** - Kommuniziert mit PortaPack Mayhem
- **CC1101** - 300-928 MHz Transceiver (KeeLoq, PT2260, OOK/ASK)
- **CAN Bus** - OBD-II & CAN 2.0
- **Hardware Crypto** - AES, SHA256 via mbedtls
- **30+ Commands** - PING, KEELOQ_ENCRYPT/DECRYPT/BRUTEFORCE, PT2260_DECODE/ENCODE, TRANSMIT, etc.

Für Details: siehe `FLASH_INSTRUCTIONS.md`
