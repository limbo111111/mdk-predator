/**
 * Unit tests for Bluetooth Security Analysis Module
 * 
 * These tests validate the Bluetooth analyzer functionality without
 * requiring actual hardware.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bluetooth_analyzer.h"

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
 * Test: Initialize Bluetooth analyzer with valid config
 */
void test_bluetooth_init_valid() {
    bluetooth_config_t config;
    bool result = bluetooth_analyzer_init(&config);
    
    TEST_ASSERT(result == true, "Init should succeed with valid config");
    TEST_ASSERT(config.mode == BT_MODE_SCAN, "Default mode should be SCAN");
    TEST_ASSERT(config.scan_classic == true, "Classic scan should be enabled");
    TEST_ASSERT(config.scan_ble == true, "BLE scan should be enabled");
    TEST_ASSERT(config.scan_duration == 10, "Default scan duration should be 10 seconds");
}

/**
 * Test: Initialize Bluetooth analyzer with null config
 */
void test_bluetooth_init_null() {
    bool result = bluetooth_analyzer_init(NULL);
    
    TEST_ASSERT(result == false, "Init should fail with NULL config");
}

/**
 * Test: Scan devices with valid parameters
 */
void test_bluetooth_scan_devices_valid() {
    bluetooth_config_t config;
    bt_device_t devices[MAX_BT_DEVICES];
    uint32_t count;
    
    bluetooth_analyzer_init(&config);
    bool result = bluetooth_scan_devices(&config, devices, &count);
    
    TEST_ASSERT(result == true, "Device scan should succeed with valid parameters");
    TEST_ASSERT(count == 0, "Count should be initialized to 0");
}

/**
 * Test: Scan devices with null config
 */
void test_bluetooth_scan_devices_null_config() {
    bt_device_t devices[MAX_BT_DEVICES];
    uint32_t count;
    bool result = bluetooth_scan_devices(NULL, devices, &count);
    
    TEST_ASSERT(result == false, "Device scan should fail with NULL config");
}

/**
 * Test: Scan devices with null devices buffer
 */
void test_bluetooth_scan_devices_null_buffer() {
    bluetooth_config_t config;
    uint32_t count;
    
    bluetooth_analyzer_init(&config);
    bool result = bluetooth_scan_devices(&config, NULL, &count);
    
    TEST_ASSERT(result == false, "Device scan should fail with NULL devices buffer");
}

/**
 * Test: Scan devices with null count
 */
void test_bluetooth_scan_devices_null_count() {
    bluetooth_config_t config;
    bt_device_t devices[MAX_BT_DEVICES];
    
    bluetooth_analyzer_init(&config);
    bool result = bluetooth_scan_devices(&config, devices, NULL);
    
    TEST_ASSERT(result == false, "Device scan should fail with NULL count");
}

/**
 * Test: Analyze security with valid parameters
 */
void test_bluetooth_analyze_security_valid() {
    bt_device_t device;
    bt_security_analysis_t analysis;
    
    memset(&device, 0, sizeof(device));
    
    bool result = bluetooth_analyze_security(&device, &analysis);
    
    TEST_ASSERT(result == true, "Security analysis should succeed with valid parameters");
    TEST_ASSERT(analysis.is_discoverable == true, "Device should be discoverable by default");
}

/**
 * Test: Analyze security with null device
 */
void test_bluetooth_analyze_security_null_device() {
    bt_security_analysis_t analysis;
    bool result = bluetooth_analyze_security(NULL, &analysis);
    
    TEST_ASSERT(result == false, "Security analysis should fail with NULL device");
}

/**
 * Test: Analyze security with null analysis buffer
 */
void test_bluetooth_analyze_security_null_buffer() {
    bt_device_t device;
    memset(&device, 0, sizeof(device));
    
    bool result = bluetooth_analyze_security(&device, NULL);
    
    TEST_ASSERT(result == false, "Security analysis should fail with NULL analysis buffer");
}

/**
 * Test: Enumerate services with valid parameters
 */
void test_bluetooth_enumerate_services_valid() {
    bt_device_t device;
    bt_service_t services[MAX_BT_SERVICES];
    uint32_t count;
    
    memset(&device, 0, sizeof(device));
    
    bool result = bluetooth_enumerate_services(&device, services, &count);
    
    TEST_ASSERT(result == true, "Service enumeration should succeed with valid parameters");
    TEST_ASSERT(count == 0, "Count should be initialized to 0");
}

/**
 * Test: Enumerate services with null device
 */
void test_bluetooth_enumerate_services_null_device() {
    bt_service_t services[MAX_BT_SERVICES];
    uint32_t count;
    
    bool result = bluetooth_enumerate_services(NULL, services, &count);
    
    TEST_ASSERT(result == false, "Service enumeration should fail with NULL device");
}

/**
 * Test: Enumerate services with null services buffer
 */
void test_bluetooth_enumerate_services_null_buffer() {
    bt_device_t device;
    uint32_t count;
    
    memset(&device, 0, sizeof(device));
    bool result = bluetooth_enumerate_services(&device, NULL, &count);
    
    TEST_ASSERT(result == false, "Service enumeration should fail with NULL services buffer");
}

/**
 * Test: Enumerate services with null count
 */
void test_bluetooth_enumerate_services_null_count() {
    bt_device_t device;
    bt_service_t services[MAX_BT_SERVICES];
    
    memset(&device, 0, sizeof(device));
    bool result = bluetooth_enumerate_services(&device, services, NULL);
    
    TEST_ASSERT(result == false, "Service enumeration should fail with NULL count");
}

/**
 * Test: Capture BLE advertisement with valid parameters
 */
void test_bluetooth_capture_ble_adv_valid() {
    bluetooth_config_t config;
    ble_adv_data_t adv_data;
    
    bluetooth_analyzer_init(&config);
    bool result = bluetooth_capture_ble_adv(&config, &adv_data);
    
    TEST_ASSERT(result == true, "BLE advertisement capture should succeed with valid parameters");
    TEST_ASSERT(adv_data.packet_count == 0, "Packet count should be 0 initially");
}

/**
 * Test: Capture BLE advertisement with null config
 */
void test_bluetooth_capture_ble_adv_null_config() {
    ble_adv_data_t adv_data;
    bool result = bluetooth_capture_ble_adv(NULL, &adv_data);
    
    TEST_ASSERT(result == false, "BLE advertisement capture should fail with NULL config");
}

/**
 * Test: Capture BLE advertisement with null adv_data buffer
 */
void test_bluetooth_capture_ble_adv_null_buffer() {
    bluetooth_config_t config;
    
    bluetooth_analyzer_init(&config);
    bool result = bluetooth_capture_ble_adv(&config, NULL);
    
    TEST_ASSERT(result == false, "BLE advertisement capture should fail with NULL adv_data buffer");
}

/**
 * Test: Test MITM vulnerability with valid parameters
 */
void test_bluetooth_test_mitm_valid() {
    bt_device_t device;
    mitm_test_result_t result_data;
    
    memset(&device, 0, sizeof(device));
    
    bool result = bluetooth_test_mitm(&device, &result_data);
    
    TEST_ASSERT(result == true, "MITM test should succeed with valid parameters");
    TEST_ASSERT(result_data.is_vulnerable == false, "Device should not be vulnerable by default");
    TEST_ASSERT(result_data.attack_feasible == false, "Attack should not be feasible by default");
}

/**
 * Test: Test MITM vulnerability with null device
 */
void test_bluetooth_test_mitm_null_device() {
    mitm_test_result_t result_data;
    bool result = bluetooth_test_mitm(NULL, &result_data);
    
    TEST_ASSERT(result == false, "MITM test should fail with NULL device");
}

/**
 * Test: Test MITM vulnerability with null result buffer
 */
void test_bluetooth_test_mitm_null_buffer() {
    bt_device_t device;
    memset(&device, 0, sizeof(device));
    
    bool result = bluetooth_test_mitm(&device, NULL);
    
    TEST_ASSERT(result == false, "MITM test should fail with NULL result buffer");
}

/**
 * Test: Cleanup with valid config
 */
void test_bluetooth_cleanup_valid() {
    bluetooth_config_t config;
    bluetooth_analyzer_init(&config);
    
    // Should not crash
    bluetooth_analyzer_cleanup(&config);
    
    TEST_ASSERT(true, "Cleanup should succeed with valid config");
}

/**
 * Test: Cleanup with null config
 */
void test_bluetooth_cleanup_null() {
    // Should not crash
    bluetooth_analyzer_cleanup(NULL);
    
    TEST_ASSERT(true, "Cleanup should handle NULL config gracefully");
}

/**
 * Main test runner
 */
int main(void) {
    printf("========================================\n");
    printf("Bluetooth Security Analyzer Unit Tests\n");
    printf("========================================\n");
    
    RUN_TEST(test_bluetooth_init_valid);
    RUN_TEST(test_bluetooth_init_null);
    RUN_TEST(test_bluetooth_scan_devices_valid);
    RUN_TEST(test_bluetooth_scan_devices_null_config);
    RUN_TEST(test_bluetooth_scan_devices_null_buffer);
    RUN_TEST(test_bluetooth_scan_devices_null_count);
    RUN_TEST(test_bluetooth_analyze_security_valid);
    RUN_TEST(test_bluetooth_analyze_security_null_device);
    RUN_TEST(test_bluetooth_analyze_security_null_buffer);
    RUN_TEST(test_bluetooth_enumerate_services_valid);
    RUN_TEST(test_bluetooth_enumerate_services_null_device);
    RUN_TEST(test_bluetooth_enumerate_services_null_buffer);
    RUN_TEST(test_bluetooth_enumerate_services_null_count);
    RUN_TEST(test_bluetooth_capture_ble_adv_valid);
    RUN_TEST(test_bluetooth_capture_ble_adv_null_config);
    RUN_TEST(test_bluetooth_capture_ble_adv_null_buffer);
    RUN_TEST(test_bluetooth_test_mitm_valid);
    RUN_TEST(test_bluetooth_test_mitm_null_device);
    RUN_TEST(test_bluetooth_test_mitm_null_buffer);
    RUN_TEST(test_bluetooth_cleanup_valid);
    RUN_TEST(test_bluetooth_cleanup_null);
    
    printf("\n========================================\n");
    printf("Test Results:\n");
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("========================================\n");
    
    return tests_failed > 0 ? 1 : 0;
}
