/**
 * MDK Hardware Interface
 *
 * Comprehensive hardware interface for Mayhem-MDK Predator Suite
 * Provides I2C, DMA, parallel streams, GPIO, and UART functionality
 * for ESP32-S3 compatible devices and hardware-accelerated operations
 */

#ifndef MDK_HARDWARE_INTERFACE_H
#define MDK_HARDWARE_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ========================================================================
 * I2C Device Communication (ESP32-S3 Compatible)
 * ======================================================================== */

/** I2C bus identifier */
typedef enum {
    MDK_I2C_BUS_0 = 0,
    MDK_I2C_BUS_1 = 1
} mdk_i2c_bus_t;

/** Number of I2C buses available */
#define MDK_I2C_BUS_COUNT 2

/** I2C speed modes */
typedef enum {
    MDK_I2C_SPEED_STANDARD = 100000,   /* 100 kHz */
    MDK_I2C_SPEED_FAST = 400000,       /* 400 kHz */
    MDK_I2C_SPEED_FAST_PLUS = 1000000  /* 1 MHz */
} mdk_i2c_speed_t;

/** I2C configuration */
typedef struct {
    mdk_i2c_bus_t bus;
    mdk_i2c_speed_t speed;
    uint8_t sda_pin;
    uint8_t scl_pin;
    bool pullup_enable;
    uint32_t timeout_ms;
} mdk_i2c_config_t;

/** I2C device handle */
typedef struct {
    mdk_i2c_bus_t bus;
    uint8_t device_addr;
    bool initialized;
} mdk_i2c_device_t;

/* I2C Functions */
bool mdk_i2c_init(const mdk_i2c_config_t *config);
bool mdk_i2c_deinit(mdk_i2c_bus_t bus);
bool mdk_i2c_device_probe(mdk_i2c_bus_t bus, uint8_t device_addr);
bool mdk_i2c_device_open(mdk_i2c_device_t *device, mdk_i2c_bus_t bus, uint8_t device_addr);
bool mdk_i2c_device_close(mdk_i2c_device_t *device);
bool mdk_i2c_write(const mdk_i2c_device_t *device, const uint8_t *data, size_t length);
bool mdk_i2c_read(const mdk_i2c_device_t *device, uint8_t *data, size_t length);
bool mdk_i2c_write_read(const mdk_i2c_device_t *device,
                        const uint8_t *write_data, size_t write_len,
                        uint8_t *read_data, size_t read_len);
bool mdk_i2c_write_reg(const mdk_i2c_device_t *device, uint8_t reg_addr, uint8_t value);
bool mdk_i2c_read_reg(const mdk_i2c_device_t *device, uint8_t reg_addr, uint8_t *value);

/* I2CDECMDL_PPMOD Integration */
bool mdk_i2c_init_i2cdecmdl(void);
bool mdk_i2c_i2cdecmdl_read_status(uint8_t *status);
bool mdk_i2c_i2cdecmdl_configure(const uint8_t *config_data, size_t length);

/* ========================================================================
 * DMA (Direct Memory Access) Interface
 * ======================================================================== */

/** DMA channel identifier */
typedef enum {
    MDK_DMA_CHANNEL_0 = 0,
    MDK_DMA_CHANNEL_1 = 1,
    MDK_DMA_CHANNEL_2 = 2,
    MDK_DMA_CHANNEL_3 = 3
} mdk_dma_channel_t;

/** Number of DMA channels available */
#define MDK_DMA_CHANNEL_COUNT 4

/** DMA transfer direction */
typedef enum {
    MDK_DMA_MEM_TO_MEM,
    MDK_DMA_MEM_TO_PERIPH,
    MDK_DMA_PERIPH_TO_MEM
} mdk_dma_direction_t;

/** DMA transfer priority */
typedef enum {
    MDK_DMA_PRIORITY_LOW,
    MDK_DMA_PRIORITY_MEDIUM,
    MDK_DMA_PRIORITY_HIGH,
    MDK_DMA_PRIORITY_VERY_HIGH
} mdk_dma_priority_t;

/** DMA callback function type */
typedef void (*mdk_dma_callback_t)(mdk_dma_channel_t channel, bool success, void *user_data);

/** DMA configuration */
typedef struct {
    mdk_dma_channel_t channel;
    mdk_dma_direction_t direction;
    mdk_dma_priority_t priority;
    bool circular_mode;
    bool interrupt_enable;
    mdk_dma_callback_t callback;
    void *user_data;
} mdk_dma_config_t;

/** DMA transfer descriptor */
typedef struct {
    void *src_addr;
    void *dst_addr;
    size_t length;
    bool completed;
    bool error;
} mdk_dma_transfer_t;

/* DMA Functions */
bool mdk_dma_init(const mdk_dma_config_t *config);
bool mdk_dma_deinit(mdk_dma_channel_t channel);
bool mdk_dma_transfer(mdk_dma_channel_t channel, const mdk_dma_transfer_t *transfer);
bool mdk_dma_transfer_async(mdk_dma_channel_t channel, const mdk_dma_transfer_t *transfer);
bool mdk_dma_wait_complete(mdk_dma_channel_t channel, uint32_t timeout_ms);
bool mdk_dma_abort(mdk_dma_channel_t channel);
bool mdk_dma_get_status(mdk_dma_channel_t channel, bool *active, size_t *bytes_transferred);

/* Signal Capture with DMA */
bool mdk_dma_capture_signal(mdk_dma_channel_t channel, void *buffer, size_t buffer_size,
                            uint32_t sample_rate, uint32_t duration_ms);
bool mdk_dma_get_capture_status(mdk_dma_channel_t channel, size_t *samples_captured);

/* ========================================================================
 * Parallel Stream Management (Hardware Acceleration)
 * ======================================================================== */

/** Stream identifier */
typedef enum {
    MDK_STREAM_0 = 0,
    MDK_STREAM_1 = 1,
    MDK_STREAM_2 = 2,
    MDK_STREAM_3 = 3,
    MDK_STREAM_MAX = 4
} mdk_stream_id_t;

/** Stream operation mode */
typedef enum {
    MDK_STREAM_MODE_CAPTURE,
    MDK_STREAM_MODE_PROCESS,
    MDK_STREAM_MODE_TRANSMIT
} mdk_stream_mode_t;

/** Stream processing callback */
typedef void (*mdk_stream_callback_t)(mdk_stream_id_t stream, void *data, size_t length, void *user_data);

/** Stream configuration */
typedef struct {
    mdk_stream_id_t stream_id;
    mdk_stream_mode_t mode;
    mdk_dma_channel_t dma_channel;
    void *buffer;
    size_t buffer_size;
    mdk_stream_callback_t callback;
    void *user_data;
    bool enable_acceleration;
} mdk_stream_config_t;

/** Parallel stream group configuration */
typedef struct {
    uint8_t num_streams;
    mdk_stream_config_t streams[MDK_STREAM_MAX];
    bool synchronized;
    uint32_t base_frequency;
} mdk_parallel_stream_config_t;

/** Stream statistics */
typedef struct {
    uint64_t samples_processed;
    uint64_t errors;
    uint32_t throughput_mbps;
    float acceleration_factor;
} mdk_stream_stats_t;

/* Stream Functions */
bool mdk_stream_init(const mdk_stream_config_t *config);
bool mdk_stream_deinit(mdk_stream_id_t stream);
bool mdk_stream_start(mdk_stream_id_t stream);
bool mdk_stream_stop(mdk_stream_id_t stream);
bool mdk_stream_write(mdk_stream_id_t stream, const void *data, size_t length);
bool mdk_stream_read(mdk_stream_id_t stream, void *data, size_t length, size_t *bytes_read);
bool mdk_stream_get_stats(mdk_stream_id_t stream, mdk_stream_stats_t *stats);

/* Parallel Stream Functions */
bool mdk_parallel_streams_init(const mdk_parallel_stream_config_t *config);
bool mdk_parallel_streams_deinit(void);
bool mdk_parallel_streams_start_all(void);
bool mdk_parallel_streams_stop_all(void);
bool mdk_parallel_streams_sync(void);
float mdk_parallel_streams_get_acceleration(void);

/* Bruteforce Acceleration with Parallel Streams */
typedef bool (*mdk_bruteforce_test_fn_t)(const void *key, size_t key_len, void *user_data);

bool mdk_parallel_bruteforce(const void *key_space, size_t key_len,
                             size_t num_keys, mdk_bruteforce_test_fn_t test_fn,
                             void *user_data, void *result_key);

/* ========================================================================
 * GPIO (General Purpose I/O) Interface
 * ======================================================================== */

/** GPIO pin number */
typedef uint8_t mdk_gpio_pin_t;

/** Number of GPIO pins available */
#define MDK_GPIO_PIN_COUNT 64

/** GPIO direction */
typedef enum {
    MDK_GPIO_INPUT,
    MDK_GPIO_OUTPUT
} mdk_gpio_direction_t;

/** GPIO pull mode */
typedef enum {
    MDK_GPIO_PULL_NONE,
    MDK_GPIO_PULL_UP,
    MDK_GPIO_PULL_DOWN
} mdk_gpio_pull_t;

/** GPIO interrupt mode */
typedef enum {
    MDK_GPIO_INT_DISABLE,
    MDK_GPIO_INT_RISING,
    MDK_GPIO_INT_FALLING,
    MDK_GPIO_INT_BOTH
} mdk_gpio_int_mode_t;

/** GPIO interrupt callback */
typedef void (*mdk_gpio_callback_t)(mdk_gpio_pin_t pin, void *user_data);

/** GPIO configuration */
typedef struct {
    mdk_gpio_pin_t pin;
    mdk_gpio_direction_t direction;
    mdk_gpio_pull_t pull;
    mdk_gpio_int_mode_t interrupt;
    mdk_gpio_callback_t callback;
    void *user_data;
} mdk_gpio_config_t;

/* GPIO Functions */
bool mdk_gpio_init(const mdk_gpio_config_t *config);
bool mdk_gpio_deinit(mdk_gpio_pin_t pin);
bool mdk_gpio_set_level(mdk_gpio_pin_t pin, bool level);
bool mdk_gpio_get_level(mdk_gpio_pin_t pin, bool *level);
bool mdk_gpio_toggle(mdk_gpio_pin_t pin);
bool mdk_gpio_set_direction(mdk_gpio_pin_t pin, mdk_gpio_direction_t direction);
bool mdk_gpio_set_pull(mdk_gpio_pin_t pin, mdk_gpio_pull_t pull);
bool mdk_gpio_enable_interrupt(mdk_gpio_pin_t pin, mdk_gpio_int_mode_t mode);
bool mdk_gpio_disable_interrupt(mdk_gpio_pin_t pin);

/* ========================================================================
 * UART (Universal Asynchronous Receiver/Transmitter) Interface
 * ======================================================================== */

/** UART port identifier */
typedef enum {
    MDK_UART_PORT_0 = 0,
    MDK_UART_PORT_1 = 1,
    MDK_UART_PORT_2 = 2
} mdk_uart_port_t;

/** Number of UART ports available */
#define MDK_UART_PORT_COUNT 3

/** UART baud rate */
typedef enum {
    MDK_UART_BAUD_9600 = 9600,
    MDK_UART_BAUD_19200 = 19200,
    MDK_UART_BAUD_38400 = 38400,
    MDK_UART_BAUD_57600 = 57600,
    MDK_UART_BAUD_115200 = 115200,
    MDK_UART_BAUD_230400 = 230400,
    MDK_UART_BAUD_460800 = 460800,
    MDK_UART_BAUD_921600 = 921600
} mdk_uart_baud_t;

/** UART data bits */
typedef enum {
    MDK_UART_DATA_5_BITS = 5,
    MDK_UART_DATA_6_BITS = 6,
    MDK_UART_DATA_7_BITS = 7,
    MDK_UART_DATA_8_BITS = 8
} mdk_uart_data_bits_t;

/** UART parity */
typedef enum {
    MDK_UART_PARITY_NONE,
    MDK_UART_PARITY_EVEN,
    MDK_UART_PARITY_ODD
} mdk_uart_parity_t;

/** UART stop bits */
typedef enum {
    MDK_UART_STOP_1_BIT,
    MDK_UART_STOP_1_5_BITS,
    MDK_UART_STOP_2_BITS
} mdk_uart_stop_bits_t;

/** UART configuration */
typedef struct {
    mdk_uart_port_t port;
    mdk_uart_baud_t baud_rate;
    mdk_uart_data_bits_t data_bits;
    mdk_uart_parity_t parity;
    mdk_uart_stop_bits_t stop_bits;
    uint8_t tx_pin;
    uint8_t rx_pin;
    bool use_dma;
    mdk_dma_channel_t dma_channel;
} mdk_uart_config_t;

/* UART Functions */
bool mdk_uart_init(const mdk_uart_config_t *config);
bool mdk_uart_deinit(mdk_uart_port_t port);
bool mdk_uart_write(mdk_uart_port_t port, const uint8_t *data, size_t length);
bool mdk_uart_read(mdk_uart_port_t port, uint8_t *data, size_t length, size_t *bytes_read);
bool mdk_uart_write_async(mdk_uart_port_t port, const uint8_t *data, size_t length);
bool mdk_uart_available(mdk_uart_port_t port, size_t *bytes_available);
bool mdk_uart_flush(mdk_uart_port_t port);
bool mdk_uart_set_baud_rate(mdk_uart_port_t port, mdk_uart_baud_t baud_rate);

/* ========================================================================
 * Hardware Status and Control
 * ======================================================================== */

/** Hardware subsystem */
typedef enum {
    MDK_HW_SUBSYS_I2C,
    MDK_HW_SUBSYS_DMA,
    MDK_HW_SUBSYS_STREAM,
    MDK_HW_SUBSYS_GPIO,
    MDK_HW_SUBSYS_UART
} mdk_hw_subsystem_t;

/** Hardware status */
typedef struct {
    bool initialized;
    bool active;
    uint32_t error_count;
    const char *last_error;
} mdk_hw_status_t;

/** System-wide hardware status */
typedef struct {
    mdk_hw_status_t i2c_bus[2];
    mdk_hw_status_t dma_channel[4];
    mdk_hw_status_t stream[MDK_STREAM_MAX];
    mdk_hw_status_t uart_port[3];
    uint32_t total_errors;
} mdk_hw_system_status_t;

/* Status Functions */
bool mdk_hw_get_status(mdk_hw_subsystem_t subsystem, uint8_t unit, mdk_hw_status_t *status);
bool mdk_hw_get_system_status(mdk_hw_system_status_t *status);
bool mdk_hw_reset_subsystem(mdk_hw_subsystem_t subsystem, uint8_t unit);
const char* mdk_hw_get_error_string(mdk_hw_subsystem_t subsystem);

/* ========================================================================
 * System Initialization and Cleanup
 * ======================================================================== */

/** Hardware interface configuration */
typedef struct {
    bool enable_i2c;
    bool enable_dma;
    bool enable_parallel_streams;
    bool enable_gpio;
    bool enable_uart;
    uint32_t max_dma_buffer_size;
    uint32_t parallel_stream_count;
} mdk_hardware_interface_config_t;

/* System Functions */
bool mdk_hardware_interface_init(const mdk_hardware_interface_config_t *config);
void mdk_hardware_interface_cleanup(void);
const char* mdk_hardware_interface_version(void);
bool mdk_hardware_interface_self_test(void);

#endif /* MDK_HARDWARE_INTERFACE_H */
