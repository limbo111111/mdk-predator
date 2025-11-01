# Build Script Examples and Testing Guide

This document provides examples of using the updated build scripts and demonstrates that the build process works correctly.

## Quick Start Examples

### Complete Build from Scratch (Most Common Use Case)

#### On Linux/macOS:
```bash
# One command to install everything and build
./scripts/build_portapack_app.sh --install-deps --download-firmware

# Short version
./scripts/build_portapack_app.sh -i -d
```

#### On Windows (PowerShell as Administrator):
```powershell
# One command to install everything and build
.\scripts\build_portapack_app.ps1 -InstallDeps -DownloadFirmware
```

#### On Windows (CMD as Administrator):
```cmd
scripts\build_portapack_app.bat --install-deps --download-firmware
```

#### On Windows (WSL Debian/Ubuntu) - RECOMMENDED:
```bash
# In WSL terminal
./scripts/build_portapack_app_wsl.sh --install-deps --download-firmware

# Short version
./scripts/build_portapack_app_wsl.sh -i -d
```

**Why WSL?** WSL provides better compatibility with ARM toolchains and build tools on Windows. See [BUILD_APPROACH.md](BUILD_APPROACH.md) for details.

### What This Does:
1. **Installs Dependencies**:
   - ARM GCC toolchain (arm-none-eabi-gcc)
   - CMake 3.16+
   - Python 3.7+
   - Make
   - Git
   - dfu-util (Linux/WSL)
   - build-essential (WSL)

2. **Downloads Mayhem Firmware**:
   - Clones from https://github.com/portapack-mayhem/mayhem-firmware
   - Initializes submodules
   - Default location: `build/mayhem-firmware/`

3. **Builds Application**:
   - Integrates MDK-Predator with Mayhem firmware
   - Compiles external application ONLY (not full firmware)
   - Creates `mdk_predator.ppma` file
   - Output in: `build/portapack/`

## Library Build Examples

### Build Library with Auto-Install

#### Linux/macOS:
```bash
# Install dependencies and build
./scripts/build.sh --install-deps

# Just build (if deps already installed)
./scripts/build.sh

# Clean build
./scripts/build.sh --clean

# Build and test
./scripts/build.sh --test

# Build specific module
./scripts/build.sh --target automotive
```

#### Windows:
```powershell
# Install dependencies and build
.\scripts\build.ps1 -InstallDeps

# Just build
.\scripts\build.ps1

# Clean build
.\scripts\build.ps1 -Clean

# Build and test
.\scripts\build.ps1 -Test

# Build specific module
.\scripts\build.ps1 -Target wireless
```

## Advanced Usage Examples

### Update Existing Firmware

If firmware already downloaded but need to update:

```bash
# Linux/macOS
./scripts/build_portapack_app.sh -d

# Windows
.\scripts\build_portapack_app.ps1 -DownloadFirmware
```

The script will detect existing firmware and update it via `git pull`.

### Build with Custom Firmware Location

```bash
# Linux/macOS
./scripts/build_portapack_app.sh -m ~/my-custom-mayhem-fork

# Windows
.\scripts\build_portapack_app.ps1 -MayhemPath "C:\my-firmware"
```

### Clean Build with Custom Output

```bash
# Linux/macOS
./scripts/build_portapack_app.sh -m ~/mayhem -c -o /tmp/output

# Windows
.\scripts\build_portapack_app.ps1 -MayhemPath "C:\mayhem" -Clean -OutputDir "C:\output"
```

### Download to Custom Location

```bash
# Linux/macOS
./scripts/build_portapack_app.sh -d -m ~/custom-location

# Windows
.\scripts\build_portapack_app.ps1 -DownloadFirmware -MayhemPath "C:\custom"
```

## Dependency Installation Details

### Linux (Ubuntu/Debian)
The script will run:
```bash
sudo apt-get update
sudo apt-get install -y \
    gcc-arm-none-eabi \
    binutils-arm-none-eabi \
    cmake \
    python3 \
    python3-pip \
    git \
    make \
    dfu-util
```

### Linux (Fedora/RHEL/CentOS)
The script will run:
```bash
sudo dnf install -y \
    arm-none-eabi-gcc-cs \
    arm-none-eabi-newlib \
    cmake \
    python3 \
    python3-pip \
    git \
    make \
    dfu-util
```

### Linux (Arch/Manjaro)
The script will run:
```bash
sudo pacman -S --noconfirm \
    arm-none-eabi-gcc \
    arm-none-eabi-newlib \
    cmake \
    python \
    python-pip \
    git \
    make \
    dfu-util
```

### Windows (via Chocolatey)
The script will:
1. Install Chocolatey (if not present)
2. Run:
```powershell
choco install gcc-arm-embedded -y
choco install cmake -y
choco install python -y
choco install git -y
choco install make -y
```

**Note**: Windows installation requires Administrator privileges.

## Testing the Scripts

### Test Help Output

```bash
# Linux
./scripts/build_portapack_app.sh --help
./scripts/build.sh --help

# Windows
.\scripts\build_portapack_app.ps1 -Help
.\scripts\build.ps1 -Help
```

### Test Dependency Checking

```bash
# Linux - will show which dependencies are missing
./scripts/build.sh

# Windows
.\scripts\build.ps1
```

Expected output when dependencies missing:
```
[INFO] Checking build requirements...
[WARN] ARM toolchain not found
[INFO] Make: GNU Make 4.3
[ERROR] Missing dependencies: arm-none-eabi-gcc

[INFO] You can install them automatically with: ./scripts/build.sh --install-deps
```

### Test Firmware Download

```bash
# Linux (test mode - download to temporary location)
./scripts/build_portapack_app.sh -d -m /tmp/test-mayhem

# Windows
.\scripts\build_portapack_app.ps1 -DownloadFirmware -MayhemPath "C:\Temp\test"
```

## Troubleshooting

### Linux: Permission Denied

```bash
chmod +x scripts/*.sh
```

### Windows: ExecutionPolicy Error

Run PowerShell as Administrator:
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### Chocolatey Installation Fails

Install manually:
1. Visit https://chocolatey.org/install
2. Follow installation instructions
3. Run build script again

### Git Clone Fails

Check internet connection and firewall settings. The script needs to access:
- https://github.com/portapack-mayhem/mayhem-firmware

### ARM Toolchain Not Found After Install

On Linux, you may need to refresh your shell:
```bash
source ~/.bashrc
# or
source ~/.zshrc
```

On Windows, restart your terminal or computer.

## Verification Steps

After running the build scripts:

### Verify Library Build

```bash
# Check library exists
ls -lh build/lib/libmdk_predator.a

# Expected output: library file with size ~50-200 KB
```

### Verify PortaPack Build

```bash
# Check application exists
ls -lh build/portapack/mdk_predator.ppma

# Check all files present
ls -la build/portapack/
# Should show:
# - mdk_predator.ppma
# - mdk_predator.conf
# - README.txt
```

### Verify Firmware Download

```bash
# Check firmware exists
ls -la build/mayhem-firmware/

# Should contain:
# - CMakeLists.txt
# - firmware/
# - .git/
```

## CI/CD Integration

The scripts can be used in CI/CD pipelines:

### GitHub Actions Example

```yaml
name: Build MDK-Predator

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2

      - name: Install dependencies and build
        run: |
          ./scripts/build_portapack_app.sh --install-deps --download-firmware

      - name: Upload artifact
        uses: actions/upload-artifact@v2
        with:
          name: mdk_predator.ppma
          path: build/portapack/mdk_predator.ppma
```

## Summary

The build scripts now provide a complete, automated build experience:

✅ **Automatic dependency installation**
✅ **Automatic firmware download**
✅ **Cross-platform support (Linux/macOS/Windows)**
✅ **Clear error messages and guidance**
✅ **Flexible options for advanced users**
✅ **One-command build from scratch**

This makes it easy for anyone to build MDK-Predator, regardless of their system setup or experience level.
