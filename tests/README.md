# MDK-Predator Test Suite

This directory contains the test suite for MDK-Predator modules.

## Overview

The test suite validates the functionality of all MDK-Predator modules on actual hardware.

## Test Structure

```
tests/
├── automotive/     # Automotive security module tests
├── wireless/       # Wireless security module tests
├── crypto/         # Cryptographic analysis tests
└── integration/    # Integration tests
```

## Hardware Requirements

Testing requires:
- HackRF One with Mayhem-MDK module
- Mayhem firmware installed
- Test signals for protocol validation

## Running Tests

```bash
# Build the library first
make

# Run all tests (requires hardware)
make test
```

## Test Guidelines

### Unit Tests
- Test individual functions in isolation
- Use mock data when possible
- Verify error handling

### Hardware Tests
- Require actual HackRF hardware
- Test signal capture and analysis
- Validate protocol decoding

### Integration Tests
- Test module interactions
- Verify data flow
- Test configuration loading

## Safety Considerations

⚠️ **WARNING**: Testing with RF signals requires:
- Proper RF shielding
- Authorized frequency usage
- Compliance with local regulations

## Adding New Tests

1. Create test file in appropriate directory
2. Follow existing test structure
3. Document test requirements
4. Include sample data if needed
5. Update this README

## Test Coverage

Current test coverage areas:
- Key fob signal capture
- Rolling code validation
- SubGHz protocol decoding
- WiFi security assessment
- Bluetooth enumeration
- Cryptographic analysis

## Notes

- Tests require hardware access
- Some tests need specific test signals
- Active transmission tests disabled by default
- Always verify legal compliance before testing
