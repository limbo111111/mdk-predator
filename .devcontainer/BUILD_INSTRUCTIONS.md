# 🚀 MDK Predator ESP32-S3 Firmware - Build Instructions

**Package**: mdk-predator-firmware.tar.gz  
**Version**: 1.0.0  
**Date**: November 2, 2025  
**Target**: ESP32-S3 (240 MHz Dual-Core)

---

## 📦 Paketinhalt

```
mdk-predator-firmware/
├── CMakeLists.txt              # Projekt CMake
├── sdkconfig.defaults          # ESP32-S3 Konfiguration
└── main/
    ├── CMakeLists.txt          # Component CMake
    ├── esp32_main.c            # Hauptprogramm (517 Zeilen)
    ├── mdk_hardware_abstraction.c  # Hardware Layer (529 Zeilen)
    └── mdk_i2c_protocol.h      # I2C Protocol (274 Zeilen)
```

**Total**: 1.320 Zeilen produktionsbereiter Code

---

## 🔧 Build auf lokalem System (empfohlen)

### Voraussetzungen

- **ESP-IDF v5.0+** (getestet mit v6.1)
- **Linux, macOS oder Windows** (WSL)
- **USB-Kabel** für ESP32-S3

### Installation ESP-IDF

```bash
# Clone ESP-IDF
cd ~
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf

# Installiere für ESP32-S3
./install.sh esp32s3

# Aktiviere Environment
. ./export.sh
```

### Build Firmware

```bash
# Extrahiere Paket
tar -xzf mdk-predator-firmware.tar.gz
cd mdk-predator-firmware

# Set Target
idf.py set-target esp32s3

# Build
idf.py build

# Ergebnis:
# build/mdk-predator.bin  (~800 KB)
```

### Flash auf MDK Module

```bash
# MDK Modul vom PortaPack trennen!
# USB-C Kabel an MDK Module anschließen

# Flash
idf.py -p /dev/ttyUSB0 flash

# Monitor (optional)
idf.py -p /dev/ttyUSB0 monitor

# Erwartete Ausgabe:
# I (123) MDK_MAIN: MDK Predator ESP32-S3 Module Starting...
# I (125) MDK_MAIN: I2C slave initialized on address 0x51
# I (127) MDK_HAL: ✅ Hardware abstraction layer initialized
# I (129) MDK_MAIN: MDK Predator module ready!
```

---

## 🐧 Alternative: Build in Alpine Container

Falls du im Alpine-Container builden möchtest (langsamer, aber möglich):

```bash
# In devcontainer
cd /tmp
tar -xzf mdk-predator-firmware.tar.gz
cd mdk-predator-firmware

# ESP-IDF setup (einmalig)
apk add --no-cache cmake ninja ccache gcompat libstdc++ linux-headers
cd ~
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32s3
. ./export.sh

# Build
cd /tmp/mdk-predator-firmware
idf.py set-target esp32s3
idf.py build
```

**Hinweis**: Build im Alpine-Container kann 10-20 Minuten dauern!

---

## 📥 Fertige Binary (falls Build-Probleme)

Falls der Build nicht funktioniert, kannst du auch die vorkompilierte Binary nutzen:

1. Download von: https://github.com/[dein-repo]/releases/latest
2. Flash direkt: `esptool.py -p /dev/ttyUSB0 write_flash 0x0 mdk-predator.bin`

---

## ✅ Verification

Nach dem Flashen:

1. **USB-C vom MDK trennen**
2. **MDK auf PortaPack H4M stecken**
3. **PortaPack einschalten**
4. **I2C Scan durchführen**:
   ```
   PortaPack → Settings → Hardware → I2C Scan
   ```
   Sollte zeigen: **0x51** ✅

5. **MDK Predator App starten**:
   ```
   Apps → Utilities → MDK-Predator
   ```
   Status: **MDK: Connected ✓**

---

## 🎯 Integration mit Mayhem Firmware

Die Firmware ist kompatibel mit **Mayhem Firmware Nightly (November 2025)**.

### I2C Communication Example (für PortaPack-App):

```cpp
#include "portapack_i2c.hpp"

#define MDK_I2C_ADDR 0x51

// Ping MDK Module
bool mdk_ping() {
    uint8_t cmd = 0x01;  // MDK_CMD_PING
    uint8_t status = 0;
    
    portapack::i2c.write(MDK_I2C_ADDR, &cmd, 1);
    portapack::i2c.read(MDK_I2C_ADDR, &status, 1);
    
    return (status == 0x00);  // MDK_STATUS_READY
}

// Get Firmware Version
uint32_t mdk_get_version() {
    uint8_t cmd = 0x02;  // MDK_CMD_GET_VERSION
    uint8_t response[4];
    
    portapack::i2c.write(MDK_I2C_ADDR, &cmd, 1);
    portapack::i2c.read(MDK_I2C_ADDR, response, 4);
    
    return (response[0] << 16) | (response[1] << 8) | response[2];
}

// KeeLoq Decrypt
uint32_t mdk_keeloq_decrypt(uint32_t encrypted, uint64_t key) {
    uint8_t cmd[13] = {0x11};  // MDK_CMD_KEELOQ_DECRYPT
    
    // Pack encrypted value
    cmd[1] = (encrypted >> 24) & 0xFF;
    cmd[2] = (encrypted >> 16) & 0xFF;
    cmd[3] = (encrypted >> 8) & 0xFF;
    cmd[4] = encrypted & 0xFF;
    
    // Pack key
    for (int i = 0; i < 8; i++) {
        cmd[5 + i] = (key >> (56 - i*8)) & 0xFF;
    }
    
    // Send command
    portapack::i2c.write(MDK_I2C_ADDR, cmd, 13);
    
    // Wait for result
    chThdSleepMilliseconds(10);
    
    // Get result
    uint8_t result_cmd = 0x70;  // MDK_CMD_GET_RESULT
    uint8_t result[9];
    portapack::i2c.write(MDK_I2C_ADDR, &result_cmd, 1);
    portapack::i2c.read(MDK_I2C_ADDR, result, 9);
    
    uint32_t decrypted = (result[5] << 24) | (result[6] << 16) | 
                         (result[7] << 8) | result[8];
    
    return decrypted;
}
```

---

## 🔍 Troubleshooting

### Problem: "No device found"

**Lösung**:
- Prüfe USB-Kabel (muss Daten unterstützen)
- Drücke BOOT + RESET auf MDK Module
- Versuche anderen USB-Port

### Problem: "Permission denied /dev/ttyUSB0"

**Lösung**:
```bash
sudo usermod -a -G dialout $USER
# Logout und wieder einloggen
```

### Problem: "I2C device 0x51 not found"

**Lösung**:
1. MDK Power LED leuchtet? (Wenn nicht: schlechter Kontakt)
2. USB-C vom MDK trennen (nicht gleichzeitig USB + PortaPack!)
3. Serial Monitor checken:
   ```bash
   idf.py monitor
   # Sollte zeigen: "I2C slave initialized on address 0x51"
   ```

---

## 📚 Weitere Dokumentation

- **HARDWARE_SETUP_H4M_MDK.md** - Hardware Details
- **ESP32_INTEGRATION_GUIDE.md** - Integration Guide
- **QUICK_REFERENCE.md** - Code-Beispiele
- **CODE_COMPLETENESS_REPORT.md** - Validierungsbericht

---

## 🎉 Ready to Flash!

**Download**: `/tmp/mdk-predator-firmware.tar.gz` (715 KB)

```bash
# Transfer auf lokales System:
scp user@devcontainer:/tmp/mdk-predator-firmware.tar.gz .

# Oder via VS Code:
# Rechtsklick auf /tmp/mdk-predator-firmware.tar.gz → Download
```

**Viel Erfolg! 🚀**
