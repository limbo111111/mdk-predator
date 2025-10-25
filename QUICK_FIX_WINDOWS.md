# Quick Fix for Windows CMake/nmake Error

## Error You're Seeing

```
CMake Error at CMakeLists.txt:32 (project):
  Running 'nmake' '-?'
  failed with:
  Das System kann die angegebene Datei nicht finden
```

## Quick Solutions (Pick One)

### Option 1: Use the Fixed Script (EASIEST)

The build script has been updated to automatically handle this issue:

```powershell
# Run in PowerShell as Administrator
.\scripts\build_portapack_app.ps1 -InstallDeps -DownloadFirmware
```

This will:
- Install all required tools (including MinGW and Ninja)
- Automatically try multiple CMake generators until one works
- Build your application

### Option 2: Use the Simple Script

If you already have dependencies installed:

```powershell
# First download Mayhem firmware if you haven't:
git clone --depth 1 https://github.com/portapack-mayhem/mayhem-firmware.git

# Then build:
.\scripts\simple_build_windows.ps1 -MayhemPath .\mayhem-firmware
```

### Option 3: Use WSL (RECOMMENDED)

Most reliable option for Windows:

```powershell
# In PowerShell as Administrator
wsl --install

# After restart, in WSL terminal:
cd /mnt/c/path/to/mdk-predator
./scripts/build_portapack_app_wsl.sh -i -d
```

### Option 4: Manual Fix

If you want to fix it manually:

1. Install MinGW:
   ```powershell
   choco install mingw
   ```

2. Run CMake with explicit generator:
   ```powershell
   cd mayhem-firmware\build
   cmake .. -G "MinGW Makefiles"
   make external_apps
   ```

## Why This Error Happens

CMake on Windows tries to use `nmake` (from Visual Studio) by default, but most users don't have it installed. The fix tells CMake to use MinGW's `make` instead.

## What's Been Fixed

The scripts now:
1. ✅ Try multiple CMake generators automatically
2. ✅ Install MinGW and Ninja during dependency installation
3. ✅ Clean CMakeCache between attempts
4. ✅ Provide clear error messages with next steps

## Still Having Issues?

See [scripts/WINDOWS_BUILD_FIXES.md](scripts/WINDOWS_BUILD_FIXES.md) for comprehensive troubleshooting.

## Files Updated

- `scripts/build_portapack_app.ps1` - Now tries multiple generators
- `scripts/simple_build_windows.ps1` - NEW: Simpler build process
- `scripts/simple_build_windows.bat` - NEW: Batch wrapper
- `scripts/WINDOWS_BUILD_FIXES.md` - Full documentation
