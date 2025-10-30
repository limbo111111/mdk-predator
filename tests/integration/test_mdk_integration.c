/**
 * Integration tests for MDK-Predator Main Module
 *
 * These tests validate the integration and coordination between modules
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mdk_predator.h"

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
 * Test: Initialize MDK-Predator with valid config
 */
void test_mdk_init_valid() {
    mdk_predator_config_t config;
    config.default_module = MODULE_AUTOMOTIVE;
    config.enable_logging = true;
    config.buffer_size = 1024;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.parallel_streams = 4;
    config.hardware.use_dma = true;
    config.hardware.max_codes_per_second = 1000000;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    bool result = mdk_predator_init(&config);

    TEST_ASSERT(result == true, "Init should succeed with valid config");

    // Cleanup after test
    mdk_predator_cleanup();
}

/**
 * Test: Initialize MDK-Predator with null config
 */
void test_mdk_init_null() {
    bool result = mdk_predator_init(NULL);

    TEST_ASSERT(result == false, "Init should fail with NULL config");
}

/**
 * Test: Get status after initialization
 */
void test_mdk_get_status_after_init() {
    mdk_predator_config_t config;
    mdk_status_t status;

    config.default_module = MODULE_SUBGHZ;
    config.enable_logging = false;
    config.buffer_size = 2048;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.parallel_streams = 4;
    config.hardware.use_dma = true;
    config.hardware.max_codes_per_second = 1000000;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    mdk_predator_init(&config);
    bool result = mdk_get_status(&status);

    TEST_ASSERT(result == true, "Get status should succeed after init");
    TEST_ASSERT(status.is_initialized == true, "Status should show initialized");
    TEST_ASSERT(status.active_module == MODULE_SUBGHZ, "Active module should match config");

    mdk_predator_cleanup();
}

/**
 * Test: Get status before initialization
 */
void test_mdk_get_status_before_init() {
    mdk_status_t status;

    // Ensure cleanup to start from clean state
    mdk_predator_cleanup();

    bool result = mdk_get_status(&status);

    TEST_ASSERT(result == false, "Get status should fail before init");
}

/**
 * Test: Get status with null buffer
 */
void test_mdk_get_status_null() {
    mdk_predator_config_t config;
    config.default_module = MODULE_AUTOMOTIVE;
    config.enable_logging = false;
    config.buffer_size = 1024;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.parallel_streams = 4;
    config.hardware.use_dma = true;
    config.hardware.max_codes_per_second = 1000000;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    mdk_predator_init(&config);
    bool result = mdk_get_status(NULL);

    TEST_ASSERT(result == false, "Get status should fail with NULL buffer");

    mdk_predator_cleanup();
}

/**
 * Test: Set active module after initialization
 */
void test_mdk_set_active_module_valid() {
    mdk_predator_config_t config;
    config.default_module = MODULE_AUTOMOTIVE;
    config.enable_logging = false;
    config.buffer_size = 1024;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.parallel_streams = 4;
    config.hardware.use_dma = true;
    config.hardware.max_codes_per_second = 1000000;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    mdk_predator_init(&config);

    bool result = mdk_set_active_module(MODULE_WIFI);
    TEST_ASSERT(result == true, "Set active module should succeed");

    result = mdk_set_active_module(MODULE_BLUETOOTH);
    TEST_ASSERT(result == true, "Should be able to switch modules");

    result = mdk_set_active_module(MODULE_CRYPTO);
    TEST_ASSERT(result == true, "Should be able to switch to crypto module");

    mdk_predator_cleanup();
}

/**
 * Test: Set active module before initialization
 */
void test_mdk_set_active_module_before_init() {
    mdk_predator_cleanup();

    bool result = mdk_set_active_module(MODULE_WIFI);

    TEST_ASSERT(result == false, "Set active module should fail before init");
}

/**
 * Test: Run diagnostic after initialization
 */
void test_mdk_run_diagnostic_valid() {
    mdk_predator_config_t config;
    diagnostic_result_t result;

    config.default_module = MODULE_AUTOMOTIVE;
    config.enable_logging = false;
    config.buffer_size = 1024;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.parallel_streams = 4;
    config.hardware.use_dma = true;
    config.hardware.max_codes_per_second = 1000000;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    mdk_predator_init(&config);

    bool test_result = mdk_run_diagnostic(&result);

    TEST_ASSERT(test_result == true, "Diagnostic should succeed after init");
    TEST_ASSERT(result.hardware_ok == true, "Hardware should be OK");
    TEST_ASSERT(result.automotive_ok == true, "Automotive module should be OK");
    TEST_ASSERT(result.wifi_ok == true, "WiFi module should be OK");
    TEST_ASSERT(result.bluetooth_ok == true, "Bluetooth module should be OK");
    TEST_ASSERT(result.subghz_ok == true, "SubGHz module should be OK");
    TEST_ASSERT(result.crypto_ok == true, "Crypto module should be OK");

    mdk_predator_cleanup();
}

/**
 * Test: Run diagnostic before initialization
 */
void test_mdk_run_diagnostic_before_init() {
    diagnostic_result_t result;

    mdk_predator_cleanup();

    bool test_result = mdk_run_diagnostic(&result);

    TEST_ASSERT(test_result == false, "Diagnostic should fail before init");
}

/**
 * Test: Run diagnostic with null result buffer
 */
void test_mdk_run_diagnostic_null() {
    mdk_predator_config_t config;
    config.default_module = MODULE_AUTOMOTIVE;
    config.enable_logging = false;
    config.buffer_size = 1024;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.parallel_streams = 4;
    config.hardware.use_dma = true;
    config.hardware.max_codes_per_second = 1000000;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    mdk_predator_init(&config);

    bool result = mdk_run_diagnostic(NULL);

    TEST_ASSERT(result == false, "Diagnostic should fail with NULL result buffer");

    mdk_predator_cleanup();
}

/**
 * Test: Get version string
 */
void test_mdk_get_version() {
    const char *version = mdk_get_version();

    TEST_ASSERT(version != NULL, "Version string should not be NULL");
    TEST_ASSERT(strlen(version) > 0, "Version string should not be empty");
}

/**
 * Test: Cleanup is safe to call multiple times
 */
void test_mdk_cleanup_multiple() {
    mdk_predator_config_t config;
    config.default_module = MODULE_AUTOMOTIVE;
    config.enable_logging = false;
    config.buffer_size = 1024;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.parallel_streams = 4;
    config.hardware.use_dma = true;
    config.hardware.max_codes_per_second = 1000000;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    mdk_predator_init(&config);

    // Call cleanup multiple times - should not crash
    mdk_predator_cleanup();
    mdk_predator_cleanup();
    mdk_predator_cleanup();

    TEST_ASSERT(true, "Multiple cleanups should be safe");
}

/**
 * Test: Hardware init function
 */
void test_mdk_hardware_init() {
    bool result = mdk_hardware_init();

    TEST_ASSERT(result == true, "Hardware init should succeed");
}

/**
 * Test: Hardware acceleration configuration
 */
void test_hardware_acceleration_config() {
    mdk_predator_config_t config;
    config.default_module = MODULE_AUTOMOTIVE;
    config.enable_logging = false;
    config.buffer_size = 1024;
    config.hardware.hackrf_sample_rate = 2000000;
    config.hardware.hackrf_bandwidth = 1750000;
    config.hardware.hackrf_lna_gain = 8;
    config.hardware.hackrf_vga_gain = 20;
    config.hardware.hackrf_txvga_gain = 0;
    config.hardware.parallel_streams = 4;
    config.hardware.use_dma = true;
    config.hardware.max_codes_per_second = 1000000;
    config.security.allow_transmit = false;
    config.security.require_confirmation = true;
    config.security.log_all_activity = true;

    bool result = mdk_predator_init(&config);

    TEST_ASSERT(result == true, "Init should succeed with hardware acceleration config");
    TEST_ASSERT(config.hardware.parallel_streams == 4, "Parallel streams should be 4");
    TEST_ASSERT(config.hardware.use_dma == true, "DMA should be enabled");
    TEST_ASSERT(config.hardware.max_codes_per_second == 1000000, "Max codes per second should be 1000000");

    mdk_predator_cleanup();
}

/**
 * Main test runner
 */
int main(void) {
    printf("========================================\n");
    printf("MDK-Predator Integration Tests\n");
    printf("========================================\n");

    RUN_TEST(test_mdk_init_valid);
    RUN_TEST(test_mdk_init_null);
    RUN_TEST(test_mdk_get_status_after_init);
    RUN_TEST(test_mdk_get_status_before_init);
    RUN_TEST(test_mdk_get_status_null);
    RUN_TEST(test_mdk_set_active_module_valid);
    RUN_TEST(test_mdk_set_active_module_before_init);
    RUN_TEST(test_mdk_run_diagnostic_valid);
    RUN_TEST(test_mdk_run_diagnostic_before_init);
    RUN_TEST(test_mdk_run_diagnostic_null);
    RUN_TEST(test_mdk_get_version);
    RUN_TEST(test_mdk_cleanup_multiple);
    RUN_TEST(test_mdk_hardware_init);
    RUN_TEST(test_hardware_acceleration_config);

    printf("\n========================================\n");
    printf("Test Results:\n");
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
