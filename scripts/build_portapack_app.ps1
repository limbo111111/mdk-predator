#
# MDK-Predator PortaPack Application Build Script for Windows
# PowerShell script to build PortaPack Mayhem external application
#

param(
    [string]$MayhemPath = "",
    [string]$OutputDir = "",
    [switch]$Clean,
    [switch]$Help
)

# Script configuration
$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RootDir = Split-Path -Parent $ScriptDir

# Default output directory
if (-not $OutputDir) {
    $OutputDir = Join-Path $RootDir "build\portapack"
}

# Colors for output
function Write-Info {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor Green
}

function Write-Warning-Custom {
    param([string]$Message)
    Write-Host "[WARN] $Message" -ForegroundColor Yellow
}

function Write-Error-Custom {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor Red
}

function Show-Usage {
    Write-Host @"
MDK-Predator PortaPack Application Build Script for Windows
============================================================

Usage: .\build_portapack_app.ps1 [OPTIONS]

Build MDK-Predator as a PortaPack Mayhem external application.

OPTIONS:
    -MayhemPath <path>  Path to PortaPack Mayhem firmware source (REQUIRED)
    -OutputDir <path>   Output directory for built application
                        Default: build\portapack
    -Clean              Clean before building
    -Help               Show this help message

EXAMPLES:
    # Build with Mayhem firmware at specific path
    .\build_portapack_app.ps1 -MayhemPath "C:\portapack-mayhem"

    # Clean build
    .\build_portapack_app.ps1 -MayhemPath "C:\portapack-mayhem" -Clean

    # Custom output directory
    .\build_portapack_app.ps1 -MayhemPath "C:\portapack-mayhem" -OutputDir "C:\output"

REQUIREMENTS:
    - PortaPack Mayhem firmware source
    - ARM toolchain (arm-none-eabi-gcc)
    - CMake 3.16+
    - Python 3.7+
    - GNU Make

NOTES:
    - Ensure all dependencies are in your PATH
    - Build process integrates MDK-Predator with Mayhem firmware
    - Output will be a .ppma file for PortaPack

"@
}

function Test-Dependency {
    param(
        [string]$Command,
        [string]$Name,
        [string]$VersionArg = "--version"
    )
    
    Write-Info "Checking for $Name..."
    
    $result = Get-Command $Command -ErrorAction SilentlyContinue
    if (-not $result) {
        Write-Error-Custom "$Name not found. Please install $Name."
        return $false
    }
    
    Write-Info "$Name found: $($result.Path)"
    
    if ($VersionArg) {
        try {
            $versionOutput = & $Command $VersionArg 2>&1 | Select-Object -First 1
            Write-Info "  Version: $versionOutput"
        } catch {
            # Version check failed, but tool exists
        }
    }
    
    return $true
}

function Test-AllDependencies {
    Write-Info "Checking build requirements..."
    Write-Host ""
    
    $allGood = $true
    
    # Check for ARM toolchain
    if (-not (Test-Dependency "arm-none-eabi-gcc" "ARM GCC Toolchain")) {
        $allGood = $false
        Write-Host ""
        Write-Warning-Custom "ARM toolchain not found."
        Write-Host "Install from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads"
        Write-Host ""
    }
    
    # Check for CMake
    if (-not (Test-Dependency "cmake" "CMake")) {
        $allGood = $false
        Write-Host ""
        Write-Warning-Custom "CMake not found."
        Write-Host "Install from: https://cmake.org/download/"
        Write-Host "Or with Chocolatey: choco install cmake"
        Write-Host ""
    }
    
    # Check for Python
    if (-not (Test-Dependency "python" "Python" "--version")) {
        # Try python3
        if (-not (Test-Dependency "python3" "Python3" "--version")) {
            $allGood = $false
            Write-Host ""
            Write-Warning-Custom "Python not found."
            Write-Host "Install from: https://www.python.org/downloads/"
            Write-Host "Or with Chocolatey: choco install python"
            Write-Host ""
        }
    }
    
    # Check for make
    if (-not (Test-Dependency "make" "GNU Make")) {
        $allGood = $false
        Write-Host ""
        Write-Warning-Custom "GNU Make not found."
        Write-Host "Install options:"
        Write-Host "  1. MinGW: https://sourceforge.net/projects/mingw/"
        Write-Host "  2. Cygwin: https://www.cygwin.com/"
        Write-Host "  3. Chocolatey: choco install make"
        Write-Host ""
    }
    
    if (-not $allGood) {
        Write-Error-Custom "Missing required dependencies. Please install them and try again."
        exit 1
    }
    
    Write-Host ""
    Write-Info "All requirements satisfied!"
    Write-Host ""
}

function Test-MayhemPath {
    if (-not $MayhemPath) {
        Write-Error-Custom "Mayhem firmware path not specified. Use -MayhemPath parameter."
        Write-Host ""
        Show-Usage
        exit 1
    }
    
    if (-not (Test-Path $MayhemPath)) {
        Write-Error-Custom "Mayhem firmware directory not found: $MayhemPath"
        exit 1
    }
    
    $cmakePath = Join-Path $MayhemPath "CMakeLists.txt"
    if (-not (Test-Path $cmakePath)) {
        Write-Error-Custom "Invalid Mayhem firmware directory (CMakeLists.txt not found)"
        exit 1
    }
    
    Write-Info "Mayhem firmware found at: $MayhemPath"
    Write-Host ""
}

function Invoke-Integration {
    Write-Info "Integrating MDK-Predator with Mayhem firmware..."
    
    $externalDir = Join-Path $MayhemPath "firmware\application\external\mdk_predator"
    
    # Create external app directory
    if (-not (Test-Path $externalDir)) {
        New-Item -ItemType Directory -Path $externalDir -Force | Out-Null
    }
    
    # Copy application files
    Write-Info "Copying application files..."
    $appDir = Join-Path $RootDir "app"
    if (Test-Path $appDir) {
        Copy-Item -Path "$appDir\*" -Destination $externalDir -Recurse -Force
    }
    
    # Copy source files
    Write-Info "Copying source files..."
    $srcDir = Join-Path $RootDir "src"
    $includeDir = Join-Path $RootDir "include"
    
    if (Test-Path $srcDir) {
        Copy-Item -Path $srcDir -Destination $externalDir -Recurse -Force
    }
    if (Test-Path $includeDir) {
        Copy-Item -Path $includeDir -Destination $externalDir -Recurse -Force
    }
    
    # Copy configuration
    Write-Info "Copying configuration..."
    $configFile = Join-Path $RootDir "mdk_predator.conf"
    if (Test-Path $configFile) {
        Copy-Item -Path $configFile -Destination $externalDir -Force
    }
    
    Write-Info "Integration complete"
    Write-Host ""
}

function Invoke-Clean {
    Write-Info "Cleaning build artifacts..."
    
    $buildDir = Join-Path $MayhemPath "build"
    if (Test-Path $buildDir) {
        Remove-Item -Recurse -Force $buildDir -ErrorAction SilentlyContinue
    }
    
    $externalDir = Join-Path $MayhemPath "firmware\application\external\mdk_predator"
    if (Test-Path $externalDir) {
        Remove-Item -Recurse -Force $externalDir -ErrorAction SilentlyContinue
    }
    
    Write-Info "Clean complete"
    Write-Host ""
}

function Invoke-Build {
    Write-Info "Building PortaPack firmware with MDK-Predator..."
    
    $buildDir = Join-Path $MayhemPath "build"
    
    # Create build directory
    if (-not (Test-Path $buildDir)) {
        New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
    }
    
    # Change to build directory
    Push-Location $buildDir
    
    try {
        # Configure with CMake
        Write-Info "Running CMake configuration..."
        & cmake ..
        
        if ($LASTEXITCODE -ne 0) {
            Write-Error-Custom "CMake configuration failed"
            exit 1
        }
        
        Write-Host ""
        
        # Build external apps
        Write-Info "Building external applications..."
        & make external_apps
        
        if ($LASTEXITCODE -ne 0) {
            Write-Error-Custom "Build failed"
            exit 1
        }
        
        Write-Host ""
        Write-Info "Build complete"
        Write-Host ""
        
    } finally {
        Pop-Location
    }
}

function Copy-Output {
    Write-Info "Copying built application..."
    
    $appFile = Join-Path $MayhemPath "firmware\application\external\mdk_predator.ppma"
    
    if (-not (Test-Path $appFile)) {
        Write-Error-Custom "Built application not found: $appFile"
        Write-Warning-Custom "Check build logs for errors"
        exit 1
    }
    
    # Create output directory
    if (-not (Test-Path $OutputDir)) {
        New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
    }
    
    # Copy application
    Copy-Item -Path $appFile -Destination $OutputDir -Force
    
    # Copy configuration
    $configFile = Join-Path $RootDir "mdk_predator.conf"
    if (Test-Path $configFile) {
        Copy-Item -Path $configFile -Destination $OutputDir -Force
    }
    
    # Create README
    $readmeContent = @"
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
"@
    
    $readmePath = Join-Path $OutputDir "README.txt"
    Set-Content -Path $readmePath -Value $readmeContent -Force
    
    Write-Host ""
    Write-Info "Application copied to: $OutputDir"
    Write-Info "  - mdk_predator.ppma"
    Write-Info "  - mdk_predator.conf"
    Write-Info "  - README.txt"
    Write-Host ""
}

function Show-BuildInfo {
    Write-Host ""
    Write-Host "MDK-Predator PortaPack Build Configuration" -ForegroundColor Cyan
    Write-Host "===========================================" -ForegroundColor Cyan
    Write-Host "MDK Root:     $RootDir"
    Write-Host "Mayhem Path:  $MayhemPath"
    Write-Host "Output Dir:   $OutputDir"
    Write-Host ""
}

# Main script execution
function Main {
    if ($Help) {
        Show-Usage
        exit 0
    }
    
    # Show build info
    Show-BuildInfo
    
    # Verify Mayhem path
    Test-MayhemPath
    
    # Check dependencies
    Test-AllDependencies
    
    # Clean if requested
    if ($Clean) {
        Invoke-Clean
    }
    
    # Build process
    Invoke-Integration
    Invoke-Build
    Copy-Output
    
    # Success message
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "Build Successful!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "Application built: $(Join-Path $OutputDir "mdk_predator.ppma")"
    Write-Host ""
    Write-Host "Next steps:"
    Write-Host "  1. Copy mdk_predator.ppma to SD card /APPS/ directory"
    Write-Host "  2. Copy mdk_predator.conf to SD card /MDK-PREDATOR/config/"
    Write-Host "  3. Insert SD card into PortaPack"
    Write-Host "  4. Launch from Apps menu"
    Write-Host ""
    Write-Host "See docs/DEPLOYMENT.md for detailed installation instructions."
    Write-Host ""
}

# Run main function
Main
