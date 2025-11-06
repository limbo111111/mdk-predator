MDK-PREDATOR SOURCE PACKAGE
===========================

This package contains everything you need to compile MDK-Predator on your PC.

CONTENTS:
---------
- src/              - Core C modules (automotive, wireless, crypto)
- include/          - Header files
- app/              - PortaPack UI application wrapper
- tests/            - Comprehensive test suite
- docs/             - Technical documentation
- Build scripts     - For Windows, Linux, and macOS
- Configuration     - mdk_predator.conf
- Documentation     - README, BUILD guides, etc.

BUILD OPTIONS:
--------------

1. DOCKER BUILD (Recommended - Works on ALL platforms)
   
   Windows (PowerShell):
   .\docker-build.ps1
   
   Windows (CMD):
   docker-build.bat
   
   Linux/macOS:
   ./docker-build.sh
   
   Output: build/docker-output/mdk_predator.ppma

2. NATIVE BUILD (Requires ARM toolchain)
   
   Linux/macOS:
   ./build.sh -d
   
   Windows (PowerShell):
   .\build.ps1 -Download
   
   Output: build/output/mdk_predator.ppma

3. LIBRARY BUILD (For testing only)
   
   make
   make test
   
   Output: build/lib/libmdk_predator.a

REQUIREMENTS:
-------------

Docker Build:
- Docker Desktop (Windows/macOS) or Docker Engine (Linux)
- No other dependencies!

Native Build:
- ARM GCC toolchain (arm-none-eabi-gcc 9.2.1)
- CMake 3.16+
- Python 3.7+
- Git, Make

QUICK START:
------------

1. Extract this package:
   tar -xzf mdk-predator-source.tar.gz
   cd mdk-predator-source/

2. Build with Docker (easiest):
   ./docker-build.sh
   
3. Copy to SD card:
   cp build/docker-output/mdk_predator.ppma /path/to/sdcard/APPS/

4. Insert SD card into PortaPack and launch from Apps menu!

DOCUMENTATION:
--------------
- README.md           - Project overview
- BUILD.md            - Detailed build instructions
- DOCKER_BUILD.md     - Docker build guide
- QUICKSTART.md       - 5-minute setup guide
- docs/ARCHITECTURE.md - Technical architecture
- docs/API.md         - API reference

SUPPORT:
--------
- GitHub: https://github.com/limbo111111/mdk-predator
- Issues: https://github.com/limbo111111/mdk-predator/issues

LICENSE:
--------
MIT License - See LICENSE file for details

SECURITY WARNING:
-----------------
This software is for authorized security research only.
Users must comply with all applicable laws and regulations.
