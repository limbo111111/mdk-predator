/**
 * MDK PortaPack Interface Header
 *
 * Real hardware interface wrappers for PortaPack Mayhem firmware integration.
 * These are called by mdk_hardware_interface.c to access actual HackRF/Mayhem hardware.
 *
 * Compatible with Mayhem firmware v2.3.1+
 */

#ifndef MDK_PORTAPACK_INTERFACE_H
#define MDK_PORTAPACK_INTERFACE_H

#include "mdk_hardware_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * I2C Wrapper Functions (PortaPack/Mayhem API)
 * ======================================================================== */

/**
 * Initialize I2C bus using Mayhem firmware I2C driver.
 * @param config I2C configuration (bus, speed, pins, etc.)
 * @return true if successful, false on error
 */
bool mdk_portapack_i2c_init(const mdk_i2c_config_t *config);

/**
 * Probe for I2C device on bus.
 * @param bus I2C bus identifier
 * @param device_addr 7-bit I2C device address
 * @return true if device responds to address, false otherwise
 */
bool mdk_portapack_i2c_probe(mdk_i2c_bus_t bus, uint8_t device_addr);

/**
 * Read from I2C device.
 * @param device I2C device handle
 * @param data Buffer to receive data
 * @param length Number of bytes to read
 * @return true if successful, false on error or NACK
 */
bool mdk_portapack_i2c_read(const mdk_i2c_device_t *device, uint8_t *data, size_t length);

/**
 * Write to I2C device.
 * @param device I2C device handle
 * @param data Data to transmit
 * @param length Number of bytes to write
 * @return true if successful, false on error or NACK
 */
bool mdk_portapack_i2c_write(const mdk_i2c_device_t *device, const uint8_t *data, size_t length);

/* ========================================================================
 * DMA Wrapper Functions (PortaPack LPC43xx GPDMA)
 * ======================================================================== */

/**
 * Initialize DMA channel for signal capture or data transfer.
 * @param config DMA configuration
 * @return true if successful, false on error
 */
bool mdk_portapack_dma_init(const mdk_dma_config_t *config);

/**
 * Perform blocking DMA transfer.
 * @param channel DMA channel identifier
 * @param transfer Transfer descriptor (source, destination, length)
 * @return true if successful, false on error
 */
bool mdk_portapack_dma_transfer(mdk_dma_channel_t channel, const mdk_dma_transfer_t *transfer);

/**
 * Capture RF signal via DMA from HackRF baseband.
 * Configures HackRF ADC and DMA for continuous signal capture.
 * @param channel DMA channel to use
 * @param buffer Output buffer for samples
 * @param buffer_size Size of buffer in bytes
 * @param sample_rate Sampling frequency in Hz
 * @param duration_ms Duration to capture in milliseconds
 * @return true if capture started/completed successfully
 */
bool mdk_portapack_dma_capture_rf_signal(mdk_dma_channel_t channel, void *buffer, size_t buffer_size,
                                         uint32_t frequency, uint32_t sample_rate, uint32_t duration_ms);

/* ========================================================================
 * GPIO Wrapper Functions
 * ======================================================================== */

/**
 * Initialize GPIO pin (input, output, interrupt, etc.).
 * @param config GPIO configuration
 * @return true if successful, false on error
 */
bool mdk_portapack_gpio_init(const mdk_gpio_config_t *config);

/**
 * Set GPIO output pin level.
 * @param pin GPIO pin number
 * @param level true for HIGH, false for LOW
 * @return true if successful, false on error
 */
bool mdk_portapack_gpio_set_level(mdk_gpio_pin_t pin, bool level);

/**
 * Read GPIO input pin level.
 * @param pin GPIO pin number
 * @param level Pointer to store input level
 * @return true if successful, false on error
 */
bool mdk_portapack_gpio_get_level(mdk_gpio_pin_t pin, bool *level);

/* ========================================================================
 * UART Wrapper Functions (Debug/Logging)
 * ======================================================================== */

/**
 * Initialize UART port for debug console or device communication.
 * @param config UART configuration (port, baud rate, data format, etc.)
 * @return true if successful, false on error
 */
bool mdk_portapack_uart_init(const mdk_uart_config_t *config);

/**
 * Write data to UART port.
 * @param port UART port identifier
 * @param data Bytes to transmit
 * @param length Number of bytes
 * @return true if successful, false on error
 */
bool mdk_portapack_uart_write(mdk_uart_port_t port, const uint8_t *data, size_t length);

/**
 * Read data from UART port.
 * @param port UART port identifier
 * @param data Buffer to receive data
 * @param length Number of bytes to read
 * @param bytes_read Pointer to store number of bytes actually read
 * @return true if successful, false on error
 */
bool mdk_portapack_uart_read(mdk_uart_port_t port, uint8_t *data, size_t length, size_t *bytes_read);

/* ========================================================================
 * System Integration Helpers
 * ======================================================================== */

/**
 * Print Mayhem firmware version and hardware info for diagnostics.
 * Useful for troubleshooting version compatibility issues.
 */
void mdk_portapack_print_system_info(void);

/**
 * Initialize all enabled hardware subsystems.
 * Called once during MDK-Predator startup to set up I2C, DMA, GPIO, etc.
 * @param config Hardware interface configuration
 * @return true if all enabled subsystems initialized successfully
 */
bool mdk_portapack_init_all(const mdk_hardware_interface_config_t *config);

#ifdef __cplusplus
}
#endif

#endif /* MDK_PORTAPACK_INTERFACE_H */
