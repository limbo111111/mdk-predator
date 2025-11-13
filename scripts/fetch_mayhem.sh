#!/bin/bash
set -e

# Usage: ./scripts/fetch_mayhem.sh [version]
# Example: ./scripts/fetch_mayhem.sh 2.3.1

VER=${1:-2.3.1}
DEST=build/mayhem-firmware

echo "Will fetch Mayhem firmware version: $VER into $DEST"
mkdir -p build

if [ -d "$DEST/.git" ]; then
    echo "Mayhem repo already present in $DEST. Updating..."
    cd "$DEST"
    git fetch --all --tags || true
    if git rev-parse --verify --quiet "origin/$VER" >/dev/null; then
        git checkout -B build_target "origin/$VER" || true
    elif git rev-parse --verify --quiet "$VER" >/dev/null; then
        git checkout -B build_target "$VER" || true
    else
        echo "Requested version not found locally; attempting to fetch tags and checkout $VER"
        git fetch --depth=1 origin "$VER" || git fetch --tags || true
        if git rev-parse --verify --quiet "origin/$VER" >/dev/null; then
            git checkout -B build_target "origin/$VER" || true
        else
            echo "Could not checkout $VER; staying on current branch"
        fi
    fi
    cd - >/dev/null
else
    echo "Cloning mayhem-firmware@$VER into $DEST (shallow clone)"
    git clone --depth 1 --branch "$VER" https://github.com/portapack-mayhem/mayhem-firmware.git "$DEST" || {
        echo "Shallow clone failed; trying full clone (this may take longer)"
        rm -rf "$DEST"
        git clone https://github.com/portapack-mayhem/mayhem-firmware.git "$DEST"
        cd "$DEST"
        git checkout "$VER" || true
        cd - >/dev/null
    }
fi

echo "Searching for application_information_t occurrences in the cloned repo..."

grep -R --line-number --color=always "application_information_t" "$DEST" || true

grep -R --line-number --color=always "struct application_information_t" "$DEST" || true

echo "Listing top-level include folders and header samples (up to 100 files)"
find "$DEST" -type f -name '*.h' | head -n 100

echo "Done. Inspect $DEST for the Mayhem headers and structures."
