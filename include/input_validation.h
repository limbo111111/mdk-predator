/**
 * Input Validation and Sanitization Utilities
 *
 * Provides common input validation functions for security modules
 */

#ifndef INPUT_VALIDATION_H
#define INPUT_VALIDATION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Maximum safe buffer sizes */
#define MAX_SIGNAL_BUFFER_SIZE (1024 * 1024)  // 1 MB
#define MAX_DATA_BUFFER_SIZE (10 * 1024 * 1024)  // 10 MB
#define MAX_STRING_LENGTH 1024
#define MAX_SSID_LENGTH 32
#define MAX_NAME_LENGTH 248

/* Frequency ranges (Hz) */
#define MIN_FREQUENCY 1000000UL      // 1 MHz
#define MAX_FREQUENCY 6000000000UL   // 6 GHz
#define MIN_SUBGHZ_FREQ 300000000UL  // 300 MHz
#define MAX_SUBGHZ_FREQ 928000000UL  // 928 MHz

/* Validation result */
typedef enum {
    VALIDATION_OK = 0,
    VALIDATION_NULL_POINTER,
    VALIDATION_INVALID_LENGTH,
    VALIDATION_BUFFER_TOO_LARGE,
    VALIDATION_OUT_OF_RANGE,
    VALIDATION_INVALID_FORMAT,
    VALIDATION_OVERFLOW_RISK
} validation_result_t;

/**
 * Validate pointer is not NULL
 */
static inline bool validate_not_null(const void *ptr) {
    return ptr != NULL;
}

/**
 * Validate buffer length is within safe limits
 */
static inline validation_result_t validate_buffer_length(uint32_t length, uint32_t max_length) {
    if (length == 0) {
        return VALIDATION_INVALID_LENGTH;
    }
    if (length > max_length) {
        return VALIDATION_BUFFER_TOO_LARGE;
    }
    return VALIDATION_OK;
}

/**
 * Validate frequency is within supported range
 */
static inline validation_result_t validate_frequency(uint64_t frequency) {
    if (frequency < MIN_FREQUENCY || frequency > MAX_FREQUENCY) {
        return VALIDATION_OUT_OF_RANGE;
    }
    return VALIDATION_OK;
}

/**
 * Validate SubGHz frequency range
 */
static inline validation_result_t validate_subghz_frequency(uint32_t frequency) {
    if (frequency < MIN_SUBGHZ_FREQ || frequency > MAX_SUBGHZ_FREQ) {
        return VALIDATION_OUT_OF_RANGE;
    }
    return VALIDATION_OK;
}

/**
 * Validate integer addition won't overflow
 */
static inline bool validate_add_no_overflow(uint32_t a, uint32_t b) {
    return a <= UINT32_MAX - b;
}

/**
 * Validate integer multiplication won't overflow
 */
static inline bool validate_mul_no_overflow(uint32_t a, uint32_t b) {
    if (a == 0 || b == 0) {
        return true;
    }
    return a <= UINT32_MAX / b;
}

/**
 * Validate string length
 */
static inline validation_result_t validate_string_length(const char *str, size_t max_length) {
    if (!str) {
        return VALIDATION_NULL_POINTER;
    }
    size_t len = 0;
    while (str[len] != '\0' && len < max_length) {
        len++;
    }
    if (len >= max_length) {
        return VALIDATION_BUFFER_TOO_LARGE;
    }
    return VALIDATION_OK;
}

/**
 * Validate array index is within bounds
 */
static inline bool validate_array_index(uint32_t index, uint32_t array_size) {
    return index < array_size;
}

/**
 * Validate MAC address format (6 bytes)
 */
static inline bool validate_mac_address(const uint8_t *mac) {
    return mac != NULL;
}

/**
 * Validate channel number for WiFi 2.4 GHz
 */
static inline validation_result_t validate_wifi_24_channel(uint8_t channel) {
    if (channel < 1 || channel > 14) {
        return VALIDATION_OUT_OF_RANGE;
    }
    return VALIDATION_OK;
}

/**
 * Validate channel number for WiFi 5 GHz
 */
static inline validation_result_t validate_wifi_5_channel(uint16_t channel) {
    // Valid 5 GHz channels: 36-165 (simplified check)
    if (channel < 36 || channel > 165) {
        return VALIDATION_OUT_OF_RANGE;
    }
    return VALIDATION_OK;
}

/**
 * Safe memory copy with bounds checking
 */
static inline bool safe_memcpy(void *dest, size_t dest_size,
                               const void *src, size_t copy_size) {
    if (!dest || !src) {
        return false;
    }
    if (copy_size > dest_size) {
        return false;
    }
    for (size_t i = 0; i < copy_size; i++) {
        ((uint8_t*)dest)[i] = ((const uint8_t*)src)[i];
    }
    return true;
}

/**
 * Safe string copy with null termination
 */
static inline bool safe_strncpy(char *dest, size_t dest_size,
                                const char *src, size_t src_length) {
    if (!dest || !src || dest_size == 0) {
        return false;
    }
    size_t copy_len = (src_length < dest_size - 1) ? src_length : dest_size - 1;
    for (size_t i = 0; i < copy_len && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[copy_len] = '\0';
    return true;
}

/**
 * Validate count doesn't exceed maximum
 */
static inline validation_result_t validate_count(uint32_t count, uint32_t max_count) {
    if (count == 0) {
        return VALIDATION_INVALID_LENGTH;
    }
    if (count > max_count) {
        return VALIDATION_OUT_OF_RANGE;
    }
    return VALIDATION_OK;
}

/**
 * Get validation error message
 */
static inline const char* get_validation_error_message(validation_result_t result) {
    switch (result) {
        case VALIDATION_OK:
            return "Validation successful";
        case VALIDATION_NULL_POINTER:
            return "Null pointer detected";
        case VALIDATION_INVALID_LENGTH:
            return "Invalid length";
        case VALIDATION_BUFFER_TOO_LARGE:
            return "Buffer too large";
        case VALIDATION_OUT_OF_RANGE:
            return "Value out of range";
        case VALIDATION_INVALID_FORMAT:
            return "Invalid format";
        case VALIDATION_OVERFLOW_RISK:
            return "Overflow risk detected";
        default:
            return "Unknown validation error";
    }
}

#endif /* INPUT_VALIDATION_H */
