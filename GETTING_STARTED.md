# Getting Started with MDK-Predator

Welcome to MDK-Predator! This guide will help you get your security research suite up and running on your HackRF PortaPack H4M with Mayhem-MDK module.

## What is MDK-Predator?

MDK-Predator is a comprehensive security research suite ported from the Flipper Zero Predator Security Suite to work on the Mayhem-MDK platform. It provides professional-grade tools for:

- 🚗 **Automotive Security**: Key fob analysis and rolling code testing
- 📶 **WiFi Security**: Network scanning and security assessment
- 📱 **Bluetooth Security**: Device discovery and service enumeration
- 📡 **SubGHz RF**: Spectrum analysis and signal capture (300-928 MHz)
- 🔐 **Cryptographic Analysis**: Algorithm identification and entropy testing

## Choose Your Path

### 🚀 Quick Start (5 minutes)
**Best for**: First-time users who want to get running quickly

→ Follow the [QUICKSTART.md](QUICKSTART.md) guide

### 📚 Detailed Installation
**Best for**: Users who want comprehensive instructions

→ Follow the [DEPLOYMENT.md](docs/DEPLOYMENT.md) guide

### ✅ Installation Checklist
**Best for**: Ensuring complete and correct installation

→ Use the [INSTALLATION_CHECKLIST.md](docs/INSTALLATION_CHECKLIST.md)

## What You'll Need

### Hardware Requirements

1. **HackRF One**
   - SDR transceiver (1 MHz - 6 GHz)
   - USB connection

2. **PortaPack H4M**
   - LCD display and controls
   - SD card slot

3. **Mayhem-MDK Module**
   - Addon board for enhanced capabilities

4. **Accessories**
   - MicroSD card (8GB+, FAT32 formatted)
   - Antennas for target frequencies:
     - 315/433/868/915 MHz (automotive)
     - 2.4 GHz (WiFi, Bluetooth)
   - USB cable
   - Power supply (5V, 2A minimum)

### Software Requirements

- Mayhem firmware 1.7.0 or later
- MDK-Predator application (download or build)

## Installation Methods

### Method 1: Pre-built Application (Recommended)

1. Download `mdk_predator.ppma` from releases
2. Copy to SD card `/APPS/` directory
3. Copy configuration to `/MDK-PREDATOR/config/`
4. Launch from Apps menu

**Time required**: ~5 minutes

### Method 2: Build from Source

1. Clone repository
2. Run build script with Mayhem firmware path
3. Copy built `.ppma` to SD card
4. Launch from Apps menu

**Time required**: ~30 minutes (first build)

## Quick Setup Steps

### 1. Prepare SD Card
```
SD_CARD/
├── APPS/
│   └── mdk_predator.ppma
└── MDK-PREDATOR/
    └── config/
        └── mdk_predator.conf
```

### 2. Install
- Insert SD card into PortaPack
- Power on device
- Navigate to Apps menu

### 3. Launch
- Select **MDK-Predator**
- Application initializes
- Main menu appears

### 4. Start Using
- Choose a module (Automotive, WiFi, etc.)
- Connect appropriate antenna
- Begin your authorized security research

## Features Overview

### Main Application Menu

```
┌───────────────────────────┐
│ MDK-Predator              │
├───────────────────────────┤
│ > Automotive Security     │
│   WiFi Analysis           │
│   Bluetooth Analysis      │
│   SubGHz RF               │
│   Crypto Analysis         │
│   Exit                    │
├───────────────────────────┤
│ Status: Initialized       │
└───────────────────────────┘
```

### Module Capabilities

#### 🚗 Automotive Security
- Capture key fob signals (315/433.92/868/915 MHz)
- Analyze rolling codes (KeeLoq, HCS301)
- Research automotive encryption protocols

#### 📶 WiFi Analysis
- Scan networks (2.4 GHz & 5 GHz)
- Identify security protocols
- Assess network security

#### 📱 Bluetooth Analysis
- Discover Bluetooth Classic devices
- Scan for BLE devices
- Enumerate services
- Security testing

#### 📡 SubGHz RF
- Spectrum scanning (300-928 MHz)
- Signal capture and recording
- Protocol decoding
- Raw signal analysis

#### 🔐 Crypto Analysis
- Algorithm identification
- Entropy testing
- Key exchange analysis
- Weak key detection

## First Test

Try this simple test to verify everything works:

### Test WiFi Scanning

1. Launch MDK-Predator
2. Select **WiFi Analysis**
3. Select **Network Scanner**
4. Wait 10-15 seconds
5. You should see nearby WiFi networks!

**Success?** ✓ Your installation works!

**Issues?** → See Troubleshooting below

## Configuration

### Basic Configuration

Edit `/MDK-PREDATOR/config/mdk_predator.conf`:

```ini
[General]
default_module=automotive    # Starting module
enable_logging=true          # Enable logs

[Security]
allow_transmit=false         # KEEP FALSE for safety!
require_confirmation=true    # Confirm actions

[Hardware]
sample_rate=2000000         # Sample rate
lna_gain=8                  # LNA gain
vga_gain=20                 # VGA gain
```

### Important: Safety First! 🔒

- ✅ Transmit is **disabled by default** - keep it that way!
- ✅ Only receive/analyze mode is active
- ✅ Always get authorization before testing
- ✅ Follow local RF regulations

## Documentation Resources

### Essential Reading

1. **[QUICKSTART.md](QUICKSTART.md)**
   - 5-minute quick start guide
   - Basic usage examples

2. **[DEPLOYMENT.md](docs/DEPLOYMENT.md)**
   - Complete installation instructions
   - Building from source
   - Troubleshooting guide

3. **[USER_GUIDE.md](docs/USER_GUIDE.md)**
   - Detailed usage instructions
   - All modules explained
   - Advanced features

4. **[INSTALLATION_CHECKLIST.md](docs/INSTALLATION_CHECKLIST.md)**
   - Step-by-step checklist
   - Verification procedures

### Technical Documentation

- **[ARCHITECTURE.md](docs/ARCHITECTURE.md)** - System design
- **[API.md](docs/API.md)** - API reference
- **[README.md](README.md)** - Project overview

## Common Tasks

### Capture a Key Fob Signal

1. Connect 433 MHz antenna
2. Launch **Automotive Security**
3. Select **Key Fob Analyzer**
4. Press key fob button
5. View captured signal

### Scan WiFi Networks

1. Connect 2.4 GHz antenna
2. Launch **WiFi Analysis**
3. Select **Network Scanner**
4. Review discovered networks

### Spectrum Analysis

1. Connect appropriate antenna
2. Launch **SubGHz RF**
3. Select **Spectrum Scanner**
4. View RF spectrum

## Troubleshooting

### Application Won't Launch

**Check**:
- SD card formatted as FAT32
- File in `/APPS/` directory
- Firmware version 1.7.0+

**Solution**: Try different SD card

### "Init Failed" Error

**Check**:
- HackRF connection
- Mayhem-MDK module seated
- Power supply adequate

**Solution**: Restart device, check connections

### No Signals Detected

**Check**:
- Correct antenna connected
- Antenna connection tight
- Correct frequency selected

**Solution**: Increase gain, move closer to source

### Need More Help?

- 📖 Check [DEPLOYMENT.md](docs/DEPLOYMENT.md) troubleshooting section
- 🐛 Open an [issue on GitHub](https://github.com/limbo111111/mdk-predator/issues)
- 📚 Review documentation in `/docs` directory

## Safety and Legal

### ⚠️ Important Warnings

**Legal Requirements**:
- Only test systems you own or have written permission to test
- Unauthorized access is illegal
- RF transmission may violate regulations
- You are responsible for compliance

**Safety Precautions**:
- Transmit is disabled by default - **keep it that way!**
- Use proper antennas for frequencies
- Follow local RF regulations
- Document all testing activities

**Responsible Use**:
- For authorized security research only
- Report vulnerabilities responsibly
- Respect privacy and property rights

## Build Options

### Option 1: Use Pre-built Binary
Download from releases - easiest option!

### Option 2: Build with Script

**Linux/macOS:**
```bash
./scripts/build_portapack_app.sh -m /path/to/mayhem-firmware
```

**Windows:**
```powershell
.\scripts\build_portapack_app.ps1 -MayhemPath "C:\path\to\mayhem-firmware"
```

See [BUILDING_WINDOWS.md](docs/BUILDING_WINDOWS.md) for detailed Windows instructions.

### Option 3: Manual Build
See [DEPLOYMENT.md](docs/DEPLOYMENT.md) for instructions

## What's Next?

### After Installation

1. ✅ Complete [Installation Checklist](docs/INSTALLATION_CHECKLIST.md)
2. ✅ Read [User Guide](docs/USER_GUIDE.md)
3. ✅ Configure for your needs
4. ✅ Test all modules
5. ✅ Start authorized research!

### Learning More

- Review module-specific documentation
- Understand signal analysis techniques
- Learn about RF security best practices
- Join the community

## Support

### Getting Help

- 📖 **Documentation**: See `/docs` directory
- 🐛 **Issues**: [GitHub Issues](https://github.com/limbo111111/mdk-predator/issues)
- 💬 **Community**: PortaPack Mayhem forums
- 📧 **Discussions**: GitHub Discussions

### Reporting Issues

Include:
- Hardware configuration
- Firmware version
- Log files from `/MDK-PREDATOR/logs/`
- Steps to reproduce

## Version Information

- **Current Version**: 1.0.0
- **License**: MIT
- **Homepage**: https://github.com/limbo111111/mdk-predator
- **Based on**: Flipper Zero Predator Security Suite

## Quick Reference Card

### File Locations
- App: `/APPS/mdk_predator.ppma`
- Config: `/MDK-PREDATOR/config/mdk_predator.conf`
- Logs: `/MDK-PREDATOR/logs/`

### Key Features
- 5 security modules
- Multi-frequency support (300 MHz - 6 GHz)
- Receive-only default mode
- Comprehensive logging
- Professional-grade analysis

### Safety Defaults
- 🔒 Transmit: **DISABLED**
- 🔒 Mode: **RECEIVE ONLY**
- 🔒 Confirmation: **REQUIRED**

## Ready to Begin?

Choose your installation method:

1. **Quick Start** → [QUICKSTART.md](QUICKSTART.md)
2. **Detailed Guide** → [DEPLOYMENT.md](docs/DEPLOYMENT.md)
3. **Checklist** → [INSTALLATION_CHECKLIST.md](docs/INSTALLATION_CHECKLIST.md)

---

**Remember**: Only for authorized security research. Always verify you have permission before testing any system!

🚀 **Let's get started with MDK-Predator!**
