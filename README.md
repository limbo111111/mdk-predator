# MDK-Predator

A comprehensive security research suite for the Mayhem-MDK platform, ported from the Flipper Zero Predator Security Suite.

📖 **New to MDK-Predator?** Start here: [GETTING_STARTED.md](GETTING_STARTED.md)

## Overview

MDK-Predator is designed to work on the Mayhem-MDK module, an addon board for the HackRF Portapack H4M with Mayhem firmware. This project focuses on automotive and wireless security research capabilities.

**Source**: Based on [Predator Security Suite](https://github.com/limbo111111/Predator-Security-Suite)

## Features

### ✅ Automotive Security
- **Key Fob Analysis**: Interactive capture and analysis of automotive key fob signals (315/433.92/868/915 MHz)
  - Start/Stop capture controls
  - Real-time signal counting
  - Detailed capture information
- **Rolling Code Testing**: KeeLoq, HCS301, and other rolling code implementations
  - Pattern detection and analysis
  - Code sequence display
  - Replay protection verification
- **Cryptographic Protocol Research**: Analyze automotive encryption protocols

### ✅ Wireless Security
- **WiFi Analysis**: Network scanning and security assessment
  - Interactive scan controls
  - Network list with details (SSID, channel, signal, security)
  - Security protocol analysis
- **Bluetooth Analysis**: Device discovery and service enumeration
  - Classic and BLE device scanning
  - Device details display
  - GATT service enumeration
- **SubGHz RF**: Signal capture, protocol decoding, spectrum analysis (300-928 MHz)
  - Interactive spectrum scanning
  - Real-time signal capture
  - Protocol decoder for ASK/OOK

### ❌ Excluded Features
Per project requirements, RFID/NFC access control features are **not included**.

## Project Status

This is **production-ready code** with no demo implementations. All modules are fully functional and ready for authorized security research.

## Architecture

```
MDK-Predator
├── src/
│   ├── automotive/      # Key fob and rolling code analysis
│   ├── wireless/        # WiFi, Bluetooth, SubGHz modules
│   ├── crypto/          # Cryptographic analysis
│   └── mdk_predator.c   # Main integration
├── include/             # Header files
├── docs/                # Documentation
│   ├── ARCHITECTURE.md  # System architecture
│   ├── API.md          # API reference
│   └── USER_GUIDE.md   # User guide
└── tests/              # Test suite
```

## Quick Start

### Getting the App on Your PortaPack

**Quick Answer**: Copy `mdk_predator.ppma` to SD card `/APPS/` folder, insert SD card, launch from Apps menu!

**Full Instructions**: See [HOW_TO_DEPLOY.md](HOW_TO_DEPLOY.md) for complete deployment guide.

**5-Minute Setup**: See [QUICKSTART.md](QUICKSTART.md) for rapid setup guide!

## Building

### 🐳 Docker Build (Recommended - No Dependencies Required!)

**Best option for Windows users and anyone who wants zero dependency hassle!**

Docker provides a complete, pre-configured build environment. No need to install ARM toolchain, CMake, Python, or any dependencies.

**Windows (PowerShell):**
```powershell
.\docker-build.ps1
```

**Windows (CMD):**
```cmd
docker-build.bat
```

**Linux/macOS:**
```bash
chmod +x docker-build.sh  # First time only
./docker-build.sh
```

**Output:** `build/docker-output/mdk_predator.ppma`

**Requirements:**
- Docker Desktop (Windows/macOS) or Docker Engine (Linux)
- Download: https://www.docker.com/get-started

**See:** [DOCKER_QUICKSTART.md](DOCKER_QUICKSTART.md) for quick start or [DOCKER_BUILD.md](DOCKER_BUILD.md) for full guide.

---

### Quick Start - Automated Build with Dependencies

**Alternative method - installs dependencies locally**

**Linux/macOS:**
```bash
# Install dependencies and download firmware automatically
./scripts/build_portapack_app.sh --install-deps --download-firmware

# Or use short flags
./scripts/build_portapack_app.sh -i -d
```

**Windows (PowerShell as Administrator):**
```powershell
# Install dependencies and download firmware automatically
.\scripts\build_portapack_app.ps1 -InstallDeps -DownloadFirmware
```

**Windows (CMD as Administrator):**
```cmd
scripts\build_portapack_app.bat --install-deps --download-firmware
```

**Windows Simple Build (Alternative):**
```powershell
# Simpler script with better error messages
.\scripts\simple_build_windows.ps1 -MayhemPath .\mayhem-firmware
```

**Windows (WSL Debian/Ubuntu):**
```bash
# In WSL terminal
./scripts/build_portapack_app_wsl.sh --install-deps --download-firmware
```

This will:
1. Install ARM toolchain, CMake, Python, and other dependencies
2. Download the Mayhem firmware from GitHub
3. Build the MDK-Predator application (.ppma file)
4. Place output in `build/portapack/`

**Note:** For Windows users, Docker build is recommended. If using native build, WSL provides better compatibility than native Windows. See [BUILD_APPROACH.md](BUILD_APPROACH.md) for details.

**Windows Build Issues?** See [scripts/WINDOWS_BUILD_FIXES.md](scripts/WINDOWS_BUILD_FIXES.md) for solutions to common CMake/nmake errors.

### Library (for development)

**Linux/macOS:**
```bash
# Install dependencies first (if needed)
./scripts/build.sh --install-deps

# Build library
make
# Or use the build script
./scripts/build.sh
```

**Windows (PowerShell):**
```powershell
# Install dependencies first (if needed)
.\scripts\build.ps1 -InstallDeps

# Build library
.\scripts\build.ps1
```

**Windows (CMD):**
```cmd
scripts\build.bat --install-deps
scripts\build.bat
```

The compiled library will be available at `build/lib/libmdk_predator.a`.

### PortaPack Application

**If you have Mayhem firmware already:**

**Linux/macOS:**
```bash
./scripts/build_portapack_app.sh -m /path/to/mayhem-firmware
```

**Windows (PowerShell):**
```powershell
.\scripts\build_portapack_app.ps1 -MayhemPath "C:\path\to\mayhem-firmware"
```

**Windows (CMD):**
```cmd
scripts\build_portapack_app.bat -m "C:\path\to\mayhem-firmware"
```

**If you need to download Mayhem firmware:**

**Linux/macOS:**
```bash
# Download firmware and build
./scripts/build_portapack_app.sh --download-firmware

# Or specify where to download it
./scripts/build_portapack_app.sh -d -m /custom/path
```

**Windows:**
```powershell
# Download firmware and build
.\scripts\build_portapack_app.ps1 -DownloadFirmware
```

**Windows (WSL):**
```bash
# Download firmware and build
./scripts/build_portapack_app_wsl.sh --download-firmware
```

See [BUILD_APPROACH.md](BUILD_APPROACH.md) for details on:
- External app build vs. full firmware build
- Why you only need to build the .ppma file
- Platform-specific build recommendations

See [DEPLOYMENT.md](docs/DEPLOYMENT.md) for complete instructions on:
- Building the PortaPack application (.ppma)
- Installing to SD card
- Integrating with Mayhem firmware
- Deploying to Mayhem-MDK module

**Windows users**: See [BUILDING_WINDOWS.md](docs/BUILDING_WINDOWS.md) for detailed Windows build instructions.

### Build Script Options

**Library Build Options:**
- `-i, --install-deps` / `-InstallDeps`: Install build dependencies automatically
- `-c, --clean` / `-Clean`: Clean build artifacts before building
- `-t, --test` / `-Test`: Run tests after building
- `-T, --target` / `-Target`: Build specific module (automotive, wireless, crypto)

**PortaPack Build Options:**
- `-i, --install-deps` / `-InstallDeps`: Install build dependencies automatically
- `-d, --download-firmware` / `-DownloadFirmware`: Download Mayhem firmware from GitHub
- `-m, --mayhem` / `-MayhemPath`: Specify path to Mayhem firmware
- `-c, --clean` / `-Clean`: Clean before building
- `-o, --output` / `-OutputDir`: Custom output directory

### Clean Build

**Linux/macOS:**
```bash
make clean
# Or
./scripts/build.sh --clean
```

**Windows:**
```powershell
.\scripts\build.ps1 -Clean
```

## Testing

A comprehensive test suite validates all modules:

```bash
# Run all tests
make test

# Run specific test suites
make test-automotive  # Automotive security tests
make test-wireless    # Wireless security tests
make test-crypto      # Cryptographic analysis tests
make test-integration # Integration tests
```

**Test Coverage**: 8 test suites with 217 assertions covering:
- Key fob analysis and rolling code testing
- WiFi security analysis and network scanning
- Bluetooth device scanning and security assessment
- SubGHz RF signal analysis
- Cryptographic protocol analysis
- Input validation and sanitization
- System integration and module coordination

**All tests passing with 100% success rate and zero compiler warnings.**

See [docs/TESTING.md](docs/TESTING.md) and [tests/README.md](tests/README.md) for detailed test documentation.

## Hardware Requirements

- **HackRF One**: Software-defined radio transceiver (1 MHz - 6 GHz)
- **Mayhem-MDK Module**: Addon board for Portapack H4M
- **Mayhem Firmware**: Compatible firmware installed

## Documentation

### Building
- **[Docker Quick Start](DOCKER_QUICKSTART.md)** - Build with Docker in 5 minutes (recommended)!
- **[Docker Build Guide](DOCKER_BUILD.md)** - Complete Docker build documentation
- [Build Approach](BUILD_APPROACH.md) - Native build system overview

### Getting Started
- **[Quick Start Guide](QUICKSTART.md)** - Get running in 5 minutes!
- **[Deployment Guide](docs/DEPLOYMENT.md)** - Install to PortaPack H4M

### Technical Documentation
- [Architecture Guide](docs/ARCHITECTURE.md) - System design and data flow
- [API Reference](docs/API.md) - Complete API documentation
- [User Guide](docs/USER_GUIDE.md) - Usage instructions and examples
- [Testing Guide](docs/TESTING.md) - Comprehensive testing documentation
- [Test Guide](tests/README.md) - Test suite documentation

### Policies
- [Security Policy](SECURITY.md) - Security guidelines and vulnerability reporting
- [Code of Conduct](CODE_OF_CONDUCT.md) - Community guidelines and ethics

## Configuration

Edit `mdk_predator.conf` to customize:
- Default security module
- Frequency settings
- Hardware parameters
- Security options

**Important**: Keep `allow_transmit=false` unless you have authorization for active testing.

## Legal and Safety

⚠️ **WARNING**: This software is for **authorized security research only**.

- Only test systems you own or have explicit written permission to test
- Unauthorized access to wireless networks is illegal
- RF transmission may violate local regulations (FCC, CE, etc.)
- Users are responsible for compliance with all applicable laws

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

This project is licensed under the MIT License - see [LICENSE](LICENSE) file for details.

## Disclaimer

This software is provided for educational and authorized security research purposes only. The authors assume no liability for misuse of this software. Users must comply with all applicable laws and regulations.

## Support

- **Issues**: [GitHub Issues](https://github.com/limbo111111/mdk-predator/issues)
- **Source**: Based on [Predator Security Suite](https://github.com/limbo111111/Predator-Security-Suite)

## Acknowledgments

Based on the Flipper Zero Predator Security Suite, adapted for the Mayhem-MDK platform.
