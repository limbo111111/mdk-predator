#include "hal.h"
#include <string.h>
#include "input_validation.h"

static mdk_accel_config_t g_accel_config;
static bool g_mdk_initialized = false;
static bool g_mdk_available = false;

void hal_init(void) {
    g_mdk_available = true;
}

void hal_cleanup(void) {
    g_mdk_initialized = false;
    memset(&g_accel_config, 0, sizeof(mdk_accel_config_t));
}

void hal_set_frequency(uint32_t frequency) {
    (void)frequency;
}

void hal_set_sample_rate(uint32_t sample_rate) {
    (void)sample_rate;
}

void hal_set_bandwidth(uint32_t bandwidth) {
    (void)bandwidth;
}

void hal_set_lna_gain(uint8_t gain) {
    (void)gain;
}

void hal_set_vga_gain(uint8_t gain) {
    (void)gain;
}

void hal_set_txvga_gain(uint8_t gain) {
    (void)gain;
}

int hal_rx_callback(void* data, uint32_t length) {
    (void)data;
    (void)length;
    return 0;
}

int hal_tx_callback(void* data, uint32_t length) {
    (void)data;
    (void)length;
    return 0;
}
