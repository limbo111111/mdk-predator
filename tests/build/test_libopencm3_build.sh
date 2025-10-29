#!/bin/bash
#
# Test: Verify libopencm3 is built before application compilation
#
# This test ensures that the build scripts properly build libopencm3
# to generate nvic.h and other required headers before attempting to
# build the application, preventing the error:
#   fatal error: libopencm3/lpc43xx/m0/nvic.h: No such file or directory
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test counters
TESTS_RUN=0
TESTS_PASSED=0
TESTS_FAILED=0

# Helper functions
test_pass() {
    echo -e "${GREEN}✓ PASS${NC}: $1"
    TESTS_PASSED=$((TESTS_PASSED + 1))
}

test_fail() {
    echo -e "${RED}✗ FAIL${NC}: $1"
    TESTS_FAILED=$((TESTS_FAILED + 1))
}

print_info() {
    echo -e "${YELLOW}[INFO]${NC} $1"
}

# Test 1: Check docker-entrypoint.sh has build_libopencm3 function
test_docker_entrypoint_has_build_function() {
    TESTS_RUN=$((TESTS_RUN + 1))
    local script="docker-entrypoint.sh"

    if [ ! -f "$script" ]; then
        test_fail "$script does not exist"
        return
    fi

    if grep -q "build_libopencm3()" "$script"; then
        test_pass "$script contains build_libopencm3 function"
    else
        test_fail "$script does not contain build_libopencm3 function"
    fi
}

# Test 2: Check docker-entrypoint.sh calls build_libopencm3 before build
test_docker_entrypoint_calls_build() {
    TESTS_RUN=$((TESTS_RUN + 1))
    local script="docker-entrypoint.sh"

    # Extract the do_build function and check order
    if grep -A 50 "^do_build()" "$script" | grep -B 5 "integrate_mdk_predator\|build_with_make\|build_with_ninja" | grep -q "build_libopencm3"; then
        test_pass "$script calls build_libopencm3 before building application"
    else
        test_fail "$script does not call build_libopencm3 before building application"
    fi
}

# Test 3: Check build_portapack_app.sh has build_libopencm3 function
test_build_script_has_function() {
    TESTS_RUN=$((TESTS_RUN + 1))
    local script="scripts/build_portapack_app.sh"

    if [ ! -f "$script" ]; then
        test_fail "$script does not exist"
        return
    fi

    if grep -q "build_libopencm3()" "$script"; then
        test_pass "$script contains build_libopencm3 function"
    else
        test_fail "$script does not contain build_libopencm3 function"
    fi
}

# Test 4: Check build_portapack_app.sh calls build_libopencm3 before build
test_build_script_calls_build() {
    TESTS_RUN=$((TESTS_RUN + 1))
    local script="scripts/build_portapack_app.sh"

    # Check that build_libopencm3 is called in the main execution flow
    # It should be after verify_submodules and before check_requirements or integrate_with_mayhem
    if grep -A 20 "verify_submodules" "$script" | grep -B 5 "check_requirements\|integrate_with_mayhem" | grep -q "build_libopencm3"; then
        test_pass "$script calls build_libopencm3 before building application"
    else
        test_fail "$script does not call build_libopencm3 before building application"
    fi
}

# Test 5: Check WSL script has build_libopencm3 function
test_wsl_script_has_function() {
    TESTS_RUN=$((TESTS_RUN + 1))
    local script="scripts/build_portapack_app_wsl.sh"

    if [ ! -f "$script" ]; then
        test_fail "$script does not exist"
        return
    fi

    if grep -q "build_libopencm3()" "$script"; then
        test_pass "$script contains build_libopencm3 function"
    else
        test_fail "$script does not contain build_libopencm3 function"
    fi
}

# Test 6: Check WSL script calls build_libopencm3 before build
test_wsl_script_calls_build() {
    TESTS_RUN=$((TESTS_RUN + 1))
    local script="scripts/build_portapack_app_wsl.sh"

    # Check that build_libopencm3 is called in the main execution flow
    if grep -A 10 "verify_submodules" "$script" | grep -B 5 "check_dependencies\|integrate_with_mayhem" | grep -q "build_libopencm3"; then
        test_pass "$script calls build_libopencm3 before building application"
    else
        test_fail "$script does not call build_libopencm3 before building application"
    fi
}

# Test 7: Check PowerShell script has Build-Libopencm3 function
test_ps_script_has_function() {
    TESTS_RUN=$((TESTS_RUN + 1))
    local script="scripts/build_portapack_app.ps1"

    if [ ! -f "$script" ]; then
        test_fail "$script does not exist"
        return
    fi

    if grep -q "function Build-Libopencm3" "$script"; then
        test_pass "$script contains Build-Libopencm3 function"
    else
        test_fail "$script does not contain Build-Libopencm3 function"
    fi
}

# Test 8: Check PowerShell script calls Build-Libopencm3 before build
test_ps_script_calls_build() {
    TESTS_RUN=$((TESTS_RUN + 1))
    local script="scripts/build_portapack_app.ps1"

    # Check that Build-Libopencm3 is called in the main execution flow
    if grep -A 10 "Test-Submodules" "$script" | grep -B 5 "Test-AllDependencies\|Invoke-Integration" | grep -q "Build-Libopencm3"; then
        test_pass "$script calls Build-Libopencm3 before building application"
    else
        test_fail "$script does not call Build-Libopencm3 before building application"
    fi
}

# Test 9: Verify build functions check for nvic.h generation
test_functions_verify_nvic_generation() {
    TESTS_RUN=$((TESTS_RUN + 1))
    local found=0

    # Check docker-entrypoint.sh
    if grep -A 50 "build_libopencm3()" "docker-entrypoint.sh" | grep -q "nvic.h"; then
        found=$((found + 1))
    fi

    # Check build_portapack_app.sh
    if grep -A 50 "build_libopencm3()" "scripts/build_portapack_app.sh" | grep -q "nvic.h"; then
        found=$((found + 1))
    fi

    # Check build_portapack_app_wsl.sh
    if grep -A 50 "build_libopencm3()" "scripts/build_portapack_app_wsl.sh" | grep -q "nvic.h"; then
        found=$((found + 1))
    fi

    # Check build_portapack_app.ps1
    if grep -A 50 "function Build-Libopencm3" "scripts/build_portapack_app.ps1" | grep -q "nvic.h"; then
        found=$((found + 1))
    fi

    if [ $found -eq 4 ]; then
        test_pass "All build functions verify nvic.h generation"
    else
        test_fail "Only $found/4 build functions verify nvic.h generation"
    fi
}

# Test 10: Verify build functions build for lpc43xx/m4 and lpc43xx/m0 targets
test_functions_build_lpc43xx_target() {
    TESTS_RUN=$((TESTS_RUN + 1))
    local found=0

    # Check docker-entrypoint.sh
    if grep -A 50 "build_libopencm3()" "docker-entrypoint.sh" | grep -q 'TARGETS="lpc43xx/m4 lpc43xx/m0"'; then
        found=$((found + 1))
    fi

    # Check build_portapack_app.sh
    if grep -A 50 "build_libopencm3()" "scripts/build_portapack_app.sh" | grep -q 'TARGETS="lpc43xx/m4 lpc43xx/m0"'; then
        found=$((found + 1))
    fi

    # Check build_portapack_app_wsl.sh
    if grep -A 50 "build_libopencm3()" "scripts/build_portapack_app_wsl.sh" | grep -q 'TARGETS="lpc43xx/m4 lpc43xx/m0"'; then
        found=$((found + 1))
    fi

    # Check build_portapack_app.ps1
    if grep -A 50 "function Build-Libopencm3" "scripts/build_portapack_app.ps1" | grep -q 'TARGETS="lpc43xx/m4 lpc43xx/m0"'; then
        found=$((found + 1))
    fi

    if [ $found -eq 4 ]; then
        test_pass "All build functions build for lpc43xx/m4 and lpc43xx/m0 targets"
    else
        test_fail "Only $found/4 build functions build for lpc43xx/m4 and lpc43xx/m0 targets"
    fi
}

# Main test execution
main() {
    echo ""
    echo "========================================="
    echo "Testing libopencm3 Build Process"
    echo "========================================="
    echo ""

    print_info "This test verifies that all build scripts properly build libopencm3"
    print_info "before attempting to compile the application, preventing the error:"
    print_info "  fatal error: libopencm3/lpc43xx/m0/nvic.h: No such file or directory"
    echo ""

    # Run all tests
    test_docker_entrypoint_has_build_function
    test_docker_entrypoint_calls_build
    test_build_script_has_function
    test_build_script_calls_build
    test_wsl_script_has_function
    test_wsl_script_calls_build
    test_ps_script_has_function
    test_ps_script_calls_build
    test_functions_verify_nvic_generation
    test_functions_build_lpc43xx_target

    # Print summary
    echo ""
    echo "========================================="
    echo "Test Summary"
    echo "========================================="
    echo "Tests run:    $TESTS_RUN"
    echo -e "Tests passed: ${GREEN}$TESTS_PASSED${NC}"
    if [ $TESTS_FAILED -gt 0 ]; then
        echo -e "Tests failed: ${RED}$TESTS_FAILED${NC}"
    else
        echo "Tests failed: $TESTS_FAILED"
    fi
    echo "========================================="
    echo ""

    if [ $TESTS_FAILED -gt 0 ]; then
        echo -e "${RED}OVERALL: FAILED${NC}"
        exit 1
    else
        echo -e "${GREEN}OVERALL: PASSED${NC}"
        exit 0
    fi
}

# Run from repository root
cd "$(dirname "$0")/../.."

# Execute tests
main
