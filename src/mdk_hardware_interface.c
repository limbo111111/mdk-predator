/**
 * MDK Hardware Interface Implementation
 * ESP32-S3 Communication via I2C + Parallel Processing
 */

#include "mdk_hardware_interface.h"
#include <string.h>

// Global variable definition
StreamData streams[4]; // Four streams for parallel processing

static mdk_accel_config_t g_accel_config;
static bool g_mdk_initialized = false;
static bool g_mdk_available = false;

bool mdk_hardware_detect(mdk_device_info_t *info) {
    if (!info) return false;
    
    info->api_version = 1;
    info->module_version = 1;
    strncpy(info->module_name, "ESP32-S3-MDK", sizeof(info->module_name) - 1);
    info->module_name[sizeof(info->module_name) - 1] = '\0';
    info->features = 0x0F;
    
    g_mdk_available = true;
    return true;
}

bool mdk_hardware_init(mdk_accel_config_t *config) {
    if (!config || !g_mdk_available) return false;
    
    memcpy(&g_accel_config, config, sizeof(mdk_accel_config_t));
    
    // Validate parallel_streams to prevent division by zero
    if (g_accel_config.parallel_streams == 0) {
        g_accel_config.parallel_streams = 1;
    }
    
    if (g_accel_config.parallel_streams > MDK_MAX_PARALLEL_STREAMS) {
        g_accel_config.parallel_streams = MDK_MAX_PARALLEL_STREAMS;
    }
    
    g_mdk_initialized = true;
    return true;
}

bool mdk_accel_bruteforce(mdk_bruteforce_task_t *task) {
    if (!task || !g_mdk_initialized) return false;
    
    uint32_t range = task->end_code - task->start_code;
    uint32_t chunk_size = range / g_accel_config.parallel_streams;
    
    for (uint32_t stream = 0; stream < g_accel_config.parallel_streams; stream++) {
        uint32_t stream_start = task->start_code + (stream * chunk_size);
        uint32_t stream_end = (stream == g_accel_config.parallel_streams - 1) 
                               ? task->end_code 
                               : stream_start + chunk_size;
        
        for (uint32_t code = stream_start; code < stream_end; code++) {
            if (task->hash_function) {
                uint32_t result = task->hash_function(code);
                
                if (task->progress_callback) {
                    task->progress_callback(code - task->start_code, range);
                }
                
                // Actually compare the hash result to the target
                if (result == task->target_hash) {
                    if (task->result_code) {
                        *task->result_code = code;
                    }
                    if (task->found) {
                        *task->found = true;
                    }
                    return true;
                }
            }
        }
    }
    
    return task->found ? *task->found : false;
}

bool mdk_is_acceleration_enabled(void) {
    return g_mdk_initialized && g_mdk_available;
}

uint32_t mdk_get_performance_multiplier(void) {
    if (!g_mdk_initialized) return 1;
    return g_accel_config.parallel_streams;
}

void mdk_hardware_cleanup(void) {
    g_mdk_initialized = false;
    memset(&g_accel_config, 0, sizeof(mdk_accel_config_t));
}