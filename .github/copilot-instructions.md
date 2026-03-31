# OnAirDeck — Copilot Instructions

Professional web radio engine: JUCE 8 C++ shell + embedded React/Vite web UI communicating via a custom URL bridge.

## Architecture

```
JUCEApplication
  └── MainWindow (DocumentWindow)
        └── MainComponent (~20 lines, layout only)
              └── WebUIComponent (WebBrowserComponent) — all real UI here
                    └── React/Vite SPA (vendor/web-ui/)
```

- **Native shell** (`src/`) is intentionally thin — layout and bridge only. Audio engine wiring is still in progress (current bridge actions are `DBG` stubs).
- **Web UI** (`vendor/web-ui/`) is a Git submodule tracking [`andpia/on-air-deck-figma`](https://github.com/andpia/on-air-deck-figma).
- **JS↔JUCE bridge**: JS navigates `window.location = 'myapp://<action>?key=value'`; `WebUIComponent::pageAboutToLoad` intercepts it, parses params into `juce::StringPairArray`, calls `handleBridgeAction()`, and returns `false` to cancel navigation. See [docs/bridge-api.md](../docs/bridge-api.md).

## Build and Test

See [docs/build-and-run.md](../docs/build-and-run.md) for full instructions.

**C++ (requires submodules initialized)**
```bash
cmake --preset debug && cmake --build --preset debug    # Debug
cmake --preset release && cmake --build --preset release  # Release (needs dist/ first)
```

**Web UI** (in `vendor/web-ui/`)
```bash
npm install && npm run dev    # Vite dev server at http://localhost:5173
npm run build                 # produces dist/ for Release
```

**One-command release:**
```bash
bash scripts/release.sh
```

**Cleanup:**
```bash
bash scripts/clean.sh safe    # removes out/
bash scripts/clean.sh full    # + dist/ and node_modules/
```

No automated tests exist yet (no CTest, no Jest/Vitest).

## C++ Conventions

- **Standard:** C++17. Use `#if JUCE_WINDOWS` / `#if JUCE_MAC` / `#if JUCE_DEBUG` for platform/config guards — never runtime string checks.
- **Naming:** JUCE-style — `PascalCase` for classes, `camelCase` for methods and variables. No `m_` prefix on members.
- **Every class** must have `JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR`.
- New `.cpp`/`.h` files in `src/` are picked up automatically via `GLOB_RECURSE CONFIGURE_DEPENDS` — no CMake edit needed.
- **Security:** `provideWebUIResource` rejects `..` in paths; HTML error strings are escaped before embedding.

## Web UI Conventions

- Path alias `@` → `src/`. App entry: `src/main.tsx` → `src/app/App.tsx`.
- Stack: React 18, Vite 6, TypeScript, Tailwind CSS 4 (via `@tailwindcss/vite`), MUI v7, Radix UI, react-router v7, react-hook-form.
- **Vite `base` must stay `'./'`** — relative paths required for both `file://` and `juce://` origins.
- Do not remove the `tailwindcss()` Vite plugin even if Tailwind appears unused.
- See [vendor/web-ui/guidelines/Guidelines.md](../vendor/web-ui/guidelines/Guidelines.md) for UI patterns.

## Critical Pitfalls

| Pitfall | Fix |
|---------|-----|
| Blank white screen on Windows | WebView2 SDK NuGet package is missing — install before CMake configure. See [docs/dependencies.md](../docs/dependencies.md). |
| Vite assets 404 in Release (`/assets/…`) | `vite.config.ts` `base` must be `'./'`, not `'/'`. |
| Submodule dirs empty (`vendor/JUCE` or `vendor/web-ui`) | Run `git submodule update --init --recursive`. CMake fails immediately if missing. |
| Release configure fails | `dist/` must be built (or `WEBUI_DIST_PATH` must point to a valid dir) before `cmake --preset release`. |
| Linux compile errors | Install system libs first: see [docs/build-and-run.md](../docs/build-and-run.md) or run `bash scripts/setup-linux.sh`. |
| Adding a new bridge action | Add a branch in `WebUIComponent::handleBridgeAction()`. See [docs/bridge-api.md](../docs/bridge-api.md). |

## Key Files

| Path | Purpose |
|------|---------|
| `src/WebUIComponent.cpp` | Bridge implementation — `pageAboutToLoad`, `handleBridgeAction`, resource provider |
| `src/MainComponent.cpp` | Window layout wiring |
| `CMakeLists.txt` | Build config, platform detection, WebUI copy step |
| `CMakePresets.json` | Preset definitions (`debug`, `release`) |
| `vendor/web-ui/src/app/` | React app — pages, components, routes, types |
| `vendor/web-ui/vite.config.ts` | Vite config — `base`, alias, plugins |
| `docs/architecture.md` | Component diagram and design decisions |
| `docs/bridge-api.md` | Full bridge protocol reference |
