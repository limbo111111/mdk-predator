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
        # Clone shallow by default
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

    # If a specific channel/branch is requested (e.g. nightly), try to switch to it
    if [ -n "${MAYHEM_CHANNEL:-}" ]; then
        print_step "Requested mayhem channel: $MAYHEM_CHANNEL"
        # Check if remote has the branch
        if git ls-remote --heads origin "$MAYHEM_CHANNEL" | grep -q refs/heads/; then
            print_info "Remote branch '$MAYHEM_CHANNEL' found, checking out..."
            # Fetch and check out the remote branch
            git fetch origin "$MAYHEM_CHANNEL" --depth=1 || true
            git checkout -B "$MAYHEM_CHANNEL" "origin/$MAYHEM_CHANNEL" || {
                print_warning "Could not checkout origin/$MAYHEM_CHANNEL, falling back to current HEAD"
            }
            git submodule update --init --recursive || true
        else
            print_warning "Remote branch '$MAYHEM_CHANNEL' not found; using default branch instead"
        fi
    fi

    # If a specific version/tag/commit was requested, try to check it out
    if [ -n "${MAYHEM_VERSION:-}" ]; then
        print_step "Requested mayhem version: $MAYHEM_VERSION"
        # Try to fetch tags and checkout the version
        if git rev-parse --verify --quiet "$MAYHEM_VERSION" >/dev/null; then
            print_info "Local ref '$MAYHEM_VERSION' found, checking out"
            git checkout -B build_target "$MAYHEM_VERSION" || true
        else
            print_info "Fetching tags/refs and attempting to checkout '$MAYHEM_VERSION'"
            git fetch --depth=1 origin "$MAYHEM_VERSION" || git fetch --tags || true
            if git rev-parse --verify --quiet "origin/$MAYHEM_VERSION" >/dev/null; then
                git checkout -B build_target "origin/$MAYHEM_VERSION" || true
            elif git rev-parse --verify --quiet "$MAYHEM_VERSION" >/dev/null; then
                git checkout -B build_target "$MAYHEM_VERSION" || true
            else
                print_warning "Requested mayhem version '$MAYHEM_VERSION' not found; using default branch"
            fi
        fi
        git submodule update --init --recursive || true
    fi

    # Print selected commit info
    if git rev-parse --git-dir >/dev/null 2>&1; then
        local mayhem_head
        mayhem_head=$(git rev-parse --short HEAD 2>/dev/null || echo "unknown")
        local mayhem_info
        mayhem_info=$(git show -s --format='%ci %h %s' HEAD 2>/dev/null || echo "unknown")
        print_info "Using mayhem-firmware commit: $mayhem_head"
        print_info "Commit info: $mayhem_info"
    fi
}

# Function to verify submodules are initialized
download_libopencm3() {
    print_info "Downloading libopencm3 from HackRF repository..."

    local libopencm3_path="/workspace/mayhem-firmware/hackrf/firmware/libopencm3"
    local hackrf_repo="https://github.com/portapack-mayhem/hackrf.git"
    local hackrf_commit="cf6815aaf9c4bb11c3ad3de97721c67ddf4fcb38"

    # Check if git is installed
    if ! command -v git &> /dev/null; then
        print_error "Git is not installed. Please install git first."
        exit 1
    fi

    # Create hackrf/firmware directory if it doesn't exist
    mkdir -p /workspace/mayhem-firmware/hackrf/firmware

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
        cd /workspace
        rm -rf "$temp_dir"
        exit 1
    fi

    # Copy libopencm3 to the target location
    print_info "Copying libopencm3 to $libopencm3_path"
    cp -r libopencm3 "$libopencm3_path"

    cd /workspace

    # Clean up temporary directory
    rm -rf "$temp_dir"

    print_info "✓ libopencm3 downloaded successfully"

    # Verify that libopencm3 contains expected Makefile; if not, fallback to official libopencm3 repo
    if [ ! -f "$libopencm3_path/Makefile" ]; then
        print_warning "libopencm3 Makefile not found after copy — attempting fallback clone from official libopencm3 repository"
        rm -rf "$libopencm3_path"
        mkdir -p "$(dirname \"$libopencm3_path\")"
        if git clone --depth 1 https://github.com/libopencm3/libopencm3.git "$libopencm3_path"; then
            print_info "Fallback clone of libopencm3 succeeded"
        else
            print_error "Fallback clone of libopencm3 failed — cannot continue"
            rm -rf "$temp_dir"
            exit 1
        fi
    fi
}

verify_submodules() {
    print_step "Verifying git submodules are initialized..."

    cd /workspace/mayhem-firmware

    # Check for critical submodules that must exist for successful build
    # Note: nvic.h is generated during build, so we check for source files instead
    local critical_paths=(
        "hackrf/firmware/libopencm3"
        "hackrf/firmware/libopencm3/include/libopencm3/lpc43xx"
        "hackrf/firmware/libopencm3/include/libopencm3/lpc43xx/m0/irq.yaml"
    )

    local missing_submodules=0

    for path in "${critical_paths[@]}"; do
        if [ ! -e "$path" ]; then
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
        if [ -d ".git" ]; then
            print_info "Attempting to initialize git submodules..."
            if git submodule update --init --recursive 2>&1; then
                print_info "Submodules initialized successfully via git"

                # Verify again
                missing_submodules=0
                for path in "${critical_paths[@]}"; do
                    if [ ! -e "$path" ]; then
                        missing_submodules=1
                    fi
                done
            else
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
                if [ ! -e "$path" ]; then
                    print_error "Still missing: $path after download"
                    exit 1
                fi
            done
        fi
    fi

    print_info "✓ All required submodules are properly initialized"
    cd /workspace
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
        if ! (grep -q "external/mdk_predator/src/crypto/crypto_analyzer.c" "$external_cmake" 2>/dev/null && grep -q "mdk_predator" "$external_cmake" 2>/dev/null); then
            # Backup the original file
            cp "$external_cmake" "$external_cmake.backup"

            # Add MDK-Predator sources to EXTCPPSRC and app to EXTAPPLIST
            awk '
                BEGIN { done=0 }
                /^set\(EXTCPPSRC/ {
                    if (!done) {
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
                        done=1
                        next
                    }
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

# Function to build libopencm3
build_libopencm3() {
    print_step "Building libopencm3 library..."

    local libopencm3_path="/workspace/mayhem-firmware/hackrf/firmware/libopencm3"

    if [ ! -d "$libopencm3_path" ]; then
        print_error "libopencm3 directory not found: $libopencm3_path"
        print_error "Please run verify_submodules first"
        exit 1
    fi

    cd "$libopencm3_path"

    # Check if already built
    if [ -f "lib/libopencm3_lpc43xx.a" ] && [ -f "include/libopencm3/lpc43xx/m0/nvic.h" ]; then
        print_info "libopencm3 already built, skipping..."
        cd /workspace
        return 0
    fi

    print_info "Building libopencm3 for LPC43xx (this may take a few minutes)..."

    # Build libopencm3
    if ! make TARGETS="lpc43xx/m4 lpc43xx/m0" 2>&1; then
        print_error "libopencm3 build failed"
        cd /workspace
        exit 1
    fi

    # Verify the generated header exists
    if [ ! -f "include/libopencm3/lpc43xx/m0/nvic.h" ]; then
        print_error "nvic.h was not generated during libopencm3 build"
        print_error "This is unexpected - please check the build logs"
        cd /workspace
        exit 1
    fi

    print_info "✓ libopencm3 built successfully"
    cd /workspace
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

    # Verify submodules are initialized
    verify_submodules

    # Build libopencm3 first (generates nvic.h and other headers)
    build_libopencm3

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
