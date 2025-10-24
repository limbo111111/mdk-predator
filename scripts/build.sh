#!/bin/bash
#
# MDK-Predator Library Build Script for Linux
# 
# This script builds the MDK-Predator library for development
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

# Flags
INSTALL_DEPS=0
DO_CLEAN=0
RUN_TESTS=0
TARGET="all"

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

Build MDK-Predator library for development.

OPTIONS:
    -i, --install-deps     Install required dependencies (ARM toolchain, Make)
    -c, --clean            Clean build artifacts before building
    -t, --test             Run tests after building
    -T, --target TARGET    Build specific target (default: all)
                           Options: all, automotive, wireless, crypto
    -h, --help             Show this help message

EXAMPLES:
    # Install dependencies and build
    $0 --install-deps

    # Build library
    $0

    # Clean build
    $0 --clean

    # Build and test
    $0 --test

    # Build specific module
    $0 --target automotive

REQUIREMENTS:
    - ARM toolchain (arm-none-eabi-gcc) (can be auto-installed with -i)
    - GNU Make (can be auto-installed with -i)

For PortaPack application build, use build_portapack_app.sh

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
            print_info "Installing ARM toolchain and build tools..."
            sudo apt-get update
            sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi \
                make gcc git
            ;;
        fedora|rhel|centos)
            print_info "Detected Red Hat based system"
            print_info "Installing ARM toolchain and build tools..."
            sudo dnf install -y arm-none-eabi-gcc-cs arm-none-eabi-newlib \
                make gcc git
            ;;
        arch|manjaro)
            print_info "Detected Arch based system"
            print_info "Installing ARM toolchain and build tools..."
            sudo pacman -S --noconfirm arm-none-eabi-gcc arm-none-eabi-newlib \
                make gcc git
            ;;
        *)
            print_warning "Unsupported OS: $OS"
            print_info "Please install manually:"
            print_info "  - ARM GCC toolchain (arm-none-eabi-gcc)"
            print_info "  - GNU Make"
            print_info "  - GCC (for tests)"
            print_info "  - Git"
            exit 1
            ;;
    esac
    
    print_info "Dependencies installed successfully"
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
    
    # Check for make
    if ! command -v make &> /dev/null; then
        print_warning "Make not found"
        missing_deps+=("make")
    else
        print_info "Make: $(make --version | head -1)"
    fi
    
    # Check for gcc (for tests)
    if [ $RUN_TESTS -eq 1 ]; then
        if ! command -v gcc &> /dev/null; then
            print_warning "GCC not found (required for tests)"
            missing_deps+=("gcc")
        else
            print_info "GCC: $(gcc --version | head -1)"
        fi
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

build_library() {
    print_info "Building MDK-Predator library ($TARGET)..."
    
    cd "$MDK_ROOT"
    
    if [ "$TARGET" = "all" ]; then
        make
    else
        make "$TARGET"
    fi
    
    if [ $? -eq 0 ]; then
        echo ""
        print_info "Build successful!"
        echo ""
        
        # Show output if library was built
        local lib_path="$MDK_ROOT/build/lib/libmdk_predator.a"
        if [ -f "$lib_path" ]; then
            local lib_size=$(stat -f%z "$lib_path" 2>/dev/null || stat -c%s "$lib_path" 2>/dev/null)
            local lib_size_kb=$((lib_size / 1024))
            print_info "Library created: $lib_path"
            print_info "Library size: ${lib_size_kb} KB"
        fi
    else
        print_error "Build failed"
        exit 1
    fi
}

run_tests() {
    print_info "Running tests..."
    
    cd "$MDK_ROOT"
    
    make test
    
    if [ $? -eq 0 ]; then
        echo ""
        print_info "All tests passed!"
        echo ""
    else
        print_error "Tests failed"
        exit 1
    fi
}

clean_build() {
    print_info "Cleaning build artifacts..."
    
    cd "$MDK_ROOT"
    make clean
    
    print_info "Clean complete"
}

main() {
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -i|--install-deps)
                INSTALL_DEPS=1
                shift
                ;;
            -c|--clean)
                DO_CLEAN=1
                shift
                ;;
            -t|--test)
                RUN_TESTS=1
                shift
                ;;
            -T|--target)
                TARGET="$2"
                shift 2
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
    
    echo ""
    echo -e "${GREEN}MDK-Predator Library Build${NC}"
    echo -e "${GREEN}==========================${NC}"
    echo ""
    echo "Root Directory: $MDK_ROOT"
    echo "Target:         $TARGET"
    echo ""
    
    # Install dependencies if requested
    if [ $INSTALL_DEPS -eq 1 ]; then
        install_dependencies
        echo ""
    fi
    
    # Check requirements
    check_requirements
    echo ""
    
    # Clean if requested
    if [ $DO_CLEAN -eq 1 ]; then
        clean_build
        echo ""
    fi
    
    # Build
    build_library
    
    # Test if requested
    if [ $RUN_TESTS -eq 1 ]; then
        run_tests
    fi
    
    # Success message
    echo ""
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}Build Completed Successfully!${NC}"
    echo -e "${GREEN}========================================${NC}"
    echo ""
    echo "Library location: $MDK_ROOT/build/lib/libmdk_predator.a"
    echo ""
    echo "Next steps:"
    echo "  - Use library in your embedded project"
    echo "  - Build PortaPack app with build_portapack_app.sh"
    echo "  - Run tests with: $0 --test"
    echo ""
}

# Run main function
main "$@"
