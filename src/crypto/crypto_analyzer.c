/**
 * Cryptographic Protocol Research Module
 *
 * Provides cryptographic analysis for automotive security protocols
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "crypto/crypto_analyzer.h"
#include "input_validation.h"
#include <math.h>

/**
 * Initialize crypto analyzer
 */
bool crypto_analyzer_init(crypto_config_t *config) {
    if (!validate_not_null(config)) {
        return false;
    }
    if ((int)config->algorithm < CRYPTO_ALG_AES || (int)config->algorithm > CRYPTO_ALG_UNKNOWN) {
        return false;
    }
    if (config->key_length != 128 && config->key_length != 192 && config->key_length != 256) {
        return false;
    }
    if ((int)config->mode < CRYPTO_MODE_ANALYZE || (int)config->mode > CRYPTO_MODE_WEAK_KEYS) {
        return false;
    }
    return true;
}

/**
 * Analyze encryption algorithm
 */
bool crypto_identify_algorithm(uint8_t *ciphertext, uint32_t length,
                                crypto_identification_t *result) {
    if (!validate_not_null(ciphertext) || !validate_not_null(result)) {
        return false;
    }
    if (validate_buffer_length(length, MAX_DATA_BUFFER_SIZE) != VALIDATION_OK) {
        return false;
    }

    result->algorithm = CRYPTO_ALG_UNKNOWN;
    result->confidence = 0.0f;

    // Perform entropy analysis to determine randomness
    entropy_result_t entropy_res;
    if (crypto_entropy_analysis(ciphertext, length, &entropy_res)) {
        // High entropy implies encrypted/compressed data
        if (entropy_res.randomness_quality > 0.90f) {
            // Check block sizes
            if (length % 16 == 0) {
                result->algorithm = CRYPTO_ALG_AES;
                result->confidence = 0.7f;
                safe_strncpy(result->description, 128, "Likely AES (16-byte blocks, high entropy)", 127);
            } else if (length % 8 == 0) {
                result->algorithm = CRYPTO_ALG_DES; // or 3DES
                result->confidence = 0.6f;
                safe_strncpy(result->description, 128, "Likely DES/3DES (8-byte blocks, high entropy)", 127);
            }
        } else {
            // KeeLoq often uses 32-bit (4-byte) blocks and is partially predictable
            if (length % 4 == 0) {
                result->algorithm = CRYPTO_ALG_KEELOQ;
                result->confidence = 0.65f;
                safe_strncpy(result->description, 128, "Potential KeeLoq (4-byte blocks, medium entropy)", 127);
            }
        }
    }

    return true;
}

/**
 * Analyze key exchange protocol
 */
bool crypto_analyze_key_exchange(key_exchange_data_t *data,
                                  key_exchange_analysis_t *analysis) {
    if (!validate_not_null(data) || !validate_not_null(analysis)) {
        return false;
    }

    // Default to unknown
    analysis->protocol_type = KEY_EXCHANGE_UNKNOWN;
    analysis->is_secure = false;

    // Look for RSA modulus patterns (typical sizes 1024, 2048, 4096 bits)
    if (data->public_key_length == 128 || data->public_key_length == 256 || data->public_key_length == 512) {
        analysis->protocol_type = KEY_EXCHANGE_RSA;
        analysis->estimated_key_strength = data->public_key_length * 8;
        if (analysis->estimated_key_strength >= 2048) {
            analysis->is_secure = true;
            safe_strncpy(analysis->notes, 256, "Secure RSA key exchange detected", 255);
        } else {
            safe_strncpy(analysis->notes, 256, "Weak RSA key length detected (<2048 bit)", 255);
        }
    } else if (data->public_key_length == 32 || data->public_key_length == 65) {
        // Typical elliptic curve uncompressed/compressed lengths (e.g. secp256k1/secp256r1)
        analysis->protocol_type = KEY_EXCHANGE_ECDH;
        analysis->is_secure = true;
        analysis->estimated_key_strength = 256;
        safe_strncpy(analysis->notes, 256, "ECDH key exchange detected", 255);
    }

    return true;
}

/**
 * Test for weak keys
 */
bool crypto_test_weak_keys(crypto_config_t *config, uint8_t *key,
                            uint32_t key_length, weak_key_result_t *result) {
    if (!validate_not_null(config) || !validate_not_null(key) || !validate_not_null(result)) {
        return false;
    }
    if (validate_buffer_length(key_length, 256) != VALIDATION_OK) { // Max key length 256 bytes
        return false;
    }

    result->is_weak = false;

    entropy_result_t ent_res;
    crypto_entropy_analysis(key, key_length, &ent_res);
    result->entropy = ent_res.entropy;

    // Check for all zeros or all ones
    bool all_zeros = true;
    bool all_ones = true;
    for (uint32_t i = 0; i < key_length; i++) {
        if (key[i] != 0x00) all_zeros = false;
        if (key[i] != 0xFF) all_ones = false;
    }

    if (all_zeros) {
        result->is_weak = true;
        safe_strncpy(result->reason, 128, "Key consists entirely of null bytes", 127);
        return true;
    }

    if (all_ones) {
        result->is_weak = true;
        safe_strncpy(result->reason, 128, "Key consists entirely of 0xFF bytes", 127);
        return true;
    }

    if (ent_res.randomness_quality < 0.5f) {
        result->is_weak = true;
        safe_strncpy(result->reason, 128, "Key has unacceptably low entropy", 127);
        return true;
    }

    // KeeLoq standard test key (often 0x0123456789ABCDEF)
    if (key_length == 8) {
        uint8_t keeloq_test[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
        bool is_keeloq_test = true;
        for (int i=0; i<8; i++) {
            if (key[i] != keeloq_test[i]) is_keeloq_test = false;
        }
        if (is_keeloq_test) {
            result->is_weak = true;
            safe_strncpy(result->reason, 128, "Default KeeLoq Manufacturer Test Key detected", 127);
            return true;
        }
    }

    return true;
}

/**
 * Analyze rolling code crypto
 */
bool crypto_analyze_rolling_code(uint8_t *encrypted_data, uint32_t length,
                                  rolling_crypto_analysis_t *analysis) {
    if (!validate_not_null(encrypted_data) || !validate_not_null(analysis)) {
        return false;
    }
    if (validate_buffer_length(length, MAX_DATA_BUFFER_SIZE) != VALIDATION_OK) {
        return false;
    }

    analysis->algorithm_detected = false;
    analysis->key_derivation_detected = false;

    // KeeLoq is typically 66 bits (34 fixed/button, 32 rolling) sent in blocks
    // On the air, it's often represented in ~9 bytes.
    if (length == 9 || length == 8) {
        analysis->algorithm_detected = true;
        analysis->detected_algorithm = CRYPTO_ALG_KEELOQ;
        safe_strncpy(analysis->algorithm_name, 32, "KeeLoq", 31);

        // KeeLoq sometimes uses simple XOR with serial for key derivation
        // This would require further multi-packet analysis, marking as potential.
        analysis->key_derivation_detected = true;
    }
    // Hitag2 commonly uses a 48-bit secret key, and pages are 32 bits (4 bytes)
    else if (length == 4 || length == 5) {
        analysis->algorithm_detected = true;
        analysis->detected_algorithm = CRYPTO_ALG_HITAG2;
        safe_strncpy(analysis->algorithm_name, 32, "Hitag2", 31);
    }

    return true;
}

/**
 * Perform entropy analysis
 */
bool crypto_entropy_analysis(uint8_t *data, uint32_t length,
                              entropy_result_t *result) {
    if (!validate_not_null(data) || !validate_not_null(result)) {
        return false;
    }
    if (validate_buffer_length(length, MAX_DATA_BUFFER_SIZE) != VALIDATION_OK) {
        return false;
    }

    // Calculate Shannon entropy
    uint32_t counts[256] = {0};

    for (uint32_t i = 0; i < length; i++) {
        counts[data[i]]++;
    }

    double entropy = 0.0;
    for (int i = 0; i < 256; i++) {
        if (counts[i] > 0) {
            double p = (double)counts[i] / length;
            entropy -= p * (log(p) / log(2.0));
        }
    }

    result->entropy = (float)entropy;
    result->max_entropy = 8.0f;
    result->randomness_quality = entropy / 8.0f;

    return true;
}

/**
 * Cleanup resources
 */
void crypto_analyzer_cleanup(crypto_config_t *config) {
    if (config) {
        // Release resources
    }
}
