// mdk_hardware_interface.h

#ifndef MDK_HARDWARE_INTERFACE_H
#define MDK_HARDWARE_INTERFACE_H

#include <stdint.h>

// I2C ESP32-S3 API Definitions
#define I2C_MASTER_SDA_PIN 21
#define I2C_MASTER_SCL_PIN 22
#define I2C_MASTER_FREQUENCY 100000

// Maximum number of parallel streams for hardware acceleration
#define MDK_MAX_PARALLEL_STREAMS 4

void i2c_master_init();
void i2c_master_write(uint8_t device_addr, uint8_t *data, size_t size);
void i2c_master_read(uint8_t device_addr, uint8_t *data, size_t size);

// Parallel Processing Structures
typedef struct {
    uint8_t stream_id;
    uint32_t data_buffer[1024]; // Example buffer size
} StreamData;

StreamData streams[4]; // Four streams for parallel processing

// DMA Configuration
void dma_init();
void start_dma_transfer(StreamData *stream);

// Bruteforce Task Definitions
void bruteforce_task(StreamData *stream);

// Function Prototypes for Hardware Acceleration
void enable_hardware_acceleration();
void disable_hardware_acceleration();

#endif // MDK_HARDWARE_INTERFACE_H