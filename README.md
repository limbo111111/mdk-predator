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

### ☁️ GitHub Codespaces (Easiest - Build in Your Browser!)

**Zero setup required! Build directly in your browser with GitHub Codespaces.**

Click the **Code** button on GitHub → **Codespaces** → **Create codespace**. The development environment will automatically set up with all tools pre-installed.

```bash
# In Codespaces terminal
./docker-build.sh
# Or use native build
./build.sh -d
```

**See:** [.devcontainer/README.md](.devcontainer/README.md) for detailed Codespaces documentation.

---

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

### Simple Native Build

**New simplified build scripts (recommended for native builds)**

**Linux/macOS:**
```bash
# Download firmware and build
./build.sh -d

# Build with existing firmware
./build.sh -m /path/to/mayhem-firmware
```

**Windows (PowerShell):**
```powershell
# Download firmware and build
.\build.ps1 -Download

# Build with existing firmware
.\build.ps1 -MayhemPath C:\path\to\mayhem-firmware
```

**Output:** `build/output/mdk_predator.ppma`

This will:
1. Download the Mayhem firmware from GitHub (if needed)
2. Copy MDK-Predator files to the firmware
3. Register the app in external.cmake
4. Build the MDK-Predator application (.ppma file)

**See:** [BUILD.md](BUILD.md) for complete build documentation.

### Library (for development)

**Linux/macOS:**
```bash
# Build library
make
```

**Windows:**
```powershell
# Use Docker build or WSL
.\docker-build.ps1
```

The compiled library will be available at `build/lib/libmdk_predator.a`.

### PortaPack Application

**If you have Mayhem firmware already:**

**Linux/macOS:**
```bash
./build.sh -m /path/to/mayhem-firmware
```

**Windows (PowerShell):**
```powershell
.\build.ps1 -MayhemPath "C:\path\to\mayhem-firmware"
```

**If you need to download Mayhem firmware:**

**Linux/macOS:**
```bash
# Download firmware and build
./build.sh -d
```

**Windows:**
```powershell
# Download firmware and build
.\build.ps1 -Download
```

See [BUILD.md](BUILD.md) for complete build documentation including:
- External app build vs. full firmware build
- Why you only need to build the .ppma file
- Manual build instructions
- MDK module integration details

See [DEPLOYMENT.md](docs/DEPLOYMENT.md) for deployment instructions:
- Installing to SD card
- Deploying to Mayhem-MDK module

### Build Options

**Simplified Build Script Options:**
- `-m PATH` / `-MayhemPath PATH`: Specify path to Mayhem firmware
- `-d` / `-Download`: Download Mayhem firmware from GitHub
- `-c` / `-Clean`: Clean before building
- `-h` / `-Help`: Show help message

### Clean Build

**Linux/macOS:**
```bash
make clean
# Or for PortaPack app
./build.sh -c -d
```

**Windows:**
```powershell
.\build.ps1 -Clean -Download
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

### Development Environment
- **[GitHub Codespaces Setup](.devcontainer/README.md)** - Build in your browser with zero setup!
- **[DevContainer Guide](.devcontainer/README.md)** - VS Code Remote Containers setup

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
