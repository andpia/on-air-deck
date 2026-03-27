# Dependencies

This page lists external repositories required to build and run OnAirDeck workflows.

## Repository Dependencies

### JUCE

- Repository: [juce-framework/JUCE](https://github.com/juce-framework/JUCE)
- **Required version: JUCE 8.0.12 or later** (JUCE 8 introduced `WebBrowserComponent::Options::withResourceProvider`, which is used to serve the bundled Web UI without a separate server)
- Usage: native application framework and platform abstraction layer.
- Integration: included as a git submodule inside `vendor/JUCE/`.
- Impact:
  - Required for all Debug and Release native builds.
  - On Windows, the full resource-provider feature requires the [Microsoft WebView2 SDK](https://developer.microsoft.com/en-us/microsoft-edge/webview2/) at **build time** and the **Microsoft Edge WebView2 Runtime** at **run time**. Without the Runtime the app shows a dialog asking the user to install it (see [Build and Run](build-and-run.md#windows-prerequisites)).

### OnAirDeck Frontend UI

- Repository: [andpia/on-air-deck-figma](https://github.com/andpia/on-air-deck-figma)
- Usage: Vite/React frontend loaded in the embedded `WebBrowserComponent`.
- Integration:
  - Tracked as git submodule at `vendor/web-ui/`.
  - Debug: run the frontend dev server and configure `WEBUI_DEV_SERVER_URL` if needed.
  - Release: CMake auto-detects `vendor/web-ui/dist` when present, or use `-DWEBUI_DIST_PATH=/path/to/on-air-deck-figma/dist`.
- Impact:
  - Required for live UI development in Debug.
  - Required to package static Web UI assets in Release.

## Related Documentation

- [Build and Run](build-and-run.md)
- [Web UI](web-ui.md)
- [Bridge API](bridge-api.md)
