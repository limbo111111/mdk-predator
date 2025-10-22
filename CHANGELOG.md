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
- Comprehensive test suite with 95 assertions across 5 test files
  - Automotive module tests: test_key_fob_analyzer.c, test_rolling_code_tester.c
  - Wireless module tests: test_subghz_analyzer.c
  - Crypto module tests: test_crypto_analyzer.c
  - Integration tests: test_mdk_integration.c
- Enhanced Makefile with test build system
  - `make test` - Build and run all tests
  - `make test-automotive` - Run automotive tests only
  - `make test-wireless` - Run wireless tests only
  - `make test-crypto` - Run crypto tests only
  - `make test-integration` - Run integration tests only
  - `make test-clean` - Clean test artifacts
- Updated tests/README.md with comprehensive test documentation
- Test directory structure with subdirectories for automotive, wireless, crypto, and integration tests
- `make test` target for running all tests with summary reporting

### Changed
- Tests now run without hardware requirements (unit tests with mock data)
- Test build uses native gcc instead of ARM cross-compiler
- All tests passing with 100% success rate

### Planned
- Extended protocol support
- Performance optimizations
- Hardware-specific test cases
- UI integration examples
- Signal database management

---

## Version History

- **1.0.0** (2025-10-22): Initial release with full feature set

## Notes

This project is based on the Flipper Zero Predator Security Suite, adapted for the Mayhem-MDK platform.
