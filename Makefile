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
.PHONY: all clean directories test info automotive wireless crypto

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

# Test target
test:
	@echo "MDK-Predator Test Suite"
	@echo "======================="
	@echo "Note: Tests require HackRF One with Mayhem-MDK hardware"
	@echo ""
	@if [ -d tests ]; then \
		echo "Test structure ready. Implement hardware tests as needed."; \
	else \
		echo "Error: tests/ directory not found"; \
		exit 1; \
	fi
