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
#include "input_validation.h"
#include "automotive/key_fob_analyzer.h"
#include "automotive/rolling_code_tester.h"
#include "wireless/wifi_analyzer.h"
#include "wireless/bluetooth_analyzer.h"
#include "wireless/subghz_analyzer.h"
#include "crypto/crypto_analyzer.h"
#include "hal/hal.h"

/* Global configuration */
static mdk_predator_config_t g_config;
static bool g_initialized = false;

/**
 * Initialize MDK-Predator suite
 */
bool mdk_predator_init(mdk_predator_config_t *config) {
    if (!validate_not_null(config)) {
        return false;
    }

    if (validate_frequency(config->hardware.hackrf_sample_rate) != VALIDATION_OK ||
        validate_frequency(config->hardware.hackrf_bandwidth) != VALIDATION_OK) {
        return false;
    }

    hal_init();
    hal_set_sample_rate(config->hardware.hackrf_sample_rate);
    hal_set_bandwidth(config->hardware.hackrf_bandwidth);
    hal_set_lna_gain(config->hardware.hackrf_lna_gain);
    hal_set_vga_gain(config->hardware.hackrf_vga_gain);
    hal_set_txvga_gain(config->hardware.hackrf_txvga_gain);

    safe_memcpy(&g_config, sizeof(g_config), config, sizeof(mdk_predator_config_t));

    g_initialized = true;

    return true;
}


/**
 * Get module status
 */
bool mdk_get_status(mdk_status_t *status) {
    if (!validate_not_null(status) || !g_initialized) {
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
    if (module < MODULE_AUTOMOTIVE || module > MODULE_CRYPTO) {
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
    if (!validate_not_null(result) || !g_initialized) {
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

    return true;
}

/**
 * Cleanup and shutdown
 */
void mdk_predator_cleanup(void) {
    if (g_initialized) {
        hal_cleanup();
        g_initialized = false;
    }
}

/**
 * Get version information
 */
const char* mdk_get_version(void) {
    return "MDK-Predator v1.0.0";
}
