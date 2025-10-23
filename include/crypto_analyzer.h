/**
 * Cryptographic Protocol Research Module Header
 */

#ifndef CRYPTO_ANALYZER_H
#define CRYPTO_ANALYZER_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* Cryptographic algorithms */
typedef enum {
    CRYPTO_ALG_AES,
    CRYPTO_ALG_DES,
    CRYPTO_ALG_3DES,
    CRYPTO_ALG_RSA,
    CRYPTO_ALG_KEELOQ,
    CRYPTO_ALG_HITAG2,
    CRYPTO_ALG_UNKNOWN,
    /* Aliases for app compatibility */
    CRYPTO_ALGORITHM_AES = CRYPTO_ALG_AES,
    CRYPTO_ALGORITHM_DES = CRYPTO_ALG_DES,
    CRYPTO_ALGORITHM_3DES = CRYPTO_ALG_3DES,
    CRYPTO_ALGORITHM_RSA = CRYPTO_ALG_RSA
} crypto_algorithm_t;

/* Crypto operation modes */
typedef enum {
    CRYPTO_MODE_ANALYZE,
    CRYPTO_MODE_TEST,
    CRYPTO_MODE_RESEARCH
} crypto_mode_t;

/* Key exchange protocols */
typedef enum {
    KEY_EXCHANGE_DH,
    KEY_EXCHANGE_ECDH,
    KEY_EXCHANGE_RSA,
    KEY_EXCHANGE_UNKNOWN
} key_exchange_protocol_t;

/* Crypto configuration */
typedef struct {
    crypto_algorithm_t algorithm;
    uint32_t key_length;
    crypto_mode_t mode;
} crypto_config_t;

/* Algorithm identification result */
typedef struct {
    crypto_algorithm_t algorithm;
    float confidence;
    char description[128];
} crypto_identification_t;

/* Key exchange data */
typedef struct {
    uint8_t public_key[512];
    uint32_t public_key_length;
    uint8_t exchange_data[1024];
    uint32_t exchange_data_length;
} key_exchange_data_t;

/* Key exchange analysis */
typedef struct {
    key_exchange_protocol_t protocol_type;
    bool is_secure;
    uint32_t estimated_key_strength;
    char notes[256];
} key_exchange_analysis_t;

/* Weak key test result */
typedef struct {
    bool is_weak;
    float entropy;
    char reason[128];
} weak_key_result_t;

/* Rolling code crypto analysis */
typedef struct {
    bool algorithm_detected;
    bool key_derivation_detected;
    crypto_algorithm_t detected_algorithm;
    char algorithm_name[32];
} rolling_crypto_analysis_t;

/* Entropy analysis result */
typedef struct {
    float entropy;
    float max_entropy;
    float randomness_quality;
} entropy_result_t;

/* Function prototypes */
bool crypto_analyzer_init(crypto_config_t *config);
bool crypto_identify_algorithm(uint8_t *ciphertext, uint32_t length, 
                                crypto_identification_t *result);
bool crypto_analyze_key_exchange(key_exchange_data_t *data, 
                                  key_exchange_analysis_t *analysis);
bool crypto_test_weak_keys(crypto_config_t *config, uint8_t *key, 
                            uint32_t key_length, weak_key_result_t *result);
bool crypto_analyze_rolling_code(uint8_t *encrypted_data, uint32_t length,
                                  rolling_crypto_analysis_t *analysis);
bool crypto_entropy_analysis(uint8_t *data, uint32_t length, 
                              entropy_result_t *result);
void crypto_analyzer_cleanup(crypto_config_t *config);

#endif /* CRYPTO_ANALYZER_H */
