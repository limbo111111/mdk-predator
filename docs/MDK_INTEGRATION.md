# MDK-Predator Integration Guide

This document provides comprehensive information about integrating and configuring the MDK-Predator Security Suite for optimal hardware performance.

## Table of Contents

- [Overview](#overview)
- [Configuration File](#configuration-file)
- [Hardware Acceleration](#hardware-acceleration)
- [Configuration Sections](#configuration-sections)
- [API Integration](#api-integration)

## Overview

MDK-Predator is a security research platform designed for the Mayhem-MDK hardware platform. It provides modules for automotive security testing, wireless protocol analysis, and cryptographic research.

## Configuration File

The main configuration file is `mdk_predator.conf`, which uses an INI-style format with sections and key-value pairs.

### File Location

- Default: `./mdk_predator.conf`
- Custom location can be specified via command-line argument

### File Format

```ini
[Section]
key=value
```

## Hardware Acceleration

The MDK_Hardware section provides configuration for hardware acceleration features that optimize performance for security research operations.

### [MDK_Hardware] Section

This section controls hardware acceleration features:

#### parallel_streams

- **Type:** Integer
- **Default:** 4
- **Range:** 1-16
- **Description:** Number of parallel processing streams for data processing. Increasing this value can improve throughput on multi-core systems but will consume more memory.
- **Example:**
  ```ini
  parallel_streams=4
  ```

#### use_dma

- **Type:** Boolean
- **Default:** true
- **Values:** true, false
- **Description:** Enable Direct Memory Access (DMA) for faster data transfer between hardware and software. DMA reduces CPU overhead and improves performance for high-speed signal processing. Disable only if experiencing compatibility issues.
- **Example:**
  ```ini
  use_dma=true
  ```

#### max_codes_per_second

- **Type:** Integer
- **Default:** 1000000
- **Range:** 1000-10000000
- **Description:** Maximum codes per second for bruteforce operations. This rate-limits cryptographic testing to prevent hardware overload and maintain system stability. Higher values may be possible on more powerful hardware.
- **Example:**
  ```ini
  max_codes_per_second=1000000
  ```

## Configuration Sections

### [General]

General system configuration:

- **default_module:** Default security module to load on startup (automotive, wifi, bluetooth, subghz, crypto)
- **enable_logging:** Enable debug logging (true/false)
- **buffer_size:** Buffer size for signal capture in bytes (default: 1048576)

### [Hardware]

HackRF One hardware configuration:

- **sample_rate:** Sample rate in Hz (default: 2000000)
- **baseband_filter_bandwidth:** Baseband filter bandwidth in Hz (default: 1750000)
- **enable_amp:** Enable RF amplifier (default: false)
- **enable_antenna_power:** Enable antenna power (default: false)

### [Automotive]

Automotive security module settings:

- **default_frequency:** Default frequency for key fob analysis in Hz (default: 433920000)
- **frequencies:** Comma-separated list of supported frequencies
- **rolling_code_timeout:** Rolling code detection timeout in milliseconds (default: 5000)

### [Wireless]

Wireless protocol analysis settings:

- **wifi_channels:** WiFi channels to scan (comma-separated)
- **bluetooth_scan_duration:** Bluetooth scan duration in seconds
- **subghz_freq_min:** SubGHz minimum frequency in Hz
- **subghz_freq_max:** SubGHz maximum frequency in Hz

### [Crypto]

Cryptographic analysis settings:

- **enable_crypto_analysis:** Enable cryptographic analysis (true/false)
- **max_key_length:** Maximum key length to analyze in bits (default: 256)

### [Security]

Security and safety settings:

- **allow_transmit:** Allow transmit mode - WARNING: Only enable for authorized security research
- **require_transmit_confirmation:** Require confirmation before transmit

### [Logging]

Logging configuration:

- **log_path:** Path to log file (default: /tmp/mdk_predator.log)
- **log_level:** Log level (0=ERROR, 1=WARNING, 2=INFO, 3=DEBUG)

## API Integration

### Initializing with Configuration

```c
#include "mdk_predator.h"

// Create configuration structure
mdk_predator_config_t config;

// Set general configuration
config.default_module = MODULE_AUTOMOTIVE;
config.enable_logging = true;
config.buffer_size = 1048576;

// Set hardware configuration
config.hardware.hackrf_sample_rate = 2000000;
config.hardware.hackrf_bandwidth = 1750000;
config.hardware.hackrf_lna_gain = 8;
config.hardware.hackrf_vga_gain = 20;
config.hardware.hackrf_txvga_gain = 0;

// Set hardware acceleration configuration
config.hardware.parallel_streams = 4;
config.hardware.use_dma = true;
config.hardware.max_codes_per_second = 1000000;

// Set security configuration
config.security.allow_transmit = false;
config.security.require_confirmation = true;
config.security.log_all_activity = true;

// Initialize MDK-Predator
if (!mdk_predator_init(&config)) {
    fprintf(stderr, "Failed to initialize MDK-Predator\n");
    return 1;
}
```

### Hardware Acceleration Impact

The hardware acceleration settings affect the following operations:

1. **parallel_streams:**
   - Signal processing pipelines
   - Multi-frequency scanning
   - Concurrent module operations

2. **use_dma:**
   - HackRF data transfer
   - Memory buffer management
   - Signal capture performance

3. **max_codes_per_second:**
   - Rolling code analysis
   - Key enumeration testing
   - Cryptographic operations

### Best Practices

1. **Development Environment:**
   ```ini
   parallel_streams=2
   use_dma=false
   max_codes_per_second=100000
   ```

2. **Production Environment:**
   ```ini
   parallel_streams=4
   use_dma=true
   max_codes_per_second=1000000
   ```

3. **High-Performance Environment:**
   ```ini
   parallel_streams=8
   use_dma=true
   max_codes_per_second=5000000
   ```

## Troubleshooting

### Performance Issues

If experiencing performance problems:

1. Reduce `parallel_streams` to lower memory usage
2. Verify `use_dma` is enabled for best performance
3. Adjust `max_codes_per_second` based on hardware capabilities

### Stability Issues

If experiencing crashes or instability:

1. Disable DMA (`use_dma=false`) to check for compatibility issues
2. Reduce `parallel_streams` to 1 for debugging
3. Lower `max_codes_per_second` to reduce load

### Validation

To verify configuration is loaded correctly:

```c
mdk_status_t status;
if (mdk_get_status(&status)) {
    printf("Hardware ready: %s\n", status.hardware_ready ? "Yes" : "No");
    printf("Initialized: %s\n", status.is_initialized ? "Yes" : "No");
}
```

## Support

For additional assistance with MDK-Predator integration:

- Review the [Architecture Documentation](ARCHITECTURE.md)
- Consult the [User Guide](USER_GUIDE.md)
- Check the [API Documentation](API.md)
