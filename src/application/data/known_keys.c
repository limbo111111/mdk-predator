#include "application/data/known_keys.h"

/* Known weak DES keys (parity adjusted) */
const uint8_t weak_des_key_1[] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
const uint8_t weak_des_key_2[] = {0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE};
const uint8_t weak_des_key_3[] = {0xE0, 0xE0, 0xE0, 0xE0, 0xF1, 0xF1, 0xF1, 0xF1};
const uint8_t weak_des_key_4[] = {0x1F, 0x1F, 0x1F, 0x1F, 0x0E, 0x0E, 0x0E, 0x0E};

/* List of known weak keys */
const weak_key_entry_t known_weak_keys[] = {
    {weak_des_key_1, 8, "DES Weak Key 1 (All zeros/ones)"},
    {weak_des_key_2, 8, "DES Weak Key 2 (All ones/zeros)"},
    {weak_des_key_3, 8, "DES Semi-weak Key 1"},
    {weak_des_key_4, 8, "DES Semi-weak Key 2"}
};
const size_t known_weak_keys_count = sizeof(known_weak_keys) / sizeof(weak_key_entry_t);

/* Common/Default KeeLoq Manufacturer Keys (Publicly known/Test keys) */
/* Note: These are example keys found in public documentation/datasheets */
const manufacturer_key_entry_t known_manufacturer_keys[] = {
    {{0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF}, "Microchip Default / Test Key"},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, "Zero Key (Test)"},
    {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, "All Ones Key (Test)"}
};
const size_t known_manufacturer_keys_count = sizeof(known_manufacturer_keys) / sizeof(manufacturer_key_entry_t);

/* Known Ford Transponder Keys (Test/Example) */
/* Ford uses Texas Instruments 4D63 (40-bit and 80-bit) and 4C chips */
const transponder_key_entry_t known_ford_transponder_keys[] = {
    /* 40-bit 4D63 (common in 2000-2010 Fords) */
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89}, 40, "4D63 (40-bit)", "Ford Test Key 1"},
    /* 80-bit 4D63 (common in 2011+ Fords) */
    {{0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x00, 0x00}, 80, "4D63 (80-bit)", "Ford Test Key 2"},
    /* 4C Chip (older Fords) */
    {{0x00, 0x00, 0x00, 0x00, 0x7E, 0x23, 0x45, 0x67, 0x89, 0xAB}, 32, "4C", "Ford Legacy Test Key"}
};
const size_t known_ford_transponder_keys_count = sizeof(known_ford_transponder_keys) / sizeof(transponder_key_entry_t);
