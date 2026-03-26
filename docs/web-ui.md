# Web UI

OnAirDeck embeds the frontend inside the native JUCE application using `juce::WebBrowserComponent`.

## Debug Mode

Debug builds load the Vite development server for live reload.

Default server URL:

```text
http://localhost:5173
```

Start the frontend from the companion repository:

```bash
cd vendor/web-ui
npm install
npm run dev
```

If you keep the frontend repository outside this project, use that external path instead.

If Vite runs on a different port, reconfigure CMake:

```bash
cmake -S . -B out/build/debug -DCMAKE_BUILD_TYPE=Debug \
  -DWEBUI_DEV_SERVER_URL=http://localhost:3000
```

## Release Mode

Release builds do not depend on a running development server. The frontend is not embedded into the executable itself; instead, the built static files are copied into the application package and served by JUCE's `WebBrowserComponent::ResourceProvider` from the internal `juce://` origin.

- macOS: `OnAirDeck.app/Contents/Resources/WebUI/`
- Windows and Linux: `WebUI/` next to the executable

The frontend project should keep a relative Vite base path (`base: './'`) so generated links remain portable across local origins (`file://` and `juce://`).

## Release Build Requirement

**Release builds require a pre-built Web UI dist/ folder.**  
CMake will fail at configure time with a clear error message if no dist is found and
`ONAIRDECK_ALLOW_MISSING_WEBUI` is not set:

```
Release build requires a pre-built Web UI dist/ folder, but none was found.
```

**Windows Release builds also require the Microsoft WebView2 SDK.**  
Without it, JUCE compiles with the legacy Internet Explorer backend which **cannot render
modern React/Vite applications** and will show a blank white page at runtime.  
CMake will fail with a clear error message if WebView2 SDK is missing on Windows Release:

```
Windows Release build requires the Microsoft WebView2 SDK.
```

To allow a Release build without the Web UI or WebView2 (for CI smoke-tests or unusual setups):

```bash
cmake -S . -B out/build/release -DCMAKE_BUILD_TYPE=Release \
  -DONAIRDECK_ALLOW_MISSING_WEBUI=ON
```

> This option is **off by default** and should not be used for production packages.

## Windows CI

The repository includes a dedicated GitHub Actions workflow (`.github/workflows/windows-release.yml`)
that:
1. Builds the frontend (`npm ci && npm run build`)
2. Installs the Microsoft.Web.WebView2 NuGet SDK automatically
3. Configures CMake with `WEBUI_DIST_PATH` and `WEBVIEW2_PATH`
4. Builds the native app with `JUCE_USE_WIN_WEBVIEW2=1` (Edge/Chromium backend)
5. Verifies that `WebUI/index.html` is present next to the executable
6. Uploads a ZIP artifact containing `OnAirDeck.exe` and the `WebUI/` folder

For local Windows builds, use the PowerShell helper (also installs WebView2 SDK automatically):

```powershell
.\scripts\build-windows.ps1
```

See [Build and Run](build-and-run.md) for full details.

## Failure Modes

If the app reports that it cannot connect to the server during Debug, the development server is not running or `WEBUI_DEV_SERVER_URL` points to the wrong address.

If a Release build shows a missing Web UI page, verify that `WEBUI_DIST_PATH` points at a valid frontend `dist/` directory during configuration.

If a Windows Release build opens to a **blank white page**, the most likely cause is:

1. **WebView2 SDK was not installed at compile time** — the app was compiled with the legacy IE backend (`JUCE_USE_WIN_WEBVIEW2=0`).  Modern React/Vite bundles cannot run in IE11.  Recompile using `scripts\build-windows.ps1` or the `windows-release.yml` CI workflow, both of which install the WebView2 SDK automatically.
2. **WebView2 Runtime is not installed at runtime** — on modern Windows 10/11 with Edge installed, WebView2 Runtime comes pre-installed.  On older systems without Edge, download and install it from [Microsoft Edge WebView2](https://developer.microsoft.com/en-us/microsoft-edge/webview2/).

If a Release build shows a missing Web UI page, verify that:

- `vendor/web-ui/dist` was rebuilt (`npm run build`)
- CMake copied the latest `dist/` into the app package
- the generated `dist/index.html` references `./assets/...` rather than `/assets/...`