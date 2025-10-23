# MDK-Predator Makefile
# Build system for Mayhem-MDK Predator Security Suite

# Compiler and flags
CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
CFLAGS = -Wall -Wextra -O2 -std=c11 -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
INCLUDES = -Iinclude
LDFLAGS = -lm

# Directories
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
LIB_DIR = $(BUILD_DIR)/lib

# Source files
AUTOMOTIVE_SRC = $(wildcard $(SRC_DIR)/automotive/*.c)
WIRELESS_SRC = $(wildcard $(SRC_DIR)/wireless/*.c)
CRYPTO_SRC = $(wildcard $(SRC_DIR)/crypto/*.c)
MAIN_SRC = $(SRC_DIR)/mdk_predator.c

ALL_SRC = $(AUTOMOTIVE_SRC) $(WIRELESS_SRC) $(CRYPTO_SRC) $(MAIN_SRC)

# Object files
AUTOMOTIVE_OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(AUTOMOTIVE_SRC))
WIRELESS_OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(WIRELESS_SRC))
CRYPTO_OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(CRYPTO_SRC))
MAIN_OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(MAIN_SRC))

ALL_OBJ = $(AUTOMOTIVE_OBJ) $(WIRELESS_OBJ) $(CRYPTO_OBJ) $(MAIN_OBJ)

# Output library
LIB_NAME = libmdk_predator.a
LIB_PATH = $(LIB_DIR)/$(LIB_NAME)

# Targets
.PHONY: all clean directories test test-build test-run test-automotive test-wireless test-crypto test-integration test-clean test-directories info automotive wireless crypto

all: directories $(LIB_PATH)

directories:
	@mkdir -p $(OBJ_DIR)/automotive
	@mkdir -p $(OBJ_DIR)/wireless
	@mkdir -p $(OBJ_DIR)/crypto
	@mkdir -p $(LIB_DIR)

$(LIB_PATH): $(ALL_OBJ)
	@echo "Creating library: $@"
	$(AR) rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR)

# Module-specific targets
automotive: $(AUTOMOTIVE_OBJ)
	@echo "Automotive module built"

wireless: $(WIRELESS_OBJ)
	@echo "Wireless module built"

crypto: $(CRYPTO_OBJ)
	@echo "Crypto module built"

# Print configuration
info:
	@echo "MDK-Predator Build Configuration"
	@echo "================================="
	@echo "CC: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "Source files: $(words $(ALL_SRC))"
	@echo "Object files: $(words $(ALL_OBJ))"
	@echo "Library: $(LIB_PATH)"

# Test configuration
TEST_CC = gcc
TEST_CFLAGS = -Wall -Wextra -O2 -std=c11
TEST_INCLUDES = -Iinclude
TEST_DIR = tests
TEST_BUILD_DIR = $(BUILD_DIR)/tests
TEST_BIN_DIR = $(TEST_BUILD_DIR)/bin

# Test source files
TEST_AUTOMOTIVE_SRC = $(wildcard $(TEST_DIR)/automotive/*.c)
TEST_WIRELESS_SRC = $(wildcard $(TEST_DIR)/wireless/*.c)
TEST_CRYPTO_SRC = $(wildcard $(TEST_DIR)/crypto/*.c)
TEST_INTEGRATION_SRC = $(wildcard $(TEST_DIR)/integration/*.c)

ALL_TEST_SRC = $(TEST_AUTOMOTIVE_SRC) $(TEST_WIRELESS_SRC) $(TEST_CRYPTO_SRC) $(TEST_INTEGRATION_SRC)

# Test binaries
TEST_AUTOMOTIVE_BIN = $(patsubst $(TEST_DIR)/automotive/%.c,$(TEST_BIN_DIR)/automotive/%,$(TEST_AUTOMOTIVE_SRC))
TEST_WIRELESS_BIN = $(patsubst $(TEST_DIR)/wireless/%.c,$(TEST_BIN_DIR)/wireless/%,$(TEST_WIRELESS_SRC))
TEST_CRYPTO_BIN = $(patsubst $(TEST_DIR)/crypto/%.c,$(TEST_BIN_DIR)/crypto/%,$(TEST_CRYPTO_SRC))
TEST_INTEGRATION_BIN = $(patsubst $(TEST_DIR)/integration/%.c,$(TEST_BIN_DIR)/integration/%,$(TEST_INTEGRATION_SRC))

ALL_TEST_BIN = $(TEST_AUTOMOTIVE_BIN) $(TEST_WIRELESS_BIN) $(TEST_CRYPTO_BIN) $(TEST_INTEGRATION_BIN)

# Test targets
.PHONY: test test-build test-run test-automotive test-wireless test-crypto test-integration test-clean

test: test-build test-run

test-build: $(ALL_TEST_BIN)

test-directories:
	@mkdir -p $(TEST_BIN_DIR)/automotive
	@mkdir -p $(TEST_BIN_DIR)/wireless
	@mkdir -p $(TEST_BIN_DIR)/crypto
	@mkdir -p $(TEST_BIN_DIR)/integration

# Compile test binaries - automotive
$(TEST_BIN_DIR)/automotive/%: $(TEST_DIR)/automotive/%.c $(AUTOMOTIVE_SRC) test-directories
	@echo "Building test: $@"
	$(TEST_CC) $(TEST_CFLAGS) $(TEST_INCLUDES) $< $(AUTOMOTIVE_SRC) -o $@

# Compile test binaries - wireless
$(TEST_BIN_DIR)/wireless/%: $(TEST_DIR)/wireless/%.c $(WIRELESS_SRC) test-directories
	@echo "Building test: $@"
	$(TEST_CC) $(TEST_CFLAGS) $(TEST_INCLUDES) $< $(WIRELESS_SRC) -o $@

# Compile test binaries - crypto
$(TEST_BIN_DIR)/crypto/%: $(TEST_DIR)/crypto/%.c $(CRYPTO_SRC) test-directories
	@echo "Building test: $@"
	$(TEST_CC) $(TEST_CFLAGS) $(TEST_INCLUDES) $< $(CRYPTO_SRC) -lm -o $@

# Compile test binaries - integration
$(TEST_BIN_DIR)/integration/test_input_validation: $(TEST_DIR)/integration/test_input_validation.c test-directories
	@echo "Building test: $@"
	$(TEST_CC) $(TEST_CFLAGS) $(TEST_INCLUDES) $< -o $@

$(TEST_BIN_DIR)/integration/%: $(TEST_DIR)/integration/%.c $(ALL_SRC) test-directories
	@echo "Building test: $@"
	$(TEST_CC) $(TEST_CFLAGS) $(TEST_INCLUDES) $< $(filter-out $(MAIN_SRC),$(ALL_SRC)) $(MAIN_SRC) -lm -o $@

test-run:
	@echo ""
	@echo "========================================="
	@echo "Running MDK-Predator Test Suite"
	@echo "========================================="
	@echo ""
	@test_count=0; \
	pass_count=0; \
	fail_count=0; \
	for test in $(ALL_TEST_BIN); do \
		if [ -f "$$test" ]; then \
			test_count=$$((test_count + 1)); \
			echo "Running: $$test"; \
			if $$test; then \
				pass_count=$$((pass_count + 1)); \
			else \
				fail_count=$$((fail_count + 1)); \
			fi; \
			echo ""; \
		fi; \
	done; \
	echo "========================================"; \
	echo "Overall Test Summary"; \
	echo "========================================"; \
	echo "Total test suites: $$test_count"; \
	echo "Passed: $$pass_count"; \
	echo "Failed: $$fail_count"; \
	echo "========================================"; \
	if [ $$fail_count -gt 0 ]; then exit 1; fi

test-automotive:
	@echo "Building and running automotive tests..."
	@$(MAKE) test-build
	@for test in $(TEST_AUTOMOTIVE_BIN); do \
		if [ -f "$$test" ]; then \
			echo "Running: $$test"; \
			$$test; \
		fi; \
	done

test-wireless:
	@echo "Building and running wireless tests..."
	@$(MAKE) test-build
	@for test in $(TEST_WIRELESS_BIN); do \
		if [ -f "$$test" ]; then \
			echo "Running: $$test"; \
			$$test; \
		fi; \
	done

test-crypto:
	@echo "Building and running crypto tests..."
	@$(MAKE) test-build
	@for test in $(TEST_CRYPTO_BIN); do \
		if [ -f "$$test" ]; then \
			echo "Running: $$test"; \
			$$test; \
		fi; \
	done

test-integration:
	@echo "Building and running integration tests..."
	@$(MAKE) test-build
	@for test in $(TEST_INTEGRATION_BIN); do \
		if [ -f "$$test" ]; then \
			echo "Running: $$test"; \
			$$test; \
		fi; \
	done

test-clean:
	@echo "Cleaning test artifacts..."
	rm -rf $(TEST_BUILD_DIR)
