/**
 * Unit tests for Rolling Code Tester Module
 *
 * These tests validate the rolling code tester functionality without
 * requiring actual hardware.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "automotive/rolling_code_tester.h"
#include "automotive/key_fob_analyzer.h"

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
 * Test: Initialize with valid config
 */
void test_rolling_code_init_valid() {
    rolling_code_config_t config = {
        .algorithm = ROLLING_CODE_KEELOQ,
        .mode = TEST_MODE_PASSIVE
    };
    rolling_code_tester_init(&config);

    TEST_ASSERT(config.algorithm == ROLLING_CODE_KEELOQ, "Default algorithm should be KeeLoq");
    TEST_ASSERT(config.mode == TEST_MODE_PASSIVE, "Default mode should be PASSIVE");
}

/**
 * Test: Initialize with null config
 */
void test_rolling_code_init_null() {
    rolling_code_tester_init(NULL);
    TEST_ASSERT(true, "Init should handle NULL config gracefully");
}

/**
 * Test: KeeLoq code validation (valid)
 */
void test_keeloq_code_valid() {
    uint8_t encrypted[8] = {0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F, 0x7A, 0x8B};
    uint8_t key[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    keeloq_result_t result;

    // Test with a valid code
    bool test_result = test_keeloq_code(encrypted, key, &result);

    TEST_ASSERT(test_result == true, "Keeloq test should succeed with valid params");
    TEST_ASSERT(result.is_valid == true, "Result should be marked as valid");
}

/**
 * Test: KeeLoq code validation (null encrypted data)
 */
void test_keeloq_code_null_encrypted() {
    uint8_t key[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    keeloq_result_t result;

    bool test_result = test_keeloq_code(NULL, key, &result);

    TEST_ASSERT(test_result == false, "Keeloq test should fail with NULL encrypted data");
}

/**
 * Test: KeeLoq code validation (zero key)
 */
void test_keeloq_code_zero_key() {
    uint8_t encrypted[8] = {0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F, 0x7A, 0x8B};
    uint8_t key[8] = {0};
    keeloq_result_t result;

    bool test_result = test_keeloq_code(encrypted, key, &result);

    TEST_ASSERT(test_result == true, "Keeloq test should succeed with zero key");
}

/**
 * Test: Analyze rolling code sequence (valid)
 */
void test_analyze_sequence_valid() {
    signal_data_t signals[3];
    sequence_analysis_t analysis;

    // Populate signals with sample data
    for (int i = 0; i < 3; i++) {
        signals[i].data = (uint8_t*)malloc(16);
        memset(signals[i].data, i, 16);
        signals[i].length = 16;
    }

    bool result = analyze_rolling_sequence(signals, 3, &analysis);
    TEST_ASSERT(result == true, "Sequence analysis should succeed with valid data");

    for (int i = 0; i < 3; i++) {
        free(signals[i].data);
    }
}

/**
 * Test: Analyze rolling code sequence (null signals)
 */
void test_analyze_sequence_null_signals() {
    sequence_analysis_t analysis;

    bool result = analyze_rolling_sequence(NULL, 3, &analysis);
    TEST_ASSERT(result == false, "Sequence analysis should fail with NULL signals");
}

/**
 * Test: Analyze rolling code sequence (null analysis result)
 */
void test_analyze_sequence_null_analysis() {
    signal_data_t signals[3];

    for (int i = 0; i < 3; i++) {
        signals[i].data = (uint8_t*)malloc(16);
        memset(signals[i].data, i, 16);
        signals[i].length = 16;
    }

    bool result = analyze_rolling_sequence(signals, 3, NULL);
    TEST_ASSERT(result == false, "Sequence analysis should fail with NULL analysis buffer");

    for (int i = 0; i < 3; i++) {
        free(signals[i].data);
    }
}

/**
 * Test: Analyze rolling code sequence (zero count)
 */
void test_analyze_sequence_zero_count() {
    signal_data_t signals[3] = {0};
    sequence_analysis_t analysis;

    bool result = analyze_rolling_sequence(signals, 0, &analysis);
    TEST_ASSERT(result == false, "Sequence analysis should fail with zero count");
}

/**
 * Test: Replay vulnerability (valid)
 */
void test_replay_vulnerability_valid() {
    rolling_code_config_t config;
    signal_data_t signal;
    replay_test_result_t result;

    config.algorithm = ROLLING_CODE_KEELOQ;
    signal.data = NULL;
    signal.length = 0;

    bool test_result = test_replay_vulnerability(&config, &signal, &result);

    TEST_ASSERT(test_result == true, "Replay vulnerability test should succeed with valid parameters");
}

/**
 * Test: Replay vulnerability (null config)
 */
void test_replay_vulnerability_null_config() {
    signal_data_t signal;
    replay_test_result_t result;

    bool test_result = test_replay_vulnerability(NULL, &signal, &result);

    TEST_ASSERT(test_result == false, "Replay vulnerability test should fail with NULL config");
}

/**
 * Test: Replay vulnerability (null signal)
 */
void test_replay_vulnerability_null_signal() {
    rolling_code_config_t config;
    replay_test_result_t result;

    config.algorithm = ROLLING_CODE_KEELOQ;

    bool test_result = test_replay_vulnerability(&config, NULL, &result);

    TEST_ASSERT(test_result == false, "Replay vulnerability test should fail with NULL signal");
}

/**
 * Test: Replay vulnerability (null result)
 */
void test_replay_vulnerability_null_result() {
    rolling_code_config_t config;
    signal_data_t signal;

    config.algorithm = ROLLING_CODE_KEELOQ;
    signal.data = NULL;
    signal.length = 0;

    bool test_result = test_replay_vulnerability(&config, &signal, NULL);

    TEST_ASSERT(test_result == false, "Replay vulnerability test should fail with NULL result");
}

/**
 * Test: Cleanup function (valid)
 */
void test_rolling_code_cleanup_valid() {
    rolling_code_config_t config;

    config.algorithm = ROLLING_CODE_KEELOQ;

    // Cleanup should not crash
    rolling_code_tester_cleanup(&config);

    TEST_ASSERT(true, "Cleanup should succeed with valid config");
}

/**
 * Test: Cleanup function (null)
 */
void test_rolling_code_cleanup_null() {
    // Cleanup should not crash
    rolling_code_tester_cleanup(NULL);

    TEST_ASSERT(true, "Cleanup should handle NULL config gracefully");
}

/**
 * Main test runner
 */
int main(void) {
    printf("========================================\n");
    printf("Rolling Code Tester Unit Tests\n");
    printf("========================================\n");

    RUN_TEST(test_rolling_code_init_valid);
    RUN_TEST(test_rolling_code_init_null);
    RUN_TEST(test_keeloq_code_valid);
    RUN_TEST(test_keeloq_code_null_encrypted);
    RUN_TEST(test_keeloq_code_zero_key);
    RUN_TEST(test_analyze_sequence_valid);
    RUN_TEST(test_analyze_sequence_null_signals);
    RUN_TEST(test_analyze_sequence_null_analysis);
    RUN_TEST(test_analyze_sequence_zero_count);
    RUN_TEST(test_replay_vulnerability_valid);
    RUN_TEST(test_replay_vulnerability_null_config);
    RUN_TEST(test_replay_vulnerability_null_signal);
    RUN_TEST(test_replay_vulnerability_null_result);
    RUN_TEST(test_rolling_code_cleanup_valid);
    RUN_TEST(test_rolling_code_cleanup_null);

    printf("\n========================================\n");
    printf("Test Results:\n");
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
