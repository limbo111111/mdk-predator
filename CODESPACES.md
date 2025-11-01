# GitHub Codespaces Quick Start Guide

Build MDK-Predator directly in your browser with **zero setup** using GitHub Codespaces!

## What is GitHub Codespaces?

GitHub Codespaces provides a complete development environment in your browser. No installation, no configuration, no dependencies - just click and start developing!

## Quick Start (3 Steps)

### 1. Create a Codespace

1. Go to the [MDK-Predator repository](https://github.com/limbo111111/mdk-predator)
2. Click the green **Code** button
3. Select the **Codespaces** tab
4. Click **Create codespace on main** (or your branch)

![Create Codespace](https://docs.github.com/assets/cb-138303/mw-1440/images/help/codespaces/new-codespace-button.webp)

### 2. Wait for Environment Setup

The first time you create a codespace:
- The container will build (5-10 minutes)
- All tools will be automatically installed:
  - ARM GCC toolchain
  - CMake, Ninja, Make
  - Python 3
  - Git and development utilities
- Future launches will be instant!

### 3. Build the Project

Once the codespace is ready, open the terminal and run:

```bash
# Build using Docker (recommended)
./docker-build.sh
```

Or use the native build:

```bash
# Download firmware and build
./build.sh -d
```

Your `mdk_predator.ppma` file will be in `build/docker-output/` or `build/output/`!

## Development in Codespaces

### Terminal Access

The integrated terminal gives you full bash access:
- Run build commands
- Execute tests with `make test`
- Use git commands
- Access all development tools

### VS Code Extensions

Pre-installed extensions for C/C++ development:
- **C/C++ IntelliSense** - Code completion and navigation
- **CMake Tools** - CMake integration
- **GitLens** - Enhanced Git capabilities
- **Docker** - Container management

### File Browser

Use the VS Code file explorer to:
- Browse source code
- Edit files
- Create new files
- Search across the project

### Running Tests

```bash
# Run all tests
make test

# Run specific test suites
make test-automotive
make test-wireless
make test-crypto
make test-integration
```

## Build Options

### Docker Build (Recommended)

```bash
# Standard build
./docker-build.sh

# Clean build
./docker-build.sh --clean

# Build with Ninja (faster)
./docker-build.sh --ninja

# Update firmware and build
./docker-build.sh --update-firmware
```

### Native Build

```bash
# Download firmware and build
./build.sh -d

# Build with existing firmware
./build.sh -m /workspace/mayhem-firmware

# Clean build
./build.sh -c -d
```

## Downloading Your Built App

### Option 1: Download from VS Code

1. Navigate to `build/docker-output/` or `build/output/`
2. Right-click on `mdk_predator.ppma`
3. Select **Download**

### Option 2: Use GitHub CLI (in Codespace terminal)

```bash
# Upload to a GitHub release (if you have permissions)
gh release upload v1.0.0 build/docker-output/mdk_predator.ppma
```

### Option 3: Copy to Your Repository

```bash
# Create a releases directory
mkdir -p releases
cp build/docker-output/mdk_predator.ppma releases/
git add releases/
git commit -m "Add built app"
git push
```

Then download from your repository via GitHub web interface.

## Persistent Data

Your codespace automatically persists:
- ✅ All source code changes
- ✅ Git commits and branches
- ✅ Mayhem firmware (in Docker volumes)
- ✅ Command history
- ✅ VS Code settings

Build artifacts in `build/` are also persisted but can be cleaned safely.

## Working with Branches

```bash
# Create a new branch
git checkout -b my-feature

# Make changes and commit
git add .
git commit -m "Add my feature"
git push -u origin my-feature

# Create a pull request from GitHub web interface
```

## Codespace Management

### Stop/Start Codespaces

- **Stop**: Click your profile icon → Codespaces → Stop
- **Resume**: Go to the repository → Code → Codespaces → Select your codespace
- **Delete**: Click your profile icon → Codespaces → Delete (to free up space)

### Codespace Limits

GitHub provides generous free tier:
- **Free tier**: 120 core-hours/month, 15 GB storage
- **Pro**: 180 core-hours/month, 20 GB storage
- Stopped codespaces don't count against your hours!

### Best Practices

1. **Stop when not using** - Saves your monthly hours
2. **Delete old codespaces** - Frees up storage
3. **One codespace per feature** - Keep work organized
4. **Regular commits** - Your work is automatically synced

## Troubleshooting

### Codespace Won't Start

- Check your GitHub Codespaces quota
- Try deleting old codespaces to free space
- Contact GitHub support if issue persists

### Build Fails

```bash
# Check Docker is running
docker --version

# Clean and rebuild
./docker-build.sh --clean
```

### Mayhem Firmware Issues

```bash
# Manually clone firmware
cd /workspace
git clone --depth 1 https://github.com/portapack-mayhem/mayhem-firmware.git
cd mayhem-firmware
git submodule update --init --recursive
```

### Out of Disk Space

```bash
# Clean build artifacts
make clean
rm -rf build/

# Remove Docker volumes (if needed)
docker system prune -a
```

## Advanced Features

### Port Forwarding

If you run a local server in your codespace, ports are automatically forwarded:

```bash
# Example: Run a local web server
python3 -m http.server 8000
```

Click the "Ports" tab to access forwarded ports.

### Debugging

Use VS Code's built-in debugger:
1. Set breakpoints in your code
2. Press F5 or Run → Start Debugging
3. Use the debug console for inspection

### Custom Devcontainer

Edit `.devcontainer/devcontainer.json` to customize:
- Add more VS Code extensions
- Change shell (bash/zsh)
- Add custom environment variables
- Modify container settings

## Comparison: Codespaces vs Local Development

| Feature | Codespaces | Local Development |
|---------|-----------|-------------------|
| Setup Time | < 1 minute | 1-2 hours |
| Dependencies | Pre-installed | Manual install |
| Access | Any browser | Specific machine |
| Performance | Good | Varies by hardware |
| Cost | Free tier available | Free (but local resources) |
| Collaboration | Easy sharing | Requires setup |

## Getting Help

- **Codespaces Docs**: https://docs.github.com/en/codespaces
- **DevContainer Docs**: https://code.visualstudio.com/docs/remote/containers
- **MDK-Predator Issues**: https://github.com/limbo111111/mdk-predator/issues
- **Build Documentation**: See [BUILD.md](BUILD.md) and [DOCKER_BUILD.md](DOCKER_BUILD.md)

## Next Steps

After building your app:

1. Download the `mdk_predator.ppma` file
2. Follow [HOW_TO_DEPLOY.md](HOW_TO_DEPLOY.md) to install on your PortaPack
3. Read [QUICKSTART.md](QUICKSTART.md) for usage guide
4. Check [docs/USER_GUIDE.md](docs/USER_GUIDE.md) for detailed instructions

Happy coding! 🚀
