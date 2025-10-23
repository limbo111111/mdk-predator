#!/bin/bash
#
# MDK-Predator PortaPack Application Build Script
# 
# This script helps integrate MDK-Predator with PortaPack Mayhem firmware
# and builds the application for deployment.
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MDK_ROOT="$(dirname "$SCRIPT_DIR")"

# Default paths
MAYHEM_PATH=""
OUTPUT_DIR="$MDK_ROOT/build/portapack"

# Functions
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Build MDK-Predator as a PortaPack Mayhem external application.

OPTIONS:
    -m, --mayhem PATH      Path to PortaPack Mayhem firmware source
    -o, --output PATH      Output directory for built application (default: build/portapack)
    -c, --clean            Clean before building
    -h, --help             Show this help message

EXAMPLES:
    # Build with Mayhem firmware at specific path
    $0 -m ~/portapack-mayhem

    # Clean build
    $0 -m ~/portapack-mayhem -c

    # Custom output directory
    $0 -m ~/portapack-mayhem -o /tmp/mdk-build

REQUIREMENTS:
    - PortaPack Mayhem firmware source
    - ARM toolchain (arm-none-eabi-gcc)
    - CMake 3.16+
    - Python 3.7+

EOF
}

check_requirements() {
    print_info "Checking build requirements..."
    
    # Check for arm-none-eabi-gcc
    if ! command -v arm-none-eabi-gcc &> /dev/null; then
        print_error "ARM toolchain not found. Please install arm-none-eabi-gcc"
        exit 1
    fi
    
    # Check for cmake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake not found. Please install CMake 3.16+"
        exit 1
    fi
    
    # Check for python3
    if ! command -v python3 &> /dev/null; then
        print_error "Python 3 not found. Please install Python 3.7+"
        exit 1
    fi
    
    print_info "All requirements satisfied"
}

verify_mayhem_path() {
    if [ -z "$MAYHEM_PATH" ]; then
        print_error "Mayhem firmware path not specified. Use -m or --mayhem option."
        print_usage
        exit 1
    fi
    
    if [ ! -d "$MAYHEM_PATH" ]; then
        print_error "Mayhem firmware directory not found: $MAYHEM_PATH"
        exit 1
    fi
    
    if [ ! -f "$MAYHEM_PATH/CMakeLists.txt" ]; then
        print_error "Invalid Mayhem firmware directory (CMakeLists.txt not found)"
        exit 1
    fi
    
    print_info "Mayhem firmware found at: $MAYHEM_PATH"
}

integrate_with_mayhem() {
    print_info "Integrating MDK-Predator with Mayhem firmware..."
    
    local external_dir="$MAYHEM_PATH/firmware/application/external/mdk_predator"
    
    # Create external app directory
    mkdir -p "$external_dir"
    
    # Copy application files
    print_info "Copying application files..."
    cp -r "$MDK_ROOT/app"/* "$external_dir/"
    
    # Copy source files
    print_info "Copying source files..."
    cp -r "$MDK_ROOT/src" "$external_dir/"
    cp -r "$MDK_ROOT/include" "$external_dir/"
    
    # Copy configuration
    print_info "Copying configuration..."
    cp "$MDK_ROOT/mdk_predator.conf" "$external_dir/"
    
    print_info "Integration complete"
}

build_application() {
    print_info "Building PortaPack firmware with MDK-Predator..."
    
    local build_dir="$MAYHEM_PATH/build"
    
    # Create build directory
    mkdir -p "$build_dir"
    cd "$build_dir"
    
    # Configure with CMake
    print_info "Running CMake configuration..."
    if ! cmake ..; then
        print_error "CMake configuration failed"
        exit 1
    fi
    
    # Build external apps
    print_info "Building external applications..."
    if ! make external_apps; then
        print_error "Build failed"
        exit 1
    fi
    
    print_info "Build complete"
}

copy_output() {
    print_info "Copying built application..."
    
    local app_file="$MAYHEM_PATH/firmware/application/external/mdk_predator.ppma"
    
    if [ ! -f "$app_file" ]; then
        print_error "Built application not found: $app_file"
        exit 1
    fi
    
    # Create output directory
    mkdir -p "$OUTPUT_DIR"
    
    # Copy application
    cp "$app_file" "$OUTPUT_DIR/"
    
    # Copy configuration
    cp "$MDK_ROOT/mdk_predator.conf" "$OUTPUT_DIR/"
    
    # Create README
    cat > "$OUTPUT_DIR/README.txt" << EOF
MDK-Predator PortaPack Application
===================================

Files:
  - mdk_predator.ppma       Application binary
  - mdk_predator.conf       Configuration file

Installation:
  1. Format SD card as FAT32
  2. Create directory structure:
     /APPS/
     /MDK-PREDATOR/config/
  3. Copy mdk_predator.ppma to /APPS/
  4. Copy mdk_predator.conf to /MDK-PREDATOR/config/
  5. Insert SD card and launch from Apps menu

See DEPLOYMENT.md for detailed instructions.
EOF
    
    print_info "Application copied to: $OUTPUT_DIR"
    print_info "  - mdk_predator.ppma"
    print_info "  - mdk_predator.conf"
    print_info "  - README.txt"
}

clean_build() {
    print_info "Cleaning build artifacts..."
    
    if [ -d "$MAYHEM_PATH/build" ]; then
        rm -rf "$MAYHEM_PATH/build"
    fi
    
    if [ -d "$MAYHEM_PATH/firmware/application/external/mdk_predator" ]; then
        rm -rf "$MAYHEM_PATH/firmware/application/external/mdk_predator"
    fi
    
    print_info "Clean complete"
}

main() {
    local do_clean=0
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -m|--mayhem)
                MAYHEM_PATH="$2"
                shift 2
                ;;
            -o|--output)
                OUTPUT_DIR="$2"
                shift 2
                ;;
            -c|--clean)
                do_clean=1
                shift
                ;;
            -h|--help)
                print_usage
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                print_usage
                exit 1
                ;;
        esac
    done
    
    # Verify paths
    verify_mayhem_path
    
    # Check requirements
    check_requirements
    
    # Clean if requested
    if [ $do_clean -eq 1 ]; then
        clean_build
    fi
    
    # Build process
    integrate_with_mayhem
    build_application
    copy_output
    
    # Success message
    echo ""
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}Build Successful!${NC}"
    echo -e "${GREEN}========================================${NC}"
    echo ""
    echo "Application built: $OUTPUT_DIR/mdk_predator.ppma"
    echo ""
    echo "Next steps:"
    echo "  1. Copy mdk_predator.ppma to SD card /APPS/ directory"
    echo "  2. Copy mdk_predator.conf to SD card /MDK-PREDATOR/config/"
    echo "  3. Insert SD card into PortaPack"
    echo "  4. Launch from Apps menu"
    echo ""
    echo "See docs/DEPLOYMENT.md for detailed installation instructions."
    echo ""
}

# Run main function
main "$@"
