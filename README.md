# 🎙️ OnAirDeck

### Professional Web Radio Engine: Live Streaming, Playlist Management & Broadcast Tools

**OnAirDeck** is a lightweight, high-performance **radio automation** console and **web streaming** suite built with the **JUCE 8** framework. It bridges the gap between digital music playlists and live vocal performance, providing a professional "deck" experience for modern **broadcasters**.

---

## ✨ Key Features

* **Advanced Broadcast Engine**: Native support for **ASIO** (Windows) and **CoreAudio** (macOS) ensuring ultra-low latency for professional **web radio** standards.
* **Smart Streaming**: Integrated encoder for direct-to-server **streaming** via **Icecast** and **Shoutcast** protocols.
* **Pro Voice-Over (Ducking)**: Intelligent automatic volume management to balance music and speech during **live broadcast** sessions.
* **Dynamic Playlist Deck**: Seamless track transitions and high-fidelity playback for continuous **radio** rotation.
* **Embedded Web UI**: The full On Air Deck interface (built with Vite + React from [`andpia/on-air-deck-figma`](https://github.com/andpia/on-air-deck-figma)) runs inside a JUCE `WebBrowserComponent`, with a bidirectional JS ↔ JUCE bridge.

## 🔗 Related Repositories

- Core app: [`andpia/on-air-deck`](https://github.com/andpia/on-air-deck)
- Frontend UI companion (required for Web UI development and Release bundle generation): [`andpia/on-air-deck-figma`](https://github.com/andpia/on-air-deck-figma), tracked here as the `vendor/web-ui/` submodule

## 🚀 Building the Project

OnAirDeck uses **CMake** for modern, cross-platform dependency management.
> **JUCE 8 required** — OnAirDeck uses the `WebBrowserComponent::Options::withResourceProvider` API introduced in JUCE 8.  The `vendor/JUCE` submodule is pinned to JUCE 8.0.12 or later.  On Windows, the bundled resource provider is available when the [Microsoft WebView2 SDK](https://developer.microsoft.com/en-us/microsoft-edge/webview2/) is installed; otherwise the app falls back to the legacy IE backend and loads web assets from the file system directly.

1. **Clone the repository**:
    ```bash
    git clone --recursive https://github.com/andpia/on-air-deck.git
    cd on-air-deck
    ```
2. **Generate and Build (Quick Start - Debug)**:
   ```bash
   cmake -S . -B out/build/debug -DCMAKE_BUILD_TYPE=Debug
   cmake --build out/build/debug --config Debug
   ```

3. **Release build**:
   Build `vendor/web-ui/dist` first (or pass `WEBUI_DIST_PATH` to an external dist), then:
   ```bash
   cmake -S . -B out/build/release -DCMAKE_BUILD_TYPE=Release
   cmake --build out/build/release --config Release
   ```

### Linux Dependencies
If compiling on Linux, ensure you have the required development libraries installed:
```bash
sudo apt-get update && sudo apt-get install libasound2-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libgl-dev libwebkit2gtk-4.1-dev libgtk-3-dev libcurl4-openssl-dev
```

#### Ubuntu 24.04 LTS Quick Setup

For a complete automated setup on Ubuntu 24.04 LTS:
```bash
bash scripts/setup-linux.sh
```

This script installs all required JUCE dependencies and system libraries needed for OnAirDeck compilation.

---

## 🌐 Web UI Integration

The application embeds the **On Air Deck Web UI** (from [`andpia/on-air-deck-figma`](https://github.com/andpia/on-air-deck-figma)) inside a native JUCE window using `WebBrowserComponent`.

### Development Workflow (Hot Reload)

In **Debug** builds the app first loads bundled static assets (if available), and otherwise connects to the Vite dev server so UI changes appear instantly without recompiling JUCE.

1. Start the Vite dev server from the `vendor/web-ui/` submodule:
   ```bash
   cd vendor/web-ui
   npm install
   npm run dev          # starts http://localhost:5173 by default
   ```
2. Build OnAirDeck in **Debug** mode and run it:
   ```bash
   cmake -S . -B out/build/debug -DCMAKE_BUILD_TYPE=Debug
   cmake --build out/build/debug --config Debug
   ./out/build/debug/OnAirDeck_artefacts/Debug/OnAirDeck   # path varies by OS
   ```
   The embedded browser will load `http://localhost:5173` and reflect live edits.

> **Configuring the dev server URL**: pass `-DWEBUI_DEV_SERVER_URL=http://localhost:<port>` to CMake if Vite listens on a different port.

### Production Build & Packaging

1. Build the frontend static bundle:
   ```bash
   cd vendor/web-ui
   npm run build        # outputs to dist/
   ```
   > **Vite base path**: the frontend must set `base: './'` in `vite.config.*` so that asset paths are relative and work across local origins (`file://` and `juce://`). Confirm this line exists in `vite.config.ts`:
   > ```ts
   > export default defineConfig({ base: './', ... })
   > ```

2. Build OnAirDeck in **Release**:
   ```bash
   cmake -S . -B out/build/release -DCMAKE_BUILD_TYPE=Release
   cmake --build out/build/release --config Release
   ```

   If your frontend dist is outside `vendor/web-ui/dist`, point CMake at that folder explicitly:
   ```bash
   cmake -S . -B out/build/release -DCMAKE_BUILD_TYPE=Release \
         -DWEBUI_DIST_PATH=/path/to/on-air-deck-figma/dist
   cmake --build out/build/release --config Release
   ```
   CMake will copy the Web UI to the correct location automatically:
   - **macOS**: `OnAirDeck.app/Contents/Resources/WebUI/`
   - **Windows / Linux**: `WebUI/` next to the executable

   The Web UI is therefore bundled with the Release app package, but not embedded directly into the executable binary. At runtime, the app serves these assets via JUCE's internal `juce://` resource origin.

### CMake Variables

| Variable | Default | Description |
|---|---|---|
| `WEBUI_DIST_PATH` | `vendor/web-ui/dist` when present | Path to the Vite `dist/` folder. If not set explicitly, CMake auto-detects the submodule build output when available. |
| `WEBUI_DEV_SERVER_URL` | `http://localhost:5173` | URL loaded in **Debug** builds. |
| `ONAIRDECK_ALLOW_MISSING_WEBUI` | `OFF` | When `ON`, allows Release builds to proceed without a Web UI dist/. **Not recommended for production packages.** |

## 📚 Documentation

📖 **[Read the full documentation on GitHub Pages →](https://andpia.github.io/on-air-deck/)**

Project documentation is now built with **MkDocs** and published through GitHub Pages.

### Local Preview

1. Install the documentation dependencies:
   ```bash
   python3 -m pip install -r requirements-docs.txt
   ```
2. Start the local documentation server:
   ```bash
   mkdocs serve
   ```
3. Open `http://127.0.0.1:8000` in your browser.

### Static Build

To generate the static site locally:

```bash
mkdocs build --strict
```

The generated site will be written to the `out/docs/site/` directory.

## 🧰 One-Command Release Build

### Unix / macOS / Linux

To run frontend + CMake Release build in one command:

```bash
bash scripts/release.sh
```

Optional environment variables:

- `FRONTEND_DIR`: path to frontend repo (default: `vendor/web-ui`, fallback: `../on-air-deck-figma`)
- `BUILD_DIR`: output build directory (default: `out/build/release`)
- `DIST_DIR`: frontend dist path (default: `$FRONTEND_DIR/dist`)

### Windows

Use the PowerShell helper script for a one-command Windows release build:

```powershell
.\scripts\build-windows.ps1
```

This builds the frontend (`vendor/web-ui`), configures CMake with `-DWEBUI_DIST_PATH`, builds the Release target, and packages `OnAirDeck.exe` + `WebUI/` into `out/OnAirDeck-windows-release.zip`.

Optional parameters:

```powershell
# Use a pre-built dist/ from outside the repo:
.\scripts\build-windows.ps1 -WebUiDistPath C:\repos\on-air-deck-figma\dist

# Skip the frontend build (dist/ already present):
.\scripts\build-windows.ps1 -SkipFrontendBuild
```

### Windows CI

The `.github/workflows/windows-release.yml` workflow builds the frontend and native app on Windows, verifies that `WebUI/index.html` is present in the output, and uploads a ZIP artifact (`OnAirDeck-windows-release.zip`) containing `OnAirDeck.exe` and the `WebUI/` folder. It runs automatically on pushes to `main` and pull requests.

## 🧹 Cleaning Generated Files

Use the cleanup script for common reset scenarios:

```bash
bash scripts/clean.sh safe
```

Available modes:

- `safe`: removes `out/`
- `full`: removes `out/`, `vendor/web-ui/dist`, and `vendor/web-ui/node_modules`
- `deep`: removes `full` targets plus legacy `build/` and known JUCE submodule build folders
- `pristine --yes`: runs `git clean -fdx` in the repo and all submodules

Legacy generated output folders `build-debug`, `build-release`, and `site` are deprecated in favor of the `out/` structure.

---

## 🔌 JS ↔ JUCE Bridge

JavaScript can send actions to JUCE by navigating to the custom URL scheme **`myapp://`**:

```javascript
// Trigger an action from the Web UI
window.location = 'myapp://setVolume?value=0.75';
window.location = 'myapp://play';
window.location = 'myapp://stop';
```

JUCE intercepts the URL in `WebUIComponent::pageAboutToLoad()` before any network request is made, parses the action and query parameters, logs them with `DBG`, and **cancels the navigation** so the page stays intact.

### Adding a New Bridge Action

1. **JS side** – navigate to `myapp://<action>[?key=value&...]`
2. **JUCE side** – add a new `else if (action == "<action>")` branch inside `WebUIComponent::handleBridgeAction()` in `src/WebUIComponent.cpp`:

```cpp
else if (action == "myNewAction")
{
    const juce::String param = params["myParam"];
    DBG ("WebUIComponent: myNewAction -> " + param);
    // Forward to the audio engine or other JUCE subsystem
}
```

---

## 🛠️ Hardware & Streaming Setup

* **Audio Interface**: Use an **ASIO** driver on Windows for the best **broadcasting** experience.
* **Streaming Output**: Supports MP3/AAC streams for global **web radio** compatibility.
* **Bitrate**: Optimized for 64–128 kbps **broadcast** standards.

## 📄 License

This project is licensed under the **GNU GPLv3**. See the `LICENSE` file for more details.

---

## 🔧 Troubleshooting

### JUCE 8 Upgrade Notes

| Platform | Web Backend | Resource Provider | Notes |
|---|---|---|---|
| **macOS** | WebKit (WKWebView) | ✅ Available | Assets served via `juce://juce.backend/` |
| **Linux** | WebKit2GTK | ✅ Available | Assets served via `juce://juce.backend/` |
| **Windows + WebView2** | Edge/Chromium | ✅ Available | Assets served via `https://juce.backend/` |
| **Windows (no WebView2)** | Internet Explorer | ❌ Not available | Assets loaded via `file://` URL from disk |

**Enabling WebView2 on Windows** (recommended for full JUCE 8 feature set):
1. Install the [Microsoft WebView2 SDK NuGet package](https://www.nuget.org/packages/Microsoft.Web.WebView2/) into your local NuGet package directory.
2. Run CMake – `FindWebView2.cmake` (from JUCE) will detect the package automatically and enable `JUCE_USE_WIN_WEBVIEW2=1`.

If the SDK is absent CMake prints `WebView2 SDK: not found -> using legacy IE browser backend` and the build continues without WebView2.

### `withResourceProvider` compile error on Windows

If you see `error C2039: 'withResourceProvider': is not a member of 'juce::WebBrowserComponent::Options'`, it means your JUCE copy is older than JUCE 8 or WebView2 is not enabled.  Make sure:
- `vendor/JUCE` is at JUCE 8.0.12 or newer (`git submodule update --init --recursive`)
- On Windows, install the WebView2 SDK as described above (or accept the IE fallback)

---

**Made with ❤️, JUCE 8 and a passion for Radio Broadcasting**

