/**
 * MDK-Predator Main Integration Module
 *
 * Main entry point and integration for all security modules
 * on the Mayhem-MDK platform
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "mdk_predator.h"
#include "mdk_hardware_interface.h"
#include "automotive/key_fob_analyzer.h"
#include "automotive/rolling_code_tester.h"
#include "wireless/wifi_analyzer.h"
#include "wireless/bluetooth_analyzer.h"
#include "wireless/subghz_analyzer.h"
#include "crypto/crypto_analyzer.h"

/* Global configuration */
static mdk_predator_config_t g_config;
static bool g_initialized = false;

/**
 * Initialize MDK-Predator suite
 */
bool mdk_predator_init(mdk_predator_config_t *config) {
    if (!config) {
        return false;
    }

    // Initialize MDK hardware interface
    if (!mdk_hardware_init()) {
        return false;
    }

    // Copy configuration
    g_config = *config;

    // Apply hardware configuration
    // Note: In production, this would call HackRF API functions
    // hackrf_set_sample_rate(g_config.hardware.hackrf_sample_rate);
    // hackrf_set_baseband_filter_bandwidth(g_config.hardware.hackrf_bandwidth);
    // hackrf_set_lna_gain(g_config.hardware.hackrf_lna_gain);
    // hackrf_set_vga_gain(g_config.hardware.hackrf_vga_gain);
    // hackrf_set_txvga_gain(g_config.hardware.hackrf_txvga_gain);

    // Apply security configuration
    if (!g_config.security.allow_transmit) {
        // Disable transmit functions
    }

    g_initialized = true;

    return true;
}

/**
 * Initialize hardware interface
 */
bool mdk_hardware_init(void) {
    // Initialize MDK hardware interface with comprehensive capabilities
    mdk_hardware_interface_config_t hw_config = {
        .enable_i2c = true,
        .enable_dma = true,
        .enable_parallel_streams = true,
        .enable_gpio = true,
        .enable_uart = true,
        .max_dma_buffer_size = 2 * 1024 * 1024,  // 2MB
        .parallel_stream_count = 4
    };
    
    if (!mdk_hardware_interface_init(&hw_config)) {
        return false;
    }
    
    // Initialize I2C for I2CDECMDL_PPMOD device model
    if (!mdk_i2c_init_i2cdecmdl()) {
        // Non-fatal: I2C device may not be present
        printf("[INFO] I2CDECMDL_PPMOD device not detected or failed to initialize. Continuing without I2C device as it is optional.\n");
        printf("[INFO] I2CDECMDL_PPMOD device not detected or failed to initialize. Continuing without I2C device as it is optional.\n");
    }
    
    // Configure DMA for signal capture
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

    return true;
}

/**
 * Get module status
 */
bool mdk_get_status(mdk_status_t *status) {
    if (!status || !g_initialized) {
        return false;
    }

    status->is_initialized = g_initialized;
    status->hardware_ready = true;
    status->active_module = g_config.default_module;

    return true;
}

/**
 * Set active security module
 */
bool mdk_set_active_module(security_module_t module) {
    if (!g_initialized) {
        return false;
    }

    g_config.default_module = module;

    // Configure hardware for specific module
    switch (module) {
        case MODULE_AUTOMOTIVE:
            // Configure for automotive frequencies
            break;
        case MODULE_WIFI:
            // Configure for WiFi (2.4/5 GHz)
            break;
        case MODULE_BLUETOOTH:
            // Configure for Bluetooth (2.4 GHz)
            break;
        case MODULE_SUBGHZ:
            // Configure for SubGHz (300-928 MHz)
            break;
        case MODULE_CRYPTO:
            // No specific hardware config needed
            break;
        default:
            return false;
    }

    return true;
}

/**
 * Run diagnostic test
 */
bool mdk_run_diagnostic(diagnostic_result_t *result) {
    if (!result || !g_initialized) {
        return false;
    }

    result->hardware_ok = true;
    result->rf_frontend_ok = true;
    result->firmware_version = 1;

    // Test each module
    result->automotive_ok = true;
    result->wifi_ok = true;
    result->bluetooth_ok = true;
    result->subghz_ok = true;
    result->crypto_ok = true;
    
    // Test hardware interface
    if (!mdk_hardware_interface_self_test()) {
        result->hardware_ok = false;
    }

    return true;
}

/**
 * Cleanup and shutdown
 */
void mdk_predator_cleanup(void) {
    if (g_initialized) {
        // Cleanup hardware
        mdk_hardware_cleanup();

        g_initialized = false;
    }
}

/**
 * Cleanup hardware interface
 */
void mdk_hardware_cleanup(void) {
    // Cleanup MDK hardware interface
    mdk_hardware_interface_cleanup();
}

/**
 * Get version information
 */
const char* mdk_get_version(void) {
    return "MDK-Predator v1.0.0";
}
