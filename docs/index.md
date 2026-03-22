# OnAirDeck

OnAirDeck is a JUCE-based radio automation console and web streaming application built for live broadcast workflows.

## What It Covers

- Native desktop application built with JUCE and CMake.
- Embedded Web UI loaded inside a JUCE `WebBrowserComponent`.
- Debug workflow with Vite hot reload.
- Release packaging with bundled static frontend assets.

## Documentation Map

- [Architecture](architecture.md): runtime structure and native/frontend boundaries.
- [Build and Run](build-and-run.md): local build steps for Debug and Release.
- [Dependencies](dependencies.md): external repositories required by build and packaging workflows.
- [Web UI](web-ui.md): how the embedded frontend is loaded in development and production.
- [Bridge API](bridge-api.md): the `myapp://` communication bridge between JavaScript and JUCE.
- [Docs Workflow](docs-workflow.md): local preview and GitHub Pages publishing.

## Repositories

- Core application: [andpia/on-air-deck](https://github.com/andpia/on-air-deck)
- Frontend UI: [andpia/on-air-deck-figma](https://github.com/andpia/on-air-deck-figma)

## Quick Start

```bash
git clone --recursive https://github.com/andpia/on-air-deck.git
cd on-air-deck
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug --config Debug
```

If you want live UI reload in Debug, start the frontend dev server from the companion repository before launching the app.