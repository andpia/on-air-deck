# Build and Run

## Prerequisites

- CMake 3.22 or newer
- A C++17-capable compiler
- **JUCE 8.0.12 or later** — Git submodules initialized so the `vendor/JUCE/` and `vendor/web-ui/` directories are populated
- Access to the companion frontend repository `andpia/on-air-deck-figma` (included as `vendor/web-ui/` submodule by default)
  - Debug workflow: run the Vite development server
  - Release workflow: build `dist/` and pass it through `WEBUI_DIST_PATH`
- **Windows only (optional)**: [Microsoft WebView2 SDK](https://developer.microsoft.com/en-us/microsoft-edge/webview2/) NuGet package to enable the Chromium-based backend and the bundled resource provider.  Without it the IE backend is used and assets are loaded via `file://`.

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

Release packaging copies the frontend files next to the app or inside the `.app` bundle; it does not embed them directly into the executable binary. At runtime, JUCE serves those files from the internal `juce://` resource origin.

If your frontend lives outside this repository, point CMake at the generated `dist/` directory explicitly:

```bash
cmake -S . -B out/build/release -DCMAKE_BUILD_TYPE=Release \
  -DWEBUI_DIST_PATH=/path/to/on-air-deck-figma/dist
cmake --build out/build/release --config Release
```

## Linux Packages

On Linux, install the required system dependencies before configuring:

```bash
sudo apt-get update && sudo apt-get install libasound2-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libgl-dev libwebkit2gtk-4.1-dev libgtk-3-dev libcurl4-openssl-dev
```

## Cleanup

For routine cleanup of generated files:

```bash
bash scripts/clean.sh safe
```

Other modes:

- `bash scripts/clean.sh full`: removes `out/`, `vendor/web-ui/dist`, and `vendor/web-ui/node_modules`
- `bash scripts/clean.sh deep`: removes `full` targets plus legacy `build/` and known JUCE submodule build folders
- `bash scripts/clean.sh pristine --yes`: removes all untracked and ignored files in the main repo and submodules

## Useful CMake Variables

| Variable | Default | Description |
| --- | --- | --- |
| `WEBUI_DEV_SERVER_URL` | `http://localhost:5173` | URL used by Debug builds for the embedded frontend. |
| `WEBUI_DIST_PATH` | auto (`vendor/web-ui/dist` if present) | Path to the built frontend assets for Release packaging. |
| `ONAIRDECK_ALLOW_MISSING_WEBUI` | `OFF` | When `ON`, allows Release builds to succeed even without a Web UI dist/. **Not recommended for production packages.** |
| `ONAIRDECK_USE_CURL` | `ON` | Enables CURL support when available. |
| `ONAIRDECK_ENABLE_SANITIZERS` | `ON` | Enables AddressSanitizer in Debug on supported toolchains. |
| `ONAIRDECK_UNITY_BUILD` | `OFF` | Enables CMake unity builds for faster compilation. |

## Windows Build

On Windows, use the PowerShell helper script for a one-command release build:

```powershell
.\scripts\build-windows.ps1
```

This script:
1. Builds the frontend in `vendor/web-ui` (Node.js >= 20 required).
2. Configures CMake with `-DWEBUI_DIST_PATH` pointing at the built dist/.
3. Builds the Release target.
4. Packages `OnAirDeck.exe` and `WebUI/` into `out/OnAirDeck-windows-release.zip`.

Optional parameters:

```powershell
# Use a pre-built dist/ from outside the repo:
.\scripts\build-windows.ps1 -WebUiDistPath C:\repos\on-air-deck-figma\dist

# Skip the frontend build step (dist/ already present):
.\scripts\build-windows.ps1 -SkipFrontendBuild
```

### Windows CI workflow

The `.github/workflows/windows-release.yml` workflow runs automatically on pushes to `main`
and on pull requests.  It performs the same steps as `build-windows.ps1`, verifies that
`WebUI/index.html` exists in the output, and uploads `OnAirDeck-windows-release.zip` as a
build artifact.