// mdk_hardware_interface.h

#ifndef MDK_HARDWARE_INTERFACE_H
#define MDK_HARDWARE_INTERFACE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Constants
#define MDK_MAX_PARALLEL_STREAMS 8
#define MDK_MODULE_NAME_SIZE 32

// Device information structure
typedef struct {
    uint8_t api_version;
    uint8_t module_version;
    char module_name[MDK_MODULE_NAME_SIZE];
    uint32_t features;
} mdk_device_info_t;

// Acceleration configuration structure
typedef struct {
    uint8_t parallel_streams;
    uint8_t reserved[3];
} mdk_accel_config_t;

// Callback function types
typedef uint32_t (*mdk_hash_func_t)(uint32_t code);
typedef void (*mdk_progress_callback_t)(uint32_t current, uint32_t total);

// Bruteforce task structure
typedef struct {
    uint32_t start_code;
    uint32_t end_code;
    mdk_hash_func_t hash_function;
    mdk_progress_callback_t progress_callback;
    uint32_t *result_code;
    bool *found;
} mdk_bruteforce_task_t;

// Hardware detection and initialization
bool mdk_hardware_detect(mdk_device_info_t *info);
bool mdk_hardware_init(mdk_accel_config_t *config);
void mdk_hardware_cleanup(void);

// Hardware acceleration functions
bool mdk_accel_bruteforce(mdk_bruteforce_task_t *task);
bool mdk_is_acceleration_enabled(void);
uint32_t mdk_get_performance_multiplier(void);

// I2C ESP32-S3 API Definitions (for compatibility)
#define I2C_MASTER_SDA_PIN 21
#define I2C_MASTER_SCL_PIN 22
#define I2C_MASTER_FREQUENCY 100000

void i2c_master_init(void);
void i2c_master_write(uint8_t device_addr, uint8_t *data, size_t size);
void i2c_master_read(uint8_t device_addr, uint8_t *data, size_t size);

// Parallel Processing Structures (for compatibility)
typedef struct {
    uint8_t stream_id;
    uint32_t data_buffer[1024];
} StreamData;

extern StreamData streams[4];

// DMA Configuration (for compatibility)
void dma_init(void);
void start_dma_transfer(StreamData *stream);

// Bruteforce Task Definitions (for compatibility)
void bruteforce_task(StreamData *stream);

// Function Prototypes for Hardware Acceleration (for compatibility)
void enable_hardware_acceleration(void);
void disable_hardware_acceleration(void);

#endif // MDK_HARDWARE_INTERFACE_H