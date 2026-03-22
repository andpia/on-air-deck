# Build and Run

## Prerequisites

- CMake 3.22 or newer
- A C++17-capable compiler
- Git submodules initialized so the `vendor/JUCE/` and `vendor/web-ui/` directories are populated
- Access to the companion frontend repository `andpia/on-air-deck-figma` (included as `vendor/web-ui/` submodule by default)
  - Debug workflow: run the Vite development server
  - Release workflow: build `dist/` and pass it through `WEBUI_DIST_PATH`

Clone the repository with submodules:

```bash
git clone --recursive https://github.com/andpia/on-air-deck.git
cd on-air-deck
```

If you already cloned without submodules:

```bash
git submodule update --init --recursive
```

For frontend setup details, see [Web UI](web-ui.md) and [Dependencies](dependencies.md).

## Debug Build

```bash
cmake -S . -B out/build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build out/build/debug --config Debug
```

In Debug, the app loads the Web UI from the Vite development server configured by `WEBUI_DEV_SERVER_URL`.

## Release Build

Build the frontend bundle first in the `vendor/web-ui/` submodule. CMake auto-detects `vendor/web-ui/dist` when available:

```bash
cd vendor/web-ui
npm install
npm run build
cd ..
cmake -S . -B out/build/release -DCMAKE_BUILD_TYPE=Release
cmake --build out/build/release --config Release
```

If your frontend lives outside this repository, point CMake at the generated `dist/` directory explicitly:

```bash
cmake -S . -B out/build/release -DCMAKE_BUILD_TYPE=Release \
  -DWEBUI_DIST_PATH=/path/to/on-air-deck-figma/dist
cmake --build out/build/release --config Release
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
| `WEBUI_DIST_PATH` | auto (`vendor/web-ui/dist` if present) | Path to the built frontend assets for Release packaging. |
| `ONAIRDECK_USE_CURL` | `ON` | Enables CURL support when available. |
| `ONAIRDECK_ENABLE_SANITIZERS` | `ON` | Enables AddressSanitizer in Debug on supported toolchains. |
| `ONAIRDECK_UNITY_BUILD` | `OFF` | Enables CMake unity builds for faster compilation. |