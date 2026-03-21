# Build and Run

## Prerequisites

- CMake 3.22 or newer
- A C++17-capable compiler
- Git submodules initialized so the `JUCE/` directory is populated

Clone the repository with submodules:

```bash
git clone --recursive https://github.com/andpia/on-air-deck.git
cd on-air-deck
```

If you already cloned without submodules:

```bash
git submodule update --init --recursive
```

## Debug Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

In Debug, the app loads the Web UI from the Vite development server configured by `WEBUI_DEV_SERVER_URL`.

## Release Build

Build the frontend bundle first in the UI repository, then point CMake at the generated `dist/` directory:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DWEBUI_DIST_PATH=/path/to/on-air-deck-figma/dist
cmake --build build --config Release
```

## Linux Packages

On Linux, install the required system dependencies before configuring:

```bash
sudo apt-get update && sudo apt-get install libasound2-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libgl1-mesa-dev libwebkitgtk-6.0-dev libgtk-3-dev libcurl4-openssl-dev
```

## Useful CMake Variables

| Variable | Default | Description |
| --- | --- | --- |
| `WEBUI_DEV_SERVER_URL` | `http://localhost:5173` | URL used by Debug builds for the embedded frontend. |
| `WEBUI_DIST_PATH` | empty | Path to the built frontend assets for Release packaging. |
| `ONAIRDECK_USE_CURL` | `ON` | Enables CURL support when available. |
| `ONAIRDECK_ENABLE_SANITIZERS` | `ON` | Enables AddressSanitizer in Debug on supported toolchains. |
| `ONAIRDECK_UNITY_BUILD` | `OFF` | Enables CMake unity builds for faster compilation. |