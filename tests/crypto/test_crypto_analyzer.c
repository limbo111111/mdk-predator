/**
 * Unit tests for Crypto Analyzer Module
 *
 * These tests validate the cryptographic analysis functionality without
 * requiring actual hardware.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "application/crypto/crypto_analyzer.h"

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
 * Test: Initialize crypto analyzer with valid config
 */
void test_crypto_init_valid() {
    crypto_config_t config;
    bool result = crypto_analyzer_init(&config);

    TEST_ASSERT(result == true, "Init should succeed with valid config");
    TEST_ASSERT(config.algorithm == CRYPTO_ALG_AES, "Default algorithm should be AES");
    TEST_ASSERT(config.key_length == 128, "Default key length should be 128");
    TEST_ASSERT(config.mode == CRYPTO_MODE_ANALYZE, "Default mode should be ANALYZE");
}

/**
 * Test: Initialize crypto analyzer with null config
 */
void test_crypto_init_null() {
    bool result = crypto_analyzer_init(NULL);

    TEST_ASSERT(result == false, "Init should fail with NULL config");
}

/**
 * Test: Identify algorithm with valid parameters
 */
void test_crypto_identify_algorithm_valid() {
    uint8_t ciphertext[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                               0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    crypto_identification_t result;

    bool test_result = crypto_identify_algorithm(ciphertext, 16, &result);

    TEST_ASSERT(test_result == true, "Algorithm identification should succeed with valid parameters");
}

/**
 * Test: Identify algorithm with null ciphertext
 */
void test_crypto_identify_algorithm_null_data() {
    crypto_identification_t result;

    bool test_result = crypto_identify_algorithm(NULL, 16, &result);

    TEST_ASSERT(test_result == false, "Algorithm identification should fail with NULL ciphertext");
}

/**
 * Test: Identify algorithm with null result buffer
 */
void test_crypto_identify_algorithm_null_result() {
    uint8_t ciphertext[16] = {0};

    bool result = crypto_identify_algorithm(ciphertext, 16, NULL);

    TEST_ASSERT(result == false, "Algorithm identification should fail with NULL result buffer");
}

/**
 * Test: Identify algorithm with zero length
 */
void test_crypto_identify_algorithm_zero_length() {
    uint8_t ciphertext[16] = {0};
    crypto_identification_t result;

    bool test_result = crypto_identify_algorithm(ciphertext, 0, &result);

    TEST_ASSERT(test_result == false, "Algorithm identification should fail with zero length");
}

/**
 * Test: Analyze key exchange with valid parameters
 */
void test_crypto_key_exchange_valid() {
    key_exchange_data_t data;
    key_exchange_analysis_t analysis;

    memset(&data, 0, sizeof(data));
    data.public_key_length = 128;
    data.exchange_data_length = 256;

    bool result = crypto_analyze_key_exchange(&data, &analysis);

    TEST_ASSERT(result == true, "Key exchange analysis should succeed with valid parameters");
}

/**
 * Test: Analyze key exchange with null data
 */
void test_crypto_key_exchange_null_data() {
    key_exchange_analysis_t analysis;

    bool result = crypto_analyze_key_exchange(NULL, &analysis);

    TEST_ASSERT(result == false, "Key exchange analysis should fail with NULL data");
}

/**
 * Test: Analyze key exchange with null analysis buffer
 */
void test_crypto_key_exchange_null_analysis() {
    key_exchange_data_t data;
    memset(&data, 0, sizeof(data));

    bool result = crypto_analyze_key_exchange(&data, NULL);

    TEST_ASSERT(result == false, "Key exchange analysis should fail with NULL analysis buffer");
}

/**
 * Test: Test weak keys with valid parameters
 */
void test_crypto_weak_keys_valid() {
    crypto_config_t config;
    uint8_t key[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    weak_key_result_t result;

    crypto_analyzer_init(&config);
    bool test_result = crypto_test_weak_keys(&config, key, 16, &result);

    TEST_ASSERT(test_result == true, "Weak key test should succeed with valid parameters");
}

/**
 * Test: Test weak keys with null config
 */
void test_crypto_weak_keys_null_config() {
    uint8_t key[16] = {0};
    weak_key_result_t result;

    bool test_result = crypto_test_weak_keys(NULL, key, 16, &result);

    TEST_ASSERT(test_result == false, "Weak key test should fail with NULL config");
}

/**
 * Test: Analyze rolling code with valid parameters
 */
void test_crypto_rolling_code_valid() {
    uint8_t encrypted_data[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                   0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    rolling_crypto_analysis_t analysis;

    bool result = crypto_analyze_rolling_code(encrypted_data, 16, &analysis);

    TEST_ASSERT(result == true, "Rolling code analysis should succeed with valid parameters");
}

/**
 * Test: Analyze rolling code with null data
 */
void test_crypto_rolling_code_null_data() {
    rolling_crypto_analysis_t analysis;

    bool result = crypto_analyze_rolling_code(NULL, 16, &analysis);

    TEST_ASSERT(result == false, "Rolling code analysis should fail with NULL data");
}

/**
 * Test: Entropy analysis with valid parameters
 */
void test_crypto_entropy_valid() {
    uint8_t data[256];
    entropy_result_t result;

    // Fill with varied data
    for (int i = 0; i < 256; i++) {
        data[i] = (uint8_t)i;
    }

    bool test_result = crypto_entropy_analysis(data, 256, &result);

    TEST_ASSERT(test_result == true, "Entropy analysis should succeed with valid parameters");
}

/**
 * Test: Known weak key detection
 */
void test_known_weak_key_detection() {
    crypto_config_t config;
    weak_key_result_t result;

    // Known DES weak key (from known_keys.h: 0x01, 0x01, ...)
    uint8_t weak_key[] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

    crypto_analyzer_init(&config);
    // Test with a known weak key
    bool test_result = crypto_test_weak_keys(&config, weak_key, 8, &result);

    TEST_ASSERT(test_result == true, "Weak key test should succeed");
    TEST_ASSERT(result.is_weak == true, "Should identify known weak key");

    // Test with a safe key
    uint8_t safe_key[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF};

    test_result = crypto_test_weak_keys(&config, safe_key, 8, &result);

    TEST_ASSERT(test_result == true, "Safe key test should succeed");
    TEST_ASSERT(result.is_weak == false, "Should not identify safe key as weak");
}

/**
 * Test: Entropy analysis with null data
 */
void test_crypto_entropy_null_data() {
    entropy_result_t result;

    bool test_result = crypto_entropy_analysis(NULL, 256, &result);

    TEST_ASSERT(test_result == false, "Entropy analysis should fail with NULL data");
}

/**
 * Test: Cleanup with valid config
 */
void test_crypto_cleanup_valid() {
    crypto_config_t config;
    crypto_analyzer_init(&config);

    // Should not crash
    crypto_analyzer_cleanup(&config);

    TEST_ASSERT(true, "Cleanup should succeed with valid config");
}

/**
 * Test: Cleanup with null config
 */
void test_crypto_cleanup_null() {
    // Should not crash
    crypto_analyzer_cleanup(NULL);

    TEST_ASSERT(true, "Cleanup should handle NULL config gracefully");
}

/**
 * Main test runner
 */
int main(void) {
    printf("========================================\n");
    printf("Crypto Analyzer Unit Tests\n");
    printf("========================================\n");

    RUN_TEST(test_crypto_init_valid);
    RUN_TEST(test_crypto_init_null);
    RUN_TEST(test_crypto_identify_algorithm_valid);
    RUN_TEST(test_crypto_identify_algorithm_null_data);
    RUN_TEST(test_crypto_identify_algorithm_null_result);
    RUN_TEST(test_crypto_identify_algorithm_zero_length);
    RUN_TEST(test_crypto_key_exchange_valid);
    RUN_TEST(test_crypto_key_exchange_null_data);
    RUN_TEST(test_crypto_key_exchange_null_analysis);
    RUN_TEST(test_crypto_weak_keys_valid);
    RUN_TEST(test_crypto_weak_keys_null_config);
    RUN_TEST(test_crypto_rolling_code_valid);
    RUN_TEST(test_crypto_rolling_code_null_data);
    RUN_TEST(test_crypto_entropy_valid);
    RUN_TEST(test_crypto_entropy_null_data);
    RUN_TEST(test_crypto_cleanup_valid);
    RUN_TEST(test_crypto_cleanup_null);
    RUN_TEST(test_known_weak_key_detection);

    printf("\n========================================\n");
    printf("Test Results:\n");
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
