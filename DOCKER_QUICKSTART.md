# Docker Quick Start Guide

**🐳 Building MDK-Predator with Docker - No Dependencies Required!**

This is the **recommended** build method for Windows users and anyone who wants to avoid dependency installation issues.

## TL;DR - Quick Commands

### Windows (PowerShell)
```powershell
.\docker-build.ps1
```

### Windows (CMD)
```cmd
docker-build.bat
```

### Linux/macOS
```bash
chmod +x docker-build.sh  # First time only
./docker-build.sh
```

**Output:** `build/docker-output/mdk_predator.ppma`

## What You Need

1. **Docker Desktop** (Windows/macOS) or **Docker Engine** (Linux)
   - Download: https://www.docker.com/get-started

That's it! No ARM toolchain, no CMake, no Python, no dependency headaches.

## Common Build Commands

| What You Want | Command |
|---------------|---------|
| Standard build | `.\docker-build.ps1` or `./docker-build.sh` |
| Fast build (Ninja) | `.\docker-build.ps1 -Ninja` or `./docker-build.sh --ninja` |
| Clean build | `.\docker-build.ps1 -Clean` or `./docker-build.sh --clean` |
| Update firmware | `.\docker-build.ps1 -UpdateFirmware` or `./docker-build.sh --update-firmware` |
| Debug/Shell | `.\docker-build.ps1 -Shell` or `./docker-build.sh --shell` |

## First Build

The first build will:
1. Download Docker base image (~200 MB)
2. Install ARM toolchain and build tools (~500 MB)
3. Clone mayhem-firmware (~500 MB)
4. Build mdk-predator (~5-10 minutes)

**Total time:** ~15-20 minutes
**Total space:** ~2 GB

Subsequent builds are much faster (~2-5 minutes) as everything is cached.

## Installation After Build

1. Copy `build/docker-output/mdk_predator.ppma` to SD card `/APPS/`
2. Copy `build/docker-output/mdk_predator.conf` to SD card `/MDK-PREDATOR/config/`
3. Insert SD card into PortaPack
4. Launch from Apps menu

## Troubleshooting

**Docker not found?**
- Install Docker Desktop: https://www.docker.com/get-started

**Build failed?**
- Try clean rebuild: `.\docker-build.ps1 -Clean -Rebuild`

**Slow on Windows?**
- Use WSL2 backend in Docker Desktop settings
- Try Ninja: `.\docker-build.ps1 -Ninja`

**Need help?**
- See full guide: [DOCKER_BUILD.md](DOCKER_BUILD.md)
- Open issue: https://github.com/limbo111111/mdk-predator/issues

## Why Docker?

✅ No dependency installation
✅ Works perfectly on Windows
✅ Reproducible builds
✅ Clean system
✅ Easy updates

## More Information

- **Full Docker Guide:** [DOCKER_BUILD.md](DOCKER_BUILD.md)
- **Native Build:** [BUILD_APPROACH.md](BUILD_APPROACH.md)
- **Deployment:** [docs/DEPLOYMENT.md](docs/DEPLOYMENT.md)
- **Main README:** [README.md](README.md)
