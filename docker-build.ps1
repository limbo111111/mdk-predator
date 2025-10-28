#
# MDK-Predator Docker Build Script for Windows (PowerShell)
#
# This script builds mdk-predator using Docker on Windows
# without any local dependency issues!
#

param(
    [switch]$Clean,
    [switch]$UpdateFirmware,
    [switch]$Ninja,
    [switch]$Shell,
    [switch]$Rebuild,
    [switch]$Help
)

# Colors for output
function Write-Info {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor Green
}

function Write-Warning {
    param([string]$Message)
    Write-Host "[WARN] $Message" -ForegroundColor Yellow
}

function Write-Error {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor Red
}

function Show-Usage {
    Write-Host @"
Usage: .\docker-build.ps1 [OPTIONS]

Build MDK-Predator using Docker (no local dependencies required!)

OPTIONS:
    -Clean              Clean build (remove previous build artifacts)
    -UpdateFirmware     Update mayhem firmware from git (includes submodules)
    -Ninja              Use Ninja build system (faster than Make)
    -Shell              Open interactive shell in container for debugging
    -Rebuild            Rebuild Docker image (use after pulling updates)
    -Help               Show this help message

EXAMPLES:
    # Standard build
    .\docker-build.ps1

    # Clean build
    .\docker-build.ps1 -Clean

    # Build with Ninja (faster)
    .\docker-build.ps1 -Ninja

    # Clean build with Ninja and update firmware
    .\docker-build.ps1 -Clean -UpdateFirmware -Ninja

    # Open shell for debugging
    .\docker-build.ps1 -Shell

    # Rebuild Docker image (after git pull or build errors)
    .\docker-build.ps1 -Rebuild

TROUBLESHOOTING:
    If you get build errors after pulling updates, try:
    .\docker-build.ps1 -Rebuild -Clean -UpdateFirmware

REQUIREMENTS:
    - Docker Desktop for Windows

OUTPUT:
    Built files will be in: build\docker-output\mdk_predator.ppma

"@
}

if ($Help) {
    Show-Usage
    exit 0
}

# Check if Docker is installed
try {
    $null = docker --version
}
catch {
    Write-Error "Docker is not installed!"
    Write-Host ""
    Write-Host "Please install Docker Desktop for Windows:"
    Write-Host "https://docs.docker.com/desktop/install/windows-install/"
    Write-Host ""
    exit 1
}

# Check if docker-compose is available
$DockerCompose = "docker-compose"
try {
    $null = & docker-compose version 2>$null
}
catch {
    try {
        $null = docker compose version
        $DockerCompose = "docker", "compose"
    }
    catch {
        Write-Error "docker-compose is not installed!"
        Write-Host ""
        Write-Host "docker-compose is usually included with Docker Desktop."
        Write-Host ""
        exit 1
    }
}

# Rebuild image if requested
if ($Rebuild) {
    Write-Info "Rebuilding Docker image..."
    & $DockerCompose build --no-cache
}

# Create output directory
if (!(Test-Path "build\docker-output")) {
    New-Item -ItemType Directory -Path "build\docker-output" | Out-Null
}

# Set environment variables
$env:CLEAN_BUILD = if ($Clean) { "1" } else { "0" }
$env:UPDATE_FIRMWARE = if ($UpdateFirmware) { "1" } else { "0" }

Write-Info "MDK-Predator Docker Build"
Write-Host ""
Write-Info "Configuration:"
Write-Host "  - Clean build: $(if ($Clean) { 'Yes' } else { 'No' })"
Write-Host "  - Update firmware: $(if ($UpdateFirmware) { 'Yes' } else { 'No' })"
Write-Host "  - Build system: $(if ($Ninja) { 'Ninja' } else { 'Make' })"
Write-Host "  - Output directory: build\docker-output\"
Write-Host ""

if ($Shell) {
    Write-Info "Opening interactive shell..."
    & $DockerCompose run --rm mdk-predator-shell
}
else {
    if ($Ninja) {
        Write-Info "Building with Ninja..."
        & $DockerCompose run --rm mdk-predator-build-ninja
    }
    elseif ($Clean) {
        Write-Info "Starting clean build..."
        & $DockerCompose run --rm mdk-predator-build-clean
    }
    else {
        Write-Info "Starting build..."
        & $DockerCompose run --rm mdk-predator-build
    }
    
    Write-Host ""
    Write-Info "Build completed!"
    Write-Host ""
    Write-Info "Output files:"
    if (Test-Path "build\docker-output") {
        Get-ChildItem "build\docker-output" | Format-Table Name, Length, LastWriteTime
    }
    else {
        Write-Warning "No output files found"
    }
    Write-Host ""
    Write-Info "Next steps:"
    Write-Host "  1. Copy build\docker-output\mdk_predator.ppma to SD card /APPS/ directory"
    Write-Host "  2. Copy build\docker-output\mdk_predator.conf to SD card /MDK-PREDATOR/config/"
    Write-Host "  3. Insert SD card into PortaPack"
    Write-Host "  4. Launch from Apps menu"
    Write-Host ""
}
