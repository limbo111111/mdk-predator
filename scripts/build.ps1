#
# MDK-Predator Windows Build Script
# PowerShell script to build MDK-Predator library on Windows
#

param(
    [switch]$Clean,
    [switch]$Test,
    [switch]$InstallDeps,
    [switch]$Help,
    [string]$Target = "all"
)

# Script configuration
$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RootDir = Split-Path -Parent $ScriptDir
$BuildDir = Join-Path $RootDir "build"
$ObjDir = Join-Path $BuildDir "obj"
$LibDir = Join-Path $BuildDir "lib"

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
MDK-Predator Windows Build Script
==================================

Usage: .\build.ps1 [OPTIONS]

Build MDK-Predator library on Windows using ARM toolchain.

OPTIONS:
    -InstallDeps        Install required dependencies (ARM toolchain, Make)
    -Clean              Clean build artifacts before building
    -Test               Run tests after building
    -Target <name>      Build specific target (default: all)
                        Options: all, automotive, wireless, crypto
    -Help               Show this help message

EXAMPLES:
    # Install dependencies and build
    .\build.ps1 -InstallDeps

    # Build library
    .\build.ps1

    # Clean build
    .\build.ps1 -Clean

    # Build and test
    .\build.ps1 -Test

    # Build specific module
    .\build.ps1 -Target automotive

REQUIREMENTS:
    - ARM toolchain (arm-none-eabi-gcc) (can be auto-installed with -InstallDeps)
    - GNU Make (via MinGW, Cygwin, or standalone) (can be auto-installed with -InstallDeps)
    - PowerShell 5.0+

For PortaPack application build, use build_portapack_app.ps1

"@
}

function Test-Dependency {
    param(
        [string]$Command,
        [string]$Name,
        [string]$Version = ""
    )
    
    Write-Info "Checking for $Name..."
    
    $result = Get-Command $Command -ErrorAction SilentlyContinue
    if (-not $result) {
        Write-Error-Custom "$Name not found. Please install $Name."
        return $false
    }
    
    Write-Info "$Name found: $($result.Path)"
    
    if ($Version) {
        try {
            $versionOutput = & $Command $Version 2>&1 | Select-Object -First 1
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
        Write-Host "  - Make: choco install make (or via MinGW/Cygwin/Git Bash)"
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
    
    # Install Make
    Write-Info "Installing Make..."
    choco install make -y
    
    # Install GCC for tests
    Write-Info "Installing MinGW GCC (for tests)..."
    choco install mingw -y
    
    Write-Host ""
    Write-Info "Dependencies installed successfully!"
    Write-Info "You may need to restart your terminal for PATH changes to take effect."
    Write-Host ""
    
    # Refresh environment variables
    $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
}

function Test-AllDependencies {
    Write-Info "Checking build dependencies..."
    Write-Host ""
    
    $missingDeps = @()
    
    # Check for ARM toolchain
    if (-not (Test-Dependency "arm-none-eabi-gcc" "ARM GCC Toolchain" "--version")) {
        $missingDeps += "arm-none-eabi-gcc"
        Write-Host ""
    }
    
    # Check for make
    if (-not (Test-Dependency "make" "GNU Make" "--version")) {
        $missingDeps += "make"
        Write-Host ""
    }
    
    # Check for ar (part of ARM toolchain)
    if (Test-Dependency "arm-none-eabi-ar" "ARM Archiver" "--version") {
        # Good
    }
    
    # Check for gcc if testing
    if ($Test) {
        if (-not (Test-Dependency "gcc" "GCC (for tests)" "--version")) {
            $missingDeps += "gcc"
            Write-Host ""
        }
    }
    
    if ($missingDeps.Count -gt 0) {
        Write-Error-Custom "Missing dependencies: $($missingDeps -join ', ')"
        Write-Host ""
        Write-Info "You can install them automatically with: .\build.ps1 -InstallDeps"
        Write-Host ""
        exit 1
    }
    
    Write-Host ""
    Write-Info "All dependencies satisfied!"
    Write-Host ""
}
}

function Invoke-Clean {
    Write-Info "Cleaning build artifacts..."
    
    if (Test-Path $BuildDir) {
        Remove-Item -Recurse -Force $BuildDir
        Write-Info "Build directory cleaned"
    } else {
        Write-Info "Build directory does not exist, nothing to clean"
    }
}

function Invoke-Build {
    param([string]$BuildTarget = "all")
    
    Write-Info "Building MDK-Predator ($BuildTarget)..."
    
    # Change to root directory
    Push-Location $RootDir
    
    try {
        # Use make to build
        if ($BuildTarget -eq "all") {
            & make
        } else {
            & make $BuildTarget
        }
        
        if ($LASTEXITCODE -ne 0) {
            Write-Error-Custom "Build failed with exit code $LASTEXITCODE"
            exit $LASTEXITCODE
        }
        
        Write-Host ""
        Write-Info "Build successful!"
        Write-Host ""
        
        # Show output
        $libPath = Join-Path $LibDir "libmdk_predator.a"
        if (Test-Path $libPath) {
            $libSize = (Get-Item $libPath).Length
            Write-Info "Library created: $libPath"
            Write-Info "Library size: $([math]::Round($libSize/1KB, 2)) KB"
        }
        
    } finally {
        Pop-Location
    }
}

function Invoke-Tests {
    Write-Info "Running tests..."
    
    Push-Location $RootDir
    
    try {
        & make test
        
        if ($LASTEXITCODE -ne 0) {
            Write-Error-Custom "Tests failed with exit code $LASTEXITCODE"
            exit $LASTEXITCODE
        }
        
        Write-Host ""
        Write-Info "All tests passed!"
        Write-Host ""
        
    } finally {
        Pop-Location
    }
}

function Show-BuildInfo {
    Write-Host ""
    Write-Host "MDK-Predator Build Configuration" -ForegroundColor Cyan
    Write-Host "=================================" -ForegroundColor Cyan
    Write-Host "Root Directory:  $RootDir"
    Write-Host "Build Directory: $BuildDir"
    Write-Host "Target:          $Target"
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
    
    # Show build info
    Show-BuildInfo
    
    # Check dependencies
    Test-AllDependencies
    
    # Clean if requested
    if ($Clean) {
        Invoke-Clean
    }
    
    # Build
    Invoke-Build -BuildTarget $Target
    
    # Test if requested
    if ($Test) {
        Invoke-Tests
    }
    
    # Success message
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "Build Completed Successfully!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "Library location: $(Join-Path $LibDir "libmdk_predator.a")"
    Write-Host ""
    Write-Host "Next steps:"
    Write-Host "  - Use library in your embedded project"
    Write-Host "  - Build PortaPack app with build_portapack_app.ps1"
    Write-Host "  - Run tests with: .\build.ps1 -Test"
    Write-Host ""
}

# Run main function
Main
