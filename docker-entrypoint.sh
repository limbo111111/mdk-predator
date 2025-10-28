#!/bin/bash
#
# MDK-Predator Docker Entrypoint Script
# 
# This script handles building mdk-predator with mayhem-firmware
# in the Docker container environment.
#

set -e # exit immediately on any failure

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_step() {
    echo -e "${BLUE}==>${NC} $1"
}

# Function to clone mayhem firmware if needed
clone_mayhem_firmware() {
    if [ ! -d "/workspace/mayhem-firmware/.git" ]; then
        print_step "Mayhem firmware not found, cloning from GitHub..."
        cd /workspace
        git clone --depth 1 https://github.com/portapack-mayhem/mayhem-firmware.git
        cd mayhem-firmware
        git submodule update --init --recursive
        print_info "Mayhem firmware cloned successfully"
    else
        print_info "Mayhem firmware already present"
        cd /workspace/mayhem-firmware
        # Update if requested
        if [ "$UPDATE_FIRMWARE" = "1" ]; then
            print_step "Updating mayhem firmware..."
            git pull
            git submodule update --init --recursive
        fi
    fi
}

# Function to integrate mdk-predator with mayhem firmware
integrate_mdk_predator() {
    print_step "Integrating MDK-Predator with Mayhem firmware..."
    
    local external_dir="/workspace/mayhem-firmware/firmware/application/external/mdk_predator"
    
    # Clean previous integration if exists
    if [ -d "$external_dir" ]; then
        print_warning "Removing previous integration..."
        rm -rf "$external_dir"
    fi
    
    # Create external app directory
    mkdir -p "$external_dir"
    
    # Copy application files
    if [ -d "/workspace/mdk-predator/app" ]; then
        print_info "Copying application files..."
        cp -r /workspace/mdk-predator/app/* "$external_dir/"
    else
        print_error "MDK-Predator app directory not found!"
        exit 1
    fi
    
    # Copy source files
    if [ -d "/workspace/mdk-predator/src" ]; then
        print_info "Copying source files..."
        cp -r /workspace/mdk-predator/src "$external_dir/"
    else
        print_error "MDK-Predator src directory not found!"
        exit 1
    fi
    
    # Copy include files
    if [ -d "/workspace/mdk-predator/include" ]; then
        print_info "Copying include files..."
        cp -r /workspace/mdk-predator/include "$external_dir/"
    else
        print_error "MDK-Predator include directory not found!"
        exit 1
    fi
    
    # Copy configuration
    if [ -f "/workspace/mdk-predator/mdk_predator.conf" ]; then
        print_info "Copying configuration..."
        cp /workspace/mdk-predator/mdk_predator.conf "$external_dir/"
    fi
    
    # Register in external.cmake
    local external_cmake="/workspace/mayhem-firmware/firmware/application/external/external.cmake"
    
    if [ -f "$external_cmake" ]; then
        print_info "Registering MDK-Predator in external.cmake..."
        
        # Check if already registered
        if ! grep -q "external/mdk_predator/main.cpp" "$external_cmake" 2>/dev/null; then
            # Backup the original file
            cp "$external_cmake" "$external_cmake.backup"
            
            # Add MDK-Predator sources to EXTCPPSRC and app to EXTAPPLIST
            awk '
                /^set\(EXTCPPSRC/ {
                    print
                    print ""
                    print "\t#mdk_predator"
                    print "\texternal/mdk_predator/main.cpp"
                    print "\texternal/mdk_predator/mdk_predator_app.cpp"
                    print "\texternal/mdk_predator/src/mdk_predator.c"
                    print "\texternal/mdk_predator/src/automotive/key_fob_analyzer.c"
                    print "\texternal/mdk_predator/src/automotive/rolling_code_tester.c"
                    print "\texternal/mdk_predator/src/wireless/wifi_analyzer.c"
                    print "\texternal/mdk_predator/src/wireless/bluetooth_analyzer.c"
                    print "\texternal/mdk_predator/src/wireless/subghz_analyzer.c"
                    print "\texternal/mdk_predator/src/crypto/crypto_analyzer.c"
                    next
                }
                /^set\(EXTAPPLIST/ {
                    print
                    print "\tmdk_predator"
                    next
                }
                { print }
            ' "$external_cmake.backup" > "$external_cmake"
            
            print_info "MDK-Predator registered in external.cmake"
        else
            print_info "MDK-Predator already registered in external.cmake"
        fi
    else
        print_warning "external.cmake not found - build may fail!"
        print_warning "This might be an older version of mayhem-firmware"
    fi
    
    print_info "Integration complete"
}

# Function to build with make
build_with_make() {
    print_step "Building with Make..."
    cd /workspace/mayhem-firmware
    
    # Clean previous build if requested
    if [ "$CLEAN_BUILD" = "1" ]; then
        print_info "Cleaning previous build..."
        rm -rf build
    fi
    
    mkdir -p build
    cd build
    
    print_info "Running CMake configuration..."
    cmake ..
    
    print_info "Building external apps (including mdk_predator)..."
    make application "$@"
    
    print_info "Build completed successfully!"
}

# Function to build with ninja
build_with_ninja() {
    print_step "Building with Ninja..."
    cd /workspace/mayhem-firmware
    
    # Clean previous build if requested
    if [ "$CLEAN_BUILD" = "1" ]; then
        print_info "Cleaning previous build..."
        rm -rf build
    fi
    
    mkdir -p build
    cd build
    
    print_info "Running CMake configuration..."
    cmake -G Ninja ..
    
    print_info "Building external apps (including mdk_predator)..."
    ninja application "$@"
    
    print_info "Build completed successfully!"
}

# Function to copy output files
copy_output() {
    print_step "Copying output files..."
    
    local app_file="/workspace/mayhem-firmware/firmware/application/external/mdk_predator.ppma"
    
    if [ -f "$app_file" ]; then
        mkdir -p /workspace/output
        cp "$app_file" /workspace/output/
        
        # Copy configuration if exists
        if [ -f "/workspace/mdk-predator/mdk_predator.conf" ]; then
            cp /workspace/mdk-predator/mdk_predator.conf /workspace/output/
        fi
        
        # Create README
        cat > /workspace/output/README.txt << EOF
MDK-Predator PortaPack Application
===================================

Built on: $(date)

Files:
  - mdk_predator.ppma       Application binary
  - mdk_predator.conf       Configuration file (if present)

Installation:
  1. Format SD card as FAT32
  2. Create directory structure:
     /APPS/
     /MDK-PREDATOR/config/
  3. Copy mdk_predator.ppma to /APPS/
  4. Copy mdk_predator.conf to /MDK-PREDATOR/config/ (if present)
  5. Insert SD card into PortaPack and launch from Apps menu

See documentation for detailed instructions.
EOF
        
        print_info "Output copied to /workspace/output/"
        ls -lh /workspace/output/
    else
        print_error "Built application not found: $app_file"
        print_error "Build may have failed!"
        exit 1
    fi
}

# Main build function
do_build() {
    print_info "Starting MDK-Predator build process..."
    echo ""
    
    # Check if mdk-predator source exists
    if [ ! -d "/workspace/mdk-predator" ]; then
        print_error "MDK-Predator source not found in /workspace/mdk-predator"
        print_error "Please mount your mdk-predator source directory to /workspace/mdk-predator"
        exit 1
    fi
    
    # Clone/update mayhem firmware
    clone_mayhem_firmware
    
    # Integrate mdk-predator
    integrate_mdk_predator
    
    # Build based on command
    if [ "$1" = "ninja" ]; then
        shift
        build_with_ninja "$@"
    else
        # Remove 'make' if it's the first argument
        if [ "$1" = "make" ]; then
            shift
        fi
        build_with_make "$@"
    fi
    
    # Copy output
    copy_output
    
    echo ""
    print_info "Build process completed successfully!"
    print_info "Your mdk_predator.ppma file is in /workspace/output/"
    echo ""
}

# Entry point logic
if [ "$1" = 'make' ]; then
    shift # remove 'make' from arguments
    do_build make "$@"
    exit 0
elif [[ $1 == -j* ]]; then
    # allow passing -j without typing 'make'
    do_build make "$@"
    exit 0
elif [ "$1" = 'ninja' ]; then
    shift # remove 'ninja' from arguments
    do_build ninja "$@"
    exit 0
elif [ "$1" = 'build' ]; then
    # Explicit build command
    shift
    do_build "$@"
    exit 0
elif [ "$1" = 'bash' ] || [ "$1" = 'sh' ]; then
    # Allow interactive shell
    exec "$@"
else
    # Default: build with make
    do_build make "$@"
    exit 0
fi
