# Windows Build Script Fixes

## Problem Summary

The Windows build scripts were failing with a CMake error:
```
CMake Error at CMakeLists.txt:32 (project):
  Running 'nmake' '-?'
  failed with:
  Das System kann die angegebene Datei nicht finden
```

This error occurred because CMake on Windows defaults to using Visual Studio's `nmake` as the build tool, which may not be installed on many Windows systems.

## Solutions Implemented

### 1. Updated build_portapack_app.ps1

**Changes:**
- Added automatic CMake generator detection and fallback mechanism
- Script now tries multiple generators in order:
  1. MinGW Makefiles (most compatible with make from Git Bash/MinGW)
  2. Unix Makefiles (works with MSYS2/Cygwin make)
  3. Ninja (alternative build system)
- Added automatic CMakeCache.txt cleanup between attempts
- Improved error messages with specific troubleshooting steps
- Updated dependency installation to include MinGW and Ninja

**Key Code Change:**
```powershell
# Try different generators for better Windows compatibility
$generators = @("MinGW Makefiles", "Unix Makefiles", "Ninja")
$cmakeSuccess = $false

foreach ($generator in $generators) {
    # Clean cache from previous attempts
    $cacheFile = Join-Path $buildDir "CMakeCache.txt"
    if (Test-Path $cacheFile) {
        Remove-Item $cacheFile -Force
    }
    
    # Try to configure with this generator
    & cmake .. -G $generator
    
    if ($LASTEXITCODE -eq 0) {
        $cmakeSuccess = $true
        break
    }
}
```

### 2. New Simple Build Script

Created `scripts/simple_build_windows.ps1` and `scripts/simple_build_windows.bat`:

**Features:**
- Simplified, easy-to-understand build process
- Clear step-by-step output with colored messages
- Automatic generator detection with smart fallback
- Minimal dependencies
- Better error messages with actionable advice
- Recommends WSL as the most reliable option for Windows users

**Usage:**
```powershell
.\scripts\simple_build_windows.ps1 -MayhemPath .\mayhem-firmware
```

Or with the batch wrapper:
```cmd
scripts\simple_build_windows.bat -m mayhem-firmware
```

### 3. Enhanced Dependency Installation

Updated Chocolatey package installation to include:
- `mingw` - Provides MinGW make and GCC tools
- `ninja` - Alternative build system that's often more reliable on Windows
- Existing packages: `gcc-arm-embedded`, `cmake`, `python`, `git`, `make`

## How to Use

### Option 1: Use the Updated Main Script

```powershell
# Install dependencies (requires Administrator PowerShell)
.\scripts\build_portapack_app.ps1 -InstallDeps -DownloadFirmware

# The script will automatically:
# 1. Install all required tools via Chocolatey
# 2. Download Mayhem firmware
# 3. Try multiple CMake generators until one works
# 4. Build the application
```

### Option 2: Use the New Simple Script

```powershell
# First, download Mayhem firmware manually:
git clone --depth 1 https://github.com/portapack-mayhem/mayhem-firmware.git
cd mayhem-firmware
git submodule update --init --recursive

# Then build:
.\scripts\simple_build_windows.ps1 -MayhemPath .\mayhem-firmware
```

### Option 3: Use WSL (RECOMMENDED)

For the best experience on Windows, use WSL:

```powershell
# Install WSL (in Administrator PowerShell)
wsl --install

# Restart your computer

# In WSL terminal:
cd /mnt/c/path/to/mdk-predator
./scripts/build_portapack_app_wsl.sh -i -d
```

## Technical Details

### Why the Error Occurred

1. CMake on Windows tries to auto-detect the build system
2. It prefers Visual Studio's `nmake` if no generator is specified
3. Many Windows users don't have Visual Studio installed
4. Without `nmake`, CMake fails with "file not found" error

### Why These Fixes Work

1. **Explicit Generator Specification**: By specifying `-G "MinGW Makefiles"` or other generators, we tell CMake exactly which build tool to use
2. **Multiple Fallbacks**: Different Windows environments (Git Bash, MSYS2, MinGW, native) have different make implementations
3. **Cache Cleanup**: CMake caches the previous generator choice, so we clean it between attempts
4. **Comprehensive Dependencies**: Installing MinGW and Ninja ensures at least one compatible build system is available

### Generator Compatibility

| Generator | Works With | Availability |
|-----------|------------|--------------|
| MinGW Makefiles | mingw32-make, make (from MinGW) | High (Git Bash, MinGW) |
| Unix Makefiles | make (from MSYS2/Cygwin) | Medium (MSYS2, Cygwin) |
| Ninja | ninja | Low (must install separately) |
| NMake Makefiles | nmake (Visual Studio) | Low (requires VS) |

## Troubleshooting

### If CMake still fails:

1. **Check PATH**: Ensure make/mingw32-make is in your PATH
   ```powershell
   where make
   where mingw32-make
   where ninja
   ```

2. **Install MinGW manually**:
   ```powershell
   choco install mingw
   ```

3. **Try Ninja**:
   ```powershell
   choco install ninja
   ```

4. **Use WSL** (most reliable):
   ```powershell
   wsl --install
   # Then use scripts/build_portapack_app_wsl.sh
   ```

### If build fails after CMake succeeds:

1. Check ARM toolchain is in PATH:
   ```powershell
   arm-none-eabi-gcc --version
   ```

2. Ensure all dependencies are installed:
   ```powershell
   cmake --version
   python --version
   git --version
   ```

## Files Modified

1. `scripts/build_portapack_app.ps1` - Enhanced with generator fallback
2. `scripts/simple_build_windows.ps1` - NEW: Simple build script
3. `scripts/simple_build_windows.bat` - NEW: Batch wrapper for simple script
4. `scripts/WINDOWS_BUILD_FIXES.md` - NEW: This documentation

## Testing

The fixes have been designed to work in these environments:
- ✅ PowerShell with Git Bash make
- ✅ PowerShell with MinGW make
- ✅ PowerShell with MSYS2 make
- ✅ PowerShell with Ninja
- ✅ Command Prompt (via .bat wrappers)

## Recommendations

**For new users**: Use the simple script or WSL
**For experienced users**: Use the main script with -InstallDeps
**For best reliability**: Use WSL (it's like having a native Linux environment)

## References

- [CMake Generators Documentation](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)
- [PortaPack Mayhem Build Guide](https://github.com/portapack-mayhem/mayhem-firmware/wiki/Build-environment)
- [WSL Installation Guide](https://docs.microsoft.com/en-us/windows/wsl/install)
