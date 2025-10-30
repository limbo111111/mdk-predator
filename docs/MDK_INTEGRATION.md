# MDK Hardware Interface Integration Guide

## Overview

The MDK Hardware Interface provides a comprehensive API for hardware-accelerated security research on the Mayhem-MDK platform. This document describes integration with existing MDK device models and Predator modules.

## Features

- **I2C Communication**: ESP32-S3 compatible I2C interface for device communication
- **DMA Transfers**: High-speed data transfers with Direct Memory Access
- **Parallel Streams**: Hardware-accelerated parallel processing (4x+ performance)
- **GPIO Control**: Flexible GPIO configuration and interrupt handling
- **UART Interface**: Serial communication with DMA support

## Architecture

```
┌─────────────────────────────────────────────────────┐
│        MDK Predator Security Suite                  │
│  (Automotive, Wireless, Crypto Modules)             │
└──────────────────┬──────────────────────────────────┘
                   │
┌──────────────────▼──────────────────────────────────┐
│       MDK Hardware Interface Layer                  │
│  ┌─────────┬─────────┬─────────┬────────┬────────┐ │
│  │   I2C   │   DMA   │ Streams │  GPIO  │  UART  │ │
│  └─────────┴─────────┴─────────┴────────┴────────┘ │
└──────────────────┬──────────────────────────────────┘
                   │
┌──────────────────▼──────────────────────────────────┐
│          HackRF One + Mayhem-MDK                    │
│       (I2CDECMDL_PPMOD Integration)                 │
└─────────────────────────────────────────────────────┘
```

## I2C Integration

### I2CDECMDL_PPMOD Device Model

The I2CDECMDL_PPMOD is a Mayhem-MDK device model that provides I2C decoder functionality. The hardware interface provides native support for this device.

#### Basic I2C Setup

```c
#include "mdk_hardware_interface.h"

// Initialize I2C for I2CDECMDL_PPMOD
bool init_i2c_decoder(void) {
    // Initialize I2CDECMDL device
    if (!mdk_i2c_init_i2cdecmdl()) {
        return false;
    }
    
    // Read device status
    uint8_t status;
    if (!mdk_i2c_i2cdecmdl_read_status(&status)) {
        return false;
    }
    
    return true;
}
```

#### Custom I2C Device

```c
// Configure custom I2C bus
mdk_i2c_config_t config = {
    .bus = MDK_I2C_BUS_0,
    .speed = MDK_I2C_SPEED_FAST,  // 400 kHz
    .sda_pin = 21,
    .scl_pin = 22,
    .pullup_enable = true,
    .timeout_ms = 1000
};

if (!mdk_i2c_init(&config)) {
    return false;
}

// Open device at address 0x50
mdk_i2c_device_t device;
if (!mdk_i2c_device_open(&device, MDK_I2C_BUS_0, 0x50)) {
    return false;
}

// Write to register
mdk_i2c_write_reg(&device, 0x01, 0xAB);

// Read from register
uint8_t value;
mdk_i2c_read_reg(&device, 0x01, &value);

mdk_i2c_device_close(&device);
```

### ESP32-S3 Compatibility

The I2C implementation is fully compatible with ESP32-S3 I2C peripherals:
- Standard mode (100 kHz)
- Fast mode (400 kHz)
- Fast mode plus (1 MHz)
- Clock stretching support
- Multi-master capability

## DMA Integration

### Signal Capture with DMA

High-speed signal capture using DMA eliminates CPU overhead and enables real-time processing.

```c
#include "mdk_hardware_interface.h"

// Initialize DMA for signal capture
mdk_dma_config_t dma_config = {
    .channel = MDK_DMA_CHANNEL_0,
    .direction = MDK_DMA_PERIPH_TO_MEM,
    .priority = MDK_DMA_PRIORITY_VERY_HIGH,
    .circular_mode = true,
    .interrupt_enable = true,
    .callback = signal_capture_callback,
    .user_data = NULL
};

if (!mdk_dma_init(&dma_config)) {
    return false;
}

// Capture 1M samples at 2 MS/s for 500ms
uint8_t *buffer = malloc(1024 * 1024);
if (!mdk_dma_capture_signal(MDK_DMA_CHANNEL_0, buffer, 
                            1024 * 1024, 2000000, 500)) {
    free(buffer);
    return false;
}

// Process captured data
size_t samples;
mdk_dma_get_capture_status(MDK_DMA_CHANNEL_0, &samples);
```

### Fast Data Transfers

```c
// Asynchronous DMA transfer
uint8_t src_data[4096];
uint8_t dst_data[4096];

mdk_dma_transfer_t transfer = {
    .src_addr = src_data,
    .dst_addr = dst_data,
    .length = 4096,
    .completed = false,
    .error = false
};

if (!mdk_dma_transfer_async(MDK_DMA_CHANNEL_1, &transfer)) {
    return false;
}

// Wait for completion
mdk_dma_wait_complete(MDK_DMA_CHANNEL_1, 1000);
```

## Parallel Stream Processing

### Hardware Acceleration (4x+ Performance)

Parallel streams enable simultaneous processing of multiple data streams for significant performance gains.

```c
#include "mdk_hardware_interface.h"

// Configure parallel streams for automotive bruteforce
void setup_parallel_bruteforce(void) {
    // Allocate buffers for each stream
    void *buffers[4];
    for (int i = 0; i < 4; i++) {
        buffers[i] = malloc(512 * 1024);
    }
    
    // Configure parallel stream group
    mdk_parallel_stream_config_t config = {
        .num_streams = 4,
        .synchronized = true,
        .base_frequency = 433920000,  // 433.92 MHz
        .streams = {
            {
                .stream_id = MDK_STREAM_0,
                .mode = MDK_STREAM_MODE_PROCESS,
                .dma_channel = MDK_DMA_CHANNEL_0,
                .buffer = buffers[0],
                .buffer_size = 512 * 1024,
                .callback = process_stream_0,
                .user_data = NULL,
                .enable_acceleration = true
            },
            {
                .stream_id = MDK_STREAM_1,
                .mode = MDK_STREAM_MODE_PROCESS,
                .dma_channel = MDK_DMA_CHANNEL_1,
                .buffer = buffers[1],
                .buffer_size = 512 * 1024,
                .callback = process_stream_1,
                .user_data = NULL,
                .enable_acceleration = true
            },
            {
                .stream_id = MDK_STREAM_2,
                .mode = MDK_STREAM_MODE_PROCESS,
                .dma_channel = MDK_DMA_CHANNEL_2,
                .buffer = buffers[2],
                .buffer_size = 512 * 1024,
                .callback = process_stream_2,
                .user_data = NULL,
                .enable_acceleration = true
            },
            {
                .stream_id = MDK_STREAM_3,
                .mode = MDK_STREAM_MODE_PROCESS,
                .dma_channel = MDK_DMA_CHANNEL_3,
                .buffer = buffers[3],
                .buffer_size = 512 * 1024,
                .callback = process_stream_3,
                .user_data = NULL,
                .enable_acceleration = true
            }
        }
    };
    
    if (!mdk_parallel_streams_init(&config)) {
        return;
    }
    
    // Start all streams
    mdk_parallel_streams_start_all();
    
    // Get acceleration factor
    float accel = mdk_parallel_streams_get_acceleration();
    printf("Acceleration: %.1fx\n", accel);  // Should be ~4.0x
}
```

### Parallel Bruteforce Example

```c
// Test function for rolling code
bool test_rolling_code(const void *key, size_t key_len, void *user_data) {
    // Test if this key decrypts the rolling code
    uint64_t test_key = *(uint64_t *)key;
    uint32_t encrypted = *(uint32_t *)user_data;
    
    // KeeLoq decryption logic here
    uint32_t decrypted = keeloq_decrypt(encrypted, test_key);
    
    return is_valid_code(decrypted);
}

// Bruteforce with parallel acceleration
bool automotive_parallel_bruteforce(uint32_t encrypted_code) {
    // Generate key space (example: 24-bit keys)
    size_t num_keys = 1 << 24;  // 16M keys
    uint64_t *keys = malloc(num_keys * sizeof(uint64_t));
    for (size_t i = 0; i < num_keys; i++) {
        keys[i] = i;
    }
    
    // Run parallel bruteforce
    uint64_t result_key = 0;
    bool found = mdk_parallel_bruteforce(keys, sizeof(uint64_t), 
                                         num_keys, test_rolling_code,
                                         &encrypted_code, &result_key);
    
    free(keys);
    
    if (found) {
        printf("Key found: 0x%016llx\n", result_key);
        return true;
    }
    
    return false;
}
```

## Module Integration

### Automotive Module Integration

```c
#include "mdk_hardware_interface.h"
#include "automotive/key_fob_analyzer.h"
#include "automotive/rolling_code_tester.h"

bool automotive_init_with_hardware(void) {
    // Initialize hardware interface
    mdk_hardware_interface_config_t hw_config = {
        .enable_i2c = true,
        .enable_dma = true,
        .enable_parallel_streams = true,
        .enable_gpio = false,
        .enable_uart = false,
        .max_dma_buffer_size = 2 * 1024 * 1024,
        .parallel_stream_count = 4
    };
    
    if (!mdk_hardware_interface_init(&hw_config)) {
        return false;
    }
    
    // Setup DMA for signal capture
    mdk_dma_config_t dma_config = {
        .channel = MDK_DMA_CHANNEL_0,
        .direction = MDK_DMA_PERIPH_TO_MEM,
        .priority = MDK_DMA_PRIORITY_VERY_HIGH,
        .circular_mode = true,
        .interrupt_enable = true,
        .callback = NULL,
        .user_data = NULL
    };
    mdk_dma_init(&dma_config);
    
    // Initialize automotive modules
    keyfob_config_t keyfob_config = {
        .frequency = 433920000,
        .bandwidth = 200000,
        .sample_rate = 2000000,
        .mode = KEYFOB_MODE_RECEIVE
    };
    keyfob_analyzer_init(&keyfob_config);
    
    return true;
}
```

### Wireless Module Integration

```c
#include "mdk_hardware_interface.h"
#include "wireless/wifi_analyzer.h"
#include "wireless/bluetooth_analyzer.h"

bool wireless_init_with_hardware(void) {
    // Initialize parallel streams for multi-channel WiFi scanning
    setup_parallel_wifi_scanning();
    
    // Initialize Bluetooth with UART
    mdk_uart_config_t uart_config = {
        .port = MDK_UART_PORT_0,
        .baud_rate = MDK_UART_BAUD_115200,
        .data_bits = MDK_UART_DATA_8_BITS,
        .parity = MDK_UART_PARITY_NONE,
        .stop_bits = MDK_UART_STOP_1_BIT,
        .tx_pin = 17,
        .rx_pin = 16,
        .use_dma = true,
        .dma_channel = MDK_DMA_CHANNEL_2
    };
    mdk_uart_init(&uart_config);
    
    return true;
}
```

### Crypto Module Integration

```c
#include "mdk_hardware_interface.h"
#include "crypto/crypto_analyzer.h"

bool crypto_init_with_hardware(void) {
    // Use parallel streams for accelerated crypto analysis
    mdk_parallel_stream_config_t config;
    // ... configure streams for parallel crypto operations
    
    mdk_parallel_streams_init(&config);
    
    return true;
}
```

## GPIO and UART Examples

### GPIO Control

```c
// Configure GPIO for RF control
mdk_gpio_config_t gpio_config = {
    .pin = 25,
    .direction = MDK_GPIO_OUTPUT,
    .pull = MDK_GPIO_PULL_NONE,
    .interrupt = MDK_GPIO_INT_DISABLE,
    .callback = NULL,
    .user_data = NULL
};
mdk_gpio_init(&gpio_config);

// Control RF amplifier
mdk_gpio_set_level(25, true);   // Enable amp
mdk_gpio_set_level(25, false);  // Disable amp
```

### UART Communication

```c
// Configure UART for external device
mdk_uart_config_t uart_config = {
    .port = MDK_UART_PORT_1,
    .baud_rate = MDK_UART_BAUD_115200,
    .data_bits = MDK_UART_DATA_8_BITS,
    .parity = MDK_UART_PARITY_NONE,
    .stop_bits = MDK_UART_STOP_1_BIT,
    .tx_pin = 17,
    .rx_pin = 16,
    .use_dma = false,
    .dma_channel = 0
};
mdk_uart_init(&uart_config);

// Send data
const char *cmd = "AT+TEST\r\n";
mdk_uart_write(MDK_UART_PORT_1, (uint8_t *)cmd, strlen(cmd));

// Read response
uint8_t response[128];
size_t bytes_read;
mdk_uart_read(MDK_UART_PORT_1, response, sizeof(response), &bytes_read);
```

## Configuration

### mdk_predator.conf Settings

```ini
[HardwareInterface]
enable_i2c=true
i2c_speed=400000
i2c_sda_pin=21
i2c_scl_pin=22

enable_dma=true
max_dma_buffer_size=2097152

enable_parallel_streams=true
parallel_stream_count=4

enable_gpio=true
enable_uart=true

[ParallelAcceleration]
num_streams=4
enable_acceleration=true
synchronized_streams=true
base_frequency=433920000
stream_dma_channels=0,1,2,3
stream_buffer_size=524288
```

## Performance Optimization

### DMA Best Practices

1. Use circular mode for continuous capture
2. Enable interrupts for asynchronous operations
3. Align buffers to cache line boundaries
4. Use appropriate DMA priority levels

### Parallel Stream Optimization

1. Distribute workload evenly across streams
2. Use synchronized mode for time-critical operations
3. Monitor acceleration factor with `mdk_parallel_streams_get_acceleration()`
4. Adjust buffer sizes based on processing requirements

### Expected Performance

- **Single Stream**: Baseline performance
- **2 Parallel Streams**: ~1.9x acceleration
- **4 Parallel Streams**: ~3.8-4.2x acceleration
- **DMA Signal Capture**: Up to 20 MS/s with zero CPU overhead

## Error Handling

### Status Checking

```c
// Check subsystem status
mdk_hw_status_t status;
if (mdk_hw_get_status(MDK_HW_SUBSYS_DMA, 0, &status)) {
    if (status.error_count > 0) {
        printf("DMA errors: %u, Last: %s\n", 
               status.error_count, status.last_error);
    }
}

// Get system-wide status
mdk_hw_system_status_t sys_status;
if (mdk_hw_get_system_status(&sys_status)) {
    printf("Total errors: %u\n", sys_status.total_errors);
}
```

### Resource Cleanup

```c
// Always cleanup resources
void cleanup_hardware(void) {
    // Stop parallel streams
    mdk_parallel_streams_stop_all();
    mdk_parallel_streams_deinit();
    
    // Cleanup DMA channels
    for (int i = 0; i < 4; i++) {
        mdk_dma_deinit(i);
    }
    
    // Cleanup I2C
    mdk_i2c_deinit(MDK_I2C_BUS_0);
    mdk_i2c_deinit(MDK_I2C_BUS_1);
    
    // Final cleanup
    mdk_hardware_interface_cleanup();
}
```

## Testing

Run the hardware interface self-test:

```c
if (!mdk_hardware_interface_self_test()) {
    fprintf(stderr, "Hardware self-test failed\n");
    return false;
}
```

## API Reference

See `include/mdk_hardware_interface.h` for complete API documentation.

## Troubleshooting

### I2C Issues

- Check SDA/SCL pin configuration
- Verify pullup resistors are enabled or present
- Ensure device address is correct (7-bit vs 8-bit)
- Check bus speed compatibility

### DMA Issues

- Verify buffer alignment
- Check DMA channel conflicts
- Ensure sufficient buffer size
- Monitor error status

### Parallel Stream Issues

- Check DMA channel assignments
- Verify buffer sizes are adequate
- Monitor acceleration factor
- Check for synchronization issues

## Version

MDK Hardware Interface v1.0.0

## License

MIT License - See LICENSE file for details
