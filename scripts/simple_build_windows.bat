@echo off
REM Simple MDK-Predator Build Script for Windows
REM Wrapper for PowerShell simple build script

setlocal

REM Get the script directory
set SCRIPT_DIR=%~dp0

REM Check if PowerShell is available
where powershell >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: PowerShell not found. Please install PowerShell.
    exit /b 1
)

REM Parse arguments
set PS_ARGS=

:parse_args
if "%~1"=="" goto run_script
if /i "%~1"=="-m" (
    set PS_ARGS=%PS_ARGS% -MayhemPath "%~2"
    shift
    shift
    goto parse_args
)
if /i "%~1"=="--mayhem" (
    set PS_ARGS=%PS_ARGS% -MayhemPath "%~2"
    shift
    shift
    goto parse_args
)
if /i "%~1"=="-h" set PS_ARGS=%PS_ARGS% -Help
if /i "%~1"=="--help" set PS_ARGS=%PS_ARGS% -Help
shift
goto parse_args

:run_script
echo Simple MDK-Predator Build Script
echo =================================
echo.

REM Run PowerShell script
powershell.exe -ExecutionPolicy Bypass -File "%SCRIPT_DIR%simple_build_windows.ps1" %PS_ARGS%

exit /b %errorlevel%
