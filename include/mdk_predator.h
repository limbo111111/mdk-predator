/**
 * MDK-Predator Main Header
 * 
 * Main integration for Mayhem-MDK Predator Security Suite
 */

#ifndef MDK_PREDATOR_H
#define MDK_PREDATOR_H

#include <stdint.h>
#include <stdbool.h>

/* Security modules */
typedef enum {
    MODULE_AUTOMOTIVE,
    MODULE_WIFI,
    MODULE_BLUETOOTH,
    MODULE_SUBGHZ,
    MODULE_CRYPTO
} security_module_t;

/* Main configuration */
typedef struct {
    security_module_t default_module;
    bool enable_logging;
    uint32_t buffer_size;
} mdk_predator_config_t;

/* System status */
typedef struct {
    bool is_initialized;
    bool hardware_ready;
    security_module_t active_module;
} mdk_status_t;

/* Diagnostic result */
typedef struct {
    bool hardware_ok;
    bool rf_frontend_ok;
    uint32_t firmware_version;
    bool automotive_ok;
    bool wifi_ok;
    bool bluetooth_ok;
    bool subghz_ok;
    bool crypto_ok;
} diagnostic_result_t;

/* Function prototypes */
bool mdk_predator_init(mdk_predator_config_t *config);
bool mdk_hardware_init(void);
bool mdk_get_status(mdk_status_t *status);
bool mdk_set_active_module(security_module_t module);
bool mdk_run_diagnostic(diagnostic_result_t *result);
void mdk_predator_cleanup(void);
void mdk_hardware_cleanup(void);
const char* mdk_get_version(void);

#endif /* MDK_PREDATOR_H */
