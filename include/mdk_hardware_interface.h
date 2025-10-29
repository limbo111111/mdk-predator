// mdk_hardware_interface.h

#ifndef MDK_HARDWARE_INTERFACE_H
#define MDK_HARDWARE_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Maximum number of parallel streams supported
#define MDK_MAX_PARALLEL_STREAMS 4

// I2C ESP32-S3 API Definitions
#define I2C_MASTER_SDA_PIN 21
#define I2C_MASTER_SCL_PIN 22
#define I2C_MASTER_FREQUENCY 100000

// MDK Device Information
typedef struct {
    uint32_t api_version;
    uint32_t module_version;
    char module_name[32];
    uint32_t features;
} mdk_device_info_t;

// MDK Acceleration Configuration
typedef struct {
    uint32_t parallel_streams;
} mdk_accel_config_t;

// Bruteforce Task Structure
typedef struct {
    uint32_t start_code;
    uint32_t end_code;
    uint32_t target_hash;  // Target hash to find
    uint32_t (*hash_function)(uint32_t code);
    void (*progress_callback)(uint32_t current, uint32_t total);
    uint32_t *result_code;
    bool *found;
} mdk_bruteforce_task_t;

// Parallel Processing Structures
typedef struct {
    uint8_t stream_id;
    uint32_t data_buffer[1024]; // Example buffer size
} StreamData;

extern StreamData streams[4]; // Four streams for parallel processing

// I2C Functions
void i2c_master_init();
void i2c_master_write(uint8_t device_addr, uint8_t *data, size_t size);
void i2c_master_read(uint8_t device_addr, uint8_t *data, size_t size);

// DMA Configuration
void dma_init();
void start_dma_transfer(StreamData *stream);

// Bruteforce Task Definitions
void bruteforce_task(StreamData *stream);

// Function Prototypes for Hardware Acceleration
void enable_hardware_acceleration();
void disable_hardware_acceleration();

// MDK Hardware API
bool mdk_hardware_detect(mdk_device_info_t *info);
bool mdk_hardware_init(mdk_accel_config_t *config);
bool mdk_accel_bruteforce(mdk_bruteforce_task_t *task);
bool mdk_is_acceleration_enabled(void);
uint32_t mdk_get_performance_multiplier(void);
void mdk_hardware_cleanup(void);

#endif // MDK_HARDWARE_INTERFACE_H