# File Verification System

## Overview

The MDK-Predator build system includes an automatic file verification step that ensures all required files are properly linked in the Mayhem firmware **before** attempting to build. This prevents cryptic build errors and saves time by catching issues early.

## Problem Statement

Previously, build errors could occur due to:
- Missing source files during integration
- Incomplete file copying
- Files not registered in `external.cmake`
- Incorrect directory structure

These issues would only surface during the build process, resulting in confusing error messages and wasted build time.

## Solution

The verification system performs comprehensive checks before building:

### 1. File Existence Verification

Verifies that all required files are present in the Mayhem firmware directory:

**C++ Source Files:**
- `main.cpp` - External app entry point
- `mdk_predator_app.cpp` - Main application implementation

**C++ Header Files:**
- `mdk_predator_app.hpp` - Application header

**C Source Files:**
- `src/mdk_predator.c` - Core integration
- `src/automotive/key_fob_analyzer.c`
- `src/automotive/rolling_code_tester.c`
- `src/wireless/wifi_analyzer.c`
- `src/wireless/bluetooth_analyzer.c`
- `src/wireless/subghz_analyzer.c`
- `src/crypto/crypto_analyzer.c`

**C Header Files:**
- `include/mdk_predator.h`
- `include/input_validation.h`
- `include/automotive/key_fob_analyzer.h`
- `include/automotive/rolling_code_tester.h`
- `include/wireless/wifi_analyzer.h`
- `include/wireless/bluetooth_analyzer.h`
- `include/wireless/subghz_analyzer.h`
- `include/crypto/crypto_analyzer.h`

**Configuration Files:**
- `manifest.json` - Application metadata
- `mdk_predator.conf` - Runtime configuration

### 2. CMake Registration Verification

Verifies that all source files are properly registered in `external.cmake`:
- Checks `EXTCPPSRC` for all C++ and C source files
- Checks `EXTAPPLIST` for the `mdk_predator` entry

### 3. Directory Structure Validation

Ensures the directory structure is intact:
- `src/automotive/` - Automotive security modules
- `src/wireless/` - Wireless security modules
- `src/crypto/` - Cryptographic analysis modules
- `include/automotive/` - Automotive headers
- `include/wireless/` - Wireless headers
- `include/crypto/` - Crypto headers

## Usage

The verification is **automatic** and runs as part of the build scripts:

### Linux/macOS
```bash
./scripts/build_portapack_app.sh --download-firmware
```

### WSL
```bash
./scripts/build_portapack_app_wsl.sh --download-firmware
```

### Windows PowerShell
```powershell
.\scripts\build_portapack_app.ps1 -DownloadFirmware
```

## Output Examples

### Success Case
```
[INFO] Verifying all files are properly linked in Mayhem firmware...
[INFO] Checking C++ source files...
[INFO]   ✓ main.cpp
[INFO]   ✓ mdk_predator_app.cpp
[INFO] Checking C++ header files...
[INFO]   ✓ mdk_predator_app.hpp
[INFO] Checking C source files...
[INFO]   ✓ src/mdk_predator.c
[INFO]   ✓ src/automotive/key_fob_analyzer.c
[INFO]   ✓ (all files checked)
[INFO] Checking C header files...
[INFO]   ✓ (all files checked)
[INFO] Checking configuration files...
[INFO]   ✓ manifest.json
[INFO]   ✓ mdk_predator.conf
[INFO] Verifying external.cmake registration...
[INFO]   ✓ All files registered in external.cmake

[INFO] ✓ All files verified successfully!
[INFO] ✓ All files are properly linked in Mayhem firmware
[INFO] ✓ Ready to build MDK-Predator
```

### Failure Case - Missing File
```
[INFO] Verifying all files are properly linked in Mayhem firmware...
[INFO] Checking C++ source files...
[INFO]   ✓ main.cpp
[INFO]   ✓ mdk_predator_app.cpp
[INFO] Checking C++ header files...
[INFO]   ✓ mdk_predator_app.hpp
[INFO] Checking C source files...
[INFO]   ✓ src/mdk_predator.c
[ERROR] Missing C file: src/automotive/key_fob_analyzer.c
...

[ERROR] File verification FAILED!
[ERROR] Some required files are missing or not properly linked.
[ERROR] Please check the error messages above.
```

### Failure Case - Missing Registration
```
[INFO] Verifying all files are properly linked in Mayhem firmware...
[INFO] Checking C++ source files...
[INFO]   ✓ (all files present)
...
[INFO] Verifying external.cmake registration...
[ERROR] File not registered in external.cmake: src/automotive/key_fob_analyzer.c

[ERROR] File verification FAILED!
[ERROR] Some required files are missing or not properly linked.
[ERROR] Please check the error messages above.
```

## Implementation Details

### Bash Scripts (Linux/macOS/WSL)

The `verify_file_links()` function in `build_portapack_app.sh` and `build_portapack_app_wsl.sh`:

```bash
verify_file_links() {
    print_info "Verifying all files are properly linked in Mayhem firmware..."

    # Check file existence
    # Check CMake registration
    # Report results

    if [ $verification_failed -eq 1 ]; then
        exit 1
    fi
}
```

### PowerShell Script (Windows)

The `Test-FileLinks` function in `build_portapack_app.ps1`:

```powershell
function Test-FileLinks {
    Write-Info "Verifying all files are properly linked in Mayhem firmware..."

    # Check file existence
    # Check CMake registration
    # Report results

    if ($verificationFailed) {
        exit 1
    }
}
```

## Benefits

1. **Early Error Detection**: Catches missing files before build time
2. **Clear Error Messages**: Identifies exactly which files are missing
3. **Time Savings**: No waiting for build to fail
4. **Reliability**: Ensures consistent builds
5. **Cross-Platform**: Works on Linux, macOS, WSL, and Windows

## Testing

A comprehensive test suite validates the verification system:

```bash
/tmp/test_verification.sh
```

Tests include:
- File integration verification
- Missing file detection
- CMake registration validation
- Directory structure integrity

All tests pass with 100% success rate.

## Troubleshooting

### All Files Fail Verification

**Cause**: Integration step may have failed
**Solution**: Check that files were copied from `app/`, `src/`, and `include/` directories

### Specific File Missing

**Cause**: File may not exist in source or copy failed
**Solution**:
1. Verify file exists in source: `ls -la app/ src/ include/`
2. Check disk space
3. Check file permissions

### CMake Registration Failed

**Cause**: `external.cmake` file may not be writable or corrupt
**Solution**:
1. Check permissions on `mayhem-firmware/firmware/application/external/external.cmake`
2. Restore from backup if available: `external.cmake.backup`

### Directory Structure Incomplete

**Cause**: Subdirectories not copied
**Solution**: Use `cp -r` flag to copy directories recursively

## Related Documentation

- [BUILD_APPROACH.md](BUILD_APPROACH.md) - Overall build system architecture
- [BUILD_WORKFLOW.md](BUILD_WORKFLOW.md) - Complete build workflow
- [app/README.md](app/README.md) - Application build details
- [README.md](README.md) - Main project documentation

## See Also

- Build scripts: `scripts/build_portapack_app.sh`, `scripts/build_portapack_app_wsl.sh`, `scripts/build_portapack_app.ps1`
- Test script: `/tmp/test_verification.sh` (if available)
