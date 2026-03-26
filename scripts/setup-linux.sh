#!/bin/bash

# OnAirDeck Linux Development Environment Setup
# Installs all required build dependencies for Ubuntu 24.04 LTS
# Usage: bash scripts/setup-linux.sh

set -e

echo "=== OnAirDeck Linux Setup ==="
echo "Installing build dependencies for Ubuntu 24.04 LTS..."
echo

# List of required packages
PACKAGES=(
    # Audio support
    "libasound2-dev"
    # X11 and windowing
    "libx11-dev"
    "libxrandr-dev"
    "libxinerama-dev"
    "libxcursor-dev"
    # Graphics
    "libgl-dev"
    # GTK3 for JUCE GUI
    "libgtk-3-dev"
    # WebKit for embedded browser
    "libwebkit2gtk-4.1-dev"
    # HTTP support
    "libcurl4-openssl-dev"
)

echo "Packages to install:"
for pkg in "${PACKAGES[@]}"; do
    echo "  - $pkg"
done
echo

# Update package cache
echo "Updating package cache..."
sudo apt-get update

# Install packages
echo "Installing packages..."
sudo apt-get install -y "${PACKAGES[@]}"

echo
echo "=== Setup Complete ==="
echo "Dependencies installed successfully."
echo
echo "Next steps:"
echo "  1. Clone with submodules (if not already done):"
echo "     git submodule update --init --recursive"
echo
echo "  2. Configure and build:"
echo "     cmake --preset debug"
echo "     cmake --build --preset debug"
echo
echo "For more information, see docs/build-and-run.md"
