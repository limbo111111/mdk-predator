# Building MDK-Predator on Windows

This guide explains how to build MDK-Predator on Windows using the provided build scripts.

## Prerequisites

### Required Tools

1. **ARM GCC Toolchain**
   - Download from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
   - Install and add to PATH
   - Verify: `arm-none-eabi-gcc --version`

2. **GNU Make**
   - Option 1: Install MinGW (https://sourceforge.net/projects/mingw/)
   - Option 2: Install with Chocolatey: `choco install make`
   - Option 3: Use Git Bash (includes make)
   - Verify: `make --version`

3. **CMake** (for PortaPack builds)
   - Download from: https://cmake.org/download/
   - Or install with Chocolatey: `choco install cmake`
   - Add to PATH during installation
   - Verify: `cmake --version`

4. **Python 3.7+** (for PortaPack builds)
   - Download from: https://www.python.org/downloads/
   - Or install with Chocolatey: `choco install python`
   - Add to PATH during installation
   - Verify: `python --version`

5. **PowerShell 5.0+** (usually pre-installed on Windows 10/11)
   - Verify: `$PSVersionTable.PSVersion`

### Optional: Chocolatey Package Manager

To install all tools at once using Chocolatey:

```powershell
# Install Chocolatey (if not already installed)
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# Install build tools
choco install make cmake python3 -y
```

You'll still need to install the ARM toolchain manually from the ARM website.

## Building the Library

The library build creates `libmdk_predator.a` for use in embedded projects.

### Using PowerShell

```powershell
# Navigate to scripts directory
cd scripts

# Basic build
.\build.ps1

# Clean build
.\build.ps1 -Clean

# Build and test
.\build.ps1 -Test

# Build specific module
.\build.ps1 -Target automotive

# Show help
.\build.ps1 -Help
```

### Using Command Prompt (CMD)

```cmd
# Navigate to scripts directory
cd scripts

# Basic build
build.bat

# Clean build
build.bat -clean

# Build and test
build.bat -test

# Build specific module
build.bat -target automotive
```

### Build Targets

- `all` - Build complete library (default)
- `automotive` - Build automotive module only
- `wireless` - Build wireless module only
- `crypto` - Build crypto module only

### Build Output

After successful build:
- Library: `build/lib/libmdk_predator.a`
- Object files: `build/obj/`

## Building the PortaPack Application

The PortaPack build creates a `.ppma` file for deployment to PortaPack H4M.

### Prerequisites

In addition to the tools above, you need:
- **PortaPack Mayhem firmware source code**
- Clone from: https://github.com/eried/portapack-mayhem

### Using PowerShell

```powershell
# Navigate to scripts directory
cd scripts

# Build with Mayhem firmware path
.\build_portapack_app.ps1 -MayhemPath "C:\path\to\portapack-mayhem"

# Clean build
.\build_portapack_app.ps1 -MayhemPath "C:\path\to\portapack-mayhem" -Clean

# Custom output directory
.\build_portapack_app.ps1 -MayhemPath "C:\path\to\portapack-mayhem" -OutputDir "C:\output"

# Show help
.\build_portapack_app.ps1 -Help
```

### Using Command Prompt (CMD)

```cmd
# Navigate to scripts directory
cd scripts

# Build with Mayhem firmware path
build_portapack_app.bat -m "C:\path\to\portapack-mayhem"

# Clean build
build_portapack_app.bat -m "C:\path\to\portapack-mayhem" -clean

# Custom output directory
build_portapack_app.bat -m "C:\path\to\portapack-mayhem" -o "C:\output"
```

### Build Output

After successful build:
- Application: `build/portapack/mdk_predator.ppma`
- Configuration: `build/portapack/mdk_predator.conf`
- Instructions: `build/portapack/README.txt`

## Running Tests

### Using PowerShell

```powershell
cd scripts
.\build.ps1 -Test
```

### Using Make Directly

```powershell
# Build and run all tests
make test

# Run specific test suites
make test-automotive
make test-wireless
make test-crypto
make test-integration
```

## Troubleshooting

### "command not found" or "is not recognized"

**Problem**: Tool not found in PATH

**Solution**:
1. Install the missing tool (see Prerequisites)
2. Add tool to system PATH:
   - Open System Properties → Environment Variables
   - Edit PATH variable
   - Add tool installation directory
   - Restart terminal/PowerShell

### "execution of scripts is disabled"

**Problem**: PowerShell execution policy blocks scripts

**Solution**:
```powershell
# For current session only (safe)
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass

# Or use the batch file wrapper which handles this automatically
build.bat
```

### "arm-none-eabi-gcc: not found"

**Problem**: ARM toolchain not installed or not in PATH

**Solution**:
1. Download ARM GCC from official ARM website
2. Install to a path without spaces (e.g., `C:\ARM`)
3. Add `C:\ARM\bin` to system PATH
4. Verify with: `arm-none-eabi-gcc --version`

### Build fails with "make: command not found"

**Problem**: GNU Make not installed

**Solution**:
- Install MinGW/Cygwin, or
- Install with Chocolatey: `choco install make`, or
- Use Git Bash which includes make

### CMake configuration fails

**Problem**: CMake not found or wrong version

**Solution**:
1. Install CMake 3.16 or later
2. Ensure CMake is in PATH
3. Verify with: `cmake --version`

### Python import errors during build

**Problem**: Python not found or missing modules

**Solution**:
1. Install Python 3.7 or later
2. Ensure Python is in PATH
3. Verify with: `python --version`

### Mayhem firmware path not found

**Problem**: Invalid or incorrect path to Mayhem firmware

**Solution**:
1. Clone Mayhem firmware if not already done:
   ```
   git clone https://github.com/eried/portapack-mayhem.git
   ```
2. Use full absolute path with `-MayhemPath` parameter
3. Ensure path contains `CMakeLists.txt` file

### Build succeeds but .ppma file not created

**Problem**: External apps build target may have failed

**Solution**:
1. Check build logs for errors
2. Ensure Mayhem firmware is compatible version
3. Try clean build: `.\build_portapack_app.ps1 -Clean -MayhemPath "..."`

## Environment Setup Script

For convenience, create a setup script to configure your environment:

```powershell
# setup-env.ps1
# Add this to your PowerShell profile or run before building

$env:Path += ";C:\ARM\bin"              # ARM toolchain
$env:Path += ";C:\MinGW\bin"            # Make
$env:Path += ";C:\Program Files\CMake\bin"  # CMake
$env:Path += ";C:\Python310"            # Python

Write-Host "Build environment configured!" -ForegroundColor Green
```

## Building from Visual Studio Code

You can use VS Code with the integrated terminal:

1. Open project in VS Code
2. Open integrated terminal (Ctrl+`)
3. Run build commands:
   ```powershell
   cd scripts
   .\build.ps1
   ```

### VS Code Tasks

Create `.vscode/tasks.json` for easy building:

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Build MDK-Predator",
      "type": "shell",
      "command": "powershell",
      "args": ["-ExecutionPolicy", "Bypass", "-File", "./scripts/build.ps1"],
      "group": {
        "kind": "build",
        "isDefault": true
      },
      "problemMatcher": []
    },
    {
      "label": "Clean Build",
      "type": "shell",
      "command": "powershell",
      "args": ["-ExecutionPolicy", "Bypass", "-File", "./scripts/build.ps1", "-Clean"],
      "problemMatcher": []
    },
    {
      "label": "Run Tests",
      "type": "shell",
      "command": "powershell",
      "args": ["-ExecutionPolicy", "Bypass", "-File", "./scripts/build.ps1", "-Test"],
      "problemMatcher": []
    }
  ]
}
```

## Next Steps

After building:

1. **For library builds**: Link `libmdk_predator.a` in your embedded project
2. **For PortaPack builds**: Follow deployment instructions to install on device
   - See: [DEPLOYMENT.md](../docs/DEPLOYMENT.md)
   - See: [QUICKSTART.md](../QUICKSTART.md)

## Additional Resources

- **Project README**: [README.md](../README.md)
- **Getting Started**: [GETTING_STARTED.md](../GETTING_STARTED.md)
- **Deployment Guide**: [docs/DEPLOYMENT.md](../docs/DEPLOYMENT.md)
- **User Guide**: [docs/USER_GUIDE.md](../docs/USER_GUIDE.md)

## Support

For issues or questions:
- Open an issue: https://github.com/limbo111111/mdk-predator/issues
- Check existing documentation in `/docs` directory

## Platform-Specific Notes

### Windows 10/11
- PowerShell 5.1+ included by default
- Windows Defender may flag ARM toolchain binaries (add exceptions if needed)

### Windows Subsystem for Linux (WSL)
- You can also use the Linux build scripts in WSL
- Run: `./scripts/build_portapack_app.sh`
- See: Linux build instructions in main README

### Git Bash
- Git Bash includes make and can run PowerShell scripts
- Recommended for users familiar with Unix-like environments

## License

This project is licensed under the MIT License - see [LICENSE](../LICENSE) file for details.
