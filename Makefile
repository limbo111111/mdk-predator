# MDK-Predator Makefile
# Build system for Mayhem-MDK Predator Security Suite

# Compiler and flags
CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -O2 -std=c11
INCLUDES = -Iinclude
LDFLAGS = -lm

# Directories
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
LIB_DIR = $(BUILD_DIR)/lib

# Source files
APPLICATION_SRC_DIR = $(SRC_DIR)/application
HAL_SRC_DIR = $(SRC_DIR)/hal

AUTOMOTIVE_SRC = $(wildcard $(APPLICATION_SRC_DIR)/automotive/*.c)
WIRELESS_SRC = $(wildcard $(APPLICATION_SRC_DIR)/wireless/*.c)
CRYPTO_SRC = $(wildcard $(APPLICATION_SRC_DIR)/crypto/*.c)
HARDWARE_SRC = $(wildcard $(HAL_SRC_DIR)/*.c)
MAIN_SRC = $(APPLICATION_SRC_DIR)/mdk_predator.c

ALL_SRC = $(AUTOMOTIVE_SRC) $(WIRELESS_SRC) $(CRYPTO_SRC) $(HARDWARE_SRC) $(MAIN_SRC)

# Object files
AUTOMOTIVE_OBJ = $(patsubst $(APPLICATION_SRC_DIR)/%.c,$(OBJ_DIR)/application/%.o,$(AUTOMOTIVE_SRC))
WIRELESS_OBJ = $(patsubst $(APPLICATION_SRC_DIR)/%.c,$(OBJ_DIR)/application/%.o,$(WIRELESS_SRC))
CRYPTO_OBJ = $(patsubst $(APPLICATION_SRC_DIR)/%.c,$(OBJ_DIR)/application/%.o,$(CRYPTO_SRC))
HARDWARE_OBJ = $(patsubst $(HAL_SRC_DIR)/%.c,$(OBJ_DIR)/hal/%.o,$(HARDWARE_SRC))
MAIN_OBJ = $(patsubst $(APPLICATION_SRC_DIR)/%.c,$(OBJ_DIR)/application/%.o,$(MAIN_SRC))

ALL_OBJ = $(AUTOMOTIVE_OBJ) $(WIRELESS_OBJ) $(CRYPTO_OBJ) $(HARDWARE_OBJ) $(MAIN_OBJ)

# Output library
LIB_NAME = libmdk_predator.a
LIB_PATH = $(LIB_DIR)/$(LIB_NAME)

# Targets
.PHONY: all clean directories test test-build test-run test-automotive test-wireless test-crypto test-integration test-clean test-directories info automotive wireless crypto

all: directories $(LIB_PATH)

directories:
	@mkdir -p $(OBJ_DIR)/application/automotive
	@mkdir -p $(OBJ_DIR)/application/wireless
	@mkdir -p $(OBJ_DIR)/application/crypto
	@mkdir -p $(OBJ_DIR)/hal
	@mkdir -p $(LIB_DIR)

$(LIB_PATH): $(ALL_OBJ)
	@echo "Creating library: $@"
	$(AR) rcs $@ $^

$(OBJ_DIR)/application/%.o: $(APPLICATION_SRC_DIR)/%.c
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/hal/%.o: $(HAL_SRC_DIR)/%.c
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
TEST_CFLAGS = -Wall -Wextra -O2 -std=c11 --coverage
TEST_INCLUDES = -Iinclude
TEST_DIR = tests
TEST_BUILD_DIR = $(BUILD_DIR)/tests
TEST_BIN_DIR = $(TEST_BUILD_DIR)/bin

# Test source files
TEST_AUTOMOTIVE_SRC = $(wildcard $(TEST_DIR)/automotive/*.c)
TEST_WIRELESS_SRC = $(wildcard $(TEST_DIR)/wireless/*.c)
TEST_CRYPTO_SRC = $(wildcard $(TEST_DIR)/crypto/*.c)
TEST_HARDWARE_SRC = $(wildcard $(TEST_DIR)/hardware/*.c)
TEST_INTEGRATION_SRC = $(wildcard $(TEST_DIR)/integration/*.c)

ALL_TEST_SRC = $(TEST_AUTOMOTIVE_SRC) $(TEST_WIRELESS_SRC) $(TEST_CRYPTO_SRC) $(TEST_HARDWARE_SRC) $(TEST_INTEGRATION_SRC)

# Test objects
TEST_OBJ_DIR = $(TEST_BUILD_DIR)/obj
TEST_AUTOMOTIVE_OBJ = $(patsubst $(TEST_DIR)/%.c,$(TEST_OBJ_DIR)/%.o,$(TEST_AUTOMOTIVE_SRC))
TEST_WIRELESS_OBJ = $(patsubst $(TEST_DIR)/%.c,$(TEST_OBJ_DIR)/%.o,$(TEST_WIRELESS_SRC))
TEST_CRYPTO_OBJ = $(patsubst $(TEST_DIR)/%.c,$(TEST_OBJ_DIR)/%.o,$(TEST_CRYPTO_SRC))
TEST_HARDWARE_OBJ = $(patsubst $(TEST_DIR)/%.c,$(TEST_OBJ_DIR)/%.o,$(TEST_HARDWARE_SRC))
TEST_INTEGRATION_OBJ = $(patsubst $(TEST_DIR)/%.c,$(TEST_OBJ_DIR)/%.o,$(TEST_INTEGRATION_SRC))

ALL_TEST_OBJ = $(TEST_AUTOMOTIVE_OBJ) $(TEST_WIRELESS_OBJ) $(TEST_CRYPTO_OBJ) $(TEST_HARDWARE_OBJ) $(TEST_INTEGRATION_OBJ)

# Test executable
TEST_EXEC = $(TEST_BIN_DIR)/mdk_predator_test

# Test targets
.PHONY: test test-build test-run test-automotive test-wireless test-crypto test-hardware test-integration test-clean coverage

test: test-build test-run

test-build: directories test-directories $(TEST_EXEC)

test-directories:
	@mkdir -p $(TEST_OBJ_DIR)/automotive
	@mkdir -p $(TEST_OBJ_DIR)/wireless
	@mkdir -p $(TEST_OBJ_DIR)/crypto
	@mkdir -p $(TEST_OBJ_DIR)/hardware
	@mkdir -p $(TEST_OBJ_DIR)/integration
	@mkdir -p $(TEST_BIN_DIR)

$(TEST_EXEC): $(ALL_TEST_OBJ) $(ALL_OBJ) $(TEST_DIR)/test-main.c
	@echo "Building test executable: $@"
	$(TEST_CC) $(TEST_CFLAGS) $(TEST_INCLUDES) $(ALL_TEST_OBJ) $(ALL_OBJ) $(TEST_DIR)/test-main.c -lm -o $@

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.c
	@echo "Compiling test: $<"
	$(TEST_CC) $(TEST_CFLAGS) $(TEST_INCLUDES) -c $< -o $@

test-run: test-build
	@echo ""
	@echo "========================================="
	@echo "Running MDK-Predator Test Suite"
	@echo "========================================="
	@echo ""
	@$(TEST_EXEC)

coverage: test-run
	@echo ""
	@echo "========================================="
	@echo "Generating Code Coverage Report"
	@echo "========================================="
	@echo ""
	@gcov -b -c $(ALL_SRC) > coverage.txt

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

test-hardware:
	@echo "Building and running hardware tests..."
	@$(MAKE) test-build
	@for test in $(TEST_HARDWARE_BIN); do \
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
