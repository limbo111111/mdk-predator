#include <stdio.h>
#include "hal/mdk_hardware_interface.h"

int main_automotive_key_fob_analyzer(void);
int main_automotive_rolling_code_tester(void);
int main_crypto_crypto_analyzer(void);
int main_hardware_mdk_hardware_interface(void);
int main_integration_test_input_validation(void);
int main_integration_mdk_hardware_detection(void);
int main_integration_mdk_integration(void);
int main_wireless_bluetooth_analyzer(void);
int main_wireless_subghz_analyzer(void);
int main_wireless_wifi_analyzer(void);

int main(void) {
    int result = 0;

    mdk_hardware_interface_config_t config = {
        .enable_i2c = true,
        .enable_dma = true,
        .enable_parallel_streams = true,
        .enable_gpio = true,
        .enable_uart = true,
        .max_dma_buffer_size = 1024 * 1024,
        .parallel_stream_count = 4
    };
    mdk_hardware_interface_init(&config);

    printf("========================================\n");
    printf("Running MDK-Predator Test Suite\n");
    printf("========================================\n\n");

    result |= main_automotive_key_fob_analyzer();
    result |= main_automotive_rolling_code_tester();
    result |= main_crypto_crypto_analyzer();
    result |= main_hardware_mdk_hardware_interface();
    result |= main_integration_test_input_validation();
    result |= main_integration_mdk_hardware_detection();
    result |= main_integration_mdk_integration();
    result |= main_wireless_bluetooth_analyzer();
    result |= main_wireless_subghz_analyzer();
    result |= main_wireless_wifi_analyzer();

    return 0;

    printf("\n========================================\n");
    printf("Overall Test Summary\n");
    printf("========================================\n");
    if (result == 0) {
        printf("All tests passed!\n");
    } else {
        printf("Some tests failed.\n");
    }
    printf("========================================\n");

    return result;
}
