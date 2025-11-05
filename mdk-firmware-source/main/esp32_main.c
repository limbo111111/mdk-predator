/*
 * MDK Predator ESP32-S3 Main Firmware
 * I2C Slave implementation for PortaPack H4M Integration
 * 
 * Hardware: ESP32-S3-WROOM-1 @ 240MHz Dual-Core
 * I2C Address: 0x51
 * Communication: I2C Slave to PortaPack (Master)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "nvs_flash.h"

// Include protocol implementations
#include "mdk_i2c_protocol.h"

static const char *TAG = "MDK_MAIN";

// I2C Configuration
#define I2C_SLAVE_SCL_IO           9        // GPIO 9 (SCL)
#define I2C_SLAVE_SDA_IO           8        // GPIO 8 (SDA)
#define I2C_SLAVE_NUM              I2C_NUM_0
#define I2C_SLAVE_ADDR             0x51     // Fixed address for MDK module
#define I2C_SLAVE_TX_BUF_LEN       256
#define I2C_SLAVE_RX_BUF_LEN       256
#define I2C_SLAVE_FREQ_HZ          400000   // 400 kHz

// Status LED
#define STATUS_LED_GPIO            GPIO_NUM_2

// Global state
static struct {
    uint8_t current_command;
    uint8_t protocol_mode;
    bool operation_active;
    uint32_t progress_current;
    uint32_t progress_total;
    uint8_t status;
    uint64_t result_code;
    QueueHandle_t cmd_queue;
} mdk_state = {0};

// Command structure for queue
typedef struct {
    uint8_t cmd;
    uint8_t data[64];
    size_t data_len;
} mdk_command_t;

// External protocol functions (will be linked)
extern void keeloq_init(void);
extern uint32_t keeloq_decrypt(uint32_t encrypted, uint64_t key);
extern uint32_t keeloq_encrypt(uint32_t plaintext, uint64_t key);
extern bool keeloq_bruteforce_dual_core(uint32_t encrypted, uint32_t serial, 
                                        uint64_t seed, uint64_t* result_key,
                                        void (*progress_callback)(uint32_t, uint32_t));

extern void fixed_code_init(void);
extern bool pt2260_decode(const uint16_t* pulses, size_t count, uint32_t* code, uint8_t* bits);
extern bool pt2260_encode(uint32_t code, uint8_t bits, uint16_t* pulses, size_t* count);
extern bool ht6p20_decode(const uint16_t* pulses, size_t count, uint32_t* code);
extern bool ev1527_decode(const uint16_t* pulses, size_t count, uint32_t* code);

extern void automotive_protocols_init(void);
extern void garage_door_protocols_init(void);

extern void mdk_hal_init(void);
extern bool transmit_code(uint64_t code, uint32_t frequency_hz);

// ==================== PROGRESS CALLBACK ====================

void bruteforce_progress_callback(uint32_t current, uint32_t total) {
    mdk_state.progress_current = current;
    mdk_state.progress_total = total;
    
    // Log every 1M iterations
    if (current % 1000000 == 0) {
        ESP_LOGI(TAG, "Progress: %lu / %lu (%.1f%%)", 
                 current, total, (float)current * 100.0f / total);
    }
}

// ==================== I2C SLAVE HANDLERS ====================

// I2C receive handler (commands from PortaPack)
static void i2c_slave_receive_handler(uint8_t* data, size_t len) {
    if (len < 1) return;
    
    uint8_t cmd = data[0];
    ESP_LOGD(TAG, "I2C RX: cmd=0x%02X, len=%d", cmd, len);
    
    // Queue command for processing
    if (mdk_state.cmd_queue) {
        mdk_command_t mdk_cmd = {
            .cmd = cmd,
            .data_len = (len > 1) ? (len - 1) : 0
        };
        
        if (mdk_cmd.data_len > 0) {
            memcpy(mdk_cmd.data, &data[1], mdk_cmd.data_len);
        }
        
        xQueueSend(mdk_state.cmd_queue, &mdk_cmd, 0);
    }
}

// I2C transmit handler (responses to PortaPack)
static size_t i2c_slave_transmit_handler(uint8_t* buffer, size_t max_len) {
    if (max_len < 1) return 0;
    
    // Return status byte
    buffer[0] = mdk_state.status;
    
    // If space available, add progress data
    if (max_len >= 9 && mdk_state.operation_active) {
        buffer[1] = (mdk_state.progress_current >> 24) & 0xFF;
        buffer[2] = (mdk_state.progress_current >> 16) & 0xFF;
        buffer[3] = (mdk_state.progress_current >> 8) & 0xFF;
        buffer[4] = mdk_state.progress_current & 0xFF;
        
        buffer[5] = (mdk_state.progress_total >> 24) & 0xFF;
        buffer[6] = (mdk_state.progress_total >> 16) & 0xFF;
        buffer[7] = (mdk_state.progress_total >> 8) & 0xFF;
        buffer[8] = mdk_state.progress_total & 0xFF;
        
        return 9;
    }
    
    return 1;
}

// ==================== COMMAND PROCESSOR TASK ====================

static void command_processor_task(void* arg) {
    mdk_command_t cmd;
    
    ESP_LOGI(TAG, "Command processor task started");
    
    while (1) {
        if (xQueueReceive(mdk_state.cmd_queue, &cmd, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Processing command: 0x%02X", cmd.cmd);
            
            mdk_state.current_command = cmd.cmd;
            mdk_state.status = MDK_STATUS_BUSY;
            
            switch (cmd.cmd) {
                // ===== PING =====
                case MDK_CMD_PING:
                    ESP_LOGI(TAG, "PING received");
                    mdk_state.status = MDK_STATUS_READY;
                    break;
                
                // ===== GET VERSION =====
                case MDK_CMD_GET_VERSION:
                    ESP_LOGI(TAG, "GET_VERSION");
                    mdk_state.result_code = (MDK_VERSION_MAJOR << 16) | (MDK_VERSION_MINOR << 8) | MDK_VERSION_PATCH;
                    mdk_state.status = MDK_STATUS_READY;
                    break;
                
                // ===== GET STATUS =====
                case MDK_CMD_GET_STATUS:
                    // Status already set
                    break;
                
                // ===== KEELOQ DECRYPT =====
                case MDK_CMD_KEELOQ_DECRYPT: {
                    if (cmd.data_len >= 12) {
                        uint32_t encrypted = (cmd.data[0] << 24) | (cmd.data[1] << 16) | 
                                           (cmd.data[2] << 8) | cmd.data[3];
                        uint64_t key = 0;
                        for (int i = 0; i < 8; i++) {
                            key = (key << 8) | cmd.data[4 + i];
                        }
                        
                        ESP_LOGI(TAG, "KEELOQ_DECRYPT: encrypted=0x%08lX, key=0x%016llX", encrypted, key);
                        
                        uint32_t decrypted = keeloq_decrypt(encrypted, key);
                        mdk_state.result_code = decrypted;
                        mdk_state.status = MDK_STATUS_READY;
                        
                        ESP_LOGI(TAG, "Result: 0x%08lX", decrypted);
                    } else {
                        mdk_state.status = MDK_STATUS_ERROR;
                    }
                    break;
                }
                
                // ===== KEELOQ ENCRYPT =====
                case MDK_CMD_KEELOQ_ENCRYPT: {
                    if (cmd.data_len >= 12) {
                        uint32_t plaintext = (cmd.data[0] << 24) | (cmd.data[1] << 16) | 
                                           (cmd.data[2] << 8) | cmd.data[3];
                        uint64_t key = 0;
                        for (int i = 0; i < 8; i++) {
                            key = (key << 8) | cmd.data[4 + i];
                        }
                        
                        ESP_LOGI(TAG, "KEELOQ_ENCRYPT: plaintext=0x%08lX, key=0x%016llX", plaintext, key);
                        
                        uint32_t encrypted = keeloq_encrypt(plaintext, key);
                        mdk_state.result_code = encrypted;
                        mdk_state.status = MDK_STATUS_READY;
                        
                        ESP_LOGI(TAG, "Result: 0x%08lX", encrypted);
                    } else {
                        mdk_state.status = MDK_STATUS_ERROR;
                    }
                    break;
                }
                
                // ===== KEELOQ BRUTEFORCE =====
                case MDK_CMD_KEELOQ_BRUTEFORCE: {
                    if (cmd.data_len >= 20) {
                        uint32_t encrypted = (cmd.data[0] << 24) | (cmd.data[1] << 16) | 
                                           (cmd.data[2] << 8) | cmd.data[3];
                        uint32_t serial = (cmd.data[4] << 24) | (cmd.data[5] << 16) | 
                                        (cmd.data[6] << 8) | cmd.data[7];
                        uint64_t seed = 0;
                        for (int i = 0; i < 8; i++) {
                            seed = (seed << 8) | cmd.data[8 + i];
                        }
                        uint32_t keyspace_size = (cmd.data[16] << 24) | (cmd.data[17] << 16) | 
                                                (cmd.data[18] << 8) | cmd.data[19];
                        
                        ESP_LOGI(TAG, "KEELOQ_BRUTEFORCE: encrypted=0x%08lX, serial=0x%08lX, seed=0x%016llX, keyspace=%lu",
                                 encrypted, serial, seed, keyspace_size);
                        
                        mdk_state.operation_active = true;
                        mdk_state.progress_current = 0;
                        mdk_state.progress_total = keyspace_size;
                        
                        uint64_t result_key = 0;
                        bool found = keeloq_bruteforce_dual_core(encrypted, serial, seed, 
                                                                &result_key, bruteforce_progress_callback);
                        
                        mdk_state.operation_active = false;
                        
                        if (found) {
                            mdk_state.result_code = result_key;
                            mdk_state.status = MDK_STATUS_READY;
                            ESP_LOGI(TAG, "Key found: 0x%016llX", result_key);
                        } else {
                            mdk_state.status = MDK_STATUS_ERROR;
                            ESP_LOGW(TAG, "Key not found");
                        }
                    } else {
                        mdk_state.status = MDK_STATUS_ERROR;
                    }
                    break;
                }
                
                // ===== PT2260 DECODE =====
                case MDK_CMD_PT2260_DECODE: {
                    if (cmd.data_len >= 4) {
                        uint16_t pulse_count = (cmd.data[0] << 8) | cmd.data[1];
                        if (pulse_count > 0 && cmd.data_len >= (2 + pulse_count * 2)) {
                            uint16_t pulses[128];
                            for (int i = 0; i < pulse_count && i < 128; i++) {
                                pulses[i] = (cmd.data[2 + i*2] << 8) | cmd.data[2 + i*2 + 1];
                            }
                            
                            uint32_t code = 0;
                            uint8_t bits = 0;
                            
                            if (pt2260_decode(pulses, pulse_count, &code, &bits)) {
                                mdk_state.result_code = ((uint64_t)bits << 32) | code;
                                mdk_state.status = MDK_STATUS_READY;
                                ESP_LOGI(TAG, "PT2260 decoded: code=0x%08lX, bits=%d", code, bits);
                            } else {
                                mdk_state.status = MDK_STATUS_ERROR;
                            }
                        } else {
                            mdk_state.status = MDK_STATUS_ERROR;
                        }
                    } else {
                        mdk_state.status = MDK_STATUS_ERROR;
                    }
                    break;
                }
                
                // ===== PT2260 ENCODE =====
                case MDK_CMD_PT2260_ENCODE: {
                    if (cmd.data_len >= 5) {
                        uint32_t code = (cmd.data[0] << 24) | (cmd.data[1] << 16) | 
                                      (cmd.data[2] << 8) | cmd.data[3];
                        uint8_t bits = cmd.data[4];
                        
                        uint16_t pulses[128];
                        size_t pulse_count = 0;
                        
                        if (pt2260_encode(code, bits, pulses, &pulse_count)) {
                            mdk_state.result_code = pulse_count;
                            mdk_state.status = MDK_STATUS_READY;
                            ESP_LOGI(TAG, "PT2260 encoded: %zu pulses", pulse_count);
                        } else {
                            mdk_state.status = MDK_STATUS_ERROR;
                        }
                    } else {
                        mdk_state.status = MDK_STATUS_ERROR;
                    }
                    break;
                }
                
                // ===== TRANSMIT CODE =====
                case MDK_CMD_TRANSMIT: {
                    if (cmd.data_len >= 12) {
                        uint64_t code = 0;
                        for (int i = 0; i < 8; i++) {
                            code = (code << 8) | cmd.data[i];
                        }
                        uint32_t frequency = (cmd.data[8] << 24) | (cmd.data[9] << 16) | 
                                           (cmd.data[10] << 8) | cmd.data[11];
                        
                        ESP_LOGI(TAG, "TRANSMIT: code=0x%016llX @ %lu Hz", code, frequency);
                        
                        if (transmit_code(code, frequency)) {
                            mdk_state.status = MDK_STATUS_READY;
                        } else {
                            mdk_state.status = MDK_STATUS_ERROR;
                        }
                    } else {
                        mdk_state.status = MDK_STATUS_ERROR;
                    }
                    break;
                }
                
                // ===== GET RESULT =====
                case MDK_CMD_GET_RESULT: {
                    // Result already in mdk_state.result_code
                    ESP_LOGI(TAG, "GET_RESULT: 0x%016llX", mdk_state.result_code);
                    mdk_state.status = MDK_STATUS_READY;
                    break;
                }
                
                // ===== RESET =====
                case MDK_CMD_RESET:
                    ESP_LOGI(TAG, "RESET");
                    mdk_state.operation_active = false;
                    mdk_state.progress_current = 0;
                    mdk_state.progress_total = 0;
                    mdk_state.result_code = 0;
                    mdk_state.status = MDK_STATUS_READY;
                    break;
                
                // ===== UNKNOWN =====
                default:
                    ESP_LOGW(TAG, "Unknown command: 0x%02X", cmd.cmd);
                    mdk_state.status = MDK_STATUS_ERROR;
                    break;
            }
            
            // Blink LED on command completion
            gpio_set_level(STATUS_LED_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(50));
            gpio_set_level(STATUS_LED_GPIO, 0);
        }
    }
}

// ==================== I2C SLAVE TASK ====================

static void i2c_slave_task(void* arg) {
    uint8_t rx_buffer[I2C_SLAVE_RX_BUF_LEN];
    uint8_t tx_buffer[I2C_SLAVE_TX_BUF_LEN];
    
    ESP_LOGI(TAG, "I2C slave task started");
    
    while (1) {
        // Wait for I2C transaction
        size_t rx_len = i2c_slave_read_buffer(I2C_SLAVE_NUM, rx_buffer, sizeof(rx_buffer), pdMS_TO_TICKS(100));
        
        if (rx_len > 0) {
            // Process received data
            i2c_slave_receive_handler(rx_buffer, rx_len);
        }
        
        // Prepare transmit buffer
        size_t tx_len = i2c_slave_transmit_handler(tx_buffer, sizeof(tx_buffer));
        
        if (tx_len > 0) {
            i2c_slave_write_buffer(I2C_SLAVE_NUM, tx_buffer, tx_len, pdMS_TO_TICKS(100));
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));  // Small delay
    }
}

// ==================== INITIALIZATION ====================

static void init_i2c_slave(void) {
    i2c_config_t conf_slave = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = I2C_SLAVE_ADDR,
    };
    
    esp_err_t ret = i2c_param_config(I2C_SLAVE_NUM, &conf_slave);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C param config failed: %s", esp_err_to_name(ret));
        return;
    }
    
    ret = i2c_driver_install(I2C_SLAVE_NUM, conf_slave.mode, 
                            I2C_SLAVE_RX_BUF_LEN, I2C_SLAVE_TX_BUF_LEN, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C driver install failed: %s", esp_err_to_name(ret));
        return;
    }
    
    ESP_LOGI(TAG, "I2C slave initialized on address 0x%02X", I2C_SLAVE_ADDR);
}

static void init_status_led(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << STATUS_LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_level(STATUS_LED_GPIO, 0);
    
    ESP_LOGI(TAG, "Status LED initialized on GPIO %d", STATUS_LED_GPIO);
}

// ==================== MAIN ====================

void app_main(void) {
    ESP_LOGI(TAG, "====================================");
    ESP_LOGI(TAG, "  MDK Predator ESP32-S3 Module");
    ESP_LOGI(TAG, "  Version: %d.%d.%d", MDK_VERSION_MAJOR, MDK_VERSION_MINOR, MDK_VERSION_PATCH);
    ESP_LOGI(TAG, "  I2C Address: 0x%02X", I2C_SLAVE_ADDR);
    ESP_LOGI(TAG, "====================================");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize status LED
    init_status_led();
    
    // Blink 3 times on startup
    for (int i = 0; i < 3; i++) {
        gpio_set_level(STATUS_LED_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(STATUS_LED_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    // Initialize protocol engines
    ESP_LOGI(TAG, "Initializing protocol engines...");
    keeloq_init();
    fixed_code_init();
    automotive_protocols_init();
    garage_door_protocols_init();
    ESP_LOGI(TAG, "✅ Protocol engines initialized");
    
    // Initialize hardware abstraction layer
    ESP_LOGI(TAG, "Initializing hardware abstraction layer...");
    mdk_hal_init();
    ESP_LOGI(TAG, "✅ Hardware abstraction layer initialized");
    
    // Initialize I2C slave
    ESP_LOGI(TAG, "Initializing I2C slave...");
    init_i2c_slave();
    ESP_LOGI(TAG, "✅ I2C slave initialized");
    
    // Create command queue
    mdk_state.cmd_queue = xQueueCreate(10, sizeof(mdk_command_t));
    if (!mdk_state.cmd_queue) {
        ESP_LOGE(TAG, "Failed to create command queue");
        return;
    }
    
    // Set initial status
    mdk_state.status = MDK_STATUS_READY;
    mdk_state.operation_active = false;
    
    // Start tasks
    xTaskCreatePinnedToCore(command_processor_task, "cmd_proc", 8192, NULL, 10, NULL, 0);
    xTaskCreatePinnedToCore(i2c_slave_task, "i2c_slave", 4096, NULL, 5, NULL, 1);
    
    ESP_LOGI(TAG, "====================================");
    ESP_LOGI(TAG, "  MDK Predator module ready!");
    ESP_LOGI(TAG, "  Waiting for I2C commands...");
    ESP_LOGI(TAG, "====================================");
    
    // Main loop (heartbeat LED)
    while (1) {
        if (mdk_state.status == MDK_STATUS_READY && !mdk_state.operation_active) {
            gpio_set_level(STATUS_LED_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(50));
            gpio_set_level(STATUS_LED_GPIO, 0);
            vTaskDelay(pdMS_TO_TICKS(1950));  // Blink every 2 seconds
        } else {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}
