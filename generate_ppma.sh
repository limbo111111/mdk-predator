#!/bin/bash
# Generate PPMA file for MDK-Predator

echo "=== MDK-Predator PPMA Generator ==="
echo ""

# Build the application
echo "[1/3] Building application..."
cd /workspaces/mdk-predator/build/mayhem-firmware/build
make application -j$(nproc)

if [ $? -ne 0 ]; then
    echo "❌ Build failed"
    exit 1
fi

echo "✅ Build successful"
echo ""

# Find the generated PPMA file
echo "[2/3] Looking for generated PPMA files..."
PPMA_FILE=$(find firmware/application -name "*.ppma" | grep -i mdk)

if [ -z "$PPMA_FILE" ]; then
    echo "⚠️  No PPMA file found. Checking for external app images..."
    PPMA_FILE=$(find firmware/application -name "*mdk_predator*.ppma" -o -name "external_app_mdk_predator.*")
fi

if [ -z "$PPMA_FILE" ]; then
    echo "❌ Could not find PPMA file"
    echo "Available external app files:"
    find firmware/application -name "external_app_*" | head -20
    exit 1
fi

echo "Found: $PPMA_FILE"
echo ""

# Copy to output directory
echo "[3/3] Copying PPMA to output directory..."
OUTPUT_DIR="/workspaces/mdk-predator/build/output"
mkdir -p "$OUTPUT_DIR"

for file in $PPMA_FILE; do
    cp "$file" "$OUTPUT_DIR/"
    echo "✅ Copied: $(basename $file)"
done

echo ""
echo "=== PPMA Generation Complete ==="
echo "Output files in: $OUTPUT_DIR"
ls -lh "$OUTPUT_DIR"/*.ppma 2>/dev/null || ls -lh "$OUTPUT_DIR"/external_app_mdk_predator.*

echo ""
echo "Copy these files to your PortaPack's SD card in the APPS folder."
