/**
 * Key Fob Analyzer Module Header
 */

#ifndef KEY_FOB_ANALYZER_H
#define KEY_FOB_ANALYZER_H

#include <stdint.h>
#include <stdbool.h>

/* Key fob operation modes */
typedef enum {
    KEYFOB_MODE_RECEIVE,
    KEYFOB_MODE_TRANSMIT,
    KEYFOB_MODE_ANALYZE
} keyfob_mode_t;

/* Key fob configuration */
typedef struct {
    uint32_t frequency;
    uint32_t bandwidth;
    uint32_t sample_rate;
    keyfob_mode_t mode;
} keyfob_config_t;

/* Signal data structure */
typedef struct {
    uint8_t *data;
    uint32_t length;
    uint32_t frequency;
    uint64_t timestamp;
} signal_data_t;

/* Analysis result */
typedef struct {
    uint8_t protocol_type;
    uint32_t code;
    bool is_rolling_code;
    uint8_t manufacturer_id;
} keyfob_analysis_t;

/* Rolling code information */
typedef struct {
    uint32_t counter;
    uint32_t encrypted_data;
    uint8_t algorithm_type;
} rolling_code_info_t;

/* Function prototypes */
bool keyfob_analyzer_init(keyfob_config_t *config);
bool keyfob_capture_signal(keyfob_config_t *config, signal_data_t *signal);
bool keyfob_analyze_signal(signal_data_t *signal, keyfob_analysis_t *result);
bool keyfob_detect_rolling_code(signal_data_t *signal, rolling_code_info_t *info);
void keyfob_analyzer_cleanup(keyfob_config_t *config);

#endif /* KEY_FOB_ANALYZER_H */
