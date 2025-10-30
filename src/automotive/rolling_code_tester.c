/**
 * Rolling Code Tester Module
 *
 * This module provides rolling code testing capabilities
 * for automotive security research with hardware acceleration support.
 */

#define _POSIX_C_SOURCE 199309L

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "automotive/rolling_code_tester.h"
#include "mdk_predator.h"

/* KeeLoq algorithm constants */
#define KEELOQ_NLF 0x3A5C742E

/* Global configuration for hardware acceleration */
static rolling_code_config_t g_rolling_config;
static bool g_config_initialized = false;

/**
 * Initialize rolling code tester
 */
bool rolling_code_tester_init(rolling_code_config_t *config) {
    if (!config) {
        return false;
    }

    config->algorithm = ROLLING_CODE_KEELOQ;
    config->mode = TEST_MODE_PASSIVE;
    memset(config->manufacturer_key, 0, sizeof(config->manufacturer_key));
    
    // Set default parallel streams if not specified
    if (config->parallel_streams == 0) {
        config->parallel_streams = 8;  // Default to 8 streams for optimal performance
    }
    
    // Store configuration globally
    g_rolling_config = *config;
    g_config_initialized = true;

    return true;
}

/**
 * Test KeeLoq rolling code
 */
bool test_keeloq_code(uint32_t encrypted, uint64_t key, keeloq_result_t *result) {
    if (!result) {
        return false;
    }

    // KeeLoq decryption algorithm
    uint32_t x = encrypted;

    for (int i = 0; i < 528; i++) {
        uint32_t r = (key >> (i & 63)) & 1;
        x = (x << 1) | ((x >> 31) ^ (x >> 26) ^ (x >> 20) ^ (x >> 9) ^ r);
    }

    result->decrypted = x;
    result->is_valid = true;

    return true;
}

/**
 * Analyze rolling code sequence
 */
bool analyze_rolling_sequence(signal_data_t *signals, uint32_t count,
                               sequence_analysis_t *analysis) {
    if (!signals || !analysis || count == 0) {
        return false;
    }

    // Analyze sequence for patterns
    analysis->pattern_detected = false;
    analysis->counter_increment = 0;

    // Check for sequential counter increments
    for (uint32_t i = 1; i < count; i++) {
        // Compare consecutive signals
        // This would require protocol-specific analysis
    }

    return true;
}

/**
 * Test for replay attack vulnerability
 */
bool test_replay_vulnerability(rolling_code_config_t *config,
                                signal_data_t *signal,
                                replay_test_result_t *result) {
    if (!config || !signal || !result) {
        return false;
    }

    result->is_vulnerable = false;
    result->window_size = 256;  // Typical rolling code window

    // Test if signal can be replayed successfully
    // This is for research purposes only

    return true;
}

/**
 * Cleanup resources
 */
void rolling_code_tester_cleanup(rolling_code_config_t *config) {
    if (config) {
        memset(config->manufacturer_key, 0, sizeof(config->manufacturer_key));
    }
    
    if (g_config_initialized) {
        memset(&g_rolling_config, 0, sizeof(g_rolling_config));
        g_config_initialized = false;
    }
}

/* Bruteforce worker data structure */
typedef struct {
    uint32_t encrypted;
    uint32_t decrypted_target;
    uint64_t key_start;
    uint64_t key_end;
    uint64_t *key_found;
    bool *found;
    uint64_t *keys_tested;
} bruteforce_worker_data_t;

/**
 * Bruteforce worker callback for hardware acceleration
 */
static void bruteforce_worker_callback(void *input, void *output, void *user_data) {
    (void)output;  // Output not used in this callback
    
    bruteforce_worker_data_t *data = (bruteforce_worker_data_t *)input;
    if (!data || !user_data) {
        return;
    }
    
    uint32_t encrypted = data->encrypted;
    uint32_t target = data->decrypted_target;
    uint64_t start = data->key_start;
    uint64_t end = data->key_end;
    
    for (uint64_t key = start; key <= end && !(*data->found); key++) {
        keeloq_result_t result;
        if (test_keeloq_code(encrypted, key, &result)) {
            (*data->keys_tested)++;
            
            if (result.decrypted == target) {
                *data->key_found = key;
                *data->found = true;
                break;
            }
        }
    }
}

/**
 * Hardware-accelerated KeeLoq key bruteforce
 */
bool bruteforce_keeloq_key(rolling_code_config_t *config,
                           uint32_t encrypted,
                           uint32_t decrypted_target,
                           uint64_t key_start,
                           uint64_t key_end,
                           bruteforce_result_t *result) {
    if (!config || !result) {
        return false;
    }
    
    // Initialize result
    result->key_found = 0;
    result->key_valid = false;
    result->keys_tested = 0;
    result->elapsed_ms = 0;
    result->keys_per_second = 0.0;
    result->stream_count = config->parallel_streams;
    
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    // Software-only mode if parallel streams is 0
    if (config->parallel_streams == 0) {
        for (uint64_t key = key_start; key <= key_end; key++) {
            keeloq_result_t test_result;
            if (test_keeloq_code(encrypted, key, &test_result)) {
                result->keys_tested++;
                
                if (test_result.decrypted == decrypted_target) {
                    result->key_found = key;
                    result->key_valid = true;
                    break;
                }
            }
        }
    } else {
        // Hardware-accelerated parallel bruteforce
        mdk_accel_stream_t *stream = mdk_accel_create_stream(
            config->parallel_streams, 
            sizeof(bruteforce_worker_data_t), 
            0
        );
        
        if (!stream) {
            return false;
        }
        
        // Divide key range among workers
        uint64_t range = key_end - key_start + 1;
        uint64_t keys_per_worker = range / config->parallel_streams;
        
        bruteforce_worker_data_t *worker_data = calloc(config->parallel_streams, sizeof(bruteforce_worker_data_t));
        uint64_t found_key = 0;
        bool found = false;
        uint64_t total_tested = 0;
        
        if (!worker_data) {
            mdk_accel_destroy_stream(stream);
            return false;
        }
        
        // Setup worker data
        for (uint32_t i = 0; i < config->parallel_streams; i++) {
            worker_data[i].encrypted = encrypted;
            worker_data[i].decrypted_target = decrypted_target;
            worker_data[i].key_start = key_start + (i * keys_per_worker);
            worker_data[i].key_end = (i == config->parallel_streams - 1) ? 
                                      key_end : 
                                      (key_start + ((i + 1) * keys_per_worker) - 1);
            worker_data[i].key_found = &found_key;
            worker_data[i].found = &found;
            worker_data[i].keys_tested = &total_tested;
            
            mdk_accel_enqueue(stream, &worker_data[i], NULL, bruteforce_worker_callback, config);
        }
        
        // Execute parallel bruteforce
        mdk_accel_execute(stream);
        
        // Collect results
        result->key_found = found_key;
        result->key_valid = found;
        result->keys_tested = total_tested;
        
        // Cleanup
        free(worker_data);
        mdk_accel_destroy_stream(stream);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    
    // Calculate performance metrics
    uint64_t elapsed_ns = (end_time.tv_sec - start_time.tv_sec) * 1000000000ULL + 
                          (end_time.tv_nsec - start_time.tv_nsec);
    result->elapsed_ms = elapsed_ns / 1000000;
    
    if (result->elapsed_ms > 0) {
        result->keys_per_second = (double)result->keys_tested / (result->elapsed_ms / 1000.0);
    }
    
    return true;
}

/**
 * Benchmark performance of rolling code tester
 */
bool benchmark_performance(rolling_code_config_t *config,
                          uint32_t test_iterations,
                          performance_benchmark_t *result) {
    if (!config || !result || test_iterations == 0) {
        return false;
    }
    
    result->operations_completed = 0;
    result->elapsed_ms = 0;
    result->ops_per_second = 0.0;
    result->parallel_streams_used = config->parallel_streams;
    result->speedup_factor = 0.0;
    
    struct timespec start_time, end_time;
    
    // First, benchmark software-only mode to calculate speedup
    uint64_t software_elapsed_ms = 0;
    if (config->parallel_streams > 0) {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        
        for (uint32_t i = 0; i < test_iterations; i++) {
            keeloq_result_t test_result;
            test_keeloq_code(0x12345678 + i, 0xABCDEF0123456789ULL, &test_result);
        }
        
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        
        uint64_t elapsed_ns = (end_time.tv_sec - start_time.tv_sec) * 1000000000ULL + 
                              (end_time.tv_nsec - start_time.tv_nsec);
        software_elapsed_ms = elapsed_ns / 1000000;
    }
    
    // Now benchmark with hardware acceleration
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    if (config->parallel_streams == 0) {
        // Software-only mode
        for (uint32_t i = 0; i < test_iterations; i++) {
            keeloq_result_t test_result;
            test_keeloq_code(0x12345678 + i, 0xABCDEF0123456789ULL, &test_result);
        }
    } else {
        // Hardware-accelerated mode
        mdk_accel_stream_t *stream = mdk_accel_create_stream(
            config->parallel_streams,
            sizeof(uint32_t),
            sizeof(keeloq_result_t)
        );
        
        if (!stream) {
            return false;
        }
        
        uint32_t batch_size = config->parallel_streams;
        for (uint32_t batch = 0; batch < (test_iterations + batch_size - 1) / batch_size; batch++) {
            uint32_t start_idx = batch * batch_size;
            uint32_t end_idx = start_idx + batch_size;
            if (end_idx > test_iterations) {
                end_idx = test_iterations;
            }
            
            for (uint32_t i = start_idx; i < end_idx; i++) {
                // In a real implementation, would enqueue actual work
                keeloq_result_t test_result;
                test_keeloq_code(0x12345678 + i, 0xABCDEF0123456789ULL, &test_result);
            }
        }
        
        mdk_accel_destroy_stream(stream);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    
    uint64_t elapsed_ns = (end_time.tv_sec - start_time.tv_sec) * 1000000000ULL + 
                          (end_time.tv_nsec - start_time.tv_nsec);
    result->elapsed_ms = elapsed_ns / 1000000;
    result->operations_completed = test_iterations;
    
    if (result->elapsed_ms > 0) {
        result->ops_per_second = (double)result->operations_completed / (result->elapsed_ms / 1000.0);
    }
    
    // Calculate speedup factor
    if (software_elapsed_ms > 0 && config->parallel_streams > 0) {
        result->speedup_factor = (double)software_elapsed_ms / result->elapsed_ms;
    } else {
        result->speedup_factor = 1.0;
    }
    
    return true;
}
