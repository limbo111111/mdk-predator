# MDK-Predator API Reference

## Core API

### Initialization

#### `mdk_predator_init()`
```c
bool mdk_predator_init(mdk_predator_config_t *config);
```
Initialize the MDK-Predator system.

**Parameters:**
- `config`: Configuration structure

**Returns:** `true` on success, `false` on failure

#### `mdk_predator_cleanup()`
```c
void mdk_predator_cleanup(void);
```
Cleanup and release all resources.

---

## Hardware Interface API

See [MDK_INTEGRATION.md](MDK_INTEGRATION.md) for detailed integration guide.

### I2C Communication

#### `mdk_i2c_init()`
```c
bool mdk_i2c_init(const mdk_i2c_config_t *config);
```
Initialize I2C bus (ESP32-S3 compatible).

#### `mdk_i2c_device_open()`
```c
bool mdk_i2c_device_open(mdk_i2c_device_t *device, mdk_i2c_bus_t bus, uint8_t device_addr);
```
Open I2C device for communication.

#### `mdk_i2c_init_i2cdecmdl()`
```c
bool mdk_i2c_init_i2cdecmdl(void);
```
Initialize I2CDECMDL_PPMOD device model integration.

### DMA Transfers

#### `mdk_dma_init()`
```c
bool mdk_dma_init(const mdk_dma_config_t *config);
```
Initialize DMA channel for high-speed data transfers.

#### `mdk_dma_capture_signal()`
```c
bool mdk_dma_capture_signal(mdk_dma_channel_t channel, void *buffer, size_t buffer_size,
                            uint32_t sample_rate, uint32_t duration_ms);
```
Capture RF signal using DMA for zero CPU overhead.

### Parallel Streams

#### `mdk_parallel_streams_init()`
```c
bool mdk_parallel_streams_init(const mdk_parallel_stream_config_t *config);
```
Initialize parallel stream processing for 4x+ performance acceleration.

#### `mdk_parallel_bruteforce()`
```c
bool mdk_parallel_bruteforce(const void *key_space, size_t key_len,
                             size_t num_keys, mdk_bruteforce_test_fn_t test_fn,
                             void *user_data, void *result_key);
```
Hardware-accelerated parallel bruteforce for crypto and automotive modules.

### GPIO/UART

#### `mdk_gpio_init()`
```c
bool mdk_gpio_init(const mdk_gpio_config_t *config);
```
Configure GPIO pin with interrupt support.

#### `mdk_uart_init()`
```c
bool mdk_uart_init(const mdk_uart_config_t *config);
```
Initialize UART with optional DMA support.

---

## Automotive Security Module

### Key Fob Analyzer

#### `keyfob_analyzer_init()`
```c
bool keyfob_analyzer_init(keyfob_config_t *config);
```
Initialize the key fob analyzer.

**Parameters:**
- `config`: Key fob configuration structure

**Returns:** `true` on success, `false` on failure

#### `keyfob_capture_signal()`
```c
bool keyfob_capture_signal(keyfob_config_t *config, signal_data_t *signal);
```
Capture a key fob signal.

**Parameters:**
- `config`: Configuration
- `signal`: Output signal data

**Returns:** `true` if signal captured, `false` otherwise

#### `keyfob_analyze_signal()`
```c
bool keyfob_analyze_signal(signal_data_t *signal, keyfob_analysis_t *result);
```
Analyze a captured key fob signal.

**Parameters:**
- `signal`: Captured signal data
- `result`: Analysis result output

**Returns:** `true` on successful analysis, `false` otherwise

### Rolling Code Tester

#### `rolling_code_tester_init()`
```c
bool rolling_code_tester_init(rolling_code_config_t *config);
```
Initialize the rolling code tester.

#### `test_keeloq_code()`
```c
bool test_keeloq_code(uint32_t encrypted, uint64_t key, keeloq_result_t *result);
```
Test a KeeLoq rolling code.

**Parameters:**
- `encrypted`: Encrypted data
- `key`: Manufacturer key
- `result`: Decryption result

**Returns:** `true` on success

---

## Wireless Security Module

### WiFi Analyzer

#### `wifi_analyzer_init()`
```c
bool wifi_analyzer_init(wifi_config_t *config);
```
Initialize WiFi analyzer.

#### `wifi_scan_networks()`
```c
bool wifi_scan_networks(wifi_config_t *config, wifi_network_t *networks, uint32_t *count);
```
Scan for WiFi networks.

**Parameters:**
- `config`: WiFi configuration
- `networks`: Array to store found networks
- `count`: Number of networks found

**Returns:** `true` on successful scan

#### `wifi_analyze_security()`
```c
bool wifi_analyze_security(wifi_network_t *network, wifi_security_analysis_t *analysis);
```
Analyze network security.

### Bluetooth Analyzer

#### `bluetooth_analyzer_init()`
```c
bool bluetooth_analyzer_init(bluetooth_config_t *config);
```
Initialize Bluetooth analyzer.

#### `bluetooth_scan_devices()`
```c
bool bluetooth_scan_devices(bluetooth_config_t *config, bt_device_t *devices, uint32_t *count);
```
Scan for Bluetooth devices.

#### `bluetooth_analyze_security()`
```c
bool bluetooth_analyze_security(bt_device_t *device, bt_security_analysis_t *analysis);
```
Analyze device security.

### SubGHz Analyzer

#### `subghz_analyzer_init()`
```c
bool subghz_analyzer_init(subghz_config_t *config);
```
Initialize SubGHz analyzer.

#### `subghz_scan_spectrum()`
```c
bool subghz_scan_spectrum(subghz_config_t *config, spectrum_data_t *spectrum);
```
Scan SubGHz spectrum.

#### `subghz_capture_signal()`
```c
bool subghz_capture_signal(subghz_config_t *config, rf_signal_t *signal);
```
Capture RF signal.

#### `subghz_decode_protocol()`
```c
bool subghz_decode_protocol(rf_signal_t *signal, protocol_data_t *protocol);
```
Decode captured protocol.

---

## Cryptographic Analysis Module

#### `crypto_analyzer_init()`
```c
bool crypto_analyzer_init(crypto_config_t *config);
```
Initialize crypto analyzer.

#### `crypto_identify_algorithm()`
```c
bool crypto_identify_algorithm(uint8_t *ciphertext, uint32_t length,
                                crypto_identification_t *result);
```
Identify encryption algorithm.

#### `crypto_analyze_rolling_code()`
```c
bool crypto_analyze_rolling_code(uint8_t *encrypted_data, uint32_t length,
                                  rolling_crypto_analysis_t *analysis);
```
Analyze rolling code cryptography.

#### `crypto_entropy_analysis()`
```c
bool crypto_entropy_analysis(uint8_t *data, uint32_t length,
                              entropy_result_t *result);
```
Perform entropy analysis on data.

**Parameters:**
- `data`: Data to analyze
- `length`: Data length
- `result`: Entropy analysis result

**Returns:** `true` on success

---

## Data Structures

### Configuration Structures

```c
typedef struct {
    security_module_t default_module;
    bool enable_logging;
    uint32_t buffer_size;
} mdk_predator_config_t;

typedef struct {
    uint32_t frequency;
    uint32_t bandwidth;
    uint32_t sample_rate;
    keyfob_mode_t mode;
} keyfob_config_t;
```

### Result Structures

```c
typedef struct {
    uint8_t protocol_type;
    uint32_t code;
    bool is_rolling_code;
    uint8_t manufacturer_id;
} keyfob_analysis_t;

typedef struct {
    float entropy;
    float max_entropy;
    float randomness_quality;
} entropy_result_t;
```

## Usage Examples

### Example 1: Capture and Analyze Key Fob

```c
#include "mdk_predator.h"
#include "key_fob_analyzer.h"

int main(void) {
    mdk_predator_config_t config = {
        .default_module = MODULE_AUTOMOTIVE,
        .enable_logging = true,
        .buffer_size = 1024 * 1024
    };

    if (!mdk_predator_init(&config)) {
        return -1;
    }

    keyfob_config_t keyfob_config;
    keyfob_analyzer_init(&keyfob_config);

    signal_data_t signal;
    if (keyfob_capture_signal(&keyfob_config, &signal)) {
        keyfob_analysis_t result;
        keyfob_analyze_signal(&signal, &result);
        // Process result
    }

    mdk_predator_cleanup();
    return 0;
}
```

### Example 2: WiFi Network Scan

```c
#include "wifi_analyzer.h"

wifi_config_t wifi_config;
wifi_analyzer_init(&wifi_config);

wifi_network_t networks[MAX_NETWORKS];
uint32_t count;

if (wifi_scan_networks(&wifi_config, networks, &count)) {
    for (uint32_t i = 0; i < count; i++) {
        printf("SSID: %s, Security: %d\n",
               networks[i].ssid,
               networks[i].security_type);
    }
}
```

### Example 3: Entropy Analysis

```c
#include "crypto_analyzer.h"

uint8_t data[256];
entropy_result_t entropy;

if (crypto_entropy_analysis(data, sizeof(data), &entropy)) {
    printf("Entropy: %.2f / %.2f\n",
           entropy.entropy,
           entropy.max_entropy);
    printf("Quality: %.2f%%\n",
           entropy.randomness_quality * 100);
}
```
