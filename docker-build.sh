#!/bin/bash
#
# Docker Build Script for MDK-Predator
#
# This script provides a simple interface to build mdk-predator
# using Docker, avoiding all dependency issues on Windows or any other platform.
#

set -e

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

print_usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Build MDK-Predator using Docker (no local dependencies required!)

OPTIONS:
    -c, --clean             Clean build (remove previous build artifacts)
    -u, --update-firmware   Update mayhem firmware from git
    -n, --ninja             Use Ninja build system (faster)
    -s, --shell             Open interactive shell in container
    -r, --rebuild           Rebuild Docker image
    -h, --help              Show this help message

EXAMPLES:
    # Standard build
    $0

    # Clean build
    $0 --clean

    # Build with Ninja (faster)
    $0 --ninja

    # Clean build with Ninja and update firmware
    $0 -c -u -n

    # Open shell for debugging
    $0 --shell

    # Rebuild Docker image
    $0 --rebuild

REQUIREMENTS:
    - Docker (or Docker Desktop for Windows)
    - docker-compose (usually included with Docker Desktop)

OUTPUT:
    Built files will be in: build/docker-output/mdk_predator.ppma

EOF
}

# Default options
CLEAN_BUILD=0
UPDATE_FIRMWARE=0
USE_NINJA=0
OPEN_SHELL=0
REBUILD_IMAGE=0

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -c|--clean)
            CLEAN_BUILD=1
            shift
            ;;
        -u|--update-firmware)
            UPDATE_FIRMWARE=1
            shift
            ;;
        -n|--ninja)
            USE_NINJA=1
            shift
            ;;
        -s|--shell)
            OPEN_SHELL=1
            shift
            ;;
        -r|--rebuild)
            REBUILD_IMAGE=1
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

# Check if Docker is installed
if ! command -v docker &> /dev/null; then
    print_error "Docker is not installed!"
    echo ""
    echo "Please install Docker:"
    echo "  - Windows: https://docs.docker.com/desktop/install/windows-install/"
    echo "  - macOS: https://docs.docker.com/desktop/install/mac-install/"
    echo "  - Linux: https://docs.docker.com/engine/install/"
    echo ""
    exit 1
fi

# Check if docker-compose is installed
if ! command -v docker-compose &> /dev/null && ! docker compose version &> /dev/null; then
    print_error "docker-compose is not installed!"
    echo ""
    echo "docker-compose is usually included with Docker Desktop."
    echo "If you have Docker but not docker-compose, install it from:"
    echo "https://docs.docker.com/compose/install/"
    echo ""
    exit 1
fi

# Use 'docker compose' or 'docker-compose' based on availability
DOCKER_COMPOSE="docker-compose"
if ! command -v docker-compose &> /dev/null; then
    DOCKER_COMPOSE="docker compose"
fi

# Rebuild image if requested
if [ $REBUILD_IMAGE -eq 1 ]; then
    print_info "Rebuilding Docker image..."
    $DOCKER_COMPOSE build --no-cache
fi

# Create output directory
mkdir -p build/docker-output

# Export environment variables
export CLEAN_BUILD
export UPDATE_FIRMWARE

print_info "MDK-Predator Docker Build"
echo ""
print_info "Configuration:"
echo "  - Clean build: $([ $CLEAN_BUILD -eq 1 ] && echo 'Yes' || echo 'No')"
echo "  - Update firmware: $([ $UPDATE_FIRMWARE -eq 1 ] && echo 'Yes' || echo 'No')"
echo "  - Build system: $([ $USE_NINJA -eq 1 ] && echo 'Ninja' || echo 'Make')"
echo "  - Output directory: build/docker-output/"
echo ""

if [ $OPEN_SHELL -eq 1 ]; then
    print_info "Opening interactive shell..."
    $DOCKER_COMPOSE run --rm mdk-predator-shell
else
    if [ $USE_NINJA -eq 1 ]; then
        print_info "Building with Ninja..."
        $DOCKER_COMPOSE run --rm mdk-predator-build-ninja
    elif [ $CLEAN_BUILD -eq 1 ]; then
        print_info "Starting clean build..."
        $DOCKER_COMPOSE run --rm mdk-predator-build-clean
    else
        print_info "Starting build..."
        $DOCKER_COMPOSE run --rm mdk-predator-build
    fi
    
    echo ""
    print_info "Build completed!"
    echo ""
    print_info "Output files:"
    ls -lh build/docker-output/ 2>/dev/null || print_warning "No output files found"
    echo ""
    print_info "Next steps:"
    echo "  1. Copy build/docker-output/mdk_predator.ppma to SD card /APPS/ directory"
    echo "  2. Copy build/docker-output/mdk_predator.conf to SD card /MDK-PREDATOR/config/"
    echo "  3. Insert SD card into PortaPack"
    echo "  4. Launch from Apps menu"
    echo ""
fi
