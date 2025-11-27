#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include <stdbool.h>
#include "mdk_predator.h"

#define MDK_MAX_PARALLEL_STREAMS 8

// Device Information Structure
typedef struct {
    uint32_t api_version;
    uint32_t module_version;
    char module_name[64];
    uint32_t features;
} mdk_device_info_t;

// Acceleration Configuration Structure
// Note: parallel_streams must be > 0 to avoid division by zero
typedef struct {
    uint32_t parallel_streams;  // Must be >= 1, will be capped at MDK_MAX_PARALLEL_STREAMS
} mdk_accel_config_t;

// Bruteforce Task Structure
typedef struct {
    uint32_t start_code;
    uint32_t end_code;
    uint32_t target_value;
    uint32_t (*hash_function)(uint32_t);
    void (*progress_callback)(uint32_t, uint32_t);
    uint32_t *result_code;
    bool *found;
} mdk_bruteforce_task_t;

void hal_init(void);
void hal_cleanup(void);
void hal_set_frequency(uint32_t frequency);
void hal_set_sample_rate(uint32_t sample_rate);
void hal_set_bandwidth(uint32_t bandwidth);
void hal_set_lna_gain(uint8_t gain);
void hal_set_vga_gain(uint8_t gain);
void hal_set_txvga_gain(uint8_t gain);
int hal_rx_callback(void* data, uint32_t length);
int hal_tx_callback(void* data, uint32_t length);

#endif // HAL_H
