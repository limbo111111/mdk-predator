/**
 * Unit tests for WiFi Security Analysis Module
 *
 * These tests validate the WiFi analyzer functionality without
 * requiring actual hardware.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wireless/wifi_analyzer.h"

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
 * Test: Initialize WiFi analyzer with valid config
 */
void test_wifi_init_valid() {
    wifi_config_t config = {
        .channel = 6,
        .mode = WIFI_MODE_MONITOR,
        .capture_beacons = true,
        .capture_data = true
    };
    bool result = wifi_analyzer_init(&config);

    TEST_ASSERT(result == true, "Init should succeed with valid config");
    TEST_ASSERT(config.channel == 6, "Default channel should be 6");
    TEST_ASSERT(config.mode == WIFI_MODE_MONITOR, "Default mode should be MONITOR");
    TEST_ASSERT(config.capture_beacons == true, "Beacon capture should be enabled");
    TEST_ASSERT(config.capture_data == true, "Data capture should be enabled");
}

/**
 * Test: Initialize WiFi analyzer with null config
 */
void test_wifi_init_null() {
    bool result = wifi_analyzer_init(NULL);

    TEST_ASSERT(result == false, "Init should fail with NULL config");
}

/**
 * Test: Scan networks with valid parameters
 */
void test_wifi_scan_networks_valid() {
    wifi_config_t config;
    wifi_network_t networks[MAX_NETWORKS];
    uint32_t count;

    wifi_analyzer_init(&config);
    bool result = wifi_scan_networks(&config, networks, &count, MAX_NETWORKS);

    TEST_ASSERT(result == true, "Network scan should succeed with valid parameters");
    TEST_ASSERT(count > 0, "Count should be greater than 0 after scan");
}

/**
 * Test: Scan networks with null config
 */
void test_wifi_scan_networks_null_config() {
    wifi_network_t networks[MAX_NETWORKS];
    uint32_t count;
    bool result = wifi_scan_networks(NULL, networks, &count, MAX_NETWORKS);

    TEST_ASSERT(result == false, "Network scan should fail with NULL config");
}

/**
 * Test: Scan networks with null networks buffer
 */
void test_wifi_scan_networks_null_buffer() {
    wifi_config_t config;
    uint32_t count;

    wifi_analyzer_init(&config);
    bool result = wifi_scan_networks(&config, NULL, &count, MAX_NETWORKS);

    TEST_ASSERT(result == false, "Network scan should fail with NULL networks buffer");
}

/**
 * Test: Scan networks with null count
 */
void test_wifi_scan_networks_null_count() {
    wifi_config_t config;
    wifi_network_t networks[MAX_NETWORKS];

    wifi_analyzer_init(&config);
    bool result = wifi_scan_networks(&config, networks, NULL, MAX_NETWORKS);

    TEST_ASSERT(result == false, "Network scan should fail with NULL count");
}

/**
 * Test: Analyze security with valid parameters
 */
void test_wifi_analyze_security_valid() {
    wifi_network_t network;
    wifi_security_analysis_t analysis;

    network.security_type = WIFI_SECURITY_WPA2;
    network.rssi = -50;

    bool result = wifi_analyze_security(&network, &analysis);

    TEST_ASSERT(result == true, "Security analysis should succeed with valid parameters");
    TEST_ASSERT(analysis.encryption_type == WIFI_SECURITY_WPA2, "Encryption type should match network");
    TEST_ASSERT(analysis.signal_strength == -50, "Signal strength should match network RSSI");
}

/**
 * Test: Analyze security with null network
 */
void test_wifi_analyze_security_null_network() {
    wifi_security_analysis_t analysis;
    bool result = wifi_analyze_security(NULL, &analysis);

    TEST_ASSERT(result == false, "Security analysis should fail with NULL network");
}

/**
 * Test: Analyze security with null analysis buffer
 */
void test_wifi_analyze_security_null_buffer() {
    wifi_network_t network;
    network.security_type = WIFI_SECURITY_WPA2;

    bool result = wifi_analyze_security(&network, NULL);

    TEST_ASSERT(result == false, "Security analysis should fail with NULL analysis buffer");
}

/**
 * Test: Capture handshake with valid parameters
 */
void test_wifi_capture_handshake_valid() {
    wifi_config_t config;
    uint8_t bssid[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    handshake_data_t handshake;

    wifi_analyzer_init(&config);
    bool result = wifi_capture_handshake(&config, bssid, &handshake);

    TEST_ASSERT(result == true, "Handshake capture should succeed with valid parameters");
    TEST_ASSERT(handshake.is_complete == false, "Handshake should be incomplete initially");
    TEST_ASSERT(handshake.frame_count == 0, "Frame count should be 0 initially");
}

/**
 * Test: Capture handshake with null config
 */
void test_wifi_capture_handshake_null_config() {
    uint8_t bssid[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    handshake_data_t handshake;

    bool result = wifi_capture_handshake(NULL, bssid, &handshake);

    TEST_ASSERT(result == false, "Handshake capture should fail with NULL config");
}

/**
 * Test: Capture handshake with null BSSID
 */
void test_wifi_capture_handshake_null_bssid() {
    wifi_config_t config;
    handshake_data_t handshake;

    wifi_analyzer_init(&config);
    bool result = wifi_capture_handshake(&config, NULL, &handshake);

    TEST_ASSERT(result == false, "Handshake capture should fail with NULL BSSID");
}

/**
 * Test: Capture handshake with null handshake buffer
 */
void test_wifi_capture_handshake_null_buffer() {
    wifi_config_t config;
    uint8_t bssid[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

    wifi_analyzer_init(&config);
    bool result = wifi_capture_handshake(&config, bssid, NULL);

    TEST_ASSERT(result == false, "Handshake capture should fail with NULL handshake buffer");
}

/**
 * Test: Detect deauth with valid parameters
 */
void test_wifi_detect_deauth_valid() {
    wifi_config_t config;
    deauth_detection_t detection;

    wifi_analyzer_init(&config);
    bool result = wifi_detect_deauth(&config, &detection);

    TEST_ASSERT(result == true, "Deauth detection should succeed with valid parameters");
    TEST_ASSERT(detection.deauth_count == 0, "Deauth count should be 0 initially");
    TEST_ASSERT(detection.attack_detected == false, "Attack should not be detected initially");
}

/**
 * Test: Detect deauth with null config
 */
void test_wifi_detect_deauth_null_config() {
    deauth_detection_t detection;
    bool result = wifi_detect_deauth(NULL, &detection);

    TEST_ASSERT(result == false, "Deauth detection should fail with NULL config");
}

/**
 * Test: Detect deauth with null detection buffer
 */
void test_wifi_detect_deauth_null_buffer() {
    wifi_config_t config;

    wifi_analyzer_init(&config);
    bool result = wifi_detect_deauth(&config, NULL);

    TEST_ASSERT(result == false, "Deauth detection should fail with NULL detection buffer");
}

/**
 * Test: Cleanup with valid config
 */
void test_wifi_cleanup_valid() {
    wifi_config_t config;
    wifi_analyzer_init(&config);

    // Should not crash
    wifi_analyzer_cleanup(&config);

    TEST_ASSERT(true, "Cleanup should succeed with valid config");
}

/**
 * Test: Cleanup with null config
 */
void test_wifi_cleanup_null() {
    // Should not crash
    wifi_analyzer_cleanup(NULL);

    TEST_ASSERT(true, "Cleanup should handle NULL config gracefully");
}

/**
 * Main test runner
 */
int main(void) {
    printf("========================================\n");
    printf("WiFi Security Analyzer Unit Tests\n");
    printf("========================================\n");

    RUN_TEST(test_wifi_init_valid);
    RUN_TEST(test_wifi_init_null);
    RUN_TEST(test_wifi_scan_networks_valid);
    RUN_TEST(test_wifi_scan_networks_null_config);
    RUN_TEST(test_wifi_scan_networks_null_buffer);
    RUN_TEST(test_wifi_scan_networks_null_count);
    RUN_TEST(test_wifi_analyze_security_valid);
    RUN_TEST(test_wifi_analyze_security_null_network);
    RUN_TEST(test_wifi_analyze_security_null_buffer);
    RUN_TEST(test_wifi_capture_handshake_valid);
    RUN_TEST(test_wifi_capture_handshake_null_config);
    RUN_TEST(test_wifi_capture_handshake_null_bssid);
    RUN_TEST(test_wifi_capture_handshake_null_buffer);
    RUN_TEST(test_wifi_detect_deauth_valid);
    RUN_TEST(test_wifi_detect_deauth_null_config);
    RUN_TEST(test_wifi_detect_deauth_null_buffer);
    RUN_TEST(test_wifi_cleanup_valid);
    RUN_TEST(test_wifi_cleanup_null);

    printf("\n========================================\n");
    printf("Test Results:\n");
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
