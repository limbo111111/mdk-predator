# Build Tests

This directory contains tests that verify the build system and build scripts are configured correctly.

## Tests

### test_libopencm3_build.sh

Verifies that all build scripts properly build libopencm3 before attempting to compile the application.

**Purpose**: Prevents the build error:
```
fatal error: libopencm3/lpc43xx/m0/nvic.h: No such file or directory
```

**What it tests**:
1. All build scripts have a function to build libopencm3
2. All build scripts call the build function before building the application
3. Build functions verify that nvic.h is generated
4. Build functions target the correct platform (lpc43xx)

**How to run**:
```bash
./tests/build/test_libopencm3_build.sh
```

**Scripts tested**:
- `docker-entrypoint.sh` (Docker builds)
- `scripts/build_portapack_app.sh` (Linux/macOS builds)
- `scripts/build_portapack_app_wsl.sh` (WSL builds)
- `scripts/build_portapack_app.ps1` (Windows PowerShell builds)

## Adding New Build Tests

Build tests should verify:
- Build script configuration
- Build process order
- Dependency handling
- Error prevention

Follow this structure:
```bash
#!/bin/bash
set -e

# Test counters
TESTS_RUN=0
TESTS_PASSED=0
TESTS_FAILED=0

# Helper functions
test_pass() {
    echo "✓ PASS: $1"
    TESTS_PASSED=$((TESTS_PASSED + 1))
}

test_fail() {
    echo "✗ FAIL: $1"
    TESTS_FAILED=$((TESTS_FAILED + 1))
}

# Your tests here
test_something() {
    TESTS_RUN=$((TESTS_RUN + 1))
    # Test logic
    if [ condition ]; then
        test_pass "Test description"
    else
        test_fail "Test description"
    fi
}

# Main execution
main() {
    echo "Running tests..."
    test_something
    
    # Print summary
    echo "Tests run: $TESTS_RUN"
    echo "Tests passed: $TESTS_PASSED"
    echo "Tests failed: $TESTS_FAILED"
    
    [ $TESTS_FAILED -eq 0 ] && exit 0 || exit 1
}

main
```

## Integration with CI/CD

These tests can be easily integrated into CI/CD pipelines:
- Exit code 0 on success
- Exit code 1 on failure
- Clear pass/fail reporting
- No external dependencies
- Fast execution
