#ifndef KNOWN_KEYS_H
#define KNOWN_KEYS_H

#include <stdint.h>
#include <stddef.h>

/* Structure to hold weak key information */
typedef struct {
    const uint8_t *key;
    uint32_t length;
    const char *description;
} weak_key_entry_t;

/* Structure to hold manufacturer key information */
typedef struct {
    const uint8_t key[8];
    const char *manufacturer;
} manufacturer_key_entry_t;

/* Structure to hold transponder key information */
typedef struct {
    const uint8_t data[10]; /* Up to 80-bit */
    uint32_t bit_length;
    const char *chip_type;
    const char *description;
} transponder_key_entry_t;

/* Known weak DES keys (parity adjusted) */
extern const uint8_t weak_des_key_1[];
extern const uint8_t weak_des_key_2[];
extern const uint8_t weak_des_key_3[];
extern const uint8_t weak_des_key_4[];

/* List of known weak keys */
extern const weak_key_entry_t known_weak_keys[];
extern const size_t known_weak_keys_count;

/* Common/Default KeeLoq Manufacturer Keys (Publicly known/Test keys) */
extern const manufacturer_key_entry_t known_manufacturer_keys[];
extern const size_t known_manufacturer_keys_count;

/* Known Ford Transponder Keys (Test/Example) */
extern const transponder_key_entry_t known_ford_transponder_keys[];
extern const size_t known_ford_transponder_keys_count;

#define KNOWN_WEAK_KEYS_COUNT known_weak_keys_count
#define KNOWN_MANUFACTURER_KEYS_COUNT known_manufacturer_keys_count
#define KNOWN_FORD_TRANSPONDER_KEYS_COUNT known_ford_transponder_keys_count

#endif /* KNOWN_KEYS_H */
