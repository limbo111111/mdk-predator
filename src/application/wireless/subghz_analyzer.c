/**
 * SubGHz RF Signal Analyzer Module
 *
 * Provides SubGHz RF signal analysis for Mayhem-MDK
 * - Spectrum scanning (300 MHz to 928 MHz)
 * - Signal capture and demodulation
 * - Protocol detection (PT2262, KeeLoq, proprietary)
 * - Raw IQ sample recording
 * - Analysis-only mode (no transmit)
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "application/wireless/subghz_analyzer.h"

/* Common SubGHz frequencies */
static const uint32_t SUBGHZ_FREQUENCIES[] = {
    300000000,   /* 300 MHz (remote controls) */
    315000000,   /* 315 MHz (garage doors, key fobs US) */
    433920000,   /* 433.92 MHz (ISM, home automation EU) */
    868000000,   /* 868 MHz (ISM EU) */
    915000000    /* 915 MHz (ISM US) */
};

/* Demodulation methods */
#define SUBGHZ_DEMOD_ASK 1
#define SUBGHZ_DEMOD_FSK 2

/**
 * Initialize SubGHz analyzer for PortaPack HackRF
 * Configures RF frontend for initial frequency and sample rate
 */
bool subghz_analyzer_init(subghz_config_t *config) {
    if (!config) {
        fprintf(stderr, "[ERROR] SubGHz analyzer config pointer is NULL\n");
        return false;
    }

    config->frequency = SUBGHZ_FREQUENCIES[2];  /* Default to 433.92 MHz */
    config->bandwidth = 200000;  /* 200 kHz bandwidth for narrowband analysis */
    config->sample_rate = 2000000;  /* 2 MS/s (HackRF oneHz typical) */
    config->modulation = SUBGHZ_MOD_ASK_OOK;

    /* Spectrum scan configuration */
    config->frequency_min = 300000000;  /* 300 MHz */
    config->frequency_max = 928000000;  /* 928 MHz */
    config->step_size = 50000;  /* 50 kHz steps */

    fprintf(stdout, "[SubGHz] Analyzer initialized: freq=%u Hz, BW=%u Hz, fs=%u Hz\n",
            config->frequency, config->bandwidth, config->sample_rate);
    return true;
}

/**
 * Scan SubGHz spectrum across frequency range
 * Measures signal strength (RSSI) at each frequency step
 */
bool subghz_scan_spectrum(subghz_config_t *config, spectrum_data_t *spectrum) {
    if (!config || !spectrum) {
        fprintf(stderr, "[ERROR] SubGHz spectrum scan parameters invalid\n");
        return false;
    }

    memset(spectrum, 0, sizeof(*spectrum));

    /* In real PortaPack deployment:
     * - Sweep HackRF across frequency range (config->frequency_min to _max)
     * - At each step, capture short FFT window
     * - Calculate power spectral density (PSD)
     * - Store power level and frequency
     * - Detect peaks (potential RF signals)
     */

    /* Simulation: Generate synthetic spectrum peaks at known frequencies */

    uint32_t scan_points = (config->frequency_max - config->frequency_min) / config->step_size;
    if (scan_points > 1024) scan_points = 1024;

    spectrum->sample_count = scan_points;

    /* Peak 1: 433.92 MHz (common ISM) */
    if (spectrum->sample_count > 0) {
        spectrum->frequencies[0] = 433920000;
        spectrum->power_levels[0] = -45.0f;  /* Strong signal */
        if (spectrum->power_levels[0] > spectrum->peak_power) {
            spectrum->peak_frequency = spectrum->frequencies[0];
            spectrum->peak_power = spectrum->power_levels[0];
        }
    }

    /* Peak 2: 915 MHz (US ISM) */
    if (spectrum->sample_count > 100) {
        spectrum->frequencies[100] = 915000000;
        spectrum->power_levels[100] = -65.0f;  /* Moderate signal */
    }

    /* Noise floor */
    for (uint32_t i = 0; i < spectrum->sample_count; i++) {
        if (i != 0 && i != 100) {
            spectrum->frequencies[i] = config->frequency_min + i * config->step_size;
            spectrum->power_levels[i] = -90.0f + (rand() % 5);  /* -90 to -85 dBm noise floor */
        }
    }

    fprintf(stdout, "[SubGHz] Spectrum scan: %u points, peak=%u Hz @ %.1f dBm\n",
            spectrum->sample_count, spectrum->peak_frequency, spectrum->peak_power);

    return true;
}

/**
 * Capture RF signal at configured frequency
 * Acquires raw IQ samples from HackRF ADC
 */
bool subghz_capture_signal(subghz_config_t *config, rf_signal_t *signal) {
    if (!config || !signal) {
        fprintf(stderr, "[ERROR] SubGHz signal capture parameters invalid\n");
        return false;
    }

    memset(signal, 0, sizeof(*signal));

    /* In real PortaPack deployment:
     * - Configure HackRF ADC to config->frequency
     * - Allocate buffers for I/Q samples
     * - Start DMA transfer from HackRF to RAM
     * - Wait for buffer full or timeout
     * - Return sample count and timestamp
     */

    signal->frequency = config->frequency;
    signal->sample_count = 0;  /* Placeholder: no actual hardware */
    signal->timestamp = 0;

    fprintf(stdout, "[SubGHz] Signal capture: freq=%u Hz, fs=%u Hz, samples=%u\n",
            signal->frequency, config->sample_rate, signal->sample_count);

    return true;
}

/**
 * Analyze captured RF signal
 * Detects modulation type and extracts parameters
 */
bool subghz_analyze_signal(rf_signal_t *signal, signal_analysis_t *analysis) {
    if (!signal || !analysis) {
        fprintf(stderr, "[ERROR] SubGHz signal analysis parameters invalid\n");
        return false;
    }

    memset(analysis, 0, sizeof(*analysis));

    /* In real PortaPack deployment:
     * - Compute FFT of I/Q samples
     * - Measure bandwidth (occupied frequency width)
     * - Perform autocorrelation to estimate symbol rate
     * - Analyze envelope for modulation type (ASK/FSK/MSK)
     * - Estimate bit rate from symbol rate
     */

    /* Simulation: Detect modulation based on frequency */
    if (signal->frequency >= 300000000 && signal->frequency <= 450000000) {
        analysis->modulation_detected = SUBGHZ_MOD_ASK_OOK;  /* Remote controls use ASK/OOK */
        analysis->bit_rate = 1000;  /* ~1 kbps */
        analysis->protocol_detected = true;
        strncpy(analysis->protocol_name, "Princeton-like", 32);
    } else if (signal->frequency >= 850000000 && signal->frequency <= 928000000) {
        analysis->modulation_detected = SUBGHZ_MOD_2FSK;  /* ISM band uses FSK */
        analysis->bit_rate = 10000;  /* ~10 kbps */
        analysis->protocol_detected = true;
        strncpy(analysis->protocol_name, "KeeLoq-like", 32);
    } else {
        analysis->modulation_detected = SUBGHZ_MOD_UNKNOWN;
        analysis->protocol_detected = false;
    }

    fprintf(stdout, "[SubGHz] Signal analysis: modulation=%d, bit_rate=%u bps, protocol=%s\n",
            analysis->modulation_detected, analysis->bit_rate, analysis->protocol_name);

    return true;
}

/**
 * Decode common SubGHz protocols (PT2262, KeeLoq, etc.)
 * Extracts command and address bits from demodulated signal
 */
bool subghz_decode_protocol(rf_signal_t *signal, protocol_data_t *protocol) {
    if (!signal || !protocol) {
        fprintf(stderr, "[ERROR] SubGHz protocol decode parameters invalid\n");
        return false;
    }

    memset(protocol, 0, sizeof(*protocol));

    /* In real PortaPack deployment:
     * - Demodulate signal using detected modulation method
     * - Search for preamble/sync patterns
     * - Decode frame structure (address, command, checksum)
     * - Identify protocol type (PT2262 = 12-bit address + 4-bit command)
     * - Validate checksum/CRC
     */

    /* Simulation: Decode as generic protocol */

    /* Example: PT2262-style protocol (garage door openers) */
    protocol->type = PROTOCOL_PRINCETON;

    /* Simulate decoded address + command */
    protocol->data[0] = 0x12;  /* Address byte 1 */
    protocol->data[1] = 0x34;  /* Address byte 2 */
    protocol->data[2] = 0x56;  /* Command byte */
    protocol->data_length = 3;
    protocol->bit_count = 24;

    fprintf(stdout, "[SubGHz] Protocol decode: type=%d, length=%u bits, data=%02X %02X %02X\n",
            protocol->type, protocol->bit_count, protocol->data[0], protocol->data[1], protocol->data[2]);

    return true;
}

/**
 * Record raw IQ samples to buffer
 * Captures unprocessed signal for later analysis or replay
 */
bool subghz_record_raw(subghz_config_t *config, uint32_t duration_ms,
                       raw_signal_t *raw_data) {
    if (!config || !raw_data) {
        fprintf(stderr, "[ERROR] SubGHz raw record parameters invalid\n");
        return false;
    }

    memset(raw_data, 0, sizeof(*raw_data));

    /* In real PortaPack deployment:
     * - Configure HackRF for raw IQ capture
     * - Allocate buffer: sample_count = sample_rate * duration_ms / 1000
     * - Start DMA transfer
     * - Store raw int16_t I/Q pairs
     */

    raw_data->sample_count = (config->sample_rate * duration_ms) / 1000;
    if (raw_data->sample_count > MAX_SIGNAL_SAMPLES) {
        raw_data->sample_count = MAX_SIGNAL_SAMPLES;
    }

    raw_data->duration_ms = duration_ms;

    fprintf(stdout, "[SubGHz] Raw recording: %u samples, %u ms @ %u Hz\n",
            raw_data->sample_count, raw_data->duration_ms, config->sample_rate);

    return true;
}

/**
 * Replay captured signal (ANALYSIS ONLY - transmit disabled by default)
 * 
 * SECURITY NOTE: This function is analysis-only in this build.
 * Transmission requires explicit user approval and proper regulatory compliance.
 * Unauthorized RF transmission is illegal in most jurisdictions.
 */
bool subghz_replay_signal(subghz_config_t *config, rf_signal_t *signal) {
    if (!config || !signal) {
        fprintf(stderr, "[ERROR] SubGHz replay parameters invalid\n");
        return false;
    }

    /* BLOCKED: Transmit intentionally disabled for security */
    fprintf(stderr, "[SECURITY] RF transmission blocked by default. Use 'enable_transmit' flag.\n");
    fprintf(stderr, "[WARNING] Unauthorized transmission is illegal. Check local regulations.\n");

    return false;
}

/**
 * Cleanup SubGHz analyzer resources
 */
void subghz_analyzer_cleanup(subghz_config_t *config) {
    if (!config) {
        return;
    }
    fprintf(stdout, "[SubGHz] Analyzer cleaned up\n");
}
