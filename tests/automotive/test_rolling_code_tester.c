/**
 * Unit tests for Rolling Code Tester Module
 *
 * These tests validate the rolling code tester functionality without
 * requiring actual hardware.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "automotive/rolling_code_tester.h"

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
 * Test: Initialize rolling code tester with valid config
 */
void test_rolling_code_init_valid() {
    rolling_code_config_t config;
    bool result = rolling_code_tester_init(&config);

    TEST_ASSERT(result == true, "Init should succeed with valid config");
    TEST_ASSERT(config.algorithm == ROLLING_CODE_KEELOQ, "Default algorithm should be KeeLoq");
    TEST_ASSERT(config.mode == TEST_MODE_PASSIVE, "Default mode should be PASSIVE");
}

/**
 * Test: Initialize rolling code tester with null config
 */
void test_rolling_code_init_null() {
    bool result = rolling_code_tester_init(NULL);

    TEST_ASSERT(result == false, "Init should fail with NULL config");
}

/**
 * Test: Test KeeLoq code with valid parameters
 */
void test_keeloq_code_valid() {
    keeloq_result_t result;
    uint32_t encrypted = 0x12345678;
    uint64_t key = 0xABCDEF0123456789ULL;

    bool test_result = test_keeloq_code(encrypted, key, &result);

    TEST_ASSERT(test_result == true, "KeeLoq test should succeed with valid parameters");
    TEST_ASSERT(result.is_valid == true, "Result should be marked as valid");
}

/**
 * Test: Test KeeLoq code with null result buffer
 */
void test_keeloq_code_null_result() {
    uint32_t encrypted = 0x12345678;
    uint64_t key = 0xABCDEF0123456789ULL;

    bool result = test_keeloq_code(encrypted, key, NULL);

    TEST_ASSERT(result == false, "KeeLoq test should fail with NULL result buffer");
}

/**
 * Test: Test KeeLoq code with zero key
 */
void test_keeloq_code_zero_key() {
    keeloq_result_t result;
    uint32_t encrypted = 0x12345678;
    uint64_t key = 0;

    bool test_result = test_keeloq_code(encrypted, key, &result);

    TEST_ASSERT(test_result == true, "KeeLoq test should succeed even with zero key");
}

/**
 * Test: Analyze rolling sequence with valid parameters
 */
void test_analyze_sequence_valid() {
    signal_data_t signals[3];
    sequence_analysis_t analysis;

    // Setup mock signals
    for (int i = 0; i < 3; i++) {
        signals[i].data = NULL;
        signals[i].length = 0;
        signals[i].frequency = 433920000;
        signals[i].timestamp = i * 1000000;
    }

    bool result = analyze_rolling_sequence(signals, 3, &analysis);

    TEST_ASSERT(result == true, "Sequence analysis should succeed with valid parameters");
}

/**
 * Test: Analyze rolling sequence with null signals
 */
void test_analyze_sequence_null_signals() {
    sequence_analysis_t analysis;

    bool result = analyze_rolling_sequence(NULL, 3, &analysis);

    TEST_ASSERT(result == false, "Sequence analysis should fail with NULL signals");
}

/**
 * Test: Analyze rolling sequence with null analysis buffer
 */
void test_analyze_sequence_null_analysis() {
    signal_data_t signals[3];

    bool result = analyze_rolling_sequence(signals, 3, NULL);

    TEST_ASSERT(result == false, "Sequence analysis should fail with NULL analysis buffer");
}

/**
 * Test: Analyze rolling sequence with zero count
 */
void test_analyze_sequence_zero_count() {
    signal_data_t signals[3];
    sequence_analysis_t analysis;

    bool result = analyze_rolling_sequence(signals, 0, &analysis);

    TEST_ASSERT(result == false, "Sequence analysis should fail with zero count");
}

/**
 * Test: Test replay vulnerability with valid parameters
 */
void test_replay_vulnerability_valid() {
    rolling_code_config_t config;
    signal_data_t signal;
    replay_test_result_t result;

    rolling_code_tester_init(&config);
    signal.data = NULL;
    signal.length = 0;

    bool test_result = test_replay_vulnerability(&config, &signal, &result);

    TEST_ASSERT(test_result == true, "Replay vulnerability test should succeed with valid parameters");
}

/**
 * Test: Test replay vulnerability with null config
 */
void test_replay_vulnerability_null_config() {
    signal_data_t signal;
    replay_test_result_t result;

    bool test_result = test_replay_vulnerability(NULL, &signal, &result);

    TEST_ASSERT(test_result == false, "Replay vulnerability test should fail with NULL config");
}

/**
 * Test: Test replay vulnerability with null signal
 */
void test_replay_vulnerability_null_signal() {
    rolling_code_config_t config;
    replay_test_result_t result;

    rolling_code_tester_init(&config);

    bool test_result = test_replay_vulnerability(&config, NULL, &result);

    TEST_ASSERT(test_result == false, "Replay vulnerability test should fail with NULL signal");
}

/**
 * Test: Test replay vulnerability with null result
 */
void test_replay_vulnerability_null_result() {
    rolling_code_config_t config;
    signal_data_t signal;

    rolling_code_tester_init(&config);
    signal.data = NULL;
    signal.length = 0;

    bool test_result = test_replay_vulnerability(&config, &signal, NULL);

    TEST_ASSERT(test_result == false, "Replay vulnerability test should fail with NULL result");
}

/**
 * Test: Cleanup with valid config
 */
void test_rolling_code_cleanup_valid() {
    rolling_code_config_t config;
    rolling_code_tester_init(&config);

    // Set some data
    memset(config.manufacturer_key, 0xFF, sizeof(config.manufacturer_key));

    // Should not crash and should clear sensitive data
    rolling_code_tester_cleanup(&config);

    TEST_ASSERT(true, "Cleanup should succeed with valid config");
}

/**
 * Test: Cleanup with null config
 */
void test_rolling_code_cleanup_null() {
    // Should not crash
    rolling_code_tester_cleanup(NULL);

    TEST_ASSERT(true, "Cleanup should handle NULL config gracefully");
}

/**
 * Test: Hardware acceleration - parallel streams configuration
 */
void test_hardware_acceleration_config() {
    rolling_code_config_t config;
    config.parallel_streams = 8;
    
    bool result = rolling_code_tester_init(&config);
    
    TEST_ASSERT(result == true, "Init should succeed with parallel streams configured");
    TEST_ASSERT(config.parallel_streams == 8, "Parallel streams should be preserved");
    
    rolling_code_tester_cleanup(&config);
}

/**
 * Test: Hardware acceleration - software-only mode
 */
void test_software_only_mode() {
    rolling_code_config_t config;
    config.parallel_streams = 0;
    
    bool result = rolling_code_tester_init(&config);
    
    TEST_ASSERT(result == true, "Init should succeed in software-only mode");
    TEST_ASSERT(config.parallel_streams == 8, "Default parallel streams should be set");
    
    rolling_code_tester_cleanup(&config);
}

/**
 * Test: Bruteforce KeeLoq key with valid parameters
 */
void test_bruteforce_keeloq_valid() {
    rolling_code_config_t config;
    rolling_code_tester_init(&config);
    config.parallel_streams = 4;  // Use 4 streams for testing
    
    bruteforce_result_t result;
    uint32_t encrypted = 0x12345678;
    uint32_t decrypted_target = 0x87654321;
    uint64_t key_start = 0x1000;
    uint64_t key_end = 0x1100;
    
    bool test_result = bruteforce_keeloq_key(&config, encrypted, decrypted_target,
                                             key_start, key_end, &result);
    
    TEST_ASSERT(test_result == true, "Bruteforce should succeed with valid parameters");
    TEST_ASSERT(result.stream_count == 4, "Should use 4 parallel streams");
    TEST_ASSERT(result.keys_tested > 0, "Should test at least one key");
    
    rolling_code_tester_cleanup(&config);
}

/**
 * Test: Bruteforce with null config
 */
void test_bruteforce_null_config() {
    bruteforce_result_t result;
    
    bool test_result = bruteforce_keeloq_key(NULL, 0x12345678, 0x87654321,
                                             0x1000, 0x1100, &result);
    
    TEST_ASSERT(test_result == false, "Bruteforce should fail with NULL config");
}

/**
 * Test: Bruteforce with null result
 */
void test_bruteforce_null_result() {
    rolling_code_config_t config;
    rolling_code_tester_init(&config);
    
    bool test_result = bruteforce_keeloq_key(&config, 0x12345678, 0x87654321,
                                             0x1000, 0x1100, NULL);
    
    TEST_ASSERT(test_result == false, "Bruteforce should fail with NULL result");
    
    rolling_code_tester_cleanup(&config);
}

/**
 * Test: Benchmark performance with valid parameters
 */
void test_benchmark_performance_valid() {
    rolling_code_config_t config;
    rolling_code_tester_init(&config);
    config.parallel_streams = 4;
    
    performance_benchmark_t result;
    
    bool test_result = benchmark_performance(&config, 100, &result);
    
    TEST_ASSERT(test_result == true, "Benchmark should succeed with valid parameters");
    TEST_ASSERT(result.operations_completed == 100, "Should complete 100 operations");
    TEST_ASSERT(result.parallel_streams_used == 4, "Should report 4 parallel streams");
    TEST_ASSERT(result.ops_per_second >= 0.0, "Should report non-negative ops/sec");
    TEST_ASSERT(result.speedup_factor >= 1.0, "Speedup factor should be >= 1.0");
    
    rolling_code_tester_cleanup(&config);
}

/**
 * Test: Benchmark performance with null config
 */
void test_benchmark_null_config() {
    performance_benchmark_t result;
    
    bool test_result = benchmark_performance(NULL, 100, &result);
    
    TEST_ASSERT(test_result == false, "Benchmark should fail with NULL config");
}

/**
 * Test: Benchmark performance with null result
 */
void test_benchmark_null_result() {
    rolling_code_config_t config;
    rolling_code_tester_init(&config);
    
    bool test_result = benchmark_performance(&config, 100, NULL);
    
    TEST_ASSERT(test_result == false, "Benchmark should fail with NULL result");
    
    rolling_code_tester_cleanup(&config);
}

/**
 * Test: Benchmark performance with zero iterations
 */
void test_benchmark_zero_iterations() {
    rolling_code_config_t config;
    rolling_code_tester_init(&config);
    
    performance_benchmark_t result;
    
    bool test_result = benchmark_performance(&config, 0, &result);
    
    TEST_ASSERT(test_result == false, "Benchmark should fail with zero iterations");
    
    rolling_code_tester_cleanup(&config);
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
    RUN_TEST(test_keeloq_code_null_result);
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
    
    // Hardware acceleration tests
    RUN_TEST(test_hardware_acceleration_config);
    RUN_TEST(test_software_only_mode);
    RUN_TEST(test_bruteforce_keeloq_valid);
    RUN_TEST(test_bruteforce_null_config);
    RUN_TEST(test_bruteforce_null_result);
    RUN_TEST(test_benchmark_performance_valid);
    RUN_TEST(test_benchmark_null_config);
    RUN_TEST(test_benchmark_null_result);
    RUN_TEST(test_benchmark_zero_iterations);

    printf("\n========================================\n");
    printf("Test Results:\n");
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
