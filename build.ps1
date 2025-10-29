# Simple build script for MDK-Predator PortaPack Application
# Based on the mayhem-firmware standard build process
#
# PowerShell version for Windows

param(
    [string]$MayhemPath = "$PSScriptRoot\build\mayhem-firmware",
    [switch]$Download,
    [switch]$Clean,
    [switch]$Help
)

$ErrorActionPreference = "Stop"

function Write-Info {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor Green
}

function Write-ErrorMsg {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor Red
}

function Write-Warning {
    param([string]$Message)
    Write-Host "[WARN] $Message" -ForegroundColor Yellow
}

function Show-Usage {
    @"
Usage: .\build.ps1 [OPTIONS]

Simple build script for MDK-Predator external PortaPack application.

OPTIONS:
    -MayhemPath PATH    Path to mayhem-firmware (default: build\mayhem-firmware)
    -Download           Download mayhem-firmware automatically
    -Clean              Clean build
    -Help               Show this help

EXAMPLES:
    # Download firmware and build
    .\build.ps1 -Download

    # Build with existing firmware
    .\build.ps1 -MayhemPath C:\path\to\mayhem-firmware

    # Clean build
    .\build.ps1 -Clean -Download

"@
}

if ($Help) {
    Show-Usage
    exit 0
}

# Download mayhem-firmware if requested
if ($Download) {
    if (-not (Test-Path $MayhemPath)) {
        Write-Info "Downloading mayhem-firmware..."
        $parentDir = Split-Path -Parent $MayhemPath
        if (-not (Test-Path $parentDir)) {
            New-Item -ItemType Directory -Path $parentDir -Force | Out-Null
        }
        git clone --depth 1 https://github.com/portapack-mayhem/mayhem-firmware.git $MayhemPath
        Write-Info "Initializing submodules..."
        Push-Location $MayhemPath
        git submodule update --init --recursive --depth 1 hackrf
        Pop-Location
    } else {
        Write-Info "Mayhem firmware already exists at $MayhemPath"
    }
}

# Verify mayhem-firmware exists
if (-not (Test-Path $MayhemPath)) {
    Write-ErrorMsg "Mayhem firmware not found at: $MayhemPath"
    Write-ErrorMsg "Use -Download to download automatically or -MayhemPath to specify path"
    exit 1
}

Write-Info "Using mayhem-firmware at: $MayhemPath"

# Copy app files to external directory
Write-Info "Copying app files to mayhem-firmware..."
$appDir = Join-Path $MayhemPath "firmware\application\external\mdk_predator"
if (-not (Test-Path $appDir)) {
    New-Item -ItemType Directory -Path $appDir -Force | Out-Null
}

# Copy all necessary files
Copy-Item -Path "$PSScriptRoot\app\*" -Destination $appDir -Recurse -Force
Copy-Item -Path "$PSScriptRoot\src" -Destination $appDir -Recurse -Force
Copy-Item -Path "$PSScriptRoot\include" -Destination $appDir -Recurse -Force

Write-Info "Files copied to $appDir"

# Register app in external.cmake
Write-Info "Registering app in external.cmake..."
$externalCmake = Join-Path $MayhemPath "firmware\application\external\external.cmake"

# Check if already registered
$cmakeContent = Get-Content $externalCmake -Raw
if (-not ($cmakeContent -match "mdk_predator")) {
    # Add source files
    $newEntries = @"

	#mdk_predator
	external/mdk_predator/main.cpp
	external/mdk_predator/mdk_predator_app.cpp
	external/mdk_predator/src/mdk_predator.c
	external/mdk_predator/src/automotive/key_fob_analyzer.c
	external/mdk_predator/src/automotive/rolling_code_tester.c
	external/mdk_predator/src/wireless/wifi_analyzer.c
	external/mdk_predator/src/wireless/bluetooth_analyzer.c
	external/mdk_predator/src/wireless/subghz_analyzer.c
	external/mdk_predator/src/crypto/crypto_analyzer.c
"@
    Add-Content -Path $externalCmake -Value $newEntries

    # Add app to list
    $cmakeContent = Get-Content $externalCmake
    $newContent = @()
    $inAppList = $false
    foreach ($line in $cmakeContent) {
        if ($line -match '^set\(EXTAPPLIST$') {
            $inAppList = $true
        }
        if ($inAppList -and $line -match '^\)$') {
            $newContent += "`tmdk_predator"
            $inAppList = $false
        }
        $newContent += $line
    }
    Set-Content -Path $externalCmake -Value $newContent
    Write-Info "App registered in external.cmake"
} else {
    Write-Info "App already registered in external.cmake"
}

# Build libopencm3 first (required)
Write-Info "Building libopencm3..."
Push-Location (Join-Path $MayhemPath "hackrf\firmware\libopencm3")
make TARGETS=lpc43xx
Pop-Location

# Configure and build
Push-Location $MayhemPath

if ($Clean) {
    Write-Info "Cleaning build directory..."
    Remove-Item -Path "build" -Recurse -Force -ErrorAction SilentlyContinue
}

Write-Info "Configuring build..."
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" -Force | Out-Null
}
Push-Location "build"
cmake ..

Write-Info "Building application..."
cmake --build . --target application -j $env:NUMBER_OF_PROCESSORS

Pop-Location
Pop-Location

# Find and copy output
Write-Info "Copying output files..."
$outputDir = Join-Path $PSScriptRoot "build\output"
if (-not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
}

$ppmaFile = Get-ChildItem -Path (Join-Path $MayhemPath "build") -Filter "mdk_predator.ppma" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1

if ($ppmaFile) {
    Copy-Item -Path $ppmaFile.FullName -Destination $outputDir -Force
    $confFile = Join-Path $PSScriptRoot "mdk_predator.conf"
    if (Test-Path $confFile) {
        Copy-Item -Path $confFile -Destination $outputDir -Force
    }
    Write-Info "Build successful!"
    Write-Info "Output: $(Join-Path $outputDir 'mdk_predator.ppma')"
    Write-Host ""
    Write-Info "Next steps:"
    Write-Host "  1. Copy $(Join-Path $outputDir 'mdk_predator.ppma') to SD card /APPS/ directory"
    Write-Host "  2. Insert SD card into PortaPack and launch from Apps menu"
} else {
    Write-ErrorMsg "Build failed - mdk_predator.ppma not found"
    exit 1
}
