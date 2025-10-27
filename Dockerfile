# MDK-Predator Docker Build Environment
# Based on PortaPack Mayhem Firmware Docker setup
#
# This Dockerfile creates a complete build environment for compiling
# mdk-predator with/against mayhem-firmware without dependency issues.

FROM ubuntu:noble

# Metadata
LABEL maintainer="MDK-Predator Team"
LABEL description="Build environment for MDK-Predator PortaPack Application"
LABEL version="1.0"

# Set location to download ARM toolkit from
ENV ARMBINURL=https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2?revision=108bd959-44bd-4619-9c19-26187abf5225&la=en&hash=E788CE92E5DFD64B2A8C246BBA91A249CB8E2D2D \
    PATH=$PATH:/opt/build/armbin/bin \
    LANG=C.UTF-8 \
    LC_ALL=C.UTF-8

# Create working directories
WORKDIR /workspace

# Create volumes for source code and output
VOLUME ["/workspace/mdk-predator", "/workspace/mayhem-firmware", "/workspace/output"]

# Fetch dependencies from APT
RUN apt-get update \
 && apt-get install -y \
    git \
    tar \
    wget \
    dfu-util \
    cmake \
    python3 \
    python3-pip \
    python3-yaml \
    ccache \
    bzip2 \
    liblz4-tool \
    curl \
    ninja-build \
    make \
    ca-certificates \
 && apt-get -qy autoremove \
 && rm -rf /var/lib/apt/lists/*

# Grab the GNU ARM toolchain from arm.com
# Then extract contents to /opt/build/armbin/
RUN mkdir -p /opt/build \
 && cd /opt/build \
 && wget -O gcc-arm-none-eabi "$ARMBINURL" \
 && mkdir armbin \
 && tar --strip=1 -xjvf gcc-arm-none-eabi -C armbin \
 && rm gcc-arm-none-eabi

# Copy entrypoint script
COPY docker-entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]

# Default command: build with make
CMD ["make"]
