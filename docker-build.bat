@echo off
REM MDK-Predator Docker Build Script for Windows
REM
REM This script builds mdk-predator using Docker on Windows
REM without any local dependency issues!

setlocal enabledelayedexpansion

REM Default options
set CLEAN_BUILD=0
set UPDATE_FIRMWARE=0
set USE_NINJA=0
set OPEN_SHELL=0
set REBUILD_IMAGE=0

REM Parse arguments
:parse_args
if "%~1"=="" goto check_docker
if /i "%~1"=="-c" set CLEAN_BUILD=1& shift & goto parse_args
if /i "%~1"=="--clean" set CLEAN_BUILD=1& shift & goto parse_args
if /i "%~1"=="-u" set UPDATE_FIRMWARE=1& shift & goto parse_args
if /i "%~1"=="--update-firmware" set UPDATE_FIRMWARE=1& shift & goto parse_args
if /i "%~1"=="-n" set USE_NINJA=1& shift & goto parse_args
if /i "%~1"=="--ninja" set USE_NINJA=1& shift & goto parse_args
if /i "%~1"=="-s" set OPEN_SHELL=1& shift & goto parse_args
if /i "%~1"=="--shell" set OPEN_SHELL=1& shift & goto parse_args
if /i "%~1"=="-r" set REBUILD_IMAGE=1& shift & goto parse_args
if /i "%~1"=="--rebuild" set REBUILD_IMAGE=1& shift & goto parse_args
if /i "%~1"=="-h" goto show_help
if /i "%~1"=="--help" goto show_help
echo [ERROR] Unknown option: %~1
goto show_help

:show_help
echo Usage: %~nx0 [OPTIONS]
echo.
echo Build MDK-Predator using Docker (no local dependencies required!)
echo.
echo OPTIONS:
echo     -c, --clean             Clean build (remove previous build artifacts)
echo     -u, --update-firmware   Update mayhem firmware from git
echo     -n, --ninja             Use Ninja build system (faster)
echo     -s, --shell             Open interactive shell in container
echo     -r, --rebuild           Rebuild Docker image
echo     -h, --help              Show this help message
echo.
echo EXAMPLES:
echo     %~nx0                   Standard build
echo     %~nx0 --clean           Clean build
echo     %~nx0 --ninja           Build with Ninja (faster)
echo     %~nx0 -c -u -n          Clean build with Ninja and update firmware
echo     %~nx0 --shell           Open shell for debugging
echo.
echo REQUIREMENTS:
echo     - Docker Desktop for Windows
echo.
echo OUTPUT:
echo     Built files will be in: build\docker-output\mdk_predator.ppma
echo.
exit /b 0

:check_docker
REM Check if Docker is installed
docker --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Docker is not installed!
    echo.
    echo Please install Docker Desktop for Windows:
    echo https://docs.docker.com/desktop/install/windows-install/
    echo.
    exit /b 1
)

REM Check if docker-compose is available
docker-compose version >nul 2>&1
if errorlevel 1 (
    docker compose version >nul 2>&1
    if errorlevel 1 (
        echo [ERROR] docker-compose is not installed!
        echo.
        echo docker-compose is usually included with Docker Desktop.
        echo.
        exit /b 1
    )
    set DOCKER_COMPOSE=docker compose
) else (
    set DOCKER_COMPOSE=docker-compose
)

REM Rebuild image if requested
if %REBUILD_IMAGE%==1 (
    echo [INFO] Rebuilding Docker image...
    %DOCKER_COMPOSE% build --no-cache
)

REM Create output directory
if not exist build\docker-output mkdir build\docker-output

REM Export environment variables
set CLEAN_BUILD=%CLEAN_BUILD%
set UPDATE_FIRMWARE=%UPDATE_FIRMWARE%

echo [INFO] MDK-Predator Docker Build
echo.
echo Configuration:
echo   - Clean build: !CLEAN_BUILD!
echo   - Update firmware: !UPDATE_FIRMWARE!
if %USE_NINJA%==1 (
    echo   - Build system: Ninja
) else (
    echo   - Build system: Make
)
echo   - Output directory: build\docker-output\
echo.

if %OPEN_SHELL%==1 (
    echo [INFO] Opening interactive shell...
    %DOCKER_COMPOSE% run --rm mdk-predator-shell
) else (
    if %USE_NINJA%==1 (
        echo [INFO] Building with Ninja...
        %DOCKER_COMPOSE% run --rm mdk-predator-build-ninja
    ) else if %CLEAN_BUILD%==1 (
        echo [INFO] Starting clean build...
        %DOCKER_COMPOSE% run --rm mdk-predator-build-clean
    ) else (
        echo [INFO] Starting build...
        %DOCKER_COMPOSE% run --rm mdk-predator-build
    )
    
    echo.
    echo [INFO] Build completed!
    echo.
    echo [INFO] Output files:
    dir build\docker-output\ 2>nul
    echo.
    echo [INFO] Next steps:
    echo   1. Copy build\docker-output\mdk_predator.ppma to SD card /APPS/ directory
    echo   2. Copy build\docker-output\mdk_predator.conf to SD card /MDK-PREDATOR/config/
    echo   3. Insert SD card into PortaPack
    echo   4. Launch from Apps menu
    echo.
)

exit /b 0
