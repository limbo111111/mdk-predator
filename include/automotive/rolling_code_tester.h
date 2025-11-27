#ifndef ROLLING_CODE_TESTER_H
#define ROLLING_CODE_TESTER_H

#include "automotive/key_fob_analyzer.h" // For signal_data_t
#include <stdbool.h>

#define ROLLING_CODE_KEELOQ 1
#define TEST_MODE_PASSIVE 1

typedef struct {
    int algorithm;
    int mode;
} rolling_code_config_t;

typedef struct {
    bool is_vulnerable;
    int weakness_type;
} replay_test_result_t;

typedef struct {
    bool is_valid;
} keeloq_result_t;

typedef struct {
    bool is_rolling;
} sequence_analysis_t;

void rolling_code_tester_init(rolling_code_config_t* config);
bool test_replay_vulnerability(rolling_code_config_t* config, signal_data_t* signal, replay_test_result_t* result);
void rolling_code_tester_cleanup(rolling_code_config_t* config);
bool test_keeloq_code(const uint8_t* encrypted, const uint8_t* key, keeloq_result_t* result);
bool analyze_rolling_sequence(const signal_data_t* signals, int count, sequence_analysis_t* analysis);

#endif // ROLLING_CODE_TESTER_H
