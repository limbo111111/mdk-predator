#
# Simple MDK-Predator Build Script for Windows
# Minimal dependencies, clear error messages
#

param(
    [string]$MayhemPath = "",
    [switch]$Help
)

# Colors
function Write-Step {
    param([string]$Message)
    Write-Host "`n==> $Message" -ForegroundColor Cyan
}

function Write-Success {
    param([string]$Message)
    Write-Host "    [OK] $Message" -ForegroundColor Green
}

function Write-Failure {
    param([string]$Message)
    Write-Host "    [ERROR] $Message" -ForegroundColor Red
}

function Show-Help {
    Write-Host @"
Simple MDK-Predator Build Script for Windows
============================================

This script provides a simplified build process with clear error messages.

Usage: .\simple_build_windows.ps1 [-MayhemPath <path>] [-Help]

OPTIONS:
    -MayhemPath <path>    Path to Mayhem firmware source
    -Help                 Show this help

PREREQUISITES:
    1. Git (https://git-scm.com/download/win)
    2. ARM GCC Toolchain (https://developer.arm.com/downloads/-/gnu-rm)
    3. CMake 3.16+ (https://cmake.org/download/)
    4. Python 3.7+ (https://www.python.org/downloads/)
    5. Make (via Git Bash, MinGW, or choco install make)

EXAMPLE:
    # Download firmware first
    git clone --depth 1 https://github.com/portapack-mayhem/mayhem-firmware.git
    cd mayhem-firmware
    git submodule update --init --recursive
    
    # Then build
    .\scripts\simple_build_windows.ps1 -MayhemPath .\mayhem-firmware

RECOMMENDED ALTERNATIVE:
    Use WSL (Windows Subsystem for Linux) for best compatibility:
    
    wsl --install
    # Then in WSL:
    ./scripts/build_portapack_app_wsl.sh -i -d

"@
}

# Main script
if ($Help) {
    Show-Help
    exit 0
}

# Get script directory
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RootDir = Split-Path -Parent $ScriptDir

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "MDK-Predator Simple Build for Windows" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# Step 1: Check Mayhem path
Write-Step "Checking Mayhem firmware location..."

if (-not $MayhemPath) {
    $defaultPath = Join-Path $RootDir "build\mayhem-firmware"
    if (Test-Path $defaultPath) {
        $MayhemPath = $defaultPath
        Write-Success "Using default Mayhem path: $MayhemPath"
    } else {
        Write-Failure "Mayhem firmware path not specified and default not found"
        Write-Host "`nPlease either:"
        Write-Host "  1. Specify path: -MayhemPath <path>"
        Write-Host "  2. Download first:"
        Write-Host "     git clone --depth 1 https://github.com/portapack-mayhem/mayhem-firmware.git"
        exit 1
    }
}

if (-not (Test-Path $MayhemPath)) {
    Write-Failure "Mayhem firmware not found at: $MayhemPath"
    exit 1
}

$cmakeListsPath = Join-Path $MayhemPath "CMakeLists.txt"
if (-not (Test-Path $cmakeListsPath)) {
    Write-Failure "Invalid Mayhem firmware directory (no CMakeLists.txt)"
    exit 1
}

Write-Success "Mayhem firmware found"

# Step 2: Check dependencies
Write-Step "Checking dependencies..."

$allGood = $true

if (-not (Get-Command arm-none-eabi-gcc -ErrorAction SilentlyContinue)) {
    Write-Failure "ARM GCC not found"
    $allGood = $false
} else {
    Write-Success "ARM GCC found"
}

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Failure "CMake not found"
    $allGood = $false
} else {
    Write-Success "CMake found"
}

if (-not (Get-Command python -ErrorAction SilentlyContinue) -and -not (Get-Command python3 -ErrorAction SilentlyContinue)) {
    Write-Failure "Python not found"
    $allGood = $false
} else {
    Write-Success "Python found"
}

if (-not (Get-Command make -ErrorAction SilentlyContinue)) {
    Write-Failure "Make not found"
    $allGood = $false
} else {
    Write-Success "Make found"
}

if (-not $allGood) {
    Write-Host "`nMissing dependencies detected!"
    Write-Host "Please install missing tools or use: .\build_portapack_app.ps1 -InstallDeps"
    Write-Host "`nAlternatively, use WSL for easier setup:"
    Write-Host "  wsl --install"
    Write-Host "  wsl ./scripts/build_portapack_app_wsl.sh -i -d"
    exit 1
}

# Step 3: Integrate with Mayhem
Write-Step "Integrating MDK-Predator with Mayhem..."

$externalDir = Join-Path $MayhemPath "firmware\application\external\mdk_predator"

# Create directory
if (-not (Test-Path $externalDir)) {
    New-Item -ItemType Directory -Path $externalDir -Force | Out-Null
}

# Copy files
Copy-Item -Path (Join-Path $RootDir "app\*") -Destination $externalDir -Recurse -Force
Copy-Item -Path (Join-Path $RootDir "src") -Destination $externalDir -Recurse -Force
Copy-Item -Path (Join-Path $RootDir "include") -Destination $externalDir -Recurse -Force
Copy-Item -Path (Join-Path $RootDir "mdk_predator.conf") -Destination $externalDir -Force

Write-Success "Files integrated"

# Step 4: Configure with CMake
Write-Step "Configuring build with CMake..."

$buildDir = Join-Path $MayhemPath "build"
if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
}

Push-Location $buildDir

# Try multiple generators
$generators = @(
    @{Name="MinGW Makefiles"; Cmd="mingw32-make"},
    @{Name="Unix Makefiles"; Cmd="make"},
    @{Name="Ninja"; Cmd="ninja"}
)

$configured = $false

foreach ($gen in $generators) {
    Write-Host "  Trying generator: $($gen.Name)..." -ForegroundColor Gray
    
    # Check if build tool is available
    if (-not (Get-Command $gen.Cmd -ErrorAction SilentlyContinue)) {
        Write-Host "    $($gen.Cmd) not available, skipping..." -ForegroundColor Gray
        continue
    }
    
    # Clean cache from previous attempts
    $cacheFile = Join-Path $buildDir "CMakeCache.txt"
    if (Test-Path $cacheFile) {
        Remove-Item $cacheFile -Force -ErrorAction SilentlyContinue
    }
    
    # Try configuration
    $output = & cmake .. -G $gen.Name 2>&1
    
    if ($LASTEXITCODE -eq 0) {
        Write-Success "Configuration successful with $($gen.Name)"
        $configured = $true
        $buildCmd = $gen.Cmd
        break
    }
}

if (-not $configured) {
    Pop-Location
    Write-Failure "CMake configuration failed with all generators"
    Write-Host "`nTroubleshooting:"
    Write-Host "  1. Install MinGW: choco install mingw"
    Write-Host "  2. Or install Ninja: choco install ninja"
    Write-Host "  3. Make sure ARM toolchain is in PATH"
    Write-Host "`nRECOMMENDED: Use WSL instead"
    Write-Host "  wsl ./scripts/build_portapack_app_wsl.sh -i -d"
    exit 1
}

# Step 5: Build
Write-Step "Building external applications..."

if ($buildCmd -eq "ninja") {
    & ninja external_apps
} else {
    & $buildCmd external_apps
}

$buildSuccess = $LASTEXITCODE -eq 0
Pop-Location

if (-not $buildSuccess) {
    Write-Failure "Build failed"
    exit 1
}

Write-Success "Build complete"

# Step 6: Copy output
Write-Step "Copying output files..."

$outputDir = Join-Path $RootDir "build\portapack"
if (-not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
}

# Find the .ppma file
$ppmaLocations = @(
    (Join-Path $MayhemPath "firmware\application\external\mdk_predator.ppma"),
    (Join-Path $MayhemPath "build\firmware\application\external\mdk_predator.ppma")
)

$ppmaFile = $null
foreach ($loc in $ppmaLocations) {
    if (Test-Path $loc) {
        $ppmaFile = $loc
        break
    }
}

if (-not $ppmaFile) {
    Write-Failure "Could not find mdk_predator.ppma"
    Write-Host "`nSearched locations:"
    foreach ($loc in $ppmaLocations) {
        Write-Host "  - $loc"
    }
    exit 1
}

Copy-Item -Path $ppmaFile -Destination $outputDir -Force
Copy-Item -Path (Join-Path $RootDir "mdk_predator.conf") -Destination $outputDir -Force

Write-Success "Output copied to: $outputDir"

# Done!
Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "BUILD SUCCESSFUL!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Output files:"
Write-Host "  $outputDir\mdk_predator.ppma"
Write-Host "  $outputDir\mdk_predator.conf"
Write-Host ""
Write-Host "Installation:"
Write-Host "  1. Copy mdk_predator.ppma to SD card /APPS/"
Write-Host "  2. Copy mdk_predator.conf to SD card /MDK-PREDATOR/config/"
Write-Host "  3. Insert SD card and launch from PortaPack Apps menu"
Write-Host ""
