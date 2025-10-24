#
# MDK-Predator PortaPack Application Build Script for Windows
# PowerShell script to build PortaPack Mayhem external application
#

param(
    [string]$MayhemPath = "",
    [string]$OutputDir = "",
    [switch]$Clean,
    [switch]$InstallDeps,
    [switch]$DownloadFirmware,
    [switch]$Help
)

# Script configuration
$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RootDir = Split-Path -Parent $ScriptDir
$DefaultMayhemClonePath = Join-Path $RootDir "build\mayhem-firmware"

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
    -MayhemPath <path>     Path to PortaPack Mayhem firmware source
    -OutputDir <path>      Output directory for built application
                           Default: build\portapack
    -Clean                 Clean before building
    -InstallDeps           Install required dependencies (ARM toolchain, CMake, Python)
    -DownloadFirmware      Download Mayhem firmware if not present
    -Help                  Show this help message

EXAMPLES:
    # Install dependencies and download firmware automatically
    .\build_portapack_app.ps1 -InstallDeps -DownloadFirmware

    # Build with Mayhem firmware at specific path
    .\build_portapack_app.ps1 -MayhemPath "C:\portapack-mayhem"

    # Install deps, download firmware, and build in one command
    .\build_portapack_app.ps1 -InstallDeps -DownloadFirmware

    # Clean build
    .\build_portapack_app.ps1 -MayhemPath "C:\portapack-mayhem" -Clean

    # Custom output directory
    .\build_portapack_app.ps1 -MayhemPath "C:\portapack-mayhem" -OutputDir "C:\output"

REQUIREMENTS:
    - PortaPack Mayhem firmware source (can be auto-downloaded with -DownloadFirmware)
    - ARM toolchain (arm-none-eabi-gcc) (can be auto-installed with -InstallDeps)
    - CMake 3.16+ (can be auto-installed with -InstallDeps)
    - Python 3.7+ (can be auto-installed with -InstallDeps)
    - GNU Make (can be auto-installed with -InstallDeps)

NOTES:
    - Ensure all dependencies are in your PATH
    - Build process integrates MDK-Predator with Mayhem firmware
    - Output will be a .ppma file for PortaPack
    - Use -InstallDeps to install via Chocolatey (requires admin rights)

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
            Write-Info "Version: $versionOutput"
        } catch {
            # Version check failed, but tool exists
        }
    }
    
    return $true
}

function Install-Dependencies {
    Write-Info "Installing build dependencies using Chocolatey..."
    Write-Host ""
    
    # Check if running as administrator
    $isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
    
    if (-not $isAdmin) {
        Write-Error-Custom "Administrator privileges required to install dependencies"
        Write-Host ""
        Write-Host "Please run PowerShell as Administrator and try again:"
        Write-Host "  Right-click PowerShell -> Run as Administrator"
        Write-Host ""
        Write-Host "Alternatively, install dependencies manually:"
        Write-Host "  - ARM GCC Toolchain: https://developer.arm.com/downloads/-/gnu-rm"
        Write-Host "  - CMake: https://cmake.org/download/"
        Write-Host "  - Python: https://www.python.org/downloads/"
        Write-Host "  - Git: https://git-scm.com/download/win"
        Write-Host "  - Make: choco install make (or via MinGW/Cygwin)"
        Write-Host ""
        exit 1
    }
    
    # Check if Chocolatey is installed
    $chocoInstalled = $null -ne (Get-Command choco -ErrorAction SilentlyContinue)
    
    if (-not $chocoInstalled) {
        Write-Info "Chocolatey not found. Installing Chocolatey..."
        Write-Host ""
        
        Set-ExecutionPolicy Bypass -Scope Process -Force
        [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
        
        try {
            Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
            Write-Info "Chocolatey installed successfully"
        } catch {
            Write-Error-Custom "Failed to install Chocolatey: $_"
            Write-Host ""
            Write-Host "Please install Chocolatey manually from: https://chocolatey.org/install"
            Write-Host "Then run this script again with -InstallDeps"
            exit 1
        }
        
        # Refresh environment variables
        $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
    }
    
    Write-Info "Installing dependencies via Chocolatey..."
    Write-Host ""
    
    # Install ARM toolchain
    Write-Info "Installing ARM GCC toolchain..."
    choco install gcc-arm-embedded -y
    
    # Install CMake
    Write-Info "Installing CMake..."
    choco install cmake -y
    
    # Install Python
    Write-Info "Installing Python..."
    choco install python -y
    
    # Install Git
    Write-Info "Installing Git..."
    choco install git -y
    
    # Install Make
    Write-Info "Installing Make..."
    choco install make -y
    
    Write-Host ""
    Write-Info "Dependencies installed successfully!"
    Write-Info "You may need to restart your terminal or computer for PATH changes to take effect."
    Write-Host ""
    
    # Refresh environment variables
    $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
}

function Download-MayhemFirmware {
    Write-Info "Downloading Mayhem firmware..."
    Write-Host ""
    
    # Check if git is installed
    $gitInstalled = $null -ne (Get-Command git -ErrorAction SilentlyContinue)
    
    if (-not $gitInstalled) {
        Write-Error-Custom "Git is not installed. Please install git first."
        Write-Host ""
        Write-Host "Install with: choco install git"
        Write-Host "Or download from: https://git-scm.com/download/win"
        Write-Host ""
        exit 1
    }
    
    # Set default path if not specified
    if (-not $MayhemPath) {
        $script:MayhemPath = $DefaultMayhemClonePath
        Write-Info "Using default firmware location: $MayhemPath"
    }
    
    # Check if directory already exists
    if (Test-Path $MayhemPath) {
        Write-Warning-Custom "Directory already exists: $MayhemPath"
        
        # Check if it's a git repo
        $gitDir = Join-Path $MayhemPath ".git"
        if (Test-Path $gitDir) {
            Write-Info "Updating existing Mayhem firmware..."
            Push-Location $MayhemPath
            
            try {
                git pull
                git submodule update --init --recursive
                Write-Info "Firmware updated successfully"
            } catch {
                Write-Error-Custom "Failed to update firmware: $_"
                exit 1
            } finally {
                Pop-Location
            }
            
            return
        } else {
            Write-Error-Custom "Directory exists but is not a git repository"
            Write-Error-Custom "Please remove or specify a different path with -MayhemPath"
            exit 1
        }
    }
    
    # Clone the firmware
    Write-Info "Cloning Mayhem firmware from GitHub..."
    Write-Info "This may take several minutes..."
    Write-Host ""
    
    try {
        git clone --depth 1 https://github.com/portapack-mayhem/mayhem-firmware.git $MayhemPath
    } catch {
        Write-Error-Custom "Failed to clone firmware: $_"
        exit 1
    }
    
    # Initialize submodules
    Write-Info "Initializing submodules..."
    Push-Location $MayhemPath
    
    try {
        git submodule update --init --recursive
    } catch {
        Write-Error-Custom "Failed to initialize submodules: $_"
        exit 1
    } finally {
        Pop-Location
    }
    
    Write-Host ""
    Write-Info "Mayhem firmware downloaded successfully to: $MayhemPath"
    Write-Host ""
}

function Test-AllDependencies {
    Write-Info "Checking build requirements..."
    Write-Host ""
    
    $missingDeps = @()
    
    # Check for ARM toolchain
    if (-not (Test-Dependency "arm-none-eabi-gcc" "ARM GCC Toolchain")) {
        $missingDeps += "arm-none-eabi-gcc"
        Write-Host ""
    }
    
    # Check for CMake
    if (-not (Test-Dependency "cmake" "CMake")) {
        $missingDeps += "cmake"
        Write-Host ""
    }
    
    # Check for Python
    $pythonFound = $false
    if (Test-Dependency "python" "Python" "--version") {
        $pythonFound = $true
    } elseif (Test-Dependency "python3" "Python3" "--version") {
        $pythonFound = $true
    }
    
    if (-not $pythonFound) {
        $missingDeps += "python"
        Write-Host ""
    }
    
    # Check for make
    if (-not (Test-Dependency "make" "GNU Make")) {
        $missingDeps += "make"
        Write-Host ""
    }
    
    if ($missingDeps.Count -gt 0) {
        Write-Error-Custom "Missing dependencies: $($missingDeps -join ', ')"
        Write-Host ""
        Write-Info "You can install them automatically with: .\build_portapack_app.ps1 -InstallDeps"
        Write-Host ""
        exit 1
    }
    
    Write-Host ""
    Write-Info "All requirements satisfied!"
    Write-Host ""
}

function Test-MayhemPath {
    # If download firmware was requested but no path set, it will be set by download function
    if (-not $MayhemPath) {
        if (-not $DownloadFirmware) {
            Write-Error-Custom "Mayhem firmware path not specified."
            Write-Info "Either use -MayhemPath to specify the path, or use -DownloadFirmware to download automatically."
            Write-Host ""
            Show-Usage
            exit 1
        } else {
            # Path will be set by Download-MayhemFirmware
            return
        }
    }
    
    if (-not (Test-Path $MayhemPath)) {
        Write-Error-Custom "Mayhem firmware directory not found: $MayhemPath"
        Write-Info "Use -DownloadFirmware to download it automatically"
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
    
    # Install dependencies if requested
    if ($InstallDeps) {
        Install-Dependencies
    }
    
    # Download firmware if requested
    if ($DownloadFirmware) {
        Download-MayhemFirmware
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
