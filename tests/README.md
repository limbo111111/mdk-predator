# MDK-Predator Test Suite

This directory contains the test suite for MDK-Predator modules.

## Overview

The test suite validates the functionality of all MDK-Predator modules. Tests are written in C and can be run without actual hardware for unit testing purposes.

## Test Structure

```
tests/
├── automotive/     # Automotive security module tests
│   ├── test_key_fob_analyzer.c
│   └── test_rolling_code_tester.c
├── wireless/       # Wireless security module tests
│   └── test_subghz_analyzer.c
├── crypto/         # Cryptographic analysis tests
│   └── test_crypto_analyzer.c
└── integration/    # Integration tests
    └── test_mdk_integration.c
```

## Running Tests

### Build and Run All Tests

```bash
# Build the library first
make

# Build and run all tests
make test
```

### Run Specific Test Suites

```bash
# Run automotive tests only
make test-automotive

# Run wireless tests only
make test-wireless

# Run crypto tests only
make test-crypto

# Run integration tests only
make test-integration
```

### Clean Test Artifacts

```bash
make test-clean
```

## Test Coverage

### Automotive Module Tests (35 assertions)
- **test_key_fob_analyzer.c**: Tests for key fob signal analysis
  - Configuration initialization
  - Signal capture and analysis
  - Rolling code detection
  - Error handling and cleanup

- **test_rolling_code_tester.c**: Tests for rolling code analysis
  - KeeLoq algorithm testing
  - Sequence analysis
  - Replay vulnerability testing
  - Configuration management

### Wireless Module Tests (16 assertions)
- **test_subghz_analyzer.c**: Tests for SubGHz RF analysis
  - Spectrum scanning
  - Signal capture and analysis
  - Protocol decoding
  - Configuration and cleanup

### Crypto Module Tests (20 assertions)
- **test_crypto_analyzer.c**: Tests for cryptographic analysis
  - Algorithm identification
  - Key exchange analysis
  - Weak key detection
  - Rolling code cryptography
  - Entropy analysis

### Integration Tests (24 assertions)
- **test_mdk_integration.c**: Tests for system integration
  - Module initialization and coordination
  - Status management
  - Module switching
  - System diagnostics
  - Version information

## Test Results

All tests are currently passing:
- **Total test suites**: 5
- **Total assertions**: 95
- **Pass rate**: 100%

## Test Guidelines

### Unit Tests
- Test individual functions in isolation
- Use mock data when possible
- Verify error handling
- Test boundary conditions
- Validate NULL pointer handling

### Hardware Tests
Note: Current tests are designed to run without hardware. Hardware-specific tests can be added later and will require:
- Actual HackRF hardware
- Test signal capture and analysis
- Validate protocol decoding with real signals

### Integration Tests
- Test module interactions
- Verify data flow
- Test configuration loading
- Validate state management

## Adding New Tests

1. Create test file in appropriate directory:
   ```
   tests/<category>/test_<module_name>.c
   ```

2. Follow existing test structure:
   - Use TEST_ASSERT macro for assertions
   - Use RUN_TEST macro for test execution
   - Include test counters for pass/fail tracking

3. Include appropriate headers:
   ```c
   #include <stdio.h>
   #include <stdlib.h>
   #include "module_header.h"
   ```

4. Test naming convention:
   ```c
   void test_<module>_<function>_<scenario>()
   ```

5. Build and run:
   ```bash
   make test
   ```

## Safety Considerations

⚠️ **WARNING**: Testing with RF signals requires:
- Proper RF shielding for hardware tests
- Authorized frequency usage
- Compliance with local regulations (FCC, CE, etc.)

## Continuous Integration

Tests are designed to be easily integrated into CI/CD pipelines:
- Exit code 0 on success
- Exit code 1 on failure
- Clear pass/fail reporting
- No hardware dependencies for current tests

## Notes

- Tests use standard C library only (no special test frameworks)
- All tests include comprehensive error handling validation
- Tests are self-contained and can run in any order
- Build artifacts are in `build/tests/` (ignored by git)
- Tests compile with native gcc (not ARM cross-compiler)
