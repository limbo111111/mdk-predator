# MDK-Predator AI Coding Agent Instructions

## Project Overview

MDK-Predator is a **security research suite** ported from Flipper Zero to the Mayhem-MDK platform (HackRF Portapack H4M). This is a **PortaPack external application** (.ppma) that runs on top of Mayhem firmware, NOT a standalone firmware or library project.

**Critical Context:**
- **Target Hardware**: HackRF One + Mayhem-MDK module (I2CDECMDL_PPMOD device model)
- **Platform**: ARM Cortex-M4 with FPU
- **Build Output**: `mdk_predator.ppma` file (deployed to SD card `/APPS/` directory)
- **Architecture**: Dual-layer - C core modules + C++ PortaPack UI wrapper
- **Purpose**: Production-ready RF security research (automotive, WiFi, Bluetooth, SubGHz, crypto analysis)

## Architecture: The "Two-Layer" Pattern

This codebase uses a **unique two-layer architecture** that's critical to understand:

### Layer 1: Core Security Modules (Pure C)
Located in `src/` and `include/`:
- **C11 standard** with ARM Cortex-M4 optimizations
- Hardware-agnostic analysis logic (automotive, wireless, crypto)
- Designed for potential reuse in other embedded platforms
- **No PortaPack dependencies** - uses generic types and interfaces

### Layer 2: PortaPack Application Wrapper (C++)
Located in `app/`:
- **C++17** PortaPack Mayhem UI integration
- `main.cpp`: Entry point with `application_information_t` metadata
- `mdk_predator_app.cpp/hpp`: View classes, UI widgets, event handlers
- **Calls Layer 1** via `extern "C"` includes
- **Never mix**: Keep C and C++ strictly separated by layer

**Why This Matters:** When adding features:
- Logic goes in C modules (`src/`) 
- UI goes in C++ app wrapper (`app/`)
- Never put business logic in UI code

## Build System: Multi-Mode Build Strategy

### 3 Distinct Build Modes (Choose Wisely!)

1. **Docker Build** (Recommended for development/CI)
   ```bash
   ./docker-build.sh  # Cross-platform, no dependencies
   ```
   - Output: `build/docker-output/mdk_predator.ppma`
   - Uses `docker-compose.yml` with mayhem-firmware container
   - Zero local toolchain setup required

2. **Native Build** (Fastest for iteration)
   ```bash
   ./build.sh -d  # Downloads mayhem-firmware, builds app
   ```
   - Output: `build/output/mdk_predator.ppma`
   - **Critical Steps** (automated by script):
     1. Copy `app/`, `src/`, `include/` to `mayhem-firmware/firmware/application/external/mdk_predator/`
     2. Register in `external.cmake` (both EXTCPPSRC and EXTAPPLIST)
     3. Build libopencm3 FIRST: `make TARGETS=lpc43xx` 
     4. Build with `make application` (NOT `make firmware`)
   - Requires: arm-none-eabi-gcc, CMake, Python 3

3. **Standalone Library Build** (For testing only)
   ```bash
   make  # Builds libmdk_predator.a (not for deployment)
   ```
   - Output: `build/lib/libmdk_predator.a`
   - **NOT used for PortaPack** - this is for unit testing
   - Uses native GCC, not ARM cross-compiler for tests

**Common Mistake:** Don't confuse the Makefile (library build) with the actual deployment build (via build.sh/docker-build.sh).

## File Registration Pattern

When adding new modules, **registration happens in 3 places**:

1. **`app/app_build.mk`** (PortaPack build system)
   ```makefile
   app_mdk_predator_SOURCES = \
       mdk_predator_app.cpp \
       src/new_module/new_file.c
   ```

2. **`build.sh`** (external.cmake registration)
   ```bash
   # Added to EXTCPPSRC in external.cmake
   external/mdk_predator/src/new_module/new_file.c
   ```

3. **`Makefile`** (library build for tests)
   ```makefile
   NEW_MODULE_SRC = $(wildcard $(SRC_DIR)/new_module/*.c)
   ```

## MDK Hardware Interface: The Hardware Abstraction Layer

`include/mdk_hardware_interface.h` provides comprehensive hardware access:

### I2C Communication (ESP32-S3 Compatible)
```c
// Device model: I2CDECMDL_PPMOD
mdk_i2c_init_i2cdecmdl();  // Detects MDK module via I2C
// Returns false if module not present (non-fatal - fallback to software mode)
```

### DMA and Parallel Streams (Hardware Acceleration)
```c
mdk_dma_config_t dma_config = {
    .channel = MDK_DMA_CHANNEL_0,
    .direction = MDK_DMA_PERIPH_TO_MEM,
    .circular_mode = true  // For continuous signal capture
};
```

**Auto-Detection Pattern** (see `src/mdk_predator.c`):
- Try I2C detection: `mdk_detect_i2c_module()`
- If detected: Enable hardware acceleration + parallel streams
- If not detected: Fallback to software-only mode (single stream)
- **Non-blocking**: Missing hardware should degrade gracefully, not fail

## Module Development Patterns

### Standard Module Structure
Every security module follows this pattern:

```c
// include/<category>/<module>.h
typedef struct {
    uint32_t frequency;
    uint32_t sample_rate;
    // ... module-specific config
} module_config_t;

bool module_init(module_config_t *config);
bool module_analyze(input_data_t *input, result_t *output);
void module_cleanup(module_config_t *config);
```

**Key Conventions:**
- **Always NULL-check** pointers before use
- Return `bool` for success/failure (don't use exceptions - this is C)
- Use `typedef struct` with `_t` suffix
- Cleanup functions are `void` (always run, never fail)

### Configuration Loading Pattern
See `mdk_predator.conf` and `src/mdk_predator.c`:
```c
// Config flows from .conf → mdk_predator_config_t → module configs
config.hardware.hackrf_sample_rate = 2000000;  // From .conf [Hardware]
config.security.allow_transmit = false;         // CRITICAL: Default false
```

**Security-First**: Transmit always disabled by default, requires explicit user confirmation.

## Testing Strategy

### Unit Tests (No Hardware Required)
```bash
make test  # Runs all 8 test suites, 217 assertions
```

**Test Compilation Pattern** (from Makefile):
- Tests compile with **native GCC** (not ARM cross-compiler)
- Each test suite is a standalone executable
- Tests are in `tests/<category>/test_<module>.c`
- Test pattern:
  ```c
  int main() {
      test_count++; passed_count++;  // Manual assertion tracking
      printf("✓ Test passed\n");
      return (failed_count == 0) ? 0 : 1;
  }
  ```

**No Test Framework**: Plain C with manual pass/fail counting (embedded-friendly).

## PortaPack UI Integration

### External App Metadata (main.cpp)
```cpp
application_information_t _application_information_mdk_predator = {
    .app_name = "MDK-Predator",
    .bitmap_data = { /* 16x16 icon */ },
    .icon_color = ui::Color::red().v,
    .menu_location = app_location_t::UTILITIES,
    .m4_app_tag = {'P', 'M', 'D', 'P'}  // Unique 4-char tag
};
```

### UI View Pattern (mdk_predator_app.cpp)
```cpp
class MDKPredatorView : public View {
    // Add widgets to children list in constructor
    add_children({&button_automotive, &text_status, &console});
    
    // Call C modules via extern "C" includes
    extern "C" { #include "mdk_predator.h" }
    mdk_predator_init(&config);
};
```

**Critical UI Pattern**: All business logic stays in C modules. UI only handles:
- Widget layout and events
- Calling C functions
- Displaying results

## Documentation Standards

When documenting:
- **README.md**: User-facing deployment guide (keep simple)
- **docs/ARCHITECTURE.md**: Deep technical architecture
- **docs/API.md**: Function reference with examples
- **BUILD.md**: Build process details (3 build modes)
- **Inline comments**: Implementation details and "why" decisions

**Multi-doc philosophy**: Different docs for different audiences (users vs developers vs architects).

## Common Pitfalls to Avoid

1. **Don't build full firmware**: Use `make application`, not `make firmware`
2. **Don't skip libopencm3**: Must build before main application
3. **Don't mix C/C++**: Keep Layer 1 (C) and Layer 2 (C++) strictly separated
4. **Don't forget registration**: New files need 3-place registration
5. **Don't assume hardware**: Always check MDK module detection, fallback gracefully
6. **Don't enable transmit**: Security-critical - always default false

## Quick Command Reference

```bash
# Development
./docker-build.sh --clean      # Full clean build
./build.sh -d                  # Native build with firmware download
make test                      # Run all unit tests

# Deployment
cp build/docker-output/mdk_predator.ppma /path/to/sdcard/APPS/

# Debugging
./docker-build.sh --shell      # Interactive container shell
make test-automotive           # Run specific test suite
```

## When Modifying Build Scripts

Build scripts (`build.sh`, `docker-build.sh`, `build.ps1`) follow this sequence:
1. Download/verify mayhem-firmware
2. Copy app files to `firmware/application/external/mdk_predator/`
3. Register in `external.cmake` (check if already registered!)
4. Build libopencm3 (`make TARGETS=lpc43xx`)
5. CMake configure and build (`make application`)
6. Extract `.ppma` from build directory

**Idempotency**: Scripts check existing registration before modifying `external.cmake`.

## Code Style Snapshot

```c
// Naming: snake_case for C, CamelCase for C++
typedef struct {
    uint32_t frequency;  // Explicit types (uint32_t, not int)
    bool is_valid;       // Use bool (stdbool.h)
} module_config_t;      // _t suffix for types

bool module_init(module_config_t *config) {
    if (!config) {  // Validate all inputs
        return false;
    }
    // Implementation
    return true;  // Explicit success indication
}
```

**Standards**: C11 for modules, C++17 for UI, ARM Cortex-M4 optimizations enabled.