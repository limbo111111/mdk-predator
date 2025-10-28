# MDK-Predator Deployment Guide

This guide provides complete instructions for deploying MDK-Predator to your HackRF PortaPack H4M with Mayhem-MDK module.

## Table of Contents

1. [Hardware Requirements](#hardware-requirements)
2. [Firmware Requirements](#firmware-requirements)
3. [Building the Application](#building-the-application)
4. [Deployment Methods](#deployment-methods)
5. [Installation Steps](#installation-steps)
6. [Verification](#verification)
7. [Troubleshooting](#troubleshooting)

## Hardware Requirements

### Required Hardware

1. **HackRF One**
   - Software-defined radio (SDR) transceiver
   - Frequency range: 1 MHz to 6 GHz
   - Half-duplex operation
   - USB 2.0 interface

2. **PortaPack H4M**
   - LCD display (320x240 or 240x320)
   - Navigation controls (joystick/buttons)
   - SD card slot
   - 3.5mm headphone jack
   - Built-in speaker

3. **Mayhem-MDK Module**
   - Addon board for PortaPack H4M
   - Extended RF capabilities
   - Additional processing power
   - Enhanced memory

4. **Accessories**
   - MicroSD card (8GB or larger, Class 10 recommended)
   - Appropriate antennas for target frequencies:
     - 315 MHz antenna (automotive key fobs)
     - 433 MHz antenna (automotive key fobs, SubGHz)
     - 868/915 MHz antenna (regional ISM bands)
     - 2.4 GHz antenna (WiFi, Bluetooth)
   - USB cable (Micro-B or USB-C depending on HackRF version)
   - Power supply (5V, 2A minimum)

### Optional Hardware

- External battery pack for portable operation
- Tripod mount for stable positioning
- RF shielding/Faraday bag for testing

## Firmware Requirements

### Mayhem Firmware

MDK-Predator requires PortaPack Mayhem firmware version **1.7.0 or later**.

#### Checking Your Firmware Version

1. Power on your PortaPack
2. Navigate to **Settings** → **About**
3. Check the firmware version displayed

#### Updating Mayhem Firmware

If you need to update:

1. Download the latest Mayhem firmware from:
   - GitHub: https://github.com/portapack-mayhem/mayhem-firmware/releases

2. Extract the firmware files

3. Copy `portapack-h1_h2-mayhem.bin` to your SD card root directory

4. Power on PortaPack while holding DFU button

5. Follow on-screen update instructions

## Building the Application

### Method 1: Use Pre-built Application (Recommended)

If a pre-built `.ppma` file is provided with the release:

1. Download `mdk_predator.ppma` from the releases page
2. Skip to [Deployment Methods](#deployment-methods)

### Method 2: Build from Source

#### Prerequisites

1. **PortaPack Mayhem Firmware Source**
   ```bash
   git clone https://github.com/portapack-mayhem/mayhem-firmware.git
   cd mayhem-firmware
   git submodule update --init --recursive
   ```

2. **ARM Toolchain**
   - Ubuntu/Debian:
     ```bash
     sudo apt-get install gcc-arm-none-eabi
     ```
   - macOS:
     ```bash
     brew install gcc-arm-embedded
     ```
   - Windows: Download from [ARM Developer](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)

3. **Build Tools**
   - CMake 3.16 or later
   - Make or Ninja
   - Python 3.7+

#### Build Steps

1. **Clone MDK-Predator**
   ```bash
   git clone https://github.com/limbo111111/mdk-predator.git
   cd mdk-predator
   ```

2. **Integrate with Mayhem Firmware**
   ```bash
   # Create external app directory
   mkdir -p /path/to/mayhem-firmware/firmware/application/external/mdk_predator

   # Copy application files
   cp -r app/* /path/to/mayhem-firmware/firmware/application/external/mdk_predator/

   # Copy source and headers
   cp -r src /path/to/mayhem-firmware/firmware/application/external/mdk_predator/
   cp -r include /path/to/mayhem-firmware/firmware/application/external/mdk_predator/

   # Copy configuration
   cp mdk_predator.conf /path/to/mayhem-firmware/firmware/application/external/mdk_predator/
   ```

3. **Build Mayhem Firmware with MDK-Predator**
   ```bash
   cd /path/to/mayhem-firmware
   mkdir build
   cd build
   cmake ..
   # Build application (which includes external apps)
   make application
   ```

4. **Locate Built Application**
   The build process creates:
   - `firmware/application/external/mdk_predator.ppma`

## Deployment Methods

### Method A: SD Card Deployment (Recommended)

This method is the easiest and doesn't require firmware updates.

#### Steps

1. **Prepare SD Card**
   - Format SD card as FAT32
   - Create directory structure:
     ```
     SD_CARD/
     ├── APPS/
     │   └── mdk_predator.ppma
     └── MDK-PREDATOR/
         ├── config/
         │   └── mdk_predator.conf
         └── logs/
     ```

2. **Copy Application**
   ```bash
   # Copy the .ppma file
   cp mdk_predator.ppma /path/to/sd_card/APPS/

   # Copy configuration
   mkdir -p /path/to/sd_card/MDK-PREDATOR/config
   cp mdk_predator.conf /path/to/sd_card/MDK-PREDATOR/config/
   ```

3. **Install SD Card**
   - Safely eject SD card from computer
   - Insert into PortaPack SD card slot
   - Power on device

4. **Launch Application**
   - Navigate to **Apps** menu on PortaPack
   - Scroll to find **MDK-Predator**
   - Press **Select** to launch

### Method B: Firmware Integration

This method integrates MDK-Predator directly into the firmware (advanced users).

#### Steps

1. **Build Firmware with MDK-Predator** (as described in Building section)

2. **Flash Firmware**
   ```bash
   # Using DFU mode
   dfu-util -d 1fc9:000c -D firmware.bin
   ```

3. **Verify Installation**
   - Power on PortaPack
   - MDK-Predator should appear in main menu

## Installation Steps

### Quick Start

1. **Hardware Setup**
   - Connect appropriate antenna for your target frequency
   - Insert SD card with application
   - Connect power supply or battery

2. **Power On**
   - Turn on HackRF One
   - Wait for PortaPack to boot (10-15 seconds)
   - Check that Mayhem firmware loads successfully

3. **Launch MDK-Predator**
   - From main menu, select **Apps**
   - Navigate to **MDK-Predator**
   - Press **Select/Enter**

4. **Initial Configuration**
   - Application will initialize with default settings
   - Status should show "Initialized"
   - All modules should be available

### Detailed Configuration

#### Edit Configuration File

1. Mount SD card on computer

2. Edit `/MDK-PREDATOR/config/mdk_predator.conf`:
   ```ini
   [General]
   default_module=automotive
   enable_logging=true

   [Hardware]
   sample_rate=2000000
   bandwidth=1750000
   lna_gain=8
   vga_gain=20

   [Security]
   allow_transmit=false
   require_confirmation=true
   log_all_activity=true

   [Automotive]
   default_frequency=433920000
   default_bandwidth=200000

   [SubGHz]
   frequency_min=300000000
   frequency_max=928000000
   step_size=100000
   ```

3. Save and safely eject SD card

4. Reinsert into PortaPack

5. Restart application to load new settings

## Verification

### Hardware Check

1. **Launch MDK-Predator**
2. **From main menu, note status message**
   - Should show "Status: Initialized"
   - If error shown, see [Troubleshooting](#troubleshooting)

### Module Testing

#### Test Automotive Module

1. Select **Automotive Security**
2. Select **Key Fob Analyzer**
3. Should show:
   ```
   Key Fob Analyzer: Started
   Frequency: 433.92 MHz
   Waiting for signals...
   ```
4. Press a car key fob near antenna
5. Application should detect signal

#### Test SubGHz Module

1. Select **SubGHz RF**
2. Select **Spectrum Scanner**
3. Should show:
   ```
   Spectrum Scanner: Started
   Range: 300-928 MHz
   ```
4. Spectrum display should show RF activity

#### Test WiFi Module

1. Select **WiFi Analysis**
2. Select **Network Scanner**
3. Should show:
   ```
   WiFi Scanner: Started
   Scanning all channels...
   ```
4. Nearby WiFi networks should be detected

### Diagnostic Test

MDK-Predator includes a built-in diagnostic:

1. From main menu, press **Menu** button
2. Select **Diagnostics**
3. Review diagnostic report:
   - Hardware: OK
   - Automotive module: OK
   - WiFi module: OK
   - Bluetooth module: OK
   - SubGHz module: OK
   - Crypto module: OK

All checks should show **OK** status.

## Troubleshooting

### Application Won't Launch

**Symptom**: App not visible in Apps menu or won't start

**Solutions**:
1. Verify SD card is properly inserted
2. Check that `mdk_predator.ppma` is in `/APPS/` directory
3. Verify SD card is formatted as FAT32
4. Try different SD card (some cards have compatibility issues)
5. Check firmware version (need 1.7.0+)

### "Init Failed" Error

**Symptom**: Status shows "Status: Init failed"

**Solutions**:
1. Check HackRF One is properly connected to PortaPack
2. Verify Mayhem-MDK module is properly seated
3. Check power supply is adequate (2A minimum)
4. Review configuration file for errors
5. Try default configuration (delete custom config file)

### No Signals Detected

**Symptom**: Modules work but no signals captured

**Solutions**:
1. Verify correct antenna for target frequency
2. Check antenna connection is tight
3. Increase gain settings in configuration
4. Verify signal source is active and in range
5. Check selected frequency matches signal source

### Module Startup Fails

**Symptom**: Specific module shows error on startup

**Solutions**:
1. Check module-specific configuration
2. Verify hardware supports module features
3. Review logs in `/MDK-PREDATOR/logs/`
4. Try with default module settings
5. Check for firmware conflicts

### Performance Issues

**Symptom**: Slow operation or freezing

**Solutions**:
1. Close other running applications
2. Use faster SD card (Class 10 or UHS-I)
3. Reduce sample rate in configuration
4. Disable logging if not needed
5. Restart PortaPack

### SD Card Not Recognized

**Symptom**: PortaPack won't read SD card

**Solutions**:
1. Format card as FAT32 (not exFAT)
2. Use 32GB or smaller card (some devices have limitations)
3. Try different SD card
4. Check for physical damage to card or slot
5. Clean SD card contacts

## Getting Help

### Log Files

Check log files for detailed error information:
- Location: `/MDK-PREDATOR/logs/`
- Latest log: `mdk_predator.log`

### Support Resources

- **GitHub Issues**: https://github.com/limbo111111/mdk-predator/issues
- **Documentation**: See repository `/docs` directory
- **Mayhem Forum**: PortaPack Mayhem community forums

### Reporting Issues

When reporting issues, include:
1. Firmware version
2. Hardware configuration
3. Log files from `/MDK-PREDATOR/logs/`
4. Steps to reproduce problem
5. Screenshots if applicable

## Safety and Legal

⚠️ **IMPORTANT WARNINGS**

### Legal Requirements

- Only use on systems you own or have explicit written permission to test
- Unauthorized wireless network access is illegal
- RF transmission may violate FCC/CE regulations
- Users are responsible for compliance with all laws

### Safety Precautions

- Start with receiving only (transmit disabled by default)
- Use appropriate antennas to avoid damage
- Avoid continuous operation at high power
- Don't operate near sensitive equipment
- Follow all local RF regulations

### Responsible Use

- Only for authorized security research
- Report vulnerabilities responsibly to manufacturers
- Document all testing activities
- Obtain proper authorization before testing
- Respect privacy and property rights

## Next Steps

After successful installation:

1. Review [User Guide](USER_GUIDE.md) for detailed usage instructions
2. Check [API Documentation](API.md) for integration details
3. Read [Architecture Guide](ARCHITECTURE.md) to understand system design
4. Configure modules for your specific research needs
5. Always verify authorization before testing

## Appendix: File Locations

### SD Card Structure

```
SD_CARD/
├── APPS/
│   └── mdk_predator.ppma          # Application binary
├── MDK-PREDATOR/
│   ├── config/
│   │   └── mdk_predator.conf      # Configuration
│   ├── logs/
│   │   └── mdk_predator.log       # Log files
│   ├── captures/
│   │   ├── automotive/            # Key fob captures
│   │   ├── wifi/                  # WiFi captures
│   │   ├── bluetooth/             # Bluetooth captures
│   │   └── subghz/                # SubGHz captures
│   └── data/
│       └── protocols/             # Protocol definitions
```

### Configuration Files

- Main config: `/MDK-PREDATOR/config/mdk_predator.conf`
- Module configs: `/MDK-PREDATOR/config/<module>.conf`

### Log Files

- Main log: `/MDK-PREDATOR/logs/mdk_predator.log`
- Module logs: `/MDK-PREDATOR/logs/<module>.log`

## Version History

- **1.0.0** (2024) - Initial release
  - Complete port from Flipper Zero
  - All core modules implemented
  - PortaPack Mayhem integration
  - Comprehensive documentation

---

For additional help, visit: https://github.com/limbo111111/mdk-predator
