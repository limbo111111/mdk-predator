@echo off
REM MDK-Predator Windows Build Script Wrapper
REM This batch file launches the PowerShell build script

setlocal

REM Get the script directory
set SCRIPT_DIR=%~dp0

REM Check if PowerShell is available
where powershell >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: PowerShell not found. Please install PowerShell.
    exit /b 1
)

REM Parse arguments and pass to PowerShell
set PS_ARGS=

:parse_args
if "%~1"=="" goto run_script
if /i "%~1"=="-clean" set PS_ARGS=%PS_ARGS% -Clean
if /i "%~1"=="--clean" set PS_ARGS=%PS_ARGS% -Clean
if /i "%~1"=="-test" set PS_ARGS=%PS_ARGS% -Test
if /i "%~1"=="--test" set PS_ARGS=%PS_ARGS% -Test
if /i "%~1"=="-help" set PS_ARGS=%PS_ARGS% -Help
if /i "%~1"=="--help" set PS_ARGS=%PS_ARGS% -Help
if /i "%~1"=="-h" set PS_ARGS=%PS_ARGS% -Help
if /i "%~1"=="-target" (
    set PS_ARGS=%PS_ARGS% -Target "%~2"
    shift
)
shift
goto parse_args

:run_script
echo MDK-Predator Build Script
echo =========================
echo.
echo Launching PowerShell build script...
echo.

REM Run PowerShell script with execution policy bypass
powershell.exe -ExecutionPolicy Bypass -File "%SCRIPT_DIR%build.ps1" %PS_ARGS%

exit /b %errorlevel%
