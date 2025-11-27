/**
 * WiFi Security Analysis Module Header
 */

#ifndef WIFI_ANALYZER_H
#define WIFI_ANALYZER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_SSID_LENGTH 32
#define MAX_NETWORKS 100

/* WiFi operation modes */
typedef enum {
    WIFI_MODE_MONITOR,
    WIFI_MODE_MANAGED,
    WIFI_MODE_ATTACK,
    WIFI_MODE_CAPTURE
} wifi_mode_t;

/* Security types */
typedef enum {
    WIFI_SECURITY_OPEN,
    WIFI_SECURITY_WEP,
    WIFI_SECURITY_WPA,
    WIFI_SECURITY_WPA2,
    WIFI_SECURITY_WPA3,
    WIFI_SECURITY_UNKNOWN
} wifi_security_t;

/* WiFi configuration */
typedef struct {
    uint8_t channel;
    wifi_mode_t mode;
    bool capture_beacons;
    bool capture_data;
} wifi_config_t;

/* WiFi network information */
typedef struct {
    char ssid[MAX_SSID_LENGTH];
    uint8_t bssid[6];
    wifi_security_t security_type;
    int8_t rssi;
    uint8_t channel;
} wifi_network_t;

/* Security analysis result */
typedef struct {
    wifi_security_t encryption_type;
    bool wps_enabled;
    bool pmkid_available;
    bool krack_vulnerable;
    int8_t signal_strength;
} wifi_security_analysis_t;

/* Handshake data */
typedef struct {
    uint8_t frames[4][256];
    uint32_t frame_lengths[4];
    uint32_t frame_count;
    bool is_complete;
} handshake_data_t;

/* Deauth detection */
typedef struct {
    uint32_t deauth_count;
    bool attack_detected;
    uint8_t attacker_mac[6];
} deauth_detection_t;

/* Function prototypes */
bool wifi_analyzer_init(wifi_config_t *config);
bool wifi_scan_networks(wifi_config_t *config, wifi_network_t *networks, uint32_t *count, uint32_t max_count);
bool wifi_analyze_security(wifi_network_t *network, wifi_security_analysis_t *analysis);
bool wifi_capture_handshake(wifi_config_t *config, uint8_t *bssid, handshake_data_t *handshake);
bool wifi_detect_deauth(wifi_config_t *config, deauth_detection_t *detection);
void wifi_analyzer_cleanup(wifi_config_t *config);

#endif /* WIFI_ANALYZER_H */
