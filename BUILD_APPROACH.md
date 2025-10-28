# MDK-Predator Build Approach Guide

This document clarifies the build process for MDK-Predator and explains whether you need to build the entire PortaPack firmware or just the standalone application.

## TL;DR - Quick Answer

**You only need to build the external application, NOT the entire PortaPack firmware.**

The build scripts (`build_portapack_app.sh`, `build_portapack_app.ps1`, `build_portapack_app_wsl.sh`) are configured to:
1. Integrate MDK-Predator as an external application
2. Build the application (which includes external apps via `make application` target)
3. Generate a standalone `.ppma` file that works with existing Mayhem firmware

## Build Options Comparison

### Option 1: Standalone External App Build (RECOMMENDED)

**What it builds:** Only the `mdk_predator.ppma` file

**Advantages:**
- ✅ Much faster (5-10 minutes vs 30-60 minutes)
- ✅ Smaller download (no need to clone full firmware history)
- ✅ Works with any compatible Mayhem firmware version already on your device
- ✅ Easy to update - just replace the .ppma file
- ✅ No risk of bricking device with custom firmware

**How to build:**
```bash
# Linux/macOS
./scripts/build_portapack_app.sh --install-deps --download-firmware

# Windows PowerShell
.\scripts\build_portapack_app.ps1 -InstallDeps -DownloadFirmware

# WSL Debian/Ubuntu
./scripts/build_portapack_app_wsl.sh --install-deps --download-firmware
```

**What happens:**
1. Downloads Mayhem firmware source (needed for headers and build system)
2. Integrates MDK-Predator into `firmware/application/external/mdk_predator/`
3. Runs `make application` to build the application (which includes external apps)
4. Outputs `mdk_predator.ppma` file to `build/portapack/`

**Installation:**
1. Copy `mdk_predator.ppma` to SD card `/APPS/` directory
2. Insert SD card into PortaPack
3. Launch from Apps menu
4. Your existing Mayhem firmware remains unchanged

### Option 2: Full Firmware Build (NOT RECOMMENDED for most users)

**What it builds:** Complete PortaPack firmware image + MDK-Predator embedded

**When to use:**
- You want MDK-Predator permanently integrated into firmware
- You're developing custom firmware modifications
- You need to modify core firmware behavior

**Disadvantages:**
- ❌ Much slower build time (30-60 minutes)
- ❌ Requires flashing firmware (risk of bricking)
- ❌ More complex deployment
- ❌ Harder to update MDK-Predator separately
- ❌ Requires DFU mode and firmware flashing tools

**How to build:**
```bash
cd /path/to/mayhem-firmware
# First, integrate MDK-Predator as shown in Option 1
# Then build full firmware instead of just external apps:
mkdir build && cd build
cmake ..
make firmware  # This builds the entire firmware
```

**Installation:**
1. Flash firmware to device using DFU mode
2. Risk of bricking if flash fails
3. Loses existing settings and configurations

## Why External App is Better

### Technical Reasons

1. **Modularity**: External apps are loaded dynamically, keeping the core firmware stable
2. **Safety**: No risk of firmware corruption or device bricking
3. **Flexibility**: Can update MDK-Predator without touching firmware
4. **Compatibility**: Works with official Mayhem releases and updates

### PortaPack External App Architecture

PortaPack Mayhem supports external applications through the `.ppma` (PortaPack Mayhem App) format:

```
SD Card Structure:
/
├── APPS/
│   ├── mdk_predator.ppma      ← Your external app
│   ├── other_app.ppma
│   └── ...
├── MDK-PREDATOR/
│   └── config/
│       └── mdk_predator.conf  ← Configuration file
└── ...
```

When you launch an app:
1. Firmware loads `.ppma` file from SD card
2. App runs in isolated memory space
3. App uses firmware's APIs for hardware access
4. When app exits, memory is freed

This is similar to how Android apps work on top of Android OS.

## Build System Details

### What the Build Scripts Do

All three build scripts (`build_portapack_app.sh`, `build_portapack_app.ps1`, `build_portapack_app_wsl.sh`) follow the same process:

1. **Download Mayhem Firmware** (if needed)
   - Clones from: https://github.com/portapack-mayhem/mayhem-firmware
   - Only needed for build headers and CMake configuration
   - Shallow clone (--depth 1) for faster download

2. **Integrate MDK-Predator**
   - Copies files to `firmware/application/external/mdk_predator/`:
     - `app/*` (application wrapper including main.cpp)
     - `src/*` (core implementation)
     - `include/*` (headers)
     - `mdk_predator.conf` (configuration)
   - **Registers MDK-Predator in `external/external.cmake`:**
     - Adds source files to `EXTCPPSRC` variable
     - Adds app name to `EXTAPPLIST` variable
     - This step is **critical** - without it, the build system won't compile MDK-Predator

3. **Configure with CMake**
   - Sets up build environment
   - Detects ARM toolchain
   - Configures for external apps only

4. **Build Application**
   - Runs `make application` target
   - Compiles the application including external apps
   - Generates `.ppma` files

5. **Copy Output**
   - Extracts `mdk_predator.ppma` from build directory
   - Copies to `build/portapack/` with config and README

### Build Targets Explained

The Mayhem firmware build system has multiple targets:

- `make firmware` - Builds entire firmware (kernel + all apps)
- `make application` - Builds the application (which includes external apps) → **USED BY OUR SCRIPTS**
- `make clean` - Cleans build artifacts

Our scripts specifically use `make application` to build the application with external apps.

## Integration Verification

### Required Files for External App

MDK-Predator includes all necessary files for external app integration:

✅ **Application Wrapper** (`app/`)
- `main.cpp` - External app entry point (required by CMake build system)
- `mdk_predator_app.cpp` - Main application implementation
- `mdk_predator_app.hpp` - UI definitions and class declarations
- `app_build.mk` - Build configuration for legacy Makefile system
- `manifest.json` - Application metadata

✅ **Core Implementation** (`src/`)
- `mdk_predator.c` - Core library
- `automotive/*.c` - Automotive security modules
- `wireless/*.c` - Wireless analysis modules
- `crypto/*.c` - Cryptographic analysis

✅ **Headers** (`include/`)
- All necessary header files for compilation

✅ **Configuration** 
- `mdk_predator.conf` - Runtime configuration

### CMake Registration

The build scripts automatically register MDK-Predator in the Mayhem firmware's `external/external.cmake` file by adding:

**Source files to `EXTCPPSRC`:**
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

**App name to `EXTAPPLIST`:**
```cmake
mdk_predator
```

This registration is **essential** - without it, the CMake build system won't compile MDK-Predator.

### File Verification

**NEW:** The build scripts now automatically verify all files are properly linked before building:

1. **File Existence Check**: Verifies all 20+ source, header, and configuration files are copied
2. **CMake Registration Check**: Confirms all files are registered in `external.cmake`
3. **Structure Validation**: Ensures directory structure is intact
4. **Early Failure**: Stops build immediately if any file is missing or not linked

This verification step prevents cryptic build errors and ensures a successful build.

### Build Configuration Verification (Legacy)

The `app_build.mk` file is kept for compatibility with older Makefile-based builds:

```makefile
app_mdk_predator_SOURCES = \
    mdk_predator_app.cpp \
    ../src/mdk_predator.c \
    ../src/automotive/*.c \
    ../src/wireless/*.c \
    ../src/crypto/*.c

app_mdk_predator_INCLUDES = \
    ../include
```

This tells the Mayhem build system:
- Which files to compile
- Where to find headers
- C/C++ compiler flags to use

## Platform-Specific Build Notes

### Linux / macOS
- Uses `build_portapack_app.sh`
- Native ARM toolchain support
- Fastest build times
- Most reliable

### Windows (Native)
- Uses `build_portapack_app.ps1`
- Requires Chocolatey for dependencies
- May have issues with make/CMake
- Improved with cmake --build fallback
- **Recommendation: Use WSL for better compatibility**

### Windows (WSL)
- Uses `build_portapack_app_wsl.sh`
- Best Windows experience
- Native Linux environment
- Full toolchain compatibility
- Can access Windows filesystem via `/mnt/c/`

### WSL Setup (Recommended for Windows)
```powershell
# In Windows PowerShell (as Administrator)
wsl --install

# Install Ubuntu from Microsoft Store

# In WSL Ubuntu terminal:
cd /mnt/c/path/to/mdk-predator
./scripts/build_portapack_app_wsl.sh -i -d
```

## Troubleshooting Build Issues

### Build fails with "arm-none-eabi-gcc not found"
**Solution:** Install ARM toolchain
```bash
# Linux
sudo apt-get install gcc-arm-none-eabi

# Windows
choco install gcc-arm-embedded

# WSL
sudo apt-get install gcc-arm-none-eabi
```

### Build fails with "make: command not found"
**Solution:** Install make
```bash
# Linux
sudo apt-get install make

# Windows
choco install make

# WSL
sudo apt-get install make
```

### CMake configuration fails
**Solution:** Ensure CMake 3.16+ is installed
```bash
cmake --version  # Should show 3.16 or higher
```

### .ppma file not generated
**Possible causes:**
1. Build actually failed - check output for errors
2. Looking in wrong location - scripts search multiple paths
3. Integration failed - verify files copied correctly

**Solution:** Run with clean flag
```bash
./scripts/build_portapack_app.sh -c -m /path/to/mayhem
```

## Summary

| Aspect | External App Build | Full Firmware Build |
|--------|-------------------|---------------------|
| **Build Time** | 5-10 minutes | 30-60 minutes |
| **Output Size** | ~500 KB | ~2 MB |
| **Installation** | Copy to SD card | Flash firmware |
| **Risk** | None | Device bricking |
| **Updates** | Easy | Complex |
| **Recommended** | ✅ YES | ❌ NO (for most users) |

**Bottom Line:** Use the provided build scripts to create the external application. You do NOT need to build the entire firmware. The scripts are designed to build only what's necessary for a standalone `.ppma` file.

## References

- **Build Scripts:**
  - Linux/macOS: `scripts/build_portapack_app.sh`
  - Windows PowerShell: `scripts/build_portapack_app.ps1`
  - WSL Debian: `scripts/build_portapack_app_wsl.sh`

- **Documentation:**
  - [README.md](README.md) - Main documentation
  - [BUILD_EXAMPLES.md](BUILD_EXAMPLES.md) - Build script usage examples
  - [docs/BUILDING_WINDOWS.md](docs/BUILDING_WINDOWS.md) - Windows-specific guide
  - [HOW_TO_DEPLOY.md](HOW_TO_DEPLOY.md) - Deployment instructions

- **External Resources:**
  - [PortaPack Mayhem GitHub](https://github.com/portapack-mayhem/mayhem-firmware)
  - [PortaPack Wiki](https://github.com/portapack-mayhem/mayhem-firmware/wiki)
