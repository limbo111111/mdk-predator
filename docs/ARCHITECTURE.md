# MDK-Predator Architecture

## Overview

MDK-Predator is a security research suite ported from the Flipper Zero Predator Security Suite to work on the Mayhem-MDK module, which is an addon board for the HackRF Portapack H4M with Mayhem firmware.

## System Architecture

```
┌─────────────────────────────────────────────┐
│           MDK-Predator Main                 │
│          (mdk_predator.c/h)                 │
└─────────────┬───────────────────────────────┘
              │
    ┌─────────┼─────────┬─────────┬─────────┐
    │         │         │         │         │
┌───▼───┐ ┌───▼───┐ ┌───▼───┐ ┌───▼───┐ ┌───▼───┐
│Automo-│ │ WiFi  │ │Blueto-│ │SubGHz │ │Crypto │
│ tive  │ │Analyz-│ │ oth   │ │Analyz-│ │Analyz-│
│Module │ │  er   │ │Analyz-│ │  er   │ │  er   │
│       │ │       │ │  er   │ │       │ │       │
└───┬───┘ └───┬───┘ └───┬───┘ └───┬───┘ └───┬───┘
    │         │         │         │         │
    └─────────┴─────────┴─────────┴─────────┘
                       │
              ┌────────▼────────┐
              │  HackRF One +   │
              │  Mayhem-MDK     │
              │  Hardware       │
              └─────────────────┘
```

## Modules

### 1. Automotive Security Module
- **Key Fob Analyzer**: Captures and analyzes key fob signals
- **Rolling Code Tester**: Tests rolling code implementations (KeeLoq, HCS301, etc.)
- Supports frequencies: 315 MHz, 433.92 MHz, 868 MHz, 915 MHz

### 2. WiFi Security Module
- Network scanning and enumeration
- Security protocol detection (WEP, WPA, WPA2, WPA3)
- Handshake capture
- Deauthentication attack detection

### 3. Bluetooth Security Module
- Classic Bluetooth and BLE scanning
- Service enumeration
- Security analysis
- MITM vulnerability testing

### 4. SubGHz RF Module
- Spectrum scanning (300-928 MHz)
- Signal capture and analysis
- Protocol decoding (Princeton, Came, Nice FLO, etc.)
- Raw signal recording and replay

### 5. Cryptographic Analysis Module
- Algorithm identification
- Key exchange protocol analysis
- Weak key detection
- Rolling code cryptography analysis
- Entropy analysis

## Hardware Integration

### HackRF One
- SDR transceiver (1 MHz - 6 GHz)
- Sample rate: up to 20 MS/s
- 8-bit quadrature samples (I/Q)

### Mayhem-MDK Module
- Addon board for Portapack H4M
- Display and user interface
- Enhanced processing capabilities
- Additional storage

## Data Flow

1. **Signal Acquisition**: HackRF One captures RF signals
2. **Preprocessing**: Signal conditioning and filtering
3. **Analysis**: Module-specific signal analysis
4. **Protocol Decode**: Extract meaningful data
5. **Results**: Display or store results

## Security Considerations

- All modules are designed for **authorized security research only**
- Transmit functionality requires explicit confirmation
- Rolling code testing is passive by default
- Replay attacks are disabled unless explicitly enabled for research

## Build System

The project uses a Makefile-based build system targeting ARM Cortex-M4:
- Compiler: arm-none-eabi-gcc
- Architecture: Cortex-M4 with FPU
- Output: Static library (libmdk_predator.a)

## Configuration

Configuration is managed through `mdk_predator.conf`:
- Module defaults
- Hardware settings
- Security options
- Logging configuration

## Integration Points

### Mayhem Firmware
- Compatible with Mayhem firmware app framework
- Uses standard HackRF API
- Integrates with UI system

### File System
- Signal recordings stored in standard format
- Configuration files in /etc or user directory
- Logs in /tmp or designated log directory
