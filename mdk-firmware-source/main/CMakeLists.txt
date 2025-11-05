idf_component_register(SRCS "esp32_main.c" "mdk_hardware_abstraction.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver esp_system nvs_flash spi_flash esp_timer mbedtls)
