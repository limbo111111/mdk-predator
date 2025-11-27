/**
 * Bluetooth Security Analysis Module Header
 */

#ifndef BLUETOOTH_ANALYZER_H
#define BLUETOOTH_ANALYZER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_BT_NAME_LENGTH 248
#define MAX_BT_DEVICES 100
#define MAX_BT_SERVICES 20

/* Bluetooth modes */
typedef enum {
    BT_MODE_SCAN,
    BT_MODE_MONITOR,
    BT_MODE_CONNECT,
    BT_MODE_MITM
} bt_mode_t;

/* Bluetooth scan types */
typedef enum {
    BT_SCAN_CLASSIC,
    BT_SCAN_BLE,
    BT_SCAN_BOTH
} bt_scan_type_t;

/* Pairing methods */
typedef enum {
    BT_PAIRING_LEGACY,
    BT_PAIRING_SSP,
    BT_PAIRING_LESC,
    BT_PAIRING_UNKNOWN
} bt_pairing_t;

/* Bluetooth configuration */
typedef struct {
    bt_mode_t mode;
    bt_scan_type_t scan_type;  // App compatibility
    bool scan_classic;
    bool scan_ble;
    uint32_t scan_duration;
} bluetooth_config_t;

/* Bluetooth device */
typedef struct {
    uint8_t address[6];
    char name[MAX_BT_NAME_LENGTH];
    uint32_t device_class;
    int8_t rssi;
    bool is_ble;
} bt_device_t;

/* Security analysis result */
typedef struct {
    bt_pairing_t pairing_method;
    bool is_discoverable;
    bool supports_secure_pairing;
    bool bluez_vulnerable;
} bt_security_analysis_t;

/* Bluetooth service */
typedef struct {
    uint16_t uuid;
    char name[256];
    uint8_t channel;
} bt_service_t;

/* BLE advertisement data */
typedef struct {
    uint8_t packets[100][31];
    uint32_t packet_count;
    uint64_t timestamps[100];
} ble_adv_data_t;

/* MITM test result */
typedef struct {
    bool is_vulnerable;
    bool attack_feasible;
    char description[128];
} mitm_test_result_t;

/* Function prototypes */
bool bluetooth_analyzer_init(bluetooth_config_t *config);
bool bluetooth_scan_devices(bluetooth_config_t *config, bt_device_t *devices, uint32_t *count, uint32_t max_count);
bool bluetooth_analyze_security(bt_device_t *device, bt_security_analysis_t *analysis);
bool bluetooth_enumerate_services(bt_device_t *device, bt_service_t *services, uint32_t *count, uint32_t max_count);
bool bluetooth_capture_ble_adv(bluetooth_config_t *config, ble_adv_data_t *adv_data);
bool bluetooth_test_mitm(bt_device_t *device, mitm_test_result_t *result);
void bluetooth_analyzer_cleanup(bluetooth_config_t *config);

#endif /* BLUETOOTH_ANALYZER_H */
