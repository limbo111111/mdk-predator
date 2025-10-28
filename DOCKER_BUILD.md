# Docker Build Guide for MDK-Predator

This guide explains how to build MDK-Predator using Docker, which eliminates all dependency issues on Windows, macOS, or Linux.

## Why Docker?

Building MDK-Predator requires:
- ARM GCC toolchain (arm-none-eabi-gcc)
- CMake 3.16+
- Python 3.7+
- Git, Make, and other build tools
- Mayhem firmware source code

Setting up these dependencies can be challenging, especially on Windows. Docker provides a complete, pre-configured build environment that works identically on all platforms.

## Benefits of Docker Build

✅ **No dependency installation** - Everything is in the Docker container  
✅ **Works on Windows** - No WSL or complex setup needed  
✅ **Reproducible builds** - Same environment every time  
✅ **No pollution** - Your system stays clean  
✅ **Easy updates** - Just rebuild the Docker image  

## Prerequisites

### All Platforms
- **Docker Desktop** (includes docker-compose)
  - Windows: [Download Docker Desktop](https://docs.docker.com/desktop/install/windows-install/)
  - macOS: [Download Docker Desktop](https://docs.docker.com/desktop/install/mac-install/)
  - Linux: [Install Docker Engine](https://docs.docker.com/engine/install/)

### Verify Installation
```bash
# Check Docker
docker --version

# Check docker-compose
docker-compose --version
# OR (newer versions)
docker compose version
```

## Quick Start

### Windows (PowerShell)
```powershell
# Standard build
.\docker-build.ps1

# Clean build
.\docker-build.ps1 -Clean

# Fast build with Ninja
.\docker-build.ps1 -Ninja
```

### Windows (CMD)
```cmd
REM Standard build
docker-build.bat

REM Clean build
docker-build.bat --clean

REM Fast build with Ninja
docker-build.bat --ninja
```

### Linux/macOS
```bash
# Make script executable (first time only)
chmod +x docker-build.sh

# Standard build
./docker-build.sh

# Clean build
./docker-build.sh --clean

# Fast build with Ninja
./docker-build.sh --ninja
```

## Build Options

### Using the Build Scripts

All build scripts support the same options:

| Option | Description |
|--------|-------------|
| `-c, --clean` / `-Clean` | Clean build (removes previous build artifacts) |
| `-u, --update-firmware` / `-UpdateFirmware` | Update mayhem firmware from GitHub |
| `-n, --ninja` / `-Ninja` | Use Ninja build system (faster than Make) |
| `-s, --shell` / `-Shell` | Open interactive shell in container |
| `-r, --rebuild` / `-Rebuild` | Rebuild Docker image from scratch |
| `-h, --help` / `-Help` | Show help message |

### Examples

**Standard build:**
```bash
./docker-build.sh
```

**Clean build with Ninja (fastest):**
```bash
./docker-build.sh --clean --ninja
```

**Update firmware and rebuild:**
```bash
./docker-build.sh --update-firmware --clean
```

**Interactive debugging:**
```bash
./docker-build.sh --shell
```

## Using Docker Compose Directly

If you prefer to use docker-compose directly:

### Standard Build
```bash
docker-compose run --rm mdk-predator-build
```

### Build with Ninja (Faster)
```bash
docker-compose run --rm mdk-predator-build-ninja
```

### Clean Build
```bash
docker-compose run --rm mdk-predator-build-clean
```

### Interactive Shell
```bash
docker-compose run --rm mdk-predator-shell
```

### Custom Build Command
```bash
# Build with specific make target
docker-compose run --rm mdk-predator-build make application -j8

# Build with specific cmake options
docker-compose run --rm mdk-predator-shell
# Then inside the container:
cd /workspace/mayhem-firmware
mkdir build && cd build
cmake -DCUSTOM_OPTION=ON ..
make application
```

## Output Files

After a successful build, you'll find the output in:
```
build/docker-output/
├── mdk_predator.ppma      # The application binary
├── mdk_predator.conf      # Configuration file
└── README.txt             # Installation instructions
```

## Environment Variables

You can customize the build by setting environment variables:

```bash
# Clean build before building
export CLEAN_BUILD=1

# Update mayhem firmware from git
export UPDATE_FIRMWARE=1

# Then run docker-compose
docker-compose run --rm mdk-predator-build
```

Or on Windows (PowerShell):
```powershell
$env:CLEAN_BUILD = "1"
$env:UPDATE_FIRMWARE = "1"
docker-compose run --rm mdk-predator-build
```

## Advanced Usage

### Persistent Mayhem Firmware

The mayhem firmware is stored in a Docker volume and persists between builds. This means:
- First build: Downloads mayhem firmware (~500MB, takes a few minutes)
- Subsequent builds: Reuses downloaded firmware (much faster)

To update the firmware:
```bash
./docker-build.sh --update-firmware
```

To completely remove and re-download:
```bash
docker volume rm mdk-predator_mayhem-firmware
./docker-build.sh
```

### Rebuilding the Docker Image

If you update the Dockerfile or want to get the latest base image:
```bash
./docker-build.sh --rebuild
```

Or manually:
```bash
docker-compose build --no-cache
```

### Customizing the Build

You can modify `docker-compose.yml` to:
- Change mount points
- Add environment variables
- Modify build commands
- Use different Docker images

### Interactive Development

For development and debugging:
```bash
# Open shell in container
./docker-build.sh --shell

# Inside container, you can:
cd /workspace/mdk-predator
ls -la

cd /workspace/mayhem-firmware
ls -la

# Manually run build steps
cd /workspace/mayhem-firmware
mkdir build && cd build
cmake ..
make application
```

## Troubleshooting

### Docker Not Starting
- **Windows**: Make sure Docker Desktop is running (check system tray)
- **Linux**: Start Docker service: `sudo systemctl start docker`
- **macOS**: Start Docker Desktop application

### Permission Denied on Linux
```bash
# Add your user to docker group
sudo usermod -aG docker $USER
# Log out and log back in
```

### Build Fails
```bash
# Try a clean rebuild
./docker-build.sh --clean --rebuild

# Or open shell and investigate
./docker-build.sh --shell
```

### Disk Space Issues
```bash
# Clean up Docker
docker system prune -a

# Remove mayhem firmware volume
docker volume rm mdk-predator_mayhem-firmware
```

### Slow Build on Windows
- Make sure Docker Desktop is configured to use WSL2 backend
- Allocate more CPU/RAM to Docker in Docker Desktop settings
- Use Ninja instead of Make: `.\docker-build.ps1 -Ninja`

## Comparison with Native Build

| Aspect | Docker Build | Native Build |
|--------|--------------|--------------|
| Setup Time | 5 minutes (first time) | 30-60 minutes |
| Dependencies | None (all in container) | Many (ARM toolchain, CMake, etc.) |
| Windows Support | Excellent | Challenging (requires WSL) |
| Build Speed | Good | Slightly faster |
| Disk Space | ~2 GB (container + volumes) | ~1 GB (dependencies) |
| Reproducibility | Perfect | Varies by system |

## Next Steps

After building:
1. Copy `build/docker-output/mdk_predator.ppma` to SD card `/APPS/` directory
2. Copy `build/docker-output/mdk_predator.conf` to SD card `/MDK-PREDATOR/config/`
3. Insert SD card into PortaPack
4. Launch MDK-Predator from Apps menu

See [DEPLOYMENT.md](docs/DEPLOYMENT.md) for detailed deployment instructions.

## See Also

- [README.md](README.md) - Main project documentation
- [BUILD_APPROACH.md](BUILD_APPROACH.md) - Native build documentation
- [DEPLOYMENT.md](docs/DEPLOYMENT.md) - Deployment guide
- [Docker Documentation](https://docs.docker.com/) - Official Docker docs
- [PortaPack Mayhem](https://github.com/portapack-mayhem/mayhem-firmware) - Firmware repository
