/**
 * SubGHz RF Signal Analyzer Module Header
 */

#ifndef SUBGHZ_ANALYZER_H
#define SUBGHZ_ANALYZER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_SIGNAL_SAMPLES 1000000

/* Modulation types */
typedef enum {
    SUBGHZ_MOD_ASK_OOK,
    SUBGHZ_MOD_2FSK,
    SUBGHZ_MOD_4FSK,
    SUBGHZ_MOD_GFSK,
    SUBGHZ_MOD_MSK,
    SUBGHZ_MOD_UNKNOWN
} subghz_modulation_t;

/* Protocol types */
typedef enum {
    PROTOCOL_PRINCETON,
    PROTOCOL_CAME,
    PROTOCOL_NICE_FLO,
    PROTOCOL_GATE_TX,
    PROTOCOL_KEELOQ,
    PROTOCOL_UNKNOWN
} protocol_type_t;

/* SubGHz configuration */
typedef struct {
    uint32_t frequency;
    uint32_t bandwidth;
    uint32_t sample_rate;
    subghz_modulation_t modulation;
} subghz_config_t;

/* Spectrum data */
typedef struct {
    float power_levels[1024];
    uint32_t frequencies[1024];
    uint32_t sample_count;
    uint32_t peak_frequency;
    float peak_power;
} spectrum_data_t;

/* RF signal */
typedef struct {
    int16_t *i_samples;
    int16_t *q_samples;
    uint32_t sample_count;
    uint32_t frequency;
    uint64_t timestamp;
} rf_signal_t;

/* Signal analysis result */
typedef struct {
    subghz_modulation_t modulation_detected;
    uint32_t bit_rate;
    bool protocol_detected;
    char protocol_name[32];
} signal_analysis_t;

/* Protocol data */
typedef struct {
    protocol_type_t type;
    uint8_t data[64];
    uint32_t data_length;
    uint32_t bit_count;
} protocol_data_t;

/* Raw signal recording */
typedef struct {
    int16_t *samples;
    uint32_t sample_count;
    uint32_t duration_ms;
} raw_signal_t;

/* Function prototypes */
bool subghz_analyzer_init(subghz_config_t *config);
bool subghz_scan_spectrum(subghz_config_t *config, spectrum_data_t *spectrum);
bool subghz_capture_signal(subghz_config_t *config, rf_signal_t *signal);
bool subghz_analyze_signal(rf_signal_t *signal, signal_analysis_t *analysis);
bool subghz_decode_protocol(rf_signal_t *signal, protocol_data_t *protocol);
bool subghz_record_raw(subghz_config_t *config, uint32_t duration_ms, raw_signal_t *raw_data);
bool subghz_replay_signal(subghz_config_t *config, rf_signal_t *signal);
void subghz_analyzer_cleanup(subghz_config_t *config);

#endif /* SUBGHZ_ANALYZER_H */
