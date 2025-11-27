/**
 * MDK Hardware Interface Unit Tests
 *
 * Comprehensive test suite for hardware interface functionality
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "hal/mdk_hardware_interface.h"

/* Test counters */
static int tests_passed = 0;
static int tests_failed = 0;

/* Test macros */
#define TEST(name) \
    static void name(void); \
    static void run_##name(void) { \
        printf("[TEST] %s\n", #name); \
        name(); \
    } \
    static void name(void)

#define ASSERT(condition, message) \
    do { \
        if (condition) { \
            printf("  ✓ %s\n", message); \
            tests_passed++; \
        } else { \
            printf("  ✗ %s\n", message); \
            tests_failed++; \
        } \
    } while(0)

/* ========================================================================
 * Hardware Interface Initialization Tests
 * ======================================================================== */

TEST(test_hw_interface_init_valid) {
    mdk_hardware_interface_config_t config = {
        .enable_i2c = true,
        .enable_dma = true,
        .enable_parallel_streams = true,
        .enable_gpio = true,
        .enable_uart = true,
        .max_dma_buffer_size = 1024 * 1024,
        .parallel_stream_count = 4
    };

    bool result = mdk_hardware_interface_init(&config);
    ASSERT(result == true, "Init should succeed with valid config");
}

TEST(test_hw_interface_init_null) {
    bool result = mdk_hardware_interface_init(NULL);
    ASSERT(result == false, "Init should fail with NULL config");
}

TEST(test_hw_interface_version) {
    const char *version = mdk_hardware_interface_version();
    ASSERT(version != NULL, "Version string should not be NULL");
    ASSERT(strlen(version) > 0, "Version string should not be empty");
}

TEST(test_hw_interface_self_test) {
    mdk_hardware_interface_config_t config = {
        .enable_i2c = true,
        .enable_dma = true,
        .enable_parallel_streams = false,
        .enable_gpio = false,
        .enable_uart = false,
        .max_dma_buffer_size = 1024 * 1024,
        .parallel_stream_count = 0
    };

    mdk_hardware_interface_init(&config);
    bool result = mdk_hardware_interface_self_test();
    ASSERT(result == true, "Self test should pass");
}

/* ========================================================================
 * I2C Tests
 * ======================================================================== */

TEST(test_i2c_init_valid) {
    mdk_i2c_config_t config = {
        .bus = MDK_I2C_BUS_0,
        .speed = MDK_I2C_SPEED_FAST,
        .sda_pin = 21,
        .scl_pin = 22,
        .pullup_enable = true,
        .timeout_ms = 1000
    };

    bool result = mdk_i2c_init(&config);
    ASSERT(result == true, "I2C init should succeed");

    mdk_i2c_deinit(MDK_I2C_BUS_0);
}

TEST(test_i2c_init_null) {
    bool result = mdk_i2c_init(NULL);
    ASSERT(result == false, "I2C init should fail with NULL config");
}

TEST(test_i2c_device_operations) {
    mdk_i2c_config_t config = {
        .bus = MDK_I2C_BUS_0,
        .speed = MDK_I2C_SPEED_STANDARD,
        .sda_pin = 21,
        .scl_pin = 22,
        .pullup_enable = true,
        .timeout_ms = 1000
    };

    mdk_i2c_init(&config);

    mdk_i2c_device_t device;
    bool result = mdk_i2c_device_open(&device, MDK_I2C_BUS_0, 0x50);
    ASSERT(result == true, "Device open should succeed");

    uint8_t write_data[] = {0x01, 0x02, 0x03};
    result = mdk_i2c_write(&device, write_data, sizeof(write_data));
    ASSERT(result == true, "I2C write should succeed");

    uint8_t read_data[3];
    result = mdk_i2c_read(&device, read_data, sizeof(read_data));
    ASSERT(result == true, "I2C read should succeed");

    result = mdk_i2c_write_reg(&device, 0x10, 0xAB);
    ASSERT(result == true, "I2C write register should succeed");

    uint8_t reg_value;
    result = mdk_i2c_read_reg(&device, 0x10, &reg_value);
    ASSERT(result == true, "I2C read register should succeed");

    mdk_i2c_device_close(&device);
    mdk_i2c_deinit(MDK_I2C_BUS_0);
}

TEST(test_i2c_i2cdecmdl_integration) {
    bool result = mdk_i2c_init_i2cdecmdl();
    ASSERT(result == true, "I2CDECMDL init should succeed");

    uint8_t status;
    result = mdk_i2c_i2cdecmdl_read_status(&status);
    ASSERT(result == true, "I2CDECMDL read status should succeed");

    uint8_t config_data[32] = {0x01, 0x02, 0x03, 0x04};
    result = mdk_i2c_i2cdecmdl_configure(config_data, sizeof(config_data));
    ASSERT(result == true, "I2CDECMDL configure should succeed");

    mdk_i2c_deinit(MDK_I2C_BUS_0);
}

/* ========================================================================
 * DMA Tests
 * ======================================================================== */

TEST(test_dma_init_valid) {
    mdk_dma_config_t config = {
        .channel = MDK_DMA_CHANNEL_0,
        .direction = MDK_DMA_MEM_TO_MEM,
        .priority = MDK_DMA_PRIORITY_HIGH,
        .circular_mode = false,
        .interrupt_enable = false,
        .callback = NULL,
        .user_data = NULL
    };

    bool result = mdk_dma_init(&config);
    ASSERT(result == true, "DMA init should succeed");

    mdk_dma_deinit(MDK_DMA_CHANNEL_0);
}

TEST(test_dma_transfer_sync) {
    mdk_dma_config_t config = {
        .channel = MDK_DMA_CHANNEL_0,
        .direction = MDK_DMA_MEM_TO_MEM,
        .priority = MDK_DMA_PRIORITY_HIGH,
        .circular_mode = false,
        .interrupt_enable = false,
        .callback = NULL,
        .user_data = NULL
    };

    mdk_dma_init(&config);

    uint8_t src_data[256];
    uint8_t dst_data[256];
    for (int i = 0; i < 256; i++) {
        src_data[i] = i;
        dst_data[i] = 0;
    }

    mdk_dma_transfer_t transfer = {
        .src_addr = src_data,
        .dst_addr = dst_data,
        .length = 256,
        .completed = false,
        .error = false
    };

    bool result = mdk_dma_transfer(MDK_DMA_CHANNEL_0, &transfer);
    ASSERT(result == true, "DMA transfer should succeed");
    ASSERT(memcmp(src_data, dst_data, 256) == 0, "Data should be transferred correctly");

    mdk_dma_deinit(MDK_DMA_CHANNEL_0);
}

TEST(test_dma_signal_capture) {
    mdk_dma_config_t config = {
        .channel = MDK_DMA_CHANNEL_1,
        .direction = MDK_DMA_PERIPH_TO_MEM,
        .priority = MDK_DMA_PRIORITY_VERY_HIGH,
        .circular_mode = true,
        .interrupt_enable = true,
        .callback = NULL,
        .user_data = NULL
    };

    mdk_dma_init(&config);

    uint8_t buffer[1024];
    bool result = mdk_dma_capture_signal(MDK_DMA_CHANNEL_1, buffer,
                                         sizeof(buffer), 433920000, 2000000, 100);
    ASSERT(result == true, "Signal capture should succeed");

    size_t samples;
    result = mdk_dma_get_capture_status(MDK_DMA_CHANNEL_1, &samples);
    ASSERT(result == true, "Get capture status should succeed");

    mdk_dma_deinit(MDK_DMA_CHANNEL_1);
}

/* ========================================================================
 * Stream Tests
 * ======================================================================== */

TEST(test_stream_init_valid) {
    uint8_t buffer[1024];

    mdk_stream_config_t config = {
        .stream_id = MDK_STREAM_0,
        .mode = MDK_STREAM_MODE_CAPTURE,
        .dma_channel = MDK_DMA_CHANNEL_0,
        .buffer = buffer,
        .buffer_size = sizeof(buffer),
        .callback = NULL,
        .user_data = NULL,
        .enable_acceleration = true
    };

    bool result = mdk_stream_init(&config);
    ASSERT(result == true, "Stream init should succeed");

    mdk_stream_deinit(MDK_STREAM_0);
}

TEST(test_stream_operations) {
    uint8_t buffer[1024];

    mdk_stream_config_t config = {
        .stream_id = MDK_STREAM_0,
        .mode = MDK_STREAM_MODE_PROCESS,
        .dma_channel = MDK_DMA_CHANNEL_0,
        .buffer = buffer,
        .buffer_size = sizeof(buffer),
        .callback = NULL,
        .user_data = NULL,
        .enable_acceleration = true
    };

    mdk_stream_init(&config);

    bool result = mdk_stream_start(MDK_STREAM_0);
    ASSERT(result == true, "Stream start should succeed");

    uint8_t write_data[128];
    memset(write_data, 0xAA, sizeof(write_data));
    result = mdk_stream_write(MDK_STREAM_0, write_data, sizeof(write_data));
    ASSERT(result == true, "Stream write should succeed");

    uint8_t read_data[128];
    size_t bytes_read;
    result = mdk_stream_read(MDK_STREAM_0, read_data, sizeof(read_data), &bytes_read);
    ASSERT(result == true, "Stream read should succeed");

    mdk_stream_stats_t stats;
    result = mdk_stream_get_stats(MDK_STREAM_0, &stats);
    ASSERT(result == true, "Get stream stats should succeed");

    result = mdk_stream_stop(MDK_STREAM_0);
    ASSERT(result == true, "Stream stop should succeed");

    mdk_stream_deinit(MDK_STREAM_0);
}

/* ========================================================================
 * Parallel Stream Tests
 * ======================================================================== */

TEST(test_parallel_streams_init) {
    uint8_t buffers[4][512];

    mdk_parallel_stream_config_t config = {
        .num_streams = 4,
        .synchronized = true,
        .base_frequency = 433920000,
        .streams = {
            {
                .stream_id = MDK_STREAM_0,
                .mode = MDK_STREAM_MODE_PROCESS,
                .dma_channel = MDK_DMA_CHANNEL_0,
                .buffer = buffers[0],
                .buffer_size = 512,
                .callback = NULL,
                .user_data = NULL,
                .enable_acceleration = true
            },
            {
                .stream_id = MDK_STREAM_1,
                .mode = MDK_STREAM_MODE_PROCESS,
                .dma_channel = MDK_DMA_CHANNEL_1,
                .buffer = buffers[1],
                .buffer_size = 512,
                .callback = NULL,
                .user_data = NULL,
                .enable_acceleration = true
            },
            {
                .stream_id = MDK_STREAM_2,
                .mode = MDK_STREAM_MODE_PROCESS,
                .dma_channel = MDK_DMA_CHANNEL_2,
                .buffer = buffers[2],
                .buffer_size = 512,
                .callback = NULL,
                .user_data = NULL,
                .enable_acceleration = true
            },
            {
                .stream_id = MDK_STREAM_3,
                .mode = MDK_STREAM_MODE_PROCESS,
                .dma_channel = MDK_DMA_CHANNEL_3,
                .buffer = buffers[3],
                .buffer_size = 512,
                .callback = NULL,
                .user_data = NULL,
                .enable_acceleration = true
            }
        }
    };

    bool result = mdk_parallel_streams_init(&config);
    ASSERT(result == true, "Parallel streams init should succeed");

    result = mdk_parallel_streams_start_all();
    ASSERT(result == true, "Start all streams should succeed");

    result = mdk_parallel_streams_sync();
    ASSERT(result == true, "Stream sync should succeed");

    float accel = mdk_parallel_streams_get_acceleration();
    ASSERT(accel >= 4.0f, "Acceleration should be at least 4x");

    result = mdk_parallel_streams_stop_all();
    ASSERT(result == true, "Stop all streams should succeed");

    mdk_parallel_streams_deinit();
}

/* ========================================================================
 * GPIO Tests
 * ======================================================================== */

TEST(test_gpio_operations) {
    mdk_gpio_config_t config = {
        .pin = 25,
        .direction = MDK_GPIO_OUTPUT,
        .pull = MDK_GPIO_PULL_NONE,
        .interrupt = MDK_GPIO_INT_DISABLE,
        .callback = NULL,
        .user_data = NULL
    };

    bool result = mdk_gpio_init(&config);
    ASSERT(result == true, "GPIO init should succeed");

    result = mdk_gpio_set_level(25, true);
    ASSERT(result == true, "GPIO set level should succeed");

    bool level;
    result = mdk_gpio_get_level(25, &level);
    ASSERT(result == true, "GPIO get level should succeed");

    result = mdk_gpio_toggle(25);
    ASSERT(result == true, "GPIO toggle should succeed");

    mdk_gpio_deinit(25);
}

/* ========================================================================
 * UART Tests
 * ======================================================================== */

TEST(test_uart_operations) {
    mdk_uart_config_t config = {
        .port = MDK_UART_PORT_0,
        .baud_rate = MDK_UART_BAUD_115200,
        .data_bits = MDK_UART_DATA_8_BITS,
        .parity = MDK_UART_PARITY_NONE,
        .stop_bits = MDK_UART_STOP_1_BIT,
        .tx_pin = 17,
        .rx_pin = 16,
        .use_dma = false,
        .dma_channel = 0
    };

    bool result = mdk_uart_init(&config);
    ASSERT(result == true, "UART init should succeed");

    const char *test_data = "Hello UART";
    result = mdk_uart_write(MDK_UART_PORT_0, (uint8_t *)test_data, strlen(test_data));
    ASSERT(result == true, "UART write should succeed");

    uint8_t read_data[64];
    size_t bytes_read;
    result = mdk_uart_read(MDK_UART_PORT_0, read_data, sizeof(read_data), &bytes_read);
    ASSERT(result == true, "UART read should succeed");

    size_t bytes_available;
    result = mdk_uart_available(MDK_UART_PORT_0, &bytes_available);
    ASSERT(result == true, "UART available should succeed");

    result = mdk_uart_flush(MDK_UART_PORT_0);
    ASSERT(result == true, "UART flush should succeed");

    mdk_uart_deinit(MDK_UART_PORT_0);
}

/* ========================================================================
 * Hardware Status Tests
 * ======================================================================== */

TEST(test_hw_status) {
    mdk_hardware_interface_config_t config = {
        .enable_i2c = true,
        .enable_dma = true,
        .enable_parallel_streams = false,
        .enable_gpio = true,
        .enable_uart = true,
        .max_dma_buffer_size = 1024 * 1024,
        .parallel_stream_count = 0
    };

    mdk_hardware_interface_init(&config);

    mdk_hw_system_status_t status;
    bool result = mdk_hw_get_system_status(&status);
    ASSERT(result == true, "Get system status should succeed");
}

/* ========================================================================
 * Main Test Runner
 * ======================================================================== */

int main_hardware_mdk_hardware_interface(void) {
    printf("========================================\n");
    printf("MDK Hardware Interface Unit Tests\n");
    printf("========================================\n\n");

    /* Hardware interface tests */
    run_test_hw_interface_init_valid();
    run_test_hw_interface_init_null();
    run_test_hw_interface_version();
    run_test_hw_interface_self_test();

    /* I2C tests */
    run_test_i2c_init_valid();
    run_test_i2c_init_null();
    run_test_i2c_device_operations();
    run_test_i2c_i2cdecmdl_integration();

    /* DMA tests */
    run_test_dma_init_valid();
    run_test_dma_transfer_sync();
    run_test_dma_signal_capture();

    /* Stream tests */
    run_test_stream_init_valid();
    run_test_stream_operations();

    /* Parallel stream tests */
    run_test_parallel_streams_init();

    /* GPIO tests */
    run_test_gpio_operations();

    /* UART tests */
    run_test_uart_operations();

    /* Status tests */
    run_test_hw_status();

    printf("\n========================================\n");
    printf("Test Results:\n");
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("========================================\n");

    return (tests_failed == 0) ? 0 : 1;
}
