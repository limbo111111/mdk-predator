/*
 * MDK Predator I2C Protocol Definitions
 * Communication protocol between PortaPack H4M and ESP32-S3 MDK Module
 * 
 * I2C Address: 0x51 (7-bit)
 * Speed: 100-400 kHz
 */

#ifndef MDK_I2C_PROTOCOL_H
#define MDK_I2C_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// ==================== VERSION ====================

#define MDK_VERSION_MAJOR      1
#define MDK_VERSION_MINOR      0
#define MDK_VERSION_PATCH      0

// ==================== I2C ADDRESS ====================

#define MDK_I2C_ADDR           0x51

// ==================== COMMAND CODES ====================

// System Commands
#define MDK_CMD_PING                0x01  // Ping (check if alive)
#define MDK_CMD_GET_VERSION         0x02  // Get firmware version
#define MDK_CMD_GET_STATUS          0x03  // Get current status
#define MDK_CMD_RESET               0x04  // Reset module
#define MDK_CMD_GET_CAPABILITIES    0x05  // Get supported features

// KeeLoq Commands
#define MDK_CMD_KEELOQ_ENCRYPT      0x10  // Encrypt with KeeLoq
#define MDK_CMD_KEELOQ_DECRYPT      0x11  // Decrypt with KeeLoq
#define MDK_CMD_KEELOQ_BRUTEFORCE   0x12  // Bruteforce KeeLoq key
#define MDK_CMD_KEELOQ_GET_SEED     0x13  // Get manufacturer seed

// Fixed Code Commands
#define MDK_CMD_PT2260_DECODE       0x20  // Decode PT2260 signal
#define MDK_CMD_PT2260_ENCODE       0x21  // Encode PT2260 signal
#define MDK_CMD_PT2262_DECODE       0x22  // Decode PT2262 signal
#define MDK_CMD_PT2262_ENCODE       0x23  // Encode PT2262 signal
#define MDK_CMD_HT6P20_DECODE       0x24  // Decode HT6P20 signal
#define MDK_CMD_HT6P20_ENCODE       0x25  // Encode HT6P20 signal
#define MDK_CMD_EV1527_DECODE       0x26  // Decode EV1527 signal
#define MDK_CMD_EV1527_ENCODE       0x27  // Encode EV1527 signal

// Automotive Commands
#define MDK_CMD_AUTO_TESLA_DECODE   0x30  // Decode Tesla key
#define MDK_CMD_AUTO_BMW_DECODE     0x31  // Decode BMW key
#define MDK_CMD_AUTO_MERCEDES_DECODE 0x32 // Decode Mercedes key
#define MDK_CMD_AUTO_VW_DECODE      0x33  // Decode VW/Audi key
#define MDK_CMD_AUTO_TOYOTA_DECODE  0x34  // Decode Toyota key
#define MDK_CMD_AUTO_HONDA_DECODE   0x35  // Decode Honda key

// Garage Door Commands
#define MDK_CMD_GARAGE_CHAMBERLAIN_V1 0x40  // Chamberlain Security+ v1
#define MDK_CMD_GARAGE_CHAMBERLAIN_V2 0x41  // Chamberlain Security+ v2
#define MDK_CMD_GARAGE_GENIE         0x42  // Genie Intellicode
#define MDK_CMD_GARAGE_LINEAR        0x43  // Linear MegaCode
#define MDK_CMD_GARAGE_NICE          0x44  // Nice Flor-S

// RF Commands
#define MDK_CMD_RF_SET_FREQUENCY    0x50  // Set RF frequency
#define MDK_CMD_RF_GET_RSSI         0x51  // Get RSSI
#define MDK_CMD_RF_SCAN             0x52  // Scan frequency range
#define MDK_CMD_TRANSMIT            0x53  // Transmit code

// CAN Bus Commands
#define MDK_CMD_CAN_INIT            0x60  // Initialize CAN bus
#define MDK_CMD_CAN_SEND_FRAME      0x61  // Send CAN frame
#define MDK_CMD_CAN_RECEIVE_FRAME   0x62  // Receive CAN frame

// Result Commands
#define MDK_CMD_GET_RESULT          0x70  // Get operation result
#define MDK_CMD_GET_PROGRESS        0x71  // Get operation progress

// ==================== STATUS CODES ====================

#define MDK_STATUS_READY            0x00  // Ready for commands
#define MDK_STATUS_BUSY             0x01  // Operation in progress
#define MDK_STATUS_ERROR            0x02  // Error occurred
#define MDK_STATUS_NOT_FOUND        0x03  // Key/code not found
#define MDK_STATUS_SUCCESS          0x04  // Operation successful
#define MDK_STATUS_TIMEOUT          0x05  // Operation timed out
#define MDK_STATUS_INVALID_PARAM    0x06  // Invalid parameters

// ==================== ERROR CODES ====================

#define MDK_ERR_NONE                0x00  // No error
#define MDK_ERR_INVALID_CMD         0x01  // Invalid command
#define MDK_ERR_INVALID_PARAM       0x02  // Invalid parameters
#define MDK_ERR_BUSY                0x03  // Module busy
#define MDK_ERR_TIMEOUT             0x04  // Operation timeout
#define MDK_ERR_NOT_FOUND           0x05  // Key/code not found
#define MDK_ERR_HARDWARE            0x06  // Hardware error
#define MDK_ERR_UNKNOWN             0xFF  // Unknown error

// ==================== CAPABILITY FLAGS ====================

#define MDK_CAP_KEELOQ              (1 << 0)  // KeeLoq support
#define MDK_CAP_FIXED_CODE          (1 << 1)  // Fixed code support
#define MDK_CAP_AUTOMOTIVE          (1 << 2)  // Automotive protocols
#define MDK_CAP_GARAGE_DOOR         (1 << 3)  // Garage door protocols
#define MDK_CAP_RF_TRANSMIT         (1 << 4)  // RF transmitter
#define MDK_CAP_RF_RECEIVE          (1 << 5)  // RF receiver
#define MDK_CAP_CAN_BUS             (1 << 6)  // CAN bus support
#define MDK_CAP_DUAL_CORE           (1 << 7)  // Dual-core acceleration
#define MDK_CAP_HARDWARE_CRYPTO     (1 << 8)  // Hardware crypto

// ==================== DATA STRUCTURES ====================

// KeeLoq Decrypt Request
typedef struct {
    uint32_t encrypted;      // Encrypted value
    uint64_t key;           // 64-bit key
} __attribute__((packed)) mdk_keeloq_decrypt_req_t;

// KeeLoq Encrypt Request
typedef struct {
    uint32_t plaintext;     // Plaintext value
    uint64_t key;          // 64-bit key
} __attribute__((packed)) mdk_keeloq_encrypt_req_t;

// KeeLoq Bruteforce Request
typedef struct {
    uint32_t encrypted;     // Encrypted value to crack
    uint32_t serial;       // Serial number
    uint64_t seed;         // Manufacturer seed
    uint32_t keyspace_size; // Size of keyspace to search
} __attribute__((packed)) mdk_keeloq_bruteforce_req_t;

// PT2260 Decode Request
typedef struct {
    uint16_t pulse_count;   // Number of pulses
    uint16_t pulses[128];   // Pulse widths in microseconds
} __attribute__((packed)) mdk_pt2260_decode_req_t;

// PT2260 Encode Request
typedef struct {
    uint32_t code;         // Code to encode
    uint8_t bits;          // Number of bits (12, 24, etc.)
} __attribute__((packed)) mdk_pt2260_encode_req_t;

// Transmit Request
typedef struct {
    uint64_t code;         // Code to transmit
    uint32_t frequency;    // Frequency in Hz
    uint16_t repeat_count; // Number of repetitions
} __attribute__((packed)) mdk_transmit_req_t;

// Progress Response
typedef struct {
    uint32_t current;      // Current iteration
    uint32_t total;        // Total iterations
    uint8_t percentage;    // Progress percentage (0-100)
} __attribute__((packed)) mdk_progress_rsp_t;

// Result Response (generic)
typedef struct {
    uint8_t status;        // Status code
    uint64_t result;       // Result value (key, code, etc.)
    uint32_t metadata;     // Additional metadata
} __attribute__((packed)) mdk_result_rsp_t;

// Version Response
typedef struct {
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
    uint32_t capabilities;  // Capability flags
} __attribute__((packed)) mdk_version_rsp_t;

// RSSI Response
typedef struct {
    int8_t rssi_dbm;       // RSSI in dBm
    uint32_t frequency;    // Current frequency
} __attribute__((packed)) mdk_rssi_rsp_t;

// CAN Frame
typedef struct {
    uint32_t can_id;       // CAN identifier
    uint8_t dlc;          // Data length code (0-8)
    uint8_t data[8];      // Data bytes
} __attribute__((packed)) mdk_can_frame_t;

// ==================== PROTOCOL HELPERS ====================

// Calculate expected response size for command
static inline size_t mdk_get_response_size(uint8_t cmd) {
    switch (cmd) {
        case MDK_CMD_PING:
        case MDK_CMD_GET_STATUS:
            return 1;  // Status byte only
        
        case MDK_CMD_GET_VERSION:
            return sizeof(mdk_version_rsp_t);
        
        case MDK_CMD_KEELOQ_DECRYPT:
        case MDK_CMD_KEELOQ_ENCRYPT:
        case MDK_CMD_PT2260_DECODE:
        case MDK_CMD_PT2260_ENCODE:
        case MDK_CMD_GET_RESULT:
            return sizeof(mdk_result_rsp_t);
        
        case MDK_CMD_GET_PROGRESS:
            return sizeof(mdk_progress_rsp_t);
        
        case MDK_CMD_RF_GET_RSSI:
            return sizeof(mdk_rssi_rsp_t);
        
        case MDK_CMD_CAN_RECEIVE_FRAME:
            return sizeof(mdk_can_frame_t);
        
        default:
            return 1;  // Default: status byte
    }
}

// Check if command requires parameters
static inline bool mdk_cmd_requires_params(uint8_t cmd) {
    switch (cmd) {
        case MDK_CMD_PING:
        case MDK_CMD_GET_VERSION:
        case MDK_CMD_GET_STATUS:
        case MDK_CMD_RESET:
        case MDK_CMD_GET_CAPABILITIES:
        case MDK_CMD_GET_RESULT:
        case MDK_CMD_GET_PROGRESS:
        case MDK_CMD_RF_GET_RSSI:
            return false;  // No parameters needed
        
        default:
            return true;   // Parameters required
    }
}

// Get command name (for debugging)
static inline const char* mdk_get_cmd_name(uint8_t cmd) {
    switch (cmd) {
        case MDK_CMD_PING: return "PING";
        case MDK_CMD_GET_VERSION: return "GET_VERSION";
        case MDK_CMD_GET_STATUS: return "GET_STATUS";
        case MDK_CMD_RESET: return "RESET";
        case MDK_CMD_KEELOQ_ENCRYPT: return "KEELOQ_ENCRYPT";
        case MDK_CMD_KEELOQ_DECRYPT: return "KEELOQ_DECRYPT";
        case MDK_CMD_KEELOQ_BRUTEFORCE: return "KEELOQ_BRUTEFORCE";
        case MDK_CMD_PT2260_DECODE: return "PT2260_DECODE";
        case MDK_CMD_PT2260_ENCODE: return "PT2260_ENCODE";
        case MDK_CMD_TRANSMIT: return "TRANSMIT";
        case MDK_CMD_GET_RESULT: return "GET_RESULT";
        case MDK_CMD_GET_PROGRESS: return "GET_PROGRESS";
        default: return "UNKNOWN";
    }
}

// Get status name (for debugging)
static inline const char* mdk_get_status_name(uint8_t status) {
    switch (status) {
        case MDK_STATUS_READY: return "READY";
        case MDK_STATUS_BUSY: return "BUSY";
        case MDK_STATUS_ERROR: return "ERROR";
        case MDK_STATUS_NOT_FOUND: return "NOT_FOUND";
        case MDK_STATUS_SUCCESS: return "SUCCESS";
        case MDK_STATUS_TIMEOUT: return "TIMEOUT";
        case MDK_STATUS_INVALID_PARAM: return "INVALID_PARAM";
        default: return "UNKNOWN";
    }
}

#endif // MDK_I2C_PROTOCOL_H
