# Changelog

All notable changes to the MDK-Predator project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-10-22

### Added
- Initial release of MDK-Predator
- Automotive Security Module
  - Key fob signal analyzer with support for 315/433.92/868/915 MHz
  - Rolling code tester with KeeLoq implementation
  - Support for multiple automotive protocols
- Wireless Security Module
  - WiFi analyzer with network scanning and security assessment
  - Bluetooth analyzer for Classic and BLE devices
  - SubGHz RF analyzer (300-928 MHz) with protocol decoding
- Cryptographic Analysis Module
  - Algorithm identification
  - Key exchange protocol analysis
  - Entropy analysis
  - Rolling code cryptography research
- Hardware Integration
  - HackRF One support
  - Mayhem-MDK module compatibility
  - ARM Cortex-M4 optimized build
- Documentation
  - Complete API reference
  - User guide with examples
  - Architecture documentation
  - Contributing guidelines
- Build System
  - Makefile for ARM toolchain
  - Static library output
- Configuration
  - Config file support (mdk_predator.conf)
  - Module-specific settings
  - Security options

### Security
- All transmit functions disabled by default
- Require explicit authorization for active testing
- Comprehensive legal disclaimers
- Responsible research guidelines

### Excluded
- RFID/NFC access control features (per project requirements)
- Demo/example code (production-ready only)

## [Unreleased]

### Added
- Test directory structure with subdirectories for automotive, wireless, crypto, and integration tests
- Test suite README with guidelines and safety considerations
- `make test` target for running hardware tests
- Updated .PHONY targets in Makefile

### Planned
- Extended protocol support
- Performance optimizations
- Additional test coverage
- UI integration examples
- Signal database management

---

## Version History

- **1.0.0** (2025-10-22): Initial release with full feature set

## Notes

This project is based on the Flipper Zero Predator Security Suite, adapted for the Mayhem-MDK platform.
