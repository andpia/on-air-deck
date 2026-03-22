# Dependencies

This page lists external repositories required to build and run OnAirDeck workflows.

## Repository Dependencies

### JUCE

- Repository: [juce-framework/JUCE](https://github.com/juce-framework/JUCE)
- Usage: native application framework and platform abstraction layer.
- Integration: included as a git submodule inside `vendor/JUCE/`.
- Impact:
  - Required for all Debug and Release native builds.

### OnAirDeck Frontend UI

- Repository: [andpia/on-air-deck-figma](https://github.com/andpia/on-air-deck-figma)
- Usage: Vite/React frontend loaded in the embedded `WebBrowserComponent`.
- Integration:
  - Tracked as git submodule at `web-ui/`.
  - Debug: run the frontend dev server and configure `WEBUI_DEV_SERVER_URL` if needed.
  - Release: CMake auto-detects `web-ui/dist` when present, or use `-DWEBUI_DIST_PATH=/path/to/on-air-deck-figma/dist`.
- Impact:
  - Required for live UI development in Debug.
  - Required to package static Web UI assets in Release.

## Related Documentation

- [Build and Run](build-and-run.md)
- [Web UI](web-ui.md)
- [Bridge API](bridge-api.md)
