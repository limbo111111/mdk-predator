# MDK-Predator Development Container
# Optimized for GitHub Codespaces and local development
#
# This Dockerfile creates a complete development environment for MDK-Predator
# with all necessary tools, ARM toolchain, and development utilities.

FROM ubuntu:noble

# Metadata
LABEL maintainer="MDK-Predator Team"
LABEL description="Development environment for MDK-Predator PortaPack Application"
LABEL version="1.0"

# Avoid warnings by switching to noninteractive
ENV DEBIAN_FRONTEND=noninteractive

# Set location to download ARM toolkit from
ENV ARMBINURL=https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2?revision=108bd959-44bd-4619-9c19-26187abf5225&la=en&hash=E788CE92E5DFD64B2A8C246BBA91A249CB8E2D2D \
    PATH=$PATH:/opt/build/armbin/bin \
    LANG=C.UTF-8 \
    LC_ALL=C.UTF-8

# Create a non-root user (vscode)
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Configure apt and install packages
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
    # Build essentials
    git \
    tar \
    wget \
    curl \
    ca-certificates \
    make \
    cmake \
    ninja-build \
    ccache \
    bzip2 \
    liblz4-tool \
    # Python for build scripts
    python3 \
    python3-pip \
    python3-yaml \
    # Development tools
    vim \
    nano \
    less \
    htop \
    tmux \
    # Firmware tools
    dfu-util \
    # Utilities
    zip \
    unzip \
    sudo \
    # Shell tools
    zsh \
    bash-completion \
 && apt-get autoremove -y \
 && apt-get clean -y \
 && rm -rf /var/lib/apt/lists/*

# Create the user
RUN groupadd --gid $USER_GID $USERNAME \
 && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
 && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
 && chmod 0440 /etc/sudoers.d/$USERNAME

# Grab the GNU ARM toolchain from arm.com
# Then extract contents to /opt/build/armbin/
RUN mkdir -p /opt/build \
 && cd /opt/build \
 && wget -O gcc-arm-none-eabi "$ARMBINURL" \
 && mkdir armbin \
 && tar --strip=1 -xjvf gcc-arm-none-eabi -C armbin \
 && rm gcc-arm-none-eabi

# Make ARM toolchain accessible to all users
RUN chmod -R 755 /opt/build/armbin

# Install Oh My Zsh for better shell experience (optional, for vscode user)
RUN su - $USERNAME -c 'sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended' || true

# Create workspace directory with proper permissions
RUN mkdir -p /workspace \
 && chown -R $USERNAME:$USERNAME /workspace

# Set working directory
WORKDIR /workspace

# Switch to non-root user
USER $USERNAME

# Set the default shell to bash
ENV SHELL=/bin/bash

# Switch back to dialog for any ad-hoc use of apt-get
ENV DEBIAN_FRONTEND=dialog

# Default command
CMD ["/bin/bash"]
