# MDK-Predator Quick Start Guide

Get up and running with MDK-Predator on your HackRF PortaPack H4M in 5 minutes!

## What You Need

- ✅ HackRF One with PortaPack H4M
- ✅ Mayhem-MDK module installed
- ✅ Mayhem firmware 1.7.0+ installed
- ✅ MicroSD card (8GB+, FAT32 formatted)
- ✅ Appropriate antenna (315/433/868/915 MHz or 2.4 GHz)

## Installation (3 Easy Steps)

### Step 1: Get the Application

**Option A: Download Pre-built (Easiest)**
1. Download `mdk_predator.ppma` from [Releases](https://github.com/limbo111111/mdk-predator/releases)

**Option B: Build from Source**
1. See [DEPLOYMENT.md](docs/DEPLOYMENT.md) for build instructions

### Step 2: Prepare SD Card

Create this folder structure on your SD card:

```
SD_CARD/
├── APPS/
│   └── mdk_predator.ppma
└── MDK-PREDATOR/
    ├── config/
    │   └── mdk_predator.conf
    └── logs/
```

Copy files:
```bash
# Copy application
cp mdk_predator.ppma /path/to/sd_card/APPS/

# Copy configuration
mkdir -p /path/to/sd_card/MDK-PREDATOR/config
cp mdk_predator.conf /path/to/sd_card/MDK-PREDATOR/config/
```

### Step 3: Install and Run

1. Insert SD card into PortaPack
2. Power on your device
3. Navigate to **Apps** menu
4. Select **MDK-Predator**
5. Application will initialize and show main menu

## Using MDK-Predator

### Main Menu

After launching, you'll see:

```
┌─────────────────────────────┐
│ MDK-Predator Security Suite │
│ Mayhem-MDK Module           │
│                             │
│ [Automotive Security]       │
│ [WiFi Analysis]             │
│ [Bluetooth Analysis]        │
│ [SubGHz RF]                 │
│ [Crypto Analysis]           │
│ [Exit]                      │
│                             │
│ Status: Initialized         │
└─────────────────────────────┘
```

### Quick Tests

#### Test 1: Capture Key Fob Signal

1. Connect 433 MHz antenna
2. Select **Automotive Security**
3. Select **Key Fob Analyzer** (button changes to "Stop Capture")
4. Watch the console for signal detection
5. View signal count and frequency display
6. Press button again to stop capture

**What you'll see:**
- Signal count updates in real-time
- Frequency: 433.92 MHz displayed
- Console shows capture details

#### Test 2: Scan WiFi Networks

1. Connect 2.4 GHz antenna
2. Select **WiFi Analysis**
3. Select **Start Scan** (button changes to "Stop Scan")
4. Watch discovered networks appear in console
5. View network count and status
6. Press "Security Analysis" to analyze findings

**What you'll see:**
- Networks found counter updates
- Network list with SSID, channel, signal strength
- Security protocol for each network
- Scan status indicator

#### Test 3: SubGHz Spectrum Scan

1. Connect appropriate antenna (315-928 MHz)
2. Select **SubGHz RF**
3. Select **Spectrum Scanner**
4. View frequency range and activity
5. Try **Signal Capture** to record signals
6. Use **Protocol Decoder** on captured signals

**What you'll see:**
- Frequency range: 300-928 MHz
- Activity at various frequencies
- Signal strength measurements
- Captured signal details

## Module Overview

### 🚗 Automotive Security
- **Key Fob Analyzer**: Capture and analyze key fob signals
- **Rolling Code Tester**: Test rolling code implementations
- Frequencies: 315, 433.92, 868, 915 MHz

### 📶 WiFi Analysis
- **Network Scanner**: Discover WiFi networks
- **Security Analysis**: Assess security protocols
- 2.4 GHz and 5 GHz support

### 📱 Bluetooth Analysis
- **Device Scanner**: Find Bluetooth devices
- **Service Enumeration**: List device services
- Classic and BLE support

### 📡 SubGHz RF
- **Spectrum Scanner**: View RF spectrum (300-928 MHz)
- **Signal Capture**: Record signals
- **Protocol Decoder**: Decode common protocols

### 🔐 Crypto Analysis
- **Algorithm Identifier**: Detect encryption types
- **Entropy Analysis**: Measure randomness
- **Key Analysis**: Test for weak keys

## Configuration

Edit `/MDK-PREDATOR/config/mdk_predator.conf` on SD card:

```ini
[General]
default_module=automotive       # Starting module
enable_logging=true             # Enable logging

[Hardware]
sample_rate=2000000            # 2 MS/s
bandwidth=1750000              # 1.75 MHz
lna_gain=8                     # Low noise amp gain
vga_gain=20                    # Variable gain amp

[Security]
allow_transmit=false           # Disable TX (keep false!)
require_confirmation=true      # Confirm actions
log_all_activity=true         # Log everything
```

## Important Safety Notes

⚠️ **READ BEFORE USE**

### Legal Requirements
- ✅ Only test systems you own
- ✅ Get written authorization for security testing
- ✅ Follow local RF regulations (FCC/CE/etc.)
- ❌ Never test unauthorized systems
- ❌ Never enable transmit without authorization

### Hardware Safety
- ✅ Use correct antenna for frequency
- ✅ Check antenna connections
- ✅ Use adequate power supply (5V, 2A minimum)
- ❌ Don't operate continuously at high power
- ❌ Don't use near sensitive equipment

### Default Settings
- 🔒 Transmit **DISABLED** by default
- 🔒 Receive-only mode
- 🔒 Requires confirmation for sensitive operations

## Troubleshooting

### App Won't Start
- Check SD card is FAT32 formatted
- Verify file is in `/APPS/` directory
- Confirm firmware is 1.7.0 or later

### No Signals Detected
- Verify correct antenna for frequency
- Check antenna connection
- Increase gain in configuration
- Move closer to signal source

### "Init Failed" Error
- Check HackRF is properly connected
- Verify Mayhem-MDK module is seated correctly
- Ensure adequate power supply
- Try with default configuration

## Getting Help

### Documentation
- 📖 [Full User Guide](docs/USER_GUIDE.md)
- 🚀 [Deployment Guide](docs/DEPLOYMENT.md)
- 🏗️ [Architecture](docs/ARCHITECTURE.md)
- 📚 [API Reference](docs/API.md)

### Support
- 🐛 [Report Issues](https://github.com/limbo111111/mdk-predator/issues)
- 💬 Community Forums
- 📧 GitHub Discussions

### Logs
Check `/MDK-PREDATOR/logs/mdk_predator.log` for detailed errors

## What's Next?

1. ✅ Review [User Guide](docs/USER_GUIDE.md) for detailed instructions
2. ✅ Read module-specific documentation
3. ✅ Configure for your research needs
4. ✅ Always verify authorization before testing
5. ✅ Report vulnerabilities responsibly

## Example Use Cases

### Automotive Research (Authorized)
1. Test your own vehicle's key fob security
2. Research rolling code implementations
3. Analyze key fob signals for weaknesses

### Wireless Security (Authorized)
1. Assess your home network security
2. Test Bluetooth device security
3. Analyze SubGHz RF devices (garage openers, etc.)

### Cryptographic Research
1. Analyze encryption algorithms
2. Test entropy quality
3. Research protocol security

## Version Information

- **Version**: 1.0.0
- **Release Date**: 2024
- **License**: MIT
- **Homepage**: https://github.com/limbo111111/mdk-predator

## Credits

Based on the Flipper Zero Predator Security Suite, adapted for the Mayhem-MDK platform.

---

🔐 **Remember**: Only for authorized security research. You are responsible for compliance with all applicable laws.

**Ready to start?** Launch MDK-Predator from your PortaPack Apps menu!
