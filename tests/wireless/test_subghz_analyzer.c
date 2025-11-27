/**
 * Unit tests for SubGHz RF Analyzer Module
 *
 * These tests validate the SubGHz analyzer functionality without
 * requiring actual hardware.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wireless/subghz_analyzer.h"

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
 * Test: Initialize SubGHz analyzer with valid config
 */
void test_subghz_init_valid() {
    subghz_config_t config = {
        .frequency = 433920000,
        .bandwidth = 200000,
        .sample_rate = 2000000,
        .modulation = SUBGHZ_MOD_ASK_OOK
    };
    bool result = subghz_analyzer_init(&config);

    TEST_ASSERT(result == true, "Init should succeed with valid config");
    TEST_ASSERT(config.frequency == 433920000, "Default frequency should be 433.92 MHz");
    TEST_ASSERT(config.bandwidth == 200000, "Default bandwidth should be 200 kHz");
    TEST_ASSERT(config.sample_rate == 2000000, "Default sample rate should be 2 MS/s");
    TEST_ASSERT(config.modulation == SUBGHZ_MOD_ASK_OOK, "Default modulation should be ASK/OOK");
}

/**
 * Test: Initialize SubGHz analyzer with null config
 */
void test_subghz_init_null() {
    bool result = subghz_analyzer_init(NULL);

    TEST_ASSERT(result == false, "Init should fail with NULL config");
}

/**
 * Test: Scan spectrum with valid parameters
 */
void test_subghz_scan_spectrum_valid() {
    subghz_config_t config;
    spectrum_data_t spectrum;

    subghz_analyzer_init(&config);
    bool result = subghz_scan_spectrum(&config, &spectrum);

    TEST_ASSERT(result == true, "Spectrum scan should succeed with valid parameters");
}

/**
 * Test: Scan spectrum with null config
 */
void test_subghz_scan_spectrum_null_config() {
    spectrum_data_t spectrum;
    bool result = subghz_scan_spectrum(NULL, &spectrum);

    TEST_ASSERT(result == false, "Spectrum scan should fail with NULL config");
}

/**
 * Test: Scan spectrum with null spectrum buffer
 */
void test_subghz_scan_spectrum_null_buffer() {
    subghz_config_t config;
    subghz_analyzer_init(&config);

    bool result = subghz_scan_spectrum(&config, NULL);

    TEST_ASSERT(result == false, "Spectrum scan should fail with NULL spectrum buffer");
}

/**
 * Test: Capture signal with valid parameters
 */
void test_subghz_capture_signal_valid() {
    subghz_config_t config;
    rf_signal_t signal;
    int16_t i_samples[10] = {0};
    int16_t q_samples[10] = {0};

    signal.i_samples = i_samples;
    signal.q_samples = q_samples;
    signal.sample_count = 10;

    subghz_analyzer_init(&config);
    bool result = subghz_capture_signal(&config, &signal);

    TEST_ASSERT(result == true, "Signal capture should succeed with valid parameters");
}

/**
 * Test: Capture signal with null config
 */
void test_subghz_capture_signal_null_config() {
    rf_signal_t signal;
    bool result = subghz_capture_signal(NULL, &signal);

    TEST_ASSERT(result == false, "Signal capture should fail with NULL config");
}

/**
 * Test: Capture signal with null signal buffer
 */
void test_subghz_capture_signal_null_buffer() {
    subghz_config_t config;
    subghz_analyzer_init(&config);

    bool result = subghz_capture_signal(&config, NULL);

    TEST_ASSERT(result == false, "Signal capture should fail with NULL signal buffer");
}

/**
 * Test: Analyze signal with valid parameters
 */
void test_subghz_analyze_signal_valid() {
    rf_signal_t signal;
    signal_analysis_t analysis;
    int16_t i_samples[10] = {0};
    int16_t q_samples[10] = {0};

    signal.i_samples = i_samples;
    signal.q_samples = q_samples;
    signal.sample_count = 10;

    bool result = subghz_analyze_signal(&signal, &analysis);

    TEST_ASSERT(result == true, "Signal analysis should succeed with valid parameters");
}

/**
 * Test: Analyze signal with null signal
 */
void test_subghz_analyze_signal_null_signal() {
    signal_analysis_t analysis;
    bool result = subghz_analyze_signal(NULL, &analysis);

    TEST_ASSERT(result == false, "Signal analysis should fail with NULL signal");
}

/**
 * Test: Analyze signal with null analysis buffer
 */
void test_subghz_analyze_signal_null_buffer() {
    rf_signal_t signal;
    signal.i_samples = NULL;
    signal.q_samples = NULL;

    bool result = subghz_analyze_signal(&signal, NULL);

    TEST_ASSERT(result == false, "Signal analysis should fail with NULL analysis buffer");
}

/**
 * Test: Decode protocol with valid parameters
 */
void test_subghz_decode_protocol_valid() {
    rf_signal_t signal;
    protocol_data_t protocol;
    int16_t i_samples[10] = {0};
    int16_t q_samples[10] = {0};

    signal.i_samples = i_samples;
    signal.q_samples = q_samples;
    signal.sample_count = 10;

    bool result = subghz_decode_protocol(&signal, &protocol);

    TEST_ASSERT(result == true, "Protocol decode should succeed with valid parameters");
}

/**
 * Test: Decode protocol with null signal
 */
void test_subghz_decode_protocol_null_signal() {
    protocol_data_t protocol;
    bool result = subghz_decode_protocol(NULL, &protocol);

    TEST_ASSERT(result == false, "Protocol decode should fail with NULL signal");
}

/**
 * Test: Decode protocol with null protocol buffer
 */
void test_subghz_decode_protocol_null_buffer() {
    rf_signal_t signal;
    signal.i_samples = NULL;
    signal.q_samples = NULL;

    bool result = subghz_decode_protocol(&signal, NULL);

    TEST_ASSERT(result == false, "Protocol decode should fail with NULL protocol buffer");
}

/**
 * Test: Cleanup with valid config
 */
void test_subghz_cleanup_valid() {
    subghz_config_t config;
    subghz_analyzer_init(&config);

    // Should not crash
    subghz_analyzer_cleanup(&config);

    TEST_ASSERT(true, "Cleanup should succeed with valid config");
}

/**
 * Test: Cleanup with null config
 */
void test_subghz_cleanup_null() {
    // Should not crash
    subghz_analyzer_cleanup(NULL);

    TEST_ASSERT(true, "Cleanup should handle NULL config gracefully");
}

/**
 * Main test runner
 */
int main(void) {
    printf("========================================\n");
    printf("SubGHz RF Analyzer Unit Tests\n");
    printf("========================================\n");

    RUN_TEST(test_subghz_init_valid);
    RUN_TEST(test_subghz_init_null);
    RUN_TEST(test_subghz_scan_spectrum_valid);
    RUN_TEST(test_subghz_scan_spectrum_null_config);
    RUN_TEST(test_subghz_scan_spectrum_null_buffer);
    RUN_TEST(test_subghz_capture_signal_valid);
    RUN_TEST(test_subghz_capture_signal_null_config);
    RUN_TEST(test_subghz_capture_signal_null_buffer);
    RUN_TEST(test_subghz_analyze_signal_valid);
    RUN_TEST(test_subghz_analyze_signal_null_signal);
    RUN_TEST(test_subghz_analyze_signal_null_buffer);
    RUN_TEST(test_subghz_decode_protocol_valid);
    RUN_TEST(test_subghz_decode_protocol_null_signal);
    RUN_TEST(test_subghz_decode_protocol_null_buffer);
    RUN_TEST(test_subghz_cleanup_valid);
    RUN_TEST(test_subghz_cleanup_null);

    printf("\n========================================\n");
    printf("Test Results:\n");
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
