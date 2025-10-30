#ifndef ROLLING_CODE_TESTER_H
#define ROLLING_CODE_TESTER_H

// Updated hardware acceleration API functions

// Function to encrypt using Keeloq algorithm
void keeloq_encrypt_test(void);

// Function to perform rolling code bruteforce with hardware acceleration
void rolling_code_bruteforce_hw(void);

// Function to get performance metrics of the rolling code
int rolling_code_get_performance(int multiplier);

#endif /* ROLLING_CODE_TESTER_H */