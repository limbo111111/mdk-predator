# MDK-Predator User Guide

## Introduction

MDK-Predator is a comprehensive security research suite for the Mayhem-MDK platform (HackRF Portapack H4M with Mayhem firmware). It provides tools for automotive security research, wireless protocol analysis, and cryptographic testing.

## Features

### Automotive Security
- **Key Fob Analysis**: Capture and analyze automotive key fob signals
- **Rolling Code Testing**: Research rolling code implementations (KeeLoq, HCS301, etc.)
- **Frequency Support**: 315 MHz, 433.92 MHz, 868 MHz, 915 MHz

### Wireless Security
- **WiFi Analysis**: Network scanning, security assessment, handshake capture
- **Bluetooth Analysis**: Device discovery, service enumeration, security testing
- **SubGHz RF**: Signal capture, protocol decoding, spectrum analysis

### Cryptographic Research
- **Algorithm Identification**: Detect encryption algorithms in use
- **Key Analysis**: Test for weak keys and vulnerabilities
- **Protocol Analysis**: Analyze key exchange and authentication protocols
- **Entropy Testing**: Measure randomness quality

## Installation

### Prerequisites
- HackRF One with Mayhem-MDK module
- Mayhem firmware installed
- ARM toolchain (arm-none-eabi-gcc)

### Building from Source

```bash
# Clone the repository
git clone https://github.com/limbo111111/mdk-predator.git
cd mdk-predator

# Build the library
make

# The compiled library will be in build/lib/libmdk_predator.a
```

### Configuration

Edit `mdk_predator.conf` to customize settings:

```ini
[General]
default_module=automotive
enable_logging=true

[Security]
allow_transmit=false  # Keep disabled unless authorized
```

## Usage

### Starting MDK-Predator

1. Power on your HackRF Portapack H4M
2. Load the MDK-Predator application
3. Select your desired security module

### Module Selection

Use the navigation buttons to switch between modules:
- **Automotive**: Key fob and rolling code analysis
- **WiFi**: WiFi security research
- **Bluetooth**: Bluetooth analysis
- **SubGHz**: General SubGHz RF analysis
- **Crypto**: Cryptographic analysis

## Module Guides

### Automotive Module

#### Capturing Key Fob Signals

1. Select "Automotive" module
2. Choose "Key Fob Analyzer"
3. Set frequency (315, 433.92, 868, or 915 MHz)
4. Press "Capture" and activate the key fob
5. View analysis results

#### Rolling Code Testing

1. Select "Rolling Code Tester"
2. Choose algorithm type (KeeLoq, HCS301, etc.)
3. Capture multiple signals
4. Analyze sequence for patterns

**Note**: Only for authorized security research on owned devices.

### WiFi Module

#### Network Scanning

1. Select "WiFi" module
2. Choose "Network Scanner"
3. Press "Scan" to start
4. View discovered networks and security types

#### Security Analysis

1. Select a network from the scan results
2. Choose "Analyze Security"
3. View detailed security assessment

### Bluetooth Module

#### Device Discovery

1. Select "Bluetooth" module
2. Choose scan type (Classic, BLE, or Both)
3. Press "Scan"
4. View discovered devices

#### Service Enumeration

1. Select a device from scan results
2. Choose "Enumerate Services"
3. View available services and UUIDs

### SubGHz Module

#### Spectrum Scanning

1. Select "SubGHz" module
2. Set frequency range (300-928 MHz)
3. Press "Scan Spectrum"
4. View power levels across frequencies

#### Signal Capture

1. Choose "Capture Signal"
2. Set target frequency
3. Press "Capture"
4. View or save captured signal

#### Protocol Decoding

1. Capture a signal (or load from file)
2. Choose "Decode Protocol"
3. Select protocol type or auto-detect
4. View decoded data

### Crypto Module

#### Entropy Analysis

1. Select "Crypto" module
2. Choose "Entropy Analysis"
3. Capture or load data
4. View entropy metrics

## Safety and Legal Considerations

### Important Warnings

⚠️ **Use only on networks and devices you own or have explicit permission to test**

⚠️ **Unauthorized access to wireless networks is illegal**

⚠️ **RF transmission may violate local regulations**

### Best Practices

1. **Always get written authorization** before testing any system
2. **Disable transmit functions** unless absolutely necessary
3. **Follow local RF regulations** (FCC, CE, etc.)
4. **Document your research** with proper notes and timestamps
5. **Report vulnerabilities responsibly** to manufacturers

## Troubleshooting

### Hardware Not Detected

- Check HackRF One connections
- Verify Mayhem-MDK module is properly seated
- Restart the device

### No Signals Captured

- Verify correct frequency selection
- Check antenna connection
- Ensure signal source is active
- Increase gain settings

### Build Errors

- Verify ARM toolchain is installed
- Check compiler version compatibility
- Ensure all dependencies are present

## Advanced Features

### Custom Protocol Decoding

Users can add custom protocol decoders by:
1. Implementing decoder functions
2. Registering with the SubGHz module
3. Recompiling the library

### Signal Recording

Captured signals can be saved to file for later analysis:
- Format: Complex I/Q samples
- Metadata includes frequency, sample rate, timestamp

### Logging

Enable detailed logging in configuration:
```ini
[Logging]
log_level=3  # 0=ERROR, 1=WARNING, 2=INFO, 3=DEBUG
```

## Performance Tips

1. **Buffer Size**: Increase for longer captures (default: 1MB)
2. **Sample Rate**: Lower for narrowband signals (saves memory)
3. **Filtering**: Enable hardware filters to reduce noise

## Support

For issues, questions, or contributions:
- GitHub: https://github.com/limbo111111/mdk-predator
- Source: https://github.com/limbo111111/Predator-Security-Suite

## License

See LICENSE file for details.

## Disclaimer

This software is provided for educational and authorized security research purposes only. Users are responsible for complying with all applicable laws and regulations. The authors assume no liability for misuse of this software.
