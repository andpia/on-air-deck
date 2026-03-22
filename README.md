# 🎙️ OnAirDeck

### Professional Web Radio Engine: Live Streaming, Playlist Management & Broadcast Tools

**OnAirDeck** is a lightweight, high-performance **radio automation** console and **web streaming** suite built with the **JUCE** framework. It bridges the gap between digital music playlists and live vocal performance, providing a professional "deck" experience for modern **broadcasters**.

---

## ✨ Key Features

* **Advanced Broadcast Engine**: Native support for **ASIO** (Windows) and **CoreAudio** (macOS) ensuring ultra-low latency for professional **web radio** standards.
* **Smart Streaming**: Integrated encoder for direct-to-server **streaming** via **Icecast** and **Shoutcast** protocols.
* **Pro Voice-Over (Ducking)**: Intelligent automatic volume management to balance music and speech during **live broadcast** sessions.
* **Dynamic Playlist Deck**: Seamless track transitions and high-fidelity playback for continuous **radio** rotation.
* **Embedded Web UI**: The full On Air Deck interface (built with Vite + React from [`andpia/on-air-deck-figma`](https://github.com/andpia/on-air-deck-figma)) runs inside a JUCE `WebBrowserComponent`, with a bidirectional JS ↔ JUCE bridge.

## 🚀 Building the Project

OnAirDeck uses **CMake** for modern, cross-platform dependency management.

1.  **Clone the repository**:
    ```bash
    git clone --recursive https://github.com/andpia/on-air-deck.git
    cd on-air-deck
    ```
2.  **Generate and Build**:
    ```bash
   cmake -B build-release -DCMAKE_BUILD_TYPE=Release
   cmake --build build-release --config Release
    ```

### Linux Dependencies
If compiling on Linux, ensure you have the required development libraries installed:
```bash
sudo apt-get update && sudo apt-get install libasound2-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libgl1-mesa-dev libwebkitgtk-6.0-dev libgtk-3-dev libcurl4-openssl-dev
```

---

## 🌐 Web UI Integration

The application embeds the **On Air Deck Web UI** (from [`andpia/on-air-deck-figma`](https://github.com/andpia/on-air-deck-figma)) inside a native JUCE window using `WebBrowserComponent`.

### Development Workflow (Hot Reload)

In **Debug** builds the app automatically connects to the Vite dev server so UI changes appear instantly without recompiling JUCE.

1. Clone and start the Vite dev server in the frontend repo:
   ```bash
   git clone https://github.com/andpia/on-air-deck-figma.git
   cd on-air-deck-figma
   npm install
   npm run dev          # starts http://localhost:5173 by default
   ```
2. Build OnAirDeck in **Debug** mode and run it:
   ```bash
   cd on-air-deck
   cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug
   cmake --build build-debug --config Debug
   ./build-debug/OnAirDeck_artefacts/Debug/OnAirDeck   # path varies by OS
   ```
   The embedded browser will load `http://localhost:5173` and reflect live edits.

> **Configuring the dev server URL**: pass `-DWEBUI_DEV_SERVER_URL=http://localhost:<port>` to CMake if Vite listens on a different port.

### Production Build & Packaging

1. Build the frontend static bundle:
   ```bash
   cd on-air-deck-figma
   npm run build        # outputs to dist/
   ```
   > **Vite base path**: the frontend must set `base: './'` in `vite.config.*` so that asset paths are relative and work when loaded via `file://`. Add or confirm this line in `vite.config.ts`:
   > ```ts
   > export default defineConfig({ base: './', ... })
   > ```

2. Build OnAirDeck in **Release**, pointing CMake at the dist folder:
   ```bash
   cmake -B build-release -DCMAKE_BUILD_TYPE=Release \
         -DWEBUI_DIST_PATH=/path/to/on-air-deck-figma/dist
   cmake --build build-release --config Release
   ```
   CMake will copy the Web UI to the correct location automatically:
   - **macOS**: `OnAirDeck.app/Contents/Resources/WebUI/`
   - **Windows / Linux**: `WebUI/` next to the executable

### CMake Variables

| Variable | Default | Description |
|---|---|---|
| `WEBUI_DIST_PATH` | *(empty)* | Path to the Vite `dist/` folder. When set, files are copied post-build. |
| `WEBUI_DEV_SERVER_URL` | `http://localhost:5173` | URL loaded in **Debug** builds. |

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

The generated site will be written to the `site/` directory.

## 🧰 One-Command Release Build

To run frontend + CMake Release build in one command:

```bash
bash scripts/release.sh
```

Optional environment variables:

- `FRONTEND_DIR`: path to `on-air-deck-figma` (default: `../on-air-deck-figma`)
- `BUILD_DIR`: output build directory (default: `build-release`)
- `DIST_DIR`: frontend dist path (default: `$FRONTEND_DIR/dist`)

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

**Made with ❤️, JUCE and a passion for Radio Broadcasting**

