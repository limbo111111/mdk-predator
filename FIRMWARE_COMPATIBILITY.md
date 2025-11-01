# Mayhem Firmware Compatibility Analysis

**Date:** November 1, 2025
**Mayhem Firmware Version Analyzed:** Latest (commit 00c9c6b)
**MDK-Predator App Version:** Current

## Executive Summary

✅ **MDK-Predator app is COMPATIBLE with the latest Mayhem firmware.**

The external app API structure matches the current firmware expectations, and no breaking changes have been introduced that would affect the MDK-Predator application.

## Recent Mayhem Firmware Changes

### Recent Commits (Last 50)
- Most recent changes focus on UI improvements, new apps, and bug fixes
- No breaking changes to the external app API
- External app header version remains at: `0x00000003`

### Key Updates That Could Affect External Apps

#### 1. Multi-Screen Support (Oct 3, 2025 - commit 371b6b5)
- **Impact:** LOW - UI layout improvements for different screen sizes
- **Action Required:** None immediately, but consider dynamic screen sizing in future updates
- **Details:** Apps can now adapt to different screen dimensions (standard PortaPack vs PortaRF)

#### 2. Standalone App API v3 & v4 (Sept 2025)
- **Impact:** NONE - Only affects standalone apps, not external apps like MDK-Predator
- **Details:** New file I/O capabilities for standalone apps (separate from external apps)

#### 3. Memory Address Changes for I2C Dev Loaded Apps (Oct 14, 2025 - commit 2f4d222)
- **Impact:** NONE for external .ppma apps
- **Details:** Only affects apps loaded via I2C device, not SD card apps

#### 4. External Apps Build System
- **Status:** Stable and unchanged
- **Current external.cmake format:** Compatible with our build scripts
- **External app structure:** Still using same pattern (main.cpp + ui files + app info struct)

## API Compatibility Check

### Current External App API (v3)
```cpp
struct application_information_t {
    uint8_t* memory_location;
    externalAppEntry_t externalAppEntry;
    uint32_t header_version;        // = 0x00000003
    uint32_t app_version;
    uint8_t app_name[16];
    uint8_t bitmap_data[32];
    uint32_t icon_color;
    app_location_t menu_location;
    int32_t desired_menu_position;
    portapack::spi_flash::image_tag_t m4_app_tag;
    uint32_t m4_app_offset;
};
```

### MDK-Predator Compatibility
- ✅ Header version: 0x00000003 (CURRENT_HEADER_VERSION)
- ✅ Structure layout: Matches exactly
- ✅ Entry point: `initialize_app(ui::NavigationView& nav)` - correct signature
- ✅ Namespace: `ui::external_app::mdk_predator` - follows convention
- ✅ Icon format: 32 bytes bitmap data - correct
- ✅ Menu location: `app_location_t::UTILITIES` - valid

## External Apps Integration

### Current External Apps Count: 61 apps

Sample of similar apps (for reference):
- **keyfob**: Radio key fob decoder (similar to our automotive module)
- **scanner**: Frequency scanner (similar to our wireless scanning)
- **protoview**: Protocol viewer (similar to our protocol analysis)

### Build Integration
The firmware's `external.cmake` file uses a straightforward pattern:
```cmake
set(EXTCPPSRC
    external/mdk_predator/main.cpp
    external/mdk_predator/mdk_predator_app.cpp
    external/mdk_predator/src/mdk_predator.c
    external/mdk_predator/src/automotive/key_fob_analyzer.c
    # ... other source files
)

set(EXTAPPLIST
    mdk_predator
)
```

Our docker-entrypoint.sh script correctly registers the app in this format.

## Recommendations

### Immediate Actions
✅ **No changes required** - Current implementation is compatible

### Future Considerations

1. **Multi-Screen Support (Optional Enhancement)**
   - Consider adding dynamic screen size detection
   - Adjust UI layouts for different screen dimensions
   - Priority: LOW (current UI works on all devices)

2. **Monitor Firmware Updates**
   - Check for API version changes quarterly
   - Subscribe to Mayhem firmware release notifications
   - Test builds against nightly firmware occasionally

3. **Build System**
   - Current Docker build approach is solid
   - No changes needed to integration scripts
   - External app loading mechanism is stable

## Testing Results

### Build Compatibility
- ✅ Docker build scripts compatible with current firmware structure
- ✅ External app registration pattern matches firmware expectations
- ✅ Source file organization compatible with firmware build system

### API Version Tracking
- **Current API Version:** 3
- **Minimum Version for Checksum:** 2
- **MDK-Predator Using:** 3 (current)

## Conclusion

The MDK-Predator application is **fully compatible** with the latest Mayhem firmware. No code changes are required at this time. The external app API has been stable, and recent firmware updates have focused on:
- Adding new features (sound effects, new apps)
- UI improvements (multi-screen support)
- Bug fixes (specific to other apps)
- Standalone app enhancements (separate from external apps)

The build integration via Docker and the external.cmake registration system remains unchanged and functional.

## References

- Mayhem Firmware Repository: https://github.com/portapack-mayhem/mayhem-firmware
- External App API Header: `firmware/common/external_app.hpp`
- External App Build Config: `firmware/application/external/external.cmake`
- Latest Commit Analyzed: 00c9c6b (Nov 1, 2025)
