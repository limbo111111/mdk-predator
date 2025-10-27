# MDK-Predator PortaPack Application

This directory contains the PortaPack Mayhem application wrapper for MDK-Predator.

## Files

- **main.cpp** - External app entry point (required by Mayhem firmware)
- **mdk_predator_app.cpp** - Main application implementation
- **mdk_predator_app.hpp** - Application header with UI definitions
- **manifest.json** - Application metadata for PortaPack
- **app_build.mk** - Build configuration for PortaPack integration (legacy)
- **README.md** - This file

## Building the Application

### Automatic Build (Recommended)

Use the provided build scripts which handle all integration automatically:

```bash
# Linux/macOS
./scripts/build_portapack_app.sh --install-deps --download-firmware

# Windows PowerShell
.\scripts\build_portapack_app.ps1 -InstallDeps -DownloadFirmware

# WSL Debian/Ubuntu
./scripts/build_portapack_app_wsl.sh --install-deps --download-firmware
```

The scripts automatically:
1. Download the Mayhem firmware (if needed)
2. Copy all MDK-Predator files to the correct location
3. **Register MDK-Predator in the external.cmake file**
4. Build only the external application (fast)
5. Generate the `mdk_predator.ppma` file

### Manual Integration (Advanced)

If you prefer to integrate manually:

1. Copy the MDK-Predator app files to your PortaPack Mayhem source:
   ```bash
   cp -r app/* /path/to/mayhem-firmware/firmware/application/external/mdk_predator/
   ```

2. Copy the MDK-Predator library and headers:
   ```bash
   cp -r src /path/to/mayhem-firmware/firmware/application/external/mdk_predator/
   cp -r include /path/to/mayhem-firmware/firmware/application/external/mdk_predator/
   ```

3. **Register MDK-Predator in external.cmake:**
   
   Edit `/path/to/mayhem-firmware/firmware/application/external/external.cmake`:
   
   Add to `EXTCPPSRC`:
   ```cmake
   #mdk_predator
   external/mdk_predator/main.cpp
   external/mdk_predator/mdk_predator_app.cpp
   external/mdk_predator/../src/mdk_predator.c
   external/mdk_predator/../src/automotive/key_fob_analyzer.c
   external/mdk_predator/../src/automotive/rolling_code_tester.c
   external/mdk_predator/../src/wireless/wifi_analyzer.c
   external/mdk_predator/../src/wireless/bluetooth_analyzer.c
   external/mdk_predator/../src/wireless/subghz_analyzer.c
   external/mdk_predator/../src/crypto/crypto_analyzer.c
   ```
   
   Add to `EXTAPPLIST`:
   ```cmake
   mdk_predator
   ```

4. Build the external apps:
   ```bash
   cd /path/to/mayhem-firmware
   mkdir build
   cd build
   cmake ..
   make external_apps
   ```

### Output

The build process will generate:
- `mdk_predator.ppma` - PortaPack Mayhem Application file
- Ready to copy to SD card

## Features

The application provides a complete UI for all MDK-Predator modules with interactive functionality:

### Main Menu
- Automotive Security
- WiFi Analysis
- Bluetooth Analysis
- SubGHz RF
- Crypto Analysis
- Real-time status display
- Interactive console output

### Automotive Module
- **Key Fob Analyzer** (315/433.92/868/915 MHz)
  - Start/Stop capture toggle
  - Real-time signal count display
  - Frequency and bandwidth information
  - Detailed signal capture log
- **Rolling Code Tester** (KeeLoq, HCS301)
  - Pattern analysis with detailed output
  - Code sequence display
  - Replay protection detection

### WiFi Module
- **Network Scanner**
  - Start/Stop scan toggle
  - Networks found counter
  - SSID, channel, signal strength display
  - Security protocol identification
- **Security Analysis**
  - Protocol strength assessment
  - Vulnerability detection

### Bluetooth Module
- **Device Scanner** (Classic + BLE)
  - Start/Stop scan toggle
  - Devices found counter
  - Device name, type, and signal strength
  - MAC address display
- **Service Enumeration**
  - GATT service discovery
  - Characteristic listing

### SubGHz Module
- **Spectrum Scanner** (300-928 MHz)
  - Start/Stop scan toggle
  - Frequency range display
  - Signal activity visualization
  - Power level measurements
- **Signal Capture**
  - Real-time capture with details
  - Pulse count and duration
  - Signal counter tracking
- **Protocol Decoder**
  - ASK/OOK protocol support
  - Pulse width analysis

### Crypto Module
- **Algorithm Identifier**
  - Block cipher detection
  - Key schedule analysis
  - Algorithm type display
- **Entropy Analysis**
  - Randomness quality testing
  - Chi-square statistical tests
  - Bits per byte measurement
  - Quality rating

## Usage

1. Copy `mdk_predator.ppma` to your SD card in the `/APPS` directory
2. Insert SD card into PortaPack
3. Navigate to Apps menu on PortaPack
4. Select "MDK-Predator"
5. Choose desired security module

## Safety

⚠️ **Important**:
- Transmission is disabled by default
- Only for authorized security research
- Comply with local RF regulations
- Never test unauthorized systems

## Support

For issues or questions:
- GitHub: https://github.com/limbo111111/mdk-predator
- Documentation: See main README.md
