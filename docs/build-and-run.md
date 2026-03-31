# Build and Run

## Prerequisites

- CMake 3.22 or newer
- Ninja (required by `CMakePresets.json`, which uses the `Ninja Multi-Config` generator)
- A C++17-capable compiler
- **JUCE 8.0.12 or later** — Git submodules initialized so the `vendor/JUCE/` and `vendor/web-ui/` directories are populated
- Access to the companion frontend repository `andpia/on-air-deck-figma` (included as `vendor/web-ui/` submodule by default)
  - Debug workflow: run the Vite development server
  - Release workflow: build `dist/` and pass it through `WEBUI_DIST_PATH`
- **Windows (required)**: [Microsoft WebView2 SDK](https://developer.microsoft.com/en-us/microsoft-edge/webview2/) NuGet package.  This enables the Chromium-based backend and the bundled resource provider.  Without it CMake falls back to the legacy IE backend, which does **not** support modern JavaScript (ES modules) — the app will display a blank white screen at runtime.

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
cmake --preset debug
cmake --build --preset debug
```

In Debug, the app loads the Web UI from the Vite development server configured by `WEBUI_DEV_SERVER_URL`.

## Release Build

Build the frontend bundle first in the `vendor/web-ui/` submodule. CMake auto-detects `vendor/web-ui/dist` when available:

```bash
cd vendor/web-ui
npm install
npm run build
cd ..
cmake --preset release
cmake --build --preset release
```

When the frontend sources live in `vendor/web-ui/`, CMake also auto-builds `dist/` during the app build if `npm` is available. You can disable that behavior with `-DONAIRDECK_BUILD_BUNDLED_WEBUI=OFF`.

Release packaging copies the frontend files next to the app or inside the `.app` bundle; it does not embed them directly into the executable binary. At runtime, JUCE serves those files from the internal `juce://` resource origin.

Release configure is fail-fast for single-config generators. With multi-config generators (for example Ninja Multi-Config), configure now auto-builds the bundled frontend when possible; otherwise ensure the dist folder exists before building `--config Release`.

If your frontend lives outside this repository, point CMake at the generated `dist/` directory explicitly:

```bash
cmake --preset release -DWEBUI_DIST_PATH=/path/to/on-air-deck-figma/dist
cmake --build --preset release
```

## Standard Build Presets

This repository uses `CMakePresets.json` as the default build convention:

- `debug` writes to `out/build/debug`
- `release` writes to `out/build/release`

You can use CMake directly:

```bash
cmake --preset debug
cmake --build --preset debug
cmake --preset release
cmake --build --preset release
```

## Windows Prerequisites

Before configuring CMake on Windows, install the WebView2 SDK NuGet package so that JUCE uses the modern Chromium-based backend.  Open PowerShell and run:

```powershell
Register-PackageSource -Name nuget -ProviderName NuGet `
  -Location https://www.nuget.org/api/v2 -Trusted -Force
Install-Package Microsoft.Web.WebView2 -Scope CurrentUser -Force
```

CMake's `FindWebView2.cmake` (bundled with JUCE) searches `$USERPROFILE\AppData\Local\PackageManagement\NuGet\Packages` automatically.  Once installed, re-run CMake configure and `JUCE_USE_WIN_WEBVIEW2=1` will be printed to confirm detection.

Install Ninja as well, since the standard CMake presets use the `Ninja Multi-Config` generator.

Option 1 (winget):

```powershell
winget install --id Ninja-build.Ninja --exact --source winget
```

Option 2 (Chocolatey):

```powershell
choco install ninja -y
```

### WebView2 Runtime (end-user requirement)

The built application also requires the **Microsoft Edge WebView2 Runtime** to be installed on the end-user's machine.  On Windows 10 (21H1 and later) and Windows 11 the Runtime ships as a system component and should be present by default.  On older Windows versions or Windows Server it may need to be installed separately.

If the Runtime is missing, the application will display a dialog with a download link:

```
https://go.microsoft.com/fwlink/p/?LinkId=2124703
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
| `ONAIRDECK_USE_CURL` | `ON` | Enables CURL support when available. |
| `ONAIRDECK_ENABLE_SANITIZERS` | `ON` | Enables AddressSanitizer in Debug on supported toolchains. |
| `ONAIRDECK_UNITY_BUILD` | `OFF` | Enables CMake unity builds for faster compilation. |