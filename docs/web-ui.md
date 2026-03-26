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

To allow a Release build without the Web UI (for CI smoke-tests or unusual setups):

```bash
cmake -S . -B out/build/release -DCMAKE_BUILD_TYPE=Release \
  -DONAIRDECK_ALLOW_MISSING_WEBUI=ON
```

> This option is **off by default** and should not be used for production packages.

## Windows CI

The repository includes a dedicated GitHub Actions workflow (`.github/workflows/windows-release.yml`)
that builds the frontend, configures CMake with `WEBUI_DIST_PATH`, builds the native app, verifies
that `WebUI/index.html` is present next to the executable, and uploads a ZIP artifact containing
both `OnAirDeck.exe` and the `WebUI/` folder.

For local Windows builds, use the PowerShell helper:

```powershell
.\scripts\build-windows.ps1
```

See [Build and Run](build-and-run.md) for full details.

## Failure Modes

If the app reports that it cannot connect to the server during Debug, the development server is not running or `WEBUI_DEV_SERVER_URL` points to the wrong address.

If a Release build shows a missing Web UI page, verify that `WEBUI_DIST_PATH` points at a valid frontend `dist/` directory during configuration.

If a Release build opens to a blank page, verify that:

- `vendor/web-ui/dist` was rebuilt (`npm run build`)
- CMake copied the latest `dist/` into the app package
- the generated `dist/index.html` references `./assets/...` rather than `/assets/...`