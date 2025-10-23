# How to Deploy MDK-Predator to PortaPack and Mayhem-MDK Module

This document provides a complete answer to: **"How do I get this app on my PortaPack and on the Mayhem-MDK module?"**

## TL;DR (Too Long; Didn't Read)

1. Download `mdk_predator.ppma` file
2. Copy to SD card in `/APPS/` folder
3. Insert SD card into PortaPack
4. Launch from Apps menu
5. Done! ✓

**Detailed instructions below...**

## Understanding the Components

Before we start, let's understand what you have:

### Your Hardware

```
┌─────────────────────┐
│   HackRF One       │  ← SDR transceiver (radio)
└──────────┬──────────┘
           │
┌──────────▼──────────┐
│  PortaPack H4M     │  ← Display and controls
└──────────┬──────────┘
           │
┌──────────▼──────────┐
│  Mayhem-MDK Module │  ← Enhanced capabilities addon
└─────────────────────┘
```

### The Software

- **Mayhem Firmware**: Operating system on PortaPack
- **MDK-Predator App**: Application that runs on Mayhem
- **File format**: `.ppma` (PortaPack Mayhem Application)

## Deployment Overview

MDK-Predator can be deployed in two ways:

### Method A: External App (Recommended) 🌟
- Run from SD card
- No firmware modification needed
- Easy to install/uninstall
- Can update independently

### Method B: Integrated App
- Built into firmware
- Always available
- Requires firmware rebuild
- For advanced users

**We recommend Method A for most users.**

## Method A: SD Card Deployment (Recommended)

This is the easiest and most common method.

### What You'll Need

- ✅ Your HackRF + PortaPack + Mayhem-MDK hardware
- ✅ MicroSD card (8GB or larger, formatted as FAT32)
- ✅ Computer with SD card reader
- ✅ MDK-Predator application file (`mdk_predator.ppma`)

### Step-by-Step Instructions

#### Step 1: Get the Application File

**Option A: Download Pre-built (Easiest)**

1. Go to: https://github.com/limbo111111/mdk-predator/releases
2. Download latest `mdk_predator.ppma` file
3. Save to your computer

**Option B: Build from Source**

If no pre-built version is available, see "Building from Source" section below.

#### Step 2: Prepare Your SD Card

**A. Format SD Card (if needed)**

Your SD card must be FAT32 format:

**On Windows:**
```
1. Insert SD card
2. Open "This PC"
3. Right-click SD card
4. Select "Format..."
5. Choose "FAT32" as file system
6. Click "Start"
```

**On macOS:**
```
1. Insert SD card
2. Open "Disk Utility"
3. Select SD card
4. Click "Erase"
5. Format: "MS-DOS (FAT)"
6. Click "Erase"
```

**On Linux:**
```bash
# Find your SD card device (e.g., /dev/sdb1)
lsblk

# Format as FAT32 (CAUTION: Replace /dev/sdX with your device!)
sudo mkfs.vfat -F 32 /dev/sdX1
```

**B. Create Directory Structure**

On your SD card, create these folders:

```
SD_CARD/
├── APPS/                    ← Application files go here
├── MDK-PREDATOR/
│   ├── config/             ← Configuration files
│   ├── logs/               ← Log files (created automatically)
│   └── captures/           ← Captured data (created automatically)
```

You can create these folders:
- **Windows**: Right-click → New → Folder
- **macOS/Linux**: Use Finder/File Manager or `mkdir` command

#### Step 3: Copy Files to SD Card

**A. Copy Application**

Copy `mdk_predator.ppma` to the `/APPS/` folder on your SD card.

Your SD card should now have:
```
SD_CARD/
└── APPS/
    └── mdk_predator.ppma  ← Application file here
```

**B. Copy Configuration**

1. From the repository, find `mdk_predator.conf` file
2. Copy it to `/MDK-PREDATOR/config/` on SD card

Your SD card should now have:
```
SD_CARD/
├── APPS/
│   └── mdk_predator.ppma
└── MDK-PREDATOR/
    └── config/
        └── mdk_predator.conf  ← Config file here
```

**Important**: Configuration file is optional. App will use defaults if not present.

#### Step 4: Install SD Card

1. **Safely eject** SD card from computer
   - Windows: Click "Safely Remove Hardware"
   - macOS: Drag to trash or click eject
   - Linux: Use `umount` or file manager eject

2. **Power off** your PortaPack

3. **Insert SD card** into PortaPack SD card slot

4. **Power on** your PortaPack

#### Step 5: Launch the Application

1. Wait for PortaPack to fully boot (10-15 seconds)

2. Navigate to **Apps** menu:
   - Use directional buttons/joystick
   - Find "Apps" in main menu
   - Press Select/Enter

3. Find **MDK-Predator** in app list:
   - Scroll through available apps
   - Should see "MDK-Predator" with icon
   - Press Select/Enter to launch

4. Application will initialize:
   ```
   MDK-Predator Security Suite
   Mayhem-MDK Module
   
   Status: Initialized ✓
   ```

5. **Success!** You're now running MDK-Predator!

### Troubleshooting SD Card Method

**Problem**: App not visible in Apps menu

**Solutions**:
- Check file is in `/APPS/` (not in subdirectory)
- Verify SD card is FAT32 format
- Try removing and reinserting SD card
- Check firmware version is 1.7.0 or later
- Try different SD card

**Problem**: "Init Failed" on launch

**Solutions**:
- Verify HackRF is connected to PortaPack
- Check Mayhem-MDK module is properly seated
- Ensure adequate power (5V, 2A minimum)
- Delete config file and use defaults
- Restart device

## Method B: Firmware Integration

This method builds MDK-Predator directly into the Mayhem firmware.

### When to Use This Method

- You want app always available (no SD card needed)
- You're comfortable building firmware
- You want fastest loading
- You're an advanced user

### Requirements

- PortaPack Mayhem firmware source code
- ARM toolchain (arm-none-eabi-gcc)
- CMake 3.16 or later
- Python 3.7 or later
- Build tools (make, git)

### Step-by-Step Instructions

#### Step 1: Get the Source Code

**A. Clone Mayhem Firmware**

```bash
# Clone the Mayhem firmware repository
git clone https://github.com/portapack-mayhem/mayhem-firmware.git
cd mayhem-firmware

# Initialize submodules
git submodule update --init --recursive
```

**B. Clone MDK-Predator**

```bash
# In a separate directory
cd ..
git clone https://github.com/limbo111111/mdk-predator.git
```

#### Step 2: Integrate MDK-Predator

**Option A: Use Build Script (Easiest)**

```bash
cd mdk-predator
./scripts/build_portapack_app.sh -m /path/to/mayhem-firmware
```

The script will:
- Copy all necessary files
- Configure the build
- Build the application
- Output `mdk_predator.ppma` file

**Option B: Manual Integration**

```bash
# Create external app directory
mkdir -p mayhem-firmware/firmware/application/external/mdk_predator

# Copy application files
cp -r mdk-predator/app/* mayhem-firmware/firmware/application/external/mdk_predator/

# Copy source code
cp -r mdk-predator/src mayhem-firmware/firmware/application/external/mdk_predator/
cp -r mdk-predator/include mayhem-firmware/firmware/application/external/mdk_predator/

# Copy configuration
cp mdk-predator/mdk_predator.conf mayhem-firmware/firmware/application/external/mdk_predator/
```

#### Step 3: Build the Firmware

```bash
cd mayhem-firmware

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build external apps
make external_apps
```

This will create: `firmware/application/external/mdk_predator.ppma`

#### Step 4: Deploy

Now follow Method A steps to copy the `.ppma` file to SD card!

## Building from Source (Detailed)

If no pre-built application is available, here's how to build it yourself.

### Install Build Tools

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install gcc-arm-none-eabi cmake python3 git
```

**macOS:**
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install tools
brew install gcc-arm-embedded cmake python3
```

**Windows:**
1. Download ARM toolchain from [ARM Developer](https://developer.arm.com/downloads/-/gnu-rm)
2. Install CMake from [cmake.org](https://cmake.org/download/)
3. Install Python from [python.org](https://www.python.org/downloads/)

### Build Process

```bash
# 1. Clone repositories
git clone https://github.com/portapack-mayhem/mayhem-firmware.git
git clone https://github.com/limbo111111/mdk-predator.git

# 2. Run build script
cd mdk-predator
./scripts/build_portapack_app.sh -m ../mayhem-firmware

# 3. Find built application
# Output location: build/portapack/mdk_predator.ppma
```

### Build Script Options

```bash
# Basic build
./scripts/build_portapack_app.sh -m /path/to/mayhem-firmware

# Clean build
./scripts/build_portapack_app.sh -m /path/to/mayhem-firmware -c

# Custom output directory
./scripts/build_portapack_app.sh -m /path/to/mayhem-firmware -o /custom/output

# Help
./scripts/build_portapack_app.sh --help
```

## Verification

After deployment, verify everything works:

### Basic Test

1. Launch MDK-Predator
2. Check status shows "Initialized"
3. Try navigating between modules
4. Exit and relaunch

### Module Test

Pick one module to test:

**WiFi Test:**
1. Select "WiFi Analysis"
2. Select "Network Scanner"
3. Should detect nearby networks

**Automotive Test:**
1. Select "Automotive Security"
2. Select "Key Fob Analyzer"
3. Should show ready to receive

### Full Test

See [INSTALLATION_CHECKLIST.md](docs/INSTALLATION_CHECKLIST.md) for comprehensive testing.

## Configuration

### Default Configuration

The app works out-of-the-box with safe defaults:
- Transmit: **DISABLED** (receive only)
- Default module: Automotive
- Standard gain settings
- Logging enabled

### Custom Configuration

Edit `/MDK-PREDATOR/config/mdk_predator.conf` on SD card:

```ini
[General]
default_module=automotive      # Starting module
enable_logging=true            # Enable logging

[Hardware]
sample_rate=2000000           # Sample rate (2 MS/s)
bandwidth=1750000             # Bandwidth (1.75 MHz)
lna_gain=8                    # LNA gain (0-40)
vga_gain=20                   # VGA gain (0-62)

[Security]
allow_transmit=false          # KEEP FALSE!
require_confirmation=true     # Confirm actions
log_all_activity=true        # Log everything

[Automotive]
default_frequency=433920000   # 433.92 MHz
```

## Updating

### Updating the Application

1. Download new version
2. Power off PortaPack
3. Remove SD card
4. Replace `mdk_predator.ppma` in `/APPS/`
5. Reinsert SD card
6. Power on and test

### Keeping Configuration

Your configuration in `/MDK-PREDATOR/config/` is preserved during updates.

## Uninstalling

### Remove from SD Card

1. Power off PortaPack
2. Remove SD card
3. Delete from computer:
   - `/APPS/mdk_predator.ppma`
   - `/MDK-PREDATOR/` folder (optional, to keep logs/captures)
4. Reinsert SD card

## Getting Help

### Documentation

- **Quick Start**: [QUICKSTART.md](QUICKSTART.md)
- **Getting Started**: [GETTING_STARTED.md](GETTING_STARTED.md)
- **Full Deployment**: [docs/DEPLOYMENT.md](docs/DEPLOYMENT.md)
- **User Guide**: [docs/USER_GUIDE.md](docs/USER_GUIDE.md)
- **Installation Checklist**: [docs/INSTALLATION_CHECKLIST.md](docs/INSTALLATION_CHECKLIST.md)

### Support

- **GitHub Issues**: https://github.com/limbo111111/mdk-predator/issues
- **Documentation**: See `/docs` directory in repository

### Log Files

Check logs for errors:
- Location: `/MDK-PREDATOR/logs/mdk_predator.log`
- View on computer after removing SD card

## Summary Checklist

- [ ] Downloaded or built `mdk_predator.ppma`
- [ ] SD card formatted as FAT32
- [ ] Created `/APPS/` folder on SD card
- [ ] Copied `mdk_predator.ppma` to `/APPS/`
- [ ] Created `/MDK-PREDATOR/config/` folder
- [ ] Copied `mdk_predator.conf` (optional)
- [ ] Safely ejected SD card
- [ ] Inserted into PortaPack
- [ ] Powered on device
- [ ] Launched from Apps menu
- [ ] Verified "Initialized" status
- [ ] Tested at least one module

## Safety Reminders

⚠️ **Before You Use MDK-Predator**

- ✅ Only test systems you own
- ✅ Get written authorization for security testing
- ✅ Transmit is disabled by default - keep it that way
- ✅ Follow all local RF regulations
- ❌ Never test unauthorized systems
- ❌ Never enable transmit without authorization

## Quick Reference

### File Locations on SD Card
```
/APPS/mdk_predator.ppma              ← Application
/MDK-PREDATOR/config/mdk_predator.conf  ← Config
/MDK-PREDATOR/logs/                  ← Logs
/MDK-PREDATOR/captures/              ← Captured data
```

### Launch Path
```
PortaPack Main Menu
  → Apps
    → MDK-Predator
      → [Application launches]
```

### Build Script
```bash
./scripts/build_portapack_app.sh -m /path/to/mayhem-firmware
```

## You're Ready!

You now know how to:
- ✓ Deploy MDK-Predator to PortaPack
- ✓ Deploy to Mayhem-MDK module
- ✓ Build from source if needed
- ✓ Configure the application
- ✓ Update and maintain

**Start your authorized security research with MDK-Predator!** 🚀

---

For more detailed information, see:
- [GETTING_STARTED.md](GETTING_STARTED.md) - Complete guide
- [docs/DEPLOYMENT.md](docs/DEPLOYMENT.md) - Technical details
- [docs/USER_GUIDE.md](docs/USER_GUIDE.md) - Usage instructions
