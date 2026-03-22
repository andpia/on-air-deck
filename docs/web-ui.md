# Web UI

OnAirDeck embeds the frontend inside the native JUCE application using `juce::WebBrowserComponent`.

## Debug Mode

Debug builds connect to a Vite development server for live reload.

Default server URL:

```text
http://localhost:5173
```

Start the frontend from the companion repository:

```bash
cd ../on-air-deck-figma
npm install
npm run dev
```

If Vite runs on a different port, reconfigure CMake:

```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug \
  -DWEBUI_DEV_SERVER_URL=http://localhost:3000
```

## Release Mode

Release builds do not depend on a running development server. Instead, they load `index.html` from bundled static assets copied into the application package.

- macOS: `OnAirDeck.app/Contents/Resources/WebUI/`
- Windows and Linux: `WebUI/` next to the executable

The frontend project should use a relative Vite base path so assets load correctly from `file://` URLs.

## Failure Modes

If the app reports that it cannot connect to the server during Debug, the development server is not running or `WEBUI_DEV_SERVER_URL` points to the wrong address.

If a Release build shows a missing Web UI page, verify that `WEBUI_DIST_PATH` points at a valid frontend `dist/` directory during configuration.