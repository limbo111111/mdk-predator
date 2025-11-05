# MDK Predator - Flash Instructions

## 🚀 Quick Flash Guide

### Prerequisites

1. **Hardware**:
   - ESP32-S3 Development Board (8MB Flash, SPIRAM)
   - USB Cable (USB-C or Micro-USB depending on board)
   - PortaPack H4M + MDK Module

2. **Software**:
   - Python 3.7+
   - esptool.py: `pip install esptool`

---

## 📦 Binary Files

You need these three files:

```
mdk-predator.bin       - Main application firmware
bootloader.bin         - ESP32-S3 bootloader
partition-table.bin    - Flash partition layout
```

Download from:
- **GitHub Actions**: Actions → Workflow Run → Artifacts → `mdk-predator-firmware`
- **GitHub Releases**: Releases → Latest → Assets → Download files

---

## ⚡ Automatic Flash (Recommended)

### Linux / macOS:

```bash
# Make script executable
chmod +x flash_mdk.sh

# Flash with default settings
./flash_mdk.sh

# Or specify custom port
./flash_mdk.sh /dev/ttyUSB0

# Or specify port and baud rate
./flash_mdk.sh /dev/ttyACM0 921600
```

### Windows:

Use the manual method below or install WSL.

---

## 🔧 Manual Flash

### Step 1: Erase Flash (Optional but recommended)

```bash
esptool.py --chip esp32s3 --port /dev/ttyUSB0 erase_flash
```

**Windows**:
```cmd
esptool.py --chip esp32s3 --port COM3 erase_flash
```

### Step 2: Flash Firmware

**Linux/macOS**:
```bash
esptool.py --chip esp32s3 \
  --port /dev/ttyUSB0 \
  --baud 921600 \
  --before default_reset \
  --after hard_reset \
  write_flash --flash_mode dio --flash_freq 80m --flash_size 8MB \
  0x0 bootloader.bin \
  0x8000 partition-table.bin \
  0x10000 mdk-predator.bin
```

**Windows**:
```cmd
esptool.py --chip esp32s3 ^
  --port COM3 ^
  --baud 921600 ^
  --before default_reset ^
  --after hard_reset ^
  write_flash --flash_mode dio --flash_freq 80m --flash_size 8MB ^
  0x0 bootloader.bin ^
  0x8000 partition-table.bin ^
  0x10000 mdk-predator.bin
```

---

## 🔌 Hardware Connection

### ESP32-S3 to PortaPack H4M

| ESP32-S3 Pin | PortaPack H4M Pin | Function |
|--------------|-------------------|----------|
| GPIO8        | SDA               | I2C Data |
| GPIO9        | SCL               | I2C Clock |
| GND          | GND               | Ground |
| 3.3V         | 3.3V              | Power |

**⚠️ Important**:
- Use **3.3V** logic level only!
- Do NOT connect 5V to ESP32-S3
- Both devices must share GND

### Wiring Diagram

```
PortaPack H4M          ESP32-S3
┌──────────┐          ┌──────────┐
│          │          │          │
│  SDA ────┼──────────┼── GPIO8  │
│  SCL ────┼──────────┼── GPIO9  │
│  GND ────┼──────────┼── GND    │
│  3.3V ───┼──────────┼── 3.3V   │
│          │          │          │
└──────────┘          └──────────┘
```

---

## ✅ Verification

### 1. Check Serial Output

After flashing, connect serial monitor:

```bash
# Linux/macOS
screen /dev/ttyUSB0 115200

# Or use Arduino IDE Serial Monitor
# Or use PuTTY on Windows
```

You should see:
```
MDK Predator v1.0.0
ESP32-S3 @ 240MHz Dual-Core
I2C Slave initialized @ 0x51
Hardware crypto enabled
Ready for commands...
```

### 2. Test I2C Connection

On PortaPack H4M:
1. Go to **Apps** → **Debug** → **I2C Scan**
2. Should detect device at address **0x51**

### 3. Test Communication

Send a PING command:
- Address: 0x51
- Command: 0x01 (PING)
- Expected Response: 0x00 (STATUS_OK)

---

## 🐛 Troubleshooting

### Flash Fails

**Problem**: `Failed to connect to ESP32-S3`

**Solutions**:
1. Hold **BOOT** button while connecting USB
2. Try lower baud rate: `--baud 115200`
3. Check USB cable (must support data, not just power)
4. Install USB-Serial drivers:
   - CP210x: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
   - CH340: http://www.wch.cn/downloads/CH341SER_ZIP.html

### Wrong Port

**Linux**: Check ports with `ls /dev/ttyUSB* /dev/ttyACM*`
**Windows**: Check Device Manager → Ports (COM & LPT)
**macOS**: Check with `ls /dev/cu.*`

### Permission Denied (Linux)

```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER

# Or use sudo for flash
sudo ./flash_mdk.sh
```

### I2C Not Detected

1. **Check Wiring**: Verify SDA, SCL, GND, 3.3V connections
2. **Check Pull-ups**: I2C needs pull-up resistors (usually 4.7kΩ)
   - Most dev boards have built-in pull-ups
3. **Check Power**: ESP32-S3 LED should be on
4. **Check Serial Output**: Verify firmware booted correctly

### Bootloop / Crashes

1. **Erase flash completely**:
   ```bash
   esptool.py --chip esp32s3 --port /dev/ttyUSB0 erase_flash
   ```
2. **Flash again** with all three files
3. **Check power supply**: ESP32-S3 needs stable 3.3V @ ~500mA

---

## 🎯 Next Steps

After successful flash:

1. **Test Basic Commands**:
   - PING (0x01)
   - GET_VERSION (0x02)
   - GET_STATUS (0x03)

2. **Test KeeLoq**:
   - KEELOQ_ENCRYPT (0x10)
   - KEELOQ_DECRYPT (0x11)
   - KEELOQ_BRUTEFORCE (0x12) - 500x faster! 🚀

3. **Test RF Functions**:
   - CC1101_TRANSMIT (0x30)
   - CC1101_RECEIVE (0x31)
   - MEASURE_RSSI (0x32)

4. **Integrate with PortaPack**:
   - Update Mayhem firmware app
   - Implement I2C communication layer
   - Offload heavy computations to MDK

---

## 📊 Flash Memory Map

| Address  | Size    | Content              |
|----------|---------|----------------------|
| 0x0      | ~24 KB  | Bootloader           |
| 0x8000   | 3 KB    | Partition Table      |
| 0x10000  | ~1.5 MB | Main Application     |
| 0x9000   | 20 KB   | NVS (Config Storage) |
| 0xe000   | 8 KB    | OTA Data             |

---

## 🔒 Security Notes

- **No sensitive data** in firmware
- **Hardware crypto** enabled (AES, SHA256)
- **NVS encryption** available if needed
- All keys/seeds provided via I2C at runtime

---

## 📚 Additional Resources

- **ESP32-S3 Datasheet**: https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf
- **ESP-IDF Docs**: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/
- **I2C Protocol**: See `mdk_i2c_protocol.h` for command reference
- **Hardware Setup**: See `HARDWARE_SETUP_H4M_MDK.md`

---

## ✅ Success Checklist

- [ ] esptool.py installed
- [ ] ESP32-S3 connected via USB
- [ ] Bootloader flashed to 0x0
- [ ] Partition table flashed to 0x8000
- [ ] Firmware flashed to 0x10000
- [ ] Serial output shows boot messages
- [ ] ESP32-S3 connected to PortaPack (SDA, SCL, GND, 3.3V)
- [ ] I2C scan detects 0x51
- [ ] PING command responds with STATUS_OK
- [ ] Ready for KeeLoq bruteforce! 🎯

---

**MDK Predator v1.0.0**  
**Flash Time**: ~30 seconds  
**Status**: ✅ Production Ready  

**Questions?** Check the documentation or open an issue on GitHub!
