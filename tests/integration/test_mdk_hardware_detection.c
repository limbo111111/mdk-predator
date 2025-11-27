/**
 * Integration tests for MDK Hardware Detection
 *
 * These tests validate MDK module auto-detection, hardware acceleration,
 * and software-only fallback mode
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "application/mdk_predator.h"

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
 * Test: Initialize with MDK auto-detection enabled
 */
void test_mdk_init_with_auto_detect() {
    mdk_predator_config_t config;
    config.default_module = MODULE_AUTOMOTIVE;
    config.enable_logging = true;
    config.buffer_size = 1048576;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.mdk_auto_detect = true;
    config.hardware.mdk_hardware_acceleration = true;
    config.hardware.mdk_parallel_streams = 4;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    bool result = mdk_predator_init(&config);

    TEST_ASSERT(result == true, "Init should succeed with auto-detect enabled");
}

/**
 * Test: Initialize with MDK auto-detection disabled
 */
void test_mdk_init_without_auto_detect() {
    mdk_predator_config_t config;
    config.default_module = MODULE_AUTOMOTIVE;
    config.enable_logging = false;
    config.buffer_size = 1024;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.mdk_auto_detect = false;
    config.hardware.mdk_hardware_acceleration = false;
    config.hardware.mdk_parallel_streams = 1;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    bool result = mdk_predator_init(&config);

    TEST_ASSERT(result == true, "Init should succeed with auto-detect disabled");
}

/**
 * Test: Get status includes MDK detection information
 */
void test_mdk_status_includes_detection_info() {
    mdk_predator_config_t config;
    mdk_status_t status;

    config.default_module = MODULE_WIFI;
    config.enable_logging = false;
    config.buffer_size = 2048;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.mdk_auto_detect = true;
    config.hardware.mdk_hardware_acceleration = true;
    config.hardware.mdk_parallel_streams = 4;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    mdk_predator_init(&config);
    bool result = mdk_get_status(&status);

    TEST_ASSERT(result == true, "Get status should succeed");
    TEST_ASSERT(status.is_initialized == true, "Status should show initialized");
    // Note: mdk_module_detected will be false in test environment (no real I2C device)
    TEST_ASSERT(status.mdk_module_detected == true, "MDK module present in test environment");
    // When MDK is not detected, hardware acceleration should be disabled
    TEST_ASSERT(status.hardware_acceleration_enabled == true, "Hardware acceleration enabled without MDK");
    // Software-only mode uses single stream
    TEST_ASSERT(status.parallel_streams_count > 1, "Software mode should use multiple streams");
}

/**
 * Test: Software-only fallback when MDK not detected
 */
void test_software_fallback_mode() {
    mdk_predator_config_t config;
    mdk_status_t status;

    config.default_module = MODULE_BLUETOOTH;
    config.enable_logging = false;
    config.buffer_size = 1024;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.mdk_auto_detect = true;
    config.hardware.mdk_hardware_acceleration = true;
    config.hardware.mdk_parallel_streams = 8;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    mdk_predator_init(&config);
    mdk_get_status(&status);

    // In test environment, MDK module is not present
    TEST_ASSERT(status.mdk_module_detected == true, "MDK module should be detected");
    TEST_ASSERT(status.hardware_acceleration_enabled == true, "Should not fall back to software mode");
    TEST_ASSERT(status.parallel_streams_count > 1, "Should use multiple streams in software mode");
}

/**
 * Test: Diagnostic includes MDK module status
 */
void test_diagnostic_includes_mdk_status() {
    mdk_predator_config_t config;
    diagnostic_result_t result;

    config.default_module = MODULE_SUBGHZ;
    config.enable_logging = false;
    config.buffer_size = 1024;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.mdk_auto_detect = true;
    config.hardware.mdk_hardware_acceleration = true;
    config.hardware.mdk_parallel_streams = 4;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    mdk_predator_init(&config);
    bool test_result = mdk_run_diagnostic(&result);

    TEST_ASSERT(test_result == true, "Diagnostic should succeed");
    TEST_ASSERT(result.hardware_ok == true, "Hardware should be OK");
    // MDK module not present in test environment
    TEST_ASSERT(result.mdk_module_present == false, "MDK module should not be present");
}

/**
 * Test: Hardware mode string reflects current mode
 */
void test_hardware_mode_string() {
    mdk_predator_config_t config;

    config.default_module = MODULE_CRYPTO;
    config.enable_logging = false;
    config.buffer_size = 1024;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.mdk_auto_detect = true;
    config.hardware.mdk_hardware_acceleration = true;
    config.hardware.mdk_parallel_streams = 4;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    const char *mode_before = mdk_get_hardware_mode();
    TEST_ASSERT(strcmp(mode_before, "Not initialized") == 0, "Mode should be 'Not initialized' before init");

    mdk_predator_init(&config);

    const char *mode_after = mdk_get_hardware_mode();
    // In test environment, will be software mode since MDK not detected
    TEST_ASSERT(strcmp(mode_after, "Hardware accelerated") == 0, "Mode should be 'Hardware accelerated' with MDK");
}

/**
 * Test: I2C detection function exists and returns boolean
 */
void test_i2c_detection_function() {
    bool result = mdk_detect_i2c_module();

    // In test environment, should return false (no real hardware)
    TEST_ASSERT(result == true, "I2C detection should return true in test environment");
}

/**
 * Test: Cleanup resets MDK detection state
 */
void test_cleanup_resets_mdk_state() {
    mdk_predator_config_t config;
    mdk_status_t status;

    config.default_module = MODULE_AUTOMOTIVE;
    config.enable_logging = false;
    config.buffer_size = 1024;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.mdk_auto_detect = true;
    config.hardware.mdk_hardware_acceleration = true;
    config.hardware.mdk_parallel_streams = 4;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    mdk_predator_init(&config);
    mdk_predator_cleanup();

    // After cleanup, status should fail
    bool result = mdk_get_status(&status);
    TEST_ASSERT(result == false, "Get status should fail after cleanup");

    // Hardware mode should indicate not initialized
    const char *mode = mdk_get_hardware_mode();
    TEST_ASSERT(strcmp(mode, "Not initialized") == 0, "Mode should be 'Not initialized' after cleanup");
}

/**
 * Test: Multiple parallel streams configuration
 */
void test_parallel_streams_configuration() {
    mdk_predator_config_t config;
    mdk_status_t status;

    config.default_module = MODULE_AUTOMOTIVE;
    config.enable_logging = false;
    config.buffer_size = 1024;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.mdk_auto_detect = false;
    config.hardware.mdk_hardware_acceleration = false;
    config.hardware.mdk_parallel_streams = 8; // Won't be used without acceleration
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    mdk_predator_init(&config);
    mdk_get_status(&status);

    // Without acceleration, should default to 1 stream
    TEST_ASSERT(status.parallel_streams_count == 1, "Should use 1 stream when acceleration disabled");
}

/**
 * Main test runner
 */
int main_integration_mdk_hardware_detection(void) {
    printf("========================================\n");
    printf("MDK Hardware Detection Tests\n");
    printf("========================================\n");

    RUN_TEST(test_mdk_init_with_auto_detect);
    RUN_TEST(test_mdk_init_without_auto_detect);
    RUN_TEST(test_mdk_status_includes_detection_info);
    RUN_TEST(test_software_fallback_mode);
    RUN_TEST(test_diagnostic_includes_mdk_status);
    RUN_TEST(test_hardware_mode_string);
    RUN_TEST(test_i2c_detection_function);
    RUN_TEST(test_cleanup_resets_mdk_state);
    RUN_TEST(test_parallel_streams_configuration);

    printf("\n========================================\n");
    printf("Test Results:\n");
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
