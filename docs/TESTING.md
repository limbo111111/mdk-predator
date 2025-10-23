# MDK-Predator Testing Guide

This document provides comprehensive information about testing the MDK-Predator security research suite.

## Table of Contents

- [Overview](#overview)
- [Test Structure](#test-structure)
- [Running Tests](#running-tests)
- [Test Coverage](#test-coverage)
- [Writing New Tests](#writing-new-tests)
- [Continuous Integration](#continuous-integration)

## Overview

MDK-Predator includes a comprehensive test suite that validates all security modules without requiring actual hardware. Tests are designed to:

- Verify module initialization and cleanup
- Validate input parameter checking
- Test error handling paths
- Ensure API contract compliance
- Prevent regressions

## Test Structure

Tests are organized by module category:

```
tests/
├── automotive/          # Automotive security tests
│   ├── test_key_fob_analyzer.c
│   └── test_rolling_code_tester.c
├── wireless/            # Wireless security tests
│   ├── test_wifi_analyzer.c
│   ├── test_bluetooth_analyzer.c
│   └── test_subghz_analyzer.c
├── crypto/              # Cryptographic tests
│   └── test_crypto_analyzer.c
└── integration/         # Integration tests
    └── test_mdk_integration.c
```

## Running Tests

### All Tests

Run the complete test suite:

```bash
make test
```

### Module-Specific Tests

Run tests for specific modules:

```bash
# Automotive security tests
make test-automotive

# Wireless security tests
make test-wireless

# Cryptographic analysis tests
make test-crypto

# Integration tests
make test-integration
```

### Clean Test Build

Clean test artifacts and rebuild:

```bash
make test-clean
make test
```

## Test Coverage

### Current Coverage

| Module | Test File | Assertions | Status |
|--------|-----------|------------|--------|
| Key Fob Analyzer | test_key_fob_analyzer.c | 17 | ✅ Pass |
| Rolling Code Tester | test_rolling_code_tester.c | 24 | ✅ Pass |
| WiFi Analyzer | test_wifi_analyzer.c | 18 | ✅ Pass |
| Bluetooth Analyzer | test_bluetooth_analyzer.c | 21 | ✅ Pass |
| SubGHz Analyzer | test_subghz_analyzer.c | 16 | ✅ Pass |
| Crypto Analyzer | test_crypto_analyzer.c | 20 | ✅ Pass |
| MDK Integration | test_mdk_integration.c | 24 | ✅ Pass |

**Total: 7 test suites, 140 assertions, 100% pass rate**

### What's Tested

#### Initialization and Cleanup
- Valid configuration initialization
- NULL parameter handling
- Default value verification
- Safe cleanup operations

#### Function Parameters
- NULL pointer handling
- Invalid parameter detection
- Boundary condition testing
- Buffer overflow prevention

#### Module Functionality
- Core feature validation
- API contract compliance
- Error path testing
- State management

## Writing New Tests

### Test File Template

Use this template for new test files:

```c
/**
 * Unit tests for [Module Name]
 *
 * These tests validate the [module] functionality without
 * requiring actual hardware.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "[module_header.h]"

/* Test counter */
static int tests_passed = 0;
static int tests_failed = 0;

/* Test helper macros */
#define TEST_ASSERT(condition, message) do { \
    if (condition) { \
        printf("  ✓ %s\n", message); \
        tests_passed++; \
    } else { \
        printf("  ✗ %s\n", message); \
        tests_failed++; \
    } \
} while(0)

#define RUN_TEST(test_func) do { \
    printf("\n[TEST] %s\n", #test_func); \
    test_func(); \
} while(0)

/**
 * Test: [Test description]
 */
void test_example() {
    // Arrange
    // Act
    // Assert
    TEST_ASSERT(true, "Test description");
}

/**
 * Main test runner
 */
int main(void) {
    printf("========================================\n");
    printf("[Module Name] Unit Tests\n");
    printf("========================================\n");

    RUN_TEST(test_example);

    printf("\n========================================\n");
    printf("Test Results:\n");
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
```

### Best Practices

1. **Test Naming**: Use descriptive names that indicate what is being tested
   - Format: `test_[function]_[scenario]`
   - Example: `test_wifi_init_null_config`

2. **Test Structure**: Follow the Arrange-Act-Assert pattern
   ```c
   void test_example() {
       // Arrange - Set up test data
       config_t config;

       // Act - Call the function
       bool result = function_init(&config);

       // Assert - Verify results
       TEST_ASSERT(result == true, "Should succeed");
   }
   ```

3. **Error Cases**: Always test error paths
   - NULL pointer parameters
   - Invalid parameter values
   - Boundary conditions
   - Resource exhaustion

4. **Independence**: Tests should be independent
   - Don't rely on test execution order
   - Clean up after each test
   - Use fresh data structures

5. **Assertions**: Use descriptive messages
   - Good: "Init should fail with NULL config"
   - Bad: "Test failed"

### Adding Tests to Makefile

When adding a new test file, update the Makefile:

```makefile
# Add source file variable
TEST_NEWMODULE_SRC = $(wildcard $(TEST_DIR)/newmodule/*.c)

# Add to all tests
ALL_TEST_SRC = ... $(TEST_NEWMODULE_SRC)

# Add binary variable
TEST_NEWMODULE_BIN = $(patsubst $(TEST_DIR)/newmodule/%.c,$(TEST_BIN_DIR)/newmodule/%,$(TEST_NEWMODULE_SRC))

# Add to all binaries
ALL_TEST_BIN = ... $(TEST_NEWMODULE_BIN)

# Add build rule
$(TEST_BIN_DIR)/newmodule/%: $(TEST_DIR)/newmodule/%.c $(NEWMODULE_SRC) test-directories
	@echo "Building test: $@"
	$(TEST_CC) $(TEST_CFLAGS) $(TEST_INCLUDES) $< $(NEWMODULE_SRC) -o $@
```

## Continuous Integration

Tests are automatically run on:
- Pull requests
- Commits to main branch
- Scheduled daily builds

### CI Requirements

- All tests must pass
- No compiler warnings
- Code coverage > 80%
- Static analysis clean

### Local Pre-commit Testing

Before committing, run:

```bash
# Build everything
make clean && make

# Run all tests
make test

# Check for warnings
make 2>&1 | grep -i warning
```

## Troubleshooting

### Test Build Failures

**Problem**: Test compilation fails with "undefined reference"

**Solution**: Ensure source files are included in Makefile test build rules

**Problem**: Missing header files

**Solution**: Check that `-Iinclude` is in TEST_INCLUDES

### Test Execution Failures

**Problem**: Segmentation fault in tests

**Solution**: Check for NULL pointer dereferences in test code

**Problem**: Tests fail on one platform but pass on another

**Solution**: Check for platform-specific assumptions (endianness, type sizes)

## Additional Resources

- [Architecture Documentation](ARCHITECTURE.md)
- [API Reference](API.md)
- [Contributing Guidelines](../CONTRIBUTING.md)
- [Test README](../tests/README.md)

## Support

For test-related questions or issues:
1. Check existing test files for examples
2. Review this documentation
3. Open an issue on GitHub with:
   - Test output
   - Expected behavior
   - Actual behavior
   - Steps to reproduce
