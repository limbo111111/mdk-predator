# MDK-Predator PortaPack Application

This directory contains the PortaPack Mayhem application wrapper for MDK-Predator.

## Files

- **mdk_predator_app.cpp** - Main application implementation
- **mdk_predator_app.hpp** - Application header with UI definitions
- **manifest.json** - Application metadata for PortaPack
- **app_build.mk** - Build configuration for PortaPack integration
- **README.md** - This file

## Building the Application

### Prerequisites

1. PortaPack Mayhem firmware source code
2. ARM toolchain (arm-none-eabi-gcc)
3. MDK-Predator library built

### Integration Steps

1. Copy the MDK-Predator app files to your PortaPack Mayhem source:
   ```bash
   cp -r app/* /path/to/portapack-mayhem/firmware/application/external/mdk_predator/
   ```

2. Copy the MDK-Predator library and headers:
   ```bash
   cp -r src /path/to/portapack-mayhem/firmware/application/external/mdk_predator/
   cp -r include /path/to/portapack-mayhem/firmware/application/external/mdk_predator/
   ```

3. Build the PortaPack firmware with the external app:
   ```bash
   cd /path/to/portapack-mayhem
   mkdir build
   cd build
   cmake ..
   make firmware
   ```

### Output

The build process will generate:
- `mdk_predator.ppma` - PortaPack Mayhem Application file
- Ready to copy to SD card

## Features

The application provides a complete UI for all MDK-Predator modules:

### Main Menu
- Automotive Security
- WiFi Analysis
- Bluetooth Analysis  
- SubGHz RF
- Crypto Analysis

### Automotive Module
- Key Fob Analyzer (315/433.92/868/915 MHz)
- Rolling Code Tester (KeeLoq, HCS301)

### WiFi Module
- Network Scanner
- Security Analysis

### Bluetooth Module
- Device Scanner (Classic + BLE)
- Service Enumeration

### SubGHz Module
- Spectrum Scanner (300-928 MHz)
- Signal Capture
- Protocol Decoder

### Crypto Module
- Algorithm Identifier
- Entropy Analysis

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
