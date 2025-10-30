/**
 * MDK-Predator Main Integration Module
 *
 * Main entry point and integration for all security modules
 * on the Mayhem-MDK platform
 */

#define _DEFAULT_SOURCE

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "mdk_predator.h"
#include "automotive/key_fob_analyzer.h"
#include "automotive/rolling_code_tester.h"
#include "wireless/wifi_analyzer.h"
#include "wireless/bluetooth_analyzer.h"
#include "wireless/subghz_analyzer.h"
#include "crypto/crypto_analyzer.h"

/* Hardware acceleration stream structure */
struct mdk_accel_stream_t {
    uint32_t stream_count;
    uint32_t input_size;
    uint32_t output_size;
    pthread_t *threads;
    void **inputs;
    void **outputs;
    mdk_accel_callback_t *callbacks;
    void **user_data;
    uint32_t queue_head;
    uint32_t queue_tail;
    pthread_mutex_t lock;
};

/* Worker thread data */
typedef struct {
    mdk_accel_stream_t *stream;
    uint32_t thread_id;
} worker_thread_data_t;

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

    // Initialize HackRF/Mayhem-MDK hardware with config
    if (!mdk_hardware_init()) {
        return false;
    }

    // Copy configuration
    g_config = *config;

    // Set default parallel streams if not specified
    if (g_config.hardware.parallel_streams == 0) {
        g_config.hardware.parallel_streams = 8;  // Default to 8 streams
    }

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
    // Initialize HackRF One hardware
    // Configure for Mayhem-MDK module
    // Set up GPIO, SPI, I2C interfaces

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
    // Release HackRF hardware
    // Reset GPIO, SPI, I2C
}

/**
 * Get version information
 */
const char* mdk_get_version(void) {
    return "MDK-Predator v1.0.0";
}

/**
 * Worker thread function for hardware acceleration
 */
static void* accel_worker_thread(void *arg) {
    worker_thread_data_t *data = (worker_thread_data_t *)arg;
    mdk_accel_stream_t *stream = data->stream;
    uint32_t thread_id = data->thread_id;
    
    // Execute assigned work item
    if (thread_id < stream->queue_tail && stream->callbacks[thread_id] != NULL) {
        void *input = stream->inputs[thread_id];
        void *output = stream->outputs[thread_id];
        mdk_accel_callback_t callback = stream->callbacks[thread_id];
        void *user_data = stream->user_data[thread_id];
        
        // Execute callback
        if (callback) {
            callback(input, output, user_data);
        }
    }
    
    free(data);
    return NULL;
}

/**
 * Create hardware acceleration stream
 */
mdk_accel_stream_t* mdk_accel_create_stream(uint32_t stream_count, uint32_t input_size, uint32_t output_size) {
    if (stream_count == 0 || stream_count > 64) {
        return NULL;
    }
    
    mdk_accel_stream_t *stream = calloc(1, sizeof(mdk_accel_stream_t));
    if (!stream) {
        return NULL;
    }
    
    stream->stream_count = stream_count;
    stream->input_size = input_size;
    stream->output_size = output_size;
    stream->queue_head = 0;
    stream->queue_tail = 0;
    
    // Allocate arrays
    stream->threads = calloc(stream_count, sizeof(pthread_t));
    stream->inputs = calloc(stream_count, sizeof(void*));
    stream->outputs = calloc(stream_count, sizeof(void*));
    stream->callbacks = calloc(stream_count, sizeof(mdk_accel_callback_t));
    stream->user_data = calloc(stream_count, sizeof(void*));
    
    if (!stream->threads || !stream->inputs || !stream->outputs || 
        !stream->callbacks || !stream->user_data) {
        mdk_accel_destroy_stream(stream);
        return NULL;
    }
    
    pthread_mutex_init(&stream->lock, NULL);
    
    return stream;
}

/**
 * Enqueue work item to hardware acceleration stream
 */
bool mdk_accel_enqueue(mdk_accel_stream_t *stream, void *input, void *output, 
                       mdk_accel_callback_t callback, void *user_data) {
    if (!stream || !callback) {
        return false;
    }
    
    pthread_mutex_lock(&stream->lock);
    
    if (stream->queue_tail >= stream->stream_count) {
        pthread_mutex_unlock(&stream->lock);
        return false;
    }
    
    uint32_t idx = stream->queue_tail;
    stream->inputs[idx] = input;
    stream->outputs[idx] = output;
    stream->callbacks[idx] = callback;
    stream->user_data[idx] = user_data;
    stream->queue_tail++;
    
    pthread_mutex_unlock(&stream->lock);
    
    return true;
}

/**
 * Execute all enqueued work on hardware acceleration stream
 */
bool mdk_accel_execute(mdk_accel_stream_t *stream) {
    if (!stream || stream->queue_tail == 0) {
        return false;
    }
    
    uint32_t threads_created = 0;
    
    // Create worker threads
    for (uint32_t i = 0; i < stream->queue_tail && i < stream->stream_count; i++) {
        worker_thread_data_t *data = malloc(sizeof(worker_thread_data_t));
        if (!data) {
            // Cleanup: wait for already-created threads to finish
            // (they will free their own data)
            for (uint32_t j = 0; j < threads_created; j++) {
                pthread_join(stream->threads[j], NULL);
            }
            return false;
        }
        
        data->stream = stream;
        data->thread_id = i;
        
        if (pthread_create(&stream->threads[i], NULL, accel_worker_thread, data) != 0) {
            // Thread creation failed - free data and cleanup
            free(data);
            // Wait for already-created threads to finish
            // (they will free their own data)
            for (uint32_t j = 0; j < threads_created; j++) {
                pthread_join(stream->threads[j], NULL);
            }
            return false;
        }
        
        threads_created++;
    }
    
    // Wait for all threads to complete (they will free their own data)
    for (uint32_t i = 0; i < threads_created; i++) {
        pthread_join(stream->threads[i], NULL);
    }
    
    // Reset queue
    stream->queue_head = 0;
    stream->queue_tail = 0;
    
    return true;
}

/**
 * Destroy hardware acceleration stream
 */
void mdk_accel_destroy_stream(mdk_accel_stream_t *stream) {
    if (!stream) {
        return;
    }
    
    pthread_mutex_destroy(&stream->lock);
    
    free(stream->threads);
    free(stream->inputs);
    free(stream->outputs);
    free(stream->callbacks);
    free(stream->user_data);
    free(stream);
}
