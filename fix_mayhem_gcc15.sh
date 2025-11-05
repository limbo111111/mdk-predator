#!/bin/bash
# Fix mayhem-firmware compilation issues with GCC 15
# These are upstream firmware bugs that need to be patched

MAYHEM_DIR="${1:-build/mayhem-firmware}"

echo "Applying GCC 15 compatibility patches to mayhem-firmware..."

# Fix 1: Add <cstdint> to tonesets.hpp
if ! grep -q "#include <cstdint>" "$MAYHEM_DIR/firmware/common/tonesets.hpp" 2>/dev/null; then
    echo "Patching tonesets.hpp..."
    sed -i '/#include <memory>/a #include <cstdint>' "$MAYHEM_DIR/firmware/common/tonesets.hpp"
fi

# Fix 2: Add <cstdint> to dcs.hpp
if ! grep -q "#include <cstdint>" "$MAYHEM_DIR/firmware/application/protocols/dcs.hpp" 2>/dev/null; then
    echo "Patching dcs.hpp..."
    sed -i '/#include <memory>/i #include <cstdint>' "$MAYHEM_DIR/firmware/application/protocols/dcs.hpp"
fi

# Fix 3: Fix mixed designated initializers in ui_standalone_view.cpp
if grep -q "^    \.f_open = " "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp" 2>/dev/null; then
    echo "Patching ui_standalone_view.cpp..."
    sed -i 's/^    \.f_open = /    \/* .f_open = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_close = /    \/* .f_close = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_read = /    \/* .f_read = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_write = /    \/* .f_write = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_lseek = /    \/* .f_lseek = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_truncate = /    \/* .f_truncate = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_sync = /    \/* .f_sync = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_opendir = /    \/* .f_opendir = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_closedir = /    \/* .f_closedir = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_readdir = /    \/* .f_readdir = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_findfirst = /    \/* .f_findfirst = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_findnext = /    \/* .f_findnext = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_mkdir = /    \/* .f_mkdir = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_unlink = /    \/* .f_unlink = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_rename = /    \/* .f_rename = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_stat = /    \/* .f_stat = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_utime = /    \/* .f_utime = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_getfree = /    \/* .f_getfree = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_mount = /    \/* .f_mount = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_putc = /    \/* .f_putc = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_puts = /    \/* .f_puts = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_printf = /    \/* .f_printf = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.f_gets = /    \/* .f_gets = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.draw_pixels = /    \/* .draw_pixels = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.draw_pixel = /    \/* .draw_pixel = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.exit_app = /    \/* .exit_app = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.screen_height = /    \/* .screen_height = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
    sed -i 's/^    \.screen_width = /    \/* .screen_width = *\/ /' "$MAYHEM_DIR/firmware/application/apps/ui_standalone_view.cpp"
fi

echo "Patches applied successfully!"
