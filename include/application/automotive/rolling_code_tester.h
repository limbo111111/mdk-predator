/**
 * Rolling Code Tester Module Header
 */

#ifndef ROLLING_CODE_TESTER_H
#define ROLLING_CODE_TESTER_H

#include <stdint.h>
#include <stdbool.h>
#include "key_fob_analyzer.h"

/* Rolling code algorithm types */
typedef enum {
    ROLLING_CODE_KEELOQ,
    ROLLING_CODE_HCS301,
    ROLLING_CODE_HCS200,
    ROLLING_CODE_CUSTOM
} rolling_code_algorithm_t;

/* Test modes */
typedef enum {
    TEST_MODE_PASSIVE,    // Only analyze
    TEST_MODE_ACTIVE,     // Can transmit (research only)
    /* Aliases for app compatibility */
    ROLLING_CODE_MODE_PASSIVE = TEST_MODE_PASSIVE,
    ROLLING_CODE_MODE_ACTIVE = TEST_MODE_ACTIVE
} test_mode_t;

/* Rolling code configuration - also define with 'mode' field for app compatibility */
typedef struct {
    rolling_code_algorithm_t algorithm;
    test_mode_t mode;  // Changed from test_mode to mode for app compatibility
    uint8_t manufacturer_key[8];
} rolling_code_config_t;

/* KeeLoq test result */
typedef struct {
    uint32_t decrypted;
    uint16_t button_status;
    uint16_t counter;
    bool is_valid;
} keeloq_result_t;

/* Sequence analysis result */
typedef struct {
    bool pattern_detected;
    uint32_t counter_increment;
    uint32_t samples_analyzed;
} sequence_analysis_t;

/* Replay vulnerability test result */
typedef struct {
    bool is_vulnerable;
    uint32_t window_size;
    char description[128];
} replay_test_result_t;

/* Function prototypes */
bool rolling_code_tester_init(rolling_code_config_t *config);
bool test_keeloq_code(uint32_t encrypted, uint64_t key, keeloq_result_t *result);
bool analyze_rolling_sequence(signal_data_t *signals, uint32_t count,
                               sequence_analysis_t *analysis);
bool test_replay_vulnerability(rolling_code_config_t *config,
                                signal_data_t *signal,
                                replay_test_result_t *result);
void rolling_code_tester_cleanup(rolling_code_config_t *config);

#endif /* ROLLING_CODE_TESTER_H */
