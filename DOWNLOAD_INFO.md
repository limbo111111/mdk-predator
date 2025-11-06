# MDK-Predator Source Package Download

## 📦 Package Information

**File**: `mdk-predator-source-v1.0.tar.gz`  
**Size**: 196 KB  
**Version**: 1.0  
**Date**: November 5, 2025

## 📥 Download Location

The source package is located at:
```
/workspaces/mdk-predator/mdk-predator-source-v1.0.tar.gz
```

## 📋 Package Contents

This complete source package includes:

### Source Code
- `src/` - Core C modules (automotive, wireless, crypto security modules)
- `include/` - All header files
- `app/` - PortaPack UI application wrapper (C++)
- `tests/` - Comprehensive test suite (277 assertions)

### Build System
- `Makefile` - Library build system
- `build.sh` - Linux/macOS build script
- `build.ps1` - Windows PowerShell build script
- `docker-build.sh` - Docker build for Linux/macOS
- `docker-build.ps1` - Docker build for Windows
- `docker-build.bat` - Docker build for Windows CMD
- `docker-compose.yml` - Docker configuration
- `Dockerfile` & `Dockerfile.build` - Docker images

### Configuration
- `mdk_predator.conf` - Application configuration
- `.github/copilot-instructions.md` - AI coding agent instructions

### Documentation
- `README.md` - Main project documentation
- `BUILD.md` - Detailed build instructions
- `DOCKER_BUILD.md` - Docker build guide
- `QUICKSTART.md` - 5-minute quick start
- `GETTING_STARTED.md` - Getting started guide
- `HOW_TO_DEPLOY.md` - Deployment guide
- `CODESPACES.md` - GitHub Codespaces guide
- `docs/ARCHITECTURE.md` - Technical architecture
- `docs/API.md` - Complete API reference
- `docs/TESTING.md` - Testing documentation
- `docs/USER_GUIDE.md` - User guide

### Legal & Contributing
- `LICENSE` - MIT License
- `CONTRIBUTING.md` - Contribution guidelines
- `CODE_OF_CONDUCT.md` - Community guidelines
- `SECURITY.md` - Security policy
- `CHANGELOG.md` - Version history

## 🚀 Quick Start After Download

### 1. Extract Package
```bash
tar -xzf mdk-predator-source-v1.0.tar.gz
cd mdk-predator/
```

### 2. Choose Build Method

#### Option A: Docker Build (Recommended - Works Everywhere!)
```bash
# Linux/macOS
./docker-build.sh

# Windows PowerShell
.\docker-build.ps1

# Windows CMD
docker-build.bat
```
**Output**: `build/docker-output/mdk_predator.ppma`

#### Option B: Native Build (Faster, Requires ARM Toolchain)
```bash
# Linux/macOS
./build.sh -d

# Windows PowerShell
.\build.ps1 -Download
```
**Output**: `build/output/mdk_predator.ppma`

#### Option C: Test-Only Build
```bash
make
make test
```
**Output**: `build/lib/libmdk_predator.a` (library only)

### 3. Deploy to PortaPack
```bash
# Copy to SD card
cp build/docker-output/mdk_predator.ppma /path/to/sdcard/APPS/

# Insert SD card into PortaPack H4M
# Launch from Apps menu → Utilities → MDK-Predator
```

## 💻 System Requirements

### Docker Build (Recommended)
- **Windows**: Docker Desktop 4.0+
- **macOS**: Docker Desktop 4.0+
- **Linux**: Docker Engine 20.10+
- **No other dependencies required!**

### Native Build
- ARM GCC toolchain (arm-none-eabi-gcc 9.2.1)
- CMake 3.16 or later
- Python 3.7 or later
- Git
- Make
- 2GB RAM minimum
- 5GB disk space

## 📊 What You Can Build

1. **PortaPack External App** (`.ppma` file)
   - Ready to deploy to SD card
   - No firmware flashing required
   - Safe and reversible

2. **Static Library** (for testing)
   - Unit testing without hardware
   - Development and debugging

## 🎯 Target Hardware

- **HackRF One** - Software-defined radio (1 MHz - 6 GHz)
- **PortaPack H4M** - Portable HackRF interface with Mayhem firmware
- **Mayhem-MDK Module** (Optional) - Hardware acceleration addon

## ✅ Code Quality

- ✅ All source code compiles with zero errors/warnings
- ✅ 10 test suites with 277 assertions (100% pass rate)
- ✅ Full NULL pointer checking
- ✅ Input validation on all user data
- ✅ Production-ready, no stub code

## 📖 Documentation Highlights

### For Users
- `README.md` - Start here!
- `QUICKSTART.md` - Build in 5 minutes
- `HOW_TO_DEPLOY.md` - SD card deployment

### For Developers
- `BUILD.md` - Comprehensive build guide
- `docs/ARCHITECTURE.md` - System design
- `docs/API.md` - Function reference
- `.github/copilot-instructions.md` - AI agent guide

### For Contributors
- `CONTRIBUTING.md` - Contribution guidelines
- `docs/TESTING.md` - Test suite documentation
- `CODE_OF_CONDUCT.md` - Community standards

## 🔒 Security Notice

⚠️ **WARNING**: This software is for **authorized security research only**.

- Only test systems you own or have written permission to test
- Unauthorized access to wireless networks is illegal
- RF transmission may violate regulations (FCC, CE, etc.)
- Users are responsible for compliance with all laws

Default configuration has transmit **DISABLED** for safety.

## 🆘 Support

- **GitHub Repository**: https://github.com/limbo111111/mdk-predator
- **Issues**: https://github.com/limbo111111/mdk-predator/issues
- **Documentation**: See `docs/` folder in package

## 📝 License

MIT License - See `LICENSE` file in package for full terms.

---

**Happy Building! 🚀**

For questions or issues, please open a GitHub issue or consult the documentation.
