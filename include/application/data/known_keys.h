#ifndef KNOWN_KEYS_H
#define KNOWN_KEYS_H

#include <stdint.h>

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

/* Known weak DES keys (parity adjusted) */
static const uint8_t weak_des_key_1[] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
static const uint8_t weak_des_key_2[] = {0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE};
static const uint8_t weak_des_key_3[] = {0xE0, 0xE0, 0xE0, 0xE0, 0xF1, 0xF1, 0xF1, 0xF1};
static const uint8_t weak_des_key_4[] = {0x1F, 0x1F, 0x1F, 0x1F, 0x0E, 0x0E, 0x0E, 0x0E};

/* List of known weak keys */
static const weak_key_entry_t known_weak_keys[] = {
    {weak_des_key_1, 8, "DES Weak Key 1 (All zeros/ones)"},
    {weak_des_key_2, 8, "DES Weak Key 2 (All ones/zeros)"},
    {weak_des_key_3, 8, "DES Semi-weak Key 1"},
    {weak_des_key_4, 8, "DES Semi-weak Key 2"}
};

/* Common/Default KeeLoq Manufacturer Keys (Publicly known/Test keys) */
/* Note: These are example keys found in public documentation/datasheets */
static const manufacturer_key_entry_t known_manufacturer_keys[] = {
    {{0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF}, "Microchip Default / Test Key"},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, "Zero Key (Test)"},
    {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, "All Ones Key (Test)"}
};

#define KNOWN_WEAK_KEYS_COUNT (sizeof(known_weak_keys) / sizeof(weak_key_entry_t))
#define KNOWN_MANUFACTURER_KEYS_COUNT (sizeof(known_manufacturer_keys) / sizeof(manufacturer_key_entry_t))

#endif /* KNOWN_KEYS_H */
