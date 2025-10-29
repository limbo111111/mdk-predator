#!/bin/bash
#
# MDK-Predator PortaPack Application Build Script for WSL Debian
#
# This script is optimized for Windows Subsystem for Linux (WSL) running Debian/Ubuntu
# It handles WSL-specific paths and dependencies.
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
CLEAN=0

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
MDK-Predator PortaPack Application Build Script for WSL Debian
================================================================

This script is optimized for Windows Subsystem for Linux (WSL) running Debian/Ubuntu.

Usage: $0 [OPTIONS]

Build MDK-Predator as a PortaPack Mayhem external application in WSL.

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

    # Custom output directory (can use Windows paths via /mnt/c/...)
    $0 -m ~/portapack-mayhem -o /mnt/c/Users/YourName/Desktop/output

REQUIREMENTS:
    - WSL with Debian or Ubuntu distribution
    - PortaPack Mayhem firmware source (can be auto-downloaded with -d)
    - ARM toolchain (arm-none-eabi-gcc) (can be auto-installed with -i)
    - CMake 3.16+ (can be auto-installed with -i)
    - Python 3.7+ (can be auto-installed with -i)

WSL-SPECIFIC NOTES:
    - You can access Windows files via /mnt/c/, /mnt/d/, etc.
    - Output can be saved to Windows directories for easy access
    - This script handles WSL-specific dependency installation
    - Make sure WSL is updated: wsl --update (run in Windows PowerShell)

EOF
}

check_wsl() {
    # Verify we're running in WSL
    if [ ! -f /proc/sys/fs/binfmt_misc/WSLInterop ] && [ ! -f /proc/version ] || ! grep -qi microsoft /proc/version 2>/dev/null; then
        print_warning "This script is optimized for WSL but may work on native Linux"
        print_warning "For native Linux, consider using build_portapack_app.sh instead"
        echo ""
    else
        print_info "Detected WSL environment"
        # Show WSL version info
        if command -v wsl.exe &> /dev/null; then
            WSL_VERSION=$(wsl.exe -l -v 2>/dev/null | grep -i "$(hostname)" | awk '{print $4}' || echo "Unknown")
            if [ ! -z "$WSL_VERSION" ] && [ "$WSL_VERSION" != "Unknown" ]; then
                print_info "WSL Version: $WSL_VERSION"
            fi
        fi
    fi
}

install_dependencies_debian() {
    print_info "Installing build dependencies for Debian/Ubuntu in WSL..."

    # Update package list
    print_info "Updating package lists..."
    sudo apt-get update

    # Install dependencies
    print_info "Installing ARM toolchain, CMake, Python, and build tools..."
    sudo apt-get install -y \
        gcc-arm-none-eabi \
        binutils-arm-none-eabi \
        libnewlib-arm-none-eabi \
        cmake \
        python3 \
        python3-pip \
        git \
        make \
        build-essential \
        dfu-util \
        ccache \
        ninja-build

    print_info "Dependencies installed successfully"
}

install_dependencies() {
    # Detect distribution
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS_ID=$ID

        case $OS_ID in
            ubuntu|debian)
                install_dependencies_debian
                ;;
            *)
                print_warning "Detected $OS_ID - attempting Debian/Ubuntu installation method"
                install_dependencies_debian
                ;;
        esac
    else
        print_error "Cannot detect Linux distribution"
        print_error "Please install dependencies manually:"
        echo ""
        echo "  sudo apt-get install gcc-arm-none-eabi cmake python3 git make build-essential"
        echo ""
        exit 1
    fi
}

check_dependencies() {
    print_info "Checking build requirements..."
    echo ""

    local missing_deps=0

    # Check for ARM toolchain
    if ! command -v arm-none-eabi-gcc &> /dev/null; then
        print_error "ARM toolchain (arm-none-eabi-gcc) not found"
        missing_deps=1
    else
        local arm_version=$(arm-none-eabi-gcc --version | head -n1)
        print_info "ARM toolchain: $arm_version"
    fi

    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake not found"
        missing_deps=1
    else
        local cmake_version=$(cmake --version | head -n1)
        print_info "CMake: $cmake_version"
    fi

    # Check for Python
    if ! command -v python3 &> /dev/null; then
        print_error "Python3 not found"
        missing_deps=1
    else
        local python_version=$(python3 --version)
        print_info "Python: $python_version"
    fi

    # Check for make
    if ! command -v make &> /dev/null; then
        print_error "Make not found"
        missing_deps=1
    else
        local make_version=$(make --version | head -n1)
        print_info "Make: $make_version"
    fi

    # Check for git
    if ! command -v git &> /dev/null; then
        print_error "Git not found"
        missing_deps=1
    else
        local git_version=$(git --version)
        print_info "Git: $git_version"
    fi

    echo ""

    if [ $missing_deps -eq 1 ]; then
        print_error "Missing dependencies detected"
        echo ""
        print_info "You can install them automatically with: $0 --install-deps"
        echo ""
        exit 1
    fi

    print_info "All requirements satisfied!"
    echo ""
}

download_mayhem_firmware() {
    print_info "Downloading Mayhem firmware..."
    echo ""

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
            echo ""
            return
        else
            print_error "Directory exists but is not a git repository"
            print_error "Please remove or specify a different path with -m"
            exit 1
        fi
    fi

    # Clone the firmware
    print_info "Cloning Mayhem firmware from GitHub..."
    print_info "This may take several minutes..."
    echo ""

    git clone --depth 1 https://github.com/portapack-mayhem/mayhem-firmware.git "$MAYHEM_PATH"

    # Initialize submodules
    print_info "Initializing submodules..."
    cd "$MAYHEM_PATH"
    git submodule update --init --recursive
    cd - > /dev/null

    echo ""
    print_info "Mayhem firmware downloaded successfully to: $MAYHEM_PATH"
    echo ""
}

check_mayhem_path() {
    if [ -z "$MAYHEM_PATH" ]; then
        if [ $DOWNLOAD_FIRMWARE -eq 0 ]; then
            print_error "Mayhem firmware path not specified"
            echo ""
            print_info "Either use -m to specify the path, or use -d to download automatically"
            echo ""
            print_usage
            exit 1
        else
            # Path will be set by download function
            return
        fi
    fi

    if [ ! -d "$MAYHEM_PATH" ]; then
        print_error "Mayhem firmware directory not found: $MAYHEM_PATH"
        print_info "Use -d to download it automatically"
        exit 1
    fi

    if [ ! -f "$MAYHEM_PATH/CMakeLists.txt" ]; then
        print_error "Invalid Mayhem firmware directory (CMakeLists.txt not found)"
        exit 1
    fi

    print_info "Mayhem firmware found at: $MAYHEM_PATH"
    echo ""
}

download_libopencm3() {
    print_info "Downloading libopencm3 from HackRF repository..."

    local libopencm3_path="$MAYHEM_PATH/hackrf/firmware/libopencm3"
    local hackrf_repo="https://github.com/portapack-mayhem/hackrf.git"
    local hackrf_commit="cf6815aaf9c4bb11c3ad3de97721c67ddf4fcb38"

    # Check if git is installed
    if ! command -v git &> /dev/null; then
        print_error "Git is not installed. Please install git first."
        exit 1
    fi

    # Create hackrf/firmware directory if it doesn't exist
    mkdir -p "$MAYHEM_PATH/hackrf/firmware"

    # Clone the specific commit from HackRF repository into a temporary directory
    local temp_dir=$(mktemp -d)
    print_info "Cloning HackRF repository (this may take a moment)..."

    if ! git clone "$hackrf_repo" "$temp_dir/hackrf" --depth 1 --branch master 2>&1; then
        print_warning "Failed to clone with --depth, trying full clone..."
        rm -rf "$temp_dir/hackrf"
        if ! git clone "$hackrf_repo" "$temp_dir/hackrf"; then
            print_error "Failed to clone HackRF repository"
            rm -rf "$temp_dir"
            exit 1
        fi
    fi

    cd "$temp_dir/hackrf"

    # Try to checkout the specific commit
    print_info "Checking out commit: $hackrf_commit"
    if ! git checkout "$hackrf_commit" 2>&1; then
        print_warning "Could not checkout specific commit, using latest version"
    fi

    # Initialize libopencm3 submodule in the cloned repo
    print_info "Initializing libopencm3 submodule..."
    cd firmware
    if ! git submodule update --init libopencm3; then
        print_error "Failed to initialize libopencm3 submodule"
        cd - > /dev/null
        rm -rf "$temp_dir"
        exit 1
    fi

    # Copy libopencm3 to the target location
    print_info "Copying libopencm3 to $libopencm3_path"
    cp -r libopencm3 "$libopencm3_path"

    cd - > /dev/null

    # Clean up temporary directory
    rm -rf "$temp_dir"

    print_info "✓ libopencm3 downloaded successfully"
}

verify_submodules() {
    print_info "Verifying git submodules are initialized..."

    # Check for critical submodules that must exist for successful build
    # Note: nvic.h is generated during build, so we check for source files instead
    local critical_paths=(
        "hackrf/firmware/libopencm3"
        "hackrf/firmware/libopencm3/include/libopencm3/lpc43xx"
        "hackrf/firmware/libopencm3/include/libopencm3/lpc43xx/m0/irq.yaml"
    )

    local missing_submodules=0

    for path in "${critical_paths[@]}"; do
        if [ ! -e "$MAYHEM_PATH/$path" ]; then
            print_info "Missing: $path"
            missing_submodules=1
        fi
    done

    # If libopencm3 is missing, try different approaches to get it
    if [ $missing_submodules -eq 1 ]; then
        echo ""
        print_warning "libopencm3 is not present!"
        print_warning "This will cause compilation errors like:"
        print_warning "  fatal error: libopencm3/lpc43xx/m0/nvic.h: No such file or directory"
        echo ""

        # If this is a git repository, try submodule init first
        if [ -d "$MAYHEM_PATH/.git" ]; then
            print_info "Attempting to initialize git submodules..."
            cd "$MAYHEM_PATH"
            if git submodule update --init --recursive 2>&1; then
                cd - > /dev/null
                print_info "Submodules initialized successfully via git"

                # Verify again
                missing_submodules=0
                for path in "${critical_paths[@]}"; do
                    if [ ! -e "$MAYHEM_PATH/$path" ]; then
                        missing_submodules=1
                    fi
                done
            else
                cd - > /dev/null
                print_warning "Git submodule initialization failed"
            fi
        else
            print_info "Mayhem firmware is not a git repository"
        fi

        # If still missing, download from HackRF repository
        if [ $missing_submodules -eq 1 ]; then
            print_info "Downloading libopencm3 from HackRF repository..."
            download_libopencm3

            # Final verification
            for path in "${critical_paths[@]}"; do
                if [ ! -e "$MAYHEM_PATH/$path" ]; then
                    print_error "Still missing: $path after download"
                    exit 1
                fi
            done
        fi
    fi

    print_info "✓ All required submodules are properly initialized"
    echo ""
}

build_libopencm3() {
    print_info "Building libopencm3 library..."

    local libopencm3_path="$MAYHEM_PATH/hackrf/firmware/libopencm3"

    if [ ! -d "$libopencm3_path" ]; then
        print_error "libopencm3 directory not found: $libopencm3_path"
        print_error "Please run verify_submodules first"
        exit 1
    fi

    cd "$libopencm3_path"

    # Check if already built
    if [ -f "lib/libopencm3_lpc43xx.a" ] && [ -f "include/libopencm3/lpc43xx/m0/nvic.h" ]; then
        print_info "libopencm3 already built, skipping..."
        cd "$SCRIPT_DIR"
        return 0
    fi

    print_info "Building libopencm3 for LPC43xx (this may take a few minutes)..."

    # Build libopencm3
    if ! make TARGETS=lpc43xx 2>&1; then
        print_error "libopencm3 build failed"
        cd "$SCRIPT_DIR"
        exit 1
    fi

    # Verify the generated header exists
    if [ ! -f "include/libopencm3/lpc43xx/m0/nvic.h" ]; then
        print_error "nvic.h was not generated during libopencm3 build"
        print_error "This is unexpected - please check the build logs"
        cd "$SCRIPT_DIR"
        exit 1
    fi

    print_info "✓ libopencm3 built successfully"
    cd "$SCRIPT_DIR"
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

    print_info "Integration complete"
    echo ""
}

verify_file_links() {
    print_info "Verifying all files are properly linked in Mayhem firmware..."

    local external_dir="$MAYHEM_PATH/firmware/application/external/mdk_predator"
    local external_cmake="$MAYHEM_PATH/firmware/application/external/external.cmake"
    local verification_failed=0

    # Define all required files
    local required_cpp_files=(
        "main.cpp"
        "mdk_predator_app.cpp"
    )

    local required_hpp_files=(
        "mdk_predator_app.hpp"
    )

    local required_c_files=(
        "src/mdk_predator.c"
        "src/automotive/key_fob_analyzer.c"
        "src/automotive/rolling_code_tester.c"
        "src/wireless/wifi_analyzer.c"
        "src/wireless/bluetooth_analyzer.c"
        "src/wireless/subghz_analyzer.c"
        "src/crypto/crypto_analyzer.c"
    )

    local required_h_files=(
        "include/mdk_predator.h"
        "include/input_validation.h"
        "include/automotive/key_fob_analyzer.h"
        "include/automotive/rolling_code_tester.h"
        "include/wireless/wifi_analyzer.h"
        "include/wireless/bluetooth_analyzer.h"
        "include/wireless/subghz_analyzer.h"
        "include/crypto/crypto_analyzer.h"
    )

    local required_other_files=(
        "manifest.json"
        "mdk_predator.conf"
    )

    # Check CPP files
    print_info "Checking C++ source files..."
    for file in "${required_cpp_files[@]}"; do
        if [ ! -f "$external_dir/$file" ]; then
            print_error "Missing C++ file: $file"
            verification_failed=1
        else
            print_info "  ✓ $file"
        fi
    done

    # Check HPP files
    print_info "Checking C++ header files..."
    for file in "${required_hpp_files[@]}"; do
        if [ ! -f "$external_dir/$file" ]; then
            print_error "Missing C++ header: $file"
            verification_failed=1
        else
            print_info "  ✓ $file"
        fi
    done

    # Check C files
    print_info "Checking C source files..."
    for file in "${required_c_files[@]}"; do
        if [ ! -f "$external_dir/$file" ]; then
            print_error "Missing C file: $file"
            verification_failed=1
        else
            print_info "  ✓ $file"
        fi
    done

    # Check H files
    print_info "Checking C header files..."
    for file in "${required_h_files[@]}"; do
        if [ ! -f "$external_dir/$file" ]; then
            print_error "Missing C header: $file"
            verification_failed=1
        else
            print_info "  ✓ $file"
        fi
    done

    # Check other required files
    print_info "Checking configuration files..."
    for file in "${required_other_files[@]}"; do
        if [ ! -f "$external_dir/$file" ]; then
            print_error "Missing file: $file"
            verification_failed=1
        else
            print_info "  ✓ $file"
        fi
    done

    # Verify external.cmake registration
    print_info "Verifying external.cmake registration..."
    local cmake_verification_failed=0

    # Check if all source files are registered in external.cmake
    for file in "${required_cpp_files[@]}"; do
        if ! grep -q "external/mdk_predator/$file" "$external_cmake"; then
            print_error "File not registered in external.cmake: $file"
            cmake_verification_failed=1
            verification_failed=1
        fi
    done

    for file in "${required_c_files[@]}"; do
        if ! grep -q "external/mdk_predator/$file" "$external_cmake"; then
            print_error "File not registered in external.cmake: $file"
            cmake_verification_failed=1
            verification_failed=1
        fi
    done

    # Check if mdk_predator is registered in EXTAPPLIST
    if ! grep -E -q 'set\s*\(\s*EXTAPPLIST[^\)]*mdk_predator|list\s*\(\s*APPEND\s+EXTAPPLIST[^\)]*mdk_predator' "$external_cmake"; then
        print_error "mdk_predator not found in EXTAPPLIST registration in external.cmake"
        cmake_verification_failed=1
        verification_failed=1
    fi

    if [ $cmake_verification_failed -eq 0 ]; then
        print_info "  ✓ All files registered in external.cmake"
    fi

    # Final verification result
    if [ $verification_failed -eq 1 ]; then
        echo ""
        print_error "File verification FAILED!"
        print_error "Some required files are missing or not properly linked."
        print_error "Please check the error messages above."
        echo ""
        exit 1
    fi

    echo ""
    print_info "✓ All files verified successfully!"
    print_info "✓ All files are properly linked in Mayhem firmware"
    print_info "✓ Ready to build MDK-Predator"
    echo ""
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

    echo ""

    # Build application (which includes external apps)
    print_info "Building application and external apps..."
    if ! make application; then
        print_error "Build failed"
        exit 1
    fi

    cd - > /dev/null

    echo ""
    print_info "Build complete"
    echo ""
}

copy_output() {
    print_info "Copying built application..."

    local app_file="$MAYHEM_PATH/firmware/application/external/mdk_predator.ppma"

    if [ ! -f "$app_file" ]; then
        print_error "Built application not found: $app_file"
        print_warning "Check build logs for errors"
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

    echo ""
    print_info "Application copied to: $OUTPUT_DIR"
    print_info "  - mdk_predator.ppma"
    print_info "  - mdk_predator.conf"
    print_info "  - README.txt"
    echo ""

    # If output is in WSL filesystem, show Windows path
    if [[ "$OUTPUT_DIR" == /home/* ]] || [[ "$OUTPUT_DIR" == /root/* ]]; then
        local wsl_distro=$(hostname)
        print_info "To access from Windows, use:"
        print_info "  \\\\wsl\$\\${wsl_distro}${OUTPUT_DIR}"
        echo ""
    fi
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
    echo ""
}

# Parse command-line arguments
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
            CLEAN=1
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
            echo ""
            print_usage
            exit 1
            ;;
    esac
done

# Main execution
echo ""
echo "MDK-Predator PortaPack Build for WSL Debian"
echo "============================================"
echo ""

# Check if running in WSL
check_wsl

# Install dependencies if requested
if [ $INSTALL_DEPS -eq 1 ]; then
    install_dependencies
    echo ""
fi

# Download firmware if requested
if [ $DOWNLOAD_FIRMWARE -eq 1 ]; then
    download_mayhem_firmware
fi

# Show build info
echo "MDK-Predator PortaPack Build Configuration"
echo "==========================================="
echo "MDK Root:     $MDK_ROOT"
echo "Mayhem Path:  ${MAYHEM_PATH:-<will be downloaded>}"
echo "Output Dir:   $OUTPUT_DIR"
echo ""

# Verify Mayhem path
check_mayhem_path

# Verify submodules are initialized
verify_submodules

# Build libopencm3 first (generates nvic.h and other headers)
build_libopencm3

# Check dependencies
check_dependencies

# Clean if requested
if [ $CLEAN -eq 1 ]; then
    clean_build
fi

# Build process
integrate_with_mayhem
verify_file_links
build_application
copy_output

# Success message
echo ""
echo "========================================"
echo "Build Successful!"
echo "========================================"
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
