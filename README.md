# MDK-Predator

A comprehensive security research suite for the Mayhem-MDK platform, ported from the Flipper Zero Predator Security Suite.

## Overview

MDK-Predator is designed to work on the Mayhem-MDK module, an addon board for the HackRF Portapack H4M with Mayhem firmware. This project focuses on automotive and wireless security research capabilities.

**Source**: Based on [Predator Security Suite](https://github.com/limbo111111/Predator-Security-Suite)

## Features

### ✅ Automotive Security
- **Key Fob Analysis**: Capture and analyze automotive key fob signals (315/433.92/868/915 MHz)
- **Rolling Code Testing**: KeeLoq, HCS301, and other rolling code implementations
- **Cryptographic Protocol Research**: Analyze automotive encryption protocols

### ✅ Wireless Security
- **SubGHz RF**: Signal capture, protocol decoding, spectrum analysis (300-928 MHz)

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

## Building

### Prerequisites
- ARM toolchain (`arm-none-eabi-gcc`)
- HackRF One with Mayhem-MDK module
- Mayhem firmware

### Compile

```bash
make
```

The compiled library will be available at `build/lib/libmdk_predator.a`.

### Clean Build

```bash
make clean
```

## Hardware Requirements

- **HackRF One**: Software-defined radio transceiver (1 MHz - 6 GHz)
- **Mayhem-MDK Module**: Addon board for Portapack H4M
- **Mayhem Firmware**: Compatible firmware installed

## Documentation

- [Architecture Guide](docs/ARCHITECTURE.md) - System design and data flow
- [API Reference](docs/API.md) - Complete API documentation
- [User Guide](docs/USER_GUIDE.md) - Usage instructions and examples

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
