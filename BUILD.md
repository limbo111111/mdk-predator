# MDK-Predator Build Instructions

MDK-Predator is an external application for PortaPack Mayhem firmware. This guide explains how to build it.

## What You're Building

MDK-Predator is an **external app** (`.ppma` file) that runs on top of the PortaPack Mayhem firmware. You don't need to build or flash the entire firmware - just copy the .ppma file to your SD card.

## Quick Start

### Using Docker (Easiest - No Dependencies!)

**Recommended for all users, especially Windows users.**

```bash
# Linux/macOS/Windows WSL
./docker-build.sh

# Windows PowerShell
.\docker-build.ps1

# Windows CMD
docker-build.bat
```

The built file will be in `build/docker-output/mdk_predator.ppma`.

**Requirements:** Only Docker Desktop (Windows/macOS) or Docker Engine (Linux)

### Using Simplified Build Scripts

These new simplified scripts handle everything automatically:

**Linux/macOS:**
```bash
# Download firmware and build
./build.sh -d

# Build with existing firmware
./build.sh -m /path/to/mayhem-firmware

# Clean build
./build.sh -c -d
```

**Windows PowerShell:**
```powershell
# Download firmware and build
.\build.ps1 -Download

# Build with existing firmware
.\build.ps1 -MayhemPath C:\path\to\mayhem-firmware

# Clean build
.\build.ps1 -Clean -Download
```

The built file will be in `build/output/mdk_predator.ppma`.

## What the Build Scripts Do

1. Download mayhem-firmware (if needed)
2. Copy MDK-Predator files to `firmware/application/external/mdk_predator/`
3. Register the app in `external.cmake`
4. Build libopencm3 (required dependency)
5. Build the application with `make application`
6. Extract the `.ppma` file

## Installation

1. Copy `mdk_predator.ppma` to your SD card's `/APPS/` directory
2. Insert SD card into PortaPack
3. Launch from Apps menu

That's it! Your PortaPack firmware remains unchanged.

## Manual Build (Advanced)

If you prefer full control:

1. Clone mayhem-firmware:
   ```bash
   git clone https://github.com/portapack-mayhem/mayhem-firmware.git
   cd mayhem-firmware
   git submodule update --init hackrf/firmware/libopencm3
   ```

2. Copy MDK-Predator files:
   ```bash
   cp -r /path/to/mdk-predator/app/* firmware/application/external/mdk_predator/
   cp -r /path/to/mdk-predator/src firmware/application/external/mdk_predator/
   cp -r /path/to/mdk-predator/include firmware/application/external/mdk_predator/
   ```

3. Register in `firmware/application/external/external.cmake`:
   
   Add to `EXTCPPSRC`:
   ```cmake
   #mdk_predator
   external/mdk_predator/main.cpp
   external/mdk_predator/mdk_predator_app.cpp
   external/mdk_predator/src/mdk_predator.c
   external/mdk_predator/src/automotive/key_fob_analyzer.c
   external/mdk_predator/src/automotive/rolling_code_tester.c
   external/mdk_predator/src/wireless/wifi_analyzer.c
   external/mdk_predator/src/wireless/bluetooth_analyzer.c
   external/mdk_predator/src/wireless/subghz_analyzer.c
   external/mdk_predator/src/crypto/crypto_analyzer.c
   ```
   
   Add to `EXTAPPLIST`:
   ```cmake
   mdk_predator
   ```

4. Build:
   ```bash
   cd hackrf/firmware/libopencm3
   make TARGETS="lpc43xx/m4 lpc43xx/m0"
   cd ../../../
   mkdir build && cd build
   cmake ..
   make application
   ```

5. Find `mdk_predator.ppma` in the build directory and copy to SD card.

## Requirements

- **For Docker build:** Docker only
- **For native build:**
  - PortaPack Mayhem firmware source
  - ARM toolchain (arm-none-eabi-gcc 9.2.1)
  - CMake 3.16+
  - Python 3.7+
  - Make

## MDK Module Integration

**Q: Is the MDK module integrated?**

**A:** Yes! MDK-Predator is fully integrated as a PortaPack external application. The "MDK module" refers to the Mayhem-MDK hardware addon board for the HackRF PortaPack. This software is designed specifically to run on that hardware.

The integration means:
- Proper external app structure with `main.cpp` entry point
- Correct `application_information_t` metadata for PortaPack
- All source files organized in the standard external app layout
- Ready to be registered in `external.cmake` and built with mayhem-firmware

## External App vs Standalone

**Q: Does this need to be an external app or standalone?**

**A:** This is correctly configured as an **external app** (not standalone). External apps are the recommended approach because:

✅ No firmware flashing required - just copy .ppma to SD card  
✅ Safe - no risk of bricking your device  
✅ Easy updates - just replace the .ppma file  
✅ Works with official Mayhem firmware releases  
✅ Your firmware settings remain intact  

Standalone firmware integration would require flashing and is not recommended for end users.

## Troubleshooting

**Build fails with "libopencm3/lpc43xx/m0/nvic.h: No such file"**
- The build scripts now automatically build libopencm3 first
- Make sure git submodules are initialized: `git submodule update --init`

**make: command not found (Windows)**
- Use Docker build instead (recommended)
- Or install make via Chocolatey: `choco install make`

**arm-none-eabi-gcc not found**
- Use Docker build (recommended)
- Or install ARM toolchain for your platform

## More Information

- [README.md](README.md) - Main project documentation
- [BUILD_APPROACH.md](BUILD_APPROACH.md) - Detailed build architecture
- [DOCKER_BUILD.md](DOCKER_BUILD.md) - Docker build documentation
- [Mayhem Firmware Wiki](https://github.com/portapack-mayhem/mayhem-firmware/wiki)
