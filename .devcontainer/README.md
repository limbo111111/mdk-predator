# MDK-Predator DevContainer Setup

This directory contains the DevContainer configuration for developing MDK-Predator in GitHub Codespaces or VS Code with Remote Containers.

## What is a DevContainer?

A development container (devcontainer) is a running Docker container with a well-defined tool/runtime stack and its prerequisites. It provides a consistent development environment across different machines and platforms.

## Features

This devcontainer provides:

- **ARM GCC Toolchain**: Pre-installed ARM cross-compiler for embedded development
- **Build Tools**: CMake, Ninja, Make, and ccache for efficient builds
- **Python 3**: Required for build scripts
- **Git**: Version control with Git Graph extension
- **VS Code Extensions**: 
  - C/C++ IntelliSense and debugging
  - CMake Tools
  - Docker integration
  - GitLens for enhanced Git capabilities
- **Development Utilities**: vim, nano, tmux, htop, and more
- **Persistent Volumes**: Mayhem firmware and command history persist between sessions

## Quick Start

### Using GitHub Codespaces

1. Navigate to the repository on GitHub
2. Click the green **Code** button
3. Select **Codespaces** tab
4. Click **Create codespace on main** (or your branch)
5. Wait for the container to build (first time only, ~5-10 minutes)
6. Start developing!

### Using VS Code Locally

1. Install [Docker Desktop](https://www.docker.com/products/docker-desktop)
2. Install [VS Code](https://code.visualstudio.com/)
3. Install the [Remote - Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension
4. Open the repository in VS Code
5. Click the green button in the bottom-left corner
6. Select **Reopen in Container**
7. Wait for the container to build (first time only)

## Building MDK-Predator

Once inside the devcontainer:

### Quick Build with Docker Scripts

```bash
# Build using Docker (recommended)
./docker-build.sh
```

The output will be in `build/docker-output/mdk_predator.ppma`

### Native Build

```bash
# Download firmware and build
./build.sh -d

# Or build with existing firmware
./build.sh -m /workspace/mayhem-firmware
```

### Run Tests

```bash
# Run all tests
make test

# Run specific test suites
make test-automotive
make test-wireless
make test-crypto
```

## Environment Variables

The devcontainer supports these environment variables (set in `docker-compose.yml`):

- `CLEAN_BUILD=1`: Clean build directory before building
- `UPDATE_FIRMWARE=1`: Update mayhem-firmware from git before building

## Persistent Data

The following data persists between devcontainer sessions:

- **mayhem-firmware**: The Mayhem firmware repository (volume)
- **commandhistory**: Your bash/zsh command history
- **build artifacts**: Located in your workspace (not in volumes)

## Customization

### Adding VS Code Extensions

Edit `devcontainer.json` and add extension IDs to the `extensions` array:

```json
"extensions": [
    "ms-vscode.cpptools",
    "your-extension-id"
]
```

### Changing ARM Toolchain Version

Edit `Dockerfile` and update the `ARMBINURL` environment variable to point to a different ARM GCC version.

### Installing Additional Tools

Edit `Dockerfile` and add packages to the `apt-get install` command.

## Troubleshooting

### Container Build Fails

- Check your Docker installation is working: `docker --version`
- Ensure you have enough disk space (5-10 GB recommended)
- Try rebuilding without cache: **Ctrl+Shift+P** → **Remote-Containers: Rebuild Container Without Cache**

### ARM Toolchain Not Found

The toolchain is installed at `/opt/build/armbin/bin/`. Verify it's in your PATH:

```bash
echo $PATH | grep armbin
arm-none-eabi-gcc --version
```

### Mayhem Firmware Issues

If the firmware doesn't download or has issues:

```bash
# Manually clone to the persistent volume
cd /workspace
git clone --depth 1 https://github.com/portapack-mayhem/mayhem-firmware.git
cd mayhem-firmware
git submodule update --init --recursive
```

### Permission Issues

The devcontainer runs as user `vscode` (UID 1000). If you have permission issues:

```bash
# Fix workspace permissions
sudo chown -R vscode:vscode /workspace
```

## Additional Resources

- [VS Code DevContainers Documentation](https://code.visualstudio.com/docs/remote/containers)
- [GitHub Codespaces Documentation](https://docs.github.com/en/codespaces)
- [MDK-Predator Build Documentation](../BUILD.md)
- [Docker Build Guide](../DOCKER_BUILD.md)
