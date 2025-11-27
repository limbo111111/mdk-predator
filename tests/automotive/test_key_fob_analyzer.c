/**
 * Unit tests for Key Fob Analyzer Module
 *
 * These tests validate the key fob analyzer functionality without
 * requiring actual hardware.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "application/automotive/key_fob_analyzer.h"

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
 * Test: Initialize key fob analyzer with valid config
 */
void test_keyfob_init_valid() {
    keyfob_config_t config;
    bool result = keyfob_analyzer_init(&config);

    TEST_ASSERT(result == true, "Init should succeed with valid config");
    TEST_ASSERT(config.frequency == 315000000, "Default frequency should be 315 MHz");
    TEST_ASSERT(config.bandwidth == 200000, "Default bandwidth should be 200 kHz");
    TEST_ASSERT(config.sample_rate == 2000000, "Default sample rate should be 2 MS/s");
    TEST_ASSERT(config.mode == KEYFOB_MODE_RECEIVE, "Default mode should be RECEIVE");
}

/**
 * Test: Initialize key fob analyzer with null config
 */
void test_keyfob_init_null() {
    bool result = keyfob_analyzer_init(NULL);

    TEST_ASSERT(result == false, "Init should fail with NULL config");
}

/**
 * Test: Capture signal with valid parameters
 */
void test_keyfob_capture_valid() {
    keyfob_config_t config;
    signal_data_t signal;

    keyfob_analyzer_init(&config);
    bool result = keyfob_capture_signal(&config, &signal);

    TEST_ASSERT(result == true, "Capture should succeed with valid parameters");
}

/**
 * Test: Capture signal with null config
 */
void test_keyfob_capture_null_config() {
    signal_data_t signal;
    bool result = keyfob_capture_signal(NULL, &signal);

    TEST_ASSERT(result == false, "Capture should fail with NULL config");
}

/**
 * Test: Capture signal with null signal buffer
 */
void test_keyfob_capture_null_signal() {
    keyfob_config_t config;
    keyfob_analyzer_init(&config);

    bool result = keyfob_capture_signal(&config, NULL);

    TEST_ASSERT(result == false, "Capture should fail with NULL signal buffer");
}

/**
 * Test: Analyze signal with valid parameters
 */
void test_keyfob_analyze_valid() {
    signal_data_t signal;
    keyfob_analysis_t result_data;

    // Setup mock signal data
    signal.data = NULL;
    signal.length = 0;
    signal.frequency = 315000000;
    signal.timestamp = 0;

    bool result = keyfob_analyze_signal(&signal, &result_data);

    TEST_ASSERT(result == true, "Analyze should succeed with valid parameters");
}

/**
 * Test: Analyze signal with null signal
 */
void test_keyfob_analyze_null_signal() {
    keyfob_analysis_t result_data;
    bool result = keyfob_analyze_signal(NULL, &result_data);

    TEST_ASSERT(result == false, "Analyze should fail with NULL signal");
}

/**
 * Test: Analyze signal with null result buffer
 */
void test_keyfob_analyze_null_result() {
    signal_data_t signal;
    signal.data = NULL;
    signal.length = 0;

    bool result = keyfob_analyze_signal(&signal, NULL);

    TEST_ASSERT(result == false, "Analyze should fail with NULL result buffer");
}

/**
 * Test: Detect rolling code with valid parameters
 */
void test_keyfob_rolling_code_valid() {
    signal_data_t signal;
    rolling_code_info_t info;

    signal.data = NULL;
    signal.length = 0;

    bool result = keyfob_detect_rolling_code(&signal, &info);

    TEST_ASSERT(result == true, "Rolling code detection should succeed with valid parameters");
}

/**
 * Test: Detect rolling code with null signal
 */
void test_keyfob_rolling_code_null_signal() {
    rolling_code_info_t info;
    bool result = keyfob_detect_rolling_code(NULL, &info);

    TEST_ASSERT(result == false, "Rolling code detection should fail with NULL signal");
}

/**
 * Test: Detect rolling code with null info buffer
 */
void test_keyfob_rolling_code_null_info() {
    signal_data_t signal;
    signal.data = NULL;
    signal.length = 0;

    bool result = keyfob_detect_rolling_code(&signal, NULL);

    TEST_ASSERT(result == false, "Rolling code detection should fail with NULL info buffer");
}

/**
 * Test: Cleanup with valid config
 */
void test_keyfob_cleanup_valid() {
    keyfob_config_t config;
    keyfob_analyzer_init(&config);

    // Should not crash
    keyfob_analyzer_cleanup(&config);

    TEST_ASSERT(true, "Cleanup should succeed with valid config");
}

/**
 * Test: Cleanup with null config
 */
void test_keyfob_cleanup_null() {
    // Should not crash
    keyfob_analyzer_cleanup(NULL);

    TEST_ASSERT(true, "Cleanup should handle NULL config gracefully");
}

/**
 * Main test runner
 */
int main_automotive_key_fob_analyzer(void) {
    printf("========================================\n");
    printf("Key Fob Analyzer Unit Tests\n");
    printf("========================================\n");

    RUN_TEST(test_keyfob_init_valid);
    RUN_TEST(test_keyfob_init_null);
    RUN_TEST(test_keyfob_capture_valid);
    RUN_TEST(test_keyfob_capture_null_config);
    RUN_TEST(test_keyfob_capture_null_signal);
    RUN_TEST(test_keyfob_analyze_valid);
    RUN_TEST(test_keyfob_analyze_null_signal);
    RUN_TEST(test_keyfob_analyze_null_result);
    RUN_TEST(test_keyfob_rolling_code_valid);
    RUN_TEST(test_keyfob_rolling_code_null_signal);
    RUN_TEST(test_keyfob_rolling_code_null_info);
    RUN_TEST(test_keyfob_cleanup_valid);
    RUN_TEST(test_keyfob_cleanup_null);

    printf("\n========================================\n");
    printf("Test Results:\n");
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
