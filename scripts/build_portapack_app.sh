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
DEFAULT_MAYHEM_CLONE_PATH="$MDK_ROOT/build/mayhem-firmware"

# Flags
INSTALL_DEPS=0
DOWNLOAD_FIRMWARE=0

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
    -m, --mayhem PATH          Path to PortaPack Mayhem firmware source
    -o, --output PATH          Output directory for built application (default: build/portapack)
    -c, --clean                Clean before building
    -i, --install-deps         Install required dependencies (ARM toolchain, CMake, Python)
    -d, --download-firmware    Download Mayhem firmware if not present
    -h, --help                 Show this help message

EXAMPLES:
    # Install dependencies and download firmware automatically
    $0 --install-deps --download-firmware

    # Build with Mayhem firmware at specific path
    $0 -m ~/portapack-mayhem

    # Install deps, download firmware, and build in one command
    $0 -i -d

    # Clean build
    $0 -m ~/portapack-mayhem -c

    # Custom output directory
    $0 -m ~/portapack-mayhem -o /tmp/mdk-build

REQUIREMENTS:
    - PortaPack Mayhem firmware source (can be auto-downloaded with -d)
    - ARM toolchain (arm-none-eabi-gcc) (can be auto-installed with -i)
    - CMake 3.16+ (can be auto-installed with -i)
    - Python 3.7+ (can be auto-installed with -i)

EOF
}

install_dependencies() {
    print_info "Installing build dependencies..."
    
    # Detect OS
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS=$ID
    else
        print_error "Cannot detect operating system"
        exit 1
    fi
    
    case $OS in
        ubuntu|debian)
            print_info "Detected Debian/Ubuntu system"
            print_info "Installing ARM toolchain, CMake, Python..."
            sudo apt-get update
            sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi \
                cmake python3 python3-pip git make dfu-util
            ;;
        fedora|rhel|centos)
            print_info "Detected Red Hat based system"
            print_info "Installing ARM toolchain, CMake, Python..."
            sudo dnf install -y arm-none-eabi-gcc-cs arm-none-eabi-newlib \
                cmake python3 python3-pip git make dfu-util
            ;;
        arch|manjaro)
            print_info "Detected Arch based system"
            print_info "Installing ARM toolchain, CMake, Python..."
            sudo pacman -S --noconfirm arm-none-eabi-gcc arm-none-eabi-newlib \
                cmake python python-pip git make dfu-util
            ;;
        *)
            print_warning "Unsupported OS: $OS"
            print_info "Please install manually:"
            print_info "  - ARM GCC toolchain (arm-none-eabi-gcc)"
            print_info "  - CMake 3.16+"
            print_info "  - Python 3.7+"
            print_info "  - Git, Make, dfu-util"
            exit 1
            ;;
    esac
    
    print_info "Dependencies installed successfully"
}

download_mayhem_firmware() {
    print_info "Downloading Mayhem firmware..."
    
    # Check if git is installed
    if ! command -v git &> /dev/null; then
        print_error "Git is not installed. Please install git first."
        exit 1
    fi
    
    # Set default path if not specified
    if [ -z "$MAYHEM_PATH" ]; then
        MAYHEM_PATH="$DEFAULT_MAYHEM_CLONE_PATH"
        print_info "Using default firmware location: $MAYHEM_PATH"
    fi
    
    # Check if directory already exists
    if [ -d "$MAYHEM_PATH" ]; then
        print_warning "Directory already exists: $MAYHEM_PATH"
        
        # Check if it's a git repo
        if [ -d "$MAYHEM_PATH/.git" ]; then
            print_info "Updating existing Mayhem firmware..."
            cd "$MAYHEM_PATH"
            git pull
            git submodule update --init --recursive
            cd - > /dev/null
            print_info "Firmware updated successfully"
            return 0
        else
            print_error "Directory exists but is not a git repository"
            print_error "Please remove or specify a different path with -m"
            exit 1
        fi
    fi
    
    # Clone the firmware
    print_info "Cloning Mayhem firmware from GitHub..."
    git clone --depth 1 https://github.com/portapack-mayhem/mayhem-firmware.git "$MAYHEM_PATH"
    
    # Initialize submodules
    print_info "Initializing submodules..."
    cd "$MAYHEM_PATH"
    git submodule update --init --recursive
    cd - > /dev/null
    
    print_info "Mayhem firmware downloaded successfully to: $MAYHEM_PATH"
}

check_requirements() {
    print_info "Checking build requirements..."
    
    local missing_deps=()
    
    # Check for arm-none-eabi-gcc
    if ! command -v arm-none-eabi-gcc &> /dev/null; then
        print_warning "ARM toolchain not found"
        missing_deps+=("arm-none-eabi-gcc")
    else
        print_info "ARM toolchain: $(arm-none-eabi-gcc --version | head -1)"
    fi
    
    # Check for cmake
    if ! command -v cmake &> /dev/null; then
        print_warning "CMake not found"
        missing_deps+=("cmake")
    else
        print_info "CMake: $(cmake --version | head -1)"
    fi
    
    # Check for python3
    if ! command -v python3 &> /dev/null; then
        print_warning "Python 3 not found"
        missing_deps+=("python3")
    else
        print_info "Python: $(python3 --version)"
    fi
    
    # Check for make
    if ! command -v make &> /dev/null; then
        print_warning "Make not found"
        missing_deps+=("make")
    else
        print_info "Make: $(make --version | head -1)"
    fi
    
    if [ ${#missing_deps[@]} -gt 0 ]; then
        print_error "Missing dependencies: ${missing_deps[*]}"
        echo ""
        print_info "You can install them automatically with: $0 --install-deps"
        echo ""
        exit 1
    fi
    
    print_info "All requirements satisfied"
}

verify_mayhem_path() {
    # If download firmware was requested but no path set, it will be set by download function
    if [ -z "$MAYHEM_PATH" ]; then
        if [ $DOWNLOAD_FIRMWARE -eq 0 ]; then
            print_error "Mayhem firmware path not specified."
            print_info "Either use -m to specify the path, or use -d to download automatically."
            print_usage
            exit 1
        else
            # Path will be set by download_mayhem_firmware
            return 0
        fi
    fi
    
    if [ ! -d "$MAYHEM_PATH" ]; then
        print_error "Mayhem firmware directory not found: $MAYHEM_PATH"
        print_info "Use -d or --download-firmware to download it automatically"
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
    local external_cmake="$MAYHEM_PATH/firmware/application/external/external.cmake"
    
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
    
    # Register in external.cmake if not already registered
    print_info "Registering MDK-Predator in external.cmake..."
    if ! grep -q "external/mdk_predator/main.cpp" "$external_cmake" 2>/dev/null; then
        # Backup the original file
        cp "$external_cmake" "$external_cmake.backup"
        
        # Find the line with "set(EXTCPPSRC" and add MDK-Predator sources after it
        # Using a temporary file for safe editing
        awk '
            /^set\(EXTCPPSRC/ {
                print
                print ""
                print "\t#mdk_predator"
                print "\texternal/mdk_predator/main.cpp"
                print "\texternal/mdk_predator/mdk_predator_app.cpp"
                print "\texternal/mdk_predator/../src/mdk_predator.c"
                print "\texternal/mdk_predator/../src/automotive/key_fob_analyzer.c"
                print "\texternal/mdk_predator/../src/automotive/rolling_code_tester.c"
                print "\texternal/mdk_predator/../src/wireless/wifi_analyzer.c"
                print "\texternal/mdk_predator/../src/wireless/bluetooth_analyzer.c"
                print "\texternal/mdk_predator/../src/wireless/subghz_analyzer.c"
                print "\texternal/mdk_predator/../src/crypto/crypto_analyzer.c"
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
            -i|--install-deps)
                INSTALL_DEPS=1
                shift
                ;;
            -d|--download-firmware)
                DOWNLOAD_FIRMWARE=1
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
    
    # Install dependencies if requested
    if [ $INSTALL_DEPS -eq 1 ]; then
        install_dependencies
    fi
    
    # Download firmware if requested
    if [ $DOWNLOAD_FIRMWARE -eq 1 ]; then
        download_mayhem_firmware
    fi
    
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
