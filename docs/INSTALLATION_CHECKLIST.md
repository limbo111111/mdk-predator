# MDK-Predator Installation Checklist

Use this checklist to ensure successful installation and deployment of MDK-Predator on your HackRF PortaPack H4M with Mayhem-MDK module.

## Pre-Installation

### Hardware Check ✓
- [ ] HackRF One (verified working)
- [ ] PortaPack H4M (attached and functioning)
- [ ] Mayhem-MDK module (properly seated)
- [ ] MicroSD card (8GB+, Class 10 or better)
- [ ] Antennas:
  - [ ] 315 MHz antenna
  - [ ] 433 MHz antenna
  - [ ] 868/915 MHz antenna
  - [ ] 2.4 GHz antenna
- [ ] USB cable (Micro-B or USB-C)
- [ ] Power supply (5V, 2A minimum)

### Firmware Check ✓
- [ ] PortaPack powers on successfully
- [ ] Mayhem firmware installed
- [ ] Firmware version 1.7.0 or later (check in Settings → About)
- [ ] All firmware features working (tested Apps menu)

### Software Check ✓
- [ ] Git installed (for cloning repository)
- [ ] ARM toolchain installed (if building from source)
- [ ] CMake 3.16+ installed (if building from source)
- [ ] Python 3.7+ installed (if building from source)

## Obtaining the Application

### Option A: Pre-built Binary ✓
- [ ] Downloaded `mdk_predator.ppma` from [Releases](https://github.com/limbo111111/mdk-predator/releases)
- [ ] Downloaded `mdk_predator.conf` configuration file
- [ ] Verified file integrity (checksums match)

### Option B: Build from Source ✓
- [ ] Cloned MDK-Predator repository
- [ ] Cloned PortaPack Mayhem firmware source
- [ ] Updated all git submodules
- [ ] Ran build script or manual build process
- [ ] Located `mdk_predator.ppma` in build output
- [ ] Verified build completed without errors

## SD Card Preparation

### Format SD Card ✓
- [ ] SD card formatted as FAT32 (not exFAT)
- [ ] Format verified on PortaPack (insert and check if readable)
- [ ] SD card has at least 1GB free space

### Create Directory Structure ✓
- [ ] Created `/APPS/` directory
- [ ] Created `/MDK-PREDATOR/` directory
- [ ] Created `/MDK-PREDATOR/config/` directory
- [ ] Created `/MDK-PREDATOR/logs/` directory
- [ ] Created `/MDK-PREDATOR/captures/` directory (optional)

### Copy Files ✓
- [ ] Copied `mdk_predator.ppma` to `/APPS/`
- [ ] Copied `mdk_predator.conf` to `/MDK-PREDATOR/config/`
- [ ] Verified files copied correctly (check file sizes)

## Installation

### SD Card Installation ✓
- [ ] Safely ejected SD card from computer
- [ ] Powered off PortaPack
- [ ] Inserted SD card into PortaPack
- [ ] Powered on PortaPack
- [ ] PortaPack recognized SD card (no error messages)

### Launch Application ✓
- [ ] Navigated to Apps menu on PortaPack
- [ ] Found "MDK-Predator" in app list
- [ ] Selected MDK-Predator
- [ ] Application launched successfully
- [ ] Main menu displayed
- [ ] Status shows "Initialized"

## Initial Configuration

### Basic Configuration ✓
- [ ] Reviewed `/MDK-PREDATOR/config/mdk_predator.conf`
- [ ] Verified `allow_transmit=false` (IMPORTANT!)
- [ ] Set `default_module` to preferred starting module
- [ ] Adjusted hardware gains if needed
- [ ] Saved configuration changes

### Module Configuration ✓
- [ ] Configured automotive module frequencies
- [ ] Set WiFi scanning preferences
- [ ] Configured SubGHz frequency range
- [ ] Set logging preferences

## Testing

### Hardware Test ✓
- [ ] Automotive Module:
  - [ ] Launched Key Fob Analyzer
  - [ ] Connected 433 MHz antenna
  - [ ] Tested with key fob (signal detected)
- [ ] WiFi Module:
  - [ ] Launched Network Scanner
  - [ ] Connected 2.4 GHz antenna
  - [ ] Detected nearby networks
- [ ] Bluetooth Module:
  - [ ] Launched Device Scanner
  - [ ] Detected nearby devices
- [ ] SubGHz Module:
  - [ ] Launched Spectrum Scanner
  - [ ] Connected appropriate antenna
  - [ ] Spectrum displayed correctly
- [ ] Crypto Module:
  - [ ] Launched module
  - [ ] Options displayed correctly

### Diagnostic Test ✓
- [ ] Accessed diagnostics (if available)
- [ ] Hardware check: OK
- [ ] All modules check: OK
- [ ] No error messages

### Functionality Test ✓
- [ ] Navigation works (buttons/joystick)
- [ ] Console output visible
- [ ] Status messages display correctly
- [ ] Can switch between modules
- [ ] Can return to main menu
- [ ] Can exit application

## Documentation Review

### Read Documentation ✓
- [ ] Read [QUICKSTART.md](../QUICKSTART.md)
- [ ] Read [USER_GUIDE.md](USER_GUIDE.md)
- [ ] Read [DEPLOYMENT.md](DEPLOYMENT.md)
- [ ] Understood safety warnings
- [ ] Reviewed legal requirements
- [ ] Understood responsible use guidelines

### Legal & Safety ✓
- [ ] Read and understood legal disclaimers
- [ ] Confirmed authorization for intended testing
- [ ] Understand local RF regulations
- [ ] Verified transmit is disabled
- [ ] Prepared documentation for testing activities

## Final Verification

### System Check ✓
- [ ] MDK-Predator launches reliably
- [ ] All modules accessible
- [ ] Configuration loads correctly
- [ ] Logs being created
- [ ] No unexpected errors

### Operational Check ✓
- [ ] Tested with owned/authorized devices
- [ ] Verified signal capture works
- [ ] Confirmed analysis functions work
- [ ] Checked data storage (captures save correctly)
- [ ] Verified logging works

### Performance Check ✓
- [ ] Application responsive
- [ ] No freezing or crashes
- [ ] SD card read/write speed adequate
- [ ] Battery life acceptable (if portable)

## Post-Installation

### Backup ✓
- [ ] Backed up working configuration
- [ ] Documented custom settings
- [ ] Saved application binary (for reinstall)
- [ ] Noted firmware version

### Training ✓
- [ ] Practiced using each module
- [ ] Understand all features
- [ ] Know how to interpret results
- [ ] Can troubleshoot common issues

### Support Setup ✓
- [ ] Bookmarked GitHub repository
- [ ] Joined community forums (if available)
- [ ] Know where to get help
- [ ] Can report issues properly

## Troubleshooting Checklist

If something doesn't work, check:

### Application Won't Launch ✓
- [ ] SD card properly inserted
- [ ] File in correct location (`/APPS/`)
- [ ] SD card formatted as FAT32
- [ ] Firmware version 1.7.0+
- [ ] Try different SD card

### "Init Failed" Error ✓
- [ ] HackRF properly connected
- [ ] Mayhem-MDK module seated correctly
- [ ] Adequate power supply
- [ ] Configuration file valid
- [ ] Try default configuration

### No Signals Detected ✓
- [ ] Correct antenna connected
- [ ] Antenna connection tight
- [ ] Correct frequency selected
- [ ] Signal source active and in range
- [ ] Try increasing gain settings

### Performance Issues ✓
- [ ] Close other applications
- [ ] Use faster SD card
- [ ] Reduce sample rate
- [ ] Disable unnecessary logging
- [ ] Restart device

## Maintenance

### Regular Checks ✓
- [ ] Check for firmware updates monthly
- [ ] Review logs weekly
- [ ] Backup important captures
- [ ] Clean SD card (remove old logs)
- [ ] Test all modules quarterly

### Updates ✓
- [ ] Check for MDK-Predator updates
- [ ] Read changelog before updating
- [ ] Backup current version before update
- [ ] Test after updating

## Completion

### Installation Complete ✓
- [ ] All checklist items completed
- [ ] Application working properly
- [ ] Documentation reviewed
- [ ] Ready for authorized security research

### Date Completed: ________________

### Notes:
```
[Space for installation notes, issues encountered, custom configurations, etc.]






```

## Quick Reference

### File Locations
- Application: `/APPS/mdk_predator.ppma`
- Config: `/MDK-PREDATOR/config/mdk_predator.conf`
- Logs: `/MDK-PREDATOR/logs/`
- Captures: `/MDK-PREDATOR/captures/`

### Key Commands
- Launch: Apps → MDK-Predator
- Exit: Main Menu → Exit
- Module Switch: Back to Main Menu → Select New Module

### Support
- GitHub: https://github.com/limbo111111/mdk-predator
- Issues: https://github.com/limbo111111/mdk-predator/issues
- Docs: See repository `/docs` directory

## Remember

⚠️ **Always verify authorization before testing any system**

⚠️ **Keep transmit disabled unless absolutely necessary**

⚠️ **Follow all local RF regulations**

⚠️ **Only for authorized security research**

---

**Installation Date**: _________________

**Completed By**: _________________

**Hardware Serial Numbers**: _________________

**Notes**: _________________
