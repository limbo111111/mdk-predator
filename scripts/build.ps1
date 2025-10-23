#
# MDK-Predator Windows Build Script
# PowerShell script to build MDK-Predator library on Windows
#

param(
    [switch]$Clean,
    [switch]$Test,
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
    -Clean          Clean build artifacts before building
    -Test           Run tests after building
    -Target <name>  Build specific target (default: all)
                    Options: all, automotive, wireless, crypto
    -Help           Show this help message

EXAMPLES:
    # Build library
    .\build.ps1

    # Clean build
    .\build.ps1 -Clean

    # Build and test
    .\build.ps1 -Test

    # Build specific module
    .\build.ps1 -Target automotive

REQUIREMENTS:
    - ARM toolchain (arm-none-eabi-gcc)
    - GNU Make (via MinGW, Cygwin, or standalone)
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

function Test-AllDependencies {
    Write-Info "Checking build dependencies..."
    Write-Host ""
    
    $allGood = $true
    
    # Check for ARM toolchain
    if (-not (Test-Dependency "arm-none-eabi-gcc" "ARM GCC Toolchain" "--version")) {
        $allGood = $false
        Write-Host ""
        Write-Warning-Custom "ARM toolchain not found."
        Write-Host "Install from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads"
        Write-Host ""
    }
    
    # Check for make
    if (-not (Test-Dependency "make" "GNU Make" "--version")) {
        $allGood = $false
        Write-Host ""
        Write-Warning-Custom "GNU Make not found."
        Write-Host "Install options:"
        Write-Host "  1. MinGW: https://sourceforge.net/projects/mingw/"
        Write-Host "  2. Cygwin: https://www.cygwin.com/"
        Write-Host "  3. Chocolatey: choco install make"
        Write-Host "  4. Git Bash includes make"
        Write-Host ""
    }
    
    # Check for ar (part of ARM toolchain)
    if (Test-Dependency "arm-none-eabi-ar" "ARM Archiver" "--version") {
        # Good
    }
    
    if (-not $allGood) {
        Write-Error-Custom "Missing required dependencies. Please install them and try again."
        exit 1
    }
    
    Write-Host ""
    Write-Info "All dependencies satisfied!"
    Write-Host ""
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
