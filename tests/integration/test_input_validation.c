/**
 * Unit tests for Input Validation Utilities
 *
 * These tests validate the input validation functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "input_validation.h"

/* Test counter */
static int tests_passed = 0;
static int tests_failed = 0;

/* Test helper macros */
#define TEST_ASSERT(condition, message) do { \
    if (condition) { \
        printf("  ✓ %s\n", message); \
        tests_passed++; \
    } else { \
        printf("  ✗ %s\n", message); \
        tests_failed++; \
    } \
} while(0)

#define RUN_TEST(test_func) do { \
    printf("\n[TEST] %s\n", #test_func); \
    test_func(); \
} while(0)

/**
 * Test: Validate not null
 */
void test_validate_not_null() {
    int value = 42;

    TEST_ASSERT(validate_not_null(&value) == true, "Should return true for valid pointer");
    TEST_ASSERT(validate_not_null(NULL) == false, "Should return false for NULL pointer");
}

/**
 * Test: Validate buffer length
 */
void test_validate_buffer_length() {
    TEST_ASSERT(validate_buffer_length(100, 200) == VALIDATION_OK, "Should accept valid length");
    TEST_ASSERT(validate_buffer_length(0, 200) == VALIDATION_INVALID_LENGTH, "Should reject zero length");
    TEST_ASSERT(validate_buffer_length(300, 200) == VALIDATION_BUFFER_TOO_LARGE, "Should reject too large buffer");
}

/**
 * Test: Validate frequency
 */
void test_validate_frequency() {
    TEST_ASSERT(validate_frequency(433920000) == VALIDATION_OK, "Should accept valid frequency");
    TEST_ASSERT(validate_frequency(100000) == VALIDATION_OUT_OF_RANGE, "Should reject too low frequency");
    TEST_ASSERT(validate_frequency(7000000000UL) == VALIDATION_OUT_OF_RANGE, "Should reject too high frequency");
}

/**
 * Test: Validate SubGHz frequency
 */
void test_validate_subghz_frequency() {
    TEST_ASSERT(validate_subghz_frequency(433920000) == VALIDATION_OK, "Should accept 433.92 MHz");
    TEST_ASSERT(validate_subghz_frequency(315000000) == VALIDATION_OK, "Should accept 315 MHz");
    TEST_ASSERT(validate_subghz_frequency(868000000) == VALIDATION_OK, "Should accept 868 MHz");
    TEST_ASSERT(validate_subghz_frequency(100000000) == VALIDATION_OUT_OF_RANGE, "Should reject 100 MHz");
    TEST_ASSERT(validate_subghz_frequency(1000000000) == VALIDATION_OUT_OF_RANGE, "Should reject 1 GHz");
}

/**
 * Test: Validate addition overflow
 */
void test_validate_add_no_overflow() {
    TEST_ASSERT(validate_add_no_overflow(100, 200) == true, "Should accept safe addition");
    TEST_ASSERT(validate_add_no_overflow(UINT32_MAX, 1) == false, "Should reject overflow addition");
    TEST_ASSERT(validate_add_no_overflow(UINT32_MAX - 10, 10) == true, "Should accept edge case");
    TEST_ASSERT(validate_add_no_overflow(UINT32_MAX - 10, 11) == false, "Should reject edge case overflow");
}

/**
 * Test: Validate multiplication overflow
 */
void test_validate_mul_no_overflow() {
    TEST_ASSERT(validate_mul_no_overflow(100, 200) == true, "Should accept safe multiplication");
    TEST_ASSERT(validate_mul_no_overflow(UINT32_MAX, 2) == false, "Should reject overflow multiplication");
    TEST_ASSERT(validate_mul_no_overflow(0, UINT32_MAX) == true, "Should accept multiplication by zero");
    TEST_ASSERT(validate_mul_no_overflow(65535, 65536) == true, "Should accept valid multiplication");
    TEST_ASSERT(validate_mul_no_overflow(65536, 65536) == false, "Should reject overflow");
}

/**
 * Test: Validate string length
 */
void test_validate_string_length() {
    char short_str[] = "test";
    char long_str[100];
    memset(long_str, 'A', sizeof(long_str) - 1);
    long_str[sizeof(long_str) - 1] = '\0';

    TEST_ASSERT(validate_string_length(short_str, 100) == VALIDATION_OK, "Should accept short string");
    TEST_ASSERT(validate_string_length(NULL, 100) == VALIDATION_NULL_POINTER, "Should reject NULL string");
    TEST_ASSERT(validate_string_length(long_str, 50) == VALIDATION_BUFFER_TOO_LARGE, "Should reject too long string");
}

/**
 * Test: Validate array index
 */
void test_validate_array_index() {
    TEST_ASSERT(validate_array_index(0, 10) == true, "Should accept first index");
    TEST_ASSERT(validate_array_index(9, 10) == true, "Should accept last valid index");
    TEST_ASSERT(validate_array_index(10, 10) == false, "Should reject out of bounds index");
    TEST_ASSERT(validate_array_index(100, 10) == false, "Should reject far out of bounds index");
}

/**
 * Test: Validate MAC address
 */
void test_validate_mac_address() {
    uint8_t mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

    TEST_ASSERT(validate_mac_address(mac) == true, "Should accept valid MAC pointer");
    TEST_ASSERT(validate_mac_address(NULL) == false, "Should reject NULL MAC pointer");
}

/**
 * Test: Validate WiFi 2.4 GHz channel
 */
void test_validate_wifi_24_channel() {
    TEST_ASSERT(validate_wifi_24_channel(1) == VALIDATION_OK, "Should accept channel 1");
    TEST_ASSERT(validate_wifi_24_channel(6) == VALIDATION_OK, "Should accept channel 6");
    TEST_ASSERT(validate_wifi_24_channel(14) == VALIDATION_OK, "Should accept channel 14");
    TEST_ASSERT(validate_wifi_24_channel(0) == VALIDATION_OUT_OF_RANGE, "Should reject channel 0");
    TEST_ASSERT(validate_wifi_24_channel(15) == VALIDATION_OUT_OF_RANGE, "Should reject channel 15");
}

/**
 * Test: Validate WiFi 5 GHz channel
 */
void test_validate_wifi_5_channel() {
    TEST_ASSERT(validate_wifi_5_channel(36) == VALIDATION_OK, "Should accept channel 36");
    TEST_ASSERT(validate_wifi_5_channel(100) == VALIDATION_OK, "Should accept channel 100");
    TEST_ASSERT(validate_wifi_5_channel(165) == VALIDATION_OK, "Should accept channel 165");
    TEST_ASSERT(validate_wifi_5_channel(35) == VALIDATION_OUT_OF_RANGE, "Should reject channel 35");
    TEST_ASSERT(validate_wifi_5_channel(166) == VALIDATION_OUT_OF_RANGE, "Should reject channel 166");
}

/**
 * Test: Safe memory copy
 */
void test_safe_memcpy() {
    uint8_t src[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    uint8_t dest[10];

    TEST_ASSERT(safe_memcpy(dest, sizeof(dest), src, 5) == true, "Should succeed with valid copy");
    TEST_ASSERT(dest[0] == 1 && dest[4] == 5, "Should copy data correctly");
    TEST_ASSERT(safe_memcpy(NULL, 10, src, 5) == false, "Should fail with NULL dest");
    TEST_ASSERT(safe_memcpy(dest, sizeof(dest), NULL, 5) == false, "Should fail with NULL src");
    TEST_ASSERT(safe_memcpy(dest, 5, src, 10) == false, "Should fail with too large copy");
}

/**
 * Test: Safe string copy
 */
void test_safe_strncpy() {
    char src[] = "Hello, World!";
    char dest[20];

    TEST_ASSERT(safe_strncpy(dest, sizeof(dest), src, strlen(src)) == true, "Should succeed with valid copy");
    TEST_ASSERT(strcmp(dest, "Hello, World!") == 0, "Should copy string correctly");
    TEST_ASSERT(safe_strncpy(NULL, 20, src, 5) == false, "Should fail with NULL dest");
    TEST_ASSERT(safe_strncpy(dest, sizeof(dest), NULL, 5) == false, "Should fail with NULL src");

    char small_dest[5];
    TEST_ASSERT(safe_strncpy(small_dest, sizeof(small_dest), src, strlen(src)) == true, "Should truncate if needed");
    TEST_ASSERT(strlen(small_dest) < sizeof(small_dest), "Should null-terminate truncated string");
}

/**
 * Test: Validate count
 */
void test_validate_count() {
    TEST_ASSERT(validate_count(50, 100) == VALIDATION_OK, "Should accept valid count");
    TEST_ASSERT(validate_count(0, 100) == VALIDATION_INVALID_LENGTH, "Should reject zero count");
    TEST_ASSERT(validate_count(150, 100) == VALIDATION_OUT_OF_RANGE, "Should reject too large count");
}

/**
 * Test: Get validation error message
 */
void test_get_validation_error_message() {
    const char *msg;

    msg = get_validation_error_message(VALIDATION_OK);
    TEST_ASSERT(msg != NULL && strlen(msg) > 0, "Should return message for VALIDATION_OK");

    msg = get_validation_error_message(VALIDATION_NULL_POINTER);
    TEST_ASSERT(msg != NULL && strlen(msg) > 0, "Should return message for VALIDATION_NULL_POINTER");

    msg = get_validation_error_message(VALIDATION_BUFFER_TOO_LARGE);
    TEST_ASSERT(msg != NULL && strlen(msg) > 0, "Should return message for VALIDATION_BUFFER_TOO_LARGE");
}

/**
 * Main test runner
 */
int main(void) {
    printf("========================================\n");
    printf("Input Validation Utilities Unit Tests\n");
    printf("========================================\n");

    RUN_TEST(test_validate_not_null);
    RUN_TEST(test_validate_buffer_length);
    RUN_TEST(test_validate_frequency);
    RUN_TEST(test_validate_subghz_frequency);
    RUN_TEST(test_validate_add_no_overflow);
    RUN_TEST(test_validate_mul_no_overflow);
    RUN_TEST(test_validate_string_length);
    RUN_TEST(test_validate_array_index);
    RUN_TEST(test_validate_mac_address);
    RUN_TEST(test_validate_wifi_24_channel);
    RUN_TEST(test_validate_wifi_5_channel);
    RUN_TEST(test_safe_memcpy);
    RUN_TEST(test_safe_strncpy);
    RUN_TEST(test_validate_count);
    RUN_TEST(test_get_validation_error_message);

    printf("\n========================================\n");
    printf("Test Results:\n");
    printf("  Passed: %d\n", tests_passed);
    printf("  Failed: %d\n", tests_failed);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
